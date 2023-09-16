// D import file generated from 'astar_common/src/node.d'
module astar_common.node;
public import astar_common.linked_list;
public import astar_common.state;
import core.stdc.stdlib;
import core.stdc.string;
extern (C)
{
	alias print_function = void function(a_star_node_t*);
	struct a_star_node_t
	{
		int g;
		int h;
		a_star_node_t* parent;
		state_t* state;
		size_t index_in_open_set;
		alias state this;
bool compare(a_star_node_t* other);
		size_t hash();
	}
	struct node_allocator_t
	{
		allocator_t* allocator;
		hashtable_t* nodes;
		print_function print_func;
		private static bool compare_nodes(const void* node_a, const void* node_b);
		private static size_t node_hash(hashtable_t* _unused, const void* data);
		static node_allocator_t* create(print_function print_func);
		void destroy();
		a_star_node_t* new_node(state_t* state);
		a_star_node_t* get(state_t* state);
	}
	node_allocator_t* node_allocator_create(print_function print_func);
	void node_allocator_destroy(node_allocator_t* alloc);
	a_star_node_t* node_allocator_new(node_allocator_t* alloc, state_t* state);
	a_star_node_t* node_allocator_get(node_allocator_t* alloc, state_t* state);
}
