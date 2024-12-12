#include "includes.h"

volatile sig_atomic_t stop = 0;
void* data = NULL;
int keyId = 123;
int memId = 0;
struct campo_compartilhado* memoria = NULL;
struct campo_compartilhado* visualizador = NULL;

void handle_sigint(int sig) {
  stop = 1;
}

int main() {
  int input;
  int num = 0;

  memId = memoria_compartilhada();
  data =  shmat( memId, (void *)0, 0); //atrelando a mem compatilhada a um ponteiro
  visualizador = (struct campo_compartilhado*) data; //;

  signal(SIGINT, handle_sigint);

  while (!stop) {
    printf("0, sair;\n1, mostrar arvore\n2, adicionar valor arvore\n3, remover valor da arvore\n4, limpar arvore\n");
    scanf("%d", &input);

    switch (input) {
      case 0:
        return 0;
      break;

      case 1:
        arvore_visualizar(visualizador);
        printf("\n");
      break;

      case 2:
        sem_wait(&visualizador->sem_empty); // Espera por espaço disponível
        sem_wait(&visualizador->sem_mutex); // Entra na seção crítica

        printf("\nDigite um número: \n");
        scanf("%d", &num);
        arvore_adicionar(num);

        if(visualizador->flag_produtor + 1 >= DATA_SZ){
          sem_post(&visualizador->sem_full);  // Indica que há um novo item
        }else{
          visualizador->flag_produtor++;
        }

        sem_post(&visualizador->sem_mutex);
      break;

      case 3:
        printf("\nDigite uma posicao a ser removida: \n");
        scanf("%d", &num);
        arvore_remover(num);
      break;

      case 4:
        arvore_limpar(visualizador);
      break;

     // case 5: //pausar codigo

      default:
        printf("Nenhuma função associada ao valor %d\n", input);
        break;
    }
  }

  return 0;
}
