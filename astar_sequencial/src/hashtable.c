/*
   hashtable_t em C com tamanho dinâmico para armazenar ponteiros para structs.

   Funcionalidades:
   - Inicializar uma nova hashtable com um tamanho de struct especificado.
   - Inserir uma struct na hashtable usando uma chave gerada a partir dos dados da struct.
   - Verificar se uma struct está presente na hashtable.
   - Liberar a memória utilizada pela hashtable.

   Estrutura da HashTable:
   - A hashtable utiliza o método de encadeamento separado para lidar com colisões.
   - Cada bucket na hashtable contém uma lista encadeada de entradas.
   - Cada entrada armazena um ponteiro para a struct e a chave correspondente.

   Utilização:
   1. Inclua o arquivo de cabeçalho "hashtable.h" em seu código.
   2. Crie uma nova hashtable usando a função hashtable_init(), especificando o tamanho da struct.
   3. Insira as structs na hashtable usando a função hashtable_insert().
   4. Verifique se uma struct está presente usando a função hashtable_contains().
   5. Libere a memória utilizada pela hashtable usando a função hashtable_free().

   Limitações e Considerações:
   - Esta implementação não lida com situações de concorrência.
   - A capacidade da hashtable é dinâmica e redimensiona automaticamente conforme necessário.
   - No entanto, em cenários de carga elevada ou massiva, pode haver impacto no desempenho
     devido ao redimensionamento e ao método de encadeamento separado.
   - Certifique-se de fornecer um tamanho adequado para a struct ao inicializar a hashtable.
   - Esta hashtable foi desenvolvida como parte de um projeto universitário com o objetivo de
     fornecer uma implementação simples e didática, mas pode não ser adequada para todos os casos
     de uso ou requisitos de desempenho.

   Exemplo de Uso:


   // Inicializa a hashtable
   hashtable_t* hashtable = hashtable_init(sizeof(Person));

   // Cria algumas pessoas
   Person person1 = {1, "Alice"};
   Person person2 = {2, "Bob"};

   // Insere as pessoas na hashtable
   hashtable_insert(hashtable, &person1);
   hashtable_insert(hashtable, &person2);

   // Verifica se as pessoas estão presentes na hashtable
   if (hashtable_contains(hashtable, &person1)) {
       printf("Person1 está na hashtable.\n");
   }

   if (hashtable_contains(hashtable, &person2)) {
       printf("Person2 está na hashtable.\n");
   }

   // Libera a memória utilizada pela hashtable
   hashtable_free(hashtable);
*/
#include "hashtable.h"
#include <stdlib.h>
#include <string.h>

// Definição de uma entrada na hashtable
struct entry_t {
    void* data;
    struct entry_t* next;
};

// Função para calcular o índice do bucket na hashtable
static size_t hash(hashtable_t* hashtable, const void* data) {
    // Cast dos dados para um array de bytes
    const unsigned char* bytes = (const unsigned char*)data;

    // Calcula o valor hash inicial
    size_t hash_value = 0;
    for (size_t i = 0; i < hashtable->struct_size; ++i) {
        hash_value = hash_value * 31 + bytes[i];
    }

    // Retorna o índice do bucket
    return hash_value % hashtable->capacity;
}

// Inicializa uma nova hashtable
hashtable_t* hashtable_init(size_t struct_size) {
    // Aloca memória para a estrutura da hashtable
    hashtable_t* hashtable = (hashtable_t*)malloc(sizeof(hashtable_t));
    if (hashtable == NULL) {
        return NULL;
    }

    // Define o tamanho da struct
    hashtable->struct_size = struct_size;

    // Define a capacidade inicial da hashtable
    hashtable->capacity = 16;

    // Aloca memória para os buckets da hashtable
    hashtable->buckets = (entry_t**)calloc(hashtable->capacity, sizeof(entry_t*));
    if (hashtable->buckets == NULL) {
        free(hashtable);
        return NULL;
    }

    return hashtable;
}

// Insere uma struct na hashtable
void hashtable_insert(hashtable_t* hashtable, const void* data) {
    // Calcula o índice do bucket
    size_t index = hash(hashtable, data);

    // Cria uma nova entrada
    entry_t* entry = (entry_t*)malloc(sizeof(entry_t));
    if (entry == NULL) {
        return;
    }

    // Copia os dados da struct para a entrada
    entry->data = malloc(hashtable->struct_size);
    if (entry->data == NULL) {
        free(entry);
        return;
    }
    memcpy(entry->data, data, hashtable->struct_size);

    // Insere a entrada no início do bucket
    entry->next = hashtable->buckets[index];
    hashtable->buckets[index] = entry;
}

// Verifica se uma struct já está na hashtable
bool hashtable_contains(hashtable_t* hashtable, const void* data) {
    // Calcula o índice do bucket
    size_t index = hash(hashtable, data);

    // Percorre as entradas no bucket
    entry_t* entry = hashtable->buckets[index];
    while (entry != NULL) {
        // Compara os dados da struct
        if (memcmp(entry->data, data, hashtable->struct_size) == 0) {
            return true;
        }
        entry = entry->next;
    }

    return false;
}

// Libera a memória utilizada pela hashtable
void hashtable_free(hashtable_t* hashtable) {
    // Percorre todos os buckets e libera as entradas
    for (size_t i = 0; i < hashtable->capacity; ++i) {
        entry_t* entry = hashtable->buckets[i];
        while (entry != NULL) {
            entry_t* next = entry->next;
            free(entry->data);
            free(entry);
            entry = next;
        }
    }

    // Libera a memória dos buckets e da hashtable
    free(hashtable->buckets);
    free(hashtable);
}
