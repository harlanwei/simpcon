#ifndef SIMPCON_FORK_H
#define SIMPCON_FORK_H

#include <stdbool.h>

void create(int namespaces, char *path, char *args[]);

void create_default(char *path, char *args[]);

#endif //SIMPCON_FORK_H
