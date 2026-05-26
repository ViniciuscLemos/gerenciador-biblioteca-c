/*
 * main.c — Ponto de entrada do sistema de biblioteca
 *
 * A função main() é obrigatória em todo programa C.
 * Ela retorna um int: 0 significa sucesso, outro valor = erro.
 *
 * Aqui fica apenas o menu e o loop principal.
 * Toda a lógica está em biblioteca.c.
 */

#include "biblioteca.h"

int main(void) {
    /* Array estático de livros — memória alocada em compile time */
    Livro acervo[MAX_LIVROS];
    int total = 0;
    int opcao;

    printf("=================================\n");
    printf("  SISTEMA DE GERENCIAMENTO\n");
    printf("  DE BIBLIOTECA\n");
    printf("=================================\n");

    /* Carrega dados do arquivo ao iniciar */
    if (carregar_livros(acervo, &total)) {
        printf("Dados carregados: %d livro(s) no acervo.\n", total);
    } else {
        printf("Nenhum dado salvo. Iniciando acervo vazio.\n");
        /* Popula com alguns livros de exemplo */
        printf("Adicionando livros de exemplo...\n");
        /* Cadastro direto para não precisar de input do usuário */
        strcpy(acervo[0].titulo, "O Programador Pragmático");
        strcpy(acervo[0].autor, "David Thomas & Andrew Hunt");
        strcpy(acervo[0].isbn, "978-0135957059");
        strcpy(acervo[0].genero, "Tecnologia");
        acervo[0].id = 1; acervo[0].ano_publicacao = 2019;
        acervo[0].disponivel = 1; acervo[0].qtd_emprestimos = 5;

        strcpy(acervo[1].titulo, "Clean Code");
        strcpy(acervo[1].autor, "Robert C. Martin");
        strcpy(acervo[1].isbn, "978-0132350884");
        strcpy(acervo[1].genero, "Tecnologia");
        acervo[1].id = 2; acervo[1].ano_publicacao = 2008;
        acervo[1].disponivel = 1; acervo[1].qtd_emprestimos = 8;

        strcpy(acervo[2].titulo, "Dom Casmurro");
        strcpy(acervo[2].autor, "Machado de Assis");
        strcpy(acervo[2].isbn, "978-8535917239");
        strcpy(acervo[2].genero, "Literatura");
        acervo[2].id = 3; acervo[2].ano_publicacao = 1899;
        acervo[2].disponivel = 0; acervo[2].qtd_emprestimos = 12;

        total = 3;
        salvar_livros(acervo, total);
        printf("3 livros de exemplo adicionados!\n");
    }

    /* Loop principal do menu */
    do {
        printf("\n=================================\n");
        printf("  MENU PRINCIPAL\n");
        printf("=================================\n");
        printf("1. Cadastrar livro\n");
        printf("2. Listar todos os livros\n");
        printf("3. Buscar por titulo\n");
        printf("4. Emprestar livro\n");
        printf("5. Devolver livro\n");
        printf("6. Remover livro\n");
        printf("7. Livros disponiveis\n");
        printf("8. Mais emprestados\n");
        printf("0. Sair\n");
        printf("\nEscolha: ");

        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            printf("Opcao invalida.\n");
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                cadastrar_livro(acervo, &total);
                pausar();
                break;

            case 2:
                listar_livros(acervo, total);
                pausar();
                break;

            case 3: {
                char termo[MAX_TITULO];
                int resultados[MAX_LIVROS], qtd;
                ler_string("Digite o titulo (ou parte)", termo, MAX_TITULO);
                buscar_por_titulo(acervo, total, termo, resultados, &qtd);
                if (qtd == 0) {
                    printf("Nenhum livro encontrado.\n");
                } else {
                    printf("\n%d resultado(s):\n", qtd);
                    for (int i = 0; i < qtd; i++) {
                        Livro *l = &acervo[resultados[i]];
                        printf("  [%d] %s — %s (%s)\n",
                               l->id, l->titulo, l->autor,
                               l->disponivel ? "Disponivel" : "Emprestado");
                    }
                }
                pausar();
                break;
            }

            case 4: {
                int id = ler_inteiro("ID do livro para emprestar", 1, 99999);
                emprestar_livro(acervo, total, id);
                pausar();
                break;
            }

            case 5: {
                int id = ler_inteiro("ID do livro para devolver", 1, 99999);
                devolver_livro(acervo, total, id);
                pausar();
                break;
            }

            case 6: {
                int id = ler_inteiro("ID do livro para remover", 1, 99999);
                remover_livro(acervo, &total, id);
                pausar();
                break;
            }

            case 7:
                relatorio_disponiveis(acervo, total);
                pausar();
                break;

            case 8:
                relatorio_mais_emprestados(acervo, total);
                pausar();
                break;

            case 0:
                printf("\nSaindo... Dados salvos. Ate logo!\n");
                break;

            default:
                printf("Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;  /* 0 = programa terminou com sucesso */
}
