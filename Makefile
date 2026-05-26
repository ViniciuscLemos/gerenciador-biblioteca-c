# Makefile — automatiza a compilação do projeto
#
# Em vez de digitar o comando gcc toda vez, use apenas: make
#
# Como funciona:
#   alvo: dependências
#       comando (com TAB obrigatório no início)

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c17 -g
SRC     = src/main.c src/biblioteca.c
TARGET  = biblioteca

# 'make' sem argumento executa o primeiro alvo
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "Compilado com sucesso! Execute com: ./$(TARGET)"

# 'make clean' remove o executável
clean:
	rm -f $(TARGET) biblioteca.dat

# 'make run' compila e executa
run: $(TARGET)
	./$(TARGET)

# Declara que 'all', 'clean' e 'run' não são arquivos
.PHONY: all clean run
