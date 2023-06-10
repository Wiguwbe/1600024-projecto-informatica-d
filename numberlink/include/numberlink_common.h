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
  coord* coords; // Contem a posição onde cada para está
} board_data_t;

typedef struct
{
  hashtable_t* hashtable;
  allocator_t* allocator;
  int num_pairs;
  coord* starts;
  coord* initial_coords;
  coord* goals;
  char* initial_board;
  int cols;
  int rows;
  size_t struct_size;
  size_t board_len;
} number_link_t;

// Mapeia os caracteres números A,B,C...Z para 0,1,2...
int letter_to_int(char c);

// Mapeia 0,1,2... para a,b,c...z
int letter_to_lower(char c);

// Mapeia para A,B,C...Z to 0,1,2...
char number_to_letter(int n);

// Mapeia para A,B,C...Z to 0,1,2...
char number_to_lower(int n);

// Inicializa o problema para lidar com um tabuleiro de rows * cols, e n número de links
number_link_t* number_link_init(int rows, int cols, const char* board);

// Liberta a memória utilizada pelo problema number link
void number_link_destroy(number_link_t* number_link);

// Converte a zona de memória para o formato de fácil acesso
board_data_t number_link_wrap_board(number_link_t* number_link, char* data);

// Cria um novo tabuleiro
void* number_link_create_board(number_link_t* number_link, const char* board, const coord* coords);

#endif