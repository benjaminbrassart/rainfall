#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

static void format_bin(char *buf, uint32_t n)
{
	buf[32] = '\0';

	for (int i = 0; i < 32; i += 1) {
		buf[i] = (n & (1 << (31 - i))) ? '1' : '0';
	}
}

int main(int argc, const char **argv)
{
	for (int i = 1; i < argc; i += 1) {
		int n = atoi(argv[i]);
		uint32_t sz = (uint32_t)n;
		uint32_t sz4 = n * 4U;
		char bin[33];
		char bin4[33];

		format_bin(bin, sz);
		format_bin(bin4, sz4);

		printf("\n");
		printf("==== argv[%d] ====\n", i);
		printf("s      = |%11s|\n", argv[i]);
		printf("n      = |%11d|\n", n);
		printf("sz     = |%11u| %s\n", sz, bin);
		printf("sz * 4 = |%11u| %s\n", sz4, bin4);
	}
}
