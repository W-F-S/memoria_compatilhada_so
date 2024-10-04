#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define DATA_SZ  2048
#define THREE_SZ  2048
int keyId = 123;
int memId = 0;
void* data = NULL;

struct campo_compartilhado{
  int flag_semaforo;
  char dados[DATA_SZ];
  char arvore[THREE_SZ];
};

int memoria_compartilhada(){
  key_t keytmp = ftok("./pid1.txt", keyId);
  int shmid = shmget(keytmp, sizeof(struct campo_compartilhado), 0666 | IPC_CREAT);

  return (shmid);
}



int ler_mem(){

}

int escrever_mem(){

}

void free_memoria_compartilhada() {
  if (shmdt((void*)data) == -1) {
    printf("free_memoria_compartilhada, shmdt erro");
    exit(-1);
  }

  if (shmctl(memId, IPC_RMID, NULL) == -1) {
    printf("free_memoria_compartilhada, shmctl erro");
    exit(-1);
  }

}

void handle_sigint(int sig) {
  printf("\nSignal %d, fechando processos\n", sig);
  free_memoria_compartilhada();
  exit(0);
}

void print_memoria(){

  printf("\nSignal %d, fechando processos\n", sig);

}


int main(int argc, char *argv[]){
  pid_t pid;

  int qt_processos =1;
  int rd_temp = 0;

  struct campo_compartilhado* memoria;
  int posi;
  signal(SIGINT, handle_sigint);

  //int limit = atoi(argv[1]); //limite de processos criados
  memId = memoria_compartilhada();
  data =  shmat( memId, (void *)0, 0);


  if(data == (void *) -1){
    printf("\nErro ao criar campo de memoria\n");
    exit(-1);
  }

  memoria = (struct campo_compartilhado*) data;
  memoria->flag_semaforo = 0;

  //processo consumidor
  pid = fork();
  if(pid == 0){
    while(1){
      if(memoria->flag_semaforo == 0){
        printf("\nlendo\n\n");
        memoria->flag_semaforo = 1;


        printf("[%c]\n", memoria->dados[0]);

        sleep(1);
        memoria->flag_semaforo = 0;
      }else{
        sleep(rand() % (5));
      }
    }
  }else if(pid < 0 ){
    printf("\nErro ao criar processo leitor\n");
  }else{//processo produtor
    for(int i = 0; i < qt_processos; i++){
      pid = fork();
      if (pid < 0) {
        printf("\nErro ao criar processo produtor\n");
      } else if (pid == 0) {
        while(1){
          if(memoria->flag_semaforo == 0){
            memoria->flag_semaforo = 1;
            int rd_pos = rand() % (DATA_SZ - 1); //selecionando posicao aleatoria
            int rd_num = rand() % (100 - 1 + 1); //criando valor aleatorio

            printf("Escrendo %d, na posicao %d\n", rd_num, rd_pos);

            //memoria->dados[rd_pos] = rd_num;
            memoria->dados[0] = rd_num;
            sleep(1);
            memoria->flag_semaforo = 0;
          }else{
            sleep(rand() % (5));
          }
        }
      }else{
        //pai
      }
    }

  }

  while(1){
    printf("\nfim do programa.\n\n");
    sleep(1);
  }
}
