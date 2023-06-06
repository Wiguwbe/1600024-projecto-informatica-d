#ifndef LOGIC_H
#define LOGIC_H
#include "astar_parallel.h"
#include "state.h"

// Estrutura do que contem o estado do nosso puzzle 8
typedef struct 
{
    char board[3][3];
} puzzle_state;


// Implementa a heurística do problema 8 puzzle
int heuristic(const state_t*, const state_t*);

// Encontra os vizinhos de um estado no problema 8 puzzle
void visit(state_t*, state_allocator_t*, linked_list_t*);

// Verifica se um estado é um objetivo do problema 8 puzzle
bool goal(const state_t*, const state_t*);

// Retorna a distância de um estado anterior para o proximo,
// no caso do 8 puzzle será sempre 1 visto que apenas se pode
// mover uma peça de cada vez para o espaço livre
int distance(const state_t*, const state_t*);

#endif