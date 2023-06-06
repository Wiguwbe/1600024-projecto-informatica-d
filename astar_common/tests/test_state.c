#include "state.h"
#include <check.h>
#include <stdlib.h>

typedef struct
{
  int x;
  int y;
} my_struct_t;

START_TEST(test_state_allocator)
{
  state_allocator_t* allocator = state_allocator_create(sizeof(my_struct_t));

  my_struct_t state_data_1 = {2,2};
  my_struct_t state_data_2 = {3,3};
  my_struct_t state_data_3 = {4,4};
  my_struct_t state_data_4 = {5,5};

  my_struct_t state_data_5 = {2,2};
  my_struct_t state_data_6 = {3,3};
  my_struct_t state_data_7 = {4,4};
  my_struct_t state_data_8 = {5,5};

  state_t* state_1 = state_allocator_new(allocator, &state_data_1);
  state_t* state_2 = state_allocator_new(allocator, &state_data_2);
  state_t* state_3 = state_allocator_new(allocator, &state_data_3);
  state_t* state_4 = state_allocator_new(allocator, &state_data_4);

  state_t* state_5 = state_allocator_new(allocator, &state_data_5);
  state_t* state_6 = state_allocator_new(allocator, &state_data_6);
  state_t* state_7 = state_allocator_new(allocator, &state_data_7);
  state_t* state_8 = state_allocator_new(allocator, &state_data_8);

  ck_assert_ptr_eq(state_1, state_5);
  ck_assert_int_eq(((my_struct_t*)state_1->data)->x, 2);
  ck_assert_int_eq(((my_struct_t*)state_1->data)->y, 2);
  ck_assert_int_eq(((my_struct_t*)state_5->data)->x, 2);
  ck_assert_int_eq(((my_struct_t*)state_5->data)->y, 2);

  ck_assert_ptr_eq(state_2, state_6);
  ck_assert_int_eq(((my_struct_t*)state_2->data)->x, 3);
  ck_assert_int_eq(((my_struct_t*)state_2->data)->y, 3);
  ck_assert_int_eq(((my_struct_t*)state_6->data)->x, 3);
  ck_assert_int_eq(((my_struct_t*)state_6->data)->y, 3);

  ck_assert_ptr_eq(state_3, state_7);
  ck_assert_int_eq(((my_struct_t*)state_3->data)->x, 4);
  ck_assert_int_eq(((my_struct_t*)state_3->data)->y, 4);
  ck_assert_int_eq(((my_struct_t*)state_7->data)->x, 4);
  ck_assert_int_eq(((my_struct_t*)state_7->data)->y, 4);

  ck_assert_ptr_eq(state_4, state_8);
  ck_assert_int_eq(((my_struct_t*)state_4->data)->x, 5);
  ck_assert_int_eq(((my_struct_t*)state_4->data)->y, 5);
  ck_assert_int_eq(((my_struct_t*)state_8->data)->x, 5);
  ck_assert_int_eq(((my_struct_t*)state_8->data)->y, 5);

  state_allocator_destroy(allocator);
}
END_TEST

Suite* allocator_suite()
{
  Suite* suite = suite_create("state_allocator_t");
  TCase* test_case = tcase_create("state allocation");

  tcase_add_test(test_case, test_state_allocator);

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
