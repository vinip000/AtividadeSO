#include <stdio.h>  
#include <stdlib.h>  

#define PAGE_FAULT -1 // Define -1 como valor usado para indicar Page Fault


// 1. Paginacao Hierarquica
#define LEVEL1_SIZE 10 // Define o tamanho da tabela de primeiro nivel
#define LEVEL2_SIZE 10 // Define o tamanho da tabela de segundo nivel

typedef struct { // Define uma estrutura para a tabela de segundo nivel
    int frames[LEVEL2_SIZE]; // Vetor que armazena os frames fisicos
} SecondLevelTable; // Nome da estrutura

typedef struct { // Define uma estrutura para a tabela hierarquica
    SecondLevelTable* level1[LEVEL1_SIZE]; // Vetor de ponteiros para tabelas de segundo nivel
} HierarchicalPageTable; // Nome da estrutura

void initHierarchical(HierarchicalPageTable* table) { // Funcao que inicializa a tabela hierarquica
    for (int i = 0; i < LEVEL1_SIZE; i++) { // Percorre todas as posicoes do primeiro nivel
        table->level1[i] = NULL; // Inicializa cada ponteiro como NULL
    }
}

void insertHierarchical(HierarchicalPageTable* table, int page, int frame) { // Insere uma pagina na tabela hierarquica
    int level1Index = page / LEVEL2_SIZE; // Calcula o indice do primeiro nivel
    int level2Index = page % LEVEL2_SIZE; // Calcula o indice do segundo nivel

    if (level1Index >= LEVEL1_SIZE) { // Verifica se a pagina esta fora do limite
        printf("Pagina invalida na tabela hierarquica.\n"); // Mostra mensagem de erro
        return; // Encerra a funcao
    }

    if (table->level1[level1Index] == NULL) { // Verifica se ainda nao existe tabela de segundo nivel
        table->level1[level1Index] = malloc(sizeof(SecondLevelTable)); // Aloca memoria para a tabela de segundo nivel

        for (int i = 0; i < LEVEL2_SIZE; i++) { // Percorre a tabela de segundo nivel
            table->level1[level1Index]->frames[i] = PAGE_FAULT; // Inicializa cada posicao como Page Fault
        }
    }

    table->level1[level1Index]->frames[level2Index] = frame; // Armazena o frame na posicao correspondente
}

int searchHierarchical(HierarchicalPageTable* table, int page) { // Busca uma pagina na tabela hierarquica
    int level1Index = page / LEVEL2_SIZE; // Calcula o indice do primeiro nivel
    int level2Index = page % LEVEL2_SIZE; // Calcula o indice do segundo nivel

    if (level1Index >= LEVEL1_SIZE) { // Verifica se a pagina esta fora do limite
        return PAGE_FAULT; // Retorna Page Fault
    }

    if (table->level1[level1Index] == NULL) { // Verifica se a tabela de segundo nivel nao existe
        return PAGE_FAULT; // Retorna Page Fault
    }

    return table->level1[level1Index]->frames[level2Index]; // Retorna o frame encontrado
}

// 2. Paginacao por Hash
#define HASH_SIZE 10 // Define o tamanho da tabela hash

typedef struct HashNode { // Define um no da lista encadeada da tabela hash
    int page; // Armazena a pagina logica
    int frame; // Armazena o frame fisico
    struct HashNode* next; // Ponteiro para o proximo no
} HashNode; // Nome da estrutura

typedef struct { // Define a estrutura da tabela hash
    HashNode* buckets[HASH_SIZE]; // Vetor de ponteiros para listas encadeadas
} HashedPageTable; // Nome da estrutura

void initHash(HashedPageTable* table) { // Inicializa a tabela hash
    for (int i = 0; i < HASH_SIZE; i++) { // Percorre todos os buckets
        table->buckets[i] = NULL; // Inicializa cada bucket como NULL
    }
}

int hashFunction(int page) { // Funcao hash
    return page % HASH_SIZE; // Retorna a posicao da pagina na tabela hash
}

void insertHash(HashedPageTable* table, int page, int frame) { // Insere uma pagina na tabela hash
    int index = hashFunction(page); // Calcula o indice usando a funcao hash

    HashNode* current = table->buckets[index]; // Pega o primeiro no da lista naquele indice

    while (current != NULL) { // Percorre a lista enquanto houver nos
        if (current->page == page) { // Verifica se a pagina ja existe
            current->frame = frame; // Atualiza o frame
            return; // Encerra a funcao
        }

        current = current->next; // Vai para o proximo no
    }

    HashNode* newNode = malloc(sizeof(HashNode)); // Aloca memoria para um novo no
    newNode->page = page; // Define a pagina do novo no
    newNode->frame = frame; // Define o frame do novo no
    newNode->next = table->buckets[index]; // Faz o novo no apontar para o antigo primeiro no

    table->buckets[index] = newNode; // Coloca o novo no no inicio da lista
}

