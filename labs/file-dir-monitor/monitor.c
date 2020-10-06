#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <ftw.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <signal.h>
#include "logger.h"

#define BUF_LEN (512 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define FLAGS (IN_CREATE | IN_DELETE | IN_MOVE | IN_ISDIR)
#define MAX_SUBDIRECTORIES 2048
#define MAX_CHARS 512
#define NFTW_DIR 16

static char subdirs[MAX_SUBDIRECTORIES][MAX_CHARS];
static char renamed[MAX_CHARS];
static int pastCookie = -1;
static int counter = 0;
static int loop = 1;

static void logMonitor(const char *operation, char *name, char *renamed, char *dir, int isDir);

static void logMonitor(const char *operation, char *name, char *renamed, char *dir, int isDir) {
    if (strcmp(renamed, "") != 0) {
        if (strcmp(dir, "") != 0) {
            infof("[%s - %s] - %s/%s -> %s/%s", isDir ? "Directory" : "File", operation, dir, name, dir, renamed);
        } else {
            infof("[%s - %s] - %s -> %s", isDir ? "Directory" : "File", operation, name, renamed);
        }
    } else {
        if (strcmp(dir, "") != 0) {
            infof("[%s - %s] - %s/%s", isDir ? "Directory" : "File", operation, dir, name);
        } else {
            infof("[%s - %s] - %s", isDir ? "Directory" : "File", operation, name);
        }
    }
}

static int update_subidrs(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
    if (tflag == FTW_D) {
        if (strcmp(".", fpath) != 0) {
            if (counter == MAX_SUBDIRECTORIES) {
                panicf("MAX SUBFOLDERS REACHED!");
            } else {
                strcpy(subdirs[counter], fpath);
                counter++;
            }
            return FTW_SKIP_SUBTREE;
        }
    }
    return FTW_CONTINUE;
}

static int processEvent(struct inotify_event *i, int currentWatchers[], char *path, int flags, int notifierFd) {
    int y, isDir, wd;

    isDir = i->mask & IN_ISDIR;
    if (isDir) {
        counter = 0;
        if (nftw(path, update_subidrs, NFTW_DIR, flags) == -1) {
            errorf("NFTW failed on directory %s", path);
            return -1;
        }

        wd = inotify_add_watch(notifierFd, path, FLAGS);
        if (wd == -1) {
            errorf("Failed to watch directory");
            return -1;
        }

        currentWatchers[0] = wd;

        /* Assign a watch to every subdir */
        for (int i = 0; i < counter; i++) {
            wd = inotify_add_watch(notifierFd, subdirs[i], FLAGS);
            if (wd == -1) {
                errorf("Failed to watch directory");
                return -1;
            }
            currentWatchers[i + 1] = wd;
            printf("'%d', ", currentWatchers[i + 1]);
        }
    }
    for (y = 0; y <= counter; y++) {
        if (currentWatchers[y] == i->wd) {
            break;
        }
        if (y == counter) {
            y = 0;
            break;
        }
    }

    if (i->mask & IN_CREATE) logMonitor("Create", i->name, "", y > 0 ? subdirs[y-1] : "", isDir);
    if (i->mask & IN_DELETE) logMonitor("Removal", i->name, "", y > 0 ? subdirs[y-1] : "", isDir);
    if (i->mask & IN_MOVED_FROM) {
        if (i->cookie > 0) {
            pastCookie = i->cookie;
            strcpy(renamed, i->name);
        }
    }
    if (i->mask & IN_MOVED_TO) {
        if (i->cookie > 0 && pastCookie == i->cookie) {
            logMonitor("Rename", renamed, i->name, subdirs[y-1], isDir);
        }
    }
    return 0;
}

static void signalHandler(int sign) {
    _exit(0);
}

int main(int argc, char *argv[]) {
    int notifierFd, wd, err;
    int currentWatchers[MAX_SUBDIRECTORIES + 1];
    char buffer[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;
    int flags = FTW_ACTIONRETVAL;

    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        errorf("Failed to set the exit signal.");
        errorf("%s", strerror(errno));
        return 0;
    }

    initLogger("syslog");               /* Initialize logger to syslog */

    if (argc < 2) {
        errorf("Pathname not specified.");
        return 0;
    }

    infof("Starting File/Directory Monitor on %s", argv[1]);
    infof("-------------------------------------");

    notifierFd = inotify_init();        /* Creating monitor instance */

    if (notifierFd == -1) {
        panicf("Couldn't start notifier.");
        return 0;
    }

    if (nftw(argv[1], update_subidrs, NFTW_DIR, flags) == -1) {
        errorf("NFTW failed on directory %s", argv[1]);
    }

    wd = inotify_add_watch(notifierFd, argv[1], FLAGS);
    if (wd == -1) {
        errorf("Failed to watch directory");
        return 0;
    }

    currentWatchers[0] = wd;

    /* Assign a watch to every subdir */
    for (int i = 0; i < counter; i++) {
        wd = inotify_add_watch(notifierFd, subdirs[i], FLAGS);
        if (wd == -1) {
            errorf("Failed to watch directory");
            return 0;
        }
        currentWatchers[i + 1] = wd;
    }

    while(loop) {                                  /* Read events forever */
        numRead = read(notifierFd, buffer, BUF_LEN);
        if (numRead == 0) {
            errorf("Notifier failure");
            return 0;
        }

        if (numRead == -1) {
            errorf("Failed to read");
            return 0;
        }

        for (p = buffer; p < buffer + numRead;) {
            event = (struct inotify_event *) p;
            err = processEvent(event, currentWatchers, argv[1], flags, notifierFd);
            if (err == -1) {
                return err;
            }
            p += sizeof(struct inotify_event) + event->len;
        }
    }
    return 0;
}