#include <string.h>
#include "logic.h"
#include "linked_list.h"
#include "state.h"
#include <stdlib.h>

// Este é o objectivo do nosso problema
static const puzzle_state goal_puzzle = {{{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '-'}}};

// Implementa a heuristica do problema 8 puzzle
#include <math.h>
#include "state.h"

// Função de heurística para o puzzle 8
int heuristic(const state_t* current_state, const state_t*)
{
    // Converter os estados para puzzle_state
    puzzle_state* current_puzzle = (puzzle_state*)(current_state->data);

    int h = 0;

    // Calcular a distância de Manhattan para cada peça
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char current_piece = current_puzzle->board[i][j];
            if (current_piece != '-') {
                // Encontrar a posição da peça no estado objetivo
                int goal_row = 0;
                int goal_col = 0;
                for (int x = 0; x < 3; x++) {
                    for (int y = 0; y < 3; y++) {
                        if (goal_puzzle.board[x][y] == current_piece) {
                            goal_row = x;
                            goal_col = y;
                            break;
                        }
                    }
                }
                // Calcular a distância de Manhattan para a peça atual
                h += abs(i - goal_row) + abs(j - goal_col);
            }
        }
    }

    return h;
}

// Função para visitar um estado do puzzle 8, expandir vizinhos possíveis e armazená-los na lista ligada
void visit(state_t* current_state, state_allocator_t* allocator, linked_list_t* neighbors)
{
  // Obtenha o ponteiro para a estrutura puzzle_state
  puzzle_state* puzzle = (puzzle_state*)(current_state->data);

  // Lógica para expandir o estado e adicionar os vizinhos à lista ligada

  // Localizar a posição do espaço vazio no tabuleiro
  int empty_row = -1;
  int empty_col = -1;

  for(int i = 0; i < 3; i++)
  {
    for(int j = 0; j < 3; j++)
    {
      if(puzzle->board[i][j] == '-')
      {
        empty_row = i;
        empty_col = j;
        break;
      }
    }
  }

  // Gerar novos estados vizinhos movendo o espaço vazio

  // Movimento para cima do espaço vazio
  if(empty_row > 0)
  {
    puzzle_state new_puzzle;
    memcpy(&new_puzzle, puzzle, sizeof(puzzle_state));

    new_puzzle.board[empty_row][empty_col] = new_puzzle.board[empty_row - 1][empty_col];
    new_puzzle.board[empty_row - 1][empty_col] = '-';

    state_t* new_state = state_allocator_new(allocator, &new_puzzle);
    linked_list_append(neighbors, new_state);
  }

  // Movimento para baixo fo espaço vazio
  if(empty_row < 2)
  {
    puzzle_state new_puzzle;
    memcpy(&new_puzzle, puzzle, sizeof(puzzle_state));

    new_puzzle.board[empty_row][empty_col] = new_puzzle.board[empty_row + 1][empty_col];
    new_puzzle.board[empty_row + 1][empty_col] = '-';

    state_t* new_state = state_allocator_new(allocator, &new_puzzle);
    linked_list_append(neighbors, new_state);
  }

  // Movimento para a esquerda
  if(empty_col > 0)
  {
    puzzle_state new_puzzle;
    memcpy(&new_puzzle, puzzle, sizeof(puzzle_state));

    new_puzzle.board[empty_row][empty_col] = new_puzzle.board[empty_row][empty_col - 1];
    new_puzzle.board[empty_row][empty_col - 1] = '-';

    state_t* new_state = state_allocator_new(allocator, &new_puzzle);
    linked_list_append(neighbors, new_state);
  }

  // Movimento para a direita
  if(empty_col < 2)
  {
    puzzle_state new_puzzle;
    memcpy(&new_puzzle, puzzle, sizeof(puzzle_state));

    new_puzzle.board[empty_row][empty_col] = new_puzzle.board[empty_row][empty_col + 1];
    new_puzzle.board[empty_row][empty_col + 1] = '-';

    state_t* new_state = state_allocator_new(allocator, &new_puzzle);
    linked_list_append(neighbors, new_state);
  }
}

// Verifica se um estado é um objectivo do problema 8 puzzle
bool goal(const state_t* state_a, const state_t*)
{
  return memcmp(state_a->data,&goal_puzzle,sizeof(goal_puzzle)) == 0;
}

// Retorna a distância de um estado anterior para o proximo,
// no caso do 8 puzzle será sempre 1 visto que apenas se pode
// mover uma peça de cada vez para o espaço livre
int distance(const state_t*, const state_t*)
{
  return 1;
}
