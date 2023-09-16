// D import file generated from 'astar_common/src/astar.d'
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
extern (C)
{
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
		int generated;
		int expanded;
		timespec start_time;
		timespec end_time;
		double execution_time;
		size_t max_min_heap_size;
		int nodes_new;
		int nodes_reinserted;
		int paths_worst_or_equals;
		int paths_better;
		int num_solutions;
		int num_worst_solutions;
		int num_better_solutions;
		static a_star_t* create(size_t struct_size, goal_function goal_func, visit_function visit_func, heuristic_function h_func, distance_function d_func, print_function print_func);
		void destroy();
		void print_statistics(bool csv, bool show_solution);
		version (STATS_GEN)
		{
			void attach_search_data();
		}
	}
	a_star_t* a_star_create(size_t struct_size, goal_function goal_func, visit_function visit_func, heuristic_function h_func, distance_function d_func, print_function print_func);
	void a_star_destroy(a_star_t* a_star);
	void a_star_print_statistics(a_star_t* a_star, bool csv, bool show_solution);
	version (STATS_GEN)
	{
		void a_star_attach_search_data(a_star_t* a_star);
	}
}
