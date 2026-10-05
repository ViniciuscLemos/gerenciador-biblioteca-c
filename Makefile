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
HEADERS = src/biblioteca.h
TARGET  = biblioteca
TESTE   = test_biblioteca

# 'make' sem argumento executa o primeiro alvo
all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "Compilado com sucesso! Execute com: ./$(TARGET)"

# 'make test' compila e roda os testes automatizados
# -fsanitize detecta acessos fora do array e outros erros de memória (Linux/macOS)
test: tests/test_biblioteca.c src/biblioteca.c $(HEADERS)
	$(CC) $(CFLAGS) $(SANITIZE) -o $(TESTE) tests/test_biblioteca.c src/biblioteca.c
	./$(TESTE)

# 'make clean' remove executáveis e dados
clean:
	rm -f $(TARGET) $(TARGET).exe $(TESTE) $(TESTE).exe biblioteca.dat biblioteca.dat.bak

# 'make run' compila e executa
run: $(TARGET)
	./$(TARGET)

# Declara que estes alvos não são arquivos
.PHONY: all test clean run
