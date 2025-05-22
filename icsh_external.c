#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>
#include <signal.h>
#define MAX_CMD_BUFFER 255

extern pid_t fg_pgid;

void external_cmd(char *buffer, int *exit_status) {
    int pid = fork();
    if (pid == 0){

        setpgid(0, 0);


        char *args[MAX_CMD_BUFFER];
        int i = 0;
        char *token = strtok(buffer, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (execvp(args[0], args) == -1) {
            perror("Error");
            exit(1);
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