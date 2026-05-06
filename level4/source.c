int m = 0;

void p(char *buffer)
{
	printf(buffer);
}

void n(void)
{
	char buffer[520];

	fgets(buffer, 512, stdin);
	p(buffer);

	if (m == 16930116) {
		system("cat /home/user/level5/.pass");
	}
}

int main(void)
{
	n();
}
