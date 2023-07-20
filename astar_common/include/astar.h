#ifndef ASTAR_H
#define ASTAR_H
#include "linked_list.h"
#include "node.h"
#include "state.h"
#include <time.h>
#ifdef STATS_GEN
#  include "search_data.h"
#endif

// Informação comum partilhada por ambas as versões do algoritmo A*
typedef struct a_star_t a_star_t;

// Um nó do nosso algoritmo
typedef struct a_star_node_t a_star_node_t;

// Tipo para funções que calculam a heurística
typedef int (*heuristic_function)(const state_t*, const state_t*);

// Tipo para funções que expandem um estado nos estados filho, utilizando o alocador predefinido, atualiza a min_heap e a hash_table
typedef void (*visit_function)(state_t*, state_allocator_t*, linked_list_t*);

// Tipo para funções que verificam se um estado é o objetivo a atingir
typedef bool (*goal_function)(const state_t*, const state_t*);

// Tipo para funções que devolvem a distancia de um estado para o seu vizinho
typedef int (*distance_function)(const state_t*, const state_t*);

// Estrutura que contem o estado do algoritmo A*
struct a_star_t
{
  // Estruturas para gestão de nós e estados
  state_allocator_t* state_allocator;
  node_allocator_t* node_allocator;

  // Callbacks necessárias para o algoritmo funcionar
  goal_function goal_func;
  visit_function visit_func;
  heuristic_function h_func;
  distance_function d_func;

  // Solução e estado a atingir
  a_star_node_t* solution;
  state_t* goal_state;

  // Informação estatística
  int generated;
  int expanded;
  struct timespec start_time, end_time;
  double execution_time;
  size_t max_min_heap_size;
  int nodes_new;
  int nodes_reinserted;
  int paths_worst_or_equals;
  int paths_better;
  int num_solutions;
  int num_worst_solutions;
  int num_better_solutions;
};

// Funções comuns do algoritmo
a_star_t* a_star_create(size_t struct_size,
                        goal_function goal_func,
                        visit_function visit_func,
                        heuristic_function h_func,
                        distance_function d_func,
                        print_function print_func);

// Liberta uma instância do algoritmo A* sequencial
void a_star_destroy(a_star_t* a_star);

// Imprime as estatísticas possíveis
void a_star_print_statistics(a_star_t* a_star, bool csv, bool show_solution);

#ifdef STATS_GEN
void a_star_attach_search_data(a_star_t* a_star);
#endif

#endif