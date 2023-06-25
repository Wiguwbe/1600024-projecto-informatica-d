/*
   Algoritmo A* Sequencial
*/
#ifndef ASTAR_SEQUENTIAL_H
#define ASTAR_SEQUENTIAL_H
#include "astar.h"
#include "min_heap.h"
#include "state.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct a_star_sequential_t a_star_sequential_t;

// Estrutura que contem o estado do algoritmo A*
struct a_star_sequential_t
{
  // Informação comum do nosso algoritmo
  a_star_t* common;

  // Especifico para o algoritmo sequencial
  min_heap_t* open_set;
};

// Cria uma nova instância do algoritmo A* sequencial para resolver um problema
a_star_sequential_t* a_star_sequential_create(size_t struct_size,
                                              goal_function goal_func,
                                              visit_function visit_func,
                                              heuristic_function h_func,
                                              distance_function d_func,
                                              print_function print_func);

// Liberta uma instância do algoritmo A* sequencial
void a_star_sequential_destroy(a_star_sequential_t* a_star);

// Resolve o problema através do uso do algoritmo A* sequencial
void a_star_sequential_solve(a_star_sequential_t* a_star, void* initial, void* goal);

// Imprime estatísticas sobre o algoritmo sequencial
void a_star_sequential_print_statistics(a_star_sequential_t* a_star_instance, bool csv);

#endif // ASTAR_SEQUENTIAL_H
