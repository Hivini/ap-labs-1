/**
 * Author: Hivini
 * Name: Jorge Vinicio Quintero Santos
 * 
 * Notes:
 * The program returns 0 when an error happens to avoid any problems with the Makefile testing.
 * The string.h library is only to compare the strings, no need to reinvent all the wheels :)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLAG_ADD "-add"
#define FLAG_FIND "-find"

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Not enough arguments needed, please type -add or -find.\n");
        return 0;
    }
    char *flag = argv[1]; 
    if (!strcmp(flag, FLAG_ADD)) {
        if (argc != 4) {
            printf("Not enough arguments to add, need two strings.\n");
            return 0;
        }
        char *origin = argv[2];
        char *addition = argv[3];
        char *result = mystradd(origin, addition);
        printf("Initial Length\t\t: %d\n", mystrlen(origin));
        printf("New String\t\t: %s\n", result);
        printf("New Length\t\t: %d\n", mystrlen(result));
        free(result);
    } else if (!strcmp(flag, FLAG_FIND)) {
        if (argc != 4) {
            printf("Not enough arguments to find, need two strings.\n");
            return 0;
        }
        char *origin = argv[2];
        char *substr = argv[3];
        int position = mystrfind(origin, substr);
        if (position == -1) {
            printf("['%s'] string was not found.\n", substr); 
        } else {
            printf("['%s'] string was found at [%d] position.\n", substr, position); 
        }
    } else {
        printf("No proper flag given, please specify operation by adding -add or -find at the start.\n");
    }
    return 0;
}
