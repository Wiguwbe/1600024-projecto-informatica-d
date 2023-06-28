#include "astar_parallel.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que contem a mensagem a ser passada nas queues
typedef struct
{
  a_star_node_t* parent;
  state_t* state;
} a_star_message_t;

// Função para encontrar o next worker baseada na posição de memória do estado
// Isto garante uma distribuição balanceada entre os trabalhadores e ao mesmo
// tempo garante que os nós processam sempre os mesmos estados
static size_t assign_to_worker(a_star_parallel_t* a_star, const state_t* state)
{
  size_t hash = hash_function(state->data, state->struct_size, HASH_CAPACITY);
  return hash % a_star->scheduler.num_workers;
}

// Função que implementa a lógica de um trabalhador, aqui se processa o algoritmo A*
void* a_star_worker_function(void* arg)
{
  a_star_worker_t* worker = (a_star_worker_t*)arg;
  a_star_parallel_t* a_star = worker->a_star;

  // Reinicia as estatísticas para este trabalhador
  worker->generated = 0;
  worker->expanded = 0;
  worker->max_min_heap_size = 0;
  worker->nodes_new = 0;
  worker->nodes_reinserted = 0;
  worker->paths_better = 0;
  worker->paths_worst_or_equals = 0;

  worker->idle = false;

  while(a_star->running)
  {
    // Este trabalhador considera-se ocioso caso não existam nós a explorar ou mensagens destinadas a si
    worker->idle = worker->open_set->size == 0 && !channel_has_messages(a_star->channel, worker->thread_id);

    // Processamos todos os estados que estão no canal para esta tarefa
    // Aqui que ocorre a atualização do custo do estado
    while(channel_has_messages(a_star->channel, worker->thread_id))
    {
      worker->idle = false;
      a_star_message_t* message = channel_receive(a_star->channel, worker->thread_id);

      // Retiramos os dados da mensagem e libertamos a memória
      a_star_node_t* parent_node = message->parent;
      state_t* state = message->state;
      free(message);

      // Se o nó pai não foi enviado é porque estamos a lidar com o estado inicial
      if(parent_node == NULL)
      {
        a_star_node_t* initial_node = node_allocator_new(a_star->common->node_allocator, state);
        // Atribui ao nó inicial um custo total de 0
        initial_node->g = 0;
        initial_node->h = a_star->common->h_func(initial_node->state, a_star->common->goal_state);

        // Inserimos o nó na nossa fila e saímos já que não existem mais mensagens
        initial_node->index_in_open_set = min_heap_insert(worker->open_set, initial_node->h, initial_node);
        break;
      }

      // Recebemos um estado para ser processado, verificamos se já existe um nó para este estado
      a_star_node_t* child_node = node_allocator_get(a_star->common->node_allocator, state);

      // Este é um novo no
      if(!child_node)
      {
        // Este nó ainda não existe, criamos um novo nó para este estado
        child_node = node_allocator_new(a_star->common->node_allocator, state);
        child_node->parent = parent_node;
        worker->generated++;

        // Encontra o custo de chegar do estado pai a este estado e calculamos a heurística (distância para chegar ao objetivo)
        child_node->g = parent_node->g + a_star->common->d_func(parent_node->state, child_node->state);
        child_node->h = a_star->common->h_func(child_node->state, a_star->common->goal_state);

        // Calculamos o custo
        int cost = child_node->g + child_node->h;

        // Inserimos o nó na nossa fila
        child_node->index_in_open_set = min_heap_insert(worker->open_set, cost, child_node);
        worker->nodes_new++;
      }
      else
      {
        // Encontra o custo de chegar do estado pai para este estado
        int g_attempt = parent_node->g + a_star->common->d_func(parent_node->state, child_node->state);

        // Se o custo for maior do que o nó já tem, não faz sentido atualizar
        // existe outro caminho mais curto para este estado
        if(g_attempt >= child_node->g)
        {
          worker->paths_worst_or_equals++;
          continue;
        }

        // O estado pai é o caminho mais curto para este estado, atualizamos o pai deste estado
        child_node->parent = parent_node;

        // Atualizamos os parâmetros do nó
        child_node->g = g_attempt;
        child_node->h = a_star->common->h_func(child_node->state, a_star->common->goal_state);

        // Calculamos o novo custo
        int cost = child_node->g + child_node->h;

        worker->paths_better++;
        if(child_node->index_in_open_set == SIZE_MAX)
        {
          // Inserimos o nó na nossa fila novamente
          child_node->index_in_open_set = min_heap_insert(worker->open_set, cost, child_node);
          worker->nodes_reinserted++;
        }
        else
        {
          // Atualizamos a nossa fila prioritária
          min_heap_update_cost(worker->open_set, child_node->index_in_open_set, cost);
        }
      }
    }

    if(worker->max_min_heap_size < worker->open_set->size)
      worker->max_min_heap_size = worker->open_set->size;

    // Temos pelo menos um nó na nossa lista aberta que podemos processar
    if(worker->open_set->size)
    {
      worker->idle = false;
      // A seguinte operação pode ocorrer em O(log(N))
      // se nosAbertos é um min-heap ou uma queue prioritária
      heap_node_t top_element = min_heap_pop(worker->open_set);

      // Nó atual na nossa árvore
      a_star_node_t* current_node = (a_star_node_t*)top_element.data;
      current_node->index_in_open_set = SIZE_MAX;
      worker->expanded++;

      // Verificamos se já existe uma solução, caso já exista temos de verificar se
      // este trabalhador está a procurar por soluções que se encontram a uma distância maior
      // do que a solução já encontrada, será que vale a pena continuar? Consideramos que não e saímos.
      if(a_star->common->solution != NULL)
      {
        int f_solution = a_star->common->solution->g;
        int f_current = current_node->g + current_node->h;

        if(f_current > f_solution || current_node->g > a_star->common->solution->g)
        {
          min_heap_clean(worker->open_set);
          continue;
        }
      }

      // Se encontramos o objetivo saímos e retornamos o nó
      if(a_star->common->goal_func(current_node->state, a_star->common->goal_state))
      {
        // Temos de informar que encontramos o nosso objetivo
        pthread_mutex_lock(&(a_star->lock));
        if(a_star->common->solution == NULL)
        {
          // Esta é a primeira solução encontrada nada de especial
          // a fazer
          a_star->common->solution = current_node;
        }
        else
        {
          // Já existe uma solução, temos de verificar se esta nova
          // solução tem um custo menor
          int existing_cost = a_star->common->solution->g + a_star->common->solution->h;
          int attempt_cost = current_node->g + current_node->h;
          if(existing_cost > attempt_cost)
          {
            a_star->common->solution = current_node;
          }
        }
        pthread_mutex_unlock(&(a_star->lock));
      }
      else
      {
        // Esta lista irá receber os vizinhos deste nó
        linked_list_t* neighbors = linked_list_create();

        // Executa a função que visita os vizinhos deste nó
        a_star->common->visit_func(current_node->state, a_star->common->state_allocator, neighbors);

        // Itera por todos os vizinhos gerados e envia para a devida tarefa
        for(size_t i = 0; i < linked_list_size(neighbors); i++)
        {
          // Preparamos a mensagem a ser enviada
          a_star_message_t* message = (a_star_message_t*)malloc(sizeof(a_star_message_t));
          if(message == NULL)
          {
            // Falha na alocação, continuamos e assim eventualmente a thread irá parar
            continue;
          }

          // Compomos a mensagem com os dados necessários e identificamos qual
          // o trabalhador que vai tratar deste estado
          message->parent = current_node;
          message->state = (state_t*)linked_list_get(neighbors, i);
          size_t worker_id = assign_to_worker(a_star, message->state);

          // Enviamos a mensagem para o respetivo trabalhador
          channel_send(a_star->channel, worker_id, message);
        }

        // Liberta a lista de vizinhos
        linked_list_destroy(neighbors);
      }
    }
  }

  pthread_exit(NULL);
}

