#include "options.h"
#include "daemon.h"
#include <stdlib.h>
#include <stdio.h>

void usage(void);

int main(int argc, char** argv)
{
	int cmd_argc;
	char** cmd_argv;
	struct options opts;

	options_init(&opts);
	cmd_argv = options_parse(&opts, argc, argv);
	cmd_argc = argc - (cmd_argv - argv);

	if (opts.help)
	{
		usage();
		exit(0);
	}

	init_daemon(&opts, cmd_argc, cmd_argv);
	return 0;
}

void usage(void)
{
	const char* text[] =
	{
		"daemon [-cfrs] [-p child_pidfile] [-P supervisor_pidfile] [-t title]",
		"       [-u user] [-m output_mask] [-o output_file] [-s syslog_priority]",
		"       [-T syslog_tag] [-s syslog_facility] command arguments ...",
		"",
		"-h, --help show this text",
		"",
		"-c         Change the current working directory to the root (``/'').",
		"",
		"-f         Redirect standard input, standard output and standard error to",
		"           /dev/null",
		"",
		"-S         Enable syslog output.  This is implicitly applied if other syslog",
		"           parameters are provided.  The default values are daemon, notice",
		"           and daemon for facility, priority, and tag, respectively.",
		"",
		"-o output_file",
		"           Append output from the daemonized process to output_file.  If the",
		"           file does not exist, it is created with permissions 0600.",
		"",
		"-m output_mask",
		"           Redirect output from the child process stdout (1), stderr (2), or",
		"           both (3). This value specifies what is sent to syslog and the",
		"           log file. The default is 3.",
		"",
		"-p child_pidfile",
		"           Write the ID of the created process into the child_pidfile using",
		"           the pidfile(3) functionality. The program is executed in a",
		"           spawned child process while the daemon waits until it terminates",
		"           to keep the child_pidfile locked and removes it after the process",
		"           exits.  The child_pidfile owner is the user who runs the daemon",
		"           regardless of whether the -u option is used or not.",
		"",
		"-P supervisor_pidfile",
		"           Write the ID of the daemon process into the supervisor_pidfile",
		"           using the pidfile(3) functionality. The program is executed in a",
		"           spawned child process while the daemon waits until it terminates",
		"           to keep the supervisor_pidfile locked and removes it after the",
		"           process exits. The supervisor_pidfile owner is the user who runs",
		"           the daemon regardless of whether the -u option is used or not.",
		"",
		"-r         Supervise and restart the program if it has been terminated.",
		"",
		"-t title",
		"           Set the title for the daemon process. The default is the daemo-",
		"           nized invocation.",
		"",
		"-u user",
		"           Login name of the user to execute the program under.  Requires",
		"           adequate superuser privileges.",
		"",
		"-s syslog_priority",
		"           These priorities are accepted: emerg, alert, crit, err, warning,",
		"           notice, info, and debug. The default is info.",
		"",
		"-l syslog_facility",
		"           These facilities are accepted: auth, authpriv, console, cron,",
		"           daemon, ftp, kern, lpr, mail, news, ntp, security, syslog, user,",
		"           uucp, and local0, ..., local7. The default is daemon.",
		"",
		"-T syslog_tag",
		"           Set the tag which is appended to all syslog messages. The",
		"           default is daemon.",
		"",
		"If any of the options -p, -P, -r, -o, -s, -T, -m, -S, or -l are speci-",
		"fied, the program is executed in a spawned child process. The daemon",
		"waits until it terminates to keep the pid file(s) locked and removes them",
		"after the process exits or restarts the program.  In this case if the",
		"monitoring daemon receives software termination signal (SIGTERM) it for-",
		"wards it to the spawned process.  Normally it will cause the child to",
		"exit, remove the pidfile(s) and then terminate.",
		"",
		"If neither file or syslog output are selected, all output is redirected",
		"to the daemon process and written to stdout.  The -f option may be used",
		"to suppress the stdout output completely.",
		NULL
	};
	int i;

	for (i = 0; text[i]; ++i)
		fprintf(stdout, "%s\n", text[i]);
}
