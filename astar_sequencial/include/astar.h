/*
   Algoritmo A* Sequencial


   ```
*/
#ifndef ASTAR_H
#define ASTAR_H
#include "allocator.h"
#include "hashtable.h"
#include "linked_list.h"
#include "min_heap.h"
#include "state.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct a_star_node_t a_star_node_t;
// Estrutura que define um estado para o algoritmo A* (data contêm user-defined data)
struct a_star_node_t
{
  int g;
  int h;
  a_star_node_t* parent;
  state_t* state;
  bool visited; 
};

// Tipo para funções que calculam a heuristica
typedef int (*heuristic_function)(const state_t*, const state_t*);

// Tipo para funções que expandem um estado nos estados filho, utilizando o alocador predefinido, atualiza a min_heap e a hash_table
typedef void (*visit_function)(state_t*, state_allocator_t*, linked_list_t*);

// Tipo para funções que verificam se um estado é o objectivo a atingir
typedef bool (*goal_function)(const state_t*, const state_t*);

// Tipo para funções que devolvem a distancia de um estado para o seu vizinho
typedef int (*distance_function)(const state_t*, const state_t*);

// Estrutura que contem o estado do algoritmo A*
typedef struct
{
  state_allocator_t* state_allocator;
  allocator_t* node_allocator;
  min_heap_t* open_set;
  hashtable_t* nodes;
  goal_function goal_func;
  visit_function visit_func;
  heuristic_function h_func;
  distance_function d_func;
} a_star_t;

// Cria uma nova instância para resolver um problema
a_star_t* a_star_create(
    size_t struct_size, goal_function goal_func, visit_function visit_func, heuristic_function h_func, distance_function d_func);

// Liberta uma instância do algoritmo A*
void a_star_destroy(a_star_t* a_star);

// Resolve o problema através do uso do algoritmo A*;
a_star_node_t* a_star_solve(a_star_t* a_star, void* initial, void* goal, size_t struct_size);

#endif // ASTAR_H
