#ifndef STATE_H
#define STATE_H
#include "allocator.h"
#include "hashtable.h"

// Estrutura que define um estado para o algoritmo A* (data contêm user-defined data)
typedef struct state_t state_t;
struct state_t
{
  size_t struct_size;
  void* data;
};

typedef struct
{
  size_t struct_size;
  allocator_t* state_allocator;
  allocator_t* state_data_allocator;
  hashtable_t* states;
} state_allocator_t;

// Inicializa o gestor de the estados
state_allocator_t* state_allocator_create(size_t struct_size);

// Liberta o gestor de estados e todos os estados gerados
void state_allocator_destroy(state_allocator_t* allocator);

state_t* state_allocator_new(state_allocator_t* allocator, void* state_data);

#endif