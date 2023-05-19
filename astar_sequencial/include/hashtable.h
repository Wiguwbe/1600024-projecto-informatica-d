#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct entry_t entry_t;

// Definição da hashtable
typedef struct hashtable_t {
    size_t struct_size;
    size_t capacity;
    entry_t** buckets;
} hashtable_t;

// Inicializa uma nova hashtable
hashtable_t* hashtable_init(size_t struct_size);

// Insere uma struct na hashtable
void hashtable_insert(hashtable_t* hashtable, const void* data);

// Verifica se uma struct já está na hashtable
bool hashtable_contains(hashtable_t* hashtable, const void* data);

// Libera a memória utilizada pela hashtable
void hashtable_free(hashtable_t* hashtable);

#endif  // HASHTABLE_H
