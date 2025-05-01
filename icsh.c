/* ICCS227: Project 1: icsh
 * Name: Kritsadapon Wai-on
 * StudentID:
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define MAX_CMD_BUFFER 255

int main() {
    
    printf("Starting IC shell!\n");
    printf("******************************** ^u^ **********************************\n");
    printf("Welcome to IC Shell\n");



    char buffer[MAX_CMD_BUFFER];


    
    while (1) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);

        buffer[strcspn(buffer, "\n")] = '\0';



        
        if (strcmp(buffer, "exit") == 0) {
            printf("Exiting IC shell...\n");
            break;
        } else if (strcmp(buffer, "clear") == 0) {
            system("clear");
        } else if (strncmp(buffer, "echo ", 5) == 0) {
            printf("%s\n", buffer + 5);
        }
    }
}
