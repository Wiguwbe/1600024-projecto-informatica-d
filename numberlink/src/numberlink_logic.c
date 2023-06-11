#include "numberlink_logic.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void update_neighbors(number_link_t* number_link,
                      int pair,
                      coord new_coord,
                      board_data_t board_data,
                      state_allocator_t* allocator,
                      linked_list_t* neighbors,
                      int matched_pairs)
{
  char tmp_board[number_link->board_len];
  coord tmp_curr[number_link->num_pairs];

  // Copia a configuração do tabuleiro atual
  memcpy(&tmp_board, board_data.board, number_link->board_len);
  memcpy(&tmp_curr, board_data.coords, number_link->num_pairs * sizeof(coord));

  // Atualizamos a nossa coordenada atual para este par
  tmp_curr[pair] = new_coord;

  // Iniciamos um novo tabuleiro
  number_link_state_t new_board;
  new_board.number_link = number_link;
  new_board.matched_pairs = matched_pairs;

  // Verificamos se chegamos ao objetivo deste par
  if(memcmp(&new_coord, &(number_link->goals[pair]), sizeof(coord)) == 0)
  {
    // Aumentamos o numero de pares que já estão ligados
    new_board.matched_pairs++;
  }
  else
  {
    // Verificamos se podemos colocar um link na posição solicitada
    int index = new_coord.row * number_link->cols + new_coord.col;
    if(board_data.board[index] != '.')
    {
      // não podemos colocar um link na coordenada passada, este estado não
      // é valido
      return;
    }
    // Colocamos a respetiva letra no tabuleiro e atualizamos a posição para este par
    tmp_board[index] = number_to_lower(pair);
  }

  // Alocamos o tabuleiro
  new_board.board_data = number_link_create_board(number_link, (const char*)&tmp_board, (const coord*)&tmp_curr);
  state_t* new_state = state_allocator_new(allocator, &new_board);
  linked_list_append(neighbors, new_state);
}

// Função de heurística para o puzzle 8
int heuristic(const state_t* current_state, const state_t*)
{
  // Converter os estados para puzzle_state
  number_link_state_t* state = (number_link_state_t*)(current_state->data);
  number_link_t* number_link = state->number_link;
  board_data_t board_data = number_link_wrap_board(number_link, state->board_data);

  int h = 0;
  for(int i = 0; i < number_link->num_pairs; i++)
  {
    coord curr = board_data.coords[i];
    coord goal = number_link->goals[i];
    h += abs(curr.col - goal.col) + abs(curr.row - goal.row);
  }

  return h;
}

void do_moves(
    int pair, board_data_t board_data, number_link_state_t* state, state_allocator_t* allocator, linked_list_t* neighbors)
{
  int up, down, left, right, col, row;

  if(memcmp(&(board_data.coords[pair]), &(state->number_link->goals[pair]), sizeof(coord)) == 0)
  {
    // Este par já se encontra ligado, nada a fazer
    return;
  }

  // Lemos as coordenadas atuais e movemos cada um dos links
  row = down = up = board_data.coords[pair].row;
  col = right = left = board_data.coords[pair].col;

  down++;
  up--;
  right++;
  left--;

  // Verifica se o movimento para baixo é válido
  if(down > -1 && down < state->number_link->rows)
  {
    coord new_coord = { col, down };
    update_neighbors(state->number_link, pair, new_coord, board_data, allocator, neighbors, state->matched_pairs);
  }

  // Verifica se o movimento para cima é válido
  if(up > -1 && up < state->number_link->rows)
  {
    coord new_coord = { col, up };
    update_neighbors(state->number_link, pair, new_coord, board_data, allocator, neighbors, state->matched_pairs);
  }

  // Verifica se o movimento para a esquerda é válido
  if(left > -1 && left < state->number_link->cols)
  {
    coord new_coord = { left, row };
    update_neighbors(state->number_link, pair, new_coord, board_data, allocator, neighbors, state->matched_pairs);
  }

  // Verifica se o movimento para a direita é válido
  if(right > -1 && right < state->number_link->cols)
  {
    coord new_coord = { right, row };
    update_neighbors(state->number_link, pair, new_coord, board_data, allocator, neighbors, state->matched_pairs);
  }
}

void visit(state_t* current_state, state_allocator_t* allocator, linked_list_t* neighbors)
{
  number_link_state_t* state = (number_link_state_t*)current_state->data;
  board_data_t board_data = number_link_wrap_board(state->number_link, state->board_data);

  for(int pair = 0; pair < state->number_link->num_pairs; pair++)
  {
    do_moves(pair, board_data, state, allocator, neighbors);
  }
}

// Verifica se um estado é um objetivo do problema number link
bool goal(const state_t* state_a, const state_t*)
{
  number_link_state_t* state = (number_link_state_t*)state_a->data;
  number_link_t* number_link = state->number_link;
  return state->matched_pairs == number_link->num_pairs;
}

int distance(const state_t*, const state_t*)
{
  return 1;
}
