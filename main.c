#include <stdio.h>
#include <sys/wait.h>

#include "include/fork.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program path> [args]\n", argv[0]);
    }
    create_default(argv[1], argv + 1);
    wait(NULL);
    return 0;
}
