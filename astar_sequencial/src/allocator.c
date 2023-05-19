/*
   Alocador de Memória com Páginas

   Este alocador implementa uma estratégia simples de alocação de memória baseada em páginas.
   Cada página tem um tamanho fixo de 1 gigabyte. O objetivo do alocador é alocar memória
   para uma estrutura específica, cujo tamanho é especificado na inicialização do alocador.

   Funcionalidades:

   - `allocator_init`: Inicializa o alocador de memória com o tamanho da estrutura a ser alocada.
   - `allocator_free`: Liberta o alocador de memória e todas as páginas alocadas.

   Estrutura do Alocador:

   - `struct_size`: Tamanho da estrutura a ser alocada.
   - `page_size`: Tamanho fixo da página em bytes (1 gigabyte).
   - `pages`: Array de ponteiros para as páginas alocadas.
   - `num_pages`: Número total de páginas alocadas.
   - `current_page`: Índice da página atual.
   - `offset`: Deslocamento atual dentro da página.

   Utilização:

   1. Chame `allocator_init` para criar uma instância do alocador de memória, especificando o tamanho
      da estrutura a ser alocada.
   2. Use `allocator_alloc` para alocar uma estrutura de memória no alocador. A função retornará um
      ponteiro para a memória alocada.

   Limitações e Considerações:

   - O alocador atualmente não verifica se a estrutura alocada é maior do que o tamanho da página,
     o que pode resultar em comportamento indefinido se o tamanho da estrutura exceder o tamanho da página.

   Observações:
   
   - Este alocador foi implementado com propósitos educacionais no âmbito do projecto final do curso
     de engenharia informática, não é recomendado para uso em cenários de produção real.
   - Mão foi otimizado para eficiência máxima ou lidar com casos de uso complexos.
   - É importante compreender e analisar o código antes de utilizá-lo em outro projecto,
     fazendo as adaptações necessárias para atender aos requisitos específicos do projeto.

   Exemplo de Uso:

   ```
   allocator_t* allocator = allocator_init(sizeof(my_struct_t));

   my_struct_t* my_struct = (my_struct_t*)allocator_alloc(allocator);
   // Utilize my_struct normalmente...

   allocator_free(allocator);
   free(allocator);
   ```
*/
#include "allocator.h"
#include <stdlib.h>

// Inicializa o alocador de memória
allocator_t* allocator_init(size_t struct_size) {
    allocator_t* allocator = malloc(sizeof(allocator_t));
    if (allocator == NULL) {
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

// Libera o alocador de memória e todas as páginas alocadas
void allocator_free(allocator_t* allocator) {
    for (size_t i = 0; i < allocator->num_pages; i++) {
        free(allocator->pages[i]);
    }
    free(allocator->pages);
    allocator->pages = NULL;
    allocator->num_pages = 0;
    allocator->current_page = 0;
    allocator->offset = 0;
}

// Aloca uma estrutura de memória no alocador
void* allocator_alloc(allocator_t* allocator) {
    // Se não houver páginas alocadas, alocar a primeira página
    if (allocator->num_pages == 0) {
        allocator->num_pages++;
        allocator->pages = malloc(allocator->num_pages * sizeof(void*));
        allocator->pages[0] = malloc(allocator->page_size);
    }

    // Se não houver espaço na página atual, alocar uma nova página
    if (allocator->offset + allocator->struct_size > allocator->page_size) {
        allocator->current_page++;
        allocator->offset = 0;

        // Se não houver páginas suficientes alocadas, alocar mais uma página
        if (allocator->current_page >= allocator->num_pages) {
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
