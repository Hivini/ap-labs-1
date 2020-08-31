#include <stdio.h>

#define BUFFER_SIZE 2048
#define STOP_KEY '\n'

/** 
 * Author: hivini
 * 
 * Reverse a word and print it everytime you hit enter. 
 */
int main(){
    int c, i;
    char buffer[BUFFER_SIZE];
    i = BUFFER_SIZE - 1;
    while ((c = getchar()) != EOF) {
        if (c == STOP_KEY) {
            // Read the array from where the pointer of the value stops.
            for (int y = i + 1; y < BUFFER_SIZE; y++) {
                printf("%c", buffer[y]);
            }
            printf("\n");
            // Return the pointer to the start.
            i = BUFFER_SIZE - 1;
        } else {
            if (i < 0) {
                printf("There is no space on the buffer anymore, max size is %d.\n", BUFFER_SIZE);
                return 1;
            }
            // Store the character in the array from end to start.
            buffer[i] = c;
            i--;
        }
    }
    return 0;
}
