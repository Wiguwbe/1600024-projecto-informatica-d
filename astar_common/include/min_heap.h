/*
   Min-Heap
  
   Este é um projeto de implementação de um min-heap, uma estrutura de dados fundamental
   utilizada em ciência da computação. Um min-heap é uma árvore binária completa onde
   cada nó possui um valor menor ou igual aos seus filhos. Ele é frequentemente usado
   para manter o elemento mínimo em tempo constante.
  
   Funcionalidades:
    - Inserir elemento: adiciona um novo elemento ao heap, mantendo a propriedade do min-heap.
    - Extrair mínimo: remove e retorna o elemento mínimo do heap.
    - Remover nó: remove um nó específico do heap.
    - Atualizar custo: atualiza o custo de um nó específico no heap e reorganiza a estrutura.
  
   Utilização:
   1. Crie um min-heap usando a função `min_heap_create()`.
   2. Insira elementos usando a função `min_heap_insert()`.
   3. Extraia o elemento mínimo usando a função `min_heap_pop()`.
   4. Remova um nó específico usando a função `min_heap_remove()`.
   5. Atualize o custo de um nó usando a função `min_heap_update()`.
   6. Destrua o min-heap usando a função `min_heap_destroy()` quando não for mais necessário.
  
   Exemplo de uso:
   ```
   min_heap_t* heap = min_heap_create();
   min_heap_insert(heap, 5, NULL);
   min_heap_insert(heap, 10, NULL);
   min_heap_insert(heap, 3, NULL);
   heap_node_t min_node = min_heap_pop(heap);
   min_heap_remove(heap, 10, NULL);
   min_heap_update(heap, 3, 7, NULL);
   min_heap_destroy(heap);
   ```
  
   Estrutura de Dados:
   O min-heap é implementado como uma estrutura `min_heap_t` contendo o tamanho atual do heap
   e um array de `heap_node_t`. Cada `heap_node_t` possui um custo (inteiro) e um ponteiro para um estado (void*).
  
   Limitações:
    - Não há verificação de erros para operações inválidas, como remover ou atualizar um nó inexistente.
  
   Observações:
   
   - Este alocador foi implementado com propósitos educacionais no âmbito do projecto final do curso
     de engenharia informática, não é recomendado para uso em cenários de produção real.
   - Mão foi otimizado para eficiência máxima ou lidar com casos de uso complexos.
   - É importante compreender e analisar o código antes de utilizá-lo em outro projecto,
     fazendo as adaptações necessárias para atender aos requisitos específicos do projeto. 
*/
#ifndef MIN_HEAP_H
#define MIN_HEAP_H
#include <stddef.h>

// Estrutura para representar um nó do heap
typedef struct
{
  int cost;
  void* data;
} heap_node_t;

// Estrutura para representar o min-heap
typedef struct
{
  heap_node_t* data;
  size_t capacity;
  size_t size;
} min_heap_t;

// Cria um novo min-heap
min_heap_t* min_heap_create();\

// Destroi o min-heap e liberta a memória
void min_heap_destroy(min_heap_t* heap);

// Insere um novo elemento no heap
size_t min_heap_insert(min_heap_t* heap, int cost, void* data);

// Extrai e retorna o elemento de custo mínimo do heap
heap_node_t min_heap_pop(min_heap_t* heap);

// Remove um elemento específico do heap
void min_heap_remove(min_heap_t* heap, int cost, void* data);

// Atualiza o custo de um nó específico no heap
void min_heap_update(min_heap_t* heap, int old_cost, int new_cost, void* data);

// Atualiza o custo de um nó específico no heap
void min_heap_update_cost(min_heap_t* heap, int index, int cost);

// Limpa a min_heap
void min_heap_clean(min_heap_t* heap);

#endif
