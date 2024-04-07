#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "struct.h"
#include "function.h"
#include "function_malloc.h"
#include "function_write_read.h"

/* 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 */
/* valgrind -s --leak-check=full --show-leak-kinds=all
--tool=memcheck --track-origins=yes ./sfl*/
#define DEBUG 0 /* Seteazala 1 pentru a activa afisarea, la 0 pt a dezactiva */

int main(void)
{
	sfl_t *sfl = NULL;
	dllist_t *allocated_list = NULL;
	char input[MAX_INPUT_LENGTH];

	while (fgets(input, sizeof(input), stdin)) {
		char *command = strtok(input, " \n");
		if (strcmp(command, "INIT_HEAP") == 0) {
			unsigned long start_address = strtol(strtok(NULL, " \n"), NULL, 16);
			 /* Converteste primul token la un numar in baza 16 */
			size_t num_lists = atoi(strtok(NULL, " \n"));
			size_t bytes_per_list = atoi(strtok(NULL, " \n"));
			int reconstitution_type = atoi(strtok(NULL, " \n"));
			sfl = init_heap(start_address, num_lists, bytes_per_list,
							reconstitution_type);

		} else if (strcmp(command, "MALLOC") == 0) {
			size_t num_bytes = atoi(strtok(NULL, " \n"));
			if (DEBUG)
				printf("Parametru MALLOC: num_bytes=%ld\n", num_bytes);
			malloc_memory(sfl, num_bytes, &allocated_list);

		} else if (strcmp(command, "FREE") == 0) {
			unsigned long address = strtol(strtok(NULL, " \n"), NULL, 16);

			if (DEBUG)
				printf("Parametru FREE: address=0x%lx\n", address);
			free_memory(sfl, address, &allocated_list);

		} else if (strcmp(command, "READ") == 0) {
			unsigned long address = strtol(strtok(NULL, " \n"), NULL, 16);
			size_t num_bytes = atoi(strtok(NULL, " \n"));

			if (read_memory(allocated_list, address, num_bytes) == 1) {
				printf("Segmentation fault (core dumped)\n");
				dump_memory(sfl, allocated_list);
				destroy_heap(&sfl, &allocated_list);
				break; /* se iese din program */
			}

		} else if (strcmp(command, "WRITE") == 0) {
			unsigned long address = strtol(strtok(NULL, " \n"), NULL, 16);
			char *data = strtok(NULL, "\"");
			size_t num_bytes = atoi(strtok(NULL, " \n"));
			if (write_memory(allocated_list, address, data, num_bytes) == 1) {
				printf("Segmentation fault (core dumped)\n");
				dump_memory(sfl, allocated_list);
				destroy_heap(&sfl, &allocated_list);
				break; /* se iese din program */
			}

		} else if (strcmp(command, "DUMP_MEMORY") == 0) {
			dump_memory(sfl, allocated_list);
		} else if (strcmp(command, "DESTROY_HEAP") == 0) {
			destroy_heap(&sfl, &allocated_list);
			break; /* Se iese din program */
		} else {
			printf("Invalid command\n");
		}
	}
	return 0;
}
