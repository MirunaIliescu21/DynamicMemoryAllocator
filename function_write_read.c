#include "function_write_read.h"

#define DEBUG 1

/* AM NEVOIE DE O FUNCTIE CARE IMI RETURNEAZA BLOCUL CU CEA MAI MARE ADRESA
 MAI MICA DECAT ADRESA CITITA CA PARAMETRU IN FUNCTIA WRITE */
dll_block_t *dll_address(dllist_t *list, unsigned long addr) {
    if (list == NULL || list->head == NULL) {
        if (DEBUG) {
            printf("Lista neinitializata sau goala\n");
        }
        return NULL;
    }

    if (addr < list->head->address) {
        /* blocurile din lista sunt ord cresc, deci nu exista
         o adresa <= cu adresa ceruta*/
        if (DEBUG) {
            printf("Adresa data < prima daresa din lista -> bloc null\n");
        }
        return NULL;
    }

    dll_block_t *nod = list->head;
    while (nod->next != list->head && addr >= nod->next->address) {
        nod = nod->next;
    }
    
    return nod;
}

/*functia returneaza adresa urmatorului bloc daca aceastea ar fi consecutive*/
unsigned long end_address(dll_block_t *nod) {
    unsigned long end = nod->address + nod->size;
    // if (DEBUG) {
    //     printf("end_address=0x%lx\n", end);
    // }
    return end;
}

 /* Functie returneaza 1 pt toate cazurile de seg fault */
int check_memory(dllist_t *alloc_list, unsigned long addr, size_t num_bytes) {

    if (alloc_list->head == NULL || alloc_list == NULL) {
        if(DEBUG) {
            printf("Nu exista elemente alocate in lista/lista goala\n");
        }
        return 1; /* caz de seg_fault */
    }

    dll_block_t *nod = dll_address(alloc_list, addr);

    /* Verific daca nodul este null sau are o adresa < adresa data */
    if (nod == NULL || end_address(nod) <= addr) {
        if (DEBUG) {
            printf("Nod gol / dimensiune prea mica\n");
        }
        return 1; /* caz de seg fault */
    }
    
    /* Daca s-a ajuns aici verific daca exista suficiente spatii de memorie
     continue pt a se scrie nr_bytes*/
    if (DEBUG) {
        print_block_info(nod);
    }

    long size_in_block = end_address(nod) - addr; /*cati bytes putem utiliza 
    din blocul in care se afla adresa*/
    long diff = num_bytes - size_in_block; /* de cati bytes mai avem nevoie 
    pentru a cuprinde intrega informatie */

    if (DEBUG) {
        printf("cati bytes putem utiliza din blocul in care se afla adresa=%ld\n", size_in_block);
        printf("cati bytes mai avem nevoie din blocurile urmatoare pt a scrie datele=%ld\n", diff);

    }

    /* Pargur nodurile pana cand diff este egal cu 0 sau adresele nu mai sunt consecutive */
    while (diff > 0 && nod->next != alloc_list->head) {
        if (end_address(nod) != nod->next->address) {
            if (DEBUG) {
                printf("end_address=0x%lx nod->next->adr=0x%lx\n",
                        end_address(nod), nod->next->address);
            }
            return 1; /* caz seg fault */
        }

        nod = nod->next;
        diff = diff - nod->size;
    }
    
    return 0;

}

/* Fac o functie de tip int care sa imi returneze 0 in cazul in care se 
 poate scrie in memorie si 1 atunci cand nu se poate scrie in memorie
 si trebuie apelate functiile de dump si de destroy */
int write_memory(dllist_t *allocated_list,
                  unsigned long address, char* data, size_t num_bytes) {

    /* Verific daca se poate scrie in memorie */
    if (allocated_list == NULL) {
        return 1;
    }

    if ((size_t)strlen(data) < num_bytes) {
        num_bytes = strlen(data);
        if (DEBUG) {
            printf("num_bytes=%ld\n", num_bytes);
        }
    }

    if (check_memory(allocated_list, address, num_bytes) == 1) {
        return 1;
    }

    dll_block_t *nod = dll_address(allocated_list, address);
    long size_in_block = end_address(nod) - address; /*cati bytes putem utiliza 
    din blocul in care se afla adresa*/
    long diff = num_bytes - size_in_block; /* de cati bytes mai avem nevoie 
    pentru a cuprinde intrega informatie */
    

    return 0;
}