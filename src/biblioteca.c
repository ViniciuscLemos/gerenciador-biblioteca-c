/*
 * biblioteca.c — Implementação das funções do sistema de biblioteca
 *
 * Conceitos que você vai aprender:
 * - Structs e ponteiros em C
 * - Leitura e escrita de arquivos binários (fwrite/fread)
 * - Funções com ponteiros para arrays
 * - Busca linear e ordenação simples
 * - Manipulação de strings com a biblioteca string.h
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
 * Retorna 1 em sucesso, 0 se o arquivo não existir (primeira execução).
 */
int carregar_livros(Livro livros[], int *total) {
    FILE *arquivo = fopen(ARQUIVO_DB, "rb");  /* "rb" = read binary */
    if (arquivo == NULL) {
        *total = 0;
        return 0;  /* Arquivo não existe ainda — tudo bem */
    }

    /* Lê o total de livros (primeiro inteiro no arquivo) */
    fread(total, sizeof(int), 1, arquivo);

    /* Lê todos os livros de uma vez: fread(destino, tamanho_de_um, quantidade, arquivo) */
    fread(livros, sizeof(Livro), *total, arquivo);

    fclose(arquivo);
    return 1;
}

/**
 * Salva os livros no arquivo binário.
 * Sobrescreve o arquivo inteiro a cada save.
 */
int salvar_livros(Livro livros[], int total) {
    FILE *arquivo = fopen(ARQUIVO_DB, "wb");  /* "wb" = write binary */
    if (arquivo == NULL) {
        printf("Erro: não foi possível salvar os dados.\n");
        return 0;
    }

    /* Salva o total primeiro, depois os livros */
    fwrite(&total, sizeof(int), 1, arquivo);
    fwrite(livros, sizeof(Livro), total, arquivo);

    fclose(arquivo);
    return 1;
}

/* ==========================================
 * CRUD — Create, Read, Update, Delete
 * ========================================== */

/**
 * Cadastra um novo livro.
 * Retorna o ID do livro criado ou -1 em caso de erro.
 *
 * Parâmetros com ponteiro (*total) permitem que a função
 * modifique a variável original do chamador.
 */
int cadastrar_livro(Livro livros[], int *total) {
    if (*total >= MAX_LIVROS) {
        printf("Biblioteca cheia! Limite de %d livros atingido.\n", MAX_LIVROS);
        return -1;
    }

    /* Trabalha diretamente na posição do array (sem copiar a struct) */
    Livro *novo = &livros[*total];

    /* Gera ID automaticamente: maior ID existente + 1 */
    int maior_id = 0;
    for (int i = 0; i < *total; i++) {
        if (livros[i].id > maior_id) maior_id = livros[i].id;
    }
    novo->id = maior_id + 1;

    printf("\n--- CADASTRAR LIVRO ---\n");
    ler_string("Título", novo->titulo, MAX_TITULO);
    ler_string("Autor", novo->autor, MAX_AUTOR);
    ler_string("ISBN", novo->isbn, MAX_ISBN);
    ler_string("Gênero", novo->genero, MAX_GENERO);
    novo->ano_publicacao = ler_inteiro("Ano de publicação", 1000, 2100);

    novo->disponivel = 1;
    novo->qtd_emprestimos = 0;

    (*total)++;
    salvar_livros(livros, *total);

    printf("\nLivro cadastrado com sucesso! ID: %d\n", novo->id);
    return novo->id;
}

