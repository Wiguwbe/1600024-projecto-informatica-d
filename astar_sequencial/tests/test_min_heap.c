#include <stdlib.h>
#include <check.h>
#include "min_heap.h"

struct min_heap {
    heap_node* data;
    int capacity;
    int size;
};

// Configuração do ambiente de teste
void setup(void) {
    // Inicializar recursos necessários para os testes
}

// Limpeza do ambiente de teste
void teardown(void) {
    // Liberar quaisquer recursos alocados durante os testes
}

// Caso de teste: Testar a inserção de elementos no min-heap
START_TEST(test_insert) {
    // Criar um novo min-heap
    min_heap* heap = create_min_heap();
    
    // Inserir elementos no heap
    insert(heap, 5, NULL);
    insert(heap, 10, NULL);
    insert(heap, 3, NULL);
    
    // Verificar o tamanho do heap após a inserção de elementos
    ck_assert_int_eq(heap->size, 3);
    
    // Verificar a propriedade do min-heap
    ck_assert(heap->data[0].cost <= heap->data[1].cost);
    ck_assert(heap->data[0].cost <= heap->data[2].cost);
    
    // Destruir o min-heap
    destroy_min_heap(heap);
}
END_TEST

// Caso de teste: Testar a extração do elemento mínimo do min-heap
START_TEST(test_extract_min) {
    // Criar um novo min-heap
    min_heap* heap = create_min_heap();
    
    // Inserir elementos no heap
    insert(heap, 5, NULL);
    insert(heap, 10, NULL);
    insert(heap, 3, NULL);
    
    // Extrair o elemento mínimo do heap
    heap_node min_node = extract_min(heap);
    
    // Verificar o elemento mínimo extraído
    ck_assert_int_eq(min_node.cost, 3);
    
    // Verificar o tamanho do heap após a extração
    ck_assert_int_eq(heap->size, 2);
    
    // Destruir o min-heap
    destroy_min_heap(heap);
}
END_TEST

// Teste para remover um nó específico do heap
START_TEST(test_remove_node) {
    min_heap* heap = create_min_heap();

    // Insere elementos no heap
    insert(heap, 5, NULL);
    insert(heap, 10, NULL);
    insert(heap, 3, NULL);

    // Remove um nó específico do heap
    remove_node(heap, 10, NULL);

    // Verifica se o nó foi removido corretamente
    ck_assert_int_eq(heap->size, 2);

    // Extrai o elemento mínimo do heap
    heap_node min_node = extract_min(heap);
    ck_assert_int_eq(min_node.cost, 3);

    destroy_min_heap(heap);
}
END_TEST

// Teste para atualizar o custo de um nó no heap
START_TEST(test_update_cost) {
    min_heap* heap = create_min_heap();

    // Insere elementos no heap
    insert(heap, 5, NULL);
    insert(heap, 10, NULL);
    insert(heap, 3, NULL);

    // Atualiza o custo de um nó no heap
    update_cost(heap, 10, 2, NULL);

    // Extrai o elemento mínimo do heap após a atualização
    heap_node min_node = extract_min(heap);
    ck_assert_int_eq(min_node.cost, 2);

    destroy_min_heap(heap);
}
END_TEST

// Criação do conjunto de testes
Suite* min_heap_suite(void) {
    Suite* suite = suite_create("MinHeap");
    
    // Adicionar casos de teste ao conjunto
    TCase* tc_insert = tcase_create("insert");
    tcase_add_checked_fixture(tc_insert, setup, teardown);
    tcase_add_test(tc_insert, test_insert);
    suite_add_tcase(suite, tc_insert);
    
    TCase* tc_extract_min = tcase_create("extract_min");
    tcase_add_checked_fixture(tc_extract_min, setup, teardown);
    tcase_add_test(tc_extract_min, test_extract_min);
    suite_add_tcase(suite, tc_extract_min);

    TCase* tc_remove_node = tcase_create("remove_node");
    tcase_add_checked_fixture(tc_remove_node, setup, teardown);
    tcase_add_test(tc_remove_node, test_remove_node);
    suite_add_tcase(suite, tc_remove_node);

    TCase* tc_update_node = tcase_create("update_node");
    tcase_add_checked_fixture(tc_update_node, setup, teardown);
    tcase_add_test(tc_update_node, test_update_cost);
    suite_add_tcase(suite, tc_update_node);

    return suite;
}

// Execução dos testes
int main(void) {
    Suite* suite = min_heap_suite();
    SRunner* runner = srunner_create(suite);
    
    // Executar os testes
    srunner_run_all(runner, CK_NORMAL);
    
    int num_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    
    // Retornar o número de testes falhados como código de saída
    return num_failed;
}
