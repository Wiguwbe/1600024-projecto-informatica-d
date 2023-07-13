#include "channel.h"
#include <stdlib.h>
#include <string.h>

// Inicializa o canal com o número especificado de filas
channel_t* channel_create(size_t num_queues, size_t struct_size)
{
  channel_t* channel = (channel_t*)malloc(sizeof(channel_t));
  if(channel == NULL)
  {
    return NULL;
  }

  // Aloca espaço para o array de filas
  channel->queues = malloc(num_queues * sizeof(void*));
  if(channel->queues == NULL)
  {
    free(channel);
    return NULL;
  }

  channel->queue_pos = (size_t*)malloc(num_queues * sizeof(size_t));
  if(channel->queue_pos == NULL)
  {
    free(channel->queues);
    free(channel);
    return NULL;
  }

  channel->queue_size = (size_t*)malloc(num_queues * sizeof(size_t));
  if(channel->queue_size == NULL)
  {
    free(channel->queue_size);
    free(channel->queues);
    free(channel);
    return NULL;
  }

  channel->queue_lock = (pthread_mutex_t*)malloc(num_queues * sizeof(pthread_mutex_t));
  if(channel->queue_lock == NULL)
  {
    free(channel->queue_lock);
    free(channel->queue_size);
    free(channel->queues);
    free(channel);
    return NULL;
  }

  // // Inicializa cada fila
  for(size_t i = 0; i < num_queues; i++)
  {
    channel->queues[i] = malloc(QUEUE_BUFFER_SIZE * struct_size);
    channel->queue_pos[i] = 0;
    channel->queue_size[i] = QUEUE_BUFFER_SIZE;
    pthread_mutex_init(&channel->queue_lock[i], NULL);
    if(channel->queues[i] == NULL)
    {
      // Em caso de falha, destrói as filas já criadas e liberta a memória alocada
      for(size_t j = 0; j < i; j++)
      {
        free(channel->queues[j]);
        pthread_mutex_destroy(&(channel->queue_lock[j]));
      }
      free(channel->queue_lock);
      free(channel->queue_size);
      free(channel->queue_pos);
      free(channel->queues);
      free(channel);
      return NULL;
    }
  }

  channel->num_queues = num_queues;
  channel->struct_size = struct_size;
  return channel;
}

// Envia uma mensagem para uma fila específica no canal
void channel_send(channel_t* channel, size_t queue_index, void* data)
{
  // Verifica se o índice da fila é válido
  if(queue_index >= channel->num_queues)
  {
    return; // Índice inválido
  }

  pthread_mutex_lock(&(channel->queue_lock[queue_index]));

  if(channel->queues[queue_index])
  {
    if(channel->queue_pos[queue_index] + 1 > channel->queue_size[queue_index])
    {
      channel->queue_size[queue_index] *= 2;
      channel->queues[queue_index] =
          realloc(channel->queues[queue_index], channel->struct_size * channel->queue_size[queue_index]);
    }

    memcpy(&(channel->queues[queue_index][channel->queue_pos[queue_index] * channel->struct_size]), data, channel->struct_size);
    channel->queue_pos[queue_index]++;
  }

  pthread_mutex_unlock(&(channel->queue_lock[queue_index]));
}

void* channel_receive(channel_t* channel, size_t queue_index, size_t* len)
{
  if(channel == NULL)
  {
    return NULL;
  }

  // Verifica se o índice da fila é válido
  if(queue_index >= channel->num_queues)
  {
    return NULL; // Índice inválido
  }

  pthread_mutex_lock(&(channel->queue_lock[queue_index]));

  *len = channel->queue_pos[queue_index];
  if( channel->queue_pos[queue_index] == 0)
  {
    return NULL;
  }

  void* queue_data = malloc(channel->struct_size * channel->queue_pos[queue_index]);

  if(!queue_data)
  {
    *len = 0;
    return NULL;
  }

  // Copia os dados e limpa a memoria, e coloca a posicao a 0
  memcpy(queue_data, channel->queues[queue_index], channel->struct_size * channel->queue_pos[queue_index]);
  channel->queue_pos[queue_index] = 0;

  pthread_mutex_unlock(&(channel->queue_lock[queue_index]));
  return queue_data;
}

// Liberta a memória alocada para o canal
void channel_destroy(channel_t* channel)
{
  if(channel == NULL)
  {
    return;
  }

  // Liberta cada fila e o array de filas
  for(size_t i = 0; i < channel->num_queues; i++)
  {
    pthread_mutex_lock(&channel->queue_lock[i]);
    free(channel->queues[i]);
    channel->queue_pos[i] = 0;
    channel->queue_size[i] = 0;
    pthread_mutex_unlock(&channel->queue_lock[i]);
    pthread_mutex_destroy(&channel->queue_lock[i]);
  }
  free(channel->queue_size);
  free(channel->queue_pos);
  free(channel->queues);
  free(channel);
}

bool channel_has_messages(channel_t* channel, size_t queue_index)
{
  if(channel == NULL)
  {
    return false;
  }

  // Verifica se o índice da fila é válido
  if(queue_index >= channel->num_queues)
  {
    return false; // Índice inválido
  }

  return channel->queue_pos[queue_index];
}
