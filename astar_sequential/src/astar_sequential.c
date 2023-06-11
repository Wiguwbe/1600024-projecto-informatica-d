#include "astar_sequential.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Cria uma nova instância para resolver um problema
a_star_sequential_t* a_star_sequential_create(
    size_t struct_size, goal_function goal_func, visit_function visit_func, heuristic_function h_func, distance_function d_func)
{
  a_star_sequential_t* a_star = (a_star_sequential_t*)malloc(sizeof(a_star_sequential_t));
  if(a_star == NULL)
  {
    return NULL; // Erro de alocação
  }

  // Garante que a memoria esteja limpa
  a_star->open_set = NULL;
  a_star->common = NULL;

  // Inicializamos a parte comum do nosso algoritmo
  a_star->common = a_star_create(struct_size, goal_func, visit_func, h_func, d_func);

  if(a_star->common == NULL)
  {
    a_star_sequential_destroy(a_star);
    return NULL;
  }

  // Inicializamos a parte especifica para o algoritmo sequencial

  // Conjunto com os nós por expandir e que também possam necessitar de ser expandidos
  // novamente.
  a_star->open_set = min_heap_create();
  if(a_star->open_set == NULL)
  {
    a_star_sequential_destroy(a_star);
    return NULL;
  }

  return a_star;
}

