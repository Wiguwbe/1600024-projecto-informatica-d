/*
 * linked_list - Lista Ligada
 *
 * Descrição:
 *   Esta é uma implementação de uma lista ligada em C, que permite armazenar
 *   um número dinâmico de elementos. A lista ligada é uma estrutura de dados
 *   linear em que cada elemento, chamado de nó, contém um valor e um ponteiro
 *   para o próximo nó da lista. A lista é composta por um ponteiro para o
 *   primeiro nó (cabeça) e um ponteiro para o último nó (cauda). O tamanho da
 *   lista é atualizado automaticamente durante as operações de inserção e
 *   remoção.
 *
 * Estrutura:
 *   - linked_list_node_t: Estrutura para representar um nó da lista ligada,
 *     contendo um ponteiro para o próximo nó e um ponteiro genérico para o
 *     dado armazenado.
 *
 *   - linked_list_t: Estrutura para representar a lista ligada, contendo um
 *     ponteiro para o primeiro nó (cabeça), um ponteiro para o último nó
 *     (cauda) e um inteiro para armazenar o tamanho da lista.
 *
 * Funcionalidades:
 *   - linked_list_create: Cria e inicializa uma nova lista ligada vazia.
 *
 *   - linked_list_destroy: Libera a memória alocada para a lista ligada e seus
 *     nós.
 *
 *   - linked_list_append: Adiciona um elemento no final da lista.
 *
 *   - linked_list_insert: Insere um elemento em uma posição específica da lista.
 *
 *   - linked_list_remove: Remove um elemento de uma posição específica da lista.
 *
 *   - linked_list_size: Retorna o tamanho da lista.
 *
 *   - linked_list_get: Retorna o elemento de uma posição específica da lista.
 *
 * Utilização:
 *   1. Inclua o arquivo "linked_list.h" no seu programa.
 *   2. Crie uma nova lista ligada usando a função linked_list_create().
 *   3. Adicione elementos à lista usando as funções linked_list_append() ou
 *      linked_list_insert().
 *   4. Acesse elementos da lista usando a função linked_list_get().
 *   5. Remova elementos da lista usando a função linked_list_remove().
 *   6. Obtenha o tamanho da lista usando a função linked_list_size().
 *   7. Libere a memória alocada para a lista ligada usando a função
 *      linked_list_destroy().
 *
 * Limitações:
 *   - Esta implementação não suporta a inserção ou remoção de elementos em
 *     tempo constante, pois é necessário percorrer a lista para encontrar a
 *     posição desejada.
 *   - Não é possível armazenar valores nulos (NULL) na lista.
 *
 * Considerações:
 *   - Certifique-se de liberar a memória alocada para a lista ligada usando a
 *     função linked_list_destroy() quando não for mais necessária.
 *   - Antes de acessar um elemento da lista usando a função linked_list_get(),
 *     verifique se o índice fornecido está dentro dos limites válidos usando a
 *     função linked_list_size().
 *
 */


#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stddef.h>

// Definição do struct para os nós da lista encadeada
typedef struct linked_list_node {
    struct linked_list_node* next;
    void* data;
} linked_list_node_t;

// Definição do struct para a lista encadeada
typedef struct {
    linked_list_node_t* head;
    linked_list_node_t* tail;
    size_t size;
} linked_list_t;

// Função para criar e inicializar a lista encadeada
linked_list_t* linked_list_create();

// Função para liberar a memória da lista encadeada
void linked_list_destroy(linked_list_t* list);

// Função para adicionar um elemento no final da lista encadeada
void linked_list_append(linked_list_t* list, void* data);

// Função para inserir um elemento em uma posição específica da lista encadeada
void linked_list_insert(linked_list_t* list, void* data, size_t index);

// Função para remover um elemento de uma posição específica da lista encadeada
void linked_list_remove(linked_list_t* list, size_t index);

// Função para obter o tamanho da lista encadeada
size_t linked_list_size(linked_list_t* list);

// Função para obter um elemento de uma posição específica da lista encadeada
void* linked_list_get(linked_list_t* list, size_t index);

#endif  // LINKED_LIST_H
