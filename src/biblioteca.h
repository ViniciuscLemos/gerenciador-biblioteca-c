#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TITULO     150
#define MAX_AUTOR       80
#define MAX_ISBN        20
#define MAX_GENERO      50
#define MAX_LIVROS    1000
#define ARQUIVO_DB  "biblioteca.dat"

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

typedef enum {
    CAMPO_TITULO,
    CAMPO_AUTOR
} CampoBusca;

/* retornos das funções de empréstimo/remoção */
#define OK                  1
#define ERRO_NAO_ENCONTRADO 0
#define ERRO_EMPRESTADO    -1
#define ERRO_DISPONIVEL    -2
#define ERRO_CHEIO         -3

/* arquivo */
int   carregar_livros(const char *caminho, Livro livros[], int *total);
int   salvar_livros(const char *caminho, const Livro livros[], int total);

/* operações (não usam printf/scanf, por isso dá pra testar) */
int   adicionar_livro(Livro livros[], int *total, const Livro *dados);
Livro *buscar_por_id(Livro livros[], int total, int id);
int   buscar_livros(const Livro livros[], int total, CampoBusca campo,
                    const char *termo, int resultados[]);
int   remover_livro(Livro livros[], int *total, int id);
int   emprestar_livro(Livro livros[], int total, int id);
int   devolver_livro(Livro livros[], int total, int id);
int   contem_ignorando_caixa(const char *texto, const char *termo);

/* telas */
void  tela_cadastrar(Livro livros[], int *total);
void  tela_editar(Livro livros[], int total);
void  listar_livros(const Livro livros[], int total);
void  relatorio_disponiveis(const Livro livros[], int total);
void  relatorio_mais_emprestados(const Livro livros[], int total);
void  relatorio_resumo(const Livro livros[], int total);

/* entrada do teclado */
void  limpar_buffer(void);
int   ler_inteiro(const char *prompt, int min, int max);
void  ler_string(const char *prompt, char *destino, int tamanho);
void  pausar(void);

#endif
