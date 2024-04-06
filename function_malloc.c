#include "function_malloc.h"

#define DEBUG 0
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
            printf("Blocul curent are dim=%ld\n", ((info_t *)current_block->data)->size);
        }
        if (((info_t *)current_block->data)->size >= num_bytes ) {
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

    printf("Address: 0x%lx\n", ((info_t *)block->data)->address);
    printf("Size: %ld\n", ((info_t *)block->data)->size);
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
        printf("Adresa blocului: %lu, Dimensiunea blocului: %ld\n", ((info_t *)current_block->data)->address, ((info_t *)current_block->data)->size);
        current_block = current_block->next;
    }
}

size_t find_insert_position(dllist_t *allocated_list, unsigned long new_address)
{
    size_t position = 0;
    dll_block_t *current_block = allocated_list->head;

    // Parcurgem lista până când găsim un bloc cu o adresă mai mare decât cea a noului nod
    for (size_t i = 0; i < allocated_list->count && ((info_t *)current_block->data)->address < new_address; i++) {
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
    DIE(!nod, "Failed to add new node\n");
    nod->data = malloc(sizeof(info_t));
    DIE(!nod->data, "Failed to alloc new node info\n");

    ((info_t *)nod->data)->data = malloc (size * sizeof(char));
    DIE(!((info_t *)nod->data)->data, "Eroare la alocarea memoriei pt camp data\n");

    ((info_t *)nod->data)->address = address;
    ((info_t *)nod->data)->size = size;

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

        if (((info_t *)current_block->data)->size >= num_bytes ) {
            /* Blocul curent este adecvat pentru alocare */
            index = i;
            if (DEBUG) {
                printf("Blocul curent are dim=%ld si face parte din lista de pe pozitia=%d\n",
                ((info_t *)current_block->data)->size, index);
            }
        }
    }

    if (index == -1) {
        /* Daca nu se gaseste niciun nod cu marimea >= cu num_bytes*/
        printf("Out of memory\n");
        return;
    }

    /* Daca s-a ajuns aici inseamna ca exista un bloc ce poate fi alocat */
    sfl->num_allocated_blocks++;
    sfl->num_malloc_calls++;
    sfl->total_allocated_memory += num_bytes;
    sfl->total_free_memory -= num_bytes;

    dll_block_t *removed = dll_remove_nth_node(sfl->lists[index], 0);

    if (DEBUG) {
        printf("Informatiile blocului eliminat din lista sunt :\n");
        print_block_info(removed);
    }

    size_t position = find_insert_position(*allocated_list, ((info_t *)removed->data)->address);
    
    if (DEBUG) {
        printf("position=%ld\n\n", position);
    }


    if (((info_t *)removed->data)->size == num_bytes) {
        /* Existe o lista in sfl care are blocuri de dimineansiunea num_bytes*/
        sfl->num_free_blocks--;
        if (DEBUG) {
            printf("Blocul nu necesita fragmentare.\n"); 
            printf("blocksize=%ld\n\n", ((info_t *)removed->data)->size);
        }

        dll_insert_nth_node(*allocated_list, position, ((info_t *)removed->data)->size,
                            ((info_t *)removed->data)->address);
    } else {

        if (DEBUG) {
            printf("Blocul necesita fragmentare.\n");  
        }
        sfl->num_fragmentations++;
        dll_insert_nth_node(*allocated_list, position, num_bytes, ((info_t *)removed->data)->address);
        /* acum trebuie sa gasesc pozitia unude trebuie sa adaug memoria ramasa*/
        size_t newsz = ((info_t *)removed->data)->size - num_bytes;
        unsigned long newaddr = ((info_t *)removed->data)->address + num_bytes;

        if(DEBUG) {
            printf(" Informatiile blocului pe care trebuie sa il adaug din nou in struct initiala ");
            printf(" newaddress=0x%lx si newsize=%ld\n", newaddr, newsz);
        }

        size_t p = find_insert_position(sfl->lists[newsz - 1], newaddr);
        if (DEBUG) {
            printf("position in list %ld is= %ld\n\n", newsz - 1, p);
        }
        dll_insert_nth_node(sfl->lists[newsz - 1], p, newsz, newaddr);

    }

    if (((info_t *)removed->data)->data != NULL) {
        free(((info_t *)removed->data)->data);
    }

    if (removed->data != NULL) {
        free(removed->data);
    }

    free(removed);
}

