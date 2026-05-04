enum language {
	LANG_EN = 0,
	LANG_FI = 1,
	LANG_NL = 2,
};

static enum language language = LANG_EN;

// "Hyvää päivää "
static const unsigned char MESSAGE_FI[19] = {
	0x48, 0x79, 0x76, 0xc3, 0xa4, 0xc3, 0xa4, 0x20, 0x70, 0xc3, 0xa4, 0x69,
	0x76, 0xc3, 0xa4, 0xc3, 0xa4, 0x20, 0x00
};

// "Goedemiddag! "
static const unsigned char MESSAGE_NL[14] = {
	0x47, 0x6f, 0x65, 0x64, 0x65, 0x6d, 0x69, 0x64, 0x64, 0x61, 0x67, 0x21,
	0x20, 0x00
};

// "Hello "
static const unsigned char MESSAGE_EN[7] = {
	0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x00
};

static void greetuser(void)
{
	char message[72];

	if (language == LANG_FI) {
		*(unsigned int *)(message + 0) = *(unsigned int *)(MESSAGE_FI + 0);
		*(unsigned int *)(message + 4) = *(unsigned int *)(MESSAGE_FI + 4);
		*(unsigned int *)(message + 8) = *(unsigned int *)(MESSAGE_FI + 8);
		*(unsigned int *)(message + 12) = *(unsigned int *)(MESSAGE_FI + 12);
		*(unsigned short *)(message + 16) = *(unsigned short *)(MESSAGE_FI + 16);
		*(unsigned char *)(message + 18) = *(unsigned char *)(MESSAGE_FI + 18);
	} else if (language == LANG_NL) {
		*(unsigned int *)(message + 0) = *(unsigned int *)(MESSAGE_NL + 0);
		*(unsigned int *)(message + 4) = *(unsigned int *)(MESSAGE_NL + 4);
		*(unsigned int *)(message + 8) = *(unsigned int *)(MESSAGE_NL + 8);
		*(unsigned short *)(message + 12) = *(unsigned short *)(MESSAGE_NL + 12);
	} else if (language == LANG_EN) {
		*(unsigned int *)(message + 0) = *(unsigned int *)(MESSAGE_FI + 0);
		*(unsigned short *)(message + 4) = *(unsigned short *)(MESSAGE_FI + 4);
		*(unsigned char *)(message + 6) = *(unsigned char *)(MESSAGE_FI + 6);
	}

	strcat(message, &stack_buffer_from_main);
	puts(message);
}

int main(int argc, char **argv)
{
	char buffer[80];
	char *lang;

	if (argc != 3) {
		return 1;
	}

	// https://www.felixcloutier.com/x86/rep:repe:repz:repne:repnz
	// synthetic: REP STOS DWORD PTR es:[edi], eax
	memset(buffer, 0x00, 80);

	strncpy(&buffer[0], argv[1], 40);
	strncpy(&buffer[40], argv[2], 32);

	lang = getenv("LANG");
	if (lang != NULL) {
		if (memcmp(lang, "fi", 2) == 0) {
			language = LANG_FI;
		} else if (memcmp(lang, "nl", 2) == 0) {
			language = LANG_NL;
		}
	}

	greetuser();
	return 0;
}
