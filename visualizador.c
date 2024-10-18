#include "includes.h"

volatile sig_atomic_t stop = 0;

void handle_sigint(int sig) {
  stop = 1;
}

void execute_function_01() {
  printf("Executando a função 01...\n");
}

int main() {
  int input;


  // Captura o sinal de CTRL-C
  signal(SIGINT, handle_sigint);

  while (!stop) {
    printf("0, sair;\n1, mostrar arvore\n2, adicionar valor arvore");
    scanf("%d", &input);

    // Verifica se o usuário digitou 0 para sair
    if (input == 0) {
      printf("Saindo...\n");
      break;
    }

    // Switch para verificar qual função executar
    switch (input) {
      case 0:
        execute_function_01();
        break;
      case 1:
        execute_function_01();
      break;
      case 2:
        execute_function_01();
      break;
      default:
        printf("Nenhuma função associada ao valor %d\n", input);
        break;
    }
  }

  return 0;
}
