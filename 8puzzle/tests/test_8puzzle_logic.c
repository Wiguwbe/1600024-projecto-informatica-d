#include "linked_list.h"
#include "8puzzle_logic.h"
#include "state.h"
#include <check.h>
#include <stdlib.h>

// Teste unitário para a função visit
START_TEST(test_visit_case_1)
{
  // Criação do estado inicial do puzzle
  puzzle_state initial_state = { { { '1', '2', '3' }, { '4', '5', '6' }, { '7', '8', '-' } } };
  
  // Espaço moveu para cima
  puzzle_state expected_1 = { { { '1', '2', '3' }, { '4', '5', '-' }, { '7', '8', '6' } } };
  
  // Espaço moveu para a esquerda
  puzzle_state expected_2 = { { { '1', '2', '3' }, { '4', '5', '6' }, { '7', '-', '8' } } };

  // Criação do alocador de estados
  state_allocator_t* allocator = state_allocator_create(sizeof(puzzle_state));

  // Criação da lista ligada de vizinhos
  linked_list_t* neighbors = linked_list_create();

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  // Chamada da função visit para expandir o estado inicial
  visit(initial_state_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  size_t num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 2);

  // Verificação dos vizinhos gerados
  state_t* neighbor1 = linked_list_get(neighbors, 0);
  state_t* neighbor2 = linked_list_get(neighbors, 1);

  puzzle_state* neighbor1_puzzle = (puzzle_state*)(neighbor1->data);
  puzzle_state* neighbor2_puzzle = (puzzle_state*)(neighbor2->data);

  // Verificação do vizinho 1
  ck_assert(memcmp(neighbor1_puzzle, &expected_1, sizeof(puzzle_state)) == 0);

  // Verificação do vizinho 2
  ck_assert(memcmp(neighbor2_puzzle, &expected_2, sizeof(puzzle_state)) == 0);

  // Liberta a memória utilizada
  linked_list_destroy(neighbors);
  state_allocator_destroy(allocator);
}
END_TEST

START_TEST(test_visit_case_2)
{
  // Criação do estado inicial do puzzle
  puzzle_state initial_state = { { { '1', '2', '3' }, { '4', '-', '5' }, { '6', '7', '8' } } };
  
  // Espaço moveu para cima
  puzzle_state expected_1 = { { { '1', '-', '3' }, { '4', '2', '5' }, { '6', '7', '8' } } };
  
  // Espaço moveu para baixo
  puzzle_state expected_2 = { { { '1', '2', '3' }, { '4', '7', '5' }, { '6', '-', '8' } } };
  
  // Espaço moveu para a esquerda
  puzzle_state expected_3 = { { { '1', '2', '3' }, { '-', '4', '5' }, { '6', '7', '8' } } };
  
  // Espaço moveu para a direita
  puzzle_state expected_4 = { { { '1', '2', '3' }, { '4', '5', '-' }, { '6', '7', '8' } } };

  // Criação do alocador de estados
  state_allocator_t* allocator = state_allocator_create(sizeof(puzzle_state));

  // Criação da lista ligada de vizinhos
  linked_list_t* neighbors = linked_list_create();

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  // Chamada da função visit para expandir o estado inicial
  visit(initial_state_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  size_t num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 4);

  // Verificação dos vizinhos gerados
  state_t* neighbor1 = linked_list_get(neighbors, 0);
  state_t* neighbor2 = linked_list_get(neighbors, 1);
  state_t* neighbor3 = linked_list_get(neighbors, 2);
  state_t* neighbor4 = linked_list_get(neighbors, 3);

  puzzle_state* neighbor1_puzzle = (puzzle_state*)(neighbor1->data);
  puzzle_state* neighbor2_puzzle = (puzzle_state*)(neighbor2->data);
  puzzle_state* neighbor3_puzzle = (puzzle_state*)(neighbor3->data);
  puzzle_state* neighbor4_puzzle = (puzzle_state*)(neighbor4->data);

  // Verificação do vizinho 1
  ck_assert(memcmp(neighbor1_puzzle, &expected_1, sizeof(puzzle_state)) == 0);

  // Verificação do vizinho 2
  ck_assert(memcmp(neighbor2_puzzle, &expected_2, sizeof(puzzle_state)) == 0);

  // Verificação do vizinho 3
  ck_assert(memcmp(neighbor3_puzzle, &expected_3, sizeof(puzzle_state)) == 0);

  // Verificação do vizinho 4
  ck_assert(memcmp(neighbor4_puzzle, &expected_4, sizeof(puzzle_state)) == 0);

  // Liberta a memória utilizada
  linked_list_destroy(neighbors);
  state_allocator_destroy(allocator);
}
END_TEST

