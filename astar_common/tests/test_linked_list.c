#include <stdlib.h>
#include <check.h>
#include "linked_list.h"

// Função auxiliar para comparar inteiros
int compare_ints(const void* a, const void* b) {
    int int_a = *((int*)a);
    int int_b = *((int*)b);
    if (int_a < int_b) {
        return -1;
    } else if (int_a > int_b) {
        return 1;
    }
    return 0;
}

START_TEST(test_linked_list_create) {
    linked_list_t* list = linked_list_create();

    // Verifica se a lista foi criada corretamente
    ck_assert_ptr_null(list->head);
    ck_assert_ptr_null(list->tail);
    ck_assert_int_eq(list->size, 0);

    linked_list_destroy(list);
}
END_TEST

START_TEST(test_linked_list_append) {
    linked_list_t* list = linked_list_create();

    int data1 = 10;
    int data2 = 20;

    linked_list_append(list, &data1);

    // Verifica se o primeiro elemento foi adicionado corretamente
    ck_assert_ptr_eq(linked_list_get(list, 0), &data1);
    ck_assert_ptr_eq(list->head, list->tail);
    ck_assert_int_eq(list->size, 1);

    linked_list_append(list, &data2);

    // Verifica se o segundo elemento foi adicionado corretamente
    ck_assert_ptr_eq(linked_list_get(list, 1), &data2);
    ck_assert_ptr_ne(list->head, list->tail);
    ck_assert_int_eq(list->size, 2);

    linked_list_destroy(list);
}
END_TEST

START_TEST(test_linked_list_insert) {
    linked_list_t* list = linked_list_create();

    int data1 = 10;
    int data2 = 20;
    int data3 = 30;

    linked_list_insert(list, &data2, 0);
    linked_list_insert(list, &data3, 0);
    linked_list_insert(list, &data1, 1);

    // Verifica se os elementos foram inseridos corretamente
    ck_assert_ptr_eq(linked_list_get(list, 0), &data3);
    ck_assert_ptr_eq(linked_list_get(list, 1), &data1);
    ck_assert_ptr_eq(linked_list_get(list, 2), &data2);
    ck_assert_int_eq(linked_list_size(list), 3);

    linked_list_destroy(list);
}
END_TEST

START_TEST(test_linked_list_remove) {
    linked_list_t* list = linked_list_create();

    int data1 = 10;
    int data2 = 20;
    int data3 = 30;

    linked_list_append(list, &data1);
    linked_list_append(list, &data2);
    linked_list_append(list, &data3);

    linked_list_remove(list, 1);

    // Verifica se o elemento foi removido corretamente
    ck_assert_ptr_eq(linked_list_get(list, 0), &data1);
    ck_assert_ptr_eq(linked_list_get(list, 1), &data3);
    ck_assert_int_eq(linked_list_size(list), 2);

    linked_list_destroy(list);
}
END_TEST

START_TEST(test_linked_list_size) {
    linked_list_t* list = linked_list_create();

    int data1 = 10;
    int data2 = 20;
    int data3 = 30;

    linked_list_append(list, &data1);
    linked_list_append(list, &data2);
    linked_list_append(list, &data3);

    // Verifica se o tamanho da lista está correto
    ck_assert_int_eq(linked_list_size(list), 3);

    linked_list_destroy(list);
}
END_TEST

START_TEST(test_linked_list_get) {
    linked_list_t* list = linked_list_create();

    int data1 = 10;
    int data2 = 20;
    int data3 = 30;

    linked_list_append(list, &data1);
    linked_list_append(list, &data2);
    linked_list_append(list, &data3);

    // Verifica se os elementos são retornados corretamente
    ck_assert_ptr_eq(linked_list_get(list, 0), &data1);
    ck_assert_ptr_eq(linked_list_get(list, 1), &data2);
    ck_assert_ptr_eq(linked_list_get(list, 2), &data3);

    linked_list_destroy(list);
}
END_TEST

int main() {
    // Cria a suite de testes
    Suite* suite = suite_create("LinkedList");

    // Adiciona os casos de teste à suite
    TCase* tcase = tcase_create("Core");
    tcase_add_test(tcase, test_linked_list_create);
    tcase_add_test(tcase, test_linked_list_append);
    tcase_add_test(tcase, test_linked_list_insert);
    tcase_add_test(tcase, test_linked_list_remove);
    tcase_add_test(tcase, test_linked_list_size);
    tcase_add_test(tcase, test_linked_list_get);
    suite_add_tcase(suite, tcase);

    // Cria um objeto de corrida de teste e executa os testes
    SRunner* runner = srunner_create(suite);
    srunner_run_all(runner, CK_NORMAL);

    // Obtém o número de falhas
    int num_failed = srunner_ntests_failed(runner);

    // Liberta memória
    srunner_free(runner);

    return num_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
