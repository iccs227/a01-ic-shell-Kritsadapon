/* ICCS227: Project 1: icsh
 * Name: Kritsadapon Wai-on
 * StudentID: 6481314
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD_BUFFER 255

void com_exe_script(char *buffer, char *l_cmd, int *exit_code) {
    if (strlen(buffer) == 0 || strspn(buffer, " \t") == strlen(buffer)) {
        return;
    }

    if (buffer[0] == '#' || buffer[0] == '\n') {
        return;
    }

    if (strcmp(buffer, "!!") == 0) {
        if (strlen(l_cmd) == 0) {
            printf("No previous command to repeat.\n");
            return;
        }
        strcpy(buffer, l_cmd);
        com_exe_script(buffer, l_cmd, exit_code);
        return;
    } else {
        strcpy(l_cmd, buffer);
    }

    if (strncmp(buffer, "exit", 4) == 0) {
        if (strncmp(buffer, "exit ", 5) == 0) {
            *exit_code = atoi(buffer + 5);
        }
        exit(*exit_code);
    } else if (strcmp(buffer, "clear") == 0) {
        system("clear");
    } else if (strncmp(buffer, "echo ", 5) == 0) {
        printf("%s\n", buffer + 5);
    } else {
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
}

void com_exe_cmd(char *buffer, char *l_cmd, int *exit_code) {

    if (strcmp(buffer, "!!") == 0) {
        if(strlen(l_cmd) == 0) {
            return;
        }
            printf("%s\n", l_cmd);
            strcpy(buffer, l_cmd);
        } else {
            strcpy(l_cmd, buffer);
        }

        // I already added exit without knowing that is part 3 of milestone 1
        if (strcmp(buffer, "exit") == 0) {
            if (strncmp(buffer, "exit ", 5) == 0) {
                *exit_code = atoi(buffer + 5);
            }
            printf("Exiting IC shell...\n");
            exit(*exit_code);
        } else if (strcmp(buffer, "clear") == 0) {
            system("clear");
        } else if (strncmp(buffer, "echo ", 5) == 0) {
            printf("%s\n", buffer + 5);
        }else {
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
}



int main(int argc, char *argv[]) {

    char buffer[MAX_CMD_BUFFER];
    char l_cmd[MAX_CMD_BUFFER] = "";
    FILE *fptr;
    int exit_code = 0;

    if (argc == 2){
        fptr = fopen(argv[1], "r");
        if (!fptr) {
            printf("Error can't read files %s\n", argv[1]);
            return 1;
        }
        while(fgets(buffer, MAX_CMD_BUFFER, fptr)){
            buffer[strcspn(buffer, "\n")] = '\0';
            com_exe_script(buffer, l_cmd, &exit_code);
        }
        fclose(fptr);
    }else{
        printf("Starting IC shell!\n");
        printf("******************************** ^u^ **********************************\n");
        printf("Welcome to IC Shell\n");
        while (1) {
            printf("icsh $ ");
            if (!fgets(buffer, 255, stdin)) {
                break;
            }
            buffer[strcspn(buffer, "\n")] = '\0';
            com_exe_cmd(buffer, l_cmd, &exit_code);
        }
    }
}
