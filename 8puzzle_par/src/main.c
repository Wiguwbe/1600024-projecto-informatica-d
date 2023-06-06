#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Função para ler as instâncias do arquivo e armazenar na struct
int read_instance_from_file(const char* filename, puzzle_state instances[])
{
  FILE* file = fopen(filename, "r");
  if(file == NULL)
  {
    printf("Erro ao abrir o arquivo.\n");
    return 0;
  }

  char line[256]; // Considerando que cada linha tem no máximo 11 caracteres + o caracter nulo

  for(int i = 0; i < 10; i++)
  {
    if(fgets(line, sizeof(line), file) == NULL)
    {
      printf("Erro ao ler linha do arquivo.\n");
      fclose(file);
      return i;
    }

    int k = 0;
    for(int j = 0; j < 3; j++)
    {
      for(int l = 0; l < 3; l++)
      {
        if(line[k] == ' ')
          k++; // Ignorar espaços em branco

        instances[i].board[j][l] = line[k++];
      }
      k++; // Pular o espaço ou quebra de linha
    }
  }

  fclose(file);

  return 10;
}

double solve(puzzle_state instance, int num_threads, bool first)
{
  struct timespec start_time, end_time;

  // Criamos a instância do algoritmo A*
  a_star_t* a_star_instance = a_star_create(sizeof(puzzle_state), goal, visit, heuristic, distance, num_threads);

  clock_gettime(CLOCK_MONOTONIC, &start_time);

  // Iniciamos a exploração
  if(a_star_solve(a_star_instance, &instance, NULL, first))
  {
    printf("Resultado do algoritmo: Solução encontrada.\n");
  }
  else
  {
    printf("Resultado do algoritmo: Solução não encontrada.\n");
  }

  clock_gettime(CLOCK_MONOTONIC, &end_time);

  double instance_time = (end_time.tv_sec - start_time.tv_sec);
  instance_time += (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

  printf("Estatísticas:\n");
  for(size_t i = 0; i < a_star_instance->num_workers; i++)
  {
    printf("- Trabalhador #%ld: Estados expandidos = %d, Estados visitados = %d\n",
           i + 1,
           a_star_instance->workers[i].expanded,
           a_star_instance->workers[i].visited);
  }
  printf("- Estados expandidos: %d\n", a_star_instance->expanded);
  printf("- Estados visitados: %d\n", a_star_instance->visited);
  printf("- Tempo de execução: %.6f segundos.\n", instance_time);
  printf("- Primeira solução apenas: %s\n", first ? "sim" : "não");

  // Limpamos a memória
  a_star_destroy(a_star_instance);

  return instance_time;
}

int main(int argc, char* argv[])
{
  // Verificar se o nome do arquivo foi fornecido como argumento
  if(argc < 2)
  {
    printf("Uso: %s <ficheiro_instâncias> [-n <num. trabalhadores>] [-p]\n", argv[0]);
    printf("Opções:\n");
    printf("-n : Número de trabalhadores (threads), defeito: 1\n");
    printf("-p : Termina à primeira solução encontrada, defeito: falso\n");
    return 0;
  }

  // Valores por defeito 
  int num_threads = 1;
  bool first = false;

  // Verificamos se o número de threads foi passado nos argumentos
  if(argc >= 3)
  {
    char* switch1 = argv[2];
    if(strcmp(switch1, "-n") == 0)
    {
      if(argc >= 4)
      {
        num_threads = atoi(argv[3]);
      }
      else
      {
        printf("Erro: o número de trabalhadores não é um valor válido.\n");
        return 1;
      }
    }
  }

  // Verificamos se a opção 'p' foi passada
  if(argc >= 5)
  {
    char* switch2 = argv[4];
    if(strcmp(switch2, "-p") == 0)
    {
      first = true;
    }
  }

  // Ler as instâncias do arquivo
  puzzle_state instances[10];
  int num_instances = read_instance_from_file(argv[1], instances);

  // Verificar se o número de instâncias lidas está correto
  if(num_instances != 10)
  {
    printf("Erro ao ler as instâncias do arquivo.\n");
    return 0;
  }
  // Medir o tempo de execução da função solve para cada instância
  double total_time = 0.0;

  for(int i = 0; i < 10; i++)
  {
    printf("Instância #%d\n", i + 1);

    double instance_time = solve(instances[i], num_threads, first);
    total_time += instance_time;
    printf("\n");
  }

  printf("Todas as instâncias foram executadas. Tempo total: %.6f segundos.\n", total_time);

  return 0;
}