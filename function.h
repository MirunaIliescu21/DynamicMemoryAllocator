#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdlib.h>
#include "struct.h"


/* Headerele functiilor */
dllist_t *dll_create(size_t data_size);
void dll_add_nth_node(dllist_t* list, size_t n);
dll_block_t *dll_get_nth_node(dllist_t* list, size_t n);
void print_list_addresses(dllist_t *list);
sfl_t *init_heap(void *start_address, size_t num_lists,
                 size_t bytes_per_list, int type);
void malloc_memory(sfl_t *sfl, size_t num_bytes);
void free_memory(sfl_t *sfl, void* address);
void read_memory(void* address, size_t num_bytes);
void write_memory(void* address, char* data, size_t num_bytes);
void dump_memory(sfl_t *sfl);
void destroy_heap(sfl_t *sfl);

#endif /* FUNCTION_H */
