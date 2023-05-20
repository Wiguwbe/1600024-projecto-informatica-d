#include "min_heap.h"
#include <stdio.h>

int main()
{
  // Cria um novo min-heap
  min_heap_t* heap = min_heap_create();

  // Insere alguns elementos no heap
  int cost1 = 5;
  int cost2 = 10;
  int cost3 = 3;
  int cost4 = 2;

  min_heap_insert(heap, cost1, NULL);
  min_heap_insert(heap, cost2, NULL);
  min_heap_insert(heap, cost3, NULL);
  min_heap_insert(heap, cost4, NULL);

  // Extrai e imprime o elemento de custo mínimo do heap
  heap_node_t min_node = min_heap_pop(heap);
  printf("Custo mínimo: %d\n", min_node.cost);

  // Destroi o min-heap e liberta a memória
  min_heap_destroy(heap);

  return 0;
}
