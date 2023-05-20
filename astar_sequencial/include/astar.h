/*
   Algoritmo A* Sequencial


   ```
*/
#ifndef ASTAR_H
#define ASTAR_H
#include <stdbool.h>
#include <stddef.h>
#include "allocator.h"
#include "hashtable.h"
#include "min_heap.h"
#include "linked_list.h"

// Estrutura que define um estado para o algoritmo A* (data contêm user-defined data)
typedef struct state_t
{
  int g;
  int h;
  void* parent;
  void* data;
} state_t;

// Tipo para funções que calculam a heuristica
typedef int (*heuristic_function)(state_t*, void*);

// Tipo para funções que expandem um estado nos estados filho, utilizando o alocador predefinido, atualiza a min_heap e a hash_table
typedef void (*visit_function)(state_t*, allocator_t*, hashtable_t*, linked_list_t*);

// Tipo para funções que verificam se um estado é o objectivo a atingir
typedef bool (*goal_function)(state_t*, void*);

// Tipo para funções que devolvem a distancia de um estado para o seu vizinho
typedef int (*distance_function)(state_t*, state_t*);

// Estrutura que define um caminho do objectivo ao inicio
typedef struct
{
  state_t* state;
  state_t* previous;
} path_t;

// Estrutura que contem o estado do algoritmo A*
typedef struct
{
  allocator_t* allocator;
  min_heap_t* open_set;
  hashtable_t* visited_set;
  goal_function goal_func;
  visit_function visit_func;
  heuristic_function h_func;
  distance_function d_func;
} a_star_t;

// Cria uma nova instância para resolver um problema
a_star_t* a_star_create(allocator_t* allocator,
                      size_t data_size,
                      void* goal,
                      goal_function goal_func,
                      visit_function visit_func,
                      heuristic_function h_func,
                      distance_function d_func);

// Liberta uma instância do algoritmo A*
void a_star_destroy(a_star_t* a_star);

// Resolve o problema através do uso do algoritmo A*;
path_t* a_star_solve(a_star_t* a_star, state_t* initial, void* goal);

// Retorna o caminho até o estado
path_t* a_star_path_to(a_star_t* a_star, state_t* state);

#endif // ASTAR_H
