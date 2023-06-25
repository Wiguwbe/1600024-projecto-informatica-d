#include "8puzzle_logic.h"
#include "astar_parallel.h"
#include "astar_sequential.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool load_8puzzle(const char* filename, puzzle_state* puzzle)
{
  FILE* file = fopen(filename, "r");
  if(file == NULL)
  {
    printf("Erro ao abrir o arquivo.\n");
    return false;
  }

  char line[256]; // Considerando que cada linha tem no máximo 11 caracteres + o caracter nulo

  if(fgets(line, sizeof(line), file) == NULL)
  {
    printf("Erro ao ler linha do arquivo.\n");
    fclose(file);
    return false;
  }

  int k = 0;
  for(int y = 0; y < 3; y++)
  {
    for(int x = 0; x < 3; x++)
    {
      if(line[k] == ' ')
        k++; // Ignorar espaços em branco

      puzzle->board[y][x] = line[k++];
    }
    k++; // Pular o espaço ou quebra de linha
  }

  fclose(file);

  return true;
}

void print_solution(a_star_node_t* solution)
{
  printf("Solução:\n");
  for(int y = 0; y < 3; y++)
  {
    for(int x = 0; x < 3; x++)
    {
      puzzle_state* puzzle = (puzzle_state*)(solution->state->data);
      printf("%c", (char)puzzle->board[y][x]);
    }
    printf("\n");
  }
}

// Resolve a instância utilizando a versão paralela do algoritmo A*
void solve_parallel(puzzle_state instance, int num_threads, bool first, bool csv)
{
  // Criamos a instância do algoritmo A*
  a_star_parallel_t* a_star =
      a_star_parallel_create(sizeof(puzzle_state), goal, visit, heuristic, distance, print_solution, num_threads, first);

  // Tentamos resolver o problema
  a_star_parallel_solve(a_star, &instance, NULL);

  // Imprime as estatísticas da execução
  a_star_parallel_print_statistics(a_star, csv);

  // Limpamos a memória
  a_star_parallel_destroy(a_star);
}

// Resolve a instância utilizando a versão sequencial do algoritmo A*
void solve_sequential(puzzle_state instance, bool csv)
{
  // Criamos a instância do algoritmo A*
  a_star_sequential_t* a_star = a_star_sequential_create(sizeof(puzzle_state), goal, visit, heuristic, distance, print_solution);

  // Tentamos resolver o problema
  a_star_sequential_solve(a_star, &instance, NULL);

  // Imprime as estatísticas da execução
  a_star_sequential_print_statistics(a_star, csv);

  // Limpamos a memória
  a_star_sequential_destroy(a_star);
}

int main(int argc, char* argv[])
{
  // Verificar se o nome do arquivo foi fornecido como argumento
  if(argc < 2)
  {
    printf("Uso: %s [-n <num. trabalhadores>] [-p] [-r] <ficheiro_instâncias>\n", argv[0]);
    printf("Opções:\n");
    printf("-n : Número de trabalhadores (threads), defeito: 0 (algoritmo sequencial)\n");
    printf("-p : Termina à primeira solução encontrada, defeito: falso (utilizado no algoritmo paralelo apenas)\n");
    printf("-r : Relatório em formato compatível com CSV \n");
    return 0;
  }

  // Valores por defeito
  int num_threads = 0;
  bool first = false;
  bool csv = false;

  // Verificamos se mais opções foram passadas
  int filename_arg = 1;
  for(int i = 1; i < argc; i++)
  {
    char* opt = argv[i];

    if(strcmp(opt, "-n") == 0)
    {
      if(++i < argc)
      {
        num_threads = atoi(argv[i]);
      }
      else
      {
        printf("Erro: o número de trabalhadores não é um valor válido.\n");
        return 1;
      }
      filename_arg += 2;
      continue;
    }

    if(strcmp(opt, "-p") == 0)
    {
      first = true;
      filename_arg++;
      continue;
    }

    if(strcmp(opt, "-r") == 0)
    {
      csv = true;
      filename_arg++;
      continue;
    }
  }

  if(filename_arg >= argc)
  {
    printf("Erro: o falta nome do ficheiro com dados .\n");
    return 1;
  }

  // Ler as instâncias do arquivo
  puzzle_state puzzle;

  // Verificar se o puzzle foi lido corretamente
  if(!load_8puzzle(argv[filename_arg], &puzzle))
  {
    printf("Erro ao ler o puzzle do arquivo.\n");
    return 0;
  }

  if(num_threads > 0)
  {
    solve_parallel(puzzle, num_threads, first, csv);
  }
  else
  {
    solve_sequential(puzzle, csv);
  }
}