#include "hashtable.h"
#include <check.h>
#include <stdbool.h>
#include <stdlib.h>

// Struct de exemplo
typedef struct
{
  int id;
  char name[20];
} Person;

// Teste básico da hashtable
START_TEST(test_hashtable)
{
  // Inicializa a hashtable
  hashtable_t* hashtable = hashtable_create(sizeof(Person));

  // Cria algumas pessoas
  Person person1 = { 1, "Alice" };
  Person person2 = { 2, "Bob" };
  Person person3 = { 3, "Charlie" };

  // Insere as pessoas na hashtable
  hashtable_insert(hashtable, &person1);
  hashtable_insert(hashtable, &person2);
  hashtable_insert(hashtable, &person3);

  // Verifica se as pessoas estão presentes na hashtable
  ck_assert(hashtable_contains(hashtable, &person1));
  ck_assert(hashtable_contains(hashtable, &person2));
  ck_assert(hashtable_contains(hashtable, &person3));

  // Cria uma pessoa diferente
  Person person4 = { 4, "Dave" };

  // Verifica se a pessoa diferente não está na hashtable
  ck_assert(!hashtable_contains(hashtable, &person4));

  // Liberta a memória utilizada pela hashtable
  hashtable_destroy(hashtable);
}
END_TEST

// Função principal dos testes
int main(void)
{
  // Cria a suite de testes
  Suite* suite = suite_create("hashtable_t");

  // Adiciona o teste à suite
  TCase* tcase = tcase_create("Core");
  tcase_add_test(tcase, test_hashtable);
  suite_add_tcase(suite, tcase);

  // Cria um objeto de retorno do teste
  SRunner* runner = srunner_create(suite);

  // Executa os testes
  srunner_run_all(runner, CK_NORMAL);

  // Obtém o número de falhas nos testes
  int num_failed = srunner_ntests_failed(runner);

  // Liberta a memória utilizada pelo runner
  srunner_free(runner);

  // Retorna o código de erro baseado no número de falhas
  return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
