#ifndef FUNCTION_MALLOC_H
#define FUNCTION_MALLOC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "struct.h"
#include "function.h"


/* Headerele functiilor */

dll_block_t *find_block(sfl_t *sfl, size_t num_bytes);
void print_block_info(dll_block_t *block);
void access_allocated_blocks(dllist_t *allocated_list);
size_t find_insert_position(dllist_t *allocated_list, unsigned long new_address);
void dll_insert_nth_node(dllist_t* list, size_t pos, size_t size, unsigned long  address);
void malloc_memory(sfl_t *sfl, size_t num_bytes, dllist_t **allocated_list);


// dll_block_t* dll_get_nth_node_address(dllist_t* list, size_t *n, void *address);
// void dll_add_node_addr(dllist_t *list, void *address, size_t size);


dll_block_t *dll_remove_nth_node(dllist_t* list, size_t n);


#endif /* FUNCTION_MALLOC_H */
