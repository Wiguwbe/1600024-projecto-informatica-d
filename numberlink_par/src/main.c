#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

number_link_t* init_number_link_puzzle(const char* filename)
{
  FILE* file = fopen(filename, "r");
  if(file == NULL)
  {
    printf("Erro ao abrir o arquivo.\n");
    return false;
  }

  char buffer[100]; // Tamanho maximo de uma linha
  char board[64*64]; // Tamanho máximo de um tabuleiro

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

  return number_link_init(rows, cols, (const char*)&board);
}

double solve(number_link_t* number_link, int num_threads, bool first)
{
  struct timespec start_time, end_time;

  // Criamos a instância do algoritmo A*
  a_star_t* a_star_instance = a_star_create(sizeof(number_link_state_t), goal, visit, heuristic, distance, num_threads);

  // Criamos o nosso estado inicial para lançar o algoritmo
  number_link_state_t initial = { number_link,
                                  number_link_create_board(number_link, number_link->initial_board, number_link->initial_coords),
                                  0 };

  clock_gettime(CLOCK_MONOTONIC, &start_time);

  a_star_node_t* solution = a_star_solve(a_star_instance, &initial, NULL, first);

  // Iniciamos a exploração
  if(solution)
  {
    printf("Resultado do algoritmo: Solução encontrada.\n");
    for(int y = 0; y < number_link->rows; y++) {
      for(int x = 0; x < number_link->cols; x++) {
        int index = y * number_link->cols + x;
        number_link_state_t* state = (number_link_state_t*)solution->state->data;
        board_data_t board_data = number_link_wrap_board(number_link, state->board_data);
        printf("%c", (char) board_data.board[index]);
      }
      printf("\n");
    }
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

  number_link_t* number_link = init_number_link_puzzle(argv[1]);

  // Inicializa o nosso puzzle
  if(!number_link)
  {
    printf("Erro a inicializar o puzzle, verifique o ficheiro com os dados\n");
    return 1;
  }

  printf("A resolver puzzle number link\n");
  double total_time = solve(number_link, num_threads, first);
  printf("\n");

  printf("Tempo total de execução: %.6f segundos.\n", total_time);
  number_link_destroy(number_link);
  return 0;
}