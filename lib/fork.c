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
#include "../include/fork.h"
#include "../include/id.h"
#include "../include/sys_wrap.h"

#define BUF_SIZE 128
#define INIT_STACK_SIZE 4096
#define FKROOT_ENV_NAME "simpcon_fkroot"

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

    snprintf(buf, BUF_SIZE, "%d %d 1\n", 0, to_uid);
    fd = s_open("/proc/self/uid_map", O_WRONLY, true);
    s_write(fd, buf, strlen(buf), true);
    close(fd);

    snprintf(buf, BUF_SIZE, "%d %d 1\n", 0, to_gid);
    fd = s_open("/proc/self/gid_map", O_WRONLY, true);
    s_write(fd, buf, strlen(buf), true);
    close(fd);

    free(buf);
}

static const char *get_fkroot_path() {
    const char *path_rootfs = getenv(FKROOT_ENV_NAME);

    if (!path_rootfs) {
        fprintf(stderr, "Error: rootfs path is not defined in the environment. Set `%s` to define it.\n",
                FKROOT_ENV_NAME);
        exit(1);
    }

    return path_rootfs;
}

static void get_old_root(const char *fkroot, char *buf, int buf_size) {
    if (!buf)
        exit(1);

    snprintf(buf, buf_size, "%s/tmp/oldroot", fkroot);
}

static int init_container(void *args) {
    char *old_root_path = malloc(BUF_SIZE);
    const char *fkroot_path = get_fkroot_path();
    struct init_args *_args = (struct init_args *) args;

    get_old_root(fkroot_path, old_root_path, BUF_SIZE);
    mount(fkroot_path, fkroot_path, NULL, MS_BIND, NULL);
    chdir(fkroot_path);
    s_pivot_root(fkroot_path, old_root_path, true);
    mount("/proc", "/proc", "proc", 0, NULL);
    s_umount("/tmp/oldroot", MNT_DETACH, true);
    free(old_root_path);

    chuser(_args->euid, _args->egid);

    setenv("PS1", "\\w \\$ ", true);
    spawn(_args->path, _args->args);

    return 0;
}

void create(int add_namespaces, char *path, char *args[]) {
    char *buf;
    int id, namespaces, result;
    pid_t child_pid;
    struct init_args init_container_args = {
            .id = id,
            .euid = geteuid(),
            .egid = getegid(),
            .path = path,
            .args = args,
    };

    id = get_id();
    if (id < 0) {
        fprintf(stderr, "Error: reached the maximum number of active containers.\n");
        return;
    }

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
        return;
    }

    register_container(id, child_pid);
    cgroup_attach(id);
    waitpid(child_pid, NULL, 0);
    cgroup_detach(id);
}

void create_default(char *path, char *args[]) {
    create(0, path, args);
}