START_TEST(test_visit_case_3)
{
  // Criação do estado inicial do puzzle
  puzzle_state initial_state = { { { '-', '1', '2' }, { '3', '4', '5' }, { '6', '7', '8' } } };
  
  // Espaço moveu para baixo
  puzzle_state expected_1 = { { { '3', '1', '2' }, { '-', '4', '5' }, { '6', '7', '8' } } };
  
  // Espaço moveu para a direita
  puzzle_state expected_2 = { { { '1', '-', '2' }, { '3', '4', '5' }, { '6', '7', '8' } } };

  // Criação do alocador de estados
  state_allocator_t* allocator = state_allocator_create(sizeof(puzzle_state));

  // Criação da lista ligada de vizinhos
  linked_list_t* neighbors = linked_list_create();

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  // Chamada da função visit para expandir o estado inicial
  visit(initial_state_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  size_t num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 2);

  // Verificação dos vizinhos gerados
  state_t* neighbor1 = linked_list_get(neighbors, 0);
  state_t* neighbor2 = linked_list_get(neighbors, 1);

  puzzle_state* neighbor1_puzzle = (puzzle_state*)(neighbor1->data);
  puzzle_state* neighbor2_puzzle = (puzzle_state*)(neighbor2->data);

  // Verificação do vizinho 1
  ck_assert(memcmp(neighbor1_puzzle, &expected_1, sizeof(puzzle_state)) == 0);

  // Verificação do vizinho 2
  ck_assert(memcmp(neighbor2_puzzle, &expected_2, sizeof(puzzle_state)) == 0);

  // Liberta a memória utilizada
  linked_list_destroy(neighbors);
  state_allocator_destroy(allocator);
}
END_TEST

START_TEST(test_visit_case_4)
{
  // Criação do estado inicial do puzzle
  puzzle_state initial_state = { { { '1', '2', '-' }, { '3', '4', '5' }, { '6', '7', '8' } } };
  
  // Espaço moveu para baixo
  puzzle_state expected_1 = { { { '1', '2', '5' }, { '3', '4', '-' }, { '6', '7', '8' } } };
  
  // Espaço moveu para a esquerda
  puzzle_state expected_2 = { { { '1', '-', '2' }, { '3', '4', '5' }, { '6', '7', '8' } } };

  // Criação do alocador de estados
  state_allocator_t* allocator = state_allocator_create(sizeof(puzzle_state));

  // Criação da lista ligada de vizinhos
  linked_list_t* neighbors = linked_list_create();

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  // Chamada da função visit para expandir o estado inicial
  visit(initial_state_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  size_t num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 2);

  // Verificação dos vizinhos gerados
  state_t* neighbor1 = linked_list_get(neighbors, 0);
  state_t* neighbor2 = linked_list_get(neighbors, 1);

  puzzle_state* neighbor1_puzzle = (puzzle_state*)(neighbor1->data);
  puzzle_state* neighbor2_puzzle = (puzzle_state*)(neighbor2->data);

  // Verificação do vizinho 1
  ck_assert(memcmp(neighbor1_puzzle, &expected_1, sizeof(puzzle_state)) == 0);

  // Verificação do vizinho 2
  ck_assert(memcmp(neighbor2_puzzle, &expected_2, sizeof(puzzle_state)) == 0);

  // Liberta a memória utilizada
  linked_list_destroy(neighbors);
  state_allocator_destroy(allocator);
}
END_TEST

