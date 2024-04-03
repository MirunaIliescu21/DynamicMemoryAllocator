#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "function_malloc.h"
#include "struct.h"
#define DEBUG 1


// // BUN
// dll_block_t* dll_get_nth_node_address(dllist_t* list, size_t *n, void *address) {
//     if (!list || list->count == 0) {
//         return NULL;
//     }
//     unsigned long long addr_as_ullong = (unsigned long long)address;

//     dll_block_t *node = list->head;
//     size_t index = 0;
    
//     if (node->address > addr_as_ullong) {
//         *n = index;
//         return node; /* va trebui sa se adauge inaintea nodului returnat */
//     }

//     dll_block_t *current = node->next;
//     while (current != list->head && current->address < address) {
//         index++;
//         current = current->next;
//     }

//     *n = index;
//     return current->prev; /* ca trebui sa se aduge dupa nodul returnat */
// }


// void dll_add_node_addr(dllist_t *list, void *address, size_t size)
// {
//     DIE(!list, "The list is invalid!\n");

//     /* Alocare si initializare nod */
//     dll_block_t *node = malloc(sizeof(*node));
//     DIE(!node, "Failed to add new node\n");

//     node->info = malloc(list->data_size);
//     DIE(!(node->info), "Malloc failed\n");

//     node->address = (unsigned long long)address;
//     node->size = size;

//     if (list->count == 0) {
//         /* cazul in care lista e goala */
//         /* noul nod devine si urmatorul si anteriorul */
//         list->head = node;
//         node->next = node;
//         node->prev = node;
        
//         list->count++;
//         return;
//     }

//     /* Trebuie adaugat nod dupa prev*/
//     size_t n;
//     dll_block_t *prev_node = dll_get_nth_node_address(list, &n, address);

//     node->prev = prev_node;
//     node->next = prev_node->next;
//     prev_node->next->prev = node;
//     prev_node->next = node;

//     if (n == 0) {
//         list->head = node;
//     }

//     // Incrementarea numărului de blocuri din listă
//     list->count++;
// }


dll_block_t* dll_remove_nth_node(dllist_t* list, size_t n)
{
    /* TODO */
    DIE(!list, "The list isnt initializated\n");
    DIE(!(list->head), "The list is empty\n");

    if (n >= list->count) {
        // inseamna ca vreau sa il sterg pe ultimul
        n = list->count - 1;
    }
    
    // gasim nodul de pe pozitia n
    dll_block_t *nth_node = dll_get_nth_node(list, n);
    if (DEBUG) {
        printf("AL NLEA NOD IN FUNCTIA DE REMOVE = ADICA PE CEL PE CARE TREBUIE SA IL STERG:\n");
        print_block_info(nth_node);
        printf("lungimea listei din care sterg=%ld\n", list->count);
        print_list_addresses(list);
        printf("GATA DEBUG PE REMOVE\n");
    }
    
    if (list->count == 1) {

        list->head = NULL;
        list->count--;
        return nth_node;

    } else {

        nth_node->prev->next = nth_node->next;
        nth_node->next->prev = nth_node->prev;

        if (n == 0) {
            list->head = list->head->next;
        }

        list->count--;
        return nth_node;       
    }
}

/////////////////////////////

dll_block_t *find_block(sfl_t *sfl, size_t num_bytes)
{
    if (!sfl) {
        return NULL;
    }

    /* Parcurge listele din structura sfl_t pentru a găsi un bloc liber adecvat */
    for (size_t i = num_bytes - 1; i < sfl->bytes_per_list; i++) {
        dllist_t *current_list = sfl->lists[i];
        if (current_list == NULL) {
            /* Daca lista este NULL se trece mai departe*/
            if(DEBUG) {
                printf("Lista este null\n");
            }
            continue;
        }

        if (current_list->count == 0) {
            if(DEBUG) {
                printf("Lista are 0 elemenete\n");
            }
            continue;
        }

        if (DEBUG) {
            printf("Lista %ld are %ld elemente\n", i, current_list->count);
        }

        /* Verificam blocurile din lista curenta */
        dll_block_t *current_block = current_list->head;
        if (DEBUG) {
            printf("Blocul curent are dim=%ld\n", current_block->size);
        }
        if (current_block->size >= num_bytes ) {
            /* Blocul curent este adecvat pentru alocare */
            return current_block;
        } else {
            current_block = current_block->next;
        }
    }

    /* Nu s-a gasit niciun bloc pentru alocare */
    return NULL;
}

