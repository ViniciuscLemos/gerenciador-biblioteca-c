/* make test. Sem framework: o CHECK conta o que passou e mostra o que falhou */

#include "../src/biblioteca.h"

static int passou = 0, falhou = 0;

#define CHECK(cond) do {                                              \
    if (cond) { passou++; }                                           \
    else { falhou++; printf("  FALHOU  %s:%d  %s\n", __FILE__, __LINE__, #cond); } \
} while (0)

static Livro acervo[MAX_LIVROS];
static Livro carregado[MAX_LIVROS];

static Livro novo(const char *titulo, const char *autor, int ano) {
    Livro l = {0};
    snprintf(l.titulo, MAX_TITULO, "%s", titulo);
    snprintf(l.autor, MAX_AUTOR, "%s", autor);
    l.ano_publicacao = ano;
    return l;
}

static int popular(void) {
    int total = 0;
    Livro a = novo("Clean Code", "Robert C. Martin", 2008);
    Livro b = novo("Dom Casmurro", "Machado de Assis", 1899);
    Livro c = novo("Memórias Póstumas de Brás Cubas", "Machado de Assis", 1881);
    adicionar_livro(acervo, &total, &a);
    adicionar_livro(acervo, &total, &b);
    adicionar_livro(acervo, &total, &c);
    return total;
}

static void test_adicionar_gera_ids(void) {
    int total = popular();
    CHECK(total == 3);
    CHECK(acervo[0].id == 1 && acervo[2].id == 3);
    CHECK(acervo[1].disponivel == 1);
    CHECK(acervo[1].qtd_emprestimos == 0);

    /* depois de remover o último, o id volta a ser max+1 */
    CHECK(remover_livro(acervo, &total, 3) == OK);
    Livro d = novo("Novo", "Autor", 2020);
    CHECK(adicionar_livro(acervo, &total, &d) == 3);
}

static void test_busca_sem_diferenciar_caixa(void) {
    int total = popular();
    int r[MAX_LIVROS];

    CHECK(buscar_livros(acervo, total, CAMPO_TITULO, "clean", r) == 1 && r[0] == 0);
    CHECK(buscar_livros(acervo, total, CAMPO_TITULO, "CASMURRO", r) == 1 && r[0] == 1);
    CHECK(buscar_livros(acervo, total, CAMPO_AUTOR, "machado", r) == 2);
    CHECK(buscar_livros(acervo, total, CAMPO_TITULO, "inexistente", r) == 0);
    CHECK(buscar_livros(acervo, total, CAMPO_TITULO, "", r) == 3);

    CHECK(contem_ignorando_caixa("Clean Code", "N CO"));
    CHECK(!contem_ignorando_caixa("abc", "abcd"));
}

static void test_emprestimo_e_devolucao(void) {
    int total = popular();

    CHECK(emprestar_livro(acervo, total, 1) == OK);
    CHECK(acervo[0].disponivel == 0 && acervo[0].qtd_emprestimos == 1);
    CHECK(emprestar_livro(acervo, total, 1) == ERRO_EMPRESTADO);
    CHECK(emprestar_livro(acervo, total, 99) == ERRO_NAO_ENCONTRADO);

    CHECK(devolver_livro(acervo, total, 1) == OK);
    CHECK(devolver_livro(acervo, total, 1) == ERRO_DISPONIVEL);
    CHECK(emprestar_livro(acervo, total, 1) == OK);
    CHECK(acervo[0].qtd_emprestimos == 2);
}

static void test_remover_mantem_ordem(void) {
    int total = popular();

    CHECK(emprestar_livro(acervo, total, 2) == OK);
    CHECK(remover_livro(acervo, &total, 2) == ERRO_EMPRESTADO);
    CHECK(total == 3);

    CHECK(devolver_livro(acervo, total, 2) == OK);
    CHECK(remover_livro(acervo, &total, 1) == OK);
    CHECK(total == 2);
    CHECK(acervo[0].id == 2 && acervo[1].id == 3);
    CHECK(remover_livro(acervo, &total, 1) == ERRO_NAO_ENCONTRADO);
}

static void test_limite_do_acervo(void) {
    int total = MAX_LIVROS;
    Livro l = novo("X", "Y", 2000);
    CHECK(adicionar_livro(acervo, &total, &l) == ERRO_CHEIO);
    CHECK(total == MAX_LIVROS);
}

static void test_salvar_e_carregar(void) {
    const char *arquivo = "test_biblioteca.dat";
    int total = popular();
    emprestar_livro(acervo, total, 3);

    CHECK(salvar_livros(arquivo, acervo, total) == 1);

    int total_lido = -1;
    CHECK(carregar_livros(arquivo, carregado, &total_lido) == 1);
    CHECK(total_lido == 3);
    CHECK(strcmp(carregado[2].titulo, "Memórias Póstumas de Brás Cubas") == 0);
    CHECK(carregado[2].disponivel == 0 && carregado[2].qtd_emprestimos == 1);

    remove(arquivo);
    CHECK(carregar_livros(arquivo, carregado, &total_lido) == 0);
    CHECK(total_lido == 0);
}

static void test_arquivo_corrompido(void) {
    const char *arquivo = "test_corrompido.dat";
    int total_lido = -1;

    /* total maior que o array */
    FILE *f = fopen(arquivo, "wb");
    int falso = MAX_LIVROS * 50;
    fwrite(&falso, sizeof(int), 1, f);
    fclose(f);
    CHECK(carregar_livros(arquivo, carregado, &total_lido) == -1);
    CHECK(total_lido == 0);

    /* diz que tem 2 livros mas o arquivo acaba antes */
    f = fopen(arquivo, "wb");
    int dois = 2;
    fwrite(&dois, sizeof(int), 1, f);
    fwrite(&acervo[0], sizeof(Livro), 1, f);
    fclose(f);
    CHECK(carregar_livros(arquivo, carregado, &total_lido) == -1);

    remove(arquivo);
}

static void test_coluna_conta_caracteres_e_nao_bytes(void) {
    char buf[TAM_COLUNA(10)];

    /* "ç" e "ã" têm 2 bytes cada, mas contam como 1 caractere */
    formatar_coluna(buf, "Ação", 6);
    CHECK(strcmp(buf, "Ação  ") == 0);

    /* corta em 8 caracteres, sem partir o "ó" no meio */
    formatar_coluna(buf, "Memórias Póstumas", 8);
    CHECK(strcmp(buf, "Memórias") == 0);

    formatar_coluna(buf, "", 3);
    CHECK(strcmp(buf, "   ") == 0);

    /* texto que termina no meio de um caractere não pode ler além do fim */
    formatar_coluna(buf, "a\xc3", 4);
    CHECK(strcmp(buf, "a\xc3  ") == 0);
}

int main(void) {
    test_adicionar_gera_ids();
    test_busca_sem_diferenciar_caixa();
    test_emprestimo_e_devolucao();
    test_remover_mantem_ordem();
    test_limite_do_acervo();
    test_salvar_e_carregar();
    test_arquivo_corrompido();
    test_coluna_conta_caracteres_e_nao_bytes();

    printf("\nResultado: %d verificação(ões) passaram, %d falharam.\n", passou, falhou);
    return falhou == 0 ? 0 : 1;
}
