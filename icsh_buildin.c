#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>

int buildin_cmd(char *buffer, char *l_cmd, int *exit_code, int script) {
    if (strlen(buffer) == 0 || strspn(buffer, " \t") == strlen(buffer)) {
        return 1;
    }

    //script mode check for # and /n
    if (buffer[0] == '#' || buffer[0] == '\n') {
        return 1;
    }

    if (strcmp(buffer, "!!") == 0) {
        if (strlen(l_cmd) == 0) {
            printf("No previous command to repeat.\n");
            return 1;
        }
        if (!script) {
            printf("%s\n", l_cmd);
        }
        strcpy(buffer, l_cmd);
    } else {
        strcpy(l_cmd, buffer);
    }

    //execute buffer
    if (strncmp(buffer, "exit", 4) == 0) {
        if (strncmp(buffer, "exit ", 5) == 0) {
            *exit_code = atoi(buffer + 5);
        }
        if (!script) {
            printf("Exiting IC shell...\n");
        }
        exit(*exit_code);
    } else if (strcmp(buffer, "clear") == 0) {
        system("clear");
    } else if (strncmp(buffer, "echo $?", 7) == 0) {
        printf("%d\n", *exit_code);
    } else if (strncmp(buffer, "echo ", 5) == 0) {
        printf("%s\n", buffer + 5);
    } else {
        return 0;
    }
    return 1;
}