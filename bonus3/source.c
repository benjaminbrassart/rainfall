int main(int argc, char **argv)
{
	FILE *f;
	char buffer[132];
	int res;

	f = fopen("/home/user/end/.pass", "r");
	// synthetic
	memset(buffer, 0x00, 128);

	if (f == NULL || argc != 2) {
		return -1;
	}

	fread(buffer, 66, 1, f);
	buffer[65] = '\0';

	res = atoi(argv[1]);
	buffer[res] = '\0';

	fread(&buffer[66], 1, 65, f);
	fclose(f);

	if (strcmp(buffer, argv[1]) == 0) {
		execl("/bin/sh", "sh", NULL);
	} else {
		puts(&buffer[42]);
	}
}
