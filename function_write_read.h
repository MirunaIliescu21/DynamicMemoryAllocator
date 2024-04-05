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
int check_memory(dllist_t *alloc_list, unsigned long addr, size_t num_bytes);

int write_memory(dllist_t *allocated_list, unsigned long address,
                 char* data, size_t num_bytes);


#endif