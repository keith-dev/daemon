#ifndef DAEMON_DAEMON_H
#define DAEMON_DAEMON_H

struct options;

void init_daemon(const struct options* opts, int argc, char** argv);

#endif
