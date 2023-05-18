#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>  // Adicione esta linha

typedef struct {
    void* ptr;          // Ponteiro para o bloco de memória alocado
    size_t block_size;  // Tamanho de cada bloco
    size_t num_blocks;  // Número total de blocos alocados
    size_t used_blocks; // Número de blocos atualmente em uso
} Allocator;

Allocator* allocator_create(size_t block_size, size_t num_blocks);
void allocator_destroy(Allocator* allocator);
void* allocator_alloc(Allocator* allocator);
void* allocator_realloc(Allocator* allocator, void* ptr);
void allocator_free(Allocator* allocator, void* ptr);

#endif  // ALLOCATOR_H
