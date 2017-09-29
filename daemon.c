#include "daemon.h"
#include "options.h"
#include "utils.h"

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include <stdlib.h>
#include <stdio.h>
#include <strings.h>

static void dochild(const struct options* opts, int argc, char** argv);
static void handle_sigchild();

void init_daemon(const struct options* opts, int argc, char** argv)
{
	pid_t  childpid;
	int    fd;
	int    status;

	if (opts->changeid) {
		if (setuid(opts->uid >= 0 ? opts->uid : 0) == -1) {
			fprintf(stderr, "daemon: error setuid=%d errno=%d error=%s\n", opts->uid >= 0 ? opts->uid : 0, errno, strerror(errno));
			exit(1);
		}
		if (setgid(opts->gid >= 0 ? opts->gid : 0) == -1) {
			fprintf(stderr, "daemon: error setuid=%d errno=%d error=%s\n", opts->gid >= 0 ? opts->gid : 0, errno, strerror(errno));
			exit(1);
		}
	}

	signal(SIGCHLD, handle_sigchild);
	setsid();
	opts->rootdir ? chdir(opts->rootdir) : chdir("/");

	do {
		fprintf(stderr, "daemon: spawning child=%s restart=%d\n", argv[0], opts->restart);

		childpid = fork();
		if (childpid < 0) {
			fprintf(stderr, "daemon: error spawning cmd=%s errno=%d error=%s\n", argv[0], errno, strerror(errno));
			exit(-1);
		}
		if (childpid == 0) {
			dochild(opts, argc, argv);
		}

		if (pidfile_create(&fd, opts->child_pidfile)) {
			pidfile_write(fd, childpid);
			close(fd);
		}
		if (pidfile_create(&fd, opts->super_pidfile)) {
			pidfile_write(fd, getpid());
			close(fd);
		}
		waitpid(childpid, &status, 0);
	}
	while (opts->restart);
}

static void dochild(const struct options* opts, int argc, char** argv) {
	int    fd;
	struct rlimit max_files;

	bzero(&max_files, sizeof(max_files));
	getrlimit(RLIMIT_NOFILE, &max_files);
	for (fd = 0; fd < (int)max_files.rlim_max; ++fd) {
//		close(fd);
	}

	umask(0);

	execvp(argv[0], argv);
}

static void handle_sigchild()
{
	pid_t pid;
	int status;
	
	while ((pid = wait3(&status, WNOHANG, (struct rusage *)NULL)) > 0)
		 ;
}
