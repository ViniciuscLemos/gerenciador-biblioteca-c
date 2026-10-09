# Library Manager

![Tests](https://github.com/ViniciuscLemos/library-manager-c/actions/workflows/tests.yml/badge.svg)

A library system for the terminal, written in C. You can add, edit, search and remove books, keep track of who borrowed what and see a few reports.

The data is saved to a binary file (`library.dat`) using `fwrite`/`fread`.

## Building

You need gcc (on Windows I recommend MSYS2 or WSL).

```bash
make
./library
```

Or without make:

```bash
gcc -Wall -Wextra -std=c17 -o library src/main.c src/library.c
```

Tests:

```bash
make test
```

## What's in the menu

- add, edit and remove books
- list all books
- search by title or author (case insensitive)
- borrow and return
- reports: available books, the 5 most borrowed and a summary of the collection

The first time it runs, it already adds 3 sample books.

## What it looks like

The book list:

```
ID    Title                                Author                Year  Genre         Status
--------------------------------------------------------------------------------------
1     The Pragmatic Programmer             David Thomas & Andre  2019  Technology    Available
2     Clean Code                           Robert C. Martin      2008  Technology    Available
3     Dom Casmurro                         Machado de Assis      1899  Literature    Borrowed

Total: 3 book(s)
```

This table gave me some work. `printf("%-35.35s")` aligns by counting bytes, but in UTF-8 letters like "á" and "ê" take 2 bytes. Because of that, every row with an accented title (like "Memórias Póstumas de Brás Cubas") came out shorter than the others, and when the title went over the limit it could get cut in the middle of a letter, showing a broken character. I wrote `format_column`, which counts characters instead of bytes (every byte that doesn't start with `10` in binary starts a new character). The tests even cover a text that ends in the middle of a character, and in CI they run with AddressSanitizer to make sure nothing is read outside the memory.

## Files

```
src/main.c          menu
src/library.c       the system's functions
src/library.h       Book struct and declarations
tests/              tests
Makefile
```
