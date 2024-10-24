#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>

#define DATA_SZ  10
#define MAX_PROD 3
#define MAX_CONS 3
#define SHM_KEY_PATH "./pid1.txt"
#define SHM_KEY_ID 123

// Estrutura de memória compartilhada
struct campo_compartilhado {
    int dados[DATA_SZ];       // Buffer circular
    int in;                    // Índice de produção
    int out;                   // Índice de consumo
    int ordem;                 // Variável compartilhada para ordenação
    sem_t sem_mutex;           // Semáforo para exclusão mútua
    sem_t sem_full;            // Semáforo para contar itens disponíveis
    sem_t sem_empty;           // Semáforo para contar espaços disponíveis
};

int shmid = 0;
struct campo_compartilhado* memoria = NULL;

// Função para criar e obter a memória compartilhada
int memoria_compartilhada(){
    key_t keytmp = ftok(SHM_KEY_PATH, SHM_KEY_ID);
    if (keytmp == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int shmid_local = shmget(keytmp, sizeof(struct campo_compartilhado), 0666 | IPC_CREAT);
    if (shmid_local == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    return shmid_local;
}

// Função para liberar a memória compartilhada e destruir os semáforos
void free_memoria_compartilhada() {
    if (memoria != (void*)-1 && memoria != NULL) {
        // Destruir os semáforos
        sem_destroy(&memoria->sem_mutex);
        sem_destroy(&memoria->sem_full);
        sem_destroy(&memoria->sem_empty);

        // Desanexar a memória compartilhada
        if (shmdt((void*)memoria) == -1) {
            perror("shmdt");
            // Não saímos para tentar remover a memória compartilhada mesmo em caso de erro
        }

        // Remover a memória compartilhada
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
        }
    }
}

// Manipulador de sinais para garantir a limpeza
void handle_sigint(int sig) {
    printf("\n\nSignal %d received, fechando processos e liberando recursos...\n", sig);
    free_memoria_compartilhada();
    exit(0);
}

// Função para imprimir o estado do buffer
void print_memoria(struct campo_compartilhado* memoria) {
    printf("\nBuffer: [");
    for (int i = 0; i < DATA_SZ; i++) {
        printf("%d", memoria->dados[i]);
        if (i < DATA_SZ - 1) printf(", ");
    }
    printf("]\n");
}

int main(int argc, char *argv[]) {
    printf("Iniciando programa\n");

    // Registro do manipulador de sinais
    signal(SIGINT, handle_sigint);

    // Criação da memória compartilhada
    shmid = memoria_compartilhada();
    memoria = (struct campo_compartilhado*) shmat(shmid, (void *)0, 0);
    if (memoria == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Inicialização da estrutura de memória compartilhada
    memoria->in = 0;
    memoria->out = 0;
    memoria->ordem = 1;
    for (int i = 0; i < DATA_SZ; i++) {
        memoria->dados[i] = 0;
    }

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
        perror("sem_init sem_empty");
        free_memoria_compartilhada();
        exit(EXIT_FAILURE);
    }

    // Variáveis de processo
    pid_t pid;
    int qt_processos_produtores = MAX_PROD;
    int qt_processos_consumidores = MAX_CONS;

    // Criação de consumidores
    for (int i = 0; i < qt_processos_consumidores; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Erro ao criar processo consumidor");
            free_memoria_compartilhada();
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Processo Consumidor
            srand(getpid());
            while (1) {
                sem_wait(&memoria->sem_full); // Espera por itens disponíveis
                sem_wait(&memoria->sem_mutex); // Entra na seção crítica

                // Seção crítica: Consumindo um item
                int item = memoria->dados[memoria->out];
                printf("\n(%d) Processo Consumidor [%d] consumindo %d na posição %d\n",
                       memoria->ordem++, getpid(), item, memoria->out);
                memoria->dados[memoria->out] = 0; // Opcional: limpar a posição
                memoria->out = (memoria->out + 1) % DATA_SZ;

                print_memoria(memoria);

                sem_post(&memoria->sem_mutex); // Sai da seção crítica
                sem_post(&memoria->sem_empty); // Libera espaço no buffer

                printf("----------------------------------------\n");
                sleep(rand() % 3 + 1); // Simula tempo de consumo
            }
        }
    }

    // Criação de produtores
    for (int i = 0; i < qt_processos_produtores; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Erro ao criar processo produtor");
            free_memoria_compartilhada();
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Processo Produtor
            srand(getpid());
            while (1) {
                int rd_num = rand() % 100 + 1; // Valor a ser produzido

                sem_wait(&memoria->sem_empty); // Espera por espaço disponível
                sem_wait(&memoria->sem_mutex); // Entra na seção crítica

                // Seção crítica: Produzindo um item
                memoria->dados[memoria->in] = rd_num;
                printf("\n(%d) Processo Produtor [%d] produzindo %d na posição %d\n",
                       memoria->ordem++, getpid(), rd_num, memoria->in);
                memoria->in = (memoria->in + 1) % DATA_SZ;

                print_memoria(memoria);

                sem_post(&memoria->sem_mutex); // Sai da seção crítica
                sem_post(&memoria->sem_full);  // Indica que há um novo item

                printf("----------------------------------------\n");
                sleep(rand() % 3 + 1); // Simula tempo de produção
            }
        }
    }

    // Processo Pai espera indefinidamente
    while (1) {
        pause(); // Espera por sinais
    }


    free_memoria_compartilhada();
    return 0;
}
