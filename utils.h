#ifndef DAEMON_UTILS_H
#define DAEMON_UTILS_H

#include <sys/resource.h>

int pidfile_create(int* fd, const char* name);
int pidfile_write(int fd, pid_t pid);

#endif
