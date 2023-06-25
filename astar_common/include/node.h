#ifndef NODE_H
#define NODE_H
#include "linked_list.h"
#include "state.h"
#include <time.h>

// Informação comum partilhada por ambas as versões do algoritmo A*
typedef struct node_allocator_t node_allocator_t;

// Um nó do nosso algoritmo
typedef struct a_star_node_t a_star_node_t;

// Tipo para funções que expandem um estado nos estados filho, utilizando o alocador predefinido, atualiza a min_heap e a hash_table
typedef void (*print_function)(a_star_node_t*);

// Estrutura que define um estado para o algoritmo A* (data contêm user-defined data)
struct a_star_node_t
{
  int g;
  int h;
  a_star_node_t* parent;
  state_t* state;
  size_t index_in_open_set;
};

// Estrutura que contem o estado do algoritmo A*
struct node_allocator_t
{
  allocator_t* allocator;
  hashtable_t* nodes;
  print_function print_func;
};

// Cria um gestor de nós
node_allocator_t* node_allocator_create(print_function print_func);

// Destrói um gestor de nós
void node_allocator_destroy(node_allocator_t* alloc);

// Cria um novo nó para o estado
a_star_node_t* node_allocator_new(node_allocator_t* alloc, state_t* state);

// Verifica se já existe uma nó para o estado
a_star_node_t* node_allocator_get(node_allocator_t* alloc, state_t* state);

#endif