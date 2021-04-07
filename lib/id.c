#include <stdbool.h>

#include "../include/id.h"

#define MAX_N_CONTAINERS 300

static bool id_pool[MAX_N_CONTAINERS];

#define is_id_invalid(id) \
    (id) < 0 || (id) >= MAX_N_CONTAINERS

int get_id() {
    for (int i = 0; i < MAX_N_CONTAINERS; ++i) {
        if (!id_pool[i]) {
            id_pool[i] = true;
            return i;
        }
    }
    return -1;
}

int free_id(int id) {
    if (is_id_invalid(id) || !id_pool[id]) return -1;
    id_pool[id] = false;
    return 0;
}

int check_id(int id) {
    if (is_id_invalid(id)) return -1;
    return id_pool[id];
}
