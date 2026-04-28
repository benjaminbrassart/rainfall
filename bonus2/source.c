enum language {
	LANG_EN = 0,
	LANG_FI = 1,
	LANG_NL = 2,
};

static enum language language = LANG_EN;

static void greetuser(void)
{
	char message[72];

	if (language == LANG_FI) {
		strncpy(message, "Hyvää päivää ", 19);
	} else if (language == LANG_NL) {
		strncpy(message, "Goedemiddag! ", 14);
		// XXX message._14_2_ = SUB42(uVar1, 2)
	} else if (language == LANG_EN) {
		strncpy(message, "Hello ", 7);
	}

	strcat(message, &stack0x00000004); // ???
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
	strncpy(&buffer[40], argv[1], 32);

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
