#include <stdio.h>
#include <sys/wait.h>

#include "include/fork.h"

int main() {
    char *args[] = {"sh", NULL};
    create(true, "/usr/bin/sh", args);
    wait(NULL);
    return 0;
}
