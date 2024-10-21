# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Define the executable and source files
TARGET_CAMPO_MEMORIA = processos
SRC_CAMPO_MEMORIA = processos.c lib.c

TARGET_VISUALIZADOR = visualizador
SRC_VISUALIZADOR = visualizador.c lib.c

OBJ = main.o



$(TARGET_CAMPO_MEMORIA): $(SRC_CAMPO_MEMORIA)
	touch "pid.txt"
	$(CC) $(CFLAGS) $(SRC_CAMPO_MEMORIA) -o $(TARGET_CAMPO_MEMORIA)

$(TARGET_VISUALIZADOR): $(SRC_VISUALIZADOR)
	$(CC) $(CFLAGS) $(SRC_VISUALIZADOR) -o $(TARGET_VISUALIZADOR)


# Default rule to build the executable
#$(TARGET): $(OBJ)
#	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule to build object files from source files
#%.o: %.c
#	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(TARGET) $(OBJ)

