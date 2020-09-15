#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE_SIZE 8192
#define MAX_PACKAGES 4096
#define MAX_WORD_SIZE 64
#define DATE_SIZE 32
#define REMOVED 3
#define UPGRADED 2
#define INSTALLED 1

typedef struct Package {
    char name[MAX_WORD_SIZE + 1], dateInstall[DATE_SIZE + 1], dateUpdate[DATE_SIZE + 1], dateRemoved[DATE_SIZE + 1];
    int updates;
} Package;

typedef struct Report {
    int installedPkg, removedPkg, upgradedPkg, currentInstalled, numPkg;
    Package *packages[MAX_PACKAGES];
} Report;

void analizeLog(char *logFile, char *reportFile);
void writeAndPrint(char *line, int reportFd);
void writeToFile(char *line, int reportFd);
void processLine(char *line, Report *report);
int jumpWhitespace(char *line, int i);
int jumpToNextWord(char *line, int i);

int main(int argc, char **argv) {
    char *inputFile, *reportFile;
    if (argc < 5) {
	    printf("Usage:./pacman-analizer.o -input <filename> -report <filename> \n");
	    return 1;
    }
    if (strcmp(argv[1], "-input") == 0 && strcmp(argv[3], "-report") == 0) {
        inputFile = argv[2];
        reportFile = argv[4];
    } else if (strcmp(argv[1], "-report") == 0 && strcmp(argv[3], "-input") == 0) {
        inputFile = argv[4];
        reportFile = argv[2];
    } else {
        printf("Invalid flag: -input & -report must be present\n");
        return 2;
    }

    analizeLog(inputFile, reportFile);

    return 0;
}

