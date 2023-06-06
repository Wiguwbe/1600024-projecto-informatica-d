#include "astar_parallel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estrutura que contem a mensagem a ser passada nas queues
typedef struct
{
  a_star_node_t* parent;
  state_t* state;
} a_star_message_t;

// Função utilizada na hashtable nodes para comparar se 2 nós são iguais
// è necessário comparar apenas os state_t
bool compare_a_star_nodes(const void* node_a, const void* node_b)
{
  size_t struct_size_a = ((a_star_node_t*)node_a)->state->struct_size;
  size_t struct_size_b = ((a_star_node_t*)node_b)->state->struct_size;

  // Os tamanhos tem de ser iguais
  if(struct_size_a != struct_size_b)
    return false;

  // Compara os estados
  void* state_a = ((a_star_node_t*)node_a)->state->data;
  void* state_b = ((a_star_node_t*)node_b)->state->data;
  return memcmp(state_a, state_b, struct_size_a) == 0;
}

// Função para calcular o índice do bucket na hashtable
// Tal como a comparação esta função tem de ser especifica para
// esta estrutura já que é necessário usar os dados do estado
// para gerar o índice do hash
static size_t a_star_nodes_hash(hashtable_t* hashtable, const void* data)
{
  a_star_node_t* node = (a_star_node_t*)data;
  return hash_function(node->state->data, node->state->struct_size, hashtable->capacity);
}

// Função para encontrar qual a tarefa para qual devemos enviar o state_t
static size_t assign_to_worker(const a_star_t* a_star, const state_t* state)
{
  // Calcula o valor hash inicial
  size_t hash_value = hash_function(state->data, state->struct_size, 2048);

  // Retorna o qual o worker que deve ficar com este estado
  return hash_value % a_star->num_workers;
}

// Função interna para criar um nó a colocar na fila prioritária
a_star_node_t* a_star_new_node(a_star_t* a_star, state_t* state)
{
  if(a_star == NULL)
  {
    return NULL;
  }

  a_star_node_t* node = (a_star_node_t*)allocator_alloc(a_star->node_allocator);
  node->parent = NULL;
  node->g = 0;
  node->h = 0;
  node->state = state;

  // Indexamos este nó
  hashtable_insert(a_star->nodes, node);

  return node;
}

