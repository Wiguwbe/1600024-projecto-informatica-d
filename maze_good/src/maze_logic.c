#include "maze_logic.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef STATS_GEN
#  include <time.h>
#endif

void update_neighbors(maze_solver_t* maze_solver, coord new_position, state_allocator_t* allocator, linked_list_t* neighbors)
{
  int index = new_position.row * maze_solver->cols + new_position.col;
  if(maze_solver->initial_board[index] != '.')
  {
    // não podemos colocar um link na coordenada passada, este estado não
    // é valido
    return;
  }

  maze_solver_state_t new_board;
  new_board.maze_solver = maze_solver;
  new_board.position.col = new_position.col;
  new_board.position.row = new_position.row;
  state_t* new_state = state_allocator_new(allocator, &new_board);
  linked_list_append(neighbors, new_state);
}

// Função de heurística para o puzzle 8
int heuristic(const state_t* current_state, const state_t*)
{
  // Converter os estados para puzzle_state
  maze_solver_state_t* state = (maze_solver_state_t*)(current_state->data);
  maze_solver_t* maze_solver = state->maze_solver;

  int h = abs(state->position.col - maze_solver->exit_coord.col) + abs(state->position.row - maze_solver->exit_coord.row);
  return h;
}

void visit(state_t* current_state, state_allocator_t* allocator, linked_list_t* neighbors)
{
  maze_solver_state_t* state = (maze_solver_state_t*)current_state->data;

  int up, down, left, right, col, row;

  // Lemos as coordenadas atuais e movemos cada um dos links
  row = down = up = state->position.row;
  col = right = left = state->position.col;

  down++;
  up--;
  right++;
  left--;

  // Verifica se o movimento para baixo é válido
  if(down > -1 && down < state->maze_solver->rows)
  {
    coord new_position = { col, down };
    update_neighbors(state->maze_solver, new_position, allocator, neighbors);
  }

  // Verifica se o movimento para cima é válido
  if(up > -1 && up < state->maze_solver->rows)
  {
    coord new_position = { col, up };
    update_neighbors(state->maze_solver, new_position, allocator, neighbors);
  }

  // Verifica se o movimento para a esquerda é válido
  if(left > -1 && left < state->maze_solver->cols)
  {
    coord new_position = { left, row };
    update_neighbors(state->maze_solver, new_position, allocator, neighbors);
  }

  // Verifica se o movimento para a direita é válido
  if(right > -1 && right < state->maze_solver->cols)
  {
    coord new_position = { right, row };
    update_neighbors(state->maze_solver, new_position, allocator, neighbors);
  }
}

// Verifica se um estado é um objetivo do problema number link
bool goal(const state_t* state_a, const state_t*)
{
  maze_solver_state_t* state = (maze_solver_state_t*)state_a->data;

  return state->position.row == state->maze_solver->exit_coord.row && state->position.col == state->maze_solver->exit_coord.col;
}

int distance(const state_t*, const state_t*)
{
  return 1;
}

#ifdef STATS_GEN
size_t maze_serialize_function(char* buffer, const search_data_entry_t* entry)
{
  maze_solver_state_t* state = (maze_solver_state_t*)entry->state->data;

  // We always print the position
  return sprintf(buffer, "\"position\":[%d,%d]", state->position.col, state->position.row);
}
#endif
