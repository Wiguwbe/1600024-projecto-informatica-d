#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <pthread.h>
#include <stddef.h>

// Estrutura de um nó na fila
typedef struct queue_node_t
{
  void* data; // Dados a serem armazenados no nó
  struct queue_node_t* next; // Ponteiro para o próximo nó
} queue_node_t;

// Estrutura da fila FIFO
typedef struct
{
  queue_node_t* head; // Ponteiro para o primeiro nó na fila
  queue_node_t* tail; // Ponteiro para o último nó na fila
  size_t size; // Tamanho atual da fila
  pthread_mutex_t lock; // Mutex para garantir a thread-safety
} queue_t;

// Inicializa a fila vazia
queue_t* queue_create();

// Insere um elemento na fila
void queue_push(queue_t* queue, void* data);

// Remove e retorna o primeiro elemento da fila
void* queue_pop(queue_t* queue);

// Verifica se a fila está vazia
bool queue_is_empty(queue_t* queue);

// Retorna o tamanho atual da fila
size_t queue_size(queue_t* queue);

// Liberta a memória alocada para a fila
void queue_destroy(queue_t* queue);

#endif // QUEUE_H