// Liberta uma instância do algoritmo A*
void a_star_sequential_destroy(a_star_sequential_t* a_star)
{
  if(a_star == NULL)
  {
    return;
  }

  // Limpamos a nossa fronteira
  if(a_star->open_set != NULL)
  {
    min_heap_destroy(a_star->open_set);
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
void a_star_sequential_solve(a_star_sequential_t* a_star, void* initial, void* goal)
{
  if(a_star == NULL)
  {
    return;
  }

  // Guarda os nossos estados iniciais e objetivo
  state_t* initial_state = state_allocator_new(a_star->common->state_allocator, initial);

  // Existe problemas em que o objetivo pode ser nulo, normalmente
  // nesses casos o objetivo é estático (ie. 8puzzle)
  if(goal)
  {
    // Caso tenhamos passado o goal, temos de o "containerizar" num estado
    a_star->common->goal_state = state_allocator_new(a_star->common->state_allocator, goal);

    if(a_star->common->goal_state == NULL)
    {
      return;
    }
  }

  // O open_set deve estar vazio, ou caso contrário o algoritmo não funciona bem
  if(a_star->open_set->size > 0)
  {
    return;
  }

  a_star_node_t* initial_node = a_star_new_node(a_star->common, initial_state);

  // Atribui ao nó inicial um custo total de 0
  initial_node->g = 0;
  initial_node->h = a_star->common->h_func(initial_node->state, a_star->common->goal_state);

  // Inserimos o nó inicial na nossa fila prioritária
  min_heap_insert(a_star->open_set, initial_node->g + initial_node->h, initial_node);

  clock_gettime(CLOCK_MONOTONIC, &(a_star->common->start_time));
  while(a_star->open_set->size)
  {
    // A seguinte operação pode ocorrer em O(log(N))
    // se nosAbertos é um min-heap ou uma queue prioritária
    heap_node_t top_element = min_heap_pop(a_star->open_set);

    // Nó atual na nossa árvore
    a_star_node_t* current_node = (a_star_node_t*)top_element.data;
    a_star->common->visited++;

    // Se encontramos o objetivo saímos e retornamos o nó
    if(a_star->common->goal_func(current_node->state, a_star->common->goal_state))
    {
      // Guardamos a solução e saímos do ciclo
      a_star->common->solution = current_node;
      break;
    }

    // Esta lista irá receber os vizinhos deste nó
    linked_list_t* neighbors = linked_list_create();

    // Executa a função que visita os vizinhos deste nó
    a_star->common->visit_func(current_node->state, a_star->common->state_allocator, neighbors);

    // Itera por todos os vizinhos expandidos e atualiza a nossa árvore de procura
    for(size_t i = 0; i < linked_list_size(neighbors); i++)
    {
      state_t* neighbor = (state_t*)linked_list_get(neighbors, i);

      // Verifica se o nó para este estado já se encontra na nossa lista de nós
      a_star_node_t temp_node = { 0, 0, NULL, neighbor };
      a_star_node_t* neighbor_node = hashtable_contains(a_star->common->nodes, &temp_node);

      if(neighbor_node != NULL)
      {
        // Encontra o custo de chegar do nó a este vizinho
        int g_attempt = current_node->g + a_star->common->d_func(current_node->state, neighbor_node->state);

        // Se o custo for maior do que o nó já tem, não faz sentido atualizar
        // existe outro caminho mais curto para este nó
        if(g_attempt >= neighbor_node->g)
          continue;

        // O nó atual é o caminho mais curto para este vizinho, atualizamos
        neighbor_node->parent = current_node;

        // Guardamos o custo atual
        int old_cost = neighbor_node->g + neighbor_node->h;

        // Atualizamos os parâmetros do nó
        neighbor_node->g = g_attempt;
        neighbor_node->h = a_star->common->h_func(neighbor_node->state, a_star->common->goal_state);

        // Calculamos o novo custo
        int new_cost = neighbor_node->g + neighbor_node->h;

        // Atualizamos a nossa fila prioritária
        min_heap_update(a_star->open_set, old_cost, new_cost, neighbor_node);
      }
      else
      {
        // Este nó ainda não existe, criamos um novo nó
        neighbor_node = a_star_new_node(a_star->common, neighbor);
        neighbor_node->parent = current_node;

        // Encontra o custo de chegar do nó a este vizinho e calcula a heurística para chegar ao objetivo
        neighbor_node->g = current_node->g + a_star->common->d_func(current_node->state, neighbor_node->state);
        neighbor_node->h = a_star->common->h_func(neighbor_node->state, a_star->common->goal_state);

        // Calculamos o custo
        int cost = neighbor_node->g + neighbor_node->h;

        // Inserimos o nó na nossa fila
        min_heap_insert(a_star->open_set, cost, neighbor_node);
        a_star->common->expanded++;
      }
    }

    // Liberta a lista de vizinhos
    linked_list_destroy(neighbors);
  }
  clock_gettime(CLOCK_MONOTONIC, &(a_star->common->end_time));

  // Calculamos o tempo de execução
  a_star->common->execution_time = (a_star->common->end_time.tv_sec - a_star->common->start_time.tv_sec);
  a_star->common->execution_time += (a_star->common->end_time.tv_nsec - a_star->common->start_time.tv_nsec) / 1000000000.0;
}

// Imprime estatísticas do algoritmo sequencial no formato desejado
void print_sequential_statistics(a_star_sequential_t* a_star, bool csv, print_solution_function print_solution)
{
  if(a_star == NULL)
  {
    return;
  }

  if(!csv)
  {
    if(a_star->common->solution)
    {
      printf("Resultado do algoritmo: Solução encontrada, custo: %d\n", a_star->common->solution->g);
      print_solution(a_star->common->solution);
    }
    else
    {
      printf("Resultado do algoritmo: Solução não encontrada.\n");
    }

    printf("Estatísticas Globais:\n");
    printf("- Estados expandidos: %d\n", a_star->common->expanded);
    printf("- Estados visitados: %d\n", a_star->common->visited);
    printf("- Tempo de execução: %.6f segundos.\n", a_star->common->execution_time);
  }
  else
  {
    printf("\"Solução\";\"Custo da Solução\";\"Estados Expandidos\";\"Estados Visitados\";\"Tempo de Execução\"\n");
    printf("\"%s\";%d;%d;%d;%.6f\n",
           a_star->common->solution ? "sim" : "não",
           a_star->common->solution ? a_star->common->solution->g : 0,
           a_star->common->expanded,
           a_star->common->visited,
           a_star->common->execution_time);
  }
}