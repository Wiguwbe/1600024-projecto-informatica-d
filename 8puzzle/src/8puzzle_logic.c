#include "8puzzle_logic.h"
#include "linked_list.h"
#include "state.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Este é o objetivo do nosso problema
static const puzzle_state goal_puzzle = { { { '1', '2', '3' }, { '4', '5', '6' }, { '7', '8', '-' } } };

// Estrutura para ajudar no cálculo da heurística
static const int heuristic_table[8][2] = { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 1, 0 }, { 1, 1 }, { 1, 2 }, { 2, 0 }, { 2, 1 } };

// Função de heurística para o puzzle 8
int heuristic(const state_t* current_state, const state_t*)
{
  // Converter os estados para puzzle_state
  puzzle_state* current_puzzle = (puzzle_state*)(current_state->data);

  int h = 0;

  // Calcular a distância de Manhattan para cada peça
  for(int x = 0; x < 3; x++)
  {
    for(int y = 0; y < 3; y++)
    {
      char current_piece = current_puzzle->board[y][x];

      if(current_piece == '-')
      {
        continue;
      }

      // Converte os caracteres números 1,2,3,4,5,6,7 e 8 no seu valor numérico
      int num = (current_piece - 49);
      int goal_row = heuristic_table[num][0];
      int goal_col = heuristic_table[num][1];

      // Calcular a distância de Manhattan para a peça atual
      h += abs(x - goal_col) + abs(y - goal_row);
    }
  }

  return h;
}

// Função para visitar um estado do puzzle 8, expandir vizinhos possíveis e armazená-los na lista ligada
void visit(state_t* current_state, state_allocator_t* allocator, linked_list_t* neighbors)
{
  // Obtenha o ponteiro para a estrutura puzzle_state
  puzzle_state* puzzle = (puzzle_state*)(current_state->data);
  puzzle_state new_puzzle_up, new_puzzle_down, new_puzzle_left, new_puzzle_right;

  // Copia o tabuleiro para os novos possíveis estados
  memcpy(&new_puzzle_up, puzzle, sizeof(puzzle_state));
  memcpy(&new_puzzle_down, puzzle, sizeof(puzzle_state));
  memcpy(&new_puzzle_left, puzzle, sizeof(puzzle_state));
  memcpy(&new_puzzle_right, puzzle, sizeof(puzzle_state));

  // Lógica para expandir o estado e adicionar os vizinhos à lista ligada

  // Localizar a posição do espaço vazio no tabuleiro
  int empty_row = 0;
  int empty_col = 0;

  for(int y = 0; y < 3; y++)
  {
    for(int x = 0; x < 3; x++)
    {
      if(puzzle->board[y][x] == '-')
      {
        empty_row = y;
        empty_col = x;
        break;
      }
    }
  }

  // Gerar novos estados vizinhos movendo o espaço vazio

  // Movimento para cima do espaço vazio
  if(empty_row > 0)
  {
    new_puzzle_up.board[empty_row][empty_col] = new_puzzle_up.board[empty_row - 1][empty_col];
    new_puzzle_up.board[empty_row - 1][empty_col] = '-';
    linked_list_append(neighbors, state_allocator_new(allocator, &new_puzzle_up));
  }

  // Movimento para baixo do espaço vazio
  if(empty_row < 2)
  {
    new_puzzle_down.board[empty_row][empty_col] = new_puzzle_down.board[empty_row + 1][empty_col];
    new_puzzle_down.board[empty_row + 1][empty_col] = '-';
    linked_list_append(neighbors, state_allocator_new(allocator, &new_puzzle_down));
  }

  // Movimento para a esquerda
  if(empty_col > 0)
  {
    new_puzzle_left.board[empty_row][empty_col] = new_puzzle_left.board[empty_row][empty_col - 1];
    new_puzzle_left.board[empty_row][empty_col - 1] = '-';
    linked_list_append(neighbors, state_allocator_new(allocator, &new_puzzle_left));
  }

  // Movimento para a direita
  if(empty_col < 2)
  {
    new_puzzle_right.board[empty_row][empty_col] = new_puzzle_right.board[empty_row][empty_col + 1];
    new_puzzle_right.board[empty_row][empty_col + 1] = '-';
    linked_list_append(neighbors, state_allocator_new(allocator, &new_puzzle_right));
  }
}

// Verifica se um estado é um objectivo do problema 8 puzzle
bool goal(const state_t* state_a, const state_t*)
{
  return memcmp(state_a->data, &goal_puzzle, sizeof(puzzle_state)) == 0;
}

// Retorna a distância de um estado anterior para o proximo,
// no caso do 8 puzzle será sempre 1 visto que apenas se pode
// mover uma peça de cada vez para o espaço livre
int distance(const state_t*, const state_t*)
{
  return 1;
}
