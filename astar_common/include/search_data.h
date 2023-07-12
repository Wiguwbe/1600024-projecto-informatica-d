#ifndef STATS_H
#define STATS_H
#include "linked_list.h"
#include "state.h"
#include <time.h>

enum search_action_e
{
  ACTION_VISITED = 0,
  ACTION_GOAL = 1,
  ACTION_SUCESSOR = 2,
  ACTION_END = 3
};

enum algorithm_type_e
{
  ALGO_SEQUENTIAL = 0,
  ALGO_PARALLEL_FIRST = 1,
  ALGO_PARALLEL_EXHAUSTIVE = 2
};

typedef struct search_data_t search_data_t;
typedef struct search_data_entry_t search_data_entry_t;

// Definições de callbacks
typedef size_t (*serialize_function)(char*, const search_data_entry_t*);

struct search_data_entry_t
{
  double frametime;
  enum search_action_e action;
  state_t* state;
};

struct search_data_t
{
  char* problem;
  char* instance;
  enum algorithm_type_e algorithm;
  int workers;
  double execution_time;
  linked_list_t** entries;
  struct timespec start_time;

  // Callbacks necessárias para guardar estatísticas
  serialize_function serialize_func;
};

void search_data_create(char* problem, char* instance, enum algorithm_type_e algo, int workers, serialize_function serialize_fn);
search_data_t* search_data_attach();
void search_data_destroy();
void search_data_start();
void search_data_end(double offset);
void search_data_add_entry(int worker, state_t* state, enum search_action_e action, double offset);
void search_data_print();
void search_data_save(const char* output_file);
#endif