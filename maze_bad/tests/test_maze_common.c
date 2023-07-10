#include "maze_common.h"
#include <check.h>
#include <stdlib.h>
#include <string.h>

START_TEST(test_maze_solver_init_ok)
{
  char initial_board[9] = "X.XX.XX.X";
  char initial_board_expected[9] = "XcXX.XX.X";
  int rows = 3;
  int cols = 3;
  coord entry_coord = { 1, 0 };
  coord exit_coord = { 1, 2 };

  maze_solver_t* maze_solver = maze_solver_init(rows, cols, initial_board);
  ck_assert_ptr_nonnull(maze_solver);
  ck_assert_mem_eq(maze_solver->initial_board, initial_board_expected, rows * cols);
  ck_assert_mem_eq(&(maze_solver->entry_coord), &entry_coord, sizeof(coord));
  ck_assert_mem_eq(&(maze_solver->exit_coord), &exit_coord, sizeof(coord));
  maze_solver_destroy(maze_solver);
}
END_TEST

START_TEST(test_maze_solver_board)
{
  char initial_board[9] = "X.XX.XX.X";
  int rows = 3;
  int cols = 3;
  coord position = { 1, 1 };

  maze_solver_t* maze_solver = maze_solver_init(rows, cols, initial_board);
  ck_assert_ptr_nonnull(maze_solver);
  void* data = maze_solver_create_board(maze_solver, initial_board, position);
  ck_assert_ptr_nonnull(data);
  board_data_t board_data = maze_solver_wrap_board(maze_solver, data);
  ck_assert_mem_eq(board_data.board, initial_board, rows * cols);
  ck_assert_mem_eq(&(board_data.position), &position, sizeof(coord));

  // Neste teste verificamos se ao o maze_solver_create_board
  // retorna a mesma localização de memória caso os dados
  // sejam idênticos
  {
    char another_board[9] = "XxXXxXXxX";
    coord another_position = { 1, 2 };

    ck_assert_ptr_ne(initial_board, another_board);
    ck_assert_ptr_ne(&position, &another_position);

    memcpy(another_board, initial_board, rows * cols);
    memcpy(&another_position, &position, sizeof(coord));

    ck_assert_ptr_ne(initial_board, another_board);
    ck_assert_ptr_ne(&position, &another_position);

    void* another_data = maze_solver_create_board(maze_solver, another_board, another_position);
    ck_assert_ptr_eq(data, another_data);
  }

  maze_solver_destroy(maze_solver);
}
END_TEST

// Função auxiliar para criação da suíte de testes
Suite* create_suite()
{
  Suite* suite = suite_create("maze_common");
  TCase* tcase = tcase_create("Core");
  tcase_add_test(tcase, test_maze_solver_init_ok);
  tcase_add_test(tcase, test_maze_solver_board);
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