int searchHash(HashedPageTable* table, int page) { // Busca uma pagina na tabela hash
    int index = hashFunction(page); // Calcula o indice usando a funcao hash

    HashNode* current = table->buckets[index]; // Pega o primeiro no da lista naquele indice

    while (current != NULL) { // Percorre a lista
        if (current->page == page) { // Verifica se encontrou a pagina
            return current->frame; // Retorna o frame correspondente
        }

        current = current->next; // Vai para o proximo no
    }

    return PAGE_FAULT; // Se nao encontrou, retorna Page Fault
}


// 3. Tabela de Paginas Invertida
#define MAX_FRAMES 10 // Define a quantidade maxima de frames fisicos

typedef struct { // Define uma entrada da tabela invertida
    int page; // Armazena a pagina logica
    int frame; // Armazena o frame fisico
    int occupied; // Indica se a entrada esta ocupada
} InvertedEntry; // Nome da estrutura

typedef struct { // Define a tabela de paginas invertida
    InvertedEntry entries[MAX_FRAMES]; // Vetor de entradas, uma para cada frame
} InvertedPageTable; // Nome da estrutura

void initInverted(InvertedPageTable* table) { // Inicializa a tabela invertida
    for (int i = 0; i < MAX_FRAMES; i++) { // Percorre todos os frames
        table->entries[i].page = -1; // Define a pagina inicial como invalida
        table->entries[i].frame = i; // Define o numero do frame
        table->entries[i].occupied = 0; // Marca a entrada como livre
    }
}

void insertInverted(InvertedPageTable* table, int page, int frame) { // Insere uma pagina na tabela invertida
    if (frame < 0 || frame >= MAX_FRAMES) { // Verifica se o frame e invalido
        printf("Frame invalido na tabela invertida.\n"); // Mostra mensagem de erro
        return; // Encerra a funcao
    }

    table->entries[frame].page = page; // Armazena a pagina na posicao do frame
    table->entries[frame].occupied = 1; // Marca a entrada como ocupada
}

int searchInverted(InvertedPageTable* table, int page) { // Busca uma pagina na tabela invertida
    for (int i = 0; i < MAX_FRAMES; i++) { // Percorre todos os frames
        if (table->entries[i].occupied && table->entries[i].page == page) { // Verifica se a entrada esta ocupada e se a pagina e igual
            return table->entries[i].frame; // Retorna o frame correspondente
        }
    }

    return PAGE_FAULT; // Se nao encontrou, retorna Page Fault
}

void printResult(char* tableName, int page, int result) { // Funcao para imprimir o resultado da busca
    if (result == PAGE_FAULT) { // Verifica se o resultado foi Page Fault
        printf("%s | Pagina %d -> Page Fault\n", tableName, page); // Imprime Page Fault
    } else { // Caso contrario, a pagina foi encontrada
        printf("%s | Pagina %d -> Frame %d\n", tableName, page, result); // Imprime o frame encontrado
    }
}

int main() { 
    HierarchicalPageTable hierarchical; // Cria uma tabela de paginas hierarquica
    HashedPageTable hashed; // Cria uma tabela de paginas por hash
    InvertedPageTable inverted; // Cria uma tabela de paginas invertida

    initHierarchical(&hierarchical); // Inicializa a tabela hierarquica
    initHash(&hashed); // Inicializa a tabela hash
    initInverted(&inverted); // Inicializa a tabela invertida

    insertHierarchical(&hierarchical, 23, 5); // Insere a pagina 23 no frame 5 na tabela hierarquica
    insertHash(&hashed, 23, 5); // Insere a pagina 23 no frame 5 na tabela hash
    insertInverted(&inverted, 23, 5); // Insere a pagina 23 no frame 5 na tabela invertida

    insertHierarchical(&hierarchical, 45, 8); // Insere a pagina 45 no frame 8 na tabela hierarquica
    insertHash(&hashed, 45, 8); // Insere a pagina 45 no frame 8 na tabela hash
    insertInverted(&inverted, 45, 8); // Insere a pagina 45 no frame 8 na tabela invertida

    printResult("Hierarquica", 23, searchHierarchical(&hierarchical, 23)); // Busca a pagina 23 na tabela hierarquica
    printResult("Hash", 23, searchHash(&hashed, 23)); // Busca a pagina 23 na tabela hash
    printResult("Invertida", 23, searchInverted(&inverted, 23)); // Busca a pagina 23 na tabela invertida

    printf("\n"); // Imprime uma linha em branco

    printResult("Hierarquica", 99, searchHierarchical(&hierarchical, 99)); // Busca a pagina 99 na tabela hierarquica
    printResult("Hash", 99, searchHash(&hashed, 99)); // Busca a pagina 99 na tabela hash
    printResult("Invertida", 99, searchInverted(&inverted, 99)); // Busca a pagina 99 na tabela invertida

    return 0; 
}