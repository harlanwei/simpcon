#ifndef SIMPCON_CGROUP_H
#define SIMPCON_CGROUP_H

#include <stdbool.h>

int cgroup_create(int id, bool exit_on_error);

int cgroup_attach(int id);

int cgroup_detach(int id);

int cgroup_destroy(int id);

#endif //SIMPCON_CGROUP_H
