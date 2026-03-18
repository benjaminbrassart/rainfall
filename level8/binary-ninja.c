static char *auth = NULL;
static char *auth = NULL;

/*

  c_1   -   buf[0..5] <  "auth "  UNUSED
  z_1   -   buf[0..5] == "auth "

  c_2   - 
  z_2   -

  c_3   -
  z_3   -

  c_4   -
  z_4   -

 */

int32_t main(int32_t argc, char** argv, char** envp)
{
	while (true) {
		uint32_t* const fp;
		printf("%p, %p \n", auth, service, fp);
		bool c_1 = false;
		char buf; /* at least 128 bytes */
		bool z_1 = !fgets(&buf, 128, stdin);

		if (z_1) {
			break;
		}

		int32_t i = 5;
		char* esi_1 = &buf;
		char const* const edi_1 = "auth ";

		/* repz cmps BYTE PTR ds:[esi], BYTE PTR es:[edi] */
		while (i) {
			char temp0_1 = *esi_1;
			char const temp1_1 = *edi_1;
			c_1 = temp0_1 < temp1_1;
			z_1 = temp0_1 == temp1_1;
			esi_1 = &esi_1[1];
			edi_1 = &edi_1[1];
			i -= 1;

			if (!z_1) {
				break;
			}
		}

		/*
		  r = memcmp(buf, "auth ", 5);

		  c_1 = r < 0;
		  z_1 = r == 0;
		 */

		bool c_2 = false;

		/*
		  !((!z_1 && !c_1) - c_1)


		  Truth table:

		  | z_1 | c_1 |  R  |
		  | --- | --- | --- |
		  |  0  |  0  |  0  |
		  |  1  |  0  |  1  |
		  |  0  |  1  |  0  |
		  |  1  |  1  |  0  |
		 */

		bool z_2 = !((!z_1 && !c_1) - c_1); /* z_1 && !c_1 */

		/* --> z_2 = z_1 */

		if (z_2 /* = z_1 */) {
			auth = malloc(4);
			auth[0] = '\0';

			int32_t i_1 = 0xffffffff;
			char var_8b;
			char* edi_2 = &var_8b;

			while (i_1) {
				bool cond4_1 = 0 != *edi_2;
				edi_2 = &edi_2[1];
				i_1 -= 1;

				if (!cond4_1) {
					break;
				}
			}

			c_2 = ~i_1 - 1 < 0x1e;
			z_2 = ~i_1 == 0x1f;

			if (z_2 || c_2) {
				strcpy(auth, &var_8b);
			}
		}

		int32_t i_2 = 5;
		char* esi_2 = &buf;
		char const* const edi_3 = "reset";

		while (i_2) {
			char temp2_1 = *esi_2;
			char const temp3_1 = *edi_3;
			c_2 = temp2_1 < temp3_1;
			z_2 = temp2_1 == temp3_1;
			esi_2 = &esi_2[1];
			edi_3 = &edi_3[1];
			i_2 -= 1;

			if (!z_2) {
				break;
			}
		}

		bool c_3 = false;
		bool z_3 = !((!z_2 && !c_2) - c_2);

		if (z_3) {
			free(auth);
		}

		int32_t i_3 = 6;
		char* esi_3 = &buf;
		char const* const edi_4 = "service";

		while (i_3) {
			char temp4_1 = *esi_3;
			char const temp5_1 = *edi_4;
			c_3 = temp4_1 < temp5_1;
			z_3 = temp4_1 == temp5_1;
			esi_3 = &esi_3[1];
			edi_4 = &edi_4[1];
			i_3 -= 1;

			if (!z_3) {
				break;
			}
		}

		bool c_4 = false;
		bool z_4 = !((!z_3 && !c_3) - c_3);

		if (z_4) {
			c_4 = &buf >= 0xfffffff9;
			z_4 = &buf == 0xfffffff9;
			char s[0x79];
			service = strdup(&s);
		}

		int32_t i_4 = 5;
		char* esi_4 = &buf;
		char const* const edi_5 = "login";

		while (i_4) {
			char temp7_1 = *esi_4;
			char const temp8_1 = *edi_5;
			c_4 = temp7_1 < temp8_1;
			z_4 = temp7_1 == temp8_1;
			esi_4 = &esi_4[1];
			edi_5 = &edi_5[1];
			i_4 -= 1;

			if (!z_4)
				break;
		}

		if (!((!z_4 && !c_4) - c_4)) {
			if (auth[32] == '\0') {
				fp = stdout;
				fwrite("Password:\n", 1, 0xa, fp);
			} else {
				system("/bin/sh");
			}
		}
	}

	return 0;
}
