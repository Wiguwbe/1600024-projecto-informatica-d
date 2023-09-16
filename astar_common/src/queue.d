module astar_common.queue;

import core.stdc.stdlib;
import core.sys.posix.pthread;

extern (C):

template queue(T)
{
  struct queue_node_t
  {
    T data;
    queue_node_t* next;
  }

  struct queue_t
  {
    queue_node_t* head;
    queue_node_t* tail;
    size_t size;
    pthread_mutex_t lock;

    // queue_create
    static queue_t* create()
    {
      queue_t *queue = cast(queue_t*)malloc(queue_t.sizeof);
      if(!queue)
        return null;

      queue.head = queue.tail = null;
      queue.size = 0;
      pthread_mutex_init(&queue.lock, null);

      return queue;
    }

    // queue_push
    void push(T data)
    {
      queue_node_t* new_node = cast(queue_node_t*)malloc(queue_node_t.sizeof);
      if(!new_node)
        return ;

      new_node.data = data;
      new_node.next = null;

      pthread_mutex_lock(&lock);

      if(!tail)
        head = tail = new_node;
      else
      {
        // tail = tail.next = new_node ?
        tail.next = new_node;
        tail = new_node;
      }

      size ++;

      pthread_mutex_unlock(&lock);
    }

    // queue_pop
    T pop()
    {
      if(is_empty())
        return null;

      pthread_mutex_lock(&lock);

      queue_node_t* node = head;
      void* data = node.data;

      head = head.next;

      if(!head)
        tail = null;

      size--;

      pthread_mutex_unlock(&lock);

      free(node);
      return data;
    }

    // queue_is_empty
    bool is_empty()
    {
      return head == null;
    }

    void destroy()
    {
      // when T is not a pointer, it doesn't make sense to free it
      destroy(false);
    }

    // queue_destroy
    void destroy(bool free_data)
    {
      pthread_mutex_lock(&lock);

      queue_node_t* next;
      while(head)
      {
        next = head.next;
        if(free_data)
          free(head.data);
        free(head);
        head = next;
      }

      pthread_mutex_unlock(&lock);
      pthread_mutex_destroy(&lock);

      free(&this);
    }
  }
}

/*
  C compatibility layer
*/

alias q = queue!(void*);

q.queue_t* queue_create()
{
  return q.queue_t.create();
}

void queue_push(q.queue_t* queue, void* data)
{
  return queue.push(data);
}

void* queue_pop(q.queue_t* queue)
{
  return queue.pop();
}

bool queue_is_empty(q.queue_t* queue)
{
  return queue.is_empty();
}

size_t queue_size(q.queue_t* queue)
{
  return queue.size;
}

void queue_destroy(q.queue_t* queue, bool free_data)
{
  queue.destroy(free_data);
}