START_TEST(test_visit_case_5)
{
  // Criação do estado inicial do puzzle
  puzzle_state initial_state = { { { '1', '2', '3' }, { '3', '4', '5' }, { '-', '7', '8' } } };
  
  // Espaço moveu para cima
  puzzle_state expected_1 = { { { '1', '2', '3' }, { '-', '4', '5' }, { '3', '7', '8' } } };
  
  // Espaço moveu para a direita
  puzzle_state expected_2 = { { { '1', '2', '3' }, { '3', '4', '5' }, { '7', '-', '8' } } };

  // Criação do alocador de estados
  state_allocator_t* allocator = state_allocator_create(sizeof(puzzle_state));

  // Criação da lista ligada de vizinhos
  linked_list_t* neighbors = linked_list_create();

  // Criação do estado inicial usando o alocador
  state_t* initial_state_ptr = state_allocator_new(allocator, &initial_state);

  // Chamada da função visit para expandir o estado inicial
  visit(initial_state_ptr, allocator, neighbors);

  // Verificação do tamanho da lista de vizinhos
  size_t num_neighbors = linked_list_size(neighbors);
  ck_assert_int_eq(num_neighbors, 2);

  // Verificação dos vizinhos gerados
  state_t* neighbor1 = linked_list_get(neighbors, 0);
  state_t* neighbor2 = linked_list_get(neighbors, 1);

  puzzle_state* neighbor1_puzzle = (puzzle_state*)(neighbor1->data);
  puzzle_state* neighbor2_puzzle = (puzzle_state*)(neighbor2->data);

  // Verificação do vizinho 1
  ck_assert(memcmp(neighbor1_puzzle, &expected_1, sizeof(puzzle_state)) == 0);

  // Verificação do vizinho 2
  ck_assert(memcmp(neighbor2_puzzle, &expected_2, sizeof(puzzle_state)) == 0);

  // Liberta a memória utilizada
  linked_list_destroy(neighbors);
  state_allocator_destroy(allocator);
}
END_TEST

START_TEST(test_goal)
{
  puzzle_state ok_state_data = { { { '1', '2', '3' }, { '4', '-', '5' }, { '6', '7', '8' } } };
  puzzle_state nok_state_data = { { { '1', '2', '3' }, { '-', '4', '5' }, { '3', '7', '8' } } };

  state_t ok_state = {sizeof(puzzle_state), &ok_state_data};
  state_t nok_state = {sizeof(puzzle_state), &nok_state_data};

  ck_assert(!goal(&nok_state, NULL));
  ck_assert(!goal(&ok_state, NULL));
}
END_TEST

START_TEST(test_distance)
{
  // Criação do estado inicial do puzzle
  puzzle_state ok_state_data = { { { '1', '2', '3' }, { '4', '-', '5' }, { '6', '7', '8' } } };
  puzzle_state nok_state_data = { { { '1', '2', '3' }, { '-', '4', '5' }, { '3', '7', '8' } } };

  state_t ok_state = {sizeof(puzzle_state), &ok_state_data};
  state_t nok_state = {sizeof(puzzle_state), &nok_state_data};

  ck_assert(distance(&nok_state, &ok_state) == 1);
}
END_TEST

// Teste unitário para a função heuristic
START_TEST(test_heuristic)
{
    // Criação dos estados de teste
    puzzle_state current_puzzle = {{{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '-'}}};
    puzzle_state goal_puzzle = {{{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '-'}}};
    
    // Criação dos objetos state_t para os estados de teste
    state_t current_state = {sizeof(puzzle_state), &current_puzzle};
    state_t goal_state = {sizeof(puzzle_state), &goal_puzzle};
    
    // Chamada da função heuristic para calcular a heurística
    int h = heuristic(&current_state, &goal_state);
    
    // Verificação do resultado da heurística
    ck_assert_int_eq(h, 0);  // O estado atual é igual ao estado objetivo, portanto, a heurística deve ser 0
    
    // Alteração do estado atual para um estado diferente do objetivo
    current_puzzle.board[2][2] = '3';
    current_puzzle.board[2][0] = '-';
    
    // Chamada da função heuristic novamente
    h = heuristic(&current_state, &goal_state);
    
    // Verificação do resultado da heurística
    ck_assert_int_eq(h, 2);  // O estado atual difere do estado objetivo em 2 peças, portanto, a heurística deve ser 2
}
END_TEST

// Função auxiliar para criação da suíte de testes
Suite* create_suite()
{
  Suite* suite = suite_create("8puzzle_logic");
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