// Cria uma nova instância para resolver um problema
a_star_parallel_t* a_star_parallel_create(size_t struct_size,
                                          goal_function goal_func,
                                          visit_function visit_func,
                                          heuristic_function h_func,
                                          distance_function d_func,
                                          print_function print_func,
                                          int num_workers,
                                          bool stop_on_first_solution)
{
  a_star_parallel_t* a_star = (a_star_parallel_t*)malloc(sizeof(a_star_parallel_t));
  if(a_star == NULL)
  {
    return NULL; // Erro de alocação
  }

  // Garante que a memória esteja limpa
  a_star->scheduler.workers = NULL;
  a_star->channel = NULL;
  a_star->common = NULL;

  pthread_mutex_init(&a_star->lock, NULL);

  // Inicializamos a parte comum do nosso algoritmo
  a_star->common = a_star_create(struct_size, goal_func, visit_func, h_func, d_func, print_func);

  if(a_star->common == NULL)
  {
    a_star_parallel_destroy(a_star);
    return NULL;
  }

  // Inicializamos a parte especifica para o algoritmo paralelo

  // Inicializamos o nosso scheduler
  a_star->scheduler.workers = (a_star_worker_t*)malloc(num_workers * sizeof(a_star_worker_t));
  if(a_star->scheduler.workers == NULL)
  {
    a_star_parallel_destroy(a_star);
    return NULL;
  }

  // Criamos um canal para que os trabalhadores possam comunicar
  a_star->channel = channel_create(num_workers);
  if(a_star->channel == NULL)
  {
    a_star_parallel_destroy(a_star);
    return NULL;
  }

  // Inicializamos as estruturas que vão conter o estado de cada
  // um dos trabalhadores
  a_star->scheduler.num_workers = num_workers;
  for(size_t i = 0; i < a_star->scheduler.num_workers; i++)
  {
    a_star->scheduler.workers[i].a_star = a_star;
    a_star->scheduler.workers[i].thread_id = i;
    a_star->scheduler.workers[i].open_set = min_heap_create();
    a_star->scheduler.workers[i].idle = true;

    // Reiniciamos as estatísticas internas do trabalhador
    a_star->scheduler.workers[i].expanded = 0;
    a_star->scheduler.workers[i].generated = 0;
  }

  // Reiniciamos a variável utilizada para round-robin
  a_star->scheduler.next_worker = 0;
  pthread_mutex_init(&a_star->scheduler.lock, NULL);

  // Inicializa as funções necessárias para o algoritmo funcionar
  a_star->stop_on_first_solution = stop_on_first_solution;
  a_star->running = false;

  return a_star;
}