void print_block_info(dll_block_t *block)
{
    if (block == NULL) {
        printf("Block is NULL\n");
        return;
    }

    printf("Address: %p\n", block->address);
    printf("Size: %ld\n", block->size);
    // printf("Next: %p\n", block->next);
    // printf("Prev: %p\n", block->prev);
}

void access_allocated_blocks(dllist_t *allocated_list)
{
    if (allocated_list == NULL || allocated_list->head == NULL) {
        printf("Lista alocată este goală.\n");
        return;
    }

    dll_block_t *current_block = allocated_list->head;

    for (size_t i = 0; i < allocated_list->count; i++) {
        printf("Adresa blocului: %p, Dimensiunea blocului: %ld\n", current_block->address, current_block->size);
        current_block = current_block->next;
    }
}

size_t find_insert_position(dllist_t *allocated_list, void *new_address)
{
    size_t position = 0;
    dll_block_t *current_block = allocated_list->head;

    // Parcurgem lista până când găsim un bloc cu o adresă mai mare decât cea a noului nod
    for (size_t i = 0; i < allocated_list->count && current_block->address < new_address; i++) {
        current_block = current_block->next;
        position++;
    }

    return position;
}

/* NU ESTE CIRCULARA LISTA */
// void add_block_at_position(dllist_t *list, dll_block_t *block, size_t pos)
// {
//     DIE(!list, "The list is invalid!\n");

//     if (pos > list->count) {
//         pos = list->count;
//     }


//     if (pos == 0) {
//         /* Inseram nodul pe prima poz */
//         block->next = list->head;
//         block->prev = NULL;
//         if (list->head != NULL) {
//             list->head->prev = block;
//         }
//         list->head = block;
//     } else {
//         /* Parcurgem lista pana la poz specificata*/
//         dll_block_t *current_block = list->head;
//         size_t current_position = 0;
//         while (current_position < pos - 1 && current_block != NULL) {
//             current_block = current_block->next;
//             current_position++;
//         }

//         if (current_block == NULL) {
//             /* Adugam noudul la finalul listei */
//             dll_block_t *last_block = list->head;
//             while (last_block->next != NULL) {
//                 last_block = last_block->next;
//             }
//             last_block->next = block;
//             block->prev = last_block;
//             block->next = NULL;
//         } else {
//             /* Adugam nodul intre alte doua noduri */
//             block->next = current_block->next;
//             if (current_block->next != NULL) {
//                 current_block->next->prev = block;
//             }
//             current_block->next = block;
//             block->prev = current_block;
//         }
//     }

//     // Incrementăm numărul total de blocuri din listă
//     list->count++;
// }

void dll_insert_nth_node(dllist_t* list, size_t pos, dll_block_t *block)
{
    /* TODO */
    DIE(!list, "The list is invalid!\n");

    if (pos > list->count) {
        pos = list->count;
    }

    dll_block_t *nod;
    nod = malloc(sizeof(*nod));
    nod->info = NULL;

    nod = block;
    if (DEBUG) {
        printf("COPIA LUI BLOCK DAT CA PAR IN FCTIA INSERT\n");
        print_block_info(nod);
        printf("list->size=%ld\n", list->count);
    }

    // cazul in care lista nu are inca niciun element
    if (list->count == 0) {
        // noul nod devine si urmatorul si anteriorul
        list->head = nod;
        list->head->next = nod;
        list->head->prev = nod;
    
        list->count++;
        return;
    } else {
        // aflu pozitia nodului de pe pozitia n
        // locul unde vrea sa inserez un nou nod
        dll_block_t *nth_node = dll_get_nth_node(list, pos);
        if (DEBUG) {
            printf("AL NLEA NOD: ");
            print_block_info(nth_node);
        }
        nod->prev = nth_node->prev;
        nod->next = nth_node;
        nth_node->prev->next = nod;
        nth_node->prev = nod;

        if (pos == 0) {
            list->head = nod;
        }

        list->count++;
        return;
    }

}

