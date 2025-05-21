/* ICCS227: Project 1: icsh
 * Name: Kritsadapon Wai-on
 * StudentID: 6481314
 */

#include "stdio.h"
#include "string.h"
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "icsh_buildin.h"
#include "icsh_external.h"

#define MAX_CMD_BUFFER 255

void exe_cmd(char *buffer, char *l_cmd, int *exit_code, int script) {
    char temp[MAX_CMD_BUFFER];
    strcpy(temp, buffer);
    if (!buildin_cmd(temp, l_cmd, exit_code, script)) {
        strcpy(temp, buffer);
        external_cmd(temp);
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
            exe_cmd(buffer, l_cmd, &exit_code, 1);
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
            exe_cmd(buffer, l_cmd, &exit_code, 0);
        }
    }
}
