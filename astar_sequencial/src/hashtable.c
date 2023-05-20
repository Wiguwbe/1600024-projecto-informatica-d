#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

// Definição de uma entrada na hashtable
struct entry_t
{
  void* data;
  struct entry_t* next;
};

// Função para calcular o índice do bucket na hashtable
static size_t hash(hashtable_t* hashtable, const void* data)
{
  // Cast dos dados para um array de bytes
  const unsigned char* bytes = (const unsigned char*)data;

  // Calcula o valor hash inicial
  size_t hash_value = 0;
  for(size_t i = 0; i < hashtable->struct_size; ++i)
  {
    hash_value = hash_value * 31 + bytes[i];
  }

  // Retorna o índice do bucket
  return hash_value % hashtable->capacity;
}

// Inicializa uma nova hashtable
hashtable_t* hashtable_create(size_t struct_size)
{
  // Aloca memória para a estrutura da hashtable
  hashtable_t* hashtable = (hashtable_t*)malloc(sizeof(hashtable_t));
  if(hashtable == NULL)
  {
    return NULL;
  }
  // Define o tamanho da struct
  hashtable->struct_size = struct_size;

  // Define a capacidade inicial da hashtable
  hashtable->capacity = 2048;

  // Aloca memória para os buckets da hashtable
  hashtable->buckets = (entry_t**)calloc(hashtable->capacity, sizeof(entry_t*));
  if(hashtable->buckets == NULL)
  {
    free(hashtable);
    return NULL;
  }

  return hashtable;
}

// Insere uma struct na hashtable
void hashtable_insert(hashtable_t* hashtable, void* data)
{
  // Calcula o índice do bucket
  size_t index = hash(hashtable, data);

  // Cria uma nova entrada
  entry_t* entry = (entry_t*)malloc(sizeof(entry_t));
  if(entry == NULL)
  {
    return;
  }

  // Copia os dados da struct para a entrada
  entry->data = data;

  // Insere a entrada no início do bucket
  entry->next = hashtable->buckets[index];
  hashtable->buckets[index] = entry;
}

// Verifica se uma struct já está na hashtable, retorna o ponteira para os
// dados caso exista, NULL caso não exista
void* hashtable_contains(hashtable_t* hashtable, const void* data)
{
  // Calcula o índice do bucket
  size_t index = hash(hashtable, data);

  // Percorre as entradas no bucket
  entry_t* entry = hashtable->buckets[index];
  while(entry != NULL)
  {
    // Compara os dados da struct
    if(memcmp(entry->data, data, hashtable->struct_size) == 0)
    {
      return entry->data;
    }
    entry = entry->next;
  }

  return NULL;
}

// Liberta a memória utilizada pela hashtable, atenção, não liberta os dados apenas a hashtable
void hashtable_destroy(hashtable_t* hashtable)
{
  // Percorre todos os buckets e Liberta as entradas
  for(size_t i = 0; i < hashtable->capacity; ++i)
  {
    entry_t* entry = hashtable->buckets[i];
    while(entry != NULL)
    {
      entry_t* next = entry->next;
      free(entry);
      entry = next;
    }
  }

  // Liberta a memória dos buckets e da hashtable
  free(hashtable->buckets);
  free(hashtable);
}
