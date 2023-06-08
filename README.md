# Projecto de informática 

Este repositório contêm o código desenvolvido no âmbito da disciplina de Projecto de Informática do curso de Licenciatura de Informática. 

Este projecto foi desenvolvido no sistema operativo Linux (Fedora 37) e utilizando o compilador GCC (v12.3.1).

Para os testes unitários foi utilizada a biblioteca [Check](https://libcheck.github.io/check/). 

## Notas de desenvolvimento

### 18 e 19 de maio de 2023 

* Desenvolvimento inicial de um alocador de paginas de 1 GB para alocar os estados expandido
* Desenvolvimento inicial de um min-heap para ser utilizada no algoritmo A* na sua forma sequencial
* Desenvolvimento inicial de uma hashtable para ser utilizada para manter informação de quais nós já foram expandidos
* Organização do projecto utilizando Makefiles
* Inclusão de testes unitários para o min-heap e para o alocador de memória

### 20 de maio de 2023

* Desenvolvimento inicial de uma linked-list para guardar temporariamente os vizinhos expandidos
* Desenvolvimento inicial do algoritmo A*
* Adicionada task para correr tests (vscode)
* Adicionada formatação por clang
* Mudanças de nomes de variáveis e funções para standardização
* Adicionada a flag -Wno-unused-parameter no makefile
* Melhoria dos comentários de alguns ficheiros para conter descrição da suas funcionalidades

### 21 de maio de 2023

* Criada funções dedicadas para gerir estados e a sua alocação
* Alterações do algoritmo A*, nomeadamente na criação de um nó, utilizado para a procura ao invés de usar o estado. 
* Finalização do a_star_solve, ainda sem testes.
* Melhoria dos nomes de algumas funções
* Adicionado suporte à hashtable para se fornecer um comparador
* Correção do ficheiro README.md (correção de data)

### 22 de maio de 2023

* Conversão do astar_sequencial para biblioteca
* Adição dos tests no ficheiro launch.json para depurar tests
* Pequenas correções ortográficas
* Implementação inicial do problema 8 Puzzle com testes
* Execução de alguns testes preliminares

### 23 de maio de 2023

* Algoritmo A* sequencial a funcionar, teste efetuado para o problema Puzzle
* Implementação de uma ferramenta para gerar problemas para Puzzle 8 de dificuldade elevada
* Pequenas correções ortográficas
* Correção da da lista de nós já expandidos (necessitava hash própria, ver comentários)
* Execução de alguns testes preliminares

### 31 de maio de 2023

* Separação do código sequencial das estruturas de apoio
* Estruturas de apoio serão agora desenvolvidas em astar_common
* Correção da construção de testes para a bibliotecas
* Adicionado suporte para verificação ortográfica para textos no VSCode
* Adicionado suporte para construir modo DEBUG e modo RELEASE
* Tasks.json atualizadas para construir em modo DEBUG

### 3 e 4 de junho de 2023

* Implementação inicial de um queue FIFO thread-safe com testes
* Implementação inicial de um canal de múltiplas filas para comunicação testes
* Adaptação da hashtable e do alocador de memória para suportar operações concorrentes
* Correção de alguns erros em textos
* state.c e state.h movido para a biblioteca astar_common
* Implementação inicial do algoritmo em paralelo (não testado)
* Melhoria de algum código já existente

### 5 de junho de 2023

* Continuação da implementação do algoritmo em paralelo (não testado)

### 6 de junho de 2023

* Implementação da algoritmo A* a correr em paralelo (com testes efetuados)
* Implementação do problema 8puzzle a correr o algoritmo em paralelo
* Alteração do nome astar_sequencial para astar_sequential
* Alteração dos nomes astar.c e astar.h para nomes alusivos à sua forma (ie: astar_sequential e astar_parallel)
* Remoção da flag de otimização -O3 da construção do 8puzzle_par (provoca quebra de performance do algoritmo A*)
* Implementação de estatísticas iniciais para medição da execução do algoritmo em ambas as formas (sequencial e paralelo)
* Correção da compilação em modo debug para conter símbolos
* Extração da função de hashing para uma função própria para que possa ser utilizada em outras funções
* Implementação do método channel_has_messages para verificar se uma determinada fila tem mensagens
* Alteração do formato da mensagem a ser enviada para os trabalhadores
* Alteração da função a_star_worker_function, o estado é agora processado no trabalhador a que lhe compete

### 8 e 9 de junho de 2023

* Implementação de um novo problema number link para executar novas medições
