#include "queue.h"
#include <check.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

START_TEST(test_queue)
{
  queue_t* queue = queue_create();
  ck_assert_msg(queue != NULL, "Falha na criação da fila");

  int* value1 = malloc(sizeof(int));
  *value1 = 10;
  queue_push(queue, value1);

  int* value2 = malloc(sizeof(int));
  *value2 = 20;
  queue_push(queue, value2);

  int* value3 = malloc(sizeof(int));
  *value3 = 30;
  queue_push(queue, value3);

  ck_assert_int_eq(queue_size(queue), 3);

  int* data = queue_pop(queue);
  ck_assert_int_eq(*data, 10);
  free(data);

  ck_assert_int_eq(queue_size(queue), 2);

  data = queue_pop(queue);
  ck_assert_int_eq(*data, 20);
  free(data);

  ck_assert_int_eq(queue_size(queue), 1);

  data = queue_pop(queue);
  ck_assert_int_eq(*data, 30);
  free(data);

  ck_assert_int_eq(queue_size(queue), 0);
  ck_assert(queue_is_empty(queue));

  queue_destroy(queue, false);
}
END_TEST

Suite* queue_suite()
{
  Suite* suite = suite_create("queue_t");
  TCase* tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_queue);
  suite_add_tcase(suite, tc_core);
  return suite;
}

int main()
{
  Suite* suite = queue_suite();
  SRunner* runner = srunner_create(suite);
  srunner_run_all(runner, CK_NORMAL);
  int failures = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
