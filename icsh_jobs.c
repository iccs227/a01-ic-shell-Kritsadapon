#include "icsh_jobs.h"
#include "icsh_animation.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

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
            printf("[%d]%c  %d     %s\t\t%s & \n", i+1, (i == last) ? '+' : '-', // change here
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
            printf("[%d]  %d exit    %s\n", jid, terminated_pid, job_cmd[jid-1]);
            printf("icsh $ ");
            fflush(stdout);
            release_job(terminated_pid);
        }
    }
}

void mark_job_stopped(pid_t pid, char *cmdline) {
    int jid = get_jid(pid);
    if (jid > 0) {
        job_status[jid-1] = 1;
    }
}


void fg_job(int jid, int *exit_status) {
    if (jid <= 0 || jid > MAXJOBS || jobs[jid-1] == 0) {
        printf("fg: job %d not found\n", jid);
        return;
    }

    pid_t pid = jobs[jid-1];

    // If the job was stopped, resume job
    if (job_status[jid-1]) {
        if (kill(-pid, SIGCONT) < 0) {
            perror("kill (SIGCONT)");
            return;
        }
        job_status[jid-1] = 0;
    }

    char *cmdline = job_cmd[jid-1];
    printf("%s\n", cmdline);

    //check if sleep
    int is_sleep = (strncmp(cmdline, "sleep", 5) == 0);

    if (is_sleep) {
        start_kitten_animation(cmdline);
    }

    fg_pgid = pid;
    // Set the foreground process group
    tcsetpgrp(STDIN_FILENO, pid );

    int status;
    //wait for process to finished/end
    waitpid(pid, &status, WUNTRACED);

    if (is_sleep) {
        stop_kitten_animation();
    }

    fg_pgid =0;
    tcsetpgrp(STDIN_FILENO, getpgid(0));

    if (WIFEXITED(status)) {
        *exit_status = WEXITSTATUS(status);
        release_job(pid);
    } else if (WIFSIGNALED(status)) {
        *exit_status = WTERMSIG(status);
        release_job(pid);
    } else if (WIFSTOPPED(status)) {
        *exit_status = WSTOPSIG(status);
        mark_job_stopped(pid, cmdline);
        printf("\n[%d]  + %d suspended  %s\n", jid , pid, cmdline);
    }
}

void bg_job(int jid) {
    if (jid <= 0 || jid > MAXJOBS || jobs[jid-1] == 0) {
        printf("fg: job %d not found\n", jid);
        return;
    }

    pid_t pid = jobs[jid-1];
    char *cmdline = job_cmd[jid-1];

    //check status if running
    if (!job_status[jid-1]) {
        printf("bg: job %d already running\n", jid);
        return;
    }


    if (kill(-pid, SIGCONT) < 0) {
        perror("kill_BG (SIGCONT)");
        return;
    }

    // change child status
    job_status[jid-1] = 0;
    printf("[%d] %d %s &\n", jid, pid, cmdline);
}

