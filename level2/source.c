void p(void)
{
	// part of the stack frame, not a local variable
	unsigned int unaff_retaddr;
	char buffer[76];

	fflush(stdout);
	gets(buffer);

	if ((unaff_retaddr & 0xb0000000) == 0xb0000000) {
		printf("(%p)\n", unaff_retaddr);
		_exit(1);
	}

	puts(buffer);
	strdup(buffer);
}

void main(void)
{
	p();
}
