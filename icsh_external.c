#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <errno.h>
#include "icsh_jobs.h"
#include "icsh_animation.h"
#define MAX_CMD_BUFFER 1024

extern pid_t fg_pgid;

void external_cmd(char *buffer, int *exit_status) {

    int background = 0;
    size_t len = strlen(buffer);

    //check if bg process
    //cleaning the command line removing
    if (len > 0 && buffer[len-1] == '&') {
        background= 1;
        buffer[len-1] = '\0';
        while (len > 1 && (buffer[len-2] == ' ' || buffer[len-2] =='\t')) {
            buffer[len-2] = '\0';
            len--;
        }
    }
    char cmdline[MAX_CMD_BUFFER];
    strncpy(cmdline,buffer, MAX_CMD_BUFFER-1);
    cmdline[MAX_CMD_BUFFER-1] = '\0';

    int pid = fork();
    if (pid == 0){

        setpgid(0, 0);

        //set signal default
        signal(SIGTSTP, SIG_DFL);
        signal(SIGINT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);


        if (!background) {
            tcsetpgrp(STDIN_FILENO, getpgid(0));
        }

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

        if (background) {
            save_job(pid, cmdline, 0);
        } else {
            fg_pgid = pid;

            tcsetpgrp(STDIN_FILENO, pid);

            int status;
            //check if sleep
            int is_sleep = (strncmp(buffer, "sleep", 5) == 0);
            
            if (is_sleep) {
                start_kitten_animation(cmdline);
            }

            //wait for process to finished/end
            waitpid(pid, &status, WUNTRACED);

            if (is_sleep) {
                stop_kitten_animation();
            }

            fg_pgid = 0;

            tcsetpgrp(STDIN_FILENO, getpgid(0));

            //end normally
            if (WIFEXITED(status)) {
                *exit_status = WEXITSTATUS(status);
            }//process terminated
            else if (WIFSIGNALED(status)) {
                *exit_status = WTERMSIG(status);
                release_job(pid);
            }//process suspend
            else if (WIFSTOPPED(status)) {
                *exit_status = WSTOPSIG(status);
                if (WSTOPSIG(status) == SIGTSTP) {
                    int check_status;
                    if (waitpid(pid, &check_status, WNOHANG) == 0) {
                        int jid = get_jid(pid);
                        if (jid == -1) {
                            save_job(pid, cmdline, 1);
                            printf("\n[%d]  + %d suspended  %s\n", get_jid(pid), pid, cmdline);
                        }
                    }
                }
            }
        }
    }
}