// Função que implementa a lógica de um trabalhador, aqui se processa o algoritmo A*
void* a_star_worker_function(void* arg)
{
  a_star_worker_t* worker = (a_star_worker_t*)arg;
  a_star_t* a_star = worker->a_star;

  while(a_star->running)
  {
    // Este trabalhador considera-se ocioso caso não existam nós a explorar ou mensagens destinadas a si
    worker->idle = worker->open_set->size == 0 && !channel_has_messages(a_star->channel, worker->thread_id);

    // Temos pelo menos um nó na nossa lista aberta que podemos processar
    if(worker->open_set->size)
    {
      // A seguinte operação pode ocorrer em O(log(N))
      // se nosAbertos é um min-heap ou uma queue prioritária
      heap_node_t top_element = min_heap_pop(worker->open_set);

      // Nó atual na nossa árvore
      a_star_node_t* current_node = (a_star_node_t*)top_element.data;
      worker->visited++;

      // Verificamos se já existe uma solução, caso já exista temos de verificar se
      // este trabalhador está a procurar por soluções que se encontram a uma distância maior
      // do que a solução já encontrada, será que vale a pena continuar? Consideramos que não e saímos.
      if(a_star->solution != NULL)
      {
        if(current_node->g > a_star->solution->g)
        {
          worker->idle = true;
          break;
        }
      }

      // Se encontramos o objetivo saímos e retornamos o nó
      if(a_star->goal_func(current_node->state, a_star->goal_state))
      {
        // Temos de informar que encontramos o nosso objetivo
        pthread_mutex_lock(&(a_star->lock));
        if(a_star->solution == NULL)
        {
          // Esta é a primeira solução encontrada nada de especial
          // a fazer
          a_star->solution = current_node;
        }
        else
        {
          // Já existe uma solução, temos de verificar se esta nova
          // solução tem um custo menor
          int existing_cost = a_star->solution->g + a_star->solution->h;
          int attempt_cost = current_node->g + current_node->h;
          if(existing_cost > attempt_cost)
          {
            a_star->solution = current_node;
          }
        }
        pthread_mutex_unlock(&(a_star->lock));
        // Este trabalhador já fez o seu trabalho pode parar
        worker->idle = true;
        break;
      }

      // Esta lista irá receber os vizinhos deste nó
      linked_list_t* neighbors = linked_list_create();

      // Executa a função que visita os vizinhos deste nó
      a_star->visit_func(current_node->state, a_star->state_allocator, neighbors);

      // Itera por todos os vizinhos expandidos e envia para a devida tarefa
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

    // Processamos todos os estados que estão no canal para esta tarefa
    // Aqui que ocorre a atualização do custo do estado
    while(channel_has_messages(a_star->channel, worker->thread_id))
    {
      a_star_message_t* message = channel_receive(a_star->channel, worker->thread_id);

      // Retiramos os dados da mensagem e libertamos a memória;
      a_star_node_t* parent_node = message->parent;
      state_t* state = message->state;
      free(message);

      // Se o nó pai não foi enviado é porque estamos a lidar com o estado inicial
      if(parent_node == NULL)
      {
        a_star_node_t* initial_node = a_star_new_node(a_star, state);
        // Atribui ao nó inicial um custo total de 0
        initial_node->g = 0;
        initial_node->h = a_star->h_func(initial_node->state, a_star->goal_state);

        // Inserimos o nó na nossa fila e saímos já que não existem mais mensagens
        min_heap_insert(worker->open_set, initial_node->h, initial_node);
        break;
      }

      // Recebemos um estado para ser processado, verificamos se já existe um nó para este estado
      a_star_node_t temp_node = { 0, 0, NULL, state };
      a_star_node_t* child_node = hashtable_contains(a_star->nodes, &temp_node);

      if(child_node != NULL)
      {
        // Encontra o custo de chegar do estado pai para este estado
        int g_attempt = parent_node->g + a_star->d_func(parent_node->state, child_node->state);

        // Se o custo for maior do que o nó já tem, não faz sentido atualizar
        // existe outro caminho mais curto para este estado
        if(g_attempt >= child_node->g)
        {
          continue;
        }

        // O estado pai é o caminho mais curto para este estado, atualizamos o pai deste estado
        child_node->parent = parent_node;

        // Guardamos o custo atual
        int old_cost = child_node->g + child_node->h;

        // Atualizamos os parâmetros do nó
        child_node->g = g_attempt;
        child_node->h = a_star->h_func(child_node->state, a_star->goal_state);

        // Calculamos o novo custo
        int new_cost = child_node->g + child_node->h;

        // Atualizamos a nossa fila prioritária
        min_heap_update(worker->open_set, old_cost, new_cost, child_node);
      }
      else
      {
        // Este nó ainda não existe, criamos um novo nó para este estado
        child_node = a_star_new_node(a_star, state);
        child_node->parent = parent_node;

        // Encontra o custo de chegar do estado pai a este estado e calculamos a heurística (distância para chegar ao objetivo)
        child_node->g = parent_node->g + a_star->d_func(parent_node->state, child_node->state);
        child_node->h = a_star->h_func(child_node->state, a_star->goal_state);

        // Calculamos o custo
        int cost = child_node->g + child_node->h;

        // Inserimos o nó na nossa fila
        min_heap_insert(worker->open_set, cost, child_node);
        worker->expanded++;
      }
    }
  }

  pthread_exit(NULL);
}

// Cria uma nova instância para resolver um problema
a_star_t* a_star_create(size_t struct_size,
                        goal_function goal_func,
                        visit_function visit_func,
                        heuristic_function h_func,
                        distance_function d_func,
                        int num_workers)
{
  a_star_t* a_star = (a_star_t*)malloc(sizeof(a_star_t));
  if(a_star == NULL)
  {
    return NULL; // Erro de alocação
  }

  // Inicializa os nossos alocadores de memória
  a_star->state_allocator = state_allocator_create(struct_size);
  if(a_star->state_allocator == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  a_star->node_allocator = allocator_create(sizeof(a_star_node_t));
  if(a_star->node_allocator == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  // Para mantermos controlo dos nós que já foram expandidos
  a_star->nodes = hashtable_create(sizeof(a_star_node_t), compare_a_star_nodes, a_star_nodes_hash);
  if(a_star->nodes == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  // Criamos um canal para que as threads possam comunicar
  a_star->channel = channel_create(num_workers);
  if(a_star->channel == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  a_star->num_workers = num_workers;
  a_star->workers = (a_star_worker_t*)malloc(num_workers * sizeof(a_star_worker_t));
  if(a_star->workers == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  // Inicializamos as estruturas que vão conter o estado de cada
  // um dos trabalhadores
  for(size_t i = 0; i < a_star->num_workers; i++)
  {
    a_star->workers[i].a_star = a_star;
    a_star->workers[i].thread_id = i;
    a_star->workers[i].open_set = min_heap_create();
    a_star->workers[i].idle = true;
    a_star->workers[i].visited = 0;
    a_star->workers[i].expanded = 0;
  }

  // Inicializa as funções necessárias para o algoritmo funcionar
  a_star->visit_func = visit_func;
  a_star->goal_func = goal_func;
  a_star->h_func = h_func;
  a_star->d_func = d_func;
  a_star->solution = NULL;
  a_star->goal_state = NULL;
  a_star->visited = 0;
  a_star->expanded = 0;

  return a_star;
}

// Liberta uma instância do algoritmo A*
void a_star_destroy(a_star_t* a_star)
{
  if(a_star == NULL)
  {
    return;
  }

  if(a_star->workers != NULL)
  {
    for(size_t i = 0; i < a_star->num_workers; i++)
    {
      min_heap_destroy(a_star->workers[i].open_set);
    }
  }

  if(a_star->channel != NULL)
  {
    channel_destroy(a_star->channel, true);
  }

  if(a_star->nodes != NULL)
  {
    hashtable_destroy(a_star->nodes, false);
  }

  if(a_star->state_allocator != NULL)
  {
    state_allocator_destroy(a_star->state_allocator);
  }

  if(a_star->node_allocator != NULL)
  {
    allocator_destroy(a_star->node_allocator);
  }

  // Destruímos o nosso algoritmo
  free(a_star);
}

// Resolve o problema através do uso do algoritmo A*;
a_star_node_t* a_star_solve(a_star_t* a_star, void* initial, void* goal, bool first_solution)
{
  if(a_star == NULL)
  {
    return NULL;
  }

  // Guarda os nossos estados inicial e objetivo
  state_t* initial_state = state_allocator_new(a_star->state_allocator, initial);

  if(initial_state == NULL)
  {
    return NULL;
  }

  // Preparamos o nosso objetivo caso tenha sido passado (existem problemas em que não se passam soluções)
  if(goal)
  {
    // Temos de "containerizar" o objetivo num estado
    a_star->goal_state = state_allocator_new(a_star->state_allocator, goal);

    if(a_star->goal_state == NULL)
    {
      return NULL;
    }
  }

  // Preparamos a mensagem a ser enviada para o estado inicial
  a_star_message_t* message = (a_star_message_t*)malloc(sizeof(a_star_message_t));
  if(message == NULL)
  {
    return NULL;
  }

  message->parent = NULL;
  message->state = initial_state;

  // Enviamos o estado inicial para o respetivo trabalhador
  size_t worker_id = assign_to_worker(a_star, initial_state);
  channel_send(a_star->channel, worker_id, message);

  // Com recurso a esta variável podemos enviar uma mensagem para os nossos trabalhadores
  // pararem
  a_star->running = true;

  for(size_t i = 0; i < a_star->num_workers; i++)
  {
    // Iniciamos o trabalhador
    a_star->workers[i].idle = false;
    int result = pthread_create(&(a_star->workers[i].thread), NULL, a_star_worker_function, &(a_star->workers[i]));
    if(result != 0)
    {
      printf("Erro a iniciar o trabalhador %ld\n", i);
      return NULL;
    }
  }

  size_t idle_workers = 0;
  while(a_star->running)
  {
    // Verificamos quantos workers estão ociosos, caso todos estejam ociosos assumimos que não existem mais nós a explorar
    // ou a solução já foi encontrada
    for(idle_workers = 0; idle_workers < a_star->num_workers && a_star->workers[idle_workers].idle; idle_workers++)
      ;

    // O algoritmo deve continuar a correr enquanto houver trabalhadores que não estejam ociosos, caso
    // tenhamos passado a opção first_solution saímos imediatamente após obtermos uma solução
    a_star->running = idle_workers < a_star->num_workers && (a_star->solution == NULL || !first_solution);
  }

  // Esperamos que todas os trabalhadores terminem
  for(size_t i = 0; i < a_star->num_workers; i++)
  {
    pthread_join(a_star->workers[i].thread, NULL);
    a_star->visited += a_star->workers[i].visited;
    a_star->expanded += a_star->workers[i].expanded;
  }

  return a_star->solution;
}
