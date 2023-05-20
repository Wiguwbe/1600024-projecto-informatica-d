#include "allocator.h"
#include <stdlib.h>

// Inicializa o alocador de memória
allocator_t* allocator_init(size_t struct_size)
{
  allocator_t* allocator = (allocator_t*)malloc(sizeof(allocator_t));
  if(allocator == NULL)
  {
    return NULL; // Erro de alocação
  }

  allocator->struct_size = struct_size;
  allocator->page_size = 1 * 1024 * 1024 * 1024; // 1 gigabyte
  allocator->pages = NULL;
  allocator->num_pages = 0;
  allocator->current_page = 0;
  allocator->offset = 0;

  return allocator;
}

// Liberta o alocador de memória e todas as páginas alocadas
void allocator_free(allocator_t* allocator)
{
  for(size_t i = 0; i < allocator->num_pages; i++)
  {
    free(allocator->pages[i]);
  }
  free(allocator->pages);
  allocator->pages = NULL;
  allocator->num_pages = 0;
  allocator->current_page = 0;
  allocator->offset = 0;
}

// Aloca uma estrutura de memória no alocador
void* allocator_alloc(allocator_t* allocator)
{
  // Se não houver páginas alocadas, alocar a primeira página
  if(allocator->num_pages == 0)
  {
    allocator->num_pages++;
    allocator->pages = malloc(allocator->num_pages * sizeof(void*));
    allocator->pages[0] = malloc(allocator->page_size);
  }

  // Se não houver espaço na página atual, alocar uma nova página
  if(allocator->offset + allocator->struct_size > allocator->page_size)
  {
    allocator->current_page++;
    allocator->offset = 0;

    // Se não houver páginas suficientes alocadas, alocar mais uma página
    if(allocator->current_page >= allocator->num_pages)
    {
      allocator->num_pages++;
      allocator->pages = realloc(allocator->pages, allocator->num_pages * sizeof(void*));
      allocator->pages[allocator->current_page] = malloc(allocator->page_size);
    }
  }

  // Calcular o endereço de retorno e atualizar o offset
  void* ptr = (char*)allocator->pages[allocator->current_page] + allocator->offset;
  allocator->offset += allocator->struct_size;

  return ptr;
}
