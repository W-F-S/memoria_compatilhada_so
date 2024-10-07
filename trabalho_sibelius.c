#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

#define DATA_SZ  10
int keyId = 123;
int memId = 0;
void* data = NULL;

struct campo_compartilhado{
  int flag_semaforo;  //flag que determina se deve escreber ou nao;
  char dados[DATA_SZ];

};

int memoria_compartilhada(){
  key_t keytmp = ftok("./pid1.txt", keyId);
  int shmid = shmget(keytmp, sizeof(struct campo_compartilhado), 0666 | IPC_CREAT);

  return (shmid);
}

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


int main(int argc, char *argv[]){
  pid_t pid;

  int qt_processos_produtores =3;
  int qt_processos_consumidores =2;

  int rd_temp = 0;

  struct campo_compartilhado* memoria;
  int posi;
  signal(SIGINT, handle_sigint); //capturando sinal

  memId = memoria_compartilhada();
  data =  shmat( memId, (void *)0, 0); //atrelando a mem compatilhada a um ponteiro

  if(data == (void *) -1){
    printf("\nErro ao criar campo de memoria\n");
    exit(-1);
  }

  memoria = (struct campo_compartilhado*) data; //
  memoria->flag_semaforo = 0;

  for(int i = 0; i < qt_processos_consumidores; i++){
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo produtor\n");
    } else if (pid == 0) {

      while(1){
        if(memoria->flag_semaforo == 0){
          memoria->flag_semaforo = 1;

          int rd_pos = rand() % (DATA_SZ - 1); //selecionando posicao aleatoria
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
        if(memoria->flag_semaforo == 0){
          memoria->flag_semaforo = 1;
          int rd_pos = rand() % (DATA_SZ - 1); //selecionando posicao aleatoria
          int rd_num = rand() % (100 - 1 + 1); //criando valor aleatorio
          printf("\n processo (%d) Escrendo %d, na posicao %d\n", getpid(), rd_num, rd_pos);
          memoria->dados[rd_pos] = rd_num; //sessao critica do programa
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
