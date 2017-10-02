#include "options.h"
#include "utils.h"

#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*-------------------------------------------------------------------------*/
static const char* s_rootdir = "/";

/*-------------------------------------------------------------------------*/
void options_init(struct options* opts)
{
	if (opts) {
		opts->restart = 0;
		opts->changeid = 1;
		opts->uid = opts->gid = -1;
		opts->out = opts->err = -1;
		opts->out_to_null = 0;
		opts->help = 0;

		opts->shell =
		opts->child_pidfile =
		opts->super_pidfile = NULL;
		opts->rootdir = s_rootdir;
	}
}

char** options_parse(struct options* opts, int argc, char** argv)
{
	int i;
	int fd;

	for (i = 1; i < argc; ++i) {
		if (argv[i][0] != '-') {
			fprintf(stderr, "unrecognized arg:%s\n", argv[i]);
			exit(1);
		}

		switch (argv[i][1]) {
		case 'h':
			opts->help = 1;
			return argv + i;

		case 'c': {
				if ((i + 1) >= argc) {
					fprintf(stderr, "daemon: missing root directory\n");
					exit(1);
				}
				if (access(argv[i + 1], R_OK) == -1) {
					fprintf(stderr, "daemon: root directory \"%s\" does not exist\n", argv[i + 1]);
					exit(1);
				}
				opts->rootdir = argv[++i];
			}
			break;

		case 'r':
			opts->restart = 1;
			break;

		case 'f':
			opts->out_to_null = 1;
			break;

		case 'p': {
				if ((i + 1) >= argc) {
					fprintf(stderr, "daemon: missing child pidfile\n");
					exit(1);
				}
				opts->child_pidfile = argv[++i];

				if (!pidfile_create(&fd, opts->child_pidfile)) {
					fprintf(stderr, "daemon: cannot open child pidfile: \"%s\"\n", opts->child_pidfile);
					exit(1);
				}
				close(fd);
				if (unlink(opts->child_pidfile) == -1) {
					fprintf(stderr, "daemon: cannot remove child pidfile: \"%s\"\n", opts->super_pidfile);
//					exit(1);
				}
			}
			break;

		case 'P': {
				if ((i + 1) >= argc) {
					fprintf(stderr, "daemon: missing supervisor pidfile\n");
					exit(1);
				}
				opts->super_pidfile = argv[++i];;

				if (!pidfile_create(&fd, opts->super_pidfile)) {
					fprintf(stderr, "daemon: cannot open child pidfile: \"%s\"\n", opts->super_pidfile);
					exit(1);
				}
				close(fd);
				if (unlink(opts->super_pidfile) == -1) {
					fprintf(stderr, "daemon: cannot remove supervisor pidfile: \"%s\"\n", opts->super_pidfile);
//					exit(1);
				}
			}
			break;

		case 'u': {
				const char* user = argv[++i];
				struct passwd* passwdrec = NULL;

				if ((i + 1) >= argc || user[0] == '-') {
					fprintf(stderr, "daemon: missing user\n");
					exit(1);
				}

				errno = 0;
				passwdrec = getpwnam(user);
				if (!passwdrec) {
					if (errno) {
						fprintf(stderr, "daemon: user db error. user=%s errno=%d error=%s. Using existing user\n", user, errno, strerror(errno));
						opts->changeid = 0;
					}
					else {
						fprintf(stderr, "daemon: user %s does not exist\n", user);
					}
				}
				else {
					opts->uid = passwdrec->pw_uid;
					opts->gid = passwdrec->pw_gid;
				}
			}
			break;

		case '-':
			if (strcmp(argv[i] + 2, "help") == 0) {
				opts->help = 1;
				return argv + i;
			}
			else if (strcmp(argv[i] + 2, "shell") == 0) {
				opts->shell = argv[++i];
			}
			break;

		default:
			fprintf(stderr, "unrecognized arg:%s\n", argv[i]);
			exit(1);
		}
	}

	/*  return argv of command */
	return argv + i;
}
