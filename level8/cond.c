#include <stdio.h>

static void print_entry(int z, int c)
{
	int r = !((!z && !c) - c);

	printf("|  %d  |  %d  |  %d  |\n", z, c, r);
}

int main(void)
{
	printf("|  Z  |  C  |  R  |\n");
	printf("| --- | --- | --- |\n");

	print_entry(0, 0);
	print_entry(1, 0);
	print_entry(0, 1);
	print_entry(1, 1);

	return 0;
}
