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
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct
{
  size_t struct_size; // Tamanho da estrutura a ser alocada
  size_t page_size; // Tamanho da página em bytes
  void** pages; // Array de ponteiros para as páginas alocadas
  size_t num_pages; // Número total de páginas alocadas
  size_t current_page; // Índice da página atual
  size_t offset; // Deslocamento atual dentro da página
} allocator_t;

// Inicializa o alocador de memória
allocator_t* allocator_init(size_t struct_size);

// Liberta o alocador de memória e todas as páginas alocadas, incluindo os dados existentes
void allocator_free(allocator_t* allocator);

// Aloca uma estrutura de memória no alocador
void* allocator_alloc(allocator_t* allocator);

#endif // ALLOCATOR_H
