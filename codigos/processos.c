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



 seu trabalho é criar um progama usando a linguagem C que:
  cria 16 processos diferentes usando a função fork()
  alocar uma área de memória compartilhada que contem
    uma fila ou pilha numeros inteiros
    uma árvore em array

  cada um dos 16 processos produtores gerarão um número aleatorio,
    esse número será colocado na primeira fila de dados

  após isso um consumidor consumirá um desses números e colocará na arvore binária

  o processo de geração e consumo desses números deverá ser aleatorio

  no final, crie um programa que possibilite visualizar a árvore de dados

 *
 */





int main(int argc, char *argv[]){
  int qt_processos_produtores =16;
  int qt_processos_consumidores =2;

  pid_t pid;

  signal(SIGINT, handle_sigint); //capturando sinal

  memId = memoria_compartilhada();
  data =  shmat( memId, (void *)0, 0); //atrelando a mem compatilhada a um ponteiro

  if(data == (void *) -1){
    printf("\nErro ao criar campo de memoria\n");
    exit(-1);
  }

  memoria = (struct campo_compartilhado*) data; //
  memoria->flag_produtor = 0;

  for(int i=memoria->flag_consumidor; i < DATA_SZ; i++){ memoria->dados[i] = -1; }



  // Inicialização dos semáforos
  if (sem_init(&memoria->sem_mutex, 1, 1) == -1) { // 1 indica que é compartilhado entre processos
      perror("sem_init sem_mutex");
      free_memoria_compartilhada();
      exit(EXIT_FAILURE);
  }

  //TODO fazer essa logica só com um sem
  if (sem_init(&memoria->sem_full, 1, 0) == -1) { //ver se o array de dados está cheio
        perror("sem_init sem_full");
        free_memoria_compartilhada();
        exit(EXIT_FAILURE);
  }

  if (sem_init(&memoria->sem_empty, 1, DATA_SZ) == -1) {//ver se o array de dados está vazio
        perror("sem_init sem_full");
        free_memoria_compartilhada();
        exit(EXIT_FAILURE);
  }

  for(int i = 0; i < qt_processos_consumidores; i++){
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo consumidor\n");
      free_memoria_compartilhada();
    } else if (pid == 0) {
      printf("Iniciando consumidor\n");
      srand(getpid());
      while(1){
          int mutex_value;
          int memoria_value;
          int memoria_empty;

          sem_getvalue(&memoria->sem_full, &memoria_value);
          sem_getvalue(&memoria->sem_empty, &memoria_empty);
          sem_getvalue(&memoria->sem_mutex, &mutex_value);

          printf("Tentando ler memoria: mutex = %d, memoria cheia = %d, memoria empty = %d\n", mutex_value, memoria_value, memoria_empty);
            memoria_visualizar(memoria);

            sem_wait(&memoria->sem_full);

            for(int i =0 ; i < DATA_SZ; i++){arvore_adicionar(memoria->dados[i]); memoria->dados[i] = -1; }
            memoria_visualizar(memoria);

            memoria->flag_produtor = 0;
            sem_post(&memoria->sem_empty);  // Indicate space is available

        sleep(rand() % 7 + 1);  // Simulate time for consuming
      }
    }
  }

  for(int i = 0; i < qt_processos_produtores; i++){
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo produtor\n");
      free_memoria_compartilhada();
    } else if (pid == 0) {
            printf("Iniciando consumidor\n");
      srand(getpid());
      while(1){
              // Wait for space in the buffer
        sem_wait(&memoria->sem_empty);

        int rd_num = rand() % 101;
        printf("Produtor escrevendo %d em %d\n", rd_num, memoria->flag_produtor);
        if(memoria->flag_produtor < DATA_SZ){
          memoria->dados[memoria->flag_produtor] = rd_num;
          memoria->flag_produtor++;

          memoria_visualizar(memoria);

          // Signal item availability
            sem_post(&memoria->sem_full);
        }


        sleep(rand() % 3 + 1);  // Simulate time to produce
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
  printf("\nSignal %d, fechando processos\n", sig);
  free_memoria_compartilhada();
  exit(EXIT_SUCCESS);
}


/**
 * detecta um ctr+c para que a memória seja limpa
 */
int free_memoria_compartilhada() {
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




