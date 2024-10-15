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


/*
int ler_mem(){

}

int escrever_mem(){

}
**/

/**
 * detecta um ctr+c para que a memória seja limpa
 */
void free_memoria_compartilhada() {

  if (shmdt((void*)data) == -1) {
    printf("free_memoria_compartilhada, shmdt erro(%d): %s\n",errno, strerror(errno));
    exit(-1);
  }

  if (shmctl(memId, IPC_RMID, NULL) == -1) {
    printf("free_memoria_compartilhada, shmctl(%d) erro(%d): %s\n", memId, errno, strerror(errno));
    exit(-1);
  }

}

void print_memoria(struct campo_compartilhado* memoria){
  printf("\n");
  for(int i=0; i<DATA_SZ; i++){
    printf("[%d]", memoria->dados[i]);
  }
  printf("\n");
}


void handle_sigint(int sig) {
  printf("\n\n\n\n");
  printf("\nSignal %d, fechando processos\n", sig);
  printf("\n\n\n\n");
  free_memoria_compartilhada();
  exit(0);
}


// Global file pointer
FILE *log_file = NULL;

// Function to open the file
void open_log_file(const char *filename) {
  log_file = fopen(filename, "a"); // Open in append mode
  if (log_file == NULL) {
    perror("Error opening file");
    exit(1);
  }
}

// Function to write a message to the file
void write_log(const char *message) {
  if (log_file == NULL) {
    fprintf(stderr, "Log file is not open.\n");
    return;
  }
  fprintf(log_file, "%s\n", message); // Write the message to the file
}

// Function to close the file
void close_log_file() {
  if (log_file != NULL) {
    fclose(log_file);
    log_file = NULL;
  }
}




int main(int argc, char *argv[]){
  pid_t pid;
  //char* tmpLogMensagem = (char *)malloc( DATA_SZ* sizeof(char));
  int qt_processos_produtores =3;
  int qt_processos_consumidores =2;

  int rd_temp = 0;

  struct campo_compartilhado* memoria;
  int posi;
  signal(SIGINT, handle_sigint); //capturando sinal


  //int limit = atoi(argv[1]); //limite de processos criados
  memId = memoria_compartilhada();
  data =  shmat( memId, (void *)0, 0); //atrelando a mem compatilhada a um ponteiro

  if(data == (void *) -1){
    printf("\nErro ao criar campo de memoria\n");
    exit(-1);
  }

  memoria = (struct campo_compartilhado*) data; //
  memoria->flag_semaforo = 0;
  memoria->flag_contador = 0;

  for(int i = 0; i < qt_processos_consumidores; i++){
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo produtor\n");
    } else if (pid == 0) {

      while(1){
        if(memoria->flag_semaforo == 0){
          memoria->flag_semaforo = 1;

          int rd_pos = rand() % (DATA_SZ - 1); //selecionando posicao aleatoria
          //snprintf(tmpLogMensagem, DATA_SZ-1, "\nprocesso (%d) lendo [%d]\n\n", getpid(), memoria->dados[rd_pos]);
          //open_log_file("logfile.txt");
          //write_log(tmpLogMensagem);
          //close_log_file();

          printf("\nprocesso (%d) lendo [%d]\n", getpid(), memoria->dados[rd_pos]); //sessao critica do programa

          print_memoria(memoria);

          if(memoria->dados[rd_pos] > 0){ memoria->dados[rd_pos] = -1;} //sessao critica do programa

          sleep(1);
          memoria->flag_semaforo = 0;
          sleep(1);
        }else{
          printf("\n processo (%d), leitura bloqueada", getpid());
          sleep(rand() % (5));
        }
      }
    }
  }

  for(int i = 0; i < qt_processos_produtores; i++){
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo produtor\n");
    } else if (pid == 0) {
      while(1){
        if(memoria->flag_semaforo == 0 && (memoria->flag_contador) < DATA_SZ){
          memoria->flag_semaforo = 1;


          int rd_num = rand() % (100 - 1 + 1); //criando valor aleatorio

          //snprintf(tmpLogMensagem, DATA_SZ-1, "\n processo (%d) Escrendo %d, na posicao %d\n", getpid(), rd_num, rd_pos);
          //open_log_file("logfile.txt");
          //write_log(tmpLogMensagem);
          //close_log_file();

          printf("\n processo (%d) Escrendo %d, na posicao %d\n", getpid(), rd_num, rd_pos);


          memoria->dados[flag_contador] = rd_num; //sessao critica do programa
          memoria->flag_contador++;

          print_memoria(memoria);
          sleep(1);
          memoria->flag_semaforo = 0;
          sleep(1);
        }else{
          printf("\n processo (%d), escrita bloqueada", getpid());
          sleep(rand() % (5));
        }
      }
    }
  }

  while(1){
    //printf("\nprocesso (%d) fim do programa.\n\n", getpid());
    sleep(1);
  }
}
