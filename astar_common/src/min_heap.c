#include "min_heap.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define INITIAL_CAPACITY 1024

min_heap_t* min_heap_create()
{
  // Aloca memória para a estrutura min_heap_t
  min_heap_t* heap = (min_heap_t*)malloc(sizeof(min_heap_t));

  if(heap == NULL)
  {
    return NULL;
  }

  // Aloca memória para o array de elementos heap_node_t
  heap->data = (heap_node_t*)malloc(INITIAL_CAPACITY * sizeof(heap_node_t));

  // Inicializa a capacidade e o tamanho do heap
  heap->capacity = INITIAL_CAPACITY;
  heap->size = 0;

  return heap;
}

void min_heap_destroy(min_heap_t* heap)
{
  if(heap == NULL)
  {
    return;
  }

  // Liberta a memória alocada para o array de elementos heap_node_t
  free(heap->data);

  // Liberta a memória alocada para a estrutura min_heap_t
  free(heap);
}

void ensure_capacity(min_heap_t* heap)
{
  if(heap == NULL)
  {
    return;
  }

  // Verifica se o heap está cheio e aumenta sua capacidade se necessário
  if(heap->size == heap->capacity)
  {
    // Dobra a capacidade do heap
    heap->capacity *= 2;

    // Realoca memória para o array de elementos heap_node_t com a nova capacidade
    heap->data = (heap_node_t*)realloc(heap->data, heap->capacity * sizeof(heap_node_t));
  }
}

void swap(heap_node_t* a, heap_node_t* b)
{
  // Troca os valores de dois elementos heap_node_t
  heap_node_t temp = *a;
  *a = *b;
  *b = temp;
}

void heapify_up(min_heap_t* heap, size_t index)
{
  // Move o elemento para cima no heap enquanto seu custo for menor que o custo do pai
  if(index == 0)
    return;

  int parent_index = (index - 1) / 2;

  // Se o custo do elemento atual for menor que o custo do pai, troca-os de posição
  if(heap->data[index].cost < heap->data[parent_index].cost)
  {
    swap(&(heap->data[index]), &(heap->data[parent_index]));
    heapify_up(heap, parent_index);
  }
}

void heapify_down(min_heap_t* heap, size_t index)
{
  // Move o elemento para baixo no heap enquanto seu custo for maior que o custo dos filhos
  size_t left_child_index = 2 * index + 1;
  size_t right_child_index = 2 * index + 2;
  size_t smallest = index;

  // Compara o custo do elemento atual com o custo dos filhos esquerdo e direito
  if(left_child_index < heap->size && heap->data[left_child_index].cost < heap->data[smallest].cost)
    smallest = left_child_index;

  if(right_child_index < heap->size && heap->data[right_child_index].cost < heap->data[smallest].cost)
    smallest = right_child_index;

  // Se o elemento atual não for o menor, troca-o com o menor filho e continua a verificação
  if(smallest != index)
  {
    swap(&(heap->data[index]), &(heap->data[smallest]));
    heapify_down(heap, smallest);
  }
}

void min_heap_insert(min_heap_t* heap, int cost, void* data)
{
  if(heap == NULL)
  {
    return;
  }

  // Garante que o heap tem capacidade suficiente para inserir um novo elemento
  ensure_capacity(heap);

  size_t index = heap->size;

  // Insere o novo elemento no final do heap
  heap->data[index].cost = cost;
  heap->data[index].data = data;

  // incrementa o tamanho da nossa heap
  heap->size++;

  // Realiza o heapify-up para ajustar a posição do novo elemento no heap
  heapify_up(heap, index);
}

heap_node_t min_heap_pop(min_heap_t* heap)
{
  if(heap == NULL)
  {
    // Retorna um heap_node_t inválido
    heap_node_t empty_node = { 0, NULL };
    return empty_node;
  }

  // Verifica se o heap está vazio
  if(heap->size == 0)
  {
    // Retorna um heap_node_t inválido
    heap_node_t empty_node = { 0, NULL };
    return empty_node;
  }

  // Armazena o valor mínimo
  heap_node_t min_node = heap->data[0];

  // Substitui o valor mínimo pelo último elemento do heap
  heap->data[0] = heap->data[heap->size - 1];

  // Reduz o tamanho do heap
  heap->size--;

  // Realiza o heapify-down para restaurar as propriedades do heap
  heapify_down(heap, 0);

  // Retorna o valor mínimo extraído
  return min_node;
}

void min_heap_remove(min_heap_t* heap, int cost, void* data)
{
  if(heap == NULL)
  {
    return;
  }

  size_t index = SIZE_MAX;

  // Procura o elemento no heap
  for(size_t i = 0; i < heap->size; i++)
  {
    if(heap->data[i].cost == cost && heap->data[i].data == data)
    {
      index = i;
      break;
    }
  }

  if(index == SIZE_MAX)
    return;

  // Move o último elemento para a posição do elemento a ser removido
  heap->data[index] = heap->data[heap->size - 1];
  heap->size--;

  // Reorganiza o heap
  heapify_up(heap, index);
  heapify_down(heap, index);
}

void min_heap_update(min_heap_t* heap, int old_cost, int new_cost, void* data)
{
  if(heap == NULL)
  {
    return;
  }

  size_t index = SIZE_MAX;

  // Procura o elemento no heap
  for(size_t i = 0; i < heap->size; i++)
  {
    if(heap->data[i].cost == old_cost && heap->data[i].data == data)
    {
      index = i;
      break;
    }
  }

  if(index == SIZE_MAX)
  {
    // Estes dados não estavam nesta min-heap, inserimos com o novo custo
    min_heap_insert(heap, new_cost, data);
    return;
  }

  // Atualiza o custo do elemento
  heap->data[index].cost = new_cost;

  // Reorganiza o heap
  heapify_up(heap, index);
  heapify_down(heap, index);
}