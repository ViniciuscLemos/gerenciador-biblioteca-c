# Gerenciador de Biblioteca

![Testes](https://github.com/ViniciuscLemos/gerenciador-biblioteca-c/actions/workflows/testes.yml/badge.svg)

Sistema de biblioteca no terminal, feito em C. Dá pra cadastrar, editar, buscar e remover livros, controlar os empréstimos e ver alguns relatórios.

Os dados ficam salvos num arquivo binário (`biblioteca.dat`) usando `fwrite`/`fread`.

## Compilando

Precisa do gcc (no Windows eu recomendo o MSYS2 ou o WSL).

```bash
make
./biblioteca
```

Ou sem o make:

```bash
gcc -Wall -Wextra -std=c17 -o biblioteca src/main.c src/biblioteca.c
```

Testes:

```bash
make test
```

## O que tem no menu

- cadastrar, editar e remover livros
- listar todos os livros
- buscar por título ou autor (não diferencia maiúsculas de minúsculas)
- emprestar e devolver
- relatórios: livros disponíveis, os 5 mais emprestados e um resumo do acervo

Na primeira vez que roda, ele já cadastra 3 livros de exemplo.

## Arquivos

```
src/main.c          menu
src/biblioteca.c    funções do sistema
src/biblioteca.h    struct Livro e declarações
tests/              testes
Makefile
```
