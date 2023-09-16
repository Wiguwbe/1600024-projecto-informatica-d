module astar_common.state;

public import astar_common.allocator;
public import astar_common.hashtable;

import core.stdc.string;
import core.stdc.stdlib;

extern (C):

struct state_t
{
  size_t hash;
  size_t struct_size;
  // TODO template state_t and state_allocator_t on this `void*`
  void* data;

  bool compare(state_t* other)
  {
    size_t struct_size_a = this.struct_size;
    size_t struct_size_b = other.struct_size;

    if(struct_size_a != struct_size_b)
      return false;

    return memcmp(this.data, other.data, struct_size_a) == 0;
  }
}

struct state_allocator_t
{
  size_t struct_size;
  allocator_t* allocator;
  hashtable_t* states;

  static bool compare_state_t(const void* state_a, const void* state_b)
  {
    return (cast(state_t*)state_a).compare(cast(state_t*)state_b);
  }

  // state_allocator_create
  static state_allocator_t* create(size_t struct_size)
  {
    state_allocator_t* allocator = cast(state_allocator_t*)malloc(state_allocator_t.sizeof);
    if(!allocator)
      return null;

    allocator.struct_size = struct_size;

    allocator.allocator = allocator_t.create(struct_size);

    allocator.states = hashtable_t.create(struct_size, &state_allocator_t.compare_state_t, null);
    
    return allocator;
  }

  // state_allocator_destroy
  void destroy()
  {
    states.destroy(true);
    allocator.destroy();

    free(&this);
  }

  // state_allocator_new
  state_t* new_state(void* state_data)
  {
    state_t* new_state = cast(state_t*)malloc(state_t.sizeof);
    if(!new_state)
      return null;

    new_state.struct_size = struct_size;
    new_state.hash = hash_function(state_data, struct_size, HASH_CAPACITY);
    new_state.data = state_data;

    state_t* old_state = cast(state_t*)states.contains(new_state);
    if(old_state)
    {
      free(new_state);
      return old_state;
    }

    new_state.data = allocator.alloc();
    memcpy(new_state.data, state_data, struct_size);
    states.insert(new_state);

    return new_state;
  }
}

/*
  C compatibility functions
*/

state_allocator_t* state_allocator_create(size_t struct_size)
{
  return state_allocator_t.create(struct_size);
}

void state_allocator_destroy(state_allocator_t* allocator)
{
  allocator.destroy();
}

state_t* state_allocator_new(state_allocator_t* allocator, void* state_data)
{
  return allocator.new_state(state_data);
}
