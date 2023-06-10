#include "astar.h"
#include <string.h>
#include <stdlib.h>

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

// Funções internas do algoritmo
a_star_t* a_star_create(
    size_t struct_size, goal_function goal_func, visit_function visit_func, heuristic_function h_func, distance_function d_func)
{
  a_star_t* a_star = (a_star_t*)malloc(sizeof(a_star_t));
  if(a_star == NULL)
  {
    return NULL; // Erro de alocação
  }

  // Inicializa os nossos alocadores de memória
  a_star->state_allocator = state_allocator_create(struct_size);
  if(a_star->state_allocator == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  a_star->node_allocator = allocator_create(sizeof(a_star_node_t));
  if(a_star->node_allocator == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  // Indexação para mantermos controlo dos nós que já foram expandidos
  a_star->nodes = hashtable_create(sizeof(a_star_node_t), compare_a_star_nodes, a_star_nodes_hash);
  if(a_star->nodes == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  // Inicializa as funções necessárias para o algoritmo funcionar
  a_star->visit_func = visit_func;
  a_star->goal_func = goal_func;
  a_star->h_func = h_func;
  a_star->d_func = d_func;

  // Reinicia as estatísticas
  a_star->expanded = 0;
  a_star->visited = 0;

  return a_star;
}

void a_star_destroy(a_star_t* a_star)
{

  if(a_star == NULL)
  {
    return;
  }

  if(a_star->nodes != NULL)
  {
    hashtable_destroy(a_star->nodes, false);
  }

  if(a_star->state_allocator != NULL)
  {
    state_allocator_destroy(a_star->state_allocator);
  }

  if(a_star->node_allocator != NULL)
  {
    allocator_destroy(a_star->node_allocator);
  }

  // Destruímos o nosso algoritmo
  free(a_star);
}

// Função interna para criar um nó a colocar na fila prioritária
a_star_node_t* a_star_new_node(a_star_t* a_star, state_t* state)
{
  if(a_star == NULL)
  {
    return NULL;
  }

  a_star_node_t* node = (a_star_node_t*)allocator_alloc(a_star->node_allocator);
  node->parent = NULL;
  node->g = 0;
  node->h = 0;
  node->state = state;

  // Indexamos este nó
  hashtable_insert(a_star->nodes, node);

  return node;
}
