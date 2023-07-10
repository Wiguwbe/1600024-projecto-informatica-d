#include "maze_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

maze_solver_t* maze_solver_init(int rows, int cols, const char* board)
{
  // Inicializamos a nossa estrutura
  maze_solver_t* maze_solver = (maze_solver_t*)malloc(sizeof(maze_solver_t));

  if(maze_solver == NULL)
  {
    return NULL;
  }

  maze_solver->rows = rows;
  maze_solver->cols = cols;

  // Garante a memoria limpa
  maze_solver->hashtable = NULL;
  maze_solver->allocator = NULL;
  maze_solver->initial_board = NULL;
  maze_solver->entry_coord = (coord){ 1, 0 };
  maze_solver->exit_coord = (coord){ cols - 2, rows - 1 };

  // Preparamos outros dados importantes para a resolução do nosso problema
  maze_solver->board_len = (size_t)(rows * cols);
  maze_solver->initial_board = malloc(rows * cols);
  memcpy(maze_solver->initial_board, board, rows * cols);

  // Colocamos o caracter 'c' no inicio do labirinto
  maze_solver->initial_board[1]='c';

  if(maze_solver->initial_board == NULL)
  {
    maze_solver_destroy(maze_solver);
    return NULL; // Erro de alocação
  }
  
  maze_solver->struct_size = maze_solver->board_len * sizeof(char) + sizeof(coord);
  maze_solver->allocator = allocator_create(maze_solver->struct_size);

  if(maze_solver->allocator == NULL)
  {
    maze_solver_destroy(maze_solver);
    return NULL; // Erro de alocação
  }

  maze_solver->hashtable = hashtable_create(maze_solver->struct_size, NULL, NULL);
  if(maze_solver->hashtable == NULL)
  {
    maze_solver_destroy(maze_solver);
    return NULL; // Erro de alocação
  }

  return maze_solver;
}

void maze_solver_destroy(maze_solver_t* maze_solver)
{
  if(maze_solver == NULL)
  {
    return;
  }

  if(maze_solver->hashtable != NULL)
  {
    hashtable_destroy(maze_solver->hashtable, false);
  }

  if(maze_solver->allocator != NULL)
  {
    allocator_destroy(maze_solver->allocator);
  }

  if(maze_solver->initial_board != NULL)
  {
    free(maze_solver->initial_board);
  }

  free(maze_solver);
  maze_solver = NULL;
}

board_data_t maze_solver_wrap_board(maze_solver_t* maze_solver, char* data)
{
  board_data_t state = { NULL, {0,0} };

  if(maze_solver == NULL)
  {
    return state;
  }

  char* ptr = data;

  state.board = ptr;
  ptr += maze_solver->board_len;
  
  memcpy(&state.position,ptr,sizeof(coord));

  return state;
}

void* maze_solver_create_board(maze_solver_t* maze_solver, const char* board_data, coord position)
{
  // Sem alocador não há dados
  if(maze_solver == NULL)
  {
    return NULL;
  }

  char tmp_board[maze_solver->struct_size];

  // Copiamos os dados passados para a memoria alocada
  char* ptr = (char*)&tmp_board;
  memcpy(ptr, board_data, maze_solver->board_len);
  ptr += maze_solver->board_len;
  memcpy(ptr, &position, sizeof(coord));

  // Verificamos se a configuração passada já existe em memória, e caso exista
  // retornamos o estado correspondente que aponta para esses locais na memória
  void* board = hashtable_contains(maze_solver->hashtable, &tmp_board);

  if(!board)
  {
    // O tabuleiro nesta configuração ainda não existe, alocamos e indexamos
    board = (void*)allocator_alloc(maze_solver->allocator);
    memcpy(board, &tmp_board, maze_solver->struct_size);
    hashtable_insert(maze_solver->hashtable, board);
  }

  return board;
}
