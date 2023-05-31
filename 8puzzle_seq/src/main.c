#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
        if (line[k] == ' ')
          k++; // Ignorar espaços em branco

        instances[i].board[j][l] = line[k++];
      }
      k++; // Pular o espaço ou quebra de linha
    }
  }

  fclose(file);

  return 10;
}

void solve(puzzle_state instance)
{
  // Criamos a instância do algoritmo A* 
  a_star_t* a_star_instance = a_star_create(sizeof(puzzle_state), goal, visit, heuristic, distance);

  // Iniciamos a exploração
  a_star_solve(a_star_instance, &instance, NULL);

  // Limpamos a memória
  a_star_destroy(a_star_instance);
}

int main(int argc, char* argv[])
{
  // Verificar se o nome do arquivo foi fornecido como argumento
  if (argc < 2) {
    printf("Nome do arquivo não fornecido.\n");
    return 0;
  }

  // Ler as instâncias do arquivo
  puzzle_state instances[10];
  int num_instances = read_instance_from_file(argv[1], instances);

  // Verificar se o número de instâncias lidas está correto
  if (num_instances != 10) {
    printf("Erro ao ler as instâncias do arquivo.\n");
    return 0;
  }

  // Medir o tempo de execução da função solve para cada instância
  clock_t start_time, end_time;
  double total_time = 0.0;

  for (int i = 0; i < 10; i++) {
    printf("A resolver a instância %d...\n", i + 1);

    start_time = clock(); // Início da contagem do tempo

    solve(instances[i]);

    end_time = clock(); // Fim da contagem do tempo

    double instance_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    total_time += instance_time;

    printf("A instância %d foi resolvida em %.6f segundos.\n", i + 1, instance_time);
  }

  printf("Todas as instâncias foram resolvidas. Tempo total: %.6f segundos.\n", total_time);

  return 0;
}