// Liberta uma instância do algoritmo A*
void a_star_parallel_destroy(a_star_parallel_t* a_star)
{
  if(a_star == NULL)
  {
    return;
  }

  // Limpamos os nosso trabalhadores e canal de comunicação
  if(a_star->scheduler.workers != NULL)
  {
    for(size_t i = 0; i < a_star->scheduler.num_workers; i++)
    {
      min_heap_destroy(a_star->scheduler.workers[i].open_set);
    }
  }
  if(a_star->channel != NULL)
  {
    channel_destroy(a_star->channel, true);
  }

  // Invocamos o destroy da parte comum
  if(a_star->common != NULL)
  {
    a_star_destroy(a_star->common);
  }

  // Destruímos o nosso algoritmo
  free(a_star);
}

// Resolve o problema através do uso do algoritmo A*;
void a_star_parallel_solve(a_star_parallel_t* a_star, void* initial, void* goal)
{
  if(a_star == NULL)
  {
    return;
  }

  // Guarda os nossos estados inicial e objetivo
  state_t* initial_state = state_allocator_new(a_star->common->state_allocator, initial);

  if(initial_state == NULL)
  {
    return;
  }

  // Preparamos o nosso objetivo caso tenha sido passado (existem problemas em que não se passam soluções)
  if(goal)
  {
    // Temos de "containerizar" o objetivo num estado
    a_star->common->goal_state = state_allocator_new(a_star->common->state_allocator, goal);

    if(a_star->common->goal_state == NULL)
    {
      return;
    }
  }

  // Preparamos a mensagem a ser enviada para o estado inicial
  a_star_message_t* message = (a_star_message_t*)malloc(sizeof(a_star_message_t));
  if(message == NULL)
  {
    return;
  }

  message->parent = NULL;
  message->state = initial_state;

  int worker_id = assign_to_worker(a_star, message->state);

  // Enviamos o estado inicial para o respetivo trabalhador
  channel_send(a_star->channel, worker_id, message);

  // Com recurso a esta variável podemos enviar uma mensagem para os nossos trabalhadores
  // pararem
  a_star->running = true;

  // Iniciamos cada trabalhador
  for(size_t i = 0; i < a_star->scheduler.num_workers; i++)
  {
    a_star->scheduler.workers[i].idle = false;
    int result =
        pthread_create(&(a_star->scheduler.workers[i].thread), NULL, a_star_worker_function, &(a_star->scheduler.workers[i]));
    if(result != 0)
    {
      return;
    }
  }

  // Ciclo de execução que espera pela solução ou que todos os trabalhadores fiquem
  // sem nós para processar
  clock_gettime(CLOCK_MONOTONIC, &(a_star->common->start_time));
  while(a_star->running)
  {
    // Solução já foi encontrada e queremos sair à primeira solução
    if(a_star->common->solution != NULL && a_star->stop_on_first_solution)
    {
      a_star->running = false;
      break;
    }

    // Verificamos quantos workers estão ociosos, caso todos estejam ociosos assumimos que não existem mais nós a explorar
    // ou a solução já foi encontrada
    size_t idle_workers = 0;
    for(size_t i = 0; i < a_star->scheduler.num_workers; i++)
    {

      a_star_worker_t* worker = &(a_star->scheduler.workers[i]);

      if(!worker->idle)
      {
        continue;
      }

      idle_workers++;

      // O algoritmo deve continuar a correr enquanto houver trabalhadores que não estejam ociosos
      a_star->running = idle_workers < a_star->scheduler.num_workers;
    }
  }

  // Esperamos que todas os trabalhadores terminem
  for(size_t i = 0; i < a_star->scheduler.num_workers; i++)
  {
    pthread_join(a_star->scheduler.workers[i].thread, NULL);
  }
  clock_gettime(CLOCK_MONOTONIC, &(a_star->common->end_time));

  // Calculamos o tempo de execução e outras estatísticas
  for(size_t i = 0; i < a_star->scheduler.num_workers; i++)
  {
    a_star->common->expanded += a_star->scheduler.workers[i].expanded;
    a_star->common->generated += a_star->scheduler.workers[i].generated;
    a_star->common->max_min_heap_size += a_star->scheduler.workers[i].max_min_heap_size;
    a_star->common->nodes_new += a_star->scheduler.workers[i].nodes_new;
    a_star->common->nodes_reinserted += a_star->scheduler.workers[i].nodes_reinserted;
    a_star->common->paths_better += a_star->scheduler.workers[i].paths_better;
    a_star->common->paths_worst_or_equals += a_star->scheduler.workers[i].paths_worst_or_equals;
  }
  a_star->common->execution_time = (a_star->common->end_time.tv_sec - a_star->common->start_time.tv_sec);
  a_star->common->execution_time += (a_star->common->end_time.tv_nsec - a_star->common->start_time.tv_nsec) / 1000000000.0;
}