/** Lista todos os livros cadastrados */
void listar_livros(Livro livros[], int total) {
    if (total == 0) {
        printf("\nNenhum livro cadastrado.\n");
        return;
    }

    printf("\n%-4s  %-35s  %-20s  %-4s  %-12s  %s\n",
           "ID", "Título", "Autor", "Ano", "Gênero", "Status");
    printf("%s\n", "----------------------------------------------------------------------");

    for (int i = 0; i < total; i++) {
        Livro *l = &livros[i];
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
 * Busca livros por título (busca parcial, case-insensitive).
 * Preenche o array resultados[] com os índices encontrados.
 */
int buscar_por_titulo(Livro livros[], int total, const char *termo,
                      int resultados[], int *qtd) {
    *qtd = 0;
    char termo_lower[MAX_TITULO];
    char titulo_lower[MAX_TITULO];

    /* Converte o termo para minúsculas para busca case-insensitive */
    for (int i = 0; termo[i]; i++) {
        termo_lower[i] = (char)tolower((unsigned char)termo[i]);
    }
    termo_lower[strlen(termo)] = '\0';

    for (int i = 0; i < total; i++) {
        for (int j = 0; livros[i].titulo[j]; j++) {
            titulo_lower[j] = (char)tolower((unsigned char)livros[i].titulo[j]);
        }
        titulo_lower[strlen(livros[i].titulo)] = '\0';

        /* strstr verifica se termo_lower está contido em titulo_lower */
        if (strstr(titulo_lower, termo_lower) != NULL) {
            resultados[(*qtd)++] = i;
        }
    }

    return *qtd;
}

/**
 * Remove um livro pelo ID.
 * Técnica: copia o último livro para o lugar do removido (O(1) em vez de O(n)).
 */
int remover_livro(Livro livros[], int *total, int id) {
    for (int i = 0; i < *total; i++) {
        if (livros[i].id == id) {
            if (!livros[i].disponivel) {
                printf("Não é possível remover um livro emprestado.\n");
                return 0;
            }
            /* Substitui pelo último e decrementa o total */
            livros[i] = livros[*total - 1];
            (*total)--;
            salvar_livros(livros, *total);
            printf("Livro removido com sucesso.\n");
            return 1;
        }
    }
    printf("Livro com ID %d não encontrado.\n", id);
    return 0;
}

/* ==========================================
 * EMPRÉSTIMOS
 * ========================================== */

int emprestar_livro(Livro livros[], int total, int id) {
    Livro *livro = buscar_por_id(livros, total, id);
    if (livro == NULL) {
        printf("Livro não encontrado.\n");
        return 0;
    }
    if (!livro->disponivel) {
        printf("Livro já está emprestado.\n");
        return 0;
    }
    livro->disponivel = 0;
    livro->qtd_emprestimos++;
    salvar_livros(livros, total);
    printf("Livro \"%s\" emprestado com sucesso!\n", livro->titulo);
    return 1;
}

int devolver_livro(Livro livros[], int total, int id) {
    Livro *livro = buscar_por_id(livros, total, id);
    if (livro == NULL) {
        printf("Livro não encontrado.\n");
        return 0;
    }
    if (livro->disponivel) {
        printf("Este livro não está emprestado.\n");
        return 0;
    }
    livro->disponivel = 1;
    salvar_livros(livros, total);
    printf("Livro \"%s\" devolvido com sucesso!\n", livro->titulo);
    return 1;
}

/* ==========================================
 * RELATÓRIOS
 * ========================================== */

void relatorio_disponiveis(Livro livros[], int total) {
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

/** Ordena por quantidade de empréstimos (bubble sort) e exibe top 5 */
void relatorio_mais_emprestados(Livro livros[], int total) {
    if (total == 0) { printf("Nenhum livro cadastrado.\n"); return; }

    /* Cria array de ponteiros para ordenar sem mover as structs */
    Livro *ordenados[MAX_LIVROS];
    for (int i = 0; i < total; i++) ordenados[i] = &livros[i];

    /* Bubble Sort — simples de entender, mas O(n²) */
    for (int i = 0; i < total - 1; i++) {
        for (int j = 0; j < total - i - 1; j++) {
            if (ordenados[j]->qtd_emprestimos < ordenados[j+1]->qtd_emprestimos) {
                Livro *temp = ordenados[j];
                ordenados[j] = ordenados[j+1];
                ordenados[j+1] = temp;
            }
        }
    }

    int limite = total < 5 ? total : 5;
    printf("\n--- TOP %d MAIS EMPRESTADOS ---\n", limite);
    for (int i = 0; i < limite; i++) {
        printf("  %d. [%dx] %s — %s\n",
               i+1, ordenados[i]->qtd_emprestimos,
               ordenados[i]->titulo, ordenados[i]->autor);
    }
}

/* ==========================================
 * UTILITÁRIOS
 * ========================================== */

/** Limpa o buffer de entrada (resíduo do Enter após scanf) */
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/** Lê um inteiro com validação de intervalo */
int ler_inteiro(const char *prompt, int min, int max) {
    int valor;
    do {
        printf("%s (%d-%d): ", prompt, min, max);
        while (scanf("%d", &valor) != 1) {
            printf("Entrada inválida. Tente novamente: ");
            limpar_buffer();
        }
        limpar_buffer();
    } while (valor < min || valor > max);
    return valor;
}

/** Lê uma string com segurança (evita buffer overflow) */
void ler_string(const char *prompt, char *destino, int tamanho) {
    printf("%s: ", prompt);
    fgets(destino, tamanho, stdin);
    /* Remove o '\n' que fgets inclui no final */
    destino[strcspn(destino, "\n")] = '\0';
}

void pausar(void) {
    printf("\nPressione Enter para continuar...");
    limpar_buffer();
}

void limpar_tela(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
