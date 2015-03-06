#include "list.h"

void add_processus(list* l, int pid, char* commande){
 processus* p = (struct processus*) malloc(sizeof(struct processus));
p->pid = pid;
 p->commande = (char*) malloc((strlen(commande)+1)*sizeof(char));
 strcpy(p->commande,commande);
p->next = NULL;
 
if(*l==NULL)
  *l = p;
else{
 processus* ptr = *l;
 while(ptr->next!=NULL)
	ptr=ptr->next;
ptr->next = p;

}
// print(*l);
}
void delete_terminated(list* l){
  int status;
  processus* ptr = *l;
  processus* prev = NULL;
 

  if (ptr==NULL){
    return;
  
  }
  while(ptr!=NULL){
   printf("processus %s %d ",ptr->commande, ptr->pid);
    
   if (waitpid (ptr->pid, &status, WNOHANG)){
    printf("is done\n");
    processus* next = ptr->next;
    delete_processus(l, prev);
    ptr = next; 
   
  }
   else{
    printf("is executing\n");
      prev = ptr;
      ptr=ptr->next;
   }
}
 

  /* printf("AFTER\n");
  print(*l);
  fflush(stdout);*/
}

void delete_processus(list* l, processus* prev){
if(prev==NULL){
        processus* head = *l;
	*l=head->next;
	free(head->commande);
	free(head);
}
else{
 processus* ptr = prev->next;
 prev->next = ptr->next;
 free(ptr->commande);
 free(ptr);
}
}

void print(list l){
 processus* ptr = l;
  

  while(ptr!=NULL){
    printf("** processus %d ", ptr->pid);
    printf("processus %s \n",ptr->commande);
    ptr=ptr->next;
}
 fflush(stdout);
}
