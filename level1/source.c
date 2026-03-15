#include <stdio.h>
#include <stdlib.h>

char *gets(char *);

void main(void)
{
	char buffer[76];

	gets(buffer);
}

// 0x08048444
void run(void)
{
	fwrite("Good... Wait what?\n", 1, 19, stdout);
	system("/bin/sh");
}
