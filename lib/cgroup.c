#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/cgroup.h"
#include "../include/id.h"
#include "../include/sys_wrap.h"

#define CGROUP_ROOT_PATH "/sys/fs/cgroup"
#define MAX_BUFFER_LENGTH 1024

char buf[MAX_BUFFER_LENGTH];

static int cgroup_init() {
    int result;

    snprintf(buf, MAX_BUFFER_LENGTH, "mkdir -p %s", CGROUP_ROOT_PATH);
    result = s_system(buf, "Failed to mount cgroup_dir\n", false);
    if (result) return result;

    snprintf(buf, MAX_BUFFER_LENGTH, "mount -t cgroup -o all cgroup %s", CGROUP_ROOT_PATH);
    result = s_system(buf, "Failed to mount all cgroups\n", false);
    if (result) return result;

    return 0;
}

static int _cgroup_create(int id) {
    int result;
    DIR *cgroup_dir = opendir(CGROUP_ROOT_PATH);

    if (!cgroup_dir) {
        if (errno != ENOENT || cgroup_init() != 0) {
            perror("opendir");
            return -1;
        }
    }

    snprintf(buf, MAX_BUFFER_LENGTH, "mkdir -p %s/memory/simpcon_%d", CGROUP_ROOT_PATH, id);
    result = system(buf);
    if (result) {
        fprintf(stderr, "Failed to create cgroup for %d\n", id);
        return -1;
    }

    return 0;
}

int cgroup_create(int id, bool exit_on_error) {
    int result = _cgroup_create(id);
    if (exit_on_error && result < 0) {
        exit(1);
    }
    return result;
}

int cgroup_attach(int id) {
    int result;
    pid_t pid = get_container_pid(id);

    if (!pid) {
        fprintf(stderr, "Failed to locate pid for %d\n", id);
        return -1;
    }
    snprintf(buf, MAX_BUFFER_LENGTH, "echo %d >> %s/memory/simpcon_%d/cgroup.procs", pid, CGROUP_ROOT_PATH, id);
    result = system(buf);
    if (result) {
        fprintf(stderr, "Failed to attach %d to its cgroup\n", id);
        return -1;
    }

    return 0;
}

int cgroup_detach(int id) {
    // processes in a cgroup don't have to be detached manually
}

int cgroup_destroy(int id) {
    int result;

    snprintf(buf, MAX_BUFFER_LENGTH, "rmdir %s/memory/simpcon_%d", CGROUP_ROOT_PATH, id);
    result = system(buf);
    if (result) {
        fprintf(stderr, "Failed to destroy cgroup for %d\n", id);
        return -1;
    }

    return 0;
}
