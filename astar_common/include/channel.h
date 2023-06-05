/*
  Canal produtor/consumidor 

  Este canal é utilizado para comunicação de mensagens entre threads. O canal oferece múltiplas files, uma fila
  por thread, o número de filas é especificado quando se cria o canal. Uma mensagem é uma estrutura de dados 
  de um tamanho especificado quando se inicializa o canal.

  Cada thread pode colocar uma mensagem na fila dedicada para outra thread, para isso ao colocar a mensagem tem
  de especificar em qual fila quer colocar a mensagem. As fila de mensagens são FIFO.

  As operações de colocar mensagens e de retirar mensagens são thread-safe com recurso ao uso de mutexes. Por uma
  questão de performance é possível também colocar um bloco de mensagens para filas diferentes e o canal deve
  distribuir as mensagens para a fila correta.
*/

#ifndef CHANNEL_H
#define CHANNEL_H

#include "queue.h"

// Estrutura do canal
typedef struct
{
  queue_t** queues; // Array de filas
  size_t num_queues; // Número de filas
} channel_t;

// Inicializa o canal com o número especificado de filas
channel_t* channel_create(size_t num_queues);

// Envia uma mensagem para uma fila específica no canal
void channel_send(channel_t* channel, size_t queue_index, void* data);

// Recebe uma mensagem da fila específica no canal (bloqueante)
void* channel_receive(channel_t* channel, size_t queue_index);

// Liberta a memória alocada para o canal
void channel_destroy(channel_t* channel, bool free_data);

#endif // CHANNEL_H
