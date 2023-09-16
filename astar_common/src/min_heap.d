module astar_common.min_heap;

import core.stdc.stdlib;

extern (C):

enum int INITIAL_CAPACITY = 8196;

template min_heap(T)
{

  struct heap_node_t
  {
    int cost;
    T data;
  }

  struct min_heap_t
  {
    heap_node_t* data;
    size_t capacity;
    size_t size;

    // min_heap_create
    static min_heap_t* create()
    {
      min_heap_t* heap = cast(min_heap_t*)malloc(min_heap_t.sizeof);
      if(!heap)
        return null;

      heap.data = cast(heap_node_t*)malloc(INITIAL_CAPACITY * heap_node_t.sizeof);

      heap.capacity = INITIAL_CAPACITY;
      heap.size = 0;

      return heap;
    }

    // min_heap_destroy
    void destroy()
    {
      free(data);
      free(&this);
    }

    // min_heap_insert
    size_t insert(int cost, T data)
    {
      ensure_capacity();

      size_t index = size;

      this.data[index].cost = cost;
      this.data[index].data = data;

      size++;

      heapify_up(index);

      return index;
    }

    // min_heap_pop
    heap_node_t pop()
    {
      if(!size)
        return heap_node_t(0, cast(T)null);

      heap_node_t min_node = data[0];

      data[0] = data[size-1];
      size--;

      heapify_down(0);

      return min_node;
    }

    // min_heap_remove
    void remove(int cost, T data)
    {
      size_t index = size_t.max;

      foreach(size_t i; 0 .. size)
      {
        if(this.data[i].cost == cost && this.data[i].data == data)
        {
          index = i;
          break ;
        }
      }

      if(index == size_t.max)
        return ;

      this.data[index] = this.data[size-1];
      size--;

      heapify_up(index);
    }

    // min_heap_update
    void update(int old_cost, int new_cost, T data)
    {
      size_t index = size_t.max;
      foreach(size_t i; 0 .. size)
      {
        if(this.data[i].cost == old_cost && this.data[i].data == data)
        {
          index = i;
          break;
        }
      }

      if(index == size_t.max)
        return ;

      update_cost(cast(int)index, new_cost);
    }

    // min_heap_update_cost
    void update_cost(int index, int cost)
    {
      data[index].cost = cost;
      heapify_up(index);
    }

    // min_heap_clean
    void clean()
    {
      size = 0;
    }

  private:
    void ensure_capacity()
    {
      if(size == capacity)
      {
        // double
        capacity <<= 1;

        data = cast(heap_node_t*)realloc(data, capacity * heap_node_t.sizeof);
      }
    }

    void swap(heap_node_t* a, heap_node_t* b)
    {
      heap_node_t temp = *a;
      *a = *b;
      *b = temp;
    }

    void heapify_up(size_t index)
    {
      if(!index)
        return ;

      size_t parent_index = (index-1)/2;

      if(data[index].cost < data[parent_index].cost)
      {
        swap(&data[index], &data[parent_index]);
        heapify_up(parent_index);
      }
    }

    void heapify_down(size_t index)
    {
      size_t left_child_index = 2 * index + 1;
      size_t right_child_index= 2 * index + 2;
      size_t smallest = index;

      if(left_child_index < size && data[left_child_index].cost < data[smallest].cost)
        smallest = left_child_index;

      if(right_child_index < size && data[right_child_index].cost < data[smallest].cost)
        smallest = right_child_index;

      if(smallest != index)
      {
        swap(&data[index], &data[smallest]);
        heapify_down(smallest);
      }
    }
  }
}

/*
  C compatibility layer
*/

alias h = min_heap!(void*);

h.min_heap_t* min_heap_create()
{
  return h.min_heap_t.create();
}

void min_heap_destroy(h.min_heap_t* heap)
{
  if(!heap)
    return ;

  heap.destroy();
}

size_t min_heap_insert(h.min_heap_t* heap, int cost, void* data)
{
  if(!heap)
    return size_t.max;

  return heap.insert(cost, data);
}

h.heap_node_t min_heap_pop(h.min_heap_t* heap)
{
  if(!heap)
    return h.heap_node_t(0, null);

  return heap.pop();
}

void min_heap_remove(h.min_heap_t* heap, int cost, void* data)
{
  if(!heap)
    return ;

  heap.remove(cost, data);
}

void min_heap_update(h.min_heap_t* heap, int old_cost, int new_cost, void* data)
{
  if(!heap)
    return ;
  heap.update(old_cost, new_cost, data);
}

void min_heap_update_cost(h.min_heap_t* heap, int index, int cost)
{
  if(!heap)
    return ;

  heap.update_cost(index, cost);
}

void min_heap_clean(h.min_heap_t* heap)
{
  if(!heap)
    return ;

  heap.clean();
}


