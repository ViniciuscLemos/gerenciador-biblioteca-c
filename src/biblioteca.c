#include "biblioteca.h"

/* ---------- arquivo ---------- */

/* formato do .dat: um int com o total e depois os structs em sequência.
 * retorna 1 se carregou, 0 se o arquivo não existe e -1 se está corrompido */
int carregar_livros(const char *caminho, Livro livros[], int *total) {
    *total = 0;
    FILE *arquivo = fopen(caminho, "rb");
    if (arquivo == NULL) {
        return 0;
    }

    int lido;
    if (fread(&lido, sizeof(int), 1, arquivo) != 1 || lido < 0 || lido > MAX_LIVROS) {
        fclose(arquivo);
        return -1;
    }

    if (fread(livros, sizeof(Livro), (size_t)lido, arquivo) != (size_t)lido) {
        fclose(arquivo);
        return -1;
    }

    fclose(arquivo);
    *total = lido;
    return 1;
}

int salvar_livros(const char *caminho, const Livro livros[], int total) {
    FILE *arquivo = fopen(caminho, "wb");
    if (arquivo == NULL) {
        printf("Erro: não foi possível salvar os dados.\n");
        return 0;
    }

    int ok = fwrite(&total, sizeof(int), 1, arquivo) == 1
          && fwrite(livros, sizeof(Livro), (size_t)total, arquivo) == (size_t)total;

    if (fclose(arquivo) != 0) ok = 0;
    if (!ok) printf("Erro: falha ao gravar %s.\n", caminho);
    return ok;
}

/* ---------- operações ---------- */

/* copia *dados pro acervo com um id novo (maior id + 1). retorna o id ou ERRO_CHEIO */
int adicionar_livro(Livro livros[], int *total, const Livro *dados) {
    if (*total >= MAX_LIVROS) {
        return ERRO_CHEIO;
    }

    int maior_id = 0;
    for (int i = 0; i < *total; i++) {
        if (livros[i].id > maior_id) maior_id = livros[i].id;
    }

    Livro *novo = &livros[*total];
    *novo = *dados;
    novo->id = maior_id + 1;
    novo->disponivel = 1;
    novo->qtd_emprestimos = 0;

    (*total)++;
    return novo->id;
}

Livro *buscar_por_id(Livro livros[], int total, int id) {
    for (int i = 0; i < total; i++) {
        if (livros[i].id == id) {
            return &livros[i];
        }
    }
    return NULL;
}

/* strstr que não diferencia maiúscula de minúscula */
int contem_ignorando_caixa(const char *texto, const char *termo) {
    if (*termo == '\0') return 1;

    for (; *texto; texto++) {
        const char *t = texto, *p = termo;
        while (*t && *p && tolower((unsigned char)*t) == tolower((unsigned char)*p)) {
            t++;
            p++;
        }
        if (*p == '\0') return 1;
    }
    return 0;
}

/* Corta o texto em `largura` caracteres e completa com espaços.
 * O printf("%-35.35s") conta bytes, e em UTF-8 o "ó" ocupa 2: a tabela ficava torta
 * e um título comprido podia ser cortado no meio de uma letra acentuada. Aqui eu conto
 * caracteres: todo byte que não é de continuação (10xxxxxx) começa um caractere novo.
 * `destino` precisa ter TAM_COLUNA(largura) bytes. */
void formatar_coluna(char *destino, const char *texto, int largura) {
    const unsigned char *p = (const unsigned char *)texto;
    int caracteres = 0;

    while (*p && caracteres < largura) {
        int bytes = *p >= 0xF0 ? 4 : *p >= 0xE0 ? 3 : *p >= 0xC0 ? 2 : 1;
        /* o *p no for evita passar do fim se o texto terminar no meio de um caractere */
        for (int i = 0; i < bytes && *p; i++) *destino++ = (char)*p++;
        caracteres++;
    }
    while (caracteres++ < largura) *destino++ = ' ';
    *destino = '\0';
}

/* preenche resultados[] com os índices encontrados e retorna quantos achou */
int buscar_livros(const Livro livros[], int total, CampoBusca campo,
                  const char *termo, int resultados[]) {
    int qtd = 0;
    for (int i = 0; i < total; i++) {
        const char *texto = campo == CAMPO_AUTOR ? livros[i].autor : livros[i].titulo;
        if (contem_ignorando_caixa(texto, termo)) {
            resultados[qtd++] = i;
        }
    }
    return qtd;
}

