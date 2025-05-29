#ifndef ICSH_ANIMATION_H
#define ICSH_ANIMATION_H

#include <sys/time.h>

#define MAX_CMD_BUFFER 1024

// Animation state variables
extern int show_cat_animation;
extern int animation_frame;
extern char current_animated_cmd[MAX_CMD_BUFFER];
extern const char *running_cat_frames[];

// Animation control functions
void start_kitten_animation(const char *cmdline);
void stop_kitten_animation(void);
void timer_handler(int sig);

#endif // ICSH_ANIMATION_H 