void analizeLog(char *logFile, char *reportFile) {
    int logFileFd, reportFd, numRead, linePos, i;
    char line[MAX_LINE_SIZE];
    char *reportLine;
    char c;
    Report *report = malloc(sizeof(Report));
    report->installedPkg = 0;
    report->currentInstalled = 0;
    report->upgradedPkg = 0;
    report->removedPkg = 0;
    report->numPkg = 0;
    // Implement your solution here.
    logFileFd = open(logFile, O_RDONLY);
    if (logFileFd == -1) {
        printf("Error opening file: [%s]\n", logFile);
        return;
    }
    reportFd = open(reportFile, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (reportFd == -1) {
        printf("Error creating report file at: [%s]\n", reportFile);
        return;
    }

    printf("Generating Report from: [%s] log file\n", logFile);
    linePos = 0;
    while((numRead = read(logFileFd, &c, 1 )) != 0) {
        if (numRead == -1) {
            printf("Error reading file: [%s]\n", logFile);
            return;
        }
        // Two empty spaces are needed for \n and \0.
        if (linePos == MAX_LINE_SIZE - 2) {
            printf("Max line size reached, please change default value in source code. Current MAX SIZE: [%d]\n", MAX_LINE_SIZE);
            return;
        }

        if (c == '\n') {
            line[linePos] = c;
            line[linePos + 1] = '\0';
            processLine(line, report);
            linePos = 0;
        } else {
            line[linePos] = c;
            linePos++;
        }
    }
    // Get the final line.
    if (linePos != 0) {
        line[linePos] = '\0';
        processLine(line, report);
    }

    writeAndPrint("Pacman Packages Report\n", reportFd);
    writeAndPrint("-----------------------\n", reportFd);
    sprintf(line, "- Installed packages\t:%d\n", report->installedPkg);
    writeAndPrint(line, reportFd);
    sprintf(line, "- Removed packages\t:%d\n", report->removedPkg);
    writeAndPrint(line, reportFd);
    sprintf(line, "- Upgraded packages\t:%d\n", report->upgradedPkg);
    writeAndPrint(line, reportFd);
    sprintf(line, "- Current installed\t:%d\n", report->currentInstalled);
    writeAndPrint(line, reportFd);

    writeToFile("\nList of packages\n", reportFd);
    writeToFile("----------------\n", reportFd);
    if (report->numPkg == 0) {
        writeToFile("No packages found!\n", reportFd);
    } else {
        for (i = 0; i < report->numPkg; i++) {
            Package *package = report->packages[i];
            sprintf(line, "- Package name\t: %s\n", package->name);
            writeToFile(line, reportFd);
            sprintf(line, " - Install date\t: %s\n", package->dateInstall);
            writeToFile(line, reportFd);
            sprintf(line, " - Last update date\t: %s\n", package->dateUpdate);
            writeToFile(line, reportFd);
            sprintf(line, " - How many updates\t: %d\n", package->updates);
            writeToFile(line, reportFd);
            sprintf(line, " - Removal date\t: %s\n", package->dateRemoved);
            writeToFile(line, reportFd);
        }
    }
    
    printf("Report is generated at: [%s]\n", reportFile);
    close(logFileFd);
    close(reportFd);
    free(report);
}

void processLine(char *line, Report *report) {
    int pos, i, op, foundPackage, size;
    // We add to the MAX_SIZE an extra one for the end of string symbol.
    char date[DATE_SIZE + 1], operation[MAX_WORD_SIZE + 1], packageName[MAX_WORD_SIZE + 1];
    Package *package;
    size = strlen(line);
    // Ignore empty lines.
    if (size == 0) {
        return;
    }
    if (line[0] == '\n') {
        return;
    }
    pos = 0;
    pos = jumpWhitespace(line, pos);
    // First data is the date.
    if (line[pos] == '[') {
        pos++;
        i = 0;
        while (line[pos] != ']') {
            if (pos == size) {
                printf("Character ] not found on date on line: \n\t%s\n", line);
                return;
            }
            if (i == DATE_SIZE) {
                printf("Invalid date format on line: \n\t%s", line);
                return;
            }
            date[i++] = line[pos];
            pos++;
        }
        date[i] = '\0';
        pos++;
    } else {
        return;
    }
    // Jump the name of the program that modifies the package, example: [ALPM].
    pos = jumpToNextWord(line, pos);
    if (pos == -1) {
        printf("Invalid line: \n\t%s", line);
        return;
    }
    // Get the package operation.
    i = 0;
    while (line[pos] != ' ' && line[pos] != '\n') {
        if (line[pos] == EOF) {
            return;
        }
        if (i == MAX_WORD_SIZE) {
            printf("Max word size reached on operation, something might be wrong in the file, please verify format.\n");
            return;
        }
        operation[i++] = line[pos];
        pos++;
    }
    operation[i] = '\0';
    if (strcmp(operation, "upgraded") == 0) {
        op = UPGRADED;
    } else if (strcmp(operation, "removed") == 0) {
        op = REMOVED;
    } else if (strcmp(operation, "installed") == 0) {
        op = INSTALLED;
    } else {
        return;
    }
    pos = jumpWhitespace(line, pos);
    // Get the package name.
    i = 0;
    while(line[pos] != ' ' && line[pos] != '\n') {
        if (line[pos] == EOF) {
            return;
        }
        if (i == MAX_WORD_SIZE) {
            printf("Max word size reached for package name.\n");
            return;
        }
        packageName[i++] = line[pos];
        pos++;
    }
    packageName[i] = '\0';

    // Ignore the rest of the line and add to report.
    if (op == INSTALLED) {
        report->installedPkg++;
        report->currentInstalled++;
        i = 0;
        foundPackage = 0;
        while (i < report->numPkg) {
            // If already on the record reset the values.
            package = report->packages[i];
            if (strcmp(packageName, package->name) == 0) {
                strncpy(package->dateInstall, date, strlen(date));
                strncpy(package->dateRemoved, "-\0", 2);
                strncpy(package->dateUpdate, "-\0", 2);
                package->updates = 0;
                foundPackage = 1;
                break;
            }
            i++;
        }
        package = malloc(sizeof(Package));
        if (!foundPackage) {
            strncpy(package->name, packageName, strlen(packageName) + 1);
            strncpy(package->dateInstall, date, strlen(date));
            strncpy(package->dateRemoved, "-\0", 2);
            strncpy(package->dateUpdate, "-\0", 2);
            package->updates = 0;
            report->packages[report->numPkg] = package;
            report->numPkg++;
        }
    } else if (op == UPGRADED) {
        report->upgradedPkg++;
        i = 0;
        while (i < report->numPkg) {
            package = report->packages[i];
            if (strcmp(packageName, package->name) == 0) {
                strncpy(package->dateUpdate, date, strlen(date));
                package->updates++;
                foundPackage = 1;
                break;
            }
            i++;
        }
        if (!foundPackage) {
            printf("Package '%s' was upgraded but not found on installed records.\n", packageName);
        }
    } else if (op == REMOVED) {
        report->currentInstalled--;
        report->removedPkg++;
        i = 0;
        while (i < report->numPkg) {
            package = report->packages[i];
            if (strcmp(packageName, package->name) == 0) {
                strncpy(package->dateRemoved, date, strlen(date));
                foundPackage = 1;
                break;
            }
            i++;
        }
        if (!foundPackage) {
            printf("Package '%s' removed but not found on installed records.\n", packageName);
        }
    }
}

int jumpWhitespace(char *line, int i) {
    while(line[i] == ' ') {
        i++;
    }
    return i;
}

int jumpToNextWord(char *line, int i) {
    i = jumpWhitespace(line, i);
    while(line[i] != ' ') {
        i++;
        if (i == strlen(line)) {
            return -1;
        }
    }
    return jumpWhitespace(line, i);
}

void writeAndPrint(char *line, int reportFd) {
    write(reportFd, line, strlen(line));
    printf("%s", line);
}

void writeToFile(char *line, int reportFd) {
    write(reportFd, line, strlen(line));
}