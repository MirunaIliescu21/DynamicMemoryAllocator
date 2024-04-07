#include "function.h"

#define DEBUG 0 /* Seteazala 1 pt a activa afisarea, la 0 pt a dezactiva */
#define DEBUG_DESTROY 0

/* Implementarea functiilor */

dllist_t *dll_create(size_t data_size)
{
	dllist_t *list = (dllist_t *)malloc(sizeof(dllist_t));
	DIE(!list, "Malloc failed for memory allocatin.\n");
	list->head = NULL;
	list->count = 0;
	list->data_size = data_size;
	if (DEBUG)
		printf("S-a creat o lista de dimensiunea %ld.\n", data_size);
	return list;
}

void
dll_add_nth_node(dllist_t *list, size_t n)
{
	/* TODO */
	DIE(!list, "The list is invalid!\n");

	if (n > list->count)
		n = list->count;

	// creere un nod nou
	dll_block_t *nod;
	nod = malloc(sizeof(*nod));
	DIE(!nod, "Failed to add new node\n");
	nod->data = malloc(sizeof(info_t));
	DIE(!nod->data, "Failed to alloc new node info\n");

	/* cazul in care lista nu are inca niciun element */
	if (list->count == 0) {
		/* noul nod devine si urmatorul si anteriorul */
		list->head = nod;
		nod->next = nod;
		nod->prev = nod;

		list->count++;
		return;
	}

	/* aflu pozitia nodului de pe pozitia n */
	/* locul unde vrea sa inserez un nou nod */
	dll_block_t *nth_node = dll_get_nth_node(list, n);
	/* noi vrem sa adaugam nod in locul lui nth nod */
	nod->prev = nth_node->prev;
	nod->next = nth_node;
	nth_node->prev->next = nod;
	nth_node->prev = nod;

	if (n == 0)
		list->head = nod;

	list->count++;
}

dll_block_t *
dll_get_nth_node(dllist_t *list, size_t n)
{
	if (list->count == 0)
		return NULL;

	while (n > list->count)
		n = n % list->count;

	dll_block_t *curr = list->head;

	for (unsigned int i = 0; i < n; i++)
		curr = curr->next;

	return curr;
}

void print_list_addresses(dllist_t *list)
{
	if (!list) {
		printf("Lista este nulă.\n");
		return;
	}

	dll_block_t *current_block = list->head;

	if (DEBUG)
		printf("Adresele blocurilor din lista sunt:\n");

	for (size_t i = 0; i < list->count; i++) {
		printf(" 0x%lx", ((info_t *)current_block->data)->address);
		current_block = current_block->next;
	}

	printf("\n");
}

void print_list_addresses_for_alloc(dllist_t *list)
{
	if (!list) {
		printf("Lista este nulă.\n");
		return;
	}

	dll_block_t *current_block = list->head;

	if (DEBUG)
		printf("Adresele blocurilor din lista sunt:\n");

	for (size_t i = 0; i < list->count; i++) {
		printf(" (0x%lx - %ld)", ((info_t *)current_block->data)->address,
			   ((info_t *)current_block->data)->size);
		current_block = current_block->next;
	}
}

