#ifndef __STRUCT_H__
#define __STRUCT_H__

// Strctura de nod
typedef struct block_t {
	struct block_t *next, *prev;
	void *data; /* pointeaza catre o structura de date */

} dll_block_t;

typedef struct info_t {
	unsigned long address;
	size_t size;
	// int reconstitution_type;
	char *data;

} info_t;

// Structura de lista
typedef struct {
	dll_block_t *head;
	size_t count; /* Numarul de blocuri dintr-o lista */
	size_t data_size; /* Dimensiunea datelor stocate */
} dllist_t;

// Structura vectorului de liste
typedef struct {
	dllist_t **lists; /* Vector de liste dublu inlantuite*/
	size_t num_lists;
	size_t bytes_per_list;
	int reconstitution_type;
	size_t total_memory;
	size_t total_allocated_memory;
	size_t total_free_memory;
	size_t num_free_blocks;
	size_t num_allocated_blocks;
	size_t num_malloc_calls;
	size_t num_fragmentations;
	size_t num_free_calls;
} sfl_t;

#define MAX_INPUT_LENGTH 600

/* Macro pentru gestiunea erorilor */
#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

#endif
