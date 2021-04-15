#ifndef SIMPCON_ID_H
#define SIMPCON_ID_H

#include <sys/types.h>

int get_id();

int free_id(int id);

int check_id(int id);

int register_container(int id, pid_t pid);

pid_t get_container_pid(int id);

#endif //SIMPCON_ID_H
