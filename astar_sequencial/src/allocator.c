#include "allocator.h"
#include <stdlib.h>
#include <string.h>

Allocator* allocator_create(size_t block_size, size_t num_blocks) {
    Allocator* allocator = (Allocator*)malloc(sizeof(Allocator));
    if (allocator == NULL) {
        return NULL;
    }

    allocator->ptr = malloc(block_size * num_blocks);
    if (allocator->ptr == NULL) {
        free(allocator);
        return NULL;
    }

    allocator->block_size = block_size;
    allocator->num_blocks = num_blocks;
    allocator->used_blocks = 0;

    return allocator;
}

void allocator_destroy(Allocator* allocator) {
    if (allocator == NULL) {
        return;
    }

    free(allocator->ptr);
    free(allocator);
}

void* allocator_alloc(Allocator* allocator) {
    if (allocator == NULL || allocator->used_blocks == allocator->num_blocks) {
        return NULL;
    }

    void* ptr = (char*)allocator->ptr + (allocator->block_size * allocator->used_blocks);
    allocator->used_blocks++;

    return ptr;
}

void* allocator_realloc(Allocator* allocator, void* ptr) {
    if (allocator == NULL || ptr == NULL) {
        return NULL;
    }

    void* new_ptr = realloc(ptr, allocator->block_size * allocator->num_blocks);
    if (new_ptr == NULL) {
        return NULL;
    }

    allocator->ptr = new_ptr;
    return allocator->ptr;
}

void allocator_free(Allocator* allocator, void* ptr) {
    if (allocator == NULL || ptr == NULL) {
        return;
    }

    if (allocator->used_blocks > 0) {
        allocator->used_blocks--;
    }
}

