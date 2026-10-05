/*
 * biblioteca.c — Implementação das funções do sistema de biblioteca
 *
 * Conceitos que você vai aprender:
 * - Structs e ponteiros em C
 * - Leitura e escrita de arquivos binários (fwrite/fread)
 * - Funções com ponteiros para arrays
 * - Busca linear e ordenação com qsort
 * - Manipulação de strings com a biblioteca string.h
 *
 * As funções de "regra" (adicionar, buscar, emprestar...) não usam
 * printf/scanf: elas só alteram os dados e devolvem um código.
 * Isso permite testá-las automaticamente (veja tests/test_biblioteca.c).
 */

#include "biblioteca.h"

/* ==========================================
 * PERSISTÊNCIA — Salvar e carregar do disco
 * ==========================================
 *
 * Usamos arquivo binário (.dat) para persistir os dados.
 * Diferente de arquivo texto, o binário salva os bytes exatos da struct.
 * Vantagens: mais rápido, mais compacto, mais simples de ler de volta.
 * Desvantagem: não é legível por humanos.
 */

/**
 * Carrega os livros salvos no arquivo binário.
 * Retorna 1 em sucesso, 0 se o arquivo não existir (primeira execução)
 * e -1 se o arquivo estiver corrompido.
 */
int carregar_livros(const char *caminho, Livro livros[], int *total) {
    *total = 0;
    FILE *arquivo = fopen(caminho, "rb");  /* "rb" = read binary */
    if (arquivo == NULL) {
        return 0;  /* Arquivo não existe ainda — tudo bem */
    }

    /* Lê o total de livros (primeiro inteiro no arquivo) */
    int lido;
    if (fread(&lido, sizeof(int), 1, arquivo) != 1 || lido < 0 || lido > MAX_LIVROS) {
        /* Sem esta checagem, um arquivo corrompido com total = 50000
         * faria o fread abaixo escrever além do fim do array */
        fclose(arquivo);
        return -1;
    }

    /* Lê todos os livros de uma vez: fread(destino, tamanho_de_um, quantidade, arquivo) */
    if (fread(livros, sizeof(Livro), (size_t)lido, arquivo) != (size_t)lido) {
        fclose(arquivo);
        return -1;
    }

    fclose(arquivo);
    *total = lido;
    return 1;
}

/**
 * Salva os livros no arquivo binário.
 * Sobrescreve o arquivo inteiro a cada save.
 */
int salvar_livros(const char *caminho, const Livro livros[], int total) {
    FILE *arquivo = fopen(caminho, "wb");  /* "wb" = write binary */
    if (arquivo == NULL) {
        printf("Erro: não foi possível salvar os dados.\n");
        return 0;
    }

    /* Salva o total primeiro, depois os livros */
    int ok = fwrite(&total, sizeof(int), 1, arquivo) == 1
          && fwrite(livros, sizeof(Livro), (size_t)total, arquivo) == (size_t)total;

    /* fclose também pode falhar (ex: disco cheio ao descarregar o buffer) */
    if (fclose(arquivo) != 0) ok = 0;
    if (!ok) printf("Erro: falha ao gravar %s.\n", caminho);
    return ok;
}

/* ==========================================
 * REGRAS — Create, Read, Update, Delete
 * ========================================== */

/**
 * Adiciona uma cópia de *dados ao acervo, gerando um ID novo.
 * Retorna o ID criado ou ERRO_CHEIO.
 *
 * Parâmetros com ponteiro (*total) permitem que a função
 * modifique a variável original do chamador.
 */
int adicionar_livro(Livro livros[], int *total, const Livro *dados) {
    if (*total >= MAX_LIVROS) {
        return ERRO_CHEIO;
    }

    /* Gera ID automaticamente: maior ID existente + 1 */
    int maior_id = 0;
    for (int i = 0; i < *total; i++) {
        if (livros[i].id > maior_id) maior_id = livros[i].id;
    }

    /* Copia a struct inteira de uma vez (atribuição de struct copia todos os campos) */
    Livro *novo = &livros[*total];
    *novo = *dados;
    novo->id = maior_id + 1;
    novo->disponivel = 1;
    novo->qtd_emprestimos = 0;

    (*total)++;
    return novo->id;
}

/** Busca um livro pelo ID. Retorna ponteiro para o livro ou NULL. */
Livro *buscar_por_id(Livro livros[], int total, int id) {
    for (int i = 0; i < total; i++) {
        if (livros[i].id == id) {
            return &livros[i];  /* Retorna ponteiro (endereço de memória) do livro */
        }
    }
    return NULL;  /* Convenção em C: retorna NULL quando não encontrado */
}

/**
 * Verifica se `termo` aparece em `texto`, sem diferenciar maiúsculas/minúsculas.
 * Compara caractere a caractere, sem copiar as strings (nada de buffers fixos).
 */
