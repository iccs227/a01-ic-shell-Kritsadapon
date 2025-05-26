#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#define MAX_CMD_BUFFER 1024

extern pid_t fg_pgid;

void external_cmd(char *buffer, int *exit_status) {
    int pid = fork();
    if (pid == 0){

        setpgid(0, 0);


        char *args[MAX_CMD_BUFFER];
        int i = 0;
        char *token = strtok(buffer, " ");
        char *intake_file= NULL;
        char *output_file = NULL;

        //split the command into arguments
        while (token != NULL) {
            if (strcmp(token, "<") == 0) {
                token = strtok(NULL," ");
                intake_file = token;

            } else if (strcmp(token, ">") == 0) {
                token = strtok(NULL, " ");
                output_file = token;
            } else {
                args[i++]= token;
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // for (int j = 0; args[j] != NULL; j++) {
        //     printf("args[%d]: %s\n", j, args[j]);
        // }

        // Input handling
        if (intake_file) {
            int in = open(intake_file, O_RDONLY);
            if (in < 0) {
                perror("Couldn't open a intake file");
                exit(errno);
            }
            dup2(in, 0);
            close(in);
        }
        // Output handling
        if (output_file) {
            int out = open(output_file,O_WRONLY | O_CREAT | O_TRUNC, 0666 );
            if (out < 0) {
                perror("Couldn't open a output file");
                exit(errno);
            }
            dup2(out, 1);
            close(out);
        }

        if (execvp(args[0], args) == -1 ) {
            perror("Error");
            exit(errno);
        }
    } else{
        setpgid(pid, pid);
        
        fg_pgid = pid;
        
        tcsetpgrp(STDIN_FILENO, pid);
        
        int status;
        //wait for process to finished/end
        waitpid(pid, &status, WUNTRACED);

        fg_pgid = 0;

        tcsetpgrp(STDIN_FILENO, getpgid(0));

        //end normally
        if (WIFEXITED(status)) {
            *exit_status = WEXITSTATUS(status);
        }//process terminated
        else if (WIFSIGNALED(status)) {
            *exit_status = WTERMSIG(status);
        }//process suspend
        else if (WIFSTOPPED(status)) {
            *exit_status = WSTOPSIG(status);
            printf("\n[%d] Stopped\n", pid);
        }
    }
}