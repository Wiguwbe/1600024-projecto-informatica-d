/******************************************************************************
 * Gestor de estados para o Algoritmo A*
 
   Funcionalidades:
   - Criar um gestor de estados
   - Destruir um gestor de estado
   - Alocar um novo estado caso os dados seja novos, ou retornar um estado existente 

   Utilização:
   1. Inclua o arquivo de cabeçalho "state.h" em seu código.
   2. Crie um nove gestor pela função state_allocator_create(), especificando o tamanho da struct com os dados
   3. Aloque novos estados ou obtenha acesso estados existentes com a função state_allocator_new().
   5. Liberte a memória utilizada pelo alocador com a função state_allocator_destroy().

   Limitações e Considerações:
   - Esta implementação não lida com situações de concorrência.
   - Esta estrutura foi desenvolvida como parte de um projeto universitário com o objetivo de
     fornecer uma implementação simples e didática, mas pode não ser adequada para todos os casos
     de uso ou requisitos de desempenho.
*/

#ifndef STATE_H
#define STATE_H

#include "allocator.h"
#include "hashtable.h"

/*
 * Estrutura que define um estado para o algoritmo A*.
 * O membro 'data' pode conter dados definidos pelo usuário.
 */
typedef struct state_t state_t;
struct state_t
{
  size_t hash;
  size_t struct_size;
  void* data;
};

/*
 * Estrutura que representa um gestor de estados.
 * O gestor mantém um conjunto de estados e 2 alocadores de memória
 */
typedef struct
{
  size_t struct_size;
  allocator_t* allocator;
  hashtable_t* states;
} state_allocator_t;

// Cria e inicializa um novo gestor de estados.
state_allocator_t* state_allocator_create(size_t struct_size);

// Liberta um gestor de estado (incluindo a memória)
void state_allocator_destroy(state_allocator_t* allocator);

// Aloca ou retorna um estado novo
state_t* state_allocator_new(state_allocator_t* allocator, void* state_data);

#endif
