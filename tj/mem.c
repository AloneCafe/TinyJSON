#include <stdlib.h>
#include "mem.h"

void **mem_table = NULL;
int mem_table_size = 0;

void * new(size_t size) {
    return mem_table[mem_table_size++] = malloc(size);
}

void initMem() {
    mem_table = malloc(MAX_POINTER_COUNT);
}

void destroyMem() {
    int i;
    for (i = 0; i < mem_table_size; i++) {
        free(mem_table[i]);
    }
    free(mem_table);
    mem_table = NULL;
}