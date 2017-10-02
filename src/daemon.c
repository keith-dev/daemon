#include "daemon.h"
#include "options.h"
#include "utils.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <strings.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

static void dochild(const struct options* opts, int argc, char** argv);
static int  isscript(const char* prog);
static void setopts(const struct options* opts, int* nochdir, int* noclose);
static void pidfiles_create(const struct options* opts, pid_t childpid);
static void pidfiles_remove(const struct options* opts);

void init_daemon(const struct options* opts, int argc, char** argv)
{
	int     nochdir;
	int     noclose;

	setopts(opts, &nochdir, &noclose);

	do {
		pid_t  childpid;
		int    status;

		fprintf(stderr, "daemon: spawning child. cmd=%s restart=%d\n",
			argv[0], opts->restart);

		childpid = fork();
		if (childpid < 0) {
			fprintf(stderr, "daemon: error spawning child. cmd=%s errno=%d error=%s\n",
				argv[0], errno, strerror(errno));
			exit(-1);
		}
		if (childpid == 0) {
			dochild(opts, argc, argv);
		}

		pidfiles_create(opts, childpid);
		waitpid(childpid, &status, 0);
		pidfiles_remove(opts);
	}
	while (opts->restart);
}

static void dochild(const struct options* opts, int argc, char** argv)
{
	umask(0);

	if (isscript(argv[0])) {
		int     i;
		const char* shell;
		int     new_argc;
		char**  new_argv;

		if (opts->shell) {
			shell = opts->shell;
		}
		else {
			shell = getenv("SHELL");
		}
		if (!shell) {
			shell = "/bin/sh";
		}

		if (access(shell, R_OK) == -1) {
			fprintf(stderr, "daemon: cannot find shell. shell=%s errno=%d error=%s\n",
				shell, errno, strerror(errno));
			exit(1);
		}

		new_argc = 2 + argc;
		new_argv = malloc((new_argc + 1) * sizeof(char*));
		new_argv[0] = strdup(shell);
		new_argv[1] = strdup("-c");
		for (i = 0; i < argc; ++i)
			new_argv[2 + i] = argv[i];
		new_argv[2 + argc] = NULL;

		execvp(new_argv[0], new_argv);
	}

	execvp(argv[0], argv);
}

static int isscript(const char* prog)
{
	int     ok = 0;
	int     fd;
	
	fd = open(prog, O_RDONLY);
	if (fd >= 0) {
		int     n;
		char    buf[2];

		n = read(fd, buf, 2);
		ok = n == 2 && memcmp("#!", buf, 2) == 0;
		close(fd);
	}

	return ok;
}

static void setopts(const struct options* opts, int* nochdir, int* noclose)
{
	*nochdir = 1;
	*noclose = 1;

	if (opts->out_to_null) {
		*noclose = 0;
	}
	daemon(*nochdir, *noclose);

	opts->rootdir ? chdir(opts->rootdir) : chdir("/");

	if (opts->changeid) {
		if (setuid(opts->uid >= 0 ? opts->uid : 0) == -1) {
			fprintf(stderr, "daemon: error setuid=%d errno=%d error=%s\n",
				opts->uid >= 0 ? opts->uid : 0, errno, strerror(errno));
			exit(1);
		}
		if (setgid(opts->gid >= 0 ? opts->gid : 0) == -1) {
			fprintf(stderr, "daemon: error setuid=%d errno=%d error=%s\n",
				opts->gid >= 0 ? opts->gid : 0, errno, strerror(errno));
			exit(1);
		}
	}

	setsid();
}

static void pidfiles_create(const struct options* opts, pid_t childpid)
{
	int    fd;

	if (pidfile_create(&fd, opts->child_pidfile)) {
		pidfile_write(fd, childpid);
		close(fd);
	}
	if (pidfile_create(&fd, opts->super_pidfile)) {
		pidfile_write(fd, getpid());
		close(fd);
	}
}

static void pidfiles_remove(const struct options* opts)
{
	if (opts->child_pidfile) {
		unlink(opts->child_pidfile);
	}
	if (opts->super_pidfile) {
		unlink(opts->super_pidfile);
	}
}
