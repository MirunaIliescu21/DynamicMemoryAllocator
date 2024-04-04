#include "function_malloc.h"

#define DEBUG 1
#define DEBUG_REMOVE 0

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
    if (DEBUG_REMOVE) {
        printf("AL NLEA NOD IN FUNCTIA DE REMOVE = ADICA PE CEL PE CARE TREBUIE SA IL STERG:\n");
        print_block_info(nth_node);print_block_info(nth_node);
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
        if (DEBUG_REMOVE) {
            printf("nu pica aici\n");
            print_block_info(nth_node->next);
        }
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

    printf("Address: %lu\n", block->address);
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
        printf("Adresa blocului: %lu, Dimensiunea blocului: %ld\n", current_block->address, current_block->size);
        current_block = current_block->next;
    }
}

size_t find_insert_position(dllist_t *allocated_list, unsigned long new_address)
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

void dll_insert_nth_node(dllist_t* list, size_t pos, size_t size, unsigned long  address)
{
    /* TODO */
    DIE(!list, "The list is invalid!\n");

    if (pos > list->count) {
        pos = list->count;
    }

    dll_block_t *nod;
    nod = malloc(sizeof(*nod));
    nod->info = malloc(size * sizeof(char));
    nod->address = address;
    nod->size= size;

    if (DEBUG) {
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
    
    // dll_block_t *block = find_block(sfl, num_bytes);

    /* Parcurge listele din structura sfl_t pentru a găsi un bloc liber adecvat */
    int index = -1;
    for (size_t i = num_bytes - 1; i < sfl->bytes_per_list && index == -1; i++) {
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

        if (current_block->size >= num_bytes ) {
            /* Blocul curent este adecvat pentru alocare */
            index = i;
            if (DEBUG) {
                printf("Blocul curent are dim=%ld si face parte din lista de pe pozitia=%d\n", current_block->size, index);
            }
        }
    }

    if (index == -1) {
        /* Daca nu se gaseste niciun nod cu marimea >= cu num_bytes*/
        printf("Out of memory\n");
        return;
    }

    dll_block_t *removed = dll_remove_nth_node(sfl->lists[index], 0);

    if (DEBUG) {
        printf("Informatiile blocului eliminat din lista sunt :\n");
        print_block_info(removed);
    }

    size_t position = find_insert_position(*allocated_list, removed->address);
    
    if (DEBUG) {
        printf("position=%ld\n\n", position);
    }


    if (removed->size == num_bytes) {
        /* Existe o lista in sfl care are blocuri de dimineansiunea num_bytes*/

        if (DEBUG) {
            printf("Blocul nu necesita fragmentare.\n"); 
            printf("blocksize=%ld\n\n", removed->size);
        }
        dll_insert_nth_node(*allocated_list, position, removed->size, removed->address);
        sfl->num_allocated_blocks++;
    } else {

        if (DEBUG) {
            printf("Blocul necesita fragmentare.\n");  
        }
        sfl->num_fragmentations++;
    }

    if (removed->info != NULL) {
        free(removed->info);
    }
    free(removed);



    // /* Daca s-a ajuns aici inseamna ca exista un bloc ce poate fi alocat */

    // if (DEBUG) {
    //     printf("Dimensiunea inainte de adaugare a listei este %ld\n", (*allocated_list)->count);
    // }

    // if (block->size == num_bytes) {
    //     /* Existe o lista in sfl care are blocuri de dimineansiunea num_bytes*/

    //     if (DEBUG) {
    //         printf("Blocul nu necesita fragmentare.\n"); 
    //         printf("blocksize=%ld\n\n", block->size);
    //     }

        
    //     size_t position = find_insert_position(*allocated_list, block->address);
    //     dll_insert_nth_node(*allocated_list, position, block);

    //     printf("IN IF3\n");
    //     for (size_t i = 0; i < sfl->bytes_per_list; i++) {
    //         if (sfl->lists[i]->count > 0) {
    //             /* Lista nu este goala, deci are blocuri ce contin adrese libere */
    //             printf("Blocks with %ld bytes - %ld free block(s) : ", i + 1, sfl->lists[i]->count);
    //             print_list_addresses(sfl->lists[i]);
    //         }
    //     }

    //     sfl->num_allocated_blocks++;
    //     /* Am adaugat nodul in lista de blocuri alocate */
    //     if (DEBUG) {
    //         printf("Pozitia pe care adaug in lista este %ld\n", position);
    //         printf("Noua dimensiune a listei este %ld\n", (*allocated_list)->count);
    //         access_allocated_blocks(*allocated_list);
    //         print_list_addresses(sfl->lists[7]);
    //     }

    //     /* Acum trebuie sa il elimin din lista */
    //     /* Stim ca lista din care trebuie sa eliniman blocul
    //     are blocuri de dimensiunea sizeului blocului selectat */

    //     dll_block_t *removed;
    //     removed = dll_remove_nth_node(sfl->lists[block->size - 1], 0);

    //     if (DEBUG) {
    //         printf("Informatiile blocului eliminat din lista sunt :\n");
    //         print_block_info(removed);
    //     }

    // } else {
    //     if (DEBUG) {
    //         printf("Blocul necesita fragmentare.\n");  
    //     }
    //     sfl->num_fragmentations++;

    // }
    


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

