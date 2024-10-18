#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

#define DATA_SZ  10
#define THREE_SZ  100

struct campo_compartilhado{
  int flag_semaforo;  //flag que determina se deve escreber ou nao;
  int flag_contador;
  char dados[DATA_SZ];
  char arvore[THREE_SZ];
};


int free_memoria_compartilhada();
void handle_sigint(int sig);

int arvore_adicionar(int valor);
int arvore_remover(int posicao);
int arvore_adicionar_maior(int valor, int parente);
int arvore_adicionar_menor(int valor, int parente);
void arvore_limpar();
void arvore_visualizar(struct campo_compartilhado* memoria);
int arvore_pesquisar(int valor);

int memoria_compartilhada();




extern void* data;
extern int keyId;
extern int memId;
extern struct campo_compartilhado* memoria;
