#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "function.h"
#include "struct.h"


/* Implementarea functiilor */

dllist_t *dll_create(size_t data_size) {

    dllist_t *list = (dllist_t *)malloc(sizeof(dllist_t));
    DIE(!list, "Malloc failed for memory allocatin. \n");
    list->head = NULL;
    list->count = 0;
    list->data_size = data_size;

    return list;
}

sfl_t *init_heap(void *start_address, size_t num_lists, size_t bytes_per_list, int reconstitution_type) {

    sfl_t *sfl = (sfl_t *)malloc(sizeof(sfl_t));
    if(!sfl) {
        return NULL; /* Eroare alocare memorie pentru vectorul de liste*/
    }

    size_t hmax; /* Dim maxima pe care o poate avea vectorului de liste */
    hmax = 1 << (2 + num_lists); /* 2 la puterea num_lists */
    // printf("hmax=%ld\n", hmax);
    sfl->lists = (dllist_t **)malloc(hmax * sizeof(dllist_t *));

    if(!sfl->lists) {
        free(sfl); /* Eliberare memorie pentru structura*/
        return NULL; /* Eroare alocare memorie pentru liste*/
    }
    
    for (size_t i = 0; i < hmax; i++) {
        if ((i + 1) >= 8 && ((i + 1) & i) == 0) {
            sfl->lists[i] = dll_create(i + 1); // eu acum am creat lista si va trebui sa adaung in ea noduri pana la numarul de biti per lista
            // mergand din i in i
        } else {
            sfl->lists[i] = dll_create(i + 1);
        }

    }
    
    /* Initializez campurile structurii sfl_t */
    return sfl;
}

void malloc_memory(sfl_t *sfl, size_t num_bytes) {
}

void free_memory(sfl_t *sfl, void *address) {
}

void read_memory(void *address, size_t num_bytes) {
}

void write_memory(void* address, char *data, size_t num_bytes) {
}

void dump_memory(sfl_t *sfl) {
}

void destroy_heap(sfl_t *sfl) {
}
