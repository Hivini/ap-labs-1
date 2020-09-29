#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "logger.h"

#define INFO "INFO"
#define WARN "WARN"
#define ERROR "ERROR"
#define PANIC "PANIC"

#define RESET 0
#define BRIGHT 1

#define BLACK 0
#define RED 1
#define YELLOW  3
#define BLUE  4
#define MAGENTA 5

void resetTerminalColor();
void setTerminalColor(int attr, int b);
void formatString(char *s, const char *format, int ls, char *sf, int lf);

int infof(const char *format, ...) {
    va_list arg;
    size_t l, lInfo;
    char *s;
    int done, i;

    lInfo = strlen(INFO) + 2;
    l = strlen(format);
    s = (char *) malloc(l + 1 + lInfo);
    formatString(s, format, l, INFO, lInfo);

    va_start(arg, format);
    setTerminalColor(RESET, BLUE);
    done = vfprintf(stdout, s, arg);
    free(s);
    va_end(arg);
    resetTerminalColor();
    return done;
}

int warnf(const char *format, ...) {
    va_list arg;
    size_t l, lInfo;
    char *s;
    int done, i;

    lInfo = strlen(WARN) + 2;
    l = strlen(format);
    s = (char *) malloc(l + 1 + lInfo);
    formatString(s, format, l, WARN, lInfo);

    va_start(arg, format);
    setTerminalColor(RESET, YELLOW);
    done = vfprintf(stdout, s, arg);
    free(s);
    va_end(arg);
    resetTerminalColor();
    return done;
}

int errorf(const char *format, ...) {
    va_list arg;
    size_t l, lInfo;
    char *s;
    int done, i;

    lInfo = strlen(ERROR) + 2;
    l = strlen(format);
    s = (char *) malloc(l + 1 + lInfo);
    formatString(s, format, l, ERROR, lInfo);

    va_start(arg, format);
    setTerminalColor(RESET, RED);
    done = vfprintf(stdout, s, arg);
    free(s);
    va_end(arg);
    resetTerminalColor();
    return done;
}

int panicf(const char *format, ...) {
    va_list arg;
    size_t l, lInfo;
    char *s;
    int done, i;

    lInfo = strlen(PANIC) + 2;
    l = strlen(format);
    s = (char *) malloc(l + 1 + lInfo);
    formatString(s, format, l, PANIC, lInfo);

    va_start(arg, format);
    setTerminalColor(RESET, MAGENTA);
    done = vfprintf(stdout, s, arg);
    free(s);
    va_end(arg);
    resetTerminalColor();
    return done;
}

void formatString(char *s, const char *format, int ls, char *sf, int lf) {
    int i;

    for (i = 0; i < lf-1; i++) {
        s[i] = sf[i];
    }
    s[lf-2] = ':';
    s[lf-1] = ' ';
    for (i = lf; i < ls + lf; i++) {
        s[i] = format[i-lf];
    }
    s[ls+lf] = '\n';
}

void setTerminalColor(int attr, int t) {
    char command[13];
    sprintf(command, "%c[%d;%d;1m", 0x1B, attr, t + 30);
    printf("%s", command);
}

void resetTerminalColor() {
    char command[13];
    sprintf(command, "%c[%d;0;0m", 0x1B, RESET);
    printf("%s", command);
}