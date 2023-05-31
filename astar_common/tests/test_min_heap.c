#include "min_heap.h"
#include <check.h>
#include <stdlib.h>

// Caso de teste: Testar a inserção de elementos no min-heap
START_TEST(test_insert)
{
  // Criar um novo min-heap
  min_heap_t* heap = min_heap_create();

  // Inserir elementos no heap
  min_heap_insert(heap, 5, NULL);
  min_heap_insert(heap, 10, NULL);
  min_heap_insert(heap, 3, NULL);

  // Verificar o tamanho do heap após a inserção de elementos
  ck_assert_int_eq(heap->size, 3);

  // Verificar a propriedade do min-heap
  ck_assert(heap->data[0].cost <= heap->data[1].cost);
  ck_assert(heap->data[0].cost <= heap->data[2].cost);

  // Destruir o min-heap
  min_heap_destroy(heap);
}
END_TEST

// Caso de teste: Testar a extração do elemento mínimo do min-heap
START_TEST(test_extract_min)
{
  // Criar um novo min-heap
  min_heap_t* heap = min_heap_create();

  // Inserir elementos no heap
  min_heap_insert(heap, 5, NULL);
  min_heap_insert(heap, 10, NULL);
  min_heap_insert(heap, 3, NULL);

  // Extrair o elemento mínimo do heap
  heap_node_t min_node = min_heap_pop(heap);

  // Verificar o elemento mínimo extraído
  ck_assert_int_eq(min_node.cost, 3);

  // Verificar o tamanho do heap após a extração
  ck_assert_int_eq(heap->size, 2);

  // Destruir o min-heap
  min_heap_destroy(heap);
}
END_TEST

// Teste para remover um nó específico do heap
START_TEST(test_remove_node)
{
  min_heap_t* heap = min_heap_create();

  // Insere elementos no heap
  min_heap_insert(heap, 5, NULL);
  min_heap_insert(heap, 10, NULL);
  min_heap_insert(heap, 3, NULL);

  // Remove um nó específico do heap
  min_heap_remove(heap, 10, NULL);

  // Verifica se o nó foi removido corretamente
  ck_assert_int_eq(heap->size, 2);

  // Extrai o elemento mínimo do heap
  heap_node_t min_node = min_heap_pop(heap);
  ck_assert_int_eq(min_node.cost, 3);

  min_heap_destroy(heap);
}
END_TEST

// Teste para atualizar o custo de um nó no heap
START_TEST(test_update_cost)
{
  min_heap_t* heap = min_heap_create();

  // Insere elementos no heap
  min_heap_insert(heap, 5, NULL);
  min_heap_insert(heap, 10, NULL);
  min_heap_insert(heap, 3, NULL);

  // Atualiza o custo de um nó no heap
  min_heap_update(heap, 10, 2, NULL);

  // Extrai o elemento mínimo do heap após a atualização
  heap_node_t min_node = min_heap_pop(heap);
  ck_assert_int_eq(min_node.cost, 2);

  min_heap_destroy(heap);
}
END_TEST

// Criação do conjunto de testes
Suite* min_heap_suite(void)
{
  Suite* suite = suite_create("min_heap_t");

  // Adicionar casos de teste ao conjunto
  TCase* tc_insert = tcase_create("min_heap_insert");
  tcase_add_test(tc_insert, test_insert);
  suite_add_tcase(suite, tc_insert);

  TCase* tc_extract_min = tcase_create("min_heap_pop");
  tcase_add_test(tc_extract_min, test_extract_min);
  suite_add_tcase(suite, tc_extract_min);

  TCase* tc_remove_node = tcase_create("min_heap_remove");
  tcase_add_test(tc_remove_node, test_remove_node);
  suite_add_tcase(suite, tc_remove_node);

  TCase* tc_update_node = tcase_create("update_node");
  tcase_add_test(tc_update_node, test_update_cost);
  suite_add_tcase(suite, tc_update_node);

  return suite;
}

// Execução dos testes
int main(void)
{
  Suite* suite = min_heap_suite();
  SRunner* runner = srunner_create(suite);

  // Executar os testes
  srunner_run_all(runner, CK_NORMAL);

  int num_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  // Retornar o número de testes falhados como código de saída
  return num_failed;
}
