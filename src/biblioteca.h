/*
 * biblioteca.h — Cabeçalho do sistema de gerenciamento de biblioteca
 *
 * Em C, separamos declarações (o "quê existe") em arquivos .h (header)
 * e implementações (o "como funciona") em arquivos .c.
 *
 * Isso permite que vários arquivos .c usem as mesmas funções
 * sem copiar o código — basta incluir o .h com #include.
 *
 * #ifndef / #define / #endif: "include guard"
 * Evita que o header seja incluído mais de uma vez no mesmo arquivo.
 */

#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ==========================================
 * Constantes — valores fixos com nome legível
 * ========================================== */
#define MAX_TITULO     150
#define MAX_AUTOR       80
#define MAX_ISBN        20
#define MAX_GENERO      50
#define MAX_LIVROS    1000
#define ARQUIVO_DB  "biblioteca.dat"

/* ==========================================
 * Struct: agrupa campos relacionados em um tipo
 *
 * É o equivalente em C de uma classe simples (só dados, sem métodos).
 * Após o typedef, podemos usar "Livro" no lugar de "struct Livro".
 * ========================================== */
typedef struct {
    int    id;
    char   titulo[MAX_TITULO];
    char   autor[MAX_AUTOR];
    char   isbn[MAX_ISBN];
    char   genero[MAX_GENERO];
    int    ano_publicacao;
    int    disponivel;   /* 1 = disponível, 0 = emprestado */
    int    qtd_emprestimos;
} Livro;

/* ==========================================
 * Declarações de funções (protótipos)
 *
 * O compilador precisa saber a assinatura da função
 * antes de vê-la usada em outro arquivo.
 * ========================================== */

/* Gerenciamento de dados */
int   carregar_livros(Livro livros[], int *total);
int   salvar_livros(Livro livros[], int total);

/* CRUD */
int   cadastrar_livro(Livro livros[], int *total);
void  listar_livros(Livro livros[], int total);
Livro *buscar_por_id(Livro livros[], int total, int id);
int   buscar_por_titulo(Livro livros[], int total, const char *termo, int resultados[], int *qtd);
int   remover_livro(Livro livros[], int *total, int id);

/* Empréstimos */
int   emprestar_livro(Livro livros[], int total, int id);
int   devolver_livro(Livro livros[], int total, int id);

/* Relatórios */
void  relatorio_disponiveis(Livro livros[], int total);
void  relatorio_mais_emprestados(Livro livros[], int total);

/* Utilitários */
void  limpar_buffer(void);
int   ler_inteiro(const char *prompt, int min, int max);
void  ler_string(const char *prompt, char *destino, int tamanho);
void  pausar(void);
void  limpar_tela(void);

#endif /* BIBLIOTECA_H */
