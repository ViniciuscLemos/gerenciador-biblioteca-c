# make, make test, make run, make clean

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c17 -g
SRC     = src/main.c src/biblioteca.c
HEADERS = src/biblioteca.h
TARGET  = biblioteca
TESTE   = test_biblioteca

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	@echo "Compilado com sucesso! Execute com: ./$(TARGET)"

# no CI passo SANITIZE="-fsanitize=address,undefined" pra pegar erro de memória
test: tests/test_biblioteca.c src/biblioteca.c $(HEADERS)
	$(CC) $(CFLAGS) $(SANITIZE) -o $(TESTE) tests/test_biblioteca.c src/biblioteca.c
	./$(TESTE)

clean:
	rm -f $(TARGET) $(TARGET).exe $(TESTE) $(TESTE).exe biblioteca.dat biblioteca.dat.bak

run: $(TARGET)
	./$(TARGET)

.PHONY: all test clean run
