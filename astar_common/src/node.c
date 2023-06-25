#include "node.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Função utilizada na hashtable nodes para comparar se 2 nós são iguais
// è necessário comparar apenas os state_t
static bool compare_a_star_nodes(const void* node_a, const void* node_b)
{
  size_t struct_size_a = ((a_star_node_t*)node_a)->state->struct_size;
  size_t struct_size_b = ((a_star_node_t*)node_b)->state->struct_size;

  // Os tamanhos tem de ser iguais
  if(struct_size_a != struct_size_b)
    return false;

  // Compara os estados
  void* state_a = ((a_star_node_t*)node_a)->state->data;
  void* state_b = ((a_star_node_t*)node_b)->state->data;
  return memcmp(state_a, state_b, struct_size_a) == 0;
}

// Função para calcular o índice do bucket na hashtable
// Tal como a comparação esta função tem de ser especifica para
// esta estrutura já que é necessário usar os dados do estado
// para gerar o índice do hash
static size_t a_star_nodes_hash(hashtable_t* hashtable, const void* data)
{
  a_star_node_t* node = (a_star_node_t*)data;
  return hash_function(node->state->data, node->state->struct_size, hashtable->capacity);
}

// Cria um gestor de nós
node_allocator_t* node_allocator_create(print_function print_func)  {

  node_allocator_t* alloc = (node_allocator_t *) malloc(sizeof(node_allocator_t));
  if ( alloc == NULL) {
    return NULL; // Erro de alocação
  }

  alloc->allocator = allocator_create(sizeof(a_star_node_t));
  if(alloc->allocator == NULL)
  {
    node_allocator_destroy(alloc);
    return NULL;
  }

  // Indexação para mantermos controlo dos nós que já foram gerados
  alloc->nodes = hashtable_create(sizeof(a_star_node_t), compare_a_star_nodes, a_star_nodes_hash);
  if(alloc->nodes == NULL)
  {
    node_allocator_destroy(alloc);
    return NULL;
  }

  alloc->print_func = print_func;

  return alloc;
}

// Destrói um gestor de nós
void node_allocator_destroy(node_allocator_t* alloc) {

  if (alloc == NULL) {
    return;
  }

  if (alloc->nodes != NULL) {
     hashtable_destroy(alloc->nodes, false);   
  }

  if (alloc->allocator != NULL) {
    allocator_destroy(alloc->allocator);
  }

  // Destruímos o nosso algoritmo
  free(alloc);
}

// Cria um novo nó para o estado
a_star_node_t* node_allocator_new(node_allocator_t* alloc, state_t* state) {
  if(alloc == NULL)
  {
    return NULL;
  }

  a_star_node_t* node = (a_star_node_t*)allocator_alloc(alloc->allocator);
  node->state = state;

  // Indexamos este nó
  hashtable_insert(alloc->nodes, node);

  // Limpamos a memória
  node->parent = NULL;
  node->g = 0;
  node->h = 0;
  node->index_in_open_set = SIZE_MAX;

  return node;
}

// Verifica se já existe uma nó para o estado
a_star_node_t* node_allocator_get(node_allocator_t* alloc, state_t* state) {
    a_star_node_t temp_node = { 0, 0, NULL, state, SIZE_MAX };
    return hashtable_contains(alloc->nodes, &temp_node);
}
