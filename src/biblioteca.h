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
#include <ctype.h>   /* tolower() */

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

/* Em qual campo buscar (usado por buscar_livros) */
typedef enum {
    CAMPO_TITULO,
    CAMPO_AUTOR
} CampoBusca;

/* ==========================================
 * Declarações de funções (protótipos)
 *
 * O compilador precisa saber a assinatura da função
 * antes de vê-la usada em outro arquivo.
 * ========================================== */

/* Gerenciamento de dados — o caminho do arquivo é parâmetro para facilitar testes */
int   carregar_livros(const char *caminho, Livro livros[], int *total);
int   salvar_livros(const char *caminho, const Livro livros[], int total);

/* Regras (não leem do teclado — fáceis de testar) */
int   adicionar_livro(Livro livros[], int *total, const Livro *dados);
Livro *buscar_por_id(Livro livros[], int total, int id);
int   buscar_livros(const Livro livros[], int total, CampoBusca campo,
                    const char *termo, int resultados[]);
int   remover_livro(Livro livros[], int *total, int id);
int   emprestar_livro(Livro livros[], int total, int id);
int   devolver_livro(Livro livros[], int total, int id);
int   contem_ignorando_caixa(const char *texto, const char *termo);

/* Códigos de retorno das operações acima */
#define OK                 1
#define ERRO_NAO_ENCONTRADO 0
#define ERRO_EMPRESTADO    -1
#define ERRO_DISPONIVEL    -2
#define ERRO_CHEIO         -3

/* Telas (interagem com o usuário) */
void  tela_cadastrar(Livro livros[], int *total);
void  tela_editar(Livro livros[], int total);
void  listar_livros(const Livro livros[], int total);
void  relatorio_disponiveis(const Livro livros[], int total);
void  relatorio_mais_emprestados(const Livro livros[], int total);
void  relatorio_resumo(const Livro livros[], int total);

/* Utilitários de entrada */
void  limpar_buffer(void);
int   ler_inteiro(const char *prompt, int min, int max);
void  ler_string(const char *prompt, char *destino, int tamanho);
void  pausar(void);

#endif /* BIBLIOTECA_H */
