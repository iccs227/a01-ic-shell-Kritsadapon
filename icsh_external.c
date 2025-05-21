#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD_BUFFER 255

void external_cmd(char *buffer){
    int pid = fork();
    if (pid == 0){
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
        int status;
        waitpid(pid, &status, 0);
    }
}