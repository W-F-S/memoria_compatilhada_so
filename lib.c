#include "includes.h"



int arvore_adicionar (int valor){
    if(memoria->arvore[0] == 0){
      memoria->arvore[0] = valor;
    }else if (memoria->arvore[0] > valor){
      //adicionar maior
      arvore_adicionar_maior(valor, 1);

    }else if (memoria->arvore[0] < valor){
      //adicionar menor
      arvore_adicionar_menor(valor, 0);
    }else{
      printf("erro ao adicionar valor %d na arvore", valor);
    }
}

int arvore_adicionar_maior(int valor, int parente){
  if(parente >= THREE_SZ){
    printf("Erro ao adicionar novo valor direita, %d", parente+2);
  }else if(memoria->arvore[(parente)] == 0){
    memoria->arvore[(parente)] = valor;
  }else if(memoria->arvore[(parente)] < valor){
    arvore_adicionar_maior(valor, (2*parente)+2);
  }else{
    arvore_adicionar_menor(valor, (2*parente)+1);
  }
}

int arvore_adicionar_menor(int valor, int parente){
  if(parente >= THREE_SZ){
    printf("Erro ao adicionar novo valor direita, %d", parente+2);
  }else if(memoria->arvore[(parente)] == 0){
    memoria->arvore[(parente)] = valor;
  }else if(memoria->arvore[(parente)] < valor){
    arvore_adicionar_maior(valor, (2*parente)+2);
  }else{
    arvore_adicionar_menor(valor, (2*parente)+1);
  }
}


int memoria_compartilhada(){
  key_t keytmp = ftok("./pid1.txt", keyId);
  int shmid = shmget(keytmp, sizeof(struct campo_compartilhado), 0666 | IPC_CREAT);

  return (shmid);
}

void print_memoria(struct campo_compartilhado* memoria){
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

void arvore_limpar(){
  for(int i=0; i<THREE_SZ; i++){
    memoria->arvore[i] = 0;
  }
}
