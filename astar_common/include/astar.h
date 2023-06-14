#ifndef ASTAR_H
#define ASTAR_H
#include "linked_list.h"
#include "state.h"
#include <time.h>

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

// Tipo para funções que expandem um estado nos estados filho, utilizando o alocador predefinido, atualiza a min_heap e a hash_table
typedef void (*print_function)(a_star_node_t*);

// Estrutura que define um estado para o algoritmo A* (data contêm user-defined data)
struct a_star_node_t
{
  int g;
  int h;
  a_star_node_t* parent;
  state_t* state;
};

// Estrutura que contem o estado do algoritmo A*
struct a_star_t
{
  // Estruturas para gestão de nós e estados
  state_allocator_t* state_allocator;
  allocator_t* node_allocator;
  hashtable_t* nodes;

  // Callbacks necessárias para o algoritmo funcionar
  goal_function goal_func;
  visit_function visit_func;
  heuristic_function h_func;
  distance_function d_func;

  // Solução e estado a atingir
  a_star_node_t* solution;
  state_t* goal_state;

  // Informação estatística
  int expanded;
  int visited;
  struct timespec start_time, end_time;
  double execution_time;
};

// Funções comuns do algoritmo
a_star_t* a_star_create(
    size_t struct_size, goal_function goal_func, visit_function visit_func, heuristic_function h_func, distance_function d_func);

// Liberta uma instância do algoritmo A* sequencial
void a_star_destroy(a_star_t* a_star);

// Função interna para criar um novo nó para ser utilizado pelo nosso algoritmo
a_star_node_t* a_star_new_node(a_star_t* a_star, state_t* state);

// Imprime as estatísticas possíveis
void a_star_print_statistics(a_star_t* a_star, bool csv, print_function print_fn);

#endif