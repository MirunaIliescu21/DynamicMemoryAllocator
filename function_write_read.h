#ifndef FUNCTION_WRITE_READ_H
#define FUNCTION_WRITE_READ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "struct.h"
#include "function.h"
#include "function_malloc.h"

dll_block_t *dll_address(dllist_t *list, unsigned long addr);
unsigned long end_address(dll_block_t *nod);
int check_memory(dllist_t *alloc_list, unsigned long addr, size_t num_bytes);
void remove_quotes(char *str);
int write_memory(dllist_t *allocated_list, unsigned long address,
				 char *data, size_t num_bytes);

int read_memory(dllist_t *alloc_list, unsigned long addr, size_t num_bytes);

#endif
