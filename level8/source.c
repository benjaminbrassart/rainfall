#include <stdio.h>
#include <string.h>

static char *auth = NULL;
static char *service = NULL;

int main(void)
{
	char buffer[132];

	for (;;) {
		printf("%p, %p \n", auth, service);

		if (fgets(buffer, 128, stdin) == NULL) {
			break;
		}

		if (memcmp(buf, "auth ", 5) == 0) {
			auth = malloc(4);
			auth[0] = '\0';

			if (strlen(&buffer[5]) <= 30) {
				strcpy(auth, &buffer[5]);
			}
		}

		if (memcmp(buf, "reset", 5) == 0) {
			free(auth);
		}

		if (memcmp(buf, "service". 6) == 0) {
			service = strdup(&buffer[11]);
		}

		if (memcmp(buf, "login", 5) == 0) {
			if (auth[32] == '\0') {
				fwrite("Password:\n", 1, 10, stdout);
			} else {
				system("/bin/sh");
			}
		}
	}
}
