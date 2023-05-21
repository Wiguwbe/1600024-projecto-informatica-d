/*
   hashtable_t em C com tamanho dinâmico para armazenar ponteiros para structs.

   Funcionalidades:
   - Inicializar uma nova hashtable com um tamanho de struct especificado.
   - Inserir uma struct na hashtable usando uma chave gerada a partir dos dados da struct.
   - Verificar se uma struct está presente na hashtable.
   - Libertar a memória utilizada pela hashtable.

   Estrutura da HashTable:
   - A hashtable utiliza o método de encadeamento separado para lidar com colisões.
   - Cada bucket na hashtable contém uma lista encadeada de entradas.
   - Cada entrada armazena um ponteiro para a struct e a chave correspondente.

   Utilização:
   1. Inclua o arquivo de cabeçalho "hashtable.h" em seu código.
   2. Crie uma nova hashtable usando a função hashtable_create(), especificando o tamanho da struct.
   3. Insira as structs na hashtable usando a função hashtable_insert().
   4. Verifique se uma struct está presente usando a função hashtable_contains().
   5. Libere a memória utilizada pela hashtable usando a função hashtable_destroy().

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
   hashtable_t* hashtable = hashtable_create(sizeof(Person));

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

   // Liberta a memória utilizada pela hashtable
   hashtable_destroy(hashtable);
*/
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct entry_t entry_t;

// Tipo para funções para comparar dados dentro da hastable
typedef bool (*hashtable_compare_func)(const void*, const void*);

// Definição da hashtable
typedef struct hashtable_t
{
  size_t struct_size;
  size_t capacity;
  entry_t** buckets;
  hashtable_compare_func cmp_func;
} hashtable_t;


// Inicializa uma nova hashtable
hashtable_t* hashtable_create(size_t struct_size, hashtable_compare_func cmp_func);

// Insere uma struct na hashtable
void hashtable_insert(hashtable_t* hashtable, void* data);

// Verifica se uma struct já está na hashtable, retorna NULL se os dados não foram encontrados
// ou o ponteiro para a zona de memória onde se encontra os dados
void* hashtable_contains(hashtable_t* hashtable, const void* data);

// Liberta a memória utilizada pela hashtable, atenção, não liberta os dados apenas a hashtable
void hashtable_destroy(hashtable_t* hashtable, bool free_data);

#endif // HASHTABLE_H
