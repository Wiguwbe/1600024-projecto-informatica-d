#include "channel.h"
#include <stdlib.h>

// Inicializa o canal com o número especificado de filas
channel_t* channel_create(size_t num_queues)
{
  channel_t* channel = (channel_t*)malloc(sizeof(channel_t));
  if(channel == NULL)
  {
    return NULL;
  }

  // Aloca espaço para o array de filas
  channel->queues = (queue_t**)malloc(num_queues * sizeof(queue_t*));
  if(channel->queues == NULL)
  {
    free(channel);
    return NULL;
  }

  // Inicializa cada fila
  for(size_t i = 0; i < num_queues; i++)
  {
    channel->queues[i] = queue_create();
    if(channel->queues[i] == NULL)
    {
      // Em caso de falha, destrói as filas já criadas e liberta a memória alocada
      for(size_t j = 0; j < i; j++)
      {
        queue_destroy(channel->queues[j],false);
      }
      free(channel->queues);
      free(channel);
      return NULL;
    }
  }

  channel->num_queues = num_queues;
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

  queue_push(channel->queues[queue_index], data);
}

// Recebe uma mensagem da fila específica no canal (bloqueante)
void* channel_receive(channel_t* channel, size_t queue_index)
{
  // Verifica se o índice da fila é válido
  if(queue_index >= channel->num_queues)
  {
    return NULL; // Índice inválido
  }

  return queue_pop(channel->queues[queue_index]);
}

// Liberta a memória alocada para o canal
void channel_destroy(channel_t* channel, bool free_data)
{
  if(channel == NULL)
  {
    return;
  }

  // Liberta cada fila e o array de filas
  for(size_t i = 0; i < channel->num_queues; i++)
  {
    queue_destroy(channel->queues[i], free_data);
  }
  free(channel->queues);
  free(channel);
}
