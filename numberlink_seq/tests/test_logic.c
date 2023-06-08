#include "linked_list.h"
#include "logic.h"
#include "numberlink.h"
#include "state.h"
#include <check.h>
#include <stdlib.h>

// Teste unitário para a função visit
START_TEST(test_visit_case_1)
{
  int rows = 3;
  int cols = 3;
  char initial_board[9] = "A.AB.BC.C";

  char neighbor_1_board[9] = "AaAB.BC.C";
  coord neighbor_1_coords[3] = { { 1, 0 }, { 0, 1 }, { 0, 2 } };

  char neighbor_2_board[9] = "A.ABbBC.C";
  coord neighbor_2_coords[3] = { { 0, 0 }, { 1, 1 }, { 0, 2 } };

  char neighbor_3_board[9] = "A.AB.BCcC";
  coord neighbor_3_coords[3] = { { 0, 0 }, { 0, 1 }, { 1, 2 } };

  number_link_t* number_link = number_link_init(rows, cols, initial_board);

  state_allocator_t* allocator = state_allocator_create(sizeof(number_link_state_t));

  number_link_state_t initial_state = {
    number_link, number_link_create_board(number_link, number_link->initial_board, number_link->initial_coords), 0
  };

  // Criação da lista ligada de vizinhos
  linked_list_t* neighbors = linked_list_create();

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  // Chamada da função visit para expandir o estado inicial
  visit(initial_state_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  size_t num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 3);

  state_t* neighbor_1 = linked_list_get(neighbors, 0);
  number_link_state_t* neighbor_1_state = (number_link_state_t*)neighbor_1->data;
  board_data_t neighbor_1_board_data = number_link_wrap_board(neighbor_1_state->number_link, neighbor_1_state->board_data);
  ck_assert_mem_eq(neighbor_1_board_data.board, &neighbor_1_board, rows * cols);
  ck_assert_mem_eq(neighbor_1_board_data.coords, &neighbor_1_coords, number_link->num_pairs * sizeof(coord));
  ck_assert_int_eq(neighbor_1_state->matched_pairs, 0);

  state_t* neighbor_2 = linked_list_get(neighbors, 1);
  number_link_state_t* neighbor_2_state = (number_link_state_t*)neighbor_2->data;
  board_data_t neighbor_2_board_data = number_link_wrap_board(neighbor_2_state->number_link, neighbor_2_state->board_data);
  ck_assert_mem_eq(neighbor_2_board_data.board, &neighbor_2_board, rows * cols);
  ck_assert_mem_eq(neighbor_2_board_data.coords, &neighbor_2_coords, number_link->num_pairs * sizeof(coord));
  ck_assert_int_eq(neighbor_2_state->matched_pairs, 0);

  state_t* neighbor_3 = linked_list_get(neighbors, 2);
  number_link_state_t* neighbor_3_state = (number_link_state_t*)neighbor_3->data;
  board_data_t neighbor_3_board_data = number_link_wrap_board(neighbor_3_state->number_link, neighbor_3_state->board_data);
  ck_assert_mem_eq(neighbor_3_board_data.board, &neighbor_3_board, rows * cols);
  ck_assert_mem_eq(neighbor_3_board_data.coords, &neighbor_3_coords, number_link->num_pairs * sizeof(coord));
  ck_assert_int_eq(neighbor_3_state->matched_pairs, 0);

  // Liberta a memória utilizada
  linked_list_destroy(neighbors);
  state_allocator_destroy(allocator);
  number_link_destroy(number_link);
}
END_TEST

