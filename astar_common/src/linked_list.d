module astar_common.linked_list;

import core.stdc.stdlib;

extern (C):

/*
  new D-like code
*/

template linked_list(T)
{
  struct linked_list_node_t
  {
    linked_list_node_t* next;
    T data;
  }

  struct linked_list_t
  {
    linked_list_node_t* head;
    linked_list_node_t* tail;
    size_t size;

    // linked_list_destroy/clear
    void clear()
    {
      while(this.head)
      {
        linked_list_node_t* next = this.head.next;
        free(this.head);
        this.head = next;
        this.size--;
      }
    }

    // linked_list_append
    void append(T data)
    {
      linked_list_node_t* new_node = cast(linked_list_node_t*)malloc(linked_list_node_t.sizeof);
      new_node.data = data;
      new_node.next = null;

      if(!this.head)
        this.head = this.tail = new_node;
      else
      {
        this.tail.next = new_node;
        this.tail = new_node;
      }

      this.size++;
    }

    // linked_list_insert
    void insert(T data, size_t index)
    {
      if(index > this.size)
        return ;

      linked_list_node_t* new_node = cast(linked_list_node_t*)malloc(linked_list_node_t.sizeof);
      new_node.data = data;

      if(!this.head)
      {
        new_node.next = null;
        this.head = this.tail = new_node;
      }
      else if(index == 0)
      {
        new_node.next = this.head;
        this.head = new_node;
      }
      else
      {
        linked_list_node_t* prev_node = this.head;
        foreach (size_t i; 0 .. (index-1))
          prev_node = prev_node.next;

        new_node.next = prev_node.next;
        prev_node.next = new_node;
      }

      this.size++;
    }

    // linked_list_remove
    void remove(size_t index)
    {
      if(index >= this.size)
        return ;


      if(this.size == 1)
      {
        free(this.head);
        this.head = this.tail = null;
      }
      else if(index == 0)
      {
        linked_list_node_t* node_to_remove = this.head;
        this.head = this.head.next;
        free(node_to_remove);
      }
      else
      {
        linked_list_node_t* prev_node = this.head;
        foreach (size_t i; 0 .. (index-1))
          prev_node = prev_node.next;

        linked_list_node_t* node_to_remove = prev_node.next;
        prev_node.next = node_to_remove.next;

        if(index == (this.size -1))
          this.tail = prev_node;

        free(node_to_remove);
      }

      this.size--;
    }

    // linked_list_pop_back
    T pop_back()
    {
      if(!this.size)
        // this may go wrong
        return cast(T)null ;

      linked_list_node_t* node = this.head;
      this.head = node.next;
      this.size--;

      T data = node.data;
      free(node);
      return data;
    }

    // linked_list_get
    T get(size_t index)
    {
      if(index >= this.size)
        return cast(T)null ;

      linked_list_node_t* node = this.head;
      foreach(size_t i; 0 .. index)
        node = node.next;

      return node.data;
    }

    /* TODO operator overloading (get) */
  }

  // linked_list_create
  linked_list_t* create()
  {
    linked_list_t* list = cast(linked_list_t*)malloc(linked_list_t.sizeof);
    list.head = list.tail = null;
    list.size = 0;
    return list;
  }
}

/*
  C compatibility layer

  it uses the void* as template type
*/

alias ll = linked_list!(void*);

ll.linked_list_t* linked_list_create()
{
  return ll.create();
}

void linked_list_destroy(ll.linked_list_t *list)
{
  list.clear();
  free(list);
}

void linked_list_append(ll.linked_list_t *list, void* data)
{
  list.append(data);
}

void linked_list_insert(ll.linked_list_t* list, void* data, size_t index)
{
  list.insert(data, index);
}

void linked_list_remove(ll.linked_list_t* list, size_t index)
{
  list.remove(index);
}

void* linked_list_pop_back(ll.linked_list_t* list)
{
  return list.pop_back();
}

size_t linked_list_size(ll.linked_list_t* list)
{
  return list.size;
}

void* linked_list_get(ll.linked_list_t* list, size_t index)
{
  return list.get(index);
}
