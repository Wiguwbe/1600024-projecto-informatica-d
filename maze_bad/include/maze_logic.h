#ifndef LOGIC_H
#define LOGIC_H
#include "maze_common.h"
#include "linked_list.h"
#include "state.h"
#ifdef STATS_GEN
#include "search_data.h"
#endif

// Estrutura do que contem o estado do nosso number link
typedef struct
{
  maze_solver_t* maze_solver;
  char* board_data; // Tabuleiro
} maze_solver_state_t;

int heuristic(const state_t*, const state_t*);

void visit(state_t*, state_allocator_t*, linked_list_t*);

bool goal(const state_t*, const state_t*);

int distance(const state_t*, const state_t*);

#ifdef STATS_GEN
size_t maze_serialize_function(char*, const search_data_entry_t*);
#endif

#endif