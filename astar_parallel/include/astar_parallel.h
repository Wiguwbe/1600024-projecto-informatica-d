/*
   Algoritmo A* Paralelo
*/
#ifndef ASTAR_PARALLEL_H
#define ASTAR_PARALLEL_H
#include "astar.h"
#include "channel.h"
#include "min_heap.h"
#include "state.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct a_star_worker_t a_star_worker_t;
typedef struct a_star_scheduler_t a_star_scheduler_t;
typedef struct a_star_parallel_t a_star_parallel_t;

struct a_star_scheduler_t
{
  size_t num_workers;
  a_star_worker_t* workers;
  int next_worker;
  pthread_mutex_t lock;
};

// Estrutura que contem o estado do algoritmo A*
struct a_star_parallel_t
{
  // Configuração comum do algoritmo
  a_star_t* common;

  // Gestor de tarefas e canal de comunicação, e lock para sincronixação
  a_star_scheduler_t scheduler;
  channel_t* channel;
  pthread_mutex_t lock;

  // Variáveis necessárias para controlar a execução do algoritmo em paralelo
  bool stop_on_first_solution;
  bool running;
};

// Estrutura que guarda o estado de um trabalhador
struct a_star_worker_t
{
  a_star_parallel_t* a_star;

  // Variáveis especificas para threading e controlo de execução
  pthread_t thread;
  int thread_id;
  bool idle;

  // Nós abertos locais
  min_heap_t* open_set;

  // Variáveis para estatísticas
  int generated;
  int expanded;
  size_t max_min_heap_size;
  int nodes_new;
  int nodes_reinserted;
  int paths_worst_or_equals;
  int paths_better;
};

// Cria uma nova instância do algoritmo A* para resolver um problema
a_star_parallel_t* a_star_parallel_create(size_t struct_size,
                                          goal_function goal_func,
                                          visit_function visit_func,
                                          heuristic_function h_func,
                                          distance_function d_func,
                                          print_function print_func,
                                          int num_workers, bool first);

// Liberta uma instância do algoritmo A* paralelo
void a_star_parallel_destroy(a_star_parallel_t* a_star);

// Resolve o problema através do uso do algoritmo A* paralelo
void a_star_parallel_solve(a_star_parallel_t* a_star, void* initial, void* goal);

// Imprime estatísticas sobre o algoritmo paralelo
void a_star_parallel_print_statistics(a_star_parallel_t* a_star_instance, bool csv, bool show_solution);

#endif // ASTAR_H
