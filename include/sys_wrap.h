#ifndef SIMPCON_SYS_WRAP_H
#define SIMPCON_SYS_WRAP_H

#include <stdbool.h>

int s_open(char *path, int mode, bool exit_on_error);

int s_write(int fd, void *buf, size_t len, bool exit_on_error);

/**
 * @returns A `long` since `pivot_root` uses `syscall` at its core.
 */
long s_pivot_root(const char *new_root, const char *old_root, bool exit_on_error);

/**
 * Usually used for debugging purposes.
 */
__attribute__((unused)) int s_ls(const char *dir, bool exit_on_error);

int s_umount(const char *path, int flags, bool exit_on_error);

int s_system(const char *cmd, const char *debug_msg, bool exit_on_error);

#endif //SIMPCON_SYS_WRAP_H
