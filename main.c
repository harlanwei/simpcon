#include <stdio.h>
#include <sys/wait.h>

#include "include/fork.h"

int main() {
    char *args[] = {"bash", NULL};
    create("/usr/bin/bash", args);
    wait(NULL);
    return 0;
}
