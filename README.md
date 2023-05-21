# Projecto de informática 

Este repositório contêm o código desenvolvido no ambito da disciplina de Projecto de Informática do curso de Licenciatura de Informática. 

Este projecto foi desenvolvido no sistema operativo Linux (Fedora 37) e utilizando o compilador GCC (v12.3.1).

Para os testes unitários foi utilizada a bibilioteca [Check](https://libcheck.github.io/check/). 

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
* Mudanças de nomes de variveis e funções para standardização
* Adicionada a flag -Wno-unused-parameter no makefile
* Melhoria dos comentários de alguns ficheiros para conter descrição da suas funcionalidades

### 21 de maio de 2023
* Criada funções dedicadas para gerir estados e a sua alocação
* Alterações do algoritmo A*, nomeadamente na criação de um nó, utilizado para a procura ao invés de usar o estado. 
* Finalização do a_star_solve, ainda sem testes.
* Melhoria dos nomes de algumas funções
* Adicionado suporte à hashtable para se fornecer um comparador
* Correção do ficheiro README.md (correção de data)
