#include "function_write_read.h"

#define DEBUG 0

/* AM NEVOIE DE O FUNCTIE CARE IMI RETURNEAZA BLOCUL CU CEA MAI MARE ADRESA
 MAI MICA DECAT ADRESA CITITA CA PARAMETRU IN FUNCTIA WRITE */
dll_block_t *dll_address(dllist_t *list, unsigned long adr)
{
	if (!list || !list->head) {
		if (DEBUG)
			printf("Lista neinitializata sau goala\n");
		return NULL;
	}

	if (adr < ((info_t *)list->head->data)->address) {
		/* blocurile din lista sunt ord cresc, deci nu exista
		 o adresa <= cu adresa ceruta*/
		if (DEBUG)
			printf("Adresa data < prima daresa din lista -> bloc null\n");
		return NULL;
	}

	dll_block_t *n = list->head;
	while (n->next != list->head && adr >= ((info_t *)n->next->data)->address)
		n = n->next;

	return n;
}

/*functia returneaza adresa urmatorului bloc daca aceastea ar fi consecutive*/
unsigned long end_address(dll_block_t *nod)
{
	unsigned long end;
	end = ((info_t *)nod->data)->address + ((info_t *)nod->data)->size;
	return end;
}

 /* Functie returneaza 1 pt toate cazurile de seg fault */
int check_memory(dllist_t *alloc_list, unsigned long addr, size_t num_bytes)
{
	if (!alloc_list->head || !alloc_list) {
		if (DEBUG)
			printf("Nu exista elemente alocate in lista/lista goala\n");
		return 1; /* caz de seg_fault */
	}

	dll_block_t *nod = dll_address(alloc_list, addr);

	/* Verific daca nodul este null sau are o adresa < adresa data */
	if (!nod || end_address(nod) <= addr) {
		if (DEBUG)
			printf("Nod gol / dimensiune prea mica\n");
		return 1; /* caz de seg fault */
	}

	/* Daca s-a ajuns aici verific daca exista suficiente spatii de memorie
	 continue pt a se scrie nr_bytes*/
	if (DEBUG)
		print_block_info(nod);

	long size_in_block = end_address(nod) - addr; /*cati bytes putem utiliza
	din blocul in care se afla adresa*/
	long diff = num_bytes - size_in_block; /* de cati bytes mai avem nevoie
	pentru a cuprinde intrega informatie */

	/* Pargur nodurile pana cand diff este egal cu 0
	sau adresele nu mai sunt consecutive */
	while (diff > 0 && nod->next != alloc_list->head) {
		if (end_address(nod) != ((info_t *)nod->next->data)->address) {
			if (DEBUG) {
				printf("end_address=0x%lx nod->next->adr=0x%lx\n",
					   end_address(nod), ((info_t *)nod->next->data)->address);
			}
			return 1; /* caz seg fault */
		}

		nod = nod->next;
		diff = diff - ((info_t *)nod->data)->size;
	}
	return 0;
}

void remove_quotes(char *str)
{
	int len = strlen(str);
	int i, j;
	for (i = 0, j = 0; i < len; i++) {
		if (str[i] != '"') {
			str[j] = str[i];
			j++;
		}
	}
	str[j] = '\0';
}

/* Fac o functie de tip int care sa imi returneze 0 in cazul in care se
 poate scrie in memorie si 1 atunci cand nu se poate scrie in memorie
 si trebuie apelate functiile de dump si de destroy */
int write_memory(dllist_t *allocated_list,
				 unsigned long address, char *data, size_t num_bytes)
{
	/* Verific daca se poate scrie in memorie */
	if (!allocated_list)
		return 1;

	if ((size_t)strlen(data) < num_bytes) {
		num_bytes = strlen(data);
		if (DEBUG)
			printf("num_bytes=%ld\n", num_bytes);
	}

	if (check_memory(allocated_list, address, num_bytes) == 1)
		return 1;

	dll_block_t *nod = dll_address(allocated_list, address);
	long size_in_block = end_address(nod) - address; /*cati bytes putem utiliza
	din blocul in care se afla adresa*/
	long diff = num_bytes - size_in_block; /* de cati bytes mai avem nevoie
	pentru a cuprinde intrega informatie */

	size_t index = 0;
	unsigned long idx_adrr = address - ((info_t *)nod->data)->address;
	/* idx_adrr reprezinta indicele de pe care incep sa scriu in bloc */

	remove_quotes(data); /* elimin ghilimelele */
	if (DEBUG)
		printf("Sirul fara ghilimele: %s\n", data);

	char *v = ((info_t *)nod->data)->data;

	size_t i = idx_adrr;
	while (i < ((info_t *)nod->data)->size && index < num_bytes) {
		v[i] = data[index];
		index++;
		i++;
	}

	while (diff > 0) {
		nod = nod->next;
		v = ((info_t *)nod->data)->data; /* scriu la adresa urm nod */
		size_t j = 0;
		while (j < ((info_t *)nod->data)->size && index < num_bytes) {
			v[j] = data[index];
			index++;
			j++;
		}
		diff = diff - ((info_t *)nod->data)->size;
	}

	return 0;
}

int read_memory(dllist_t *alloc_list, unsigned long addr, size_t num_bytes)
{
	/* Verific daca se poate scrie in memorie */
	if (!alloc_list)
		return 1;

	if (check_memory(alloc_list, addr, num_bytes) == 1)
		return 1;

	dll_block_t *nod = dll_address(alloc_list, addr);
	long size_in_block = end_address(nod) - addr; /*cati bytes putem utiliza
	din blocul in care se afla adresa*/
	long diff = num_bytes - size_in_block; /* de cati bytes mai avem nevoie
	pentru a cuprinde intrega informatie */

	size_t index = 0;
	unsigned long idx_adrr = addr - ((info_t *)nod->data)->address;
	/* idx_adrr reprezinta indicele de pe care incep sa scriu in bloc */

	char *v = ((info_t *)nod->data)->data;
	char *str = malloc((num_bytes + 1) * sizeof(char)); /* num_bytes + \0*/
	/*str este sirul pe care il afisez */

	memset(str, 0, (num_bytes + 1) * sizeof(char));
	size_t i = idx_adrr;
	while (i < ((info_t *)nod->data)->size && index < num_bytes) {
		str[index] = v[i];
		index++;
		i++;
	}
	/* trecem la urm nod/noduri pt a citi din ele */
	while (diff > 0) {
		nod = nod->next;
		v = ((info_t *)nod->data)->data; /* citesc de la adresa urm nod */
		size_t j = 0;
		while (j < ((info_t *)nod->data)->size && index < num_bytes) {
			str[index] = v[j];
			index++;
			j++;
		}
		diff = diff - ((info_t *)nod->data)->size;
	}

	/*afisez str*/
	printf("%s\n", str);
	free(str);
	return 0;
}
