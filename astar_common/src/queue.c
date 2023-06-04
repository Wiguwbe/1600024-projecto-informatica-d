#include "queue.h"
#include <pthread.h>
#include <stdlib.h>

// Inicializa a fila vazia
queue_t* queue_create()
{
  queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
  if(queue == NULL)
  {
    // Falha na alocação de memória
    return NULL;
  }

  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
  pthread_mutex_init(&(queue->lock), NULL);

  return queue;
}

// Insere um elemento na fila
void queue_push(queue_t* queue, void* data)
{
  queue_node_t* new_node = (queue_node_t*)malloc(sizeof(queue_node_t));
  if(new_node == NULL)
  {
    // Falha na alocação de memória
    return;
  }

  new_node->data = data;
  new_node->next = NULL;

  pthread_mutex_lock(&(queue->lock));

  if(queue->tail == NULL)
  {
    // A fila está vazia, o novo nó será o primeiro e o último
    queue->head = new_node;
    queue->tail = new_node;
  }
  else
  {
    // Adiciona o novo nó ao final da fila
    queue->tail->next = new_node;
    queue->tail = new_node;
  }

  queue->size++;

  pthread_mutex_unlock(&(queue->lock));
}

// Remove e retorna o primeiro elemento da fila
void* queue_pop(queue_t* queue)
{
  if(queue_is_empty(queue))
  {
    // A fila está vazia, não há nada para remover
    return NULL;
  }

  pthread_mutex_lock(&(queue->lock));

  queue_node_t* node = queue->head;
  void* data = node->data;

  queue->head = node->next;

  if(queue->head == NULL)
  {
    // A fila ficou vazia após a remoção
    queue->tail = NULL;
  }

  queue->size--;

  pthread_mutex_unlock(&(queue->lock));

  free(node);
  return data;
}

// Verifica se a fila está vazia
bool queue_is_empty(queue_t* queue)
{
  return queue->head == NULL;
}

// Retorna o tamanho atual da fila
size_t queue_size(queue_t* queue)
{
  return queue->size;
}

// Liberta a memória alocada para a fila
void queue_destroy(queue_t* queue)
{
  pthread_mutex_lock(&(queue->lock));

  queue_node_t* current = queue->head;
  queue_node_t* next;

  while(current != NULL)
  {
    next = current->next;
    free(current);
    current = next;
  }

  pthread_mutex_unlock(&(queue->lock));
  pthread_mutex_destroy(&(queue->lock));

  free(queue);
}
