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

  return maze_solver;
}

void maze_solver_destroy(maze_solver_t* maze_solver)
{
  if(maze_solver == NULL)
  {
    return;
  }

  if(maze_solver->initial_board != NULL)
  {
    free(maze_solver->initial_board);
  }

  free(maze_solver);
  maze_solver = NULL;
}
