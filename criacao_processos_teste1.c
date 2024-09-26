#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <sys/wait.h> 
#include <sys/ipc.h>
#include <sys/shm.h>


int memSize = 512; 

int memoria_compartilhada(int tam){
  key_t keytmp = ftok("./pid.txt", 123);
  int shmid = shmget(keytmp, tam, 0666 | IPC_CREAT);
  
  return (shmid);
}

int main(int argc, char *argv[]){
  pid_t pid;
  int count = 0;
  int memId = 0;
  void* data = NULL;
  char buffer[11];

  //int limit = atoi(argv[1]); //limite de processos criados
  memId = memoria_compartilhada(memSize);

  printf("Memory attached at %p\n", memId);
  data = shmat( memId, (void *)0, 0);
  printf("Enter something: ");
  fgets(buffer, 11, stdin)
  strncpy(shared)
  strncpy(buffer, 11, stdin);

  printf("Memory attached at %p\n", data);





  /*
  if(limit <= 0){
    limit = 2;
  }

  while (count < limit)
 
  {
    pid = fork();
    printf("\ncount, %d\n", count);
    if(pid == 0){
      printf("\t(%d) Esse é o filho %d, pai %d", count, getpid(), getppid());
      printf("\n\tCodigo executando...");
      exit(1);
    }else{
      printf("Esse é o pai %d", getpid());
    }
    count++;
  }
  */

  while(1){
    printf("aguardando\n");
    sleep(20);
  }

  printf("\nfim do programa.\n\n");
}
