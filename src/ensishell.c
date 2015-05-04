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

#include "variante.h"
#include "readcmd.h"
#include "list.h"

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
		/*for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
                        for (j=0; cmd[j]!=0; j++) {
                                printf("'%s' ", cmd[j]);
                        }
			printf("\n");
			}
		*/

		/*exec*/
		int res;
		int status;
		 if (l->in!=NULL) {
            char* name = l->in;
            int file = open(name, O_RDWR, NULL);
            if (file==-1) {
                printf("ne peut pas ouvrir le fichier \"%s\" (%s)\n",name,strerror(errno));
                exit(1);
            }
            dup2(file,0);
            execvp(l->seq[0][0],l->seq[0]);
            close(file);
        }


        if (l->out!=NULL) {
            /* On récupère le nom de fichier de sortie */
            char* name = l->out;
            /* On ouvre le fichier destination */
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            int file = open(name, O_RDWR | O_CREAT, mode);
            if (file==-1) { 
                printf("ne peut pas créer le fichier \"%s\" (%s)\n",name,strerror(errno));
                exit(1);
            }   

            /* On redirige la sortie standard sur file */
            dup2(file,1);
            /* On exécute la commande */
            execvp(l->seq[0][0],l->seq[0]);
            close(file);
        }
		int jobs = !strcmp(l->seq[0][0], "jobs");
		 
		if(jobs){
		        delete_terminated(&li);
		}
		else{
		  if(!strcmp(l->seq[0][0], "jobs")){
                /*  char str[50] = "ps -o pid,cmd --ppid ";
                    char ppid [7];
                    sprintf(ppid,"%d",getpid());
                    strcat(str,ppid);
                    system(str);*/
            }
		   if((res=fork())==0){
		     execvp(l->seq[0][0],l->seq[0]);
		   }
		   else{
		     
		     if(!l->bg)
			waitpid(res, &status, 0);
		     else
		         add_processus(&li, res, l->seq[0][0]);
		      
		     
		   }
		 		
		}
	}
}
