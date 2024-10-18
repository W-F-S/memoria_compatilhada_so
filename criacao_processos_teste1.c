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
  memoria->flag_semaforo = 0;
  memoria->flag_contador = 0;


  /*
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
        if(memoria->flag_semaforo == 0 && (memoria->flag_contador) < DATA_SZ){
          memoria->flag_semaforo = 1;
          int rd_num = rand() % (100 - 1 + 1); //criando valor aleatorio
          printf("\n processo (%d) Escrendo %d, na posicao %d\n", getpid(), rd_num, memoria->flag_contador);
          memoria->dados[memoria->flag_contador] = rd_num; //sessao critica do programa
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
  */

  arvore_limpar();

  arvore_adicionar(12);
  arvore_adicionar(10);

  arvore_visualizar(memoria);

  arvore_limpar();

  free_memoria_compartilhada();
}

void handle_sigint(int sig) {
  printf("\n\n\n\n");
  printf("\nSignal %d, fechando processos\n", sig);
  printf("\n\n\n\n");
  free_memoria_compartilhada();
  kill(getppid(), SIGCHLD);
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




