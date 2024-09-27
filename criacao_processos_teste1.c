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

struct campo_compartilhado{
  int flag_semaforo;
  char dados[DATA_SZ];
  char arvore[THREE_SZ];
};

int memoria_compartilhada(){
  key_t keytmp = ftok("./pid.txt", keyId);
  int shmid = shmget(keytmp, sizeof(struct campo_compartilhado), 0666 | IPC_CREAT);
  
  return (shmid);
}

int ler_mem(){

}

int escrever_mem(){

}

int main(int argc, char *argv[]){
  pid_t pid;
  int memId = 0;
  int qt_processos =4;
  int rd_temp = 0;
  void* data = NULL;
  struct campo_compartilhado* memoria;
  int posi;

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
          printf("\nlendo\n");
          memoria->flag_semaforo = 1;
          sleep(1);
          memoria->flag_semaforo = 0;
        }else{
          sleep(rand() % (5));
        }
    }
  }else if(pid < 0 ){
    printf("\nErro ao criar processo leitor\n");
  }else{
    for(int i = 0; i < qt_processos; i++){
      pid = fork();
      if (pid < 0) {
        printf("\nErro ao criar processo\n");
      } else if (pid == 0) {
          while(1){
            if(memoria->flag_semaforo == 0){
              printf("\nEscrevendo");
              memoria->flag_semaforo = 1;
              int rd_pos = rand() % (DATA_SZ - 1); //selecionando posicao aleatoria
              int rd_num = rand() % (100 - 1 + 1); //criando valor aleatorio

              printf("Escrendo %d, na posicao %d\n", rd_num, rd_pos);

              memoria->dados[rd_pos] = rd_num;
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


/*
  if(pid==0){
    while(1){

      printf("\nfilho escrevendo, %d \n", memoria->flag_semaforo);

      rd_temp = rand() % (5); //selecionando

      sleep(rd_temp);


      if(memoria->flag_semaforo == 0){
        printf("tentando escrever\n");
        memoria->flag_semaforo = 1;


        // Find the random number in the range [min, max]
        int rd_pos = rand() % (DATA_SZ - 1); //selecionando posicao de escrita
        int rd_num = rand() % (100 - 1 + 1); //selecionando posicao de escrita

        printf("Escrendo %d, na posicao %d\n", rd_num, rd_pos);

        memoria->dados[rd_pos] = rd_num;
        memoria->flag_semaforo = 0;
      }
    }
  }else{

    sleep(5);
    while(1){
      printf("\nPai lendo, %d \n", memoria->flag_semaforo);

      rd_temp = rand() % (5); //selecionando

      sleep(rd_temp);

      if(memoria->flag_semaforo == 0){
        printf("Pai tentando ler\n");
        memoria->flag_semaforo = 1;
        printf("Acessar qual posicao:");
        //scanf("%d", &posi);

        if(posi >0){
          posi--;
        }
        //printf("You wrote: %c\n", memoria->dados[posi]);
        sleep(1);
        memoria->flag_semaforo = 0;
      }

    }
  }
  */

  printf("\nfim do programa.\n\n");
}
