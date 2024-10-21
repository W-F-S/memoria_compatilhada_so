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
#define THREE_SZ  100

struct campo_compartilhado{
  int flag_produtor;
  int flag_consumidor;

  char dados[DATA_SZ];
  char arvore[THREE_SZ];
  sem_t sem_mutex;           // Semáforo para exclusão mútua
  sem_t sem_full;           // Semáforo para exclusão mútua
  sem_t sem_empty;           // Semáforo para exclusão mútua


};

void handle_sigint(int sig);

int arvore_remover(int posicao);
int arvore_adicionar(int valor);
int arvore_adicionar_maior(int valor, int parente);
int arvore_adicionar_menor(int valor, int parente);
void arvore_limpar();
void arvore_visualizar(struct campo_compartilhado* memoria);
int arvore_pesquisar(int valor);

int free_memoria_compartilhada();
int memoria_compartilhada();
void memoria_shift();
int memoria_adicionar(int valor);

extern void* data;
extern int keyId;
extern int memId;
extern struct campo_compartilhado* memoria;
