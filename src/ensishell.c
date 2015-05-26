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
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include "variante.h"
#include "readcmd.h"
#include "list.h"
#include <errno.h>
#ifndef VARIANTE
#error "Variante non défini !!"
#endif

/* définition du "signal handler" */
void traite_signal( int s )
{
    int status = 0;
    waitpid(WAIT_ANY, &status, WNOHANG);
    //int pid = waitpid(WAIT_ANY, &status, WNOHANG);
    /*if(pid > 0){
            printf( "Un processus fils vient de se terminer : %d\n", pid);
    }*/
}

int main() {
    printf("Variante %d: %s\n", VARIANTE, VARIANTE_STRING);
    list li = NULL;
    int ulimit = 0;

    /* Register signal and signal handler */
    signal( SIGCHLD, traite_signal);
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

        if(l->seq[0] == NULL) {
            continue;
        }

        // Commande interne au shell : ulimit
        if(strcmp(l->seq[0][0], "ulimit") == 0){
            ulimit = atoi(l->seq[0][1]);
            if(ulimit <= 0) {
                ulimit = 0;
            }
            continue;
        }
        if(strcmp(l->seq[0][0], "exit") == 0){
            break;
        }
        if(strcmp(l->seq[0][0], "jobs") == 0){
            delete_terminated(&li);
        }

        if (l->in) printf("in: %s\n", l->in);
        if (l->out) printf("out: %s\n", l->out);
        if (l->bg) printf("background (&)\n");

        /* Display each command of the pipe */
        /*for (int i=0; l->seq[i]!=0; i++) {
            char **cmd = l->seq[i];
            printf("seq[%d]: ", i);
            for (int j=0; cmd[j]!=0; j++) {
                printf("'%s' ", cmd[j]);
            }
            printf("\n");
        }*/


        /*exec*/
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

        
   
        if((res=fork())==0){
            if(ulimit > 0) {
                struct rlimit rlim;
                rlim.rlim_cur = ulimit;
                rlim.rlim_max = ulimit + 5;
                setrlimit(RLIMIT_CPU ,&rlim);
            }
            if(pipes){
                dup2(tuyau[0], 0);
                close(tuyau[1]);
                close(tuyau[0]);
                execvp(l->seq[1][0],l->seq[1]);
                printf("Command introuvable : %s\n", l->seq[0][0]);
                exit(0);
            }
            else {
                execvp(l->seq[0][0],l->seq[0]);
                printf("Command introuvable : %s\n", l->seq[0][0]);
                exit(0);
            }
        }

        if(pipes &&((res2=fork())==0)){
            dup2(tuyau[1], 1);
            close(tuyau[0]); 
            close(tuyau[1]);
            execvp(l->seq[0][0],l->seq[0]);
            printf("Command introuvable : %s\n", l->seq[0][0]);
            exit(0);
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

