#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char **argv)
{
	for (int i = 1; i < argc; i += 1) {
		const char *s = argv[i];
		int base = 10;

		if (strncmp(s, "0x", 2) == 0) {
			base = 16;
			s += 2;
		} else if (strncmp(s, "0b", 2) == 0) {
			base = 2;
			s += 2;
		} else if (strncmp(s, "0o", 2) == 0) {
			base = 8;
			s += 2;
		}

		printf("base: %d\n", base);

		int n = strtol(s, NULL, base);
		uint32_t sz = (uint32_t)n;
		uint32_t sz4 = n * 4U;

		printf("\n");
		printf("==== argv[%d] ====\n", i);
		printf("s      = |%11s| base %d\n", argv[i], base);
		printf("n      = |%11d|\n", n);
		printf("sz     = |%11u| %032b\n", sz, sz);
		printf("sz * 4 = |%11u| %032b\n", sz4, sz4);
	}
}
