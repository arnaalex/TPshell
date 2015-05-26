/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 *           Simon Nieuviarts 2002-2009              *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include "variante.h"
#include "readcmd.h"
#include "list.h"
#include <errno.h>
#ifndef VARIANTE
#error "Variante non défini !!"
#endif

int main() {
  printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);
  list li = NULL;

  while (1) {
    struct cmdline *l;
    //int i, j;
    char *prompt = "ensishell>";

    l = readcmd(prompt);

    /* If input stream closed, normal termination */
    if (!l) {
      printf("exit\n");
      exit(0);
    }

    if (l->err) {
      /* Syntax error, read another command */
      printf("error: %s\n", l->err);
      continue;
    }

    if (l->in) printf("in: %s\n", l->in);
    if (l->out) printf("out: %s\n", l->out);
    if (l->bg) printf("background (&)\n");

    /* Display each command of the pipe */
      for (int i=0; l->seq[i]!=0; i++) {
	char **cmd = l->seq[i];
	printf("seq[%d]: ", i);
	for (int j=0; cmd[j]!=0; j++) {
	  printf("'%s' ", cmd[j]);
	}
	printf("\n");
      }
    

    /*exec*/
    if(l->seq[0]!=NULL){
      int res, res2;
    int status;
    int file_in, file_out;
    int tuyau[2];
    int pipes= 0;
    int old_in = dup(0);
    int old_out = dup(1);

    if (l->in!=NULL) {
      char* name = l->in;
      file_in = open(name, O_RDWR, NULL);
      if (file_in==-1) {
	printf("ne peut pas ouvrir le fichier \"%s\" (%s)\n",name,strerror(errno));
	exit(1);
      }
      dup2(file_in,0);
      close(file_in); 
    }


    if (l->out!=NULL) {
      /* On récupère le nom de fichier de sortie */
      char* name = l->out;
      /* On ouvre le fichier destination */
      mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
      file_out = open(name, O_RDWR | O_CREAT, mode);
      if (file_out==-1) { 
	printf("ne peut pas créer le fichier \"%s\" (%s)\n",name,strerror(errno));
	exit(1);
      }   
      dup2(file_out,1);
      close(file_out);
     }

    if(l->seq[1]!=NULL){
	pipe(tuyau);
	pipes = 1;
    }  

    int jobs = !strcmp(l->seq[0][0], "jobs");
    	 
    if(jobs){
      delete_terminated(&li);
    }
    else{
      if((res=fork())==0){
	if(pipes){
	  dup2(tuyau[0], 0);
	  close(tuyau[1]);
	  close(tuyau[0]);
	  execvp(l->seq[1][0],l->seq[1]);
	}
	else
	  execvp(l->seq[0][0],l->seq[0]);
      }
    
      if(pipes &&((res2=fork())==0)){
	  dup2(tuyau[1], 1);
	  close(tuyau[0]); 
	  close(tuyau[1]);
	  execvp(l->seq[0][0],l->seq[0]);
	}
     
	if(pipes){
	  close(tuyau[0]); 
	  close(tuyau[1]);
	}

      if(!l->bg){
	  waitpid(res, &status, 0);
      }
	else
	  add_processus(&li, res, l->seq[0][0]);

	if(l->in!=NULL || pipes)
	  dup2(old_in,0);
	 	
	if(l->out!=NULL || pipes)
	  dup2(old_out,1);
	

	close(old_in);	 		
	close(old_out);
    }
    }
}
}
