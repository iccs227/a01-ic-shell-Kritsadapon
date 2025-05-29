#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "icsh_jobs.h"

extern int show_cat_animation;

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
        if (!script && strchr(l_cmd, '>') == NULL) {
            printf("%s\n", l_cmd);
        }
        strcpy(buffer, l_cmd);
    //handling "!! " for redirection
    } else if (strncmp(buffer, "!! ", 3) == 0) {
        if (strlen(l_cmd) == 0) {
            printf("No previous command to repeat.\n");
            return 1;
        }
        //creating new command by concat l_cmd + buffer(excluding !!)
        char temp[1024];
        snprintf(temp, sizeof(temp), "%s%s", l_cmd, buffer + 2);
        strncpy(buffer, temp, 1023);
        buffer[1023] = '\0';
        //incase uer type "!! "
        if (!script && strchr(buffer, '>') == NULL) {
            printf("%s\n", buffer);
        }
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
    } else if (strncmp(buffer, "kitten", 5) == 0) {
        show_cat_animation = !show_cat_animation;
        if (show_cat_animation) {
            printf("Cat is here. ฅ^•⩊•^ฅ\n");
        } else {
            printf("Cat bye bye. /ᐠ◞ᆺ◟マ\n");
        }
        return 1;
    } else if (strncmp(buffer, "echo $?", 7) == 0) {
        printf("%d\n", *exit_code);
    } else if (strncmp(buffer, "echo ", 5) == 0) {
        //special handling for echo redirection
        // i dont know if the redirection for echo should be excute with external echo or buildin command
        char *check_out_redir = strchr(buffer, '>');
        if(check_out_redir) {
            char *output_file = check_out_redir;
            *output_file = '\0';
            output_file++;
            while (*output_file == ' ' || *output_file == '\t') output_file++;

            int out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (out < 0) {
                perror("Couldn't open output file");
                return 1;
            }
        write(out, buffer + 5, strlen(buffer + 5));
        write(out, "\n", 1);
        close(out);
        }else {printf("%s\n", buffer + 5);}
    } else if (strcmp(buffer, "jobs") == 0) {
        print_jobs();
        return 1;
    } else if (strncmp(buffer, "fg %", 4) == 0) {
        int jid = atoi(buffer + 4);
        if (jid <= 0) {
            printf("fg: invalid job id\n");
            return 1;
        }
        fg_job(jid, exit_code);
        return 1;
    }else if (strncmp(buffer, "bg %", 4) == 0) {
        int jid = atoi(buffer + 4);
        if (jid <= 0) {
            printf("bg: invalid job id\n");
            return 1;
        }
        bg_job(jid);
        return 1;
    }else {
        return 0;
    }
    return 1;
}