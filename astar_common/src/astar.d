module astar_common.astar;

public import astar_common.linked_list;
public import astar_common.node;
public import astar_common.state;

version (STATS_GEN)
{
  public import astar_common.search_data;
}

import core.stdc.stdio;
import core.stdc.stdlib;
import core.sys.posix.time;

extern (C):

alias ll = linked_list!(void*);

alias heuristic_function = int function(const state_t*, const state_t*);
alias visit_function = void function(state_t*, state_allocator_t*, ll.linked_list_t*);
alias goal_function = bool function(const state_t*, const state_t*);
alias distance_function = int function(const state_t*, const state_t*);

struct a_star_t
{
  state_allocator_t* state_allocator;
  node_allocator_t* node_allocator;

  goal_function goal_func;
  visit_function visit_func;
  heuristic_function h_func;
  distance_function d_func;

  a_star_node_t* solution;
  state_t* goal_state;

  // stats
  int generated;
  int expanded;
  timespec start_time, end_time;
  double execution_time;
  size_t max_min_heap_size;
  int nodes_new;
  int nodes_reinserted;
  int paths_worst_or_equals;
  int paths_better;
  int num_solutions;
  int num_worst_solutions;
  int num_better_solutions;

  // a_star_create
  static a_star_t* create(size_t struct_size,
                          goal_function goal_func,
                          visit_function visit_func,
                          heuristic_function h_func,
                          distance_function d_func,
                          print_function print_func)
  {
    a_star_t* a_star = cast(a_star_t*)malloc(a_star_t.sizeof);
    if(!a_star)
      return null;

    a_star.state_allocator = state_allocator_t.create(struct_size);
    if(!a_star.state_allocator)
    {
      a_star.destroy();
      return null;
    }

    a_star.node_allocator = node_allocator_t.create(print_func);
    if(!a_star.node_allocator)
    {
      a_star.destroy();
      return null;
    }

    a_star.visit_func = visit_func;
    a_star.goal_func = goal_func;
    a_star.h_func = h_func;
    a_star.d_func = d_func;

    a_star.solution = null;
    a_star.goal_state = null;

    a_star.generated = a_star.expanded = 0;
    a_star.max_min_heap_size = 0;
    a_star.nodes_new = 0;
    a_star.nodes_reinserted = 0;
    a_star.paths_better = a_star.paths_worst_or_equals = 0;
    a_star.num_solutions = a_star.num_worst_solutions = a_star.num_better_solutions = 0;

    return a_star;
  }

  // a_star_destroy
  void destroy()
  {
    if(state_allocator)
      state_allocator.destroy();

    if(node_allocator)
      node_allocator.destroy();
    free(&this);
  }

  // a_star_print_statistics
  void print_statistics(bool csv, bool show_solution)
  {  
    if(show_solution)
      if(solution)
        node_allocator.print_func(solution);

    if(!csv)
    {
      if(solution)
      {
        printf("Resultado do algoritmo: Solução encontrada, custo: %d\n", solution.g);
      }
      else
      {
        printf("Resultado do algoritmo: Solução não encontrada.\n");
      }
      printf("Estatísticas Globais:\n");
      printf("- Estados gerados: %d\n", generated);
      printf("- Estados expandidos: %d\n", expanded);
      printf("- Max nós min_heap: %ld\n", max_min_heap_size);
      printf("- Novos nós: %d\n", nodes_new);
      printf("- Nós reinseridos: %d\n", nodes_reinserted);
      printf("- Caminhos piores (ignorados): %d\n", paths_worst_or_equals);
      printf("- Caminhos melhores (atualizados): %d\n", paths_better);
      printf("- Soluções encontradas: %d\n", num_solutions);
      printf("- Soluções piores encontradas (não atualizadas): %d\n", num_worst_solutions);
      printf("- Soluções melhores encontradas (atualizadas): %d\n", num_better_solutions);
    }
    else
    {
      printf("\"%s\";%d;%d;%d;%ld;%d;%d;%d;%d;%d;%d;%d;%.6f\n",
            solution ? "sim".ptr : "não".ptr,
            solution ? solution.g : 0,
            generated,
            expanded,
            max_min_heap_size,
            nodes_new,
            nodes_reinserted,
            paths_worst_or_equals,
            paths_better,
            num_solutions,
            num_worst_solutions,
            num_better_solutions,
            execution_time);
    }
  }

  version(STATS_GEN)
  {
    void attach_search_data()
    {
      // not actually implemented ?
    }
  }
}

/*
  C compatibility
*/

a_star_t* a_star_create(size_t struct_size,
                        goal_function goal_func,
                        visit_function visit_func,
                        heuristic_function h_func,
                        distance_function d_func,
                        print_function print_func)
{
  return a_star_t.create(struct_size, goal_func, visit_func,
                         h_func, d_func, print_func);
}

void a_star_destroy(a_star_t* a_star)
{
  a_star.destroy();
}

void a_star_print_statistics(a_star_t* a_star, bool csv, bool show_solution)
{
  a_star.print_statistics(csv, show_solution);
}

version(STATS_GEN)
{
  void a_star_attach_search_data(a_star_t* a_star)
  {
    a_star.attach_search_data();
  }
}
