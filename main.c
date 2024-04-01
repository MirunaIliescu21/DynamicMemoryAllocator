#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "struct.h"

/* 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 */
#define DEBUG 1 /* Seteazala 1 pentru a activa afisarea, la 0 pentru a dezactiva */


int main() {
    sfl_t sfl;
    char input[MAX_INPUT_LENGTH];

    while (fgets(input, sizeof(input), stdin) != NULL) {
        char* command = strtok(input, " \n");
        if (strcmp(command, "INIT_HEAP") == 0) {
            void* start_address = (void*)strtol(strtok(NULL, " \n"), NULL, 16);
			 /* Converteste primul token la un numar in baza 16 */
            size_t num_lists = atoi(strtok(NULL, " \n"));
            size_t bytes_per_list = atoi(strtok(NULL, " \n"));
            int reconstitution_type = atoi(strtok(NULL, " \n"));
			#ifdef DEBUG
				printf("Parametri INIT_HEAP: start_address=%p, num_lists=%ld, bytes_per_list=%ld, reconstitution_type=%d\n", 
                    start_address, num_lists, bytes_per_list, reconstitution_type);
			#endif
            initialize_heap(&sfl, start_address, num_lists, bytes_per_list, reconstitution_type);

        } else if (strcmp(command, "MALLOC") == 0) {
            size_t num_bytes = atoi(strtok(NULL, " \n"));
			#ifdef DEBUG
				printf("Parametru MALLOC: num_bytes=%ld\n", num_bytes);
			#endif
            malloc_memory(&sfl, num_bytes);

        } else if (strcmp(command, "FREE") == 0) {
            void *address = (void*)strtol(strtok(NULL, " \n"), NULL, 16);
			#ifdef DEBUG
				printf("Parametru FREE: address=%p\n", address);
			#endif
            free_memory(&sfl, address);

        } else if (strcmp(command, "READ") == 0) {
            void *address = (void*)strtol(strtok(NULL, " \n"), NULL, 16);
            size_t num_bytes = atoi(strtok(NULL, " \n"));
			#ifdef DEBUG
	            printf("Parametri READ: address=%p, num_bytes=%ld\n", address, num_bytes);
			#endif
            read_memory(address, num_bytes);

        } else if (strcmp(command, "WRITE") == 0) {
            void *address = (void*)strtol(strtok(NULL, " \n"), NULL, 16);
            char *data = strtok(NULL, "\"");
            size_t num_bytes = atoi(strtok(NULL, " \n"));
			#ifdef DEBUG
	            printf("Parametri WRITE: address=%p, data=\"%s\",  num_bytes=%ld\n", address, data, num_bytes);
			#endif
            write_memory(address, data, num_bytes);
			
        } else if (strcmp(command, "DUMP_MEMORY") == 0) {
            dump_memory(&sfl);
        } else if (strcmp(command, "DESTROY_HEAP") == 0) {
            destroy_heap(&sfl);
            break; /* Se iese din program */
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}
