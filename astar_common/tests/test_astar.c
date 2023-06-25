#include "astar.h"
#include <check.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
  int x;
  int y;
} my_struct_t;

START_TEST(test_astar)
{
  state_allocator_t* allocator = state_allocator_create(sizeof(my_struct_t));

  a_star_t* a_star = a_star_create(sizeof(my_struct_t), NULL, NULL, NULL, NULL, NULL);

  my_struct_t state_data_1 = { 2, 2 };
  my_struct_t state_data_2 = { 3, 3 };
  my_struct_t state_data_3 = { 4, 4 };
  my_struct_t state_data_4 = { 5, 5 };

  state_t* state_1 = state_allocator_new(allocator, &state_data_1);
  state_t* state_2 = state_allocator_new(allocator, &state_data_2);
  state_t* state_3 = state_allocator_new(allocator, &state_data_3);
  state_t* state_4 = state_allocator_new(allocator, &state_data_4);

  a_star_node_t* node_1 = node_allocator_new(a_star->node_allocator, state_1);
  a_star_node_t* node_2 = node_allocator_new(a_star->node_allocator, state_2);
  a_star_node_t* node_3 = node_allocator_new(a_star->node_allocator, state_3);
  a_star_node_t* node_4 = node_allocator_new(a_star->node_allocator, state_4);

  my_struct_t state_data_5 = { state_data_2.x - 1, state_data_2.x - 1 };
  my_struct_t state_data_6 = { state_data_1.x + 1, state_data_1.x + 1 };
  my_struct_t state_data_7 = { state_data_4.x - 1, state_data_4.x - 1 };
  my_struct_t state_data_8 = { state_data_3.x + 1, state_data_3.x + 1 };

  state_t* state_5 = state_allocator_new(allocator, &state_data_5);
  state_t* state_6 = state_allocator_new(allocator, &state_data_6);
  state_t* state_7 = state_allocator_new(allocator, &state_data_7);
  state_t* state_8 = state_allocator_new(allocator, &state_data_8);

  a_star_node_t* child_node_1 = node_allocator_get(a_star->node_allocator, state_5);
  a_star_node_t* child_node_2 = node_allocator_get(a_star->node_allocator, state_6);
  a_star_node_t* child_node_3 = node_allocator_get(a_star->node_allocator, state_7);
  a_star_node_t* child_node_4 = node_allocator_get(a_star->node_allocator, state_8);

  ck_assert_ptr_eq(child_node_1, node_1);
  ck_assert_int_eq(((my_struct_t*)child_node_1->state->data)->x, 2);
  ck_assert_int_eq(((my_struct_t*)child_node_1->state->data)->y, 2);
  ck_assert_int_eq(((my_struct_t*)child_node_1->state->data)->x, 2);
  ck_assert_int_eq(((my_struct_t*)child_node_1->state->data)->y, 2);

  ck_assert_ptr_eq(child_node_2, node_2);
  ck_assert_int_eq(((my_struct_t*)child_node_2->state->data)->x, 3);
  ck_assert_int_eq(((my_struct_t*)child_node_2->state->data)->y, 3);
  ck_assert_int_eq(((my_struct_t*)child_node_2->state->data)->x, 3);
  ck_assert_int_eq(((my_struct_t*)child_node_2->state->data)->y, 3);

  ck_assert_ptr_eq(child_node_3, node_3);
  ck_assert_int_eq(((my_struct_t*)child_node_3->state->data)->x, 4);
  ck_assert_int_eq(((my_struct_t*)child_node_3->state->data)->y, 4);
  ck_assert_int_eq(((my_struct_t*)child_node_3->state->data)->x, 4);
  ck_assert_int_eq(((my_struct_t*)child_node_3->state->data)->y, 4);

  ck_assert_ptr_eq(child_node_4, node_4);
  ck_assert_int_eq(((my_struct_t*)child_node_4->state->data)->x, 5);
  ck_assert_int_eq(((my_struct_t*)child_node_4->state->data)->y, 5);
  ck_assert_int_eq(((my_struct_t*)child_node_4->state->data)->x, 5);
  ck_assert_int_eq(((my_struct_t*)child_node_4->state->data)->y, 5);

  state_allocator_destroy(allocator);
  a_star_destroy(a_star);
}
END_TEST

Suite* allocator_suite()
{
  Suite* suite = suite_create("astar_t");
  TCase* test_case = tcase_create("astar test");

  tcase_add_test(test_case, test_astar);

  suite_add_tcase(suite, test_case);

  return suite;
}

int main()
{
  Suite* suite = allocator_suite();
  SRunner* runner = srunner_create(suite);

  srunner_run_all(runner, CK_NORMAL);
  int num_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return (num_failed == 0) ? 0 : 1;
}
