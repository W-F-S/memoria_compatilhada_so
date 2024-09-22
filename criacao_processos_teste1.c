#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // Para fork()
#include <sys/wait.h> // Para wait()

char* memoria_compartilhada(int tam){
  key_t keytmp = ftok("memoria", 123);
  int shmid = shmget(keytmp, tam, 0666 | IPC_CREAT);
  return (char*)shmat(shmid, (void*)0, 0);
}

int main(int argc, char *argv[]){
  pid_t pid;
  int count = 0;
  int limit = atoi(argv[1]);

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

  printf("\nfim do programa.\n\n");
}
