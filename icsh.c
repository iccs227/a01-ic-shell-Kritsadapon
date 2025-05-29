/* ICCS227: Project 1: icsh
 * Name: Kritsadapon Wai-on
 * StudentID: 6481314
 */

#include "stdio.h"
#include "string.h"
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "icsh_buildin.h"
#include "icsh_external.h"
#include "icsh_jobs.h"

#define MAX_CMD_BUFFER 1024


pid_t fg_pgid = 0;

//send SIGTSTP signal to process group foreground_pgid
void sigtstp_handler(int sig) {
    if (fg_pgid != 0) {
        kill(-fg_pgid, SIGTSTP);
    } else {
        printf("\n");
        fflush(stdout);
    }
}

//send SIGINT signal to process group foreground_pgid
void sigint_handler(int sig) {
    if (fg_pgid != 0) {
        kill(-fg_pgid, SIGINT);
    } else {
        printf("\n");
        fflush(stdout);
    }
}

void sigchld_handler(int sig) {
    check_exit_children();
}


void exe_cmd(char *buffer, char *l_cmd, int *exit_code, int script) {
    char temp[MAX_CMD_BUFFER];
    strcpy(temp, buffer);
    if (!buildin_cmd(temp, l_cmd, exit_code, script)) {
        external_cmd(temp, exit_code);
    }
}

int main(int argc, char *argv[]) {

    struct sigaction sa;
    //this part fix the ctrl + z
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    sa.sa_handler = sigtstp_handler;
    sigaction(SIGTSTP, &sa, NULL);
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = sigchld_handler;
    sigaction(SIGCHLD, &sa, NULL);

    //ignoring bg process read/write signal to terminal
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    setpgid(0, 0);
    
    char buffer[MAX_CMD_BUFFER];
    char l_cmd[MAX_CMD_BUFFER] = "";
    FILE *fptr;
    int exit_code = 0;

    //if argument == 2, script mode
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
        //command line
        printf("Starting IC shell!\n");
        printf("******************************** ^u^ **********************************\n");
        printf("Welcome to IC Shell\n");
        while (1) {
            check_exit_children();
            printf("icsh $ ");
            fflush(stdout);

            //clear buffer
            memset(buffer, 0, MAX_CMD_BUFFER);

            if (fgets(buffer, MAX_CMD_BUFFER, stdin) == NULL) {
                if (errno == EINTR) {
                    clearerr(stdin);
                    continue;
                }
                break;
            }
            buffer[strcspn(buffer, "\n")] = '\0';
            exe_cmd(buffer, l_cmd, &exit_code, 0);
        }
    }
    return 0;
}
