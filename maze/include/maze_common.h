#ifndef NUMBER_LINK_H
#define NUMBER_LINK_H
#include "allocator.h"
#include "hashtable.h"

typedef struct
{
  int col, row;
} coord;

typedef struct
{
  char* board; // Contem o tabuleiro preenchido;
  coord position; // Contem a posição onde cada para está
} board_data_t;

typedef struct
{
  hashtable_t* hashtable;
  allocator_t* allocator;
  coord entry_coord;
  coord exit_coord;
  char* initial_board;
  int cols;
  int rows;
  size_t struct_size;
  size_t board_len;
} maze_solver_t;


maze_solver_t* maze_solver_init(int rows, int cols, const char* board);

void maze_solver_destroy(maze_solver_t* maze_solver);

board_data_t maze_solver_wrap_board(maze_solver_t* maze_solver, char* data);

void* maze_solver_create_board(maze_solver_t* maze_solver, const char* board, coord coords);

#endif