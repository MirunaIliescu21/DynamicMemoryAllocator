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
size_t find_insert_position(dllist_t *allocated_list,
							unsigned long new_address);
void dll_insert_nth_node(dllist_t *list, size_t pos, size_t size,
						 unsigned long  address);
void malloc_memory(sfl_t *sfl, size_t num_bytes, dllist_t **allocated_list);
dll_block_t *dll_remove_nth_node(dllist_t *list, size_t n);

int find_addr_position(dllist_t *allocated_list, unsigned long new_address);
void free_memory(sfl_t *sfl, unsigned long address, dllist_t **allocated_list);

#endif /* FUNCTION_MALLOC_H */
