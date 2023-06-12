#include <stdlib.h>
#include <string.h>
#include "state.h"

// Esta é a funcão que é utilizada pela hashtable para comparar se 2
// estados são iguais
bool compare_state_t(const void* state_a, const void* state_b)
{
  size_t struct_size_a = ((state_t*)state_a)->struct_size;
  size_t struct_size_b = ((state_t*)state_b)->struct_size;
  
  // Os tamanhos tem de ser iguais
  if (struct_size_a != struct_size_b) 
    return false;

  // Compara os estados
  void* state_data_a = ((state_t*)state_a)->data;
  void* state_data_b = ((state_t*)state_b)->data;
  return memcmp(state_data_a,state_data_b,struct_size_a) == 0;
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
  allocator->state_allocator = allocator_create(sizeof(state_t));
  allocator->state_data_allocator = allocator_create(struct_size);

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
  hashtable_destroy(allocator->states, false);
  allocator_destroy(allocator->state_data_allocator);
  allocator_destroy(allocator->state_allocator);

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

  // Verifica se o estado expandido já existe
  state_t temp_state = {allocator->struct_size, state_data};
  state_t* state = (state_t*)hashtable_contains(allocator->states, &temp_state);
  
  // Caso seja um estado novo, alocamos utilizando os nossos gestores de memória 
  // e indexa o estado gerado
  if(!state)
  {
    // Primeiro alocamos o contentor dos dados
    state = (state_t*) allocator_alloc(allocator->state_allocator);
    
    // Alocamos memória para os dados do estado em si e copiamos os dados
    state->struct_size = allocator->struct_size;
    state->data = allocator_alloc(allocator->state_data_allocator);
    memcpy(state->data, state_data, allocator->struct_size);

    // Indexamos este estado
    hashtable_insert(allocator->states, state);
  }

  return state;
}
