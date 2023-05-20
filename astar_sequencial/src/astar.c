#include <stdlib.h>
#include "astar.h"

// Cria uma nova instância para resolver um problema
a_star_t* a_star_create(allocator_t* allocator,
                      size_t struct_size,
                      void* goal,
                      goal_function goal_func,
                      visit_function visit_func,
                      heuristic_function h_func,
                      distance_function d_func)
{
  a_star_t* a_star = (a_star_t*)malloc(sizeof(a_star_t));
  if(a_star == NULL)
  {
    return NULL; // Erro de alocação
  }

  a_star->allocator = allocator;
  a_star->visit_func = visit_func;
  a_star->goal_func = goal_func;
  a_star->h_func = h_func;
  a_star->d_func = d_func;

  // Conjunto com os nós por expandir e que também
  // possam necessitar de ser expandidos
  // novamente. Geralmente é implementada como min-heap
  // ou uma queue queue prioritária.
  a_star->open_set = min_heap_create();

  // Para mantermos controlo dos nós que já foram expandidos
  a_star->visited_set = hashtable_create(struct_size);

  return a_star;
}

// Liberta uma instância do algoritmo A*
void a_star_destroy(a_star_t* a_star) { 

  if(a_star == NULL)
  {
    return; 
  }

  min_heap_destroy( a_star->open_set);
  hashtable_destroy(a_star->visited_set);
}

// Resolve o problema através do uso do algoritmo A*;
path_t* a_star_solve(a_star_t* a_star, state_t* initial, void* goal) { 

  if(a_star == NULL)
  {
    return NULL; 
  }

  // O open_set deve estar vazio, ou caso contrário o algoritmo não funciona bem 
  if(a_star->open_set->size > 0) {
    return NULL;
  }

  // Atribui ao nó inicial um custo total de 0
  initial->g = 0;
  initial->h = 0;

  // Inserimos o nó inicial na nossa fila prioritário
  min_heap_insert(a_star->open_set, 0, initial);

  while(a_star->open_set->size)
  {
    // A seguinte operação pode ocorrer em O(log(N))
    // se nosAbertos é um min-heap ou uma queue prioritária
    heap_node_t current_node = min_heap_pop(a_star->open_set);
    state_t* current = (state_t*)current_node.data;

    // Se encontramos o objectivo saimos e retornamos o caminho para o nó
    if(a_star->goal_func(current, goal))
    {
      return a_star_path_to(a_star, current);
    }

    // Esta lista irá receber os vizinhos deste nó
    linked_list_t* neighbors = linked_list_create();

    // Executa a função que visita os vizinhos deste nó
    a_star->visit_func(current, a_star->allocator, a_star->visited_set, neighbors);

    // Itera por todos os vizinhos
    for (size_t i=0; i < linked_list_size(neighbors); i++ ) {

      state_t* neighbor = linked_list_get(neighbors,i); 

      int g_attempt = current->g + a_star->d_func(current,neighbor);

      if (g_attempt >= neighbor->g)
        continue;

      neighbor->parent = current;
      neighbor->g = g_attempt;
      neighbor->h = a_star->h_func(neighbor, goal);

      // TODO: Atualizar a min-heap
    }

    // Liberta a lista de vizinhos
    linked_list_destroy(neighbors);
  }

  return NULL;
}

// Retorna o caminho até o estado
path_t* a_star_path_to(a_star_t* a_star, state_t* state)
{
  return NULL;
}
