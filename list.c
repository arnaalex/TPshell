typedef struct processus processus;

struct processus{
	int pid;
	char* commande;
	processus* next;

};

typedef processus* list;

void add_processus(list* l, int pid, char* commande){

processus* p = malloc(sizeof(processus));
p->pid = pid;
p->commande = commande;
p->next = NULL;
 
if(*list==NULL){
	*list = p;
}
else{
processus* ptr = *list;

while(ptr->next!=NULL)
	ptr=ptr->next;

ptr->next = p;

}

}
void delete_terminated(list* l);

void delete_processus(list* l, processus* prev){
if(prev==NULL){
	processus head = *list;
	*list =(*list)->next;
	free(head);
}
else{
processus* ptr = prev->next;
prev->next = (prev->next)->next;
free(ptr);
}
}
void print(list l);