int remover_livro(Livro livros[], int *total, int id) {
    for (int i = 0; i < *total; i++) {
        if (livros[i].id == id) {
            if (!livros[i].disponivel) {
                return ERRO_EMPRESTADO;
            }
            /* puxa os próximos uma posição pra trás, mantendo a ordem */
            memmove(&livros[i], &livros[i + 1], (size_t)(*total - i - 1) * sizeof(Livro));
            (*total)--;
            return OK;
        }
    }
    return ERRO_NAO_ENCONTRADO;
}

int emprestar_livro(Livro livros[], int total, int id) {
    Livro *livro = buscar_por_id(livros, total, id);
    if (livro == NULL) return ERRO_NAO_ENCONTRADO;
    if (!livro->disponivel) return ERRO_EMPRESTADO;

    livro->disponivel = 0;
    livro->qtd_emprestimos++;
    return OK;
}

int devolver_livro(Livro livros[], int total, int id) {
    Livro *livro = buscar_por_id(livros, total, id);
    if (livro == NULL) return ERRO_NAO_ENCONTRADO;
    if (livro->disponivel) return ERRO_DISPONIVEL;

    livro->disponivel = 1;
    return OK;
}

/* ---------- telas ---------- */

void tela_cadastrar(Livro livros[], int *total) {
    if (*total >= MAX_LIVROS) {
        printf("Biblioteca cheia! Limite de %d livros atingido.\n", MAX_LIVROS);
        return;
    }

    Livro dados = {0};
    printf("\n--- CADASTRAR LIVRO ---\n");
    do {
        ler_string("Título", dados.titulo, MAX_TITULO);
    } while (dados.titulo[0] == '\0');
    ler_string("Autor", dados.autor, MAX_AUTOR);
    ler_string("ISBN", dados.isbn, MAX_ISBN);
    ler_string("Gênero", dados.genero, MAX_GENERO);
    dados.ano_publicacao = ler_inteiro("Ano de publicação", 1000, 2100);

    int id = adicionar_livro(livros, total, &dados);
    salvar_livros(ARQUIVO_DB, livros, *total);
    printf("\nLivro cadastrado! ID: %d\n", id);
}

/* mostra o valor atual e lê o novo; Enter mantém. A leitura já é limitada
 * a `tamanho`, então o texto sempre cabe no destino. */
static void editar_campo(const char *rotulo_atual, const char *rotulo_novo,
                         char *destino, int tamanho) {
    char entrada[MAX_TITULO];
    printf("%s: %s\n", rotulo_atual, destino);
    ler_string(rotulo_novo, entrada, tamanho);
    if (entrada[0]) memcpy(destino, entrada, strlen(entrada) + 1);
}

void tela_editar(Livro livros[], int total) {
    int id = ler_inteiro("ID do livro para editar", 1, 99999);
    Livro *livro = buscar_por_id(livros, total, id);
    if (livro == NULL) {
        printf("Livro com ID %d não encontrado.\n", id);
        return;
    }

    char entrada[8];
    printf("\n--- EDITAR LIVRO %d --- (Enter mantém o valor atual)\n", id);

    editar_campo("Título atual", "Novo título", livro->titulo, MAX_TITULO);
    editar_campo("Autor atual", "Novo autor", livro->autor, MAX_AUTOR);
    editar_campo("ISBN atual", "Novo ISBN", livro->isbn, MAX_ISBN);
    editar_campo("Gênero atual", "Novo gênero", livro->genero, MAX_GENERO);

    printf("Ano atual: %d\n", livro->ano_publicacao);
    ler_string("Novo ano", entrada, (int)sizeof(entrada));
    if (entrada[0]) {
        int ano = atoi(entrada);
        if (ano >= 1000 && ano <= 2100) livro->ano_publicacao = ano;
        else printf("Ano inválido, mantido %d.\n", livro->ano_publicacao);
    }

    salvar_livros(ARQUIVO_DB, livros, total);
    printf("Livro atualizado!\n");
}

void listar_livros(const Livro livros[], int total) {
    if (total == 0) {
        printf("\nNenhum livro cadastrado.\n");
        return;
    }

    char titulo[TAM_COLUNA(35)], autor[TAM_COLUNA(20)], genero[TAM_COLUNA(12)];

    formatar_coluna(titulo, "Título", 35);
    formatar_coluna(genero, "Gênero", 12);
    printf("\n%-4s  %s  %-20s  %-4s  %s  %s\n", "ID", titulo, "Autor", "Ano", genero, "Status");
    printf("%s\n", "--------------------------------------------------------------------------------------");

    for (int i = 0; i < total; i++) {
        const Livro *l = &livros[i];
        formatar_coluna(titulo, l->titulo, 35);
        formatar_coluna(autor, l->autor, 20);
        formatar_coluna(genero, l->genero, 12);
        printf("%-4d  %s  %s  %-4d  %s  %s\n",
               l->id, titulo, autor, l->ano_publicacao, genero,
               l->disponivel ? "Disponível" : "Emprestado");
    }

    printf("\nTotal: %d livro(s)\n", total);
}

