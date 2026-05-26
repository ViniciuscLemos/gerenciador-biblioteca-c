# Gerenciador de Biblioteca — C

Sistema de gerenciamento de acervo via terminal. Cadastra livros, controla empréstimos e persiste dados em arquivo binário.

## Tecnologias
- **C (padrão C17)** — linguagem principal
- **GCC** — compilador
- **Arquivo binário (.dat)** — persistência dos dados sem banco externo

## O que você vai aprender com este projeto
- Structs em C: agrupamento de dados heterogêneos
- Ponteiros: passagem por referência, ponteiro para struct, NULL
- Manipulação de arquivos binários: `fread`, `fwrite`, `fopen`, `fclose`
- Separação entre header (.h) e implementação (.c)
- Makefile: automação da compilação
- Strings em C: `strcpy`, `strstr`, `strlen`, `fgets`
- Buffer overflow: como evitar com `fgets` e `strncpy`

## Pré-requisitos
- GCC instalado (`gcc --version` para verificar)
- Linux/macOS: GCC já vem instalado ou via `sudo apt install gcc`
- Windows: instale MinGW ou use WSL

## Como compilar e rodar

### Com Makefile (recomendado)
```bash
make        # compila
make run    # compila e executa
make clean  # remove executável e dados
```

### Manualmente
```bash
gcc -Wall -std=c17 -o biblioteca src/main.c src/biblioteca.c
./biblioteca
```

## Funcionalidades
- Cadastrar livro com título, autor, ISBN, gênero e ano
- Listar todos os livros com status (disponível/emprestado)
- Buscar por título (busca parcial, ignora maiúsculas/minúsculas)
- Emprestar e devolver livros
- Remover livros do acervo
- Relatório de disponibilidade
- Ranking dos livros mais emprestados

## Persistência
Os dados são salvos automaticamente no arquivo `biblioteca.dat` (binário).
Na próxima execução, o programa carrega os dados do arquivo.

## Estrutura do projeto
```
src/
├── main.c          # Menu principal e loop do programa
├── biblioteca.c    # Implementação de todas as funções
└── biblioteca.h    # Declarações, constantes e a struct Livro
Makefile            # Automação de compilação
```
