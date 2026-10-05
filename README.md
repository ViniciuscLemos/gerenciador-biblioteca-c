# Gerenciador de Biblioteca — C

![Testes](https://github.com/ViniciuscLemos/gerenciador-biblioteca-c/actions/workflows/testes.yml/badge.svg)

Sistema de gerenciamento de acervo via terminal. Cadastra, edita e busca livros, controla empréstimos e persiste dados em arquivo binário.

## Tecnologias
- **C (padrão C17)** — linguagem principal
- **GCC / Clang** — compiladores (testados no CI)
- **Arquivo binário (.dat)** — persistência dos dados sem banco externo
- **AddressSanitizer + UBSan** — detecção de erros de memória nos testes

## O que você vai aprender com este projeto
- Structs em C: agrupamento de dados heterogêneos, cópia por atribuição
- Ponteiros: passagem por referência, ponteiro para struct, NULL, ponteiros genéricos (`void *`)
- Manipulação de arquivos binários: `fread`, `fwrite`, `fopen`, `fclose` — e como validar o que foi lido
- Separação entre header (.h) e implementação (.c)
- Separação entre **regras** (testáveis) e **telas** (entrada/saída)
- Ordenação com `qsort` e função de comparação
- Leitura segura de entrada: `fgets`, tratamento de EOF e de linhas longas
- Makefile: compilação, testes e limpeza
- Testes automatizados em C puro, sem framework

## Pré-requisitos
- GCC ou Clang (`gcc --version` para verificar)
- Linux/macOS: `sudo apt install build-essential` / Xcode Command Line Tools
- Windows: [MSYS2](https://www.msys2.org/) (MinGW) ou WSL

## Como compilar e rodar

### Com Makefile (recomendado)
```bash
make        # compila
make run    # compila e executa
make test   # compila e roda os testes
make clean  # remove executáveis e dados
```

### Manualmente
```bash
gcc -Wall -Wextra -std=c17 -o biblioteca src/main.c src/biblioteca.c
./biblioteca
```

## Funcionalidades
- Cadastrar livro com título, autor, ISBN, gênero e ano
- Editar qualquer campo de um livro (Enter mantém o valor atual)
- Listar todos os livros com status (disponível/emprestado)
- Buscar por título ou por autor (busca parcial, ignora maiúsculas/minúsculas)
- Emprestar e devolver livros
- Remover livros do acervo (mantém a ordem de cadastro)
- Relatórios: disponíveis, top 5 mais emprestados e resumo do acervo

## Persistência
Os dados são salvos automaticamente no arquivo `biblioteca.dat` (binário) após cada alteração.
Na próxima execução, o programa carrega os dados do arquivo. Se o arquivo estiver corrompido, ele é renomeado para `biblioteca.dat.bak` e o programa começa com o acervo de exemplo, em vez de travar ou ler dados inválidos.

## Testes
```bash
make test
```
Os testes cobrem cadastro, busca, empréstimo/devolução, remoção, limite do acervo, gravação/leitura do arquivo e arquivos corrompidos. No GitHub Actions eles rodam com GCC e Clang, com AddressSanitizer e UndefinedBehaviorSanitizer, além de uma execução de ponta a ponta do programa.

## Correções da versão 1.1
- `tolower` era usado sem `#include <ctype.h>`
- Um `biblioteca.dat` corrompido podia fazer o `fread` escrever além do fim do array
- Com a entrada encerrada (Ctrl+D/Ctrl+Z), o menu entrava em loop infinito lendo uma variável não inicializada
- Textos maiores que o campo "vazavam" para a próxima pergunta
- Remover um livro embaralhava a ordem da lista

## Estrutura do projeto
```
src/
├── main.c              # Menu principal e loop do programa
├── biblioteca.c        # Regras, telas, relatórios e utilitários de entrada
└── biblioteca.h        # Declarações, constantes e a struct Livro
tests/
└── test_biblioteca.c   # Testes automatizados
Makefile                # Automação de compilação e testes
```
