/* ICCS227: Project 1: icsh
 * Name: Kritsadapon Wai-on
 * StudentID:
 */

#include "stdio.h"

#define MAX_CMD_BUFFER 255

int main() {
    
    printf("Starting IC shell!\n");
    printf("******************************** ^u^ **********************************\n");
    printf("Welcome to IC Shell\n");



    char buffer[MAX_CMD_BUFFER];
    while (1) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);
        printf("you said: %s\n", buffer);
    }
}