sfl_t*
init_heap(unsigned long start_address, size_t num_lists,
		  size_t bytes_per_list, int type)
{
	sfl_t *sfl = (sfl_t *)malloc(sizeof(sfl_t));
	if (!sfl)
		return NULL; /* Eroare alocare memorie pentru vectorul de liste*/

	/* Dim maxima pe care o poate avea vectorului de liste */
	size_t hmax = bytes_per_list;
	if (DEBUG) {
		printf("hmax=%ld\n", hmax);
		printf("start_address=%lu\n", start_address);
		printf("start_address2=%lu\n", start_address + 8);
	}

	sfl->lists = (dllist_t **)malloc(hmax * sizeof(dllist_t *));

	if (!sfl->lists) {
		free(sfl); /* Eliberare memorie pentru structura*/
		return NULL; /* Eroare alocare memorie pentru liste*/
	}

	size_t cnt = 0;
	size_t num_blocks = 0;
	/* contor pt a numara daca am construit atatea liste
	cate sunt cerute initial */
	for (size_t i = 0; i < hmax; i++) {
		size_t data_size = i + 1;
		sfl->lists[i] = dll_create(sizeof(info_t));
		if ((i + 1) >= 8 && ((i + 1) & i) == 0) {
			if (cnt < num_lists) {
				/* verif daca este o puetere a lui 2 >= 8 si
				 daca s-a creat nr cerut de liste*/
				/* adaugam noduri */
				size_t num_nodes = bytes_per_list / data_size;
				num_blocks = num_blocks + num_nodes;

				for (size_t j = 0; j < num_nodes; j++) {
					dll_add_nth_node(sfl->lists[i], j);
					dll_block_t *node = dll_get_nth_node(sfl->lists[i], j);
					if (node) {
						size_t n = j * data_size;
						((info_t *)node->data)->address = start_address + n;
						((info_t *)node->data)->size = data_size;
						((info_t *)node->data)->data = NULL;
					}
				}
				/* Adresa de la care incepe urmatoarea lista */
				start_address = start_address + bytes_per_list;
				cnt++;
			}
		}
	}

	/* Initializez campurile structurii sfl_t */
	sfl->num_lists = num_lists; /* Numarul de liste ce contin blosuri libere*/
	sfl->bytes_per_list = bytes_per_list; /* Numarul total de bytes per lista*/
	sfl->reconstitution_type = type;
	sfl->total_memory = num_lists * bytes_per_list;
	/* Valoare care ramane nemodificata */
	sfl->total_allocated_memory = 0; /* Creste in urma comenzii de malloc */
	sfl->total_free_memory = sfl->total_memory;
	/* Scade in urma operatiei de malloc*/
	sfl->num_free_blocks = num_blocks; /*Numarul de blocuri din fiecare lista*/
	sfl->num_allocated_blocks = 0; /* Numarul de blocuri alocate */
	sfl->num_malloc_calls = 0; /* Numarul de apelari ale comenzii malloc */
	sfl->num_fragmentations = 0;
	/* Numarul de fragmentari ce au loc in urma apelului comenzii malloc */
	sfl->num_free_calls = 0; /* Numarul de apelari ale comenzii free */

	return sfl;
}

void dump_memory(sfl_t *sfl, dllist_t *allocated_list)
{
	printf("+++++DUMP+++++\n");
	printf("Total memory: %ld bytes\n", sfl->total_memory);
	printf("Total allocated memory: %ld bytes\n", sfl->total_allocated_memory);
	printf("Total free memory: %ld bytes\n", sfl->total_free_memory);
	printf("Free blocks: %ld\n", sfl->num_free_blocks);
	printf("Number of allocated blocks: %ld\n", sfl->num_allocated_blocks);
	printf("Number of malloc calls: %ld\n", sfl->num_malloc_calls);
	printf("Number of fragmentations: %ld\n", sfl->num_fragmentations);
	printf("Number of free calls: %ld\n", sfl->num_free_calls);

	for (size_t i = 0; i < sfl->bytes_per_list; i++) {
		if (sfl->lists[i]->count > 0) {
			/* Lista nu este goala, deci are blocuri ce contin adrese libere */
			printf("Blocks with %ld bytes", i + 1);
			printf(" - %ld free block(s) :", sfl->lists[i]->count);
			print_list_addresses(sfl->lists[i]);
		}
	}

	printf("Allocated blocks :"); // Blocurile alocate

	if (allocated_list && allocated_list->count > 0)
		print_list_addresses_for_alloc(allocated_list);

	printf("\n-----DUMP-----\n");
}

void destroy_heap(sfl_t **sfl, dllist_t **allocated_list)
{
	if (!sfl || !(*sfl))
		return;

	for (size_t i = 0; i < (*sfl)->bytes_per_list; i++) {
		dllist_t *current_list = (*sfl)->lists[i];
		if (!current_list)
			continue;

		/* Parcurg fiecare nod din lista curenta si eliberez memoria */
		size_t count = (*sfl)->lists[i]->count;

		for (size_t j = 0; j < count; j++) {
			dll_block_t *temp = dll_remove_nth_node((*sfl)->lists[i], 0);
			if (((info_t *)temp->data)->data)
				free(((info_t *)temp->data)->data);
			if (temp->data)
				free(temp->data);
			free(temp);
		}

		free((*sfl)->lists[i]);
	}
	free((*sfl)->lists);
	free((*sfl));
	*sfl = NULL;

	if (!allocated_list || !(*allocated_list))
		return;

	dll_block_t *nod = (*allocated_list)->head;
	dll_block_t *next_nod = NULL;

	for (unsigned int i = 0; i < (*allocated_list)->count; i++) {
		next_nod = nod->next;
		if (((info_t *)nod->data)->data)
			free(((info_t *)nod->data)->data);

		if (nod->data)
			free(nod->data);

		free(nod);
		nod = next_nod;
	}
	free((*allocated_list));
	*allocated_list = NULL;
}
