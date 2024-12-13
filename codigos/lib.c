#include "includes.h"
/**
considero 0 como um valor vazio para arvore.

o primeiro valor adicionado na árvore vai ser sempre na raiz/posicao[0]
 */


int memoria_compartilhada(){
  key_t keytmp = ftok("./pid1.txt", keyId);
  int shmid = shmget(keytmp, sizeof(struct campo_compartilhado), 0666 | IPC_CREAT);

  return (shmid);
}

void memoria_visualizar(struct campo_compartilhado* memoria){
  printf("\n");
  for(int i=0; i<DATA_SZ; i++){
    printf("[%d]", memoria->dados[i]);
  }
  printf("\n");
}


void arvore_visualizar(struct campo_compartilhado* memoria){
  printf("\n");
  for(int i=0; i<THREE_SZ; i++){
    printf("[%d]", memoria->arvore[i]);
  }
  printf("\n");
}

void arvore_limpar(struct campo_compartilhado* memoria){
  for(int i=1; i<THREE_SZ; i++){
    memoria->arvore[i] = 0;
  }
}

int arvore_adicionar (int valor){
    int pos = -1;
    if(valor < 0){ //ver pq um -1 é adicionado
    }
    if(memoria->arvore[0] == 0){
      memoria->arvore[0] = valor;
    }else if (memoria->arvore[0] > valor){
      //adicionar maior
      pos = arvore_adicionar_maior(valor, 1);

    }else if (memoria->arvore[0] < valor){
      //adicionar menor
      pos = arvore_adicionar_menor(valor, 0);
    }else{
      printf("erro ao adicionar valor %d na arvore\n", valor);
    }

    return pos;
}


int arvore_remover (int valor){
  return -1;
}

int arvore_adicionar_maior(int valor, int parente){
  int pos = -1;

  if(parente >= THREE_SZ){
    //printf("Erro ao adicionar novo valor direita na pos %d\n", parente+2);
    printf("(arvore_adicionar_maior) Tentando adicionar fora dos limites [%d]\n", parente+2);
  }else if(memoria->arvore[(parente)] == 0){ 
    pos = memoria->arvore[(parente)] = valor;
  }else if(memoria->arvore[(parente)] < valor){
    pos = arvore_adicionar_maior(valor, (2*parente)+2);
  }else{
    pos = arvore_adicionar_menor(valor, (2*parente)+1);
  }

  return pos;
}

int arvore_adicionar_menor(int valor, int parente){
  int pos = -1;

  if(valor < 0){
  }else if(parente >= THREE_SZ){
    //printf("Erro ao adicionar novo valor direita, %d\n", parente+2);
    printf("(arvore_adicionar_menor) Tentando adicionar fora dos limites [%d]\n", parente+2);
  }else if(memoria->arvore[(parente)] == 0){
    pos = memoria->arvore[(parente)] = valor;
  }else if(memoria->arvore[(parente)] < valor){
    pos = arvore_adicionar_maior(valor, (2*parente)+2);
  }else{
    pos = arvore_adicionar_menor(valor, (2*parente)+1);
  }

  return pos;
}

void memoria_shift(){
    int i, j = 0;
    for (i = 0; i < DATA_SZ; i++) {
        if (memoria->dados[i] != -1) {
            memoria->dados[j++] = memoria->dados[i];
        }
    }
    while (j < DATA_SZ) {
        memoria->dados[j++] = -1;
    }
}
