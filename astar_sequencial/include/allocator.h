#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    size_t struct_size;  // Tamanho da estrutura a ser alocada
    size_t page_size;    // Tamanho da página em bytes
    void** pages;        // Array de ponteiros para as páginas alocadas
    size_t num_pages;    // Número total de páginas alocadas
    size_t current_page; // Índice da página atual
    size_t offset;       // Deslocamento atual dentro da página
} allocator_t;

// Inicializa o alocador de memória
allocator_t* allocator_init(size_t struct_size);

// Libera o alocador de memória e todas as páginas alocadas
void allocator_free(allocator_t* allocator);

// Aloca uma estrutura de memória no alocador
void* allocator_alloc(allocator_t* allocator);

#endif  // ALLOCATOR_H
