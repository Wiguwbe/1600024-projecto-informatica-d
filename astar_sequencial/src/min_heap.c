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
  
   Uso:
   1. Crie um min-heap usando a função `create_min_heap()`.
   2. Insira elementos usando a função `insert()`.
   3. Extraia o elemento mínimo usando a função `extract_min()`.
   4. Remova um nó específico usando a função `remove_node()`.
   5. Atualize o custo de um nó usando a função `update_cost()`.
   6. Destrua o min-heap usando a função `destroy_min_heap()` quando não for mais necessário.
  
   Exemplo de uso:
   ```
   min_heap* heap = create_min_heap();
   insert(heap, 5, NULL);
   insert(heap, 10, NULL);
   insert(heap, 3, NULL);
   heap_node min_node = extract_min(heap);
   remove_node(heap, 10, NULL);
   update_cost(heap, 3, 7, NULL);
   destroy_min_heap(heap);
   ```
  
   Estrutura de Dados:
   O min-heap é implementado como uma estrutura `min_heap` contendo o tamanho atual do heap
   e um array de `heap_node`. Cada `heap_node` possui um custo (inteiro) e um ponteiro para um estado (void*).
  
   Limitações:
    - Não há verificação de erros para operações inválidas, como remover ou atualizar um nó inexistente.
  
   Observações:
   
   - Este alocador foi implementado com propósitos educacionais no âmbito do projecto final do curso
     de engenharia informática, não é recomendado para uso em cenários de produção real.
   - Mão foi otimizado para eficiência máxima ou lidar com casos de uso complexos.
   - É importante compreender e analisar o código antes de utilizá-lo em outro projecto,
     fazendo as adaptações necessárias para atender aos requisitos específicos do projeto. 
*/

#include "min_heap.h"
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_CAPACITY 1024

min_heap* create_min_heap() {
    // Aloca memória para a estrutura min_heap
    min_heap* heap = (min_heap*)malloc(sizeof(min_heap));
    
    // Aloca memória para o array de elementos heap_node
    heap->data = (heap_node*)malloc(INITIAL_CAPACITY * sizeof(heap_node));
    
    // Inicializa a capacidade e o tamanho do heap
    heap->capacity = INITIAL_CAPACITY;
    heap->size = 0;
    
    return heap;
}

void destroy_min_heap(min_heap* heap) {
    // Libera a memória alocada para o array de elementos heap_node
    free(heap->data);
    
    // Libera a memória alocada para a estrutura min_heap
    free(heap);
}

void ensure_capacity(min_heap* heap) {
    // Verifica se o heap está cheio e aumenta sua capacidade se necessário
    if (heap->size == heap->capacity) {
        // Dobra a capacidade do heap
        heap->capacity *= 2;
        
        // Realoca memória para o array de elementos heap_node com a nova capacidade
        heap->data = (heap_node*)realloc(heap->data, heap->capacity * sizeof(heap_node));
    }
}

void swap(heap_node* a, heap_node* b) {
    // Troca os valores de dois elementos heap_node
    heap_node temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(min_heap* heap, int index) {
    // Move o elemento para cima no heap enquanto seu custo for menor que o custo do pai
    if (index == 0)
        return;

    int parent_index = (index - 1) / 2;
    
    // Se o custo do elemento atual for menor que o custo do pai, troca-os de posição
    if (heap->data[index].cost < heap->data[parent_index].cost) {
        swap(&(heap->data[index]), &(heap->data[parent_index]));
        heapify_up(heap, parent_index);
    }
}

void heapify_down(min_heap* heap, int index) {
    // Move o elemento para baixo no heap enquanto seu custo for maior que o custo dos filhos
    int left_child_index = 2 * index + 1;
    int right_child_index = 2 * index + 2;
    int smallest = index;

    // Compara o custo do elemento atual com o custo dos filhos esquerdo e direito
    if (left_child_index < heap->size && heap->data[left_child_index].cost < heap->data[smallest].cost)
        smallest = left_child_index;

    if (right_child_index < heap->size && heap->data[right_child_index].cost < heap->data[smallest].cost)
        smallest = right_child_index;

    // Se o elemento atual não for o menor, troca-o com o menor filho e continua a verificação
    if (smallest != index) {
        swap(&(heap->data[index]), &(heap->data[smallest]));
        heapify_down(heap, smallest);
    }
}

void insert(min_heap* heap, int cost, void* data) {
    // Garante que o heap tem capacidade suficiente para inserir um novo elemento
    ensure_capacity(heap);

    int index = heap->size;
    
    // Insere o novo elemento no final do heap
    heap->data[index].cost = cost;
    heap->data[index].data = data;
    
    // incrementa o tamanho da nossa heap
    heap->size++;

    // Realiza o heapify-up para ajustar a posição do novo elemento no heap
    heapify_up(heap, index);
}

heap_node extract_min(min_heap* heap) {
    // Verifica se o heap está vazio
    if (heap->size == 0) {
        // Retorna um heap_node inválido
        heap_node empty_node = {0, NULL};
        return empty_node;
    }

    // Armazena o valor mínimo
    heap_node min_node = heap->data[0];
    
    // Substitui o valor mínimo pelo último elemento do heap
    heap->data[0] = heap->data[heap->size - 1];
    
    // Reduz o tamanho do heap
    heap->size--;
    
    // Realiza o heapify-down para restaurar as propriedades do heap
    heapify_down(heap, 0);
    
    // Retorna o valor mínimo extraído
    return min_node;
}

void remove_node(min_heap* heap, int cost, void* data) {
    int index = -1;

    // Procura o elemento no heap
    for (int i = 0; i < heap->size; i++) {
        if (heap->data[i].cost == cost && heap->data[i].data == data) {
            index = i;
            break;
        }
    }

    if (index == -1)
        return;

    // Move o último elemento para a posição do elemento a ser removido
    heap->data[index] = heap->data[heap->size - 1];
    heap->size--;

    // Reorganiza o heap
    heapify_up(heap, index);
    heapify_down(heap, index);
}

void update_cost(min_heap* heap, int old_cost, int new_cost, void* data) {
    int index = -1;

    // Procura o elemento no heap
    for (int i = 0; i < heap->size; i++) {
        if (heap->data[i].cost == old_cost && heap->data[i].data == data) {
            index = i;
            break;
        }
    }

    if (index == -1)
        return;

    // Atualiza o custo do elemento
    heap->data[index].cost = new_cost;

    // Reorganiza o heap
    heapify_up(heap, index);
    heapify_down(heap, index);
}