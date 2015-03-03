typedef struct processus processus;

struct processus{
	int pid;
	char* commande;
	processus* next;

};

typedef processus* list;

void add_processus(list* l, processus* p);
void delete_terminated(list* l);
void delete_processus(list* l, processus* prev);
void print(list l);
