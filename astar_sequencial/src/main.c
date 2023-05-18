#include <stdio.h>
#include "min_heap.h"

// Função de comparação para ordenar os elementos do heap pelo custo
int compare(const void* a, const void* b) {
    const heap_node* node_a = (const heap_node*)a;
    const heap_node* node_b = (const heap_node*)b;
    return node_a->cost - node_b->cost;
}

int main() {
    // Cria um novo min-heap
    min_heap* heap = create_min_heap();
    
    // Insere alguns elementos no heap
    int cost1 = 5;
    int cost2 = 10;
    int cost3 = 3;
    int cost4 = 2;
    
    insert(heap, cost1, NULL);
    insert(heap, cost2, NULL);
    insert(heap, cost3, NULL);
    insert(heap, cost4, NULL);
    
    // Extrai e imprime o elemento de custo mínimo do heap
    heap_node min_node = extract_min(heap);
    printf("Custo mínimo: %d\n", min_node.cost);
    
    // Destroi o min-heap e libera a memória
    destroy_min_heap(heap);
    
    return 0;
}