/* ---------- relatórios ---------- */

void relatorio_disponiveis(const Livro livros[], int total) {
    int count = 0;
    printf("\n--- LIVROS DISPONÍVEIS ---\n");
    for (int i = 0; i < total; i++) {
        if (livros[i].disponivel) {
            printf("  [%d] %s - %s (%d)\n",
                   livros[i].id, livros[i].titulo,
                   livros[i].autor, livros[i].ano_publicacao);
            count++;
        }
    }
    printf("\nTotal disponível: %d de %d\n", count, total);
}

/* qsort recebe ponteiros pros elementos; aqui cada elemento já é um Livro* */
static int comparar_emprestimos(const void *a, const void *b) {
    const Livro *la = *(const Livro *const *)a;
    const Livro *lb = *(const Livro *const *)b;
    return lb->qtd_emprestimos - la->qtd_emprestimos;
}

void relatorio_mais_emprestados(const Livro livros[], int total) {
    if (total == 0) { printf("Nenhum livro cadastrado.\n"); return; }

    /* ordena um vetor de ponteiros pra não ficar copiando os structs */
    const Livro *ordenados[MAX_LIVROS];
    for (int i = 0; i < total; i++) ordenados[i] = &livros[i];
    qsort(ordenados, (size_t)total, sizeof(ordenados[0]), comparar_emprestimos);

    int limite = total < 5 ? total : 5;
    printf("\n--- TOP %d MAIS EMPRESTADOS ---\n", limite);
    for (int i = 0; i < limite; i++) {
        printf("  %d. [%dx] %s - %s\n",
               i + 1, ordenados[i]->qtd_emprestimos,
               ordenados[i]->titulo, ordenados[i]->autor);
    }
}

void relatorio_resumo(const Livro livros[], int total) {
    int emprestados = 0, total_emprestimos = 0;
    int mais_antigo = -1;
    for (int i = 0; i < total; i++) {
        if (!livros[i].disponivel) emprestados++;
        total_emprestimos += livros[i].qtd_emprestimos;
        if (mais_antigo < 0 || livros[i].ano_publicacao < livros[mais_antigo].ano_publicacao) {
            mais_antigo = i;
        }
    }

    printf("\n--- RESUMO DO ACERVO ---\n");
    printf("  Livros cadastrados:     %d\n", total);
    printf("  Disponíveis:            %d\n", total - emprestados);
    printf("  Emprestados agora:      %d\n", emprestados);
    printf("  Empréstimos (histórico): %d\n", total_emprestimos);
    if (mais_antigo >= 0) {
        printf("  Mais antigo:            %s (%d)\n",
               livros[mais_antigo].titulo, livros[mais_antigo].ano_publicacao);
    }
}

/* ---------- entrada ---------- */

/* Ctrl+D / Ctrl+Z: sai em vez de ficar lendo pra sempre */
static void fim_da_entrada(void) {
    printf("\nEntrada encerrada. Os dados já estão salvos. Até logo!\n");
    exit(0);
}

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int ler_inteiro(const char *prompt, int min, int max) {
    int valor;
    for (;;) {
        printf("%s (%d-%d): ", prompt, min, max);
        int lidos = scanf("%d", &valor);
        if (lidos == EOF) fim_da_entrada();
        limpar_buffer();
        if (lidos == 1 && valor >= min && valor <= max) return valor;
        printf("Entrada inválida. ");
    }
}

void ler_string(const char *prompt, char *destino, int tamanho) {
    printf("%s: ", prompt);
    if (fgets(destino, tamanho, stdin) == NULL) fim_da_entrada();

    size_t len = strcspn(destino, "\n");
    if (destino[len] == '\n') {
        destino[len] = '\0';
    } else {
        /* linha maior que o buffer: joga o resto fora pra não cair na próxima pergunta */
        limpar_buffer();
    }
}

void pausar(void) {
    printf("\nPressione Enter para continuar...");
    int c = getchar();
    if (c == EOF) fim_da_entrada();
    if (c != '\n') limpar_buffer();
}
