#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../include/errno.h"
#include "../include/fork.h"
#include "../include/id.h"

int create(char *path, char *args[]) {
    pid_t child_pid;

    int id = get_id();
    if (id < 0)
        return ERR_ID_ALLOCATION_FAILED;

    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Failed to create a new subprocess\n");
        return ERR_FORK_FAILED;
    }

    if (pid == 0) {
        int result = unshare(CLONE_NEWPID);
        if (result < 0) {
            fprintf(stderr, "Failed to call `unshare`\n");
            exit(-1);
        }
        child_pid = fork();
        if (child_pid == 0) {
            result = execv(path, args);
            if (result < 0) {
                fprintf(stderr, "Failed to call `execv`\n");
                exit(-1);
            }
        }
        waitpid(child_pid, NULL, 0);
        return child_pid;
    }

    return 0;
}
