int main(int argc, char **argv)
{
	char buffer[40];
	int n;

	n = atoi(argv[1]);
	if (n >= 10) {
		return 1;
	}

	memcpy(buffer, argv[2], n * 4);

	if (n != 0x574f4c46) {
		return 0;
	}

	execl("/bin/sh");
	return 0;
}