// Imprime estatísticas do algoritmo sequencial no formato desejado
void a_star_parallel_print_statistics(a_star_parallel_t* a_star, bool csv)
{
  if(a_star == NULL)
  {
    return;
  }

  if(!csv)
  {
    if(a_star->stop_on_first_solution)
    {
      printf("Método: Primeira solução\n");
    }
    else
    {
      printf("Método: Melhor solução\n");
    }
  }

  a_star_print_statistics(a_star->common, csv);

  if(!csv)
  {
    printf("Estatísticas Trabalhadores:\n");
    for(size_t i = 0; i < a_star->scheduler.num_workers; i++)
    {
      printf("- Trabalhador #%ld\n", i + 1);
      printf("  * Estados gerados: %d, Estados expandidos: %d\n",
             a_star->scheduler.workers[i].generated,
             a_star->scheduler.workers[i].expanded);
      printf("  * Max nós min_heap: %ld, Novos nós: %d, Nós reinseridos: %d, Caminhos piores (ignorados): %d, Caminhos "
             "melhores (atualizados): %d\n",
             a_star->scheduler.workers[i].max_min_heap_size,
             a_star->scheduler.workers[i].nodes_new,
             a_star->scheduler.workers[i].nodes_reinserted,
             a_star->scheduler.workers[i].paths_worst_or_equals,
             a_star->scheduler.workers[i].paths_better);
    }
  }
}