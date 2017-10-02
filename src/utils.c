#include "utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <stdio.h>

int pidfile_create(int* fd, const char* name) {
//	char cwd[512];

	if (!name)
		return 0;

	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	*fd = creat(name, mode);
//	fprintf(stderr, "%s: cwd=%s name=%s fd=%d ret=%d\n", __FUNCTION__, getcwd(cwd, sizeof(cwd)), name, *fd, *fd >= 0);
	return *fd >= 0;
}

int pidfile_write(int fd, pid_t pid) {
	int n;
	char buf[32];

	snprintf(buf, sizeof(buf), "%d\n", pid);
//	fprintf(stderr, "%s: %s", __FUNCTION__, buf);
	n = write(fd, buf, strlen(buf));
	return n > 0;
}
