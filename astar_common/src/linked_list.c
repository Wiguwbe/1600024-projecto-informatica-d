#include "linked_list.h"
#include <stdlib.h>

linked_list_t* linked_list_create()
{
  linked_list_t* list = (linked_list_t*)malloc(sizeof(linked_list_t));
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  return list;
}

void linked_list_destroy(linked_list_t* list)
{
  linked_list_node_t* current_node = list->head;
  while(current_node != NULL)
  {
    linked_list_node_t* next_node = current_node->next;
    free(current_node);
    current_node = next_node;
  }
  free(list);
}

void linked_list_append(linked_list_t* list, void* data)
{
  // Criação do novo nó
  linked_list_node_t* new_node = (linked_list_node_t*)malloc(sizeof(linked_list_node_t));
  new_node->data = data;
  new_node->next = NULL;

  // Verifica se a lista está vazia
  if(list->head == NULL)
  {
    list->head = new_node;
    list->tail = new_node;
  }
  else
  {
    // Adiciona o novo nó no final da lista
    list->tail->next = new_node;
    list->tail = new_node;
  }

  list->size++;
}

void linked_list_insert(linked_list_t* list, void* data, size_t index)
{
  // Verifica se o índice é válido
  if(index > list->size)
  {
    return;
  }

  // Criação do novo nó
  linked_list_node_t* new_node = (linked_list_node_t*)malloc(sizeof(linked_list_node_t));
  new_node->data = data;

  // Verifica se a lista está vazia
  if(list->head == NULL)
  {
    new_node->next = NULL;
    list->head = new_node;
    list->tail = new_node;
  }
  else if(index == 0)
  {
    // Adiciona o novo nó no início da lista
    new_node->next = list->head;
    list->head = new_node;
  }
  else
  {
    // Encontra o nó anterior ao nó que será inserido
    linked_list_node_t* prev_node = list->head;
    for(size_t i = 0; i < index - 1; i++)
    {
      prev_node = prev_node->next;
    }

    // Adiciona o novo nó na posição especificada
    new_node->next = prev_node->next;
    prev_node->next = new_node;
  }

  list->size++;
}

void linked_list_remove(linked_list_t* list, size_t index)
{
  // Verifica se o índice é válido
  if(index >= list->size)
  {
    return;
  }

  // Verifica se a lista contém apenas um elemento
  if(list->size == 1)
  {
    free(list->head);
    list->head = NULL;
    list->tail = NULL;
  }
  else if(index == 0)
  {
    // Remove o primeiro nó da lista
    linked_list_node_t* node_to_remove = list->head;
    list->head = list->head->next;
    free(node_to_remove);
  }
  else
  {
    // Encontra o nó anterior ao nó que será removido
    linked_list_node_t* prev_node = list->head;
    for(size_t i = 0; i < index - 1; i++)
    {
      prev_node = prev_node->next;
    }

    // Remove o nó da posição especificada
    linked_list_node_t* node_to_remove = prev_node->next;
    prev_node->next = node_to_remove->next;

    // Atualiza a cauda da lista, se necessário
    if(index == list->size - 1)
    {
      list->tail = prev_node;
    }

    free(node_to_remove);
  }

  list->size--;
}

void* linked_list_pop_back(linked_list_t* list)
{
  // Verifica se o índice é válido
  if(!list->size)
  {
    return NULL;
  }

  linked_list_node_t* node = list->head;
  list->head = node->next;
  list->size--;

  void* data = node->data;
  free(node);
  return data;
}

size_t linked_list_size(linked_list_t* list)
{
  return list->size;
}

void* linked_list_get(linked_list_t* list, size_t index)
{
  // Verifica se o índice é válido
  if(index >= list->size)
  {
    return NULL;
  }

  // Encontra o nó na posição especificada
  linked_list_node_t* current_node = list->head;
  for(size_t i = 0; i < index; i++)
  {
    current_node = current_node->next;
  }

  return current_node->data;
}
