#include "includes.h"


int keyId = 123;
int memId = 0;
void* data = NULL;
struct campo_compartilhado* memoria = NULL;


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






int main(int argc, char *argv[]){
  pid_t pid;
  //char* tmpLogMensagem = (char *)malloc( DATA_SZ* sizeof(char));
  int qt_processos_produtores =1;
  int qt_processos_consumidores =1;

  int rd_temp = 0;

  int posi;
  signal(SIGINT, handle_sigint); //capturando sinal


  //int limit = atoi(argv[1]); //limite de processos criados
  memId = memoria_compartilhada();
  data =  shmat( memId, (void *)0, 0); //atrelando a mem compatilhada a um ponteiro

  //free_memoria_compartilhada();

  if(data == (void *) -1){
    printf("\nErro ao criar campo de memoria\n");
    exit(-1);
  }




  memoria = (struct campo_compartilhado*) data; //
  memoria->flag_produtor = 0;

    // Inicialização dos semáforos
  if (sem_init(&memoria->sem_mutex, 1, 1) == -1) { // 1 indica que é compartilhado entre processos
        perror("sem_init sem_mutex");
        free_memoria_compartilhada();
        exit(EXIT_FAILURE);
    }

  if (sem_init(&memoria->sem_full, 1, 0) == -1) {
        perror("sem_init sem_full");
        free_memoria_compartilhada();
        exit(EXIT_FAILURE);
  }
  if (sem_init(&memoria->sem_empty, 1, DATA_SZ) == -1) {
        perror("sem_init sem_full");
        free_memoria_compartilhada();
        exit(EXIT_FAILURE);
  }


  for(int i = 0; i < qt_processos_consumidores; i++){
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo produtor\n");
      free_memoria_compartilhada();
    } else if (pid == 0) {
      printf("Iniciando consumidor\n");
      srand(getpid());
      while(1){
          int mutex_value;
          int memoria_value;

          // Get the values of the semaphores
          sem_getvalue(&memoria->sem_full, &memoria_value);

          sem_getvalue(&memoria->sem_mutex, &mutex_value);

          // Print the semaphore values
          printf("Tentando ler memoria: mutex = %d, memoria cheia = %d\n", mutex_value, memoria_value);

          sem_wait(&memoria->sem_full);
          sem_wait(&memoria->sem_mutex);

          //print_memoria(memoria);
          for(; memoria->flag_consumidor < DATA_SZ; memoria->flag_consumidor++){
            arvore_adicionar(memoria->dados[memoria->flag_consumidor]);
            memoria->dados[memoria->flag_consumidor] = 0;

          }
          memoria->flag_produtor = 0;
          sem_post(&memoria->sem_mutex); // Sai da seção crítica
          sem_post(&memoria->sem_empty);

          sleep(rand() % 3 + 1); // Simula tempo de produção
      }
    }
  }

  for(int i = 0; i < qt_processos_produtores; i++){
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo produtor\n");
      free_memoria_compartilhada();
    } else if (pid == 0) {
            printf("Iniciando produtor\n");

      srand(getpid()); //nova seed para cada filho
      while(1){
          int mutex_value;
          int memoria_value;

          // Get the values of the semaphores
          sem_getvalue(&memoria->sem_empty, &memoria_value);
          sem_getvalue(&memoria->sem_mutex, &mutex_value);

          // Print the semaphore values
          printf("Tentando escrever memoria: mutex = %d, memoria cheia = %d\n", mutex_value, memoria_value);




          int rd_num = rand() % (100 - 1 + 1); //criando valor aleatorio
          printf("\n processo (%d) Escrendo %d, na posicao %d\n", getpid(), rd_num, memoria->flag_produtor);

          memoria->dados[memoria->flag_produtor] = rd_num; //sessao critica do programa
          if(memoria->flag_produtor + 1 >= DATA_SZ){
            sem_post(&memoria->sem_full);  // Indica que há um novo item
          }else{
            memoria->flag_produtor++;
          }
          sem_post(&memoria->sem_mutex);
          sleep(rand() % 3 + 1); // Simula tempo de produção

      }
    }
  }



  while(1){
    //printf("aguardando\n");
   sleep(5);
  }

  free_memoria_compartilhada();
}

void handle_sigint(int sig) {
  printf("\n\n\n\n");
  printf("\nSignal %d, fechando processos\n", sig);
  printf("\n\n\n\n");
  free_memoria_compartilhada();
  kill(getppid(), SIGTERM);
}


/**
 * detecta um ctr+c para que a memória seja limpa
 */
int free_memoria_compartilhada() {
  printf("free_memoria_compartilhada,\n");

  if (shmdt((void*)data) == -1) {//retirando acesso ao campo de memória
    printf("free_memoria_compartilhada, shmdt erro(%d): %s\n",errno, strerror(errno));
    return (-1);
  }

  if (shmctl(memId, IPC_RMID, NULL) == -1) {
    printf("free_memoria_compartilhada, shmctl(%d) erro(%d): %s\n", memId, errno, strerror(errno));
    return (-1);
  }
  return (0);
}




