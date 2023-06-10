#include "numberlink_common.h"
#include <check.h>
#include <stdlib.h>
#include <string.h>

// Teste unitário para a função visit
START_TEST(test_tools)
{
  int zero = letter_to_int('A');
  int one = letter_to_int('B');

  char a = letter_to_lower('A');
  char b = letter_to_lower('B');

  ck_assert_int_eq(zero, 0);
  ck_assert_int_eq(one, 1);

  ck_assert_int_eq(a, 'a');
  ck_assert_int_eq(b, 'b');

  ck_assert_int_eq(number_to_letter(zero), 'A');
  ck_assert_int_eq(number_to_letter(one), 'B');

  ck_assert_int_eq(number_to_lower(zero), 'a');
  ck_assert_int_eq(number_to_lower(one), 'b');
}
END_TEST

START_TEST(test_number_link_init_ok)
{
  char initial_board[9] = "A.AB.BC.C";
  int num_pairs = 3;
  int rows = 3;
  int cols = 3;
  coord starts[3] = { { 0, 0 }, { 0, 1 }, { 0, 2 } };
  coord goals[3] = { { 2, 0 }, { 2, 1 }, { 2, 2 } };

  number_link_t* number_link = number_link_init(rows, cols, initial_board);
  ck_assert_ptr_nonnull(number_link);
  ck_assert_int_eq(number_link->num_pairs, num_pairs);
  ck_assert_mem_eq(number_link->initial_board, initial_board, rows * cols);
  ck_assert_mem_eq(number_link->starts, &starts, num_pairs * sizeof(coord));
  ck_assert_mem_eq(number_link->initial_coords, &starts, num_pairs * sizeof(coord));
  ck_assert_mem_eq(number_link->goals, &goals, num_pairs * sizeof(coord));
  number_link_destroy(number_link);
}
END_TEST

START_TEST(test_number_link_init_fail)
{
  char initial_board[9] = "A.AB.BC..";
  int rows = 3;
  int cols = 3;

  number_link_t* number_link = number_link_init(rows, cols, initial_board);
  ck_assert_ptr_null(number_link);
  number_link_destroy(number_link);
}
END_TEST

START_TEST(test_number_link_board)
{
  char initial_board[9] = "A.AB.BC.C";
  coord coords[3] = { { 1, 0 }, { 0, 1 }, { 0, 2 } };
  int num_pairs = 3;
  int rows = 3;
  int cols = 3;

  number_link_t* number_link = number_link_init(rows, cols, initial_board);
  ck_assert_ptr_nonnull(number_link);
  void* data = number_link_create_board(number_link, initial_board, (const coord*)coords);
  ck_assert_ptr_nonnull(data);
  board_data_t board_data = number_link_wrap_board(number_link, data);
  ck_assert_mem_eq(board_data.board, initial_board, rows * cols);
  ck_assert_mem_eq(board_data.coords, &coords, num_pairs * sizeof(coord));
  ck_assert_int_eq(board_data.coords[0].col, 1);
  ck_assert_int_eq(board_data.coords[0].row, 0);
  ck_assert_int_eq(board_data.coords[1].col, 0);
  ck_assert_int_eq(board_data.coords[1].row, 1);
  ck_assert_int_eq(board_data.coords[2].col, 0);
  ck_assert_int_eq(board_data.coords[2].row, 2);

  // Neste teste verificamos se ao o number_link_create_board 
  // retorna a mesma localização de memória caso os dados
  // sejam idênticos
  {
    char another_board[9] = "AAABABCAC";
    coord another_coords[3] = { { 1, 2 }, { 2, 1 }, { 1, 2 } };

    ck_assert_ptr_ne(initial_board, another_board);
    ck_assert_ptr_ne(&coords, &another_coords);

    memcpy(another_board, initial_board, rows*cols);
    memcpy(&another_coords, &coords, num_pairs * sizeof(coord));

    ck_assert_ptr_ne(initial_board, another_board);
    ck_assert_ptr_ne(&coords, &another_coords);

    void* another_data = number_link_create_board(number_link, another_board, (const coord*)another_coords);
    ck_assert_ptr_eq(data, another_data);
  }

  number_link_destroy(number_link);
}
END_TEST

// Função auxiliar para criação da suíte de testes
Suite* create_suite()
{
  Suite* suite = suite_create("numberlink_common");
  TCase* tcase = tcase_create("Core");
  tcase_add_test(tcase, test_tools);
  tcase_add_test(tcase, test_number_link_init_ok);
  tcase_add_test(tcase, test_number_link_init_fail);
  tcase_add_test(tcase, test_number_link_board);
  suite_add_tcase(suite, tcase);
  return suite;
}

// Função principal de execução dos testes
int main()
{
  Suite* suite = create_suite();
  SRunner* runner = srunner_create(suite);
  srunner_run_all(runner, CK_NORMAL);
  int num_failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
