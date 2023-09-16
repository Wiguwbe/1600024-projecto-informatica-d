module astar_common.node;

public import astar_common.linked_list;
public import astar_common.state;

import core.stdc.stdlib;
import core.stdc.string;

extern (C):

alias print_function = void function(a_star_node_t*);

struct a_star_node_t
{
  int g;
  int h;
  a_star_node_t* parent;
  state_t *state;
  size_t index_in_open_set;

  alias state this;  // unknown properties are looked up in `state`

  bool compare(a_star_node_t* other)
  {
    if(this.struct_size != other.struct_size)
      return false;

    return memcmp(this.data, other.data, this.struct_size) == 0;
  }

  size_t hash()
  {
    return state.hash;
  }
}

struct node_allocator_t
{
  allocator_t* allocator;
  hashtable_t* nodes;
  print_function print_func;

  private static bool compare_nodes(const void* node_a, const void* node_b)
  {
    return (cast(a_star_node_t*)node_a).compare(cast(a_star_node_t*)node_b);
  }

  private static size_t node_hash(hashtable_t* _unused, const void* data)
  {
    return (cast(a_star_node_t*)data).hash();
  }

  // node_allocator_create
  static node_allocator_t* create(print_function print_func)
  {
    node_allocator_t* alloc = cast(node_allocator_t*)malloc(node_allocator_t.sizeof);
    if(!alloc)
      return null;

    alloc.allocator = allocator_t.create(a_star_node_t.sizeof);
    if(!alloc.allocator)
    {
      alloc.destroy();
      return null;
    }

    alloc.nodes = hashtable_t.create(a_star_node_t.sizeof, &compare_nodes, &node_hash);
    if(!alloc.nodes)
    {
      alloc.destroy();
      return null;
    }

    alloc.print_func = print_func;

    return alloc;
  }

  // node_allocator_destroy
  void destroy()
  {
    if(nodes)
      nodes.destroy(false);

    if(allocator)
      allocator.destroy();


    free(&this);
  }

  a_star_node_t* new_node(state_t* state)
  {
    a_star_node_t* node = cast(a_star_node_t*)allocator.alloc();
    node.state = state;

    nodes.insert(node);

    node.parent = null;
    node.g = node.h = 0;
    node.index_in_open_set = size_t.max;

    return node;
  }

  a_star_node_t* get(state_t* state)
  {
    a_star_node_t temp_node = a_star_node_t(0, 0, null, state, size_t.max);
    return cast(a_star_node_t*)nodes.contains(&temp_node);
  }
}

/*
  C compatability
*/

node_allocator_t* node_allocator_create(print_function print_func)
{
  return node_allocator_t.create(print_func);
}

void node_allocator_destroy(node_allocator_t* alloc)
{
  if(!alloc)
    return;
  alloc.destroy();
}

a_star_node_t* node_allocator_new(node_allocator_t* alloc, state_t* state)
{
  if(!alloc)
    return null;

  return alloc.new_node(state);
}

a_star_node_t* node_allocator_get(node_allocator_t* alloc, state_t* state)
{
  return alloc.get(state);
}
