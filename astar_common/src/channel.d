module astar_common.channel;

import core.stdc.stdlib;
import core.stdc.string;
import core.sys.posix.pthread;

//import astar_common.queue;

extern (C):

enum int QUEUE_BUFFER_SIZE = 2048;

struct channel_t
{
  void** queues;
  size_t* queue_pos;
  size_t* queue_size;
  pthread_mutex_t* queue_lock;
  size_t struct_size;
  size_t num_queues;

  // channel_create
  static channel_t* create(size_t num_queues, size_t struct_size)
  {
    channel_t* channel = cast(channel_t*)malloc(channel_t.sizeof);
    if(!channel)
      return null ;

    channel.queues = cast(void**)malloc(num_queues * (void*).sizeof);
    if(!channel.queues)
    {
      free(channel);
      return null;
    }

    channel.queue_pos = cast(size_t*)malloc(num_queues * size_t.sizeof);
    if(!channel.queue_pos)
    {
      free(channel.queues);
      free(channel);
      return null;
    }

    channel.queue_size = cast(size_t*)malloc(num_queues * size_t.sizeof);
    if(!channel.queue_size)
    {
      free(channel.queue_pos);
      free(channel.queues);
      free(channel);
      return null ;
    }

    channel.queue_lock = cast(pthread_mutex_t*)malloc(num_queues * pthread_mutex_t.sizeof);
    if(!channel.queue_lock)
    {
      free(channel.queue_size);
      free(channel.queue_pos);
      free(channel.queues);
      free(channel);
      return null ;
    }

    foreach (size_t i; 0 .. num_queues)
    {
      channel.queues[i] = malloc(QUEUE_BUFFER_SIZE * struct_size);
      channel.queue_pos[i] = 0;
      channel.queue_size[i] = QUEUE_BUFFER_SIZE;
      pthread_mutex_init(&channel.queue_lock[i], null);
      if(!channel.queues[i])
      {
        foreach(size_t j; 0 .. i)
        {
          free(channel.queues[j]);
          pthread_mutex_destroy(&channel.queue_lock[j]);
        }
        free(channel.queue_lock);
        free(channel.queue_size);
        free(channel.queue_pos);
        free(channel.queues);
        free(channel);
        return null ;
      }
    }

    channel.num_queues = num_queues;
    channel.struct_size = struct_size;
    return channel;
  }

  // channel_send
  void send(size_t queue_index, void* data)
  {
    if(queue_index >= num_queues)
      return ;

    pthread_mutex_lock(&queue_lock[queue_index]);

    if(queues[queue_index])
    {
      if(queue_pos[queue_index] + 1 > queue_size[queue_index])
      {
        queue_size[queue_index] *= 2;
        queues[queue_index] =
          realloc(queues[queue_index], struct_size * queue_size[queue_index]);
      }

      memcpy(&queues[queue_index][queue_pos[queue_index] * struct_size], data, struct_size);
      queue_pos[queue_index] ++;
    }

    pthread_mutex_unlock(&queue_lock[queue_index]);
  }

  // channel_receive
  void* receive(size_t queue_index, size_t* len)
  {
    if(queue_index >= num_queues)
      return null ;

    pthread_mutex_lock(&queue_lock[queue_index]);

    *len = queue_pos[queue_index];
    if(queue_pos[queue_index] == 0)
    {
      pthread_mutex_unlock(&queue_lock[queue_index]);
      return null;
    }

    void* queue_data = malloc(struct_size * queue_pos[queue_index]);

    if(!queue_data)
    {
      pthread_mutex_unlock(&queue_lock[queue_index]);
      *len = 0;
      return null;
    }

    memcpy(queue_data, queues[queue_index], struct_size * queue_pos[queue_index]);
    queue_pos[queue_index] = 0;

    pthread_mutex_unlock(&queue_lock[queue_index]);

    return queue_data;
  }

  // channel_destroy
  void destroy()
  {
    foreach(size_t i; 0 .. num_queues)
    {
      pthread_mutex_lock(&queue_lock[i]);
      free(queues[i]);
      queue_pos[i] = queue_size[i] = 0;
      pthread_mutex_unlock(&queue_lock[i]);
      pthread_mutex_destroy(&queue_lock[i]);
    }
    free(queue_lock);
    free(queue_size);
    free(queue_pos);
    free(queues);
    free(&this);
  }

  // channel_has_messages
  bool has_messages(size_t queue_index)
  {
    if(queue_index > num_queues)
      return false ;

    return queue_pos[queue_index] > 0;
  }
}

/*
  C compatibility layer
*/

channel_t* channel_create(size_t num_queues, size_t struct_size)
{
  return channel_t.create(num_queues, struct_size);
}

void channel_send(channel_t* channel, size_t queue_index, void* data)
{
  channel.send(queue_index, data);
}

void* channel_receive(channel_t* channel, size_t queue_index, size_t *len)
{
  if(!channel)
    return null;

  return channel.receive(queue_index, len);
}

void channel_destroy(channel_t* channel)
{
  if(!channel)
    return ;

  channel.destroy();
}

bool channel_has_messages(channel_t* channel, size_t queue_index)
{
  if(!channel)
    return false;

  return channel.has_messages(queue_index);
}

