#include <stdbool.h>
#include <sys/types.h>

#include "../include/id.h"

#define MAX_N_CONTAINERS 300

static bool id_pool[MAX_N_CONTAINERS];

#define is_id_invalid(id) \
    (id) < 0 || (id) >= MAX_N_CONTAINERS

struct container_info {
    bool valid;
    pid_t pid;
} container_pool[MAX_N_CONTAINERS];

int get_id() {
    for (int i = 0; i < MAX_N_CONTAINERS; ++i) {
        if (!id_pool[i]) {
            id_pool[i] = true;
            return i;
        }
    }
    return -1;
}

int register_container(int id, pid_t pid) {
    if (is_id_invalid(id) || !id_pool[id]) return -1;
    container_pool[id].valid = true;
    container_pool[id].pid = pid;
    return 0;
}

int free_id(int id) {
    if (is_id_invalid(id) || !id_pool[id]) return -1;
    id_pool[id] = false;
    container_pool[id].valid = false;
    return 0;
}

int check_id(int id) {
    if (is_id_invalid(id)) return -1;
    return id_pool[id];
}

/**
 * @return Will return 0 when the given `id` is invalid.
 */
pid_t get_container_pid(int id) {
    if (is_id_invalid(id) || !id_pool[id] || !container_pool[id].valid) return 0;
    return container_pool[id].pid;
}