int contem_ignorando_caixa(const char *texto, const char *termo) {
    if (*termo == '\0') return 1;  /* termo vazio está contido em qualquer texto */

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

/**
 * Busca livros por título ou autor (busca parcial, case-insensitive).
 * Preenche resultados[] com os índices encontrados e retorna a quantidade.
 */
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

/**
 * Remove um livro pelo ID.
 * Desloca os livros seguintes uma posição para trás, mantendo a ordem de cadastro.
 */
int remover_livro(Livro livros[], int *total, int id) {
    for (int i = 0; i < *total; i++) {
        if (livros[i].id == id) {
            if (!livros[i].disponivel) {
                return ERRO_EMPRESTADO;
            }
            /* memmove funciona mesmo com origem e destino sobrepostos */
            memmove(&livros[i], &livros[i + 1], (size_t)(*total - i - 1) * sizeof(Livro));
            (*total)--;
            return OK;
        }
    }
    return ERRO_NAO_ENCONTRADO;
}

/* ==========================================
 * EMPRÉSTIMOS
 * ========================================== */

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

/* ==========================================
 * TELAS — interação com o usuário
 * ========================================== */

void tela_cadastrar(Livro livros[], int *total) {
    if (*total >= MAX_LIVROS) {
        printf("Biblioteca cheia! Limite de %d livros atingido.\n", MAX_LIVROS);
        return;
    }

    Livro dados = {0};  /* {0} zera todos os campos da struct */
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
    printf("\nLivro cadastrado com sucesso! ID: %d\n", id);
}

/*
 * Mostra o valor atual de um campo e lê o novo; Enter mantém o atual.
 * A leitura já é limitada a `tamanho`, então o texto sempre cabe no destino
 * (strlen(entrada) < tamanho) e pode ser copiado com memcpy.
 */
static void editar_campo(const char *rotulo_atual, const char *rotulo_novo,
                         char *destino, int tamanho) {
    char entrada[MAX_TITULO];  /* MAX_TITULO é o maior dos campos de texto */
    printf("%s: %s\n", rotulo_atual, destino);
    ler_string(rotulo_novo, entrada, tamanho);
    if (entrada[0]) memcpy(destino, entrada, strlen(entrada) + 1);
}

/** Edita os campos de um livro. Enter mantém o valor atual. */
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

/** Lista todos os livros cadastrados */
void listar_livros(const Livro livros[], int total) {
    if (total == 0) {
        printf("\nNenhum livro cadastrado.\n");
        return;
    }

    printf("\n%-4s  %-35s  %-20s  %-4s  %-12s  %s\n",
           "ID", "Título", "Autor", "Ano", "Gênero", "Status");
    printf("%s\n", "--------------------------------------------------------------------------------------");

    for (int i = 0; i < total; i++) {
        const Livro *l = &livros[i];
        printf("%-4d  %-35.35s  %-20.20s  %-4d  %-12.12s  %s\n",
               l->id,
               l->titulo,
               l->autor,
               l->ano_publicacao,
               l->genero,
               l->disponivel ? "Disponível" : "Emprestado");
    }

    printf("\nTotal: %d livro(s)\n", total);
}

/* ==========================================
 * RELATÓRIOS
 * ========================================== */

void relatorio_disponiveis(const Livro livros[], int total) {
    int count = 0;
    printf("\n--- LIVROS DISPONÍVEIS ---\n");
    for (int i = 0; i < total; i++) {
        if (livros[i].disponivel) {
            printf("  [%d] %s — %s (%d)\n",
                   livros[i].id, livros[i].titulo,
                   livros[i].autor, livros[i].ano_publicacao);
            count++;
        }
    }
    printf("\nTotal disponível: %d de %d\n", count, total);
}

/* Função de comparação para o qsort: mais empréstimos primeiro.
 * Recebe ponteiros genéricos (const void *) para os elementos do array —
 * aqui, cada elemento é um ponteiro para Livro. */
static int comparar_emprestimos(const void *a, const void *b) {
    const Livro *la = *(const Livro *const *)a;
    const Livro *lb = *(const Livro *const *)b;
    return lb->qtd_emprestimos - la->qtd_emprestimos;
}

/** Ordena por quantidade de empréstimos e exibe o top 5 */
void relatorio_mais_emprestados(const Livro livros[], int total) {
    if (total == 0) { printf("Nenhum livro cadastrado.\n"); return; }

    /* Array de ponteiros: ordena sem mover as structs (que são grandes) */
    const Livro *ordenados[MAX_LIVROS];
    for (int i = 0; i < total; i++) ordenados[i] = &livros[i];

    /* qsort: ordenação da biblioteca padrão, O(n log n) */
    qsort(ordenados, (size_t)total, sizeof(ordenados[0]), comparar_emprestimos);

    int limite = total < 5 ? total : 5;
    printf("\n--- TOP %d MAIS EMPRESTADOS ---\n", limite);
    for (int i = 0; i < limite; i++) {
        printf("  %d. [%dx] %s — %s\n",
               i + 1, ordenados[i]->qtd_emprestimos,
               ordenados[i]->titulo, ordenados[i]->autor);
    }
}

/** Visão geral do acervo */
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

/* ==========================================
 * UTILITÁRIOS DE ENTRADA
 * ========================================== */

/* Chamado quando a entrada acaba (Ctrl+D / Ctrl+Z ou arquivo redirecionado).
 * Sem isso, os loops de leitura ficariam repetindo para sempre. */
static void fim_da_entrada(void) {
    printf("\nEntrada encerrada. Os dados já estão salvos. Até logo!\n");
    exit(0);
}

/** Limpa o buffer de entrada (resíduo do Enter após scanf) */
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/** Lê um inteiro com validação de intervalo */
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

/** Lê uma string com segurança (evita buffer overflow) */
void ler_string(const char *prompt, char *destino, int tamanho) {
    printf("%s: ", prompt);
    if (fgets(destino, tamanho, stdin) == NULL) fim_da_entrada();

    size_t len = strcspn(destino, "\n");
    if (destino[len] == '\n') {
        destino[len] = '\0';  /* Remove o '\n' que fgets inclui no final */
    } else {
        /* O texto era maior que o buffer: descarta o resto da linha,
         * senão ele seria lido como resposta da próxima pergunta */
        limpar_buffer();
    }
}

void pausar(void) {
    printf("\nPressione Enter para continuar...");
    int c = getchar();
    if (c == EOF) fim_da_entrada();
    /* Se a pessoa digitou algo antes do Enter, descarta para não virar a próxima opção */
    if (c != '\n') limpar_buffer();
}
