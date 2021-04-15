#define _GNU_SOURCE

#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/wait.h>

#include "../include/cgroup.h"
#include "../include/errno.h"
#include "../include/fork.h"
#include "../include/id.h"
#include "../include/sys_wrap.h"

#define BUF_SIZE 128
#define INIT_STACK_SIZE 4096

struct init_args {
    int id;
    char *path;
    char **args;
    uid_t euid;
    uid_t egid;
};

static int init_stack[INIT_STACK_SIZE];

static int spawn(char *path, char *args[]) {
    if (execvp(path, args) == -1) {
        perror("execvp");
        exit(1);
    }

    // Spawn should never return if everything works as expected
    return -1;
}

static void chuser(uid_t to_uid, uid_t to_gid) {
    int fd;
    char *buf = malloc(BUF_SIZE);

    fd = s_open("/proc/self/setgroups", O_WRONLY, true);
    s_write(fd, "deny", 4, true);
    close(fd);

    sprintf(buf, "%d %d 1\n", 0, to_uid);
    fd = s_open("/proc/self/uid_map", O_WRONLY, true);
    s_write(fd, buf, strlen(buf), true);
    close(fd);

    sprintf(buf, "%d %d 1\n", 0, to_gid);
    fd = s_open("/proc/self/gid_map", O_WRONLY, true);
    s_write(fd, buf, strlen(buf), true);
    close(fd);

    free(buf);
}

static int init_container(void *args) {
    struct init_args *_args = (struct init_args *) args;
    mount("/home/vian/simpcon/fkroot", "/home/vian/simpcon/fkroot", NULL, MS_BIND, NULL);
    chdir("/home/vian/simpcon/fkroot");
    s_pivot_root("/home/vian/simpcon/fkroot", "/home/vian/simpcon/fkroot/tmp/oldroot", true);
    mount("/proc", "/proc", "proc", 0, NULL);
    s_umount("/tmp/oldroot", MNT_DETACH, true);
    chuser(_args->euid, _args->egid);
    setenv("PS1", "\\w \\$ ", true);
    spawn(_args->path, _args->args);
    return 0;
}

int create(int add_namespaces, char *path, char *args[]) {
    char *buf;
    int id, namespaces, result;
    pid_t child_pid;
    struct init_args init_container_args = {
            .id = id,
            .euid = geteuid(),
            .egid = getegid(),
            .path = path,
            .args = args
    };

    id = get_id();
    if (id < 0)
        return ERR_ID_ALLOCATION_FAILED;

    cgroup_create(id, true);

    namespaces = add_namespaces | CLONE_NEWPID | CLONE_NEWUSER | CLONE_NEWIPC | CLONE_NEWNS;
    child_pid = clone(
            init_container,
            init_stack + INIT_STACK_SIZE,
            SIGCHLD | namespaces,
            (void *) &init_container_args
    );
    if (child_pid == -1) {
        fprintf(stderr, "Failed to `clone`\n");
        exit(1);
    }

    register_container(id, child_pid);
    cgroup_attach(id);
    waitpid(child_pid, NULL, 0);
    cgroup_destroy(id);

    return id;
}

int create_default(char *path, char *args[]) {
    return create(0, path, args);
}
