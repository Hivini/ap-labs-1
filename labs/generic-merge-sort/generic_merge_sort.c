#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LINES 8192
#define MAX_LINE_SIZE 1024

int numcmp(const char *, const char *);
void readLines(char *fileName, char *lineptr[], int *nLines);
void writeLines(char *lineptr[], int nLines);
void mergeSort(char *lineptr[], int start, int end, int (*comp)(const char *, const char *));
void merge(char *lineptr[], int start, int mid, int end, int (*comp)(const char *, const char *));
void swap(char *lineptr[], int i, int y);

int main(int argc, char *argv[])
{
    int nLines = 0, numeric = 0;
    char *lineptr[MAX_LINES];
    char *fileName;

    if (argc < 2) {
        printf("Input too short, need to define at least input file argument.\n");
        return 0;
    } else {
        if (argc == 3) {
            if (strcmp(argv[1], "-n") == 0) {
                numeric = 1;
            } else {
                printf("Invalid parameter, -n expected got: '%s'\n", argv[1]);
                return 0;
            }
            fileName = argv[2];
        } else {
            if (argc == 2) {
                if (strcmp(argv[1], "-n") == 0) {
                    printf("No input file argument provided. Example ./program -n file.txt\n");
                    return 0;
                }
            }
            fileName = argv[1];
        }
        readLines(fileName, lineptr, &nLines);
        if (numeric) {
            mergeSort(lineptr, 0, nLines - 1, numcmp);
        } else {
            mergeSort(lineptr, 0, nLines - 1, strcmp);
        }
        if (nLines > 0) {
            for (int i = 0; i < nLines; i++) {
                printf("- %s\n", lineptr[i]);
            }
            printf("Number of lines: %d\n", nLines);
        }
    }
    return 0;
}

void readLines(char *fileName, char *lineptr[], int *nLines) {
    int fileFd, rline, linePos;
    char c;
    char *line;

    fileFd = open(fileName, O_RDONLY);
    if (fileFd == -1) {
        printf("Error opening file: '%s'\n", fileName);
        return;
    }
    line = malloc(MAX_LINE_SIZE);
    linePos = 0;
    while ((rline = read(fileFd, &c, 1)) != 0) {
        if (rline == -1) {
            printf("Error reading file: [%s]\n", fileName);
            return;
        }
        // Two empty spaces are needed for \n and \0.
        if (linePos == MAX_LINE_SIZE - 2) {
            printf("Max line size reached, please change default value in source code. Current MAX SIZE: [%d]\n", MAX_LINE_SIZE);
            return;
        }
        if (c == '\n') {
            line[linePos + 1] = '\0';
            lineptr[*nLines] = line;
            line = malloc(MAX_LINE_SIZE);
            *nLines += 1;
            linePos = 0;
        } else {
            line[linePos] = c;
            linePos++;
        }
    }
    close(fileFd);
}

int numcmp(const char *s1, const char *s2) {
    double v1, v2;

    v1 = atof(s1);
    v2 = atof(s2);
    if (v1 < v2) {
        return -1;
    } else if (v1 > v2) {
        return 1;
    } else {
        return 0;
    }
}

void mergeSort(char *lineptr[], int start, int end, int (*comp)(const char *, const char *)) {
    if (start < end) {
        int mid = (start + end) / 2;
        mergeSort(lineptr, start, mid, comp);
        mergeSort(lineptr, mid + 1, end, comp);
        merge(lineptr, start, mid, end, comp);
    }
}

void merge(char *lineptr[], int start, int mid, int end, int (*comp)(const char *, const char *)) {
    char *tmp[end - start + 1];
    int i = start, j = mid + 1, k = 0;

    while (i <= mid && j <= end) {
        if ((*comp)(lineptr[i], lineptr[j]) <= 0) {
            tmp[k] = lineptr[i];
            k++;
            i++;
        } else {
            tmp[k] = lineptr[j];
            k++;
            j++;
        }
    }

    // Add elements of the first section.
    while (i <= mid) {
        tmp[k] = lineptr[i];
        k++;
        i++;
    }

    // Add elements of the second section.
    while (j <= end) {
        tmp[k] = lineptr[j];
        k++;
        j++;
    }

    // Copy elements.
    for (i = start; i <= end; i++) {
        lineptr[i] = tmp[i - start];
    }
}

void swap(char *lineptr[], int i, int y) {
    char *tmp;

    tmp = lineptr[i];
    lineptr[i] = lineptr[y];
    lineptr[y] = tmp;
}
