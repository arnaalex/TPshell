#include "list.h"

void add_processus(list* l, int pid, char* commande){

processus* p = malloc(sizeof(processus));
p->pid = pid;
p->commande = commande;
p->next = NULL;
 
if(*l==NULL){
	*l = p;
	printf("ololoo\n");
}
else{
processus* ptr = *l;

while(ptr->next!=NULL)
	ptr=ptr->next;

ptr->next = p;

}

}
void delete_terminated(list* l){
  int status;
  processus* ptr = *l;
  processus* prev = NULL;

  if (ptr==NULL)
    return;
  while(ptr!=NULL){
   printf("processus %s %d ",ptr->commande, ptr->pid);
    
   if (waitpid (ptr->pid, &status, WNOHANG)){
    printf("is done\n");
    // delete_processus(l, prev);
  }
  else
    printf("is executing\n");
      prev = ptr;
      ptr=ptr->next;
}

}

void delete_processus(list* l, processus* prev){
if(prev==NULL){
	processus* head = *l;
	*l =(*l)->next;
	free(head);
}
else{
processus* ptr = prev->next;
prev->next = (prev->next)->next;
free(ptr);
}
}

void print(list l){
}
