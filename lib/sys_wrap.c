/**
 * This file contains wrappers, which usually start with an `s_' in their names, to some commonly used system calls.
 */

#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mount.h>

#define CHECK_AND_EXIT_IF_REQUIRED(b, r, s) \
    if ((b) && (r)) {                       \
        perror((s));                        \
        exit(1);                            \
    }

int s_open(char *path, int mode, bool exit_on_error) {
    int fd = open(path, mode);
    CHECK_AND_EXIT_IF_REQUIRED(exit_on_error, fd == -1, "open")
    return fd;
}

int s_write(int fd, void *buf, size_t len, bool exit_on_error) {
    int result = write(fd, buf, len);
    CHECK_AND_EXIT_IF_REQUIRED(exit_on_error, result == -1, "write");
    return result;
}

long s_pivot_root(const char *new_root, const char *old_root, bool exit_on_error) {
    long result = syscall(SYS_pivot_root, new_root, old_root);
    CHECK_AND_EXIT_IF_REQUIRED(exit_on_error, result == -1, "pivot_root");
    return result;
}

__attribute__((unused)) int s_ls(const char *dir, bool exit_on_error) {
    struct dirent *d;
    DIR *dh = opendir(dir);
    CHECK_AND_EXIT_IF_REQUIRED(exit_on_error, dh == NULL, "opendir");
    while ((d = readdir(dh)) != NULL) {
        printf("%s  ", d->d_name);
        printf("\n");
    }
    closedir(dh);
}

int s_umount(const char *path, int flags, bool exit_on_error) {
    int result = umount2(path, flags);
    CHECK_AND_EXIT_IF_REQUIRED(exit_on_error, result == -1, "umount2");
    return result;
}

int s_system(const char *cmd, const char *debug_msg, bool exit_on_error) {
    int result = system(cmd);
    if (result) {
        if (debug_msg) fprintf(stderr, debug_msg);
        if (exit_on_error) exit(1);
    }
    return result;
}
