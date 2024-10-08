#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>

#define DATA_SZ  10
int keyId = 123;
int memId = 0;
void* data = NULL;
sem_t sem_empty;  // Contador de espaços vazios no buffer
sem_t sem_full;   // Contador de itens no buffer
sem_t sem_mutex;  // Mutex para proteger a seção crítica

struct campo_compartilhado{
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

void print_memoria(struct campo_compartilhado* memoria) {
  printf("\n");
  for (int i = 0; i < DATA_SZ; i++) {
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

int main(int argc, char *argv[]) {
  printf("iniciando programa\n");

  // Variáveis de processo
  pid_t pid;
  int qt_processos_produtores = 3;
  int qt_processos_consumidores = 1;
  int ordem = 1;
  struct campo_compartilhado* memoria;

  // Inicialização dos semáforos
  sem_init(&sem_empty, 1, DATA_SZ); // Inicializa semáforo com o número de espaços vazios
  sem_init(&sem_full, 1, 0);        // Inicializa semáforo com o número de itens no buffer (0)
  sem_init(&sem_mutex, 1, 1);       // Mutex inicializado para garantir exclusão mútua

  signal(SIGINT, handle_sigint); // Captura do sinal

  memId = memoria_compartilhada();
  data = shmat(memId, (void *)0, 0); // Atrelando a memória compartilhada a um ponteiro

  if (data == (void *)-1) {
    printf("\nErro ao criar campo de memoria\n");
    exit(-1);
  }

  memoria = (struct campo_compartilhado*)data;

  // Inicializa o buffer com zeros
  for (int i = 0; i < DATA_SZ; i++) {
    memoria->dados[i] = 0;
  }

  // Criando consumidores
  for (int i = 0; i < qt_processos_consumidores; i++) {
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo consumidor\n");
    } else if (pid == 0) {
      srand(getpid()); //gerando novo seed randomico para leitor
      while (1) {
        printf("Tentando ler\n");
        sem_wait(&sem_mutex); // Entra na seção crítica, bloqueia processo caso <= 0

        // sessao critica
        int rd_pos = rand() % (DATA_SZ - 1); // Seleciona posição aleatória
        printf("\n(%d) Processo (%d) lendo [%d], %d\n", ordem++, getpid(), memoria->dados[rd_pos], rd_pos);
        if (memoria->dados[rd_pos] > 0) {
          memoria->dados[rd_pos] = -1; // Consumir o item
        }

        print_memoria(memoria);
        printf("\n");

        sem_post(&sem_mutex);

        printf("----------------------------------------\n");

        sleep(rand() % (5)); //diminuir esse valor para dar preferencia para leitor;
      }
    }
  }

  // Criando produtores
  for (int i = 0; i < qt_processos_produtores; i++) {
    pid = fork();
    if (pid < 0) {
      printf("\nErro ao criar processo produtor\n");
    } else if (pid == 0) {
      srand(getpid()); //gerando novo seed randomico para produtor
      while (1) {
        printf("Tentando escrever\n");
        sem_wait(&sem_mutex); // Entra na seção crítica, bloqueia processo caso <= 0

        // Sessão crítica: Produzindo um item
        int rd_pos = rand() % (DATA_SZ - 1); // Seleciona posição aleatória
        int rd_num = rand() % 100;     // Cria um valor aleatório
        printf("\n(%d) Processo (%d) escrevendo %d na posição %d\n", ordem++, getpid(), rd_num, rd_pos);
        memoria->dados[rd_pos] = rd_num;

        print_memoria(memoria);
        printf("\n");

        sem_post(&sem_mutex); //liberando mutex/semaforo
        printf("----------------------------------------\n");
        sleep(rand() % (5));
      }
    }
  }

  // Mantendo o processo principal em execução
  while (1) {
    sleep(1);
  }

  // Destrói os semáforos ao final (não será alcançado nesse loop infinito)
  sem_destroy(&sem_empty);
  sem_destroy(&sem_full);
  sem_destroy(&sem_mutex);

  return 0;
}
