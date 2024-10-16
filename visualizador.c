#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

#define DATA_SZ  10
#define THREE_SZ  2048
int keyId = 123;
int memId = 0;
void* data = NULL;

//cpp -dM /usr/include/errno.h | grep 'define E' | sort -n -k 3


/**
 *
 *
 * 01/11
 * prova após semana da tecnologia.
 * ipc e processos e gerenciamento de memoria, padrão enade, ler o livro da bibliografica supercharts sla,
 *
 * exclusao mútua.
 */

/**
 *
 *
 *
 * data de entrega, 25/10
 *
 *
 *
 a lista de dados deve ser encadeada, devemos ter um contador para caso a fila esteja cheia. como é uma fila, voce precisa mover os integrantes quando o consumidor consumir.

 also, consumidor e produto do mesmo pai.

 lembrando que o consumidor precisa inserir em uma árvore binária estática
 não balancear a arvore, mas talvez sim, como um desafio extra.
 devemos fazer uma struct para a arvore


 a arvore precisa de um bloco, uma flag que boqueia ou nao a escrita.




 devemos criar um segundo consumidor, que modifica, visualiza, e faz operaçoes simples
 *
 */

struct campo_compartilhado{
  int flag_semaforo;  //flag que determina se deve escreber ou nao;
  int flag_contador;
  char dados[DATA_SZ];
  char arvore[THREE_SZ];
};

int memoria_compartilhada(){
  key_t keytmp = ftok("./pid1.txt", keyId);
  int shmid = shmget(keytmp, sizeof(struct campo_compartilhado), 0666 | IPC_CREAT);

  return (shmid);
}



void print_memoria(struct campo_compartilhado* memoria){
  printf("\n");
  for(int i=0; i<DATA_SZ; i++){
    printf("[%d]", memoria->dados[i]);
  }
  printf("\n");
}

int main(int argc, char *argv[]){

















  int stop = 0;
  int input;

  while (!stop) {
    printf("Digite um número (0 para sair, 1 para executar a função 01): ");
    scanf("%d", &input);

    // Verifica se o usuário digitou 0 para sair
    if (input == 0) {
      printf("Saindo...\n");
      break;
    }

    // Switch para verificar qual função executar
    switch (input) {
      case 1:

        break;
      default:

        break;
    }
  }


  pid_t pid;
  int qt_processos_consumidores =1;

  int rd_temp = 0;

  struct campo_compartilhado* memoria;
  int posi;


  //int limit = atoi(argv[1]); //limite de processos criados
  memId = memoria_compartilhada();
  data =  shmat( memId, (void *)0, 0); //atrelando a mem compatilhada a um ponteiro

  if(data == (void *) -1){
    printf("\nErro ao criar campo de memoria\n");
    exit(-1);
  }

  memoria = (struct campo_compartilhado*) data; //

  for(int i = 0; i < qt_processos_consumidores; i++){
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo produtor\n");
    } else if (pid == 0) {
      while (!stop) {
          printf("Digite um número (0 para sair, 1 para executar a função 01): ");
          scanf("%d", &input);

          // Verifica se o usuário digitou 0 para sair
          if (input == 0) {
            printf("Saindo...\n");
            break;
          }

          // Switch para verificar qual função executar
          switch (input) {
            case 1:
              print_memoria(memoria);
              break;
            default:
              printf("Nenhuma função associada ao valor %d\n", input);
              break;
          }

      }
    }
  }
}













#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

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
    printf("Digite um número (0 para sair, 1 para executar a função 01): ");
    scanf("%d", &input);

    // Verifica se o usuário digitou 0 para sair
    if (input == 0) {
      printf("Saindo...\n");
      break;
    }

    // Switch para verificar qual função executar
    switch (input) {
      case 1:
        execute_function_01();
        break;
      default:
        printf("Nenhuma função associada ao valor %d\n", input);
        break;
    }
  }

  return 0;
}





























