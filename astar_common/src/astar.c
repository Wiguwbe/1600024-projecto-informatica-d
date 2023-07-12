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
#ifdef STATS_GEN
  a_star->search_data = NULL;
#endif

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
  a_star->num_solutions = 0;
  a_star->num_worst_solutions = 0;
  a_star->num_better_solutions = 0;

  return a_star;
}

void a_star_destroy(a_star_t* a_star)
{
  if(a_star == NULL)
  {
    return;
  }

  // Limpamos a nossas estruturas
  state_allocator_destroy(a_star->state_allocator);
  node_allocator_destroy(a_star->node_allocator);
  // Destruímos o nosso algoritmo
  free(a_star);
}

// Imprime estatísticas do algoritmo sequencial no formato desejado
void a_star_print_statistics(a_star_t* a_star, bool csv, bool show_solution)
{
  if(a_star == NULL)
  {
    return;
  }

  if(show_solution)
  {
    if(a_star->solution)
    {
      a_star->node_allocator->print_func(a_star->solution);
    }
  }

  if(!csv)
  {
    if(a_star->solution)
    {
      printf("Resultado do algoritmo: Solução encontrada, custo: %d\n", a_star->solution->g);
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
    printf("- Caminhos melhores (atualizados): %d\n", a_star->paths_better);
    printf("- Soluções encontradas: %d\n", a_star->num_solutions);
    printf("- Soluções piores encontradas (não atualizadas): %d\n", a_star->num_worst_solutions);
    printf("- Soluções melhores encontradas (atualizadas): %d\n", a_star->num_better_solutions);
  }
  else
  {
    printf("\"%s\";%d;%d;%d;%ld;%d;%d;%d;%d;%d;%d;%d;%.6f\n",
           a_star->solution ? "sim" : "não",
           a_star->solution ? a_star->solution->g : 0,
           a_star->generated,
           a_star->expanded,
           a_star->max_min_heap_size,
           a_star->nodes_new,
           a_star->nodes_reinserted,
           a_star->paths_worst_or_equals,
           a_star->paths_better,
           a_star->num_solutions,
           a_star->num_worst_solutions,
           a_star->num_better_solutions,
           a_star->execution_time);
  }
}

#ifdef STATS_GEN
void a_star_attach_search_data(a_star_t* a_star) {

  if (a_star == NULL) {
    return;
  }

  a_star->search_data = search_data_attach();
}
#endif
