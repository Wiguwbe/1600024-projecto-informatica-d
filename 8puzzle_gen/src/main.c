#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PUZZLE_SIZE 9
#define MAX_MOVES 100

// Função para trocar dois elementos no puzzle
void swap(char* puzzle, int i, int j) {
    char temp = puzzle[i];
    puzzle[i] = puzzle[j];
    puzzle[j] = temp;
}

// Função para embaralhar o puzzle com uma sequência aleatória de movimentos
void shuffle_puzzle(char* puzzle) {
    int empty_index = 0; // índice do espaço vazio
    for (int i = 0; i < MAX_MOVES; i++) {
        int valid_moves[4]; // 0 - cima, 1 - baixo, 2 - esquerda, 3 - direita
        int num_valid_moves = 0;

        // Verificar movimentos válidos com base na posição do espaço vazio
        if (empty_index >= 3) {
            valid_moves[num_valid_moves++] = 0; // cima
        }
        if (empty_index <= 5) {
            valid_moves[num_valid_moves++] = 1; // baixo
        }
        if (empty_index % 3 != 0) {
            valid_moves[num_valid_moves++] = 2; // esquerda
        }
        if (empty_index % 3 != 2) {
            valid_moves[num_valid_moves++] = 3; // direita
        }

        // Escolher um movimento aleatório dos movimentos válidos
        int random_move = valid_moves[rand() % num_valid_moves];

        // Executar o movimento
        switch (random_move) {
            case 0: // cima
                swap(puzzle, empty_index, empty_index - 3);
                empty_index -= 3;
                break;
            case 1: // baixo
                swap(puzzle, empty_index, empty_index + 3);
                empty_index += 3;
                break;
            case 2: // esquerda
                swap(puzzle, empty_index, empty_index - 1);
                empty_index--;
                break;
            case 3: // direita
                swap(puzzle, empty_index, empty_index + 1);
                empty_index++;
                break;
        }
    }
}

// Função para gerar uma instância difícil do puzzle
void generate_difficult_instance(FILE* file) {
    char puzzle[PUZZLE_SIZE] = {'1', '2', '3', '4', '5', '6', '7', '8', '-'};

    shuffle_puzzle(puzzle); // embaralhar o puzzle

    // Escrever a instância no arquivo
    for (int i = 0; i < PUZZLE_SIZE; i++) {
        fprintf(file, "%c ", puzzle[i]);
    }
    fprintf(file, "\n");
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <nome_do_arquivo> <numero_de_puzzles>\n", argv[0]);
        return 1;
    }

    char* filename = argv[1];
    int num_puzzles = atoi(argv[2]);

    srand(time(NULL)); // Inicializar a semente aleatória

    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        return 1;
    }

    // Gerar as instâncias de puzzles difíceis e escrevê-las no arquivo
    for (int i = 0; i < num_puzzles; i++) {
        generate_difficult_instance(file);
    }

    fclose(file);
    return 0;
}
