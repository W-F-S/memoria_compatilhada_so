# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Define the executable and source files
TARGET_CAMPO_MEMORIA = processos
SRC_CAMPO_MEMORIA = ./codigos/processos.c ./codigos/lib.c

TARGET_VISUALIZADOR = visualizador
SRC_VISUALIZADOR = ./codigos/visualizador.c ./codigos/lib.c

OBJ = main.o

mensagem = $(shell echo "Execute os arquivos $(TARGET_CAMPO_MEMORIA) e $(TARGET_CAMPO_MEMORIA)")

$(info $(a))

comp:
	touch "./codigos/pid.txt"
	$(CC) $(CFLAGS) $(SRC_CAMPO_MEMORIA) -o $(TARGET_CAMPO_MEMORIA)
	$(CC) $(CFLAGS) $(SRC_VISUALIZADOR) -o $(TARGET_VISUALIZADOR)


#$(TARGET_CAMPO_MEMORIA): $(SRC_CAMPO_MEMORIA)
#	touch "pid.txt"
#	$(CC) $(CFLAGS) $(SRC_CAMPO_MEMORIA) -o $(TARGET_CAMPO_MEMORIA)
#	$(CC) $(CFLAGS) $(SRC_VISUALIZADOR) -o $(TARGET_VISUALIZADOR)

#$(TARGET_VISUALIZADOR): $(SRC_VISUALIZADOR)
#	$(CC) $(CFLAGS) $(SRC_VISUALIZADOR) -o $(TARGET_VISUALIZADOR)


clean:
	rm -f $(TARGET) $(OBJ)

