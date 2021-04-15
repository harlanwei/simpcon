#ifndef SIMPCON_FORK_H
#define SIMPCON_FORK_H

#include <stdbool.h>

int create(int namespaces, char *path, char *args[]);

int create_default(char *path, char *args[]);

#endif //SIMPCON_FORK_H
