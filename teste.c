#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define PAGE_FAULT -1

// ===============================
// 1. Paginação Hierárquica
// ===============================

#define LEVEL1_SIZE 10
#define LEVEL2_SIZE 10

typedef struct {
    int frames[LEVEL2_SIZE];
} SecondLevelTable;

typedef struct {
    SecondLevelTable* level1[LEVEL1_SIZE];
} HierarchicalPageTable;

void initHierarchical(HierarchicalPageTable* table) {
    for (int i = 0; i < LEVEL1_SIZE; i++) {
        table->level1[i] = NULL;
    }
}

void insertHierarchical(HierarchicalPageTable* table, int page, int frame) {
    int level1Index = page / LEVEL2_SIZE;
    int level2Index = page % LEVEL2_SIZE;

    if (level1Index >= LEVEL1_SIZE) {
        printf("Página invalida na tabela hierarquica.\n");
        return;
    }

    if (table->level1[level1Index] == NULL) {
        table->level1[level1Index] = malloc(sizeof(SecondLevelTable));

        for (int i = 0; i < LEVEL2_SIZE; i++) {
            table->level1[level1Index]->frames[i] = PAGE_FAULT;
        }
    }

    table->level1[level1Index]->frames[level2Index] = frame;
}

int searchHierarchical(HierarchicalPageTable* table, int page) {
    int level1Index = page / LEVEL2_SIZE;
    int level2Index = page % LEVEL2_SIZE;

    if (level1Index >= LEVEL1_SIZE) {
        return PAGE_FAULT;
    }

    if (table->level1[level1Index] == NULL) {
        return PAGE_FAULT;
    }

    return table->level1[level1Index]->frames[level2Index];
}

// ===============================
// 2. Paginação por Hash
// ===============================

#define HASH_SIZE 10

typedef struct HashNode {
    int page;
    int frame;
    struct HashNode* next;
} HashNode;

typedef struct {
    HashNode* buckets[HASH_SIZE];
} HashedPageTable;

void initHash(HashedPageTable* table) {
    for (int i = 0; i < HASH_SIZE; i++) {
        table->buckets[i] = NULL;
    }
}

int hashFunction(int page) {
    return page % HASH_SIZE;
}

void insertHash(HashedPageTable* table, int page, int frame) {
    int index = hashFunction(page);
    HashNode* current = table->buckets[index];

    while (current != NULL) {
        if (current->page == page) {
            current->frame = frame;
            return;
        }
        current = current->next;
    }

    HashNode* newNode = malloc(sizeof(HashNode));
    newNode->page = page;
    newNode->frame = frame;
    newNode->next = table->buckets[index];

    table->buckets[index] = newNode;
}

int searchHash(HashedPageTable* table, int page) {
    int index = hashFunction(page);
    HashNode* current = table->buckets[index];

    while (current != NULL) {
        if (current->page == page) {
            return current->frame;
        }
        current = current->next;
    }

    return PAGE_FAULT;
}

// ===============================
// 3. Tabela de Páginas Invertida
// ===============================

#define MAX_FRAMES 10

typedef struct {
    int page;
    int frame;
    int occupied;
} InvertedEntry;

typedef struct {
    InvertedEntry entries[MAX_FRAMES];
} InvertedPageTable;

void initInverted(InvertedPageTable* table) {
    for (int i = 0; i < MAX_FRAMES; i++) {
        table->entries[i].page = -1;
        table->entries[i].frame = i;
        table->entries[i].occupied = 0;
    }
}

void insertInverted(InvertedPageTable* table, int page, int frame) {
    if (frame < 0 || frame >= MAX_FRAMES) {
        printf("Frame invalido na tabela invertida.\n");
        return;
    }

    table->entries[frame].page = page;
    table->entries[frame].occupied = 1;
}

int searchInverted(InvertedPageTable* table, int page) {
    for (int i = 0; i < MAX_FRAMES; i++) {
        if (table->entries[i].occupied && table->entries[i].page == page) {
            return table->entries[i].frame;
        }
    }

    return PAGE_FAULT;
}

// ===============================
// Função auxiliar
// ===============================

void printResult(const char* tableName, int page, int result) {
    if (result == PAGE_FAULT) {
        printf("%s | Pagina %d -> Page Fault\n", tableName, page);
    } else {
        printf("%s | Pagina %d -> Frame %d\n", tableName, page, result);
    }
}

// ===============================
// Main
// ===============================

int main() {
    setlocale(LC_ALL, "");

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    HierarchicalPageTable hierarchical;
    HashedPageTable hashed;
    InvertedPageTable inverted;

    initHierarchical(&hierarchical);
    initHash(&hashed);
    initInverted(&inverted);

    insertHierarchical(&hierarchical, 23, 5);
    insertHash(&hashed, 23, 5);
    insertInverted(&inverted, 23, 5);

    insertHierarchical(&hierarchical, 45, 8);
    insertHash(&hashed, 45, 8);
    insertInverted(&inverted, 45, 8);

    printResult("Hierarquica", 23, searchHierarchical(&hierarchical, 23));
    printResult("Hash", 23, searchHash(&hashed, 23));
    printResult("Invertida", 23, searchInverted(&inverted, 23));

    printf("\n");

    printResult("Hierarquica", 99, searchHierarchical(&hierarchical, 99));
    printResult("Hash", 99, searchHash(&hashed, 99));
    printResult("Invertida", 99, searchInverted(&inverted, 99));

    return 0;
}