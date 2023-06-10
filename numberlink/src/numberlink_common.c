#include "numberlink_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mapeia os caracteres números A,B,C...Z para 0,1,2...
int letter_to_int(char c)
{
  return c - 65;
}

// Mapeia 0,1,2... para a,b,c...z
int letter_to_lower(char c)
{
  return 97 + letter_to_int(c);
}

// Mapeia para A,B,C...Z to 0,1,2...
char number_to_letter(int n)
{
  return 65 + n;
}

// Mapeia para A,B,C...Z to 0,1,2...
char number_to_lower(int n)
{
  return letter_to_lower(number_to_letter(n));
}

number_link_t* number_link_init(int rows, int cols, const char* board)
{
  int max_pairs = letter_to_int('Z');
  coord pair_starts[max_pairs];
  coord pair_goals[max_pairs];
  coord zero = {-1,-1};

  for(int i = 0; i < max_pairs; i++)
  {
    pair_starts[i] = zero;
    pair_goals[i] = zero;
  }

  // Inicializamos a nossa estrutura
  number_link_t* number_link = (number_link_t*)malloc(sizeof(number_link_t));

  number_link->rows = rows;
  number_link->cols = cols;

  if(number_link == NULL)
  {
    return NULL;
  }

  // Processa o tabuleiro passado para ver se tem toda a informação
  // necessário e é um tabuleiro válido
  number_link->num_pairs = 0;
  int num_starts = 0;
  int num_ends = 0;

  for(int y = 0; y < number_link->rows; y++)
  {
    for(int x = 0; x < number_link->cols; x++)
    {
      int index = y * number_link->cols + x;
      char c = board[index];
      if(c == '.')
      {
        continue;
      }

      int pair = letter_to_int(c);

      if(pair < 0 || pair > max_pairs)
      {
        free(number_link);
        return NULL;
      }

      coord current_coord = { x, y };

      if(pair_starts[pair].col == -1)
      {
        // Primeiro elemento do par encontrado
        pair_starts[pair] = current_coord;
        number_link->num_pairs++;
        num_starts ++;
        continue;
      }

      pair_goals[pair] = current_coord;
      num_ends++;
    }
  }

  // Os pares tem de ter inicio e fim
  if (num_ends != num_starts) {
    free(number_link);
    return NULL; 
  }

  // Alocamos a memória para guardar a informação sobre onde os pares iniciam e terminam
  // e guardamos os dados
  number_link->starts = (coord*)malloc(number_link->num_pairs * sizeof(coord));
  if(number_link->starts == NULL)
  {
    free(number_link);
    return NULL; // Erro de alocação
  }
  memcpy(number_link->starts, &pair_starts, number_link->num_pairs * sizeof(coord));

  number_link->initial_coords = (coord*)malloc(number_link->num_pairs * sizeof(coord));
  if(number_link->initial_coords == NULL)
  {
    free(number_link->starts);
    free(number_link);
    return NULL; // Erro de alocação
  }
  memcpy(number_link->initial_coords, &pair_starts, number_link->num_pairs * sizeof(coord));

  number_link->goals = (coord*)malloc(number_link->num_pairs * sizeof(coord));
  if(number_link->goals == NULL)
  {
    free(number_link->initial_coords);
    free(number_link->starts);
    free(number_link);
    return NULL; // Erro de alocação
  }
  memcpy(number_link->goals, &pair_goals, number_link->num_pairs * sizeof(coord));

  // Preparamos outros dados importantes para a resolução do nosso problema
  number_link->board_len = (size_t)(rows * cols);
  number_link->initial_board = malloc(rows * cols);
  memcpy(number_link->initial_board, board, rows * cols);

  if(number_link->initial_board == NULL)
  {
    free(number_link->goals);
    free(number_link->initial_coords);
    free(number_link->starts);
    free(number_link);
    return NULL; // Erro de alocação
  }

  number_link->struct_size = number_link->board_len * sizeof(char) + number_link->num_pairs * sizeof(coord);
  number_link->allocator = allocator_create(number_link->struct_size);

  if(number_link->allocator == NULL)
  {
    free(number_link->initial_board);
    free(number_link->goals);
    free(number_link->initial_coords);
    free(number_link->starts);
    free(number_link);
    return NULL; // Erro de alocação
  }

  number_link->hashtable = hashtable_create(number_link->struct_size, NULL, NULL);
  if(number_link->hashtable == NULL)
  {
    allocator_destroy(number_link->allocator);
    free(number_link->initial_board);
    free(number_link->goals);
    free(number_link->initial_coords);
    free(number_link->starts);
    free(number_link);
    return NULL; // Erro de alocação
  }

  return number_link;
}

void number_link_destroy(number_link_t* number_link)
{
  if(number_link == NULL)
  {
    return;
  }

  hashtable_destroy(number_link->hashtable, false);
  allocator_destroy(number_link->allocator);
  free(number_link->initial_board);
  free(number_link->goals);
  free(number_link->initial_coords);
  free(number_link->starts);
  free(number_link);
  number_link = NULL;
}

board_data_t number_link_wrap_board(number_link_t* number_link, char* data)
{
  board_data_t state = { NULL, NULL };

  if(number_link == NULL)
  {
    return state;
  }

  char* ptr = data;

  state.board = ptr;
  ptr += number_link->board_len;
  state.coords = (coord*)ptr;

  return state;
}

void* number_link_create_board(number_link_t* number_link, const char* board_data, const coord* coords)
{
  // Sem alocador não há dados
  if(number_link == NULL)
  {
    return NULL;
  }

  char tmp_board[number_link->struct_size];

  // Copiamos os dados passados para a memoria alocada
  char* ptr = (char*)&tmp_board;
  memcpy(ptr, board_data, number_link->board_len);
  ptr += number_link->board_len;
  memcpy(ptr, coords, number_link->num_pairs * sizeof(coord));

  // Verificamos se a configuração passada já existe em memória, e caso exista
  // retornamos o estado correspondente que aponta para esses locais na memória
  void* board = hashtable_contains(number_link->hashtable, &tmp_board);

  if(!board)
  {
    // O tabuleiro nesta configuração ainda não existe, alocamos e indexamos
    board = (void*)allocator_alloc(number_link->allocator);
    memcpy(board, &tmp_board, number_link->struct_size);
    hashtable_insert(number_link->hashtable, board);
  }

  return board;
}
