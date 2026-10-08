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

## Como fica

A listagem do acervo:

```
ID    Título                               Autor                 Ano   Gênero        Status
--------------------------------------------------------------------------------------
1     O Programador Pragmático             David Thomas & Andre  2019  Tecnologia    Disponível
2     Clean Code                           Robert C. Martin      2008  Tecnologia    Disponível
3     Dom Casmurro                         Machado de Assis      1899  Literatura    Emprestado

Total: 3 livro(s)
```

Essa tabela me deu trabalho. O `printf("%-35.35s")` alinha contando bytes, só que em UTF-8 letras como "á" e "ê" ocupam 2 bytes. Com isso, toda linha com acento ficava mais curta que as outras, e quando o título passava do limite ele podia ser cortado no meio de uma letra, aparecendo um caractere quebrado. Escrevi a `formatar_coluna`, que conta caracteres em vez de bytes (todo byte que não começa com `10` em binário inicia um caractere novo). Os testes cobrem até o caso de um texto que termina no meio de um caractere, e no CI eles rodam com o AddressSanitizer pra garantir que nada é lido fora da memória.

## Arquivos

```
src/main.c          menu
src/biblioteca.c    funções do sistema
src/biblioteca.h    struct Livro e declarações
tests/              testes
Makefile
```
