/**
 * Author: Hivini
 * Name: Jorge Vinicio Quintero Santos
 */
#include <stdio.h>
#include <stdlib.h>

#define END_STRING '\0'

int mystrlen(char *str){
    char *c = NULL;
    int length = 0;

    // Iterate through each character in the pointer.
    for (c = str; *c != END_STRING; c++) {
        length++;
    }
    return length;
}

char *mystradd(char *origin, char *addition){
    int lorigin, laddition, size, i;
    char *result = NULL;
    lorigin = mystrlen(origin);
    laddition = mystrlen(addition);
    size = lorigin + laddition;
    // One character is added for the end of string.
    result = (char *) malloc(size + 1);

    // Copy both of the strings to the result array.
    for (i = 0; i < size; i++) {
        if (i < lorigin) {
            result[i] = origin[i];
        } else {
            result[i] = addition[i - lorigin];
        }
    }
    result[i] = END_STRING;
    return result;
}

int mystrfind(char *origin, char *substr){
    int lorigin, lsubstr, i, psub, isub;
    char *c = NULL;
    lorigin = mystrlen(origin);
    lsubstr = mystrlen(substr);
    if (lorigin < lsubstr) {
        return 0;
    }

    // psub - Position on the substring.
    // isub - Position where the substring started appearing.
    psub = isub = 0;
    for (i = 0; i < lorigin; i++) {
        if (origin[i] == substr[psub]) {
            if (psub == 0) {
                isub = i;
            }
            psub++;
        } else {
            psub = 0;
        }
        if (psub == lsubstr) {
            return isub;
        }
    }
    return -1;
}
