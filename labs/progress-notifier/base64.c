#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "logger.h"
#include "base64implementation.h"

#define BUFFER_SIZE 512

unsigned long int size = 0, currentChar = 0;

int getFileProperties(char *fileName, size_t fileSize, char *name, char *extension);

void signalProgress(int sig) {
    infof("Current progress: %.2f%\n", ((currentChar * 1.0f) / size) * 100);
}

int main(int argc, char **argv){
    FILE *fp, *destfp;
    long int resultSize;
    int isEnconding, err;
    char *fileName, *fileExtension, *destFileName, *result, *str;
    size_t out;

    initLogger(""); // Set to STDOUT.

    if (argc < 3) {
        errorf("Not enough arguments provided, got %d. Example usage: ./base64 --encode file.txt", argc - 1);
        return 0;
    }

    if (strcmp(argv[1], "--encode") == 0) {
        infof("Encoding...");
        isEnconding = 1;
    } else if (strcmp(argv[1], "--decode") == 0) {
        infof("Decoding...");
        isEnconding = 0;
    } else {
        errorf("Not a valid argument provided. Please select operation with --encode or --decode, got: %s", argv[1]);
        return 0;
    }

    if (signal(SIGINT, signalProgress) == SIG_ERR) {
        errorf("SIGINT signal couldn't be set.");
    }

    if (signal(SIGUSR1, signalProgress) == SIG_ERR) {
        errorf("SIGUSR1 signal couldn't be set.");
    }

    /* Open the file where data is obtained from. */
    fp = fopen(argv[2], "r");
    if(fp == NULL) {
        errorf("Error opening file [%s]", argv[2]);
        return(-1);
    }

    fileName = (char *) malloc(strlen(argv[2]));
    fileExtension = (char *) malloc(strlen(argv[2]));

    err = getFileProperties(argv[2], strlen(argv[2]), fileName, fileExtension);
    if (err == -1) {
        errorf("The file must have an extension, got: %d", argv[2]);
    }

    fseek(fp, 0, SEEK_END); // Seek to end of file
    size = ftell(fp); // Get file pointer
    fseek(fp, 0, SEEK_SET);
    infof("Size of file: %ld bytes\n", size);

    // Get the size needed for the base64 characters.
    resultSize = 6 * (BUFFER_SIZE / 3) + 1;
    if (isEnconding) {
        // Destination file name has the size of the name + extension + added name + dot + \0.
        destFileName = (char *) malloc(strlen(fileName) + strlen(fileExtension) + strlen("-encoded") + 2);
        result = (char *) malloc(resultSize);
        str = (char *) malloc(BUFFER_SIZE);

        sprintf(destFileName, "%s-encoded.%s", fileName, fileExtension);
        destfp = fopen(destFileName, "w+");
        while (fgets (str, sizeof(str), fp) != NULL) {
            err = base64encode(str, strlen(str), result, resultSize);
            if (err != 0) {
                errorf("Buffer too small for enconding.");
                return 0;
            }
            fprintf(destfp, "%s\n", result);
            currentChar = ftell(fp);
        }
        infof("ENCODING DONE");
        free(result);
        free(str);
        free(destFileName);
        fclose(destfp);
    } else {
        // Destination file name has the size of the name + extension + added name + dot + \0.
        destFileName = (char *) malloc(strlen(fileName) + strlen(fileExtension) + strlen("-decoded") + 2);
        str = (char *) malloc (resultSize);
        result = (char *) malloc(resultSize);
        sprintf(destFileName, "%s-decoded.%s", fileName, fileExtension);
        destfp = fopen(destFileName, "w+");
        while (fscanf(fp, "%[^\n]\n", str) != EOF) {
            out = resultSize;
            err = b64_decode(str, result, &out);
            if (err != 1) {
                errorf("Decode error, contact admin.");
                return 0;
            }
            result[out] = '\0';
            fprintf(destfp, "%s", result);
            currentChar = ftell(fp);
        }
        infof("DECODING DONE");
        free(result);
        free(str);
    }
    free(fileName);
    free(fileExtension);
    fclose(fp);
    
    return(0);
}

int getFileProperties(char *fileName, size_t fileSize, char *name, char *extension) {
    int dotIndex = -1;
    for (int i = 0; i < fileSize; i++) {
        if (fileName[i] == '.') {
            dotIndex = i;
            strncpy(name, fileName, i);
            name[i] = '\0';
            strncpy(extension, fileName + i + 1, fileSize - i - 1);
            extension[fileSize-i-1] = '\0';
            break;
        }
    }
    return dotIndex;
}
