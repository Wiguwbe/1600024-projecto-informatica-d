// D import file generated from 'astar_common/src/state.d'
module astar_common.state;
import astar_common.allocator;
import astar_common.hashtable;
import core.stdc.string;
import core.stdc.stdlib;
extern (C)
{
	struct state_t
	{
		size_t hash;
		size_t struct_size;
		void* data;
		bool compare(state_t* other);
	}
	struct state_allocator_t
	{
		size_t struct_size;
		allocator_t* allocator;
		hashtable_t* states;
		static bool compare_state_t(const void* state_a, const void* state_b);
		static state_allocator_t* create(size_t struct_size);
		void destroy();
		state_t* new_state(void* state_data);
	}
	state_allocator_t* state_allocator_create(size_t struct_size);
	void state_allocator_destroy(state_allocator_t* allocator);
	state_t* state_allocator_new(state_allocator_t* allocator, void* state_data);
}
