#include "biblioteca.h"

/* static pra não ir pra pilha (são uns 300 KB) */
static Livro acervo[MAX_LIVROS];
static int total_livros = 0;

static void adicionar_exemplo(int *total, const char *titulo, const char *autor,
                              const char *isbn, const char *genero, int ano) {
    Livro l = {0};
    snprintf(l.titulo, MAX_TITULO, "%s", titulo);
    snprintf(l.autor, MAX_AUTOR, "%s", autor);
    snprintf(l.isbn, MAX_ISBN, "%s", isbn);
    snprintf(l.genero, MAX_GENERO, "%s", genero);
    l.ano_publicacao = ano;
    adicionar_livro(acervo, total, &l);
}

static void popular_exemplos(int *total) {
    printf("Adicionando livros de exemplo...\n");
    adicionar_exemplo(total, "O Programador Pragmático", "David Thomas & Andrew Hunt",
                      "978-0135957059", "Tecnologia", 2019);
    adicionar_exemplo(total, "Clean Code", "Robert C. Martin", "978-0132350884", "Tecnologia", 2008);
    adicionar_exemplo(total, "Dom Casmurro", "Machado de Assis", "978-8535917239", "Literatura", 1899);

    /* uns empréstimos inventados pro relatório não ficar vazio */
    acervo[0].qtd_emprestimos = 5;
    acervo[1].qtd_emprestimos = 8;
    acervo[2].qtd_emprestimos = 12;
    acervo[2].disponivel = 0;

    salvar_livros(ARQUIVO_DB, acervo, *total);
    printf("%d livros de exemplo adicionados!\n", *total);
}

static void mostrar_resultado(int codigo, const char *sucesso) {
    switch (codigo) {
        case OK:                  printf("%s\n", sucesso); break;
        case ERRO_NAO_ENCONTRADO: printf("Livro não encontrado.\n"); break;
        case ERRO_EMPRESTADO:     printf("O livro está emprestado.\n"); break;
        case ERRO_DISPONIVEL:     printf("Este livro não está emprestado.\n"); break;
        default:                  printf("Operação não realizada.\n");
    }
}

static void tela_buscar(CampoBusca campo) {
    char termo[MAX_TITULO];
    int resultados[MAX_LIVROS];

    ler_string(campo == CAMPO_AUTOR ? "Digite o autor (ou parte)" : "Digite o título (ou parte)",
               termo, MAX_TITULO);
    int qtd = buscar_livros(acervo, total_livros, campo, termo, resultados);
    if (qtd == 0) {
        printf("Nenhum livro encontrado.\n");
        return;
    }
    printf("\n%d resultado(s):\n", qtd);
    for (int i = 0; i < qtd; i++) {
        const Livro *l = &acervo[resultados[i]];
        printf("  [%d] %s - %s (%s)\n",
               l->id, l->titulo, l->autor,
               l->disponivel ? "Disponível" : "Emprestado");
    }
}

int main(void) {
    int *total = &total_livros;
    int opcao = -1;

    printf("=================================\n");
    printf("  SISTEMA DE GERENCIAMENTO\n");
    printf("  DE BIBLIOTECA\n");
    printf("=================================\n");

    int carregou = carregar_livros(ARQUIVO_DB, acervo, total);
    if (carregou == 1) {
        printf("Dados carregados: %d livro(s) no acervo.\n", *total);
    } else if (carregou == -1) {
        printf("Aviso: %s está corrompido. Renomeando para %s.bak e começando do zero.\n",
               ARQUIVO_DB, ARQUIVO_DB);
        remove(ARQUIVO_DB ".bak");
        rename(ARQUIVO_DB, ARQUIVO_DB ".bak");
        popular_exemplos(total);
    } else {
        printf("Nenhum dado salvo. Iniciando acervo vazio.\n");
        popular_exemplos(total);
    }

    while (opcao != 0) {
        printf("\n=================================\n");
        printf("  MENU PRINCIPAL\n");
        printf("=================================\n");
        printf(" 1. Cadastrar livro\n");
        printf(" 2. Listar todos os livros\n");
        printf(" 3. Buscar por título\n");
        printf(" 4. Buscar por autor\n");
        printf(" 5. Editar livro\n");
        printf(" 6. Emprestar livro\n");
        printf(" 7. Devolver livro\n");
        printf(" 8. Remover livro\n");
        printf(" 9. Livros disponíveis\n");
        printf("10. Mais emprestados\n");
        printf("11. Resumo do acervo\n");
        printf(" 0. Sair\n\n");

        opcao = ler_inteiro("Escolha", 0, 11);

        switch (opcao) {
            case 1:
                tela_cadastrar(acervo, total);
                break;

            case 2:
                listar_livros(acervo, *total);
                break;

            case 3:
                tela_buscar(CAMPO_TITULO);
                break;

            case 4:
                tela_buscar(CAMPO_AUTOR);
                break;

            case 5:
                tela_editar(acervo, *total);
                break;

            case 6: {
                int id = ler_inteiro("ID do livro para emprestar", 1, 99999);
                int r = emprestar_livro(acervo, *total, id);
                if (r == OK) salvar_livros(ARQUIVO_DB, acervo, *total);
                mostrar_resultado(r, "Livro emprestado com sucesso!");
                break;
            }

            case 7: {
                int id = ler_inteiro("ID do livro para devolver", 1, 99999);
                int r = devolver_livro(acervo, *total, id);
                if (r == OK) salvar_livros(ARQUIVO_DB, acervo, *total);
                mostrar_resultado(r, "Livro devolvido com sucesso!");
                break;
            }

            case 8: {
                int id = ler_inteiro("ID do livro para remover", 1, 99999);
                int r = remover_livro(acervo, total, id);
                if (r == OK) salvar_livros(ARQUIVO_DB, acervo, *total);
                mostrar_resultado(r, "Livro removido com sucesso.");
                break;
            }

            case 9:
                relatorio_disponiveis(acervo, *total);
                break;

            case 10:
                relatorio_mais_emprestados(acervo, *total);
                break;

            case 11:
                relatorio_resumo(acervo, *total);
                break;

            case 0:
                printf("\nSaindo... Dados salvos. Até logo!\n");
                break;
        }

        if (opcao != 0) pausar();
    }

    return 0;
}
