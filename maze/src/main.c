#include "astar_parallel.h"
#include "astar_sequential.h"
#include "maze_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

maze_solver_t* init_maze_solver_puzzle(const char* filename)
{
  FILE* file = fopen(filename, "r");
  if(file == NULL)
  {
    printf("Erro ao abrir o arquivo.\n");
    return false;
  }

  char buffer[100]; // Tamanho maximo de uma linha
  char board[100 * 100]; // Tamanho máximo de um tabuleiro

  char* ptr = (char*)&board;
  int rows = 0;
  int cols = 0;
  while(!feof(file))
  {

    if(fgets(buffer, sizeof(buffer), file) == NULL)
    {
      printf("Erro ao ler linha do arquivo.\n");
      fclose(file);
      return false;
    }

    rows++;
    buffer[strcspn(buffer, "\r\n")] = 0;
    int len = strlen(buffer);
    if(cols == 0)
    {
      cols = len;
    }
    else
    {
      if(cols != len)
      {
        printf("Erro ao ler linha do arquivo.\n");
        fclose(file);
        return false;
      }
    }

    // Copia o buffer para o tabuleiro
    memcpy(ptr, buffer, len);
    ptr += len;
  }
  fclose(file);

  return maze_solver_init(rows, cols, (const char*)&board);
}

void print_solution(a_star_node_t* solution)
{
  maze_solver_state_t* state = (maze_solver_state_t*)solution->state->data;
  maze_solver_t* maze_solver = state->maze_solver;
  board_data_t board_data = maze_solver_wrap_board(maze_solver, state->board_data);
  for(int y = 0; y < maze_solver->rows; y++)
  {
    for(int x = 0; x < maze_solver->cols; x++)
    {
      int index = y * maze_solver->cols + x;
      printf("%c", (char)board_data.board[index]);
    }
    printf("\n");
  }
}

// Resolve o problema utilizando a versão paralela do algoritmo
void solve_parallel(maze_solver_t* maze_solver, int num_threads, bool first, bool csv, bool show_solution)
{
  // Criamos a instância do algoritmo A*
  a_star_parallel_t* a_star =
      a_star_parallel_create(sizeof(maze_solver_state_t), goal, visit, heuristic, distance, print_solution, num_threads, first);

  // Criamos o nosso estado inicial para lançar o algoritmo
  maze_solver_state_t initial = { maze_solver,
                                  maze_solver_create_board(maze_solver, maze_solver->initial_board, maze_solver->entry_coord) };

  // Tentamos resolver o problema
  a_star_parallel_solve(a_star, &initial, NULL);

  // Imprime as estatísticas da execução
  a_star_parallel_print_statistics(a_star, csv, show_solution);

  // Limpamos a memória
  a_star_parallel_destroy(a_star);
}

// Resolve o problema utilizando a versão sequencial do algoritmo
void solve_sequential(maze_solver_t* maze_solver, bool csv, bool show_solution)
{
  // Criamos a instância do algoritmo A*
  a_star_sequential_t* a_star =
      a_star_sequential_create(sizeof(maze_solver_state_t), goal, visit, heuristic, distance, print_solution);

  // Criamos o nosso estado inicial para lançar o algoritmo
  maze_solver_state_t initial = { maze_solver,
                                  maze_solver_create_board(maze_solver, maze_solver->initial_board, maze_solver->entry_coord) };

  // Tentamos resolver o problema
  a_star_sequential_solve(a_star, &initial, NULL);

  // Imprime as estatísticas da execução
  a_star_sequential_print_statistics(a_star, csv, show_solution);

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
  bool show_solution = false;

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

    if(strcmp(opt, "-s") == 0)
    {
      show_solution = true;
      filename_arg++;
      continue;
    }
  }

  if(filename_arg >= argc)
  {
    printf("Erro: o falta nome do ficheiro com dados .\n");
    return 1;
  }

  maze_solver_t* maze_solver = init_maze_solver_puzzle(argv[filename_arg]);

  // Inicializa o nosso puzzle
  if(!maze_solver)
  {
    printf("Erro a inicializar o puzzle, verifique o ficheiro com os dados\n");
    return 1;
  }

  if(num_threads > 0)
  {
    solve_parallel(maze_solver, num_threads, first, csv, show_solution);
  }
  else
  {
    solve_sequential(maze_solver, csv, show_solution);
  }

  maze_solver_destroy(maze_solver);
  return 0;
}