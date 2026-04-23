#define SEPARATOR " - "

void p(char *buffer, const char *separator)
{
	char *nl;
	char rbuf[4104];

	puts(separator);
	read(STDIN_FILENO, rbuf, 4096);
	nl = strchr(rbuf, '\n');
	*nl = '\0';
	strncpy(buffer, rbuf, 20);
}

void pp(char *buffer)
{
	size_t buffer_len;

	p(buf1, SEPARATOR);
	p(buf2, SEPARATOR);

	strcpy(buffer, buf1);

	buffer_len = strlen(buffer);
	buffer[buffer_len] = ' ';
	buffer[buffer_len + 1] = '\0';

	strcat(buffer, buf2);
}

int main(void)
{
	char buffer[54];

	pp(buffer);
	puts(buffer);
}
