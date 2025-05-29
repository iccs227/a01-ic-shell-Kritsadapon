#include "icsh_jobs.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t jobs[MAXJOBS] = {0};
char job_status[MAXJOBS] = {0};
char job_cmd[MAXJOBS][MAXCMD] = {{0}};
int job_count = 0;


void save_job(pid_t pid, char *cmdline, char stopped) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i] == 0) {
            jobs[i] = pid;
            strncpy(job_cmd[i], cmdline, MAXCMD-1);
            job_cmd[i][MAXCMD-1] = '\0';
            job_status[i] = stopped;
            job_count++;
            if (!stopped) {
                printf("[%d] %d\n", i+1, pid);
            }
            return;
        }
    }
    fprintf(stderr, "Reached job limit: %d. Can't save job\n", MAXJOBS);
    kill(pid, SIGINT);
    waitpid(pid, NULL, 0);
}

void release_job(pid_t pid) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i] == pid) {
            jobs[i] = 0;
            job_status[i] = 0;
            job_cmd[i][0] = '\0';
            job_count--;
            return;
        }
    }
}

//getting job id from pid
int get_jid(pid_t pid) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i] == pid)
            return i+1;
    }
    return -1;
}

void print_jobs() {
    if (job_count == 0) {
        printf("No jobs running.\n");
        return;
    }
    int last = -1;
    for (int i = 0; i < MAXJOBS; i++)
        if (jobs[i] != 0)
            last = i;
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i] != 0) {
            printf("[%d]%c  %d %s\t\t%s & \n", i+1, (i == last) ? '+' : '-', // change here
                jobs[i], job_status[i] ? "Stopped" : "running", job_cmd[i]);
        }
    }
    printf("\n");
}

void check_exit_children() {
    int status;
    pid_t terminated_pid;
    while ((terminated_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        int jid = get_jid(terminated_pid);
        if (jid > 0) {
            printf("\n");
            printf("[%d]  %d done     %s\n", jid, terminated_pid, job_cmd[jid-1]);
            printf("icsh $ ");
            fflush(stdout);
            release_job(terminated_pid);
        }
    }
}

