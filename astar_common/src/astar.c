#include "astar.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funções internas do algoritmo
a_star_t* a_star_create(size_t struct_size,
                        goal_function goal_func,
                        visit_function visit_func,
                        heuristic_function h_func,
                        distance_function d_func,
                        print_function print_func)
{
  a_star_t* a_star = (a_star_t*)malloc(sizeof(a_star_t));
  if(a_star == NULL)
  {
    return NULL; // Erro de alocação
  }

  // Garante que a memória fique limpa
  a_star->state_allocator = NULL;
  a_star->node_allocator = NULL;

  // Inicializa os nossos gestores de nós e estados
  a_star->state_allocator = state_allocator_create(struct_size);
  if(a_star->state_allocator == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  a_star->node_allocator = node_allocator_create(print_func);
  if(a_star->node_allocator == NULL)
  {
    a_star_destroy(a_star);
    return NULL;
  }

  // Inicializa as funções necessárias para o algoritmo funcionar
  a_star->visit_func = visit_func;
  a_star->goal_func = goal_func;
  a_star->h_func = h_func;
  a_star->d_func = d_func;

  // Limpa solução e estado a atingir
  a_star->solution = NULL;
  a_star->goal_state = NULL;

  // Reinicia as estatísticas
  a_star->generated = 0;
  a_star->expanded = 0;
  a_star->max_min_heap_size = 0;
  a_star->nodes_new = 0;
  a_star->nodes_reinserted = 0;
  a_star->paths_better = 0;
  a_star->paths_worst_or_equals = 0;

  return a_star;
}

void a_star_destroy(a_star_t* a_star)
{

  if(a_star == NULL)
  {
    return;
  }

  if(a_star->state_allocator != NULL)
  {
    state_allocator_destroy(a_star->state_allocator);
  }

  if(a_star->node_allocator != NULL)
  {
    node_allocator_destroy(a_star->node_allocator);
  }

  // Destruímos o nosso algoritmo
  free(a_star);
}

// Imprime estatísticas do algoritmo sequencial no formato desejado
void a_star_print_statistics(a_star_t* a_star, bool csv)
{
  if(a_star == NULL)
  {
    return;
  }

  if(!csv)
  {
    if(a_star->solution)
    {
      printf("Resultado do algoritmo: Solução encontrada, custo: %d\n", a_star->solution->g);
      a_star->node_allocator->print_func(a_star->solution);
    }
    else
    {
      printf("Resultado do algoritmo: Solução não encontrada.\n");
    }

    printf("Estatísticas Globais:\n");
    printf("- Estados gerados: %d\n", a_star->generated);
    printf("- Estados expandidos: %d\n", a_star->expanded);
    printf("- Max nós min_heap: %ld\n", a_star->max_min_heap_size);
    printf("- Novos nós: %d\n", a_star->nodes_new);
    printf("- Nós reinseridos: %d\n", a_star->nodes_reinserted);
    printf("- Caminhos piores (ignorados): %d\n", a_star->paths_worst_or_equals);
    printf("- Caminhos melhores (atualizados): %d\n\n", a_star->paths_better);
  }
  else
  {
    printf("\"%s\";%d;%d;%d;%ld;%d;%d;%d;%d;%.6f\n",
           a_star->solution ? "sim" : "não",
           a_star->solution ? a_star->solution->g : 0,
           a_star->generated,
           a_star->expanded,
           a_star->max_min_heap_size,
           a_star->nodes_new,
           a_star->nodes_reinserted,
           a_star->paths_worst_or_equals,
           a_star->paths_better,
           a_star->execution_time);
  }
}