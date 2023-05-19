#ifndef MIN_HEAP_H
#define MIN_HEAP_H

// Estrutura para representar um nó do heap
typedef struct {
    int cost;
    void* data;
} heap_node_t;

// Estrutura para representar o min-heap
typedef struct {
    heap_node_t* data;
    int capacity;
    int size;
} min_heap_t;

// Cria um novo min-heap
min_heap_t* create_min_heap();

// Destroi o min-heap e libera a memória
void destroy_min_heap(min_heap_t* heap);

// Insere um novo elemento no heap
void insert(min_heap_t* heap, int cost, void* data);

// Extrai e retorna o elemento de custo mínimo do heap
heap_node_t extract_min(min_heap_t* heap);

// Remove um elemento específico do heap
void remove_node(min_heap_t* heap, int cost, void* data);

// Atualiza o custo de um nó específico no heap
void update_cost(min_heap_t* heap, int old_cost, int new_cost, void* data);

#endif