START_TEST(test_visit_case_2)
{
  int rows = 3;
  int cols = 3;
  char initial_board[9] = "ABC...ABC";

  char neighbor_1_board[9] = "ABCa..ABC";
  coord neighbor_1_coords[3] = { { 0, 1 }, { 1, 0 }, { 2, 0 } };

  char neighbor_2_board[9] = "ABC.b.ABC";
  coord neighbor_2_coords[3] = { { 0, 0 }, { 1, 1 }, { 2, 0 } };

  char neighbor_3_board[9] = "ABC..cABC";
  coord neighbor_3_coords[3] = { { 0, 0 }, { 1, 0 }, { 2, 1 } };

  number_link_t* number_link = number_link_init(rows, cols, initial_board);

  state_allocator_t* allocator = state_allocator_create(sizeof(number_link_state_t));

  number_link_state_t initial_state = {
    number_link, number_link_create_board(number_link, number_link->initial_board, number_link->initial_coords), 0
  };

  // Criação da lista ligada de vizinhos
  linked_list_t* neighbors = linked_list_create();

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  // Chamada da função visit para expandir o estado inicial
  visit(initial_state_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  size_t num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 3);

  state_t* neighbor_1 = linked_list_get(neighbors, 0);
  number_link_state_t* neighbor_1_state = (number_link_state_t*)neighbor_1->data;
  board_data_t neighbor_1_board_data = number_link_wrap_board(neighbor_1_state->number_link, neighbor_1_state->board_data);
  ck_assert_mem_eq(neighbor_1_board_data.board, &neighbor_1_board, rows * cols);
  ck_assert_mem_eq(neighbor_1_board_data.coords, &neighbor_1_coords, number_link->num_pairs * sizeof(coord));
  ck_assert_int_eq(neighbor_1_state->matched_pairs, 0);

  state_t* neighbor_2 = linked_list_get(neighbors, 1);
  number_link_state_t* neighbor_2_state = (number_link_state_t*)neighbor_2->data;
  board_data_t neighbor_2_board_data = number_link_wrap_board(neighbor_2_state->number_link, neighbor_2_state->board_data);
  ck_assert_mem_eq(neighbor_2_board_data.board, &neighbor_2_board, rows * cols);
  ck_assert_mem_eq(neighbor_2_board_data.coords, &neighbor_2_coords, number_link->num_pairs * sizeof(coord));
  ck_assert_int_eq(neighbor_2_state->matched_pairs, 0);

  state_t* neighbor_3 = linked_list_get(neighbors, 2);
  number_link_state_t* neighbor_3_state = (number_link_state_t*)neighbor_3->data;
  board_data_t neighbor_3_board_data = number_link_wrap_board(neighbor_3_state->number_link, neighbor_3_state->board_data);
  ck_assert_mem_eq(neighbor_3_board_data.board, &neighbor_3_board, rows * cols);
  ck_assert_mem_eq(neighbor_3_board_data.coords, &neighbor_3_coords, number_link->num_pairs * sizeof(coord));
  ck_assert_int_eq(neighbor_3_state->matched_pairs, 0);

  // Liberta a memória utilizada
  linked_list_destroy(neighbors);
  state_allocator_destroy(allocator);
  number_link_destroy(number_link);
}

END_TEST

START_TEST(test_visit_case_3) { }
END_TEST

START_TEST(test_visit_case_4) { }
END_TEST

START_TEST(test_visit_case_5) { }
END_TEST

START_TEST(test_goal) { }
END_TEST

START_TEST(test_distance) { }
END_TEST

// Teste unitário para a função heuristic
START_TEST(test_heuristic) { }
END_TEST

// Função auxiliar para criação da suíte de testes
Suite* create_suite()
{
  Suite* suite = suite_create("number_link_logic");
  TCase* tcase = tcase_create("Core");
  tcase_add_test(tcase, test_visit_case_1);
  tcase_add_test(tcase, test_visit_case_2);
  tcase_add_test(tcase, test_visit_case_3);
  tcase_add_test(tcase, test_visit_case_4);
  tcase_add_test(tcase, test_visit_case_5);
  tcase_add_test(tcase, test_goal);
  tcase_add_test(tcase, test_distance);
  tcase_add_test(tcase, test_heuristic);
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