int find_addr_position(dllist_t *allocated_list, unsigned long new_address)
{
    int position = -1;
    dll_block_t *current_block = allocated_list->head;

    if (DEBUG) {
        printf("listsize=%ld\n", allocated_list->count);
        printf("address=0x%lx\n", new_address);
    }

    /* Parcurgem lista pana dam de elementul cu aceeasi adresa ca cea cautat */
    for (size_t i = 0; i < allocated_list->count; i++) {

        if (DEBUG) {
            printf("Adresa blocului curent este=0x%lx\n", ((info_t *)current_block->data)->address);
        }

        if (((info_t *)current_block->data)->address == new_address) {
            position = i;
        }

        current_block = current_block->next;
    }

    return position;
}

void free_memory(sfl_t *sfl, unsigned long address, dllist_t **allocated_list)
{
    /* Cautam in lista de blocuri alocate, pozitia blocului
       ce are aceeasi adresa cu cea citita*/
    if (DEBUG) {
        printf("Parametrul address=0x%lx\n", address);
    }

    if (*allocated_list == NULL) {
        if (DEBUG) {
            printf("Nu s-a alocat memorie.\n");
        }
        return;
    }

    int pos = find_addr_position(*allocated_list, address);

    if (DEBUG) {
        printf("pos=%d\n", pos);
    }

    if (pos == -1) {
        /* Nu exista nicun bloc cu acresa cautata */
        printf("Invalid free\n");
        return;
    }

    /* Daca am ajuns aici inseamna ca avem un bloc pe care putem sa il eliberam
    si sa il adaugam in structura initiala */
    sfl->num_free_calls++;

    if (DEBUG) {
        printf("Blocul pe care trebuie sa il eliberam se afla pe pozitia=%d\n", pos);
    }

    dll_block_t *removed = dll_remove_nth_node(*allocated_list, pos);
    sfl->num_allocated_blocks--; /*s-a mai eliberat un bloc*/
    sfl->total_allocated_memory -= ((info_t *)removed->data)->size;
    /* s-a eliberat dim blocului scos */

    if (DEBUG) {
        printf("Informatiile blocului eliminat din lista allocated sunt :\n");
        print_block_info(removed);
    }

    /* vrem sa stim unde adaugam nodul in heap */
    /* il adaugam in lista aferenta dimensiunii sale, pe pozitia p*/
    size_t p = find_insert_position(sfl->lists[((info_t *)removed->data)->size - 1],
                                    ((info_t *)removed->data)->address);
    if (DEBUG) {
        printf("position in list %ld is= %ld\n\n", ((info_t *)removed->data)->size - 1, p);
    }

    // size_t sz = ((info_t *)removed->data)->size;
    // unsigned long addr = ((info_t *)removed->data)->address;

    dll_insert_nth_node(sfl->lists[((info_t *)removed->data)->size - 1], p, 
                        ((info_t *)removed->data)->size,
                        ((info_t *)removed->data)->address);

    sfl->num_free_blocks++; /* s-a mai adaugat un bloc in sfl*/
    // sfl->total_free_memory += ((info_t *)removed->data)->size; /* s-a adaugat memorie */
    sfl->total_free_memory += ((info_t *)removed->data)->size; /* s-a adaugat memorie */

    if (((info_t *)removed->data)->data != NULL) {
        free(((info_t *)removed->data)->data);
    }
    
    if (removed->data != NULL) {
        free(removed->data);
    }

    free(removed);

}



