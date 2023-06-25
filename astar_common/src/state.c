#include "state.h"
#include <stdlib.h>
#include <string.h>

// Esta é a funcão que é utilizada pela hashtable para comparar se 2
// estados são iguais
bool compare_state_t(const void* state_a, const void* state_b)
{
  size_t struct_size_a = ((state_t*)state_a)->struct_size;
  size_t struct_size_b = ((state_t*)state_b)->struct_size;

  // Os tamanhos tem de ser iguais
  if(struct_size_a != struct_size_b)
    return false;

  // Compara os estados
  void* state_data_a = ((state_t*)state_a)->data;
  void* state_data_b = ((state_t*)state_b)->data;
  return memcmp(state_data_a, state_data_b, struct_size_a) == 0;
}

// Aloca um novo gestor de estados
state_allocator_t* state_allocator_create(size_t struct_size)
{
  state_allocator_t* allocator = (state_allocator_t*)malloc(sizeof(state_allocator_t));

  if(allocator == NULL)
  {
    return NULL; // Erro de alocação
  }

  // Configura o alocador
  allocator->struct_size = struct_size;

  // Nos utilizamos 2 alocadores diferents, um para os estados, outro
  // para os dados do estado (dependente do algoritmo)
  allocator->allocator = allocator_create(struct_size);

  // Para indexarmos os estados que já existem
  allocator->states = hashtable_create(struct_size, compare_state_t, NULL);

  return allocator;
}

// Liberta o gestor de estados e todos os estados gerados
void state_allocator_destroy(state_allocator_t* allocator)
{
  if(allocator == NULL)
  {
    return;
  }

  // Limpamos a nossa memória
  hashtable_destroy(allocator->states, true);
  allocator_destroy(allocator->allocator);

  // Libertamos o alocador
  free(allocator);
}

// Aloca ou retorna um estado novo
state_t* state_allocator_new(state_allocator_t* allocator, void* state_data)
{
  // Sem alocador não há dados
  if(allocator == NULL)
  {
    return NULL;
  }

  state_t* new_state = (state_t*)malloc(sizeof(state_t));
  if(new_state == NULL)
  {
    return NULL;
  }

  new_state->struct_size = allocator->struct_size;
  new_state->data = state_data;

  // Verifica se o estado expandido já existe
  state_t* old_state = (state_t*)hashtable_contains(allocator->states, new_state);
  if(old_state)
  {
    free(new_state);
    return (state_t*)old_state;
  }

  // Guardamos os dados no nosso gestor de memória e indexamos o estado gerado
  new_state->data = allocator_alloc(allocator->allocator);
  memcpy(new_state->data, state_data, allocator->struct_size);
  hashtable_insert(allocator->states, new_state);

  return new_state;
}
