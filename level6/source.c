void n(void)
{
	system("/bin/cat /home/user/level7/.pass");
}

void m(void)
{
	puts("Nope");
}

typedef void (func_t)(void);

int main(int argc, char **argv)
{
	char *big;
	func_t **small;

	big = malloc(64);
	small = malloc(4);
	*small = m;
	strcpy(big, argv[1]);
	(*small)();
}
