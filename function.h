#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdlib.h>
#include "struct.h"


/* Headerele functiilor */
void initialize_heap(sfl_t *sfl, void* start_address, size_t num_lists, size_t bytes_per_list, int reconstitution_type);
void malloc_memory(sfl_t *sfl, size_t num_bytes);
void free_memory(sfl_t *sfl, void* address);
void read_memory(void* address, size_t num_bytes);
void write_memory(void* address, char* data, size_t num_bytes);
void dump_memory(sfl_t *sfl);
void destroy_heap(sfl_t *sfl);

#endif /* FUNCTION_H */
