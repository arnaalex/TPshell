#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 #include <sys/types.h>
 #include <sys/wait.h>



struct processus{
	int pid;
	char* commande;
	struct processus* next;

};

typedef struct processus processus;
typedef processus* list;

void add_processus(list* l, int pid, char* commande);
void delete_terminated(list* l);
void delete_processus(list* l, processus* prev);
void print(list l);
