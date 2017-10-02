#ifndef DAEMON_OPTIONS_H
#define DAEMON_OPTIONS_H

struct options
{
	int restart;
	int changeid;
	int uid;
	int gid;
	int out;
	int err;
	int help;
	int out_to_null;
	const char* shell;
	const char* rootdir;
	const char* child_pidfile;
	const char* super_pidfile;
};

void	options_init(struct options* opt);
char**	options_parse(struct options* opt, int argc, char** argv);

#endif
