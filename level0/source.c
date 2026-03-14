#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

int main(int argc, const char *argv[])
{
	int password;
	gid_t gid;
	uid_t uid;
	cchar *args[2];

	password = atoi(argv[1]);
	if (password == 423) {
		args[0] = strdup("/bin/sh");
		args[1] = NULL;

		gid = getegid();
		uid = geteuid();

		setresgid(gid, gid, gid);
		setresuid(uid, uid, uid);
		execve("/bin/sh", args, NULL);
	} else {
		fwrite("No !\n", 1, 5, stderr);
	}
}