void malloc_memory(sfl_t *sfl, size_t num_bytes, dllist_t **allocated_list)
{
    if(*allocated_list == NULL) {
        *allocated_list = dll_create(0);
    }

    if (!sfl) {
        return;
    }
    sfl->num_malloc_calls++;
    sfl->total_allocated_memory += num_bytes;
    sfl->total_free_memory -= num_bytes;

    if (DEBUG) {
        printf("Numarul de liste este %ld\n", sfl->bytes_per_list);
        printf("Numarul de MALLOC_CALLS este %ld\n", sfl->num_malloc_calls);
        printf("Adresa la care pointeaza sfl=%p\n", sfl);
        printf("Adresa la care pointeaza allocated=%p\n", allocated_list);
    }
    
    dll_block_t *block = find_block(sfl, num_bytes);

    if (DEBUG) {
        print_block_info(block);
    }

    if (block == NULL) {
        printf("Out of memory\n");
        return;
    }

    /* Daca s-a ajuns aici inseamna ca exista un bloc ce poate fi alocat */

    if (DEBUG) {
        printf("Dimensiunea inainte de adaugare a listei este %ld\n", (*allocated_list)->count);
    }

    if (block->size == num_bytes) {
        /* Existe o lista in sfl care are blocuri de dimineansiunea num_bytes*/

        if (DEBUG) {
            printf("Blocul nu necesita fragmentare.\n"); 
            printf("blocksize=%ld\n\n", block->size);
        }

        
        size_t position = find_insert_position(*allocated_list, block->address);
        dll_insert_nth_node(*allocated_list, position, block);

        printf("IN IF3\n");
        for (size_t i = 0; i < sfl->bytes_per_list; i++) {
            if (sfl->lists[i]->count > 0) {
                /* Lista nu este goala, deci are blocuri ce contin adrese libere */
                printf("Blocks with %ld bytes - %ld free block(s) : ", i + 1, sfl->lists[i]->count);
                print_list_addresses(sfl->lists[i]);
            }
        }

        sfl->num_allocated_blocks++;
        /* Am adaugat nodul in lista de blocuri alocate */
        if (DEBUG) {
            printf("Pozitia pe care adaug in lista este %ld\n", position);
            printf("Noua dimensiune a listei este %ld\n", (*allocated_list)->count);
            access_allocated_blocks(*allocated_list);
            print_list_addresses(sfl->lists[7]);
        }

        /* Acum trebuie sa il elimin din lista */
        /* Stim ca lista din care trebuie sa eliniman blocul
        are blocuri de dimensiunea sizeului blocului selectat */

        dll_block_t *removed;
        removed = dll_remove_nth_node(sfl->lists[block->size - 1], 0);

        if (DEBUG) {
            printf("Informatiile blocului eliminat din lista sunt :\n");
            print_block_info(removed);
        }

    } else {
        if (DEBUG) {
            printf("Blocul necesita fragmentare.\n");  
        }
        sfl->num_fragmentations++;

    }
    


    // for (size_t i = 0; i < (*sfl)->bytes_per_list; i++) {

    //     if ((*sfl)->lists[i]->data_size == num_bytes) {

    //         dllist_t *curr_list = (*sfl)->lists[i];
    //         if (curr_list->count > 0) {

    //             // scot nodul pe care il vreau
    //             dll_block_t *removed = dll_remove_nth_node(curr_list, 0);
    //             printf("removed_address=%p SIZE_NOD=%ld\n", removed->address, removed->size);
    //             dll_add_node_addr(allocated_list, removed->address, num_bytes);
    //             if (DEBUG) {
    //                 printf("Numarul de noduri din lista alocata = %ld. \n", (*allocated_list)->count);
    //                 print_list_addresses(allocated_list);
    //             }
                
    //             (*sfl)->num_allocated_blocks++;
    //         }
    //     }
    // }
}

