#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

// Função de hashing utilizada
size_t hash_function(const void* data, size_t size, size_t mod)
{
  const unsigned char* bytes = (const unsigned char*)data;

  // Calcula o valor hash inicial
  size_t hash_value = 0;
  size_t mul = 1;
  for(size_t i = 0; i < size; ++i)
  {
    mul = (i % 4 == 0) ? 1 : mul * 256;
    hash_value += bytes[i] * mul;
  }

  // Retorna o índice do bucket
  return hash_value % mod;
}

// Função para calcular o índice do bucket na hashtable
static size_t hash(hashtable_t* hashtable, const void* data)
{
  return hash_function(data, hashtable->struct_size, hashtable->capacity);
}

// Inicializa uma nova hashtable
hashtable_t* hashtable_create(size_t struct_size, hashtable_compare_func cmp_func, hashtable_hash_func hash_func)
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
  hashtable->capacity = HASH_CAPACITY;

  // Aloca memória para os buckets da hashtable
  hashtable->buckets = (entry_t**)calloc(hashtable->capacity, sizeof(entry_t*));
  if(hashtable->buckets == NULL)
  {
    free(hashtable);
    return NULL;
  }

  hashtable->cmp_func = cmp_func;
  hashtable->hash_func = hash_func;
  if(hashtable->hash_func == NULL)
  {
    hashtable->hash_func = hash;
  }

  // Inicializa os mutexes para cada bucket
  hashtable->mutexes = (pthread_mutex_t*)malloc(HASH_MAX_MUTEXES * sizeof(pthread_mutex_t));
  if(hashtable->mutexes == NULL)
  {
    free(hashtable->buckets);
    free(hashtable);
    return NULL;
  }
  for(size_t i = 0; i < HASH_MAX_MUTEXES; ++i)
  {
    pthread_mutex_init(&hashtable->mutexes[i], NULL);
  }

  return hashtable;
}

// Insere uma struct na hashtable
void hashtable_insert(hashtable_t* hashtable, void* data)
{
  // Calcula o índice do bucket
  size_t index = hashtable->hash_func(hashtable, data);

  // Cria uma nova entrada
  entry_t* entry = (entry_t*)malloc(sizeof(entry_t));
  if(entry == NULL)
  {
    return;
  }

  // Copia os dados da struct para a entrada
  entry->data = data;

  // Calcula o mutex para este índice
  int mutex_id = index % HASH_MAX_MUTEXES;

  // bloqueia o respetivo bucket
  pthread_mutex_lock(&hashtable->mutexes[mutex_id]);

  // Insere a entrada no início do bucket
  entry->next = hashtable->buckets[index];
  hashtable->buckets[index] = entry;

  // Desbloqueia o bucket
  pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);
}

// Verifica se uma struct já está na hashtable, retorna o ponteira para os
// dados caso exista, NULL caso não exista
void* hashtable_contains(hashtable_t* hashtable, const void* data)
{
  // Calcula o índice do bucket
  size_t index = hashtable->hash_func(hashtable, data);

  // Calcula o mutex para este índice
  int mutex_id = index % HASH_MAX_MUTEXES;

  // bloqueia o respetivo bucket
  pthread_mutex_lock(&hashtable->mutexes[mutex_id]);

  // Percorre as entradas no bucket
  entry_t* entry = hashtable->buckets[index];

  // Se não fornecemos um comparador utilizamos o comparador por defeito
  if(hashtable->cmp_func == NULL)
  {
    while(entry != NULL)
    {
      // Compara os dados da struct
      if(memcmp(entry->data, data, hashtable->struct_size) == 0)
      {
        // Desbloqueia o bucket
        pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);

        return entry->data;
      }
      entry = entry->next;
    }
  }
  else
  {
    while(entry != NULL)
    {
      // utiliza o comparador fornecido para verificar se os elementos são iguais
      if(hashtable->cmp_func(entry->data, data))
      {
        // Desbloqueia o bucket
        pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);

        return entry->data;
      }
      entry = entry->next;
    }
  }

  // Desbloqueia o bucket
  pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);

  return NULL;
}

// Liberta a memória utilizada pela hashtable, atenção, não liberta os dados apenas a hashtable
void hashtable_destroy(hashtable_t* hashtable, bool free_data)
{
  // Percorre todos os buckets e Liberta as entradas
  for(size_t i = 0; i < hashtable->capacity; ++i)
  {
    // Calcula o mutex para este índice
    int mutex_id = i % HASH_MAX_MUTEXES;

    // bloqueia o respetivo bucket
    pthread_mutex_lock(&hashtable->mutexes[mutex_id]);

    entry_t* entry = hashtable->buckets[i];
    while(entry != NULL)
    {
      entry_t* next = entry->next;
      if(free_data)
        free(entry->data);
      free(entry);
      entry = next;
    }

    // Desbloqueia o bucket
    pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);

    // Destroy the mutex
    pthread_mutex_destroy(&hashtable->mutexes[mutex_id]);
  }

  // Liberta a memória dos mutexes, buckets e da hashtable
  free(hashtable->mutexes);
  free(hashtable->buckets);
  free(hashtable);
}

entry_t* hashtable_reserve(hashtable_t* hashtable, void* data)
{
  // Calcula o índice do bucket
  size_t index = hashtable->hash_func(hashtable, data);

  // Calcula o mutex para este índice
  int mutex_id = index % HASH_MAX_MUTEXES;

  // bloqueia o respetivo bucket
  pthread_mutex_lock(&hashtable->mutexes[mutex_id]);

  // Percorre as entradas no bucket
  entry_t* entry = hashtable->buckets[index];

  // Se não fornecemos um comparador utilizamos o comparador por defeito
  if(hashtable->cmp_func == NULL)
  {
    while(entry != NULL)
    {
      // Compara os dados da struct
      if(memcmp(entry->data, data, hashtable->struct_size) == 0)
      {
        // Desbloqueia o bucket
        pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);

        return entry;
      }
      entry = entry->next;
    }
  }
  else
  {
    while(entry != NULL)
    {
      // utiliza o comparador fornecido para verificar se os elementos são iguais
      if(hashtable->cmp_func(entry->data, data))
      {
        // Desbloqueia o bucket
        pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);

        return entry;
      }
      entry = entry->next;
    }
  }

  // Cria uma nova entrada
  entry = (entry_t*)malloc(sizeof(entry_t));
  if(entry == NULL)
  {
    pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);
    return NULL;
  }

  // Copia os dados da struct para a entrada
  entry->data = data;

  // Insere a entrada no início do bucket
  entry->next = hashtable->buckets[index];
  hashtable->buckets[index] = entry;

  // Desbloqueia o bucket
  pthread_mutex_unlock(&hashtable->mutexes[mutex_id]);

  return entry;
}
