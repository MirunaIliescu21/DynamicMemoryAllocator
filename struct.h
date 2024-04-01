#ifndef __STRUCT_H__
#define __STRUCT_H__


typedef struct block_t {
	void *address; /* Adresa de inceput a blocului */
	size_t size; /* Dimensiunea unui bloc */
	struct Block *next, *prev;

} block_t;

typedef struct {
	block_t *head;
	size_t count; /* Numarul de blocuri dintr-o lista */

	size_t data_size;
} list_t;

typedef struct {
	list_t *lists;
	size_t num_lists;
	size_t bytes_per_list;
	int reconstitution_type;
} sfl_t;

#define MAX_INPUT_LENGTH 100

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