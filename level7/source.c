#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char c[80];

int main(int argc, char **argv)
{
	char **xs1;
	char *s1;
	char **xs2;
	char *s2;
	FILE *f;

	xs1 = malloc(8);
	xs1[0] = (char *)0x1;

	s1 = malloc(8);
	xs1[1] = s1;

	xs2 = malloc(8);
	xs2[0] = (char *)0x2;

	s2 = malloc(8);
	xs2[1] = s2;

	strcpy(xs1[1], argv[1]);
	strcpy(xs2[1], argv[2]);

	f = fopen("/home/user/level8/.pass","r");
	fgets(c, 68, f);

	puts("~~");
	return 0;
}
