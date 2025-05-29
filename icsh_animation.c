#include "icsh_animation.h"
#include <string.h>
#include <stdio.h>
#include <signal.h>

// ASCII art cat frames with moving butterfly
const char *running_cat_frames[] = {
    "                              ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                               ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                                ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                                 ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                                  ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                                   ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                                    ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                                     ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                                      ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
    "                                       ᯓᡣ𐭩\n                /ᐠ- ˕ -マ   ノ\n             乀( J   し )\n",
};

// Animation state variables
int show_cat_animation = 0;
int animation_frame = 0;
char current_animated_cmd[MAX_CMD_BUFFER] = "";

// Timer handler for running cat animation
void timer_handler(int sig) {
    if (show_cat_animation) {
        // Clear screen
        printf("\033[2J\033[H");
        printf("%s", running_cat_frames[animation_frame]);
        printf("\n$ %s\n", current_animated_cmd);
        animation_frame = (animation_frame + 1) % 10;
        fflush(stdout);
    }
}

void start_kitten_animation(const char *cmdline) {
    if (show_cat_animation) {
        strncpy(current_animated_cmd, cmdline, MAX_CMD_BUFFER-1);
        current_animated_cmd[MAX_CMD_BUFFER-1] = '\0';
        struct itimerval timer;
        //send signal after 500ms
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 500000;
        //sending signal every 500ms
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 250000;
        //sending SIGALRM to Handler
        setitimer(ITIMER_REAL, &timer, NULL);
    }
}

void stop_kitten_animation(void) {
    if (show_cat_animation) {
        // Stop the timer
        struct itimerval timer;
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = 0;
        timer.it_interval.tv_sec = 0;
        timer.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &timer, NULL);
        current_animated_cmd[0] = '\0';
    }
} 