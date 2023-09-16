#include "maze_logic.h"
#include "state.h"
#include <check.h>
#include <stdlib.h>

// Teste unitário para a função visit
START_TEST(test_visit_case_1)
{
  int rows = 5;
  int cols = 5;
  char initial_board[25] = "X.XXXX.XXXX...XX.X.XXXX.X";
  coord position = { 1, 1 };

  maze_solver_t* maze_solver = maze_solver_init(rows, cols, initial_board);

  state_allocator_t* allocator = state_allocator_create(sizeof(maze_solver_state_t));

  maze_solver_state_t initial_state = {
    maze_solver, position
  };

  // Criação da lista ligada de vizinhos
  linked_list_t* neighbors = linked_list_create();

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  // Chamada da função visit para expandir o estado inicial
  visit(initial_state_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  size_t num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 1);

  coord n1_position = { 1, 2 };

  // Retira o sucessor da lista e remove da lista
  state_t* neighbor_1_ptr = linked_list_get(neighbors, 0);
  linked_list_remove(neighbors,0);

  maze_solver_state_t* n1_state = (maze_solver_state_t*)neighbor_1_ptr->data;
  ck_assert_int_eq(n1_state->position.row, n1_position.row);
  ck_assert_int_eq(n1_state->position.col, n1_position.col);

  // Chamada da função visit para expandir o estado inicial
  visit(neighbor_1_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 3);

  coord n2_position = { 1, 3 };
  coord n3_position = { 2, 2 };

  // Retira o sucessor da lista e remove da lista
  state_t* neighbor_2_ptr = linked_list_get(neighbors, 0);
  maze_solver_state_t* n2_state = (maze_solver_state_t*)neighbor_2_ptr->data;
  ck_assert_int_eq(n2_state->position.row, n2_position.row);
  ck_assert_int_eq(n2_state->position.col, n2_position.col);

  neighbor_1_ptr = linked_list_get(neighbors, 1);
  n1_state = (maze_solver_state_t*)neighbor_1_ptr->data;
  ck_assert_int_eq(n1_state->position.row, position.row);
  ck_assert_int_eq(n1_state->position.col, position.col);

  state_t* neighbor_3_ptr = linked_list_get(neighbors, 2);
  maze_solver_state_t* n3_state = (maze_solver_state_t*)neighbor_3_ptr->data;
  ck_assert_int_eq(n3_state->position.row, n3_position.row);
  ck_assert_int_eq(n3_state->position.col, n3_position.col);

  // Liberta a memória utilizada
  linked_list_destroy(neighbors);
  state_allocator_destroy(allocator);
  maze_solver_destroy(maze_solver);
}
END_TEST

START_TEST(test_goal) { 
  int rows = 5;
  int cols = 5;
  char board[25] = "X.XXXX.XXXX...XX.X.XXXX.X";
  coord position = { 3, 4 };

  maze_solver_t* maze_solver = maze_solver_init(rows, cols, board);

  state_allocator_t* allocator = state_allocator_create(sizeof(maze_solver_state_t));

  maze_solver_state_t initial_state = {
    maze_solver, position
  };

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  bool is_goal = goal(initial_state_ptr,NULL);

  ck_assert(is_goal);

  // Liberta a memória utilizada
  state_allocator_destroy(allocator);
  maze_solver_destroy(maze_solver);  
}
END_TEST

START_TEST(test_distance) { 
  int d = distance(NULL,NULL);
  ck_assert_int_eq(d,1);
}
END_TEST

// Teste unitário para a função heuristic
START_TEST(test_heuristic) { 
  int rows = 5;
  int cols = 5;
  char board[25] = "X.XXXX.XXXX...XX.X.XXXX.X";
  coord position = { 1, 2 };

  maze_solver_t* maze_solver = maze_solver_init(rows, cols, board);

  state_allocator_t* allocator = state_allocator_create(sizeof(maze_solver_state_t));

  maze_solver_state_t initial_state = {
    maze_solver,  position
  };

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  int h = heuristic(initial_state_ptr,NULL);
  ck_assert_int_eq(h,2);

  // Liberta a memória utilizada
  state_allocator_destroy(allocator);
  maze_solver_destroy(maze_solver);    

}
END_TEST

// Função auxiliar para criação da suíte de testes
Suite* create_suite()
{
  Suite* suite = suite_create("maze_solver_logic");
  TCase* tcase = tcase_create("Core");
  tcase_add_test(tcase, test_visit_case_1);
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
