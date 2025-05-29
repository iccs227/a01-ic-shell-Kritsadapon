#ifndef ICSH_JOBS_H
#define ICSH_JOBS_H

#define MAXJOBS 16
#define MAXCMD  128

#include <sys/types.h>

extern pid_t jobs[MAXJOBS];
extern char job_status[MAXJOBS];
extern char job_cmd[MAXJOBS][MAXCMD];
extern int job_count;
extern pid_t fg_pgid;

void save_job(pid_t pid, char *cmdline, char stopped);
void release_job(pid_t pid);
int get_jid(pid_t pid);
void print_jobs();
void check_exit_children();

#endif