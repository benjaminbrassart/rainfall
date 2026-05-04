# bonus3

What happens in this program:

1. Open `~end/.pass` read-only
2. Zero read buffer (128 bytes)
3. Read 66 bytes from the file
4. Convert first argument to int using `atoi(3)`
5. Set the byte at that index value to NUL
6. Read 65 bytes from the file
7. Close the file
8. Execute `/bin/sh` if the content of the read buffer is equal to the content of the first argument (C string comparison)

The attack surface is obviously the use of `atoi(3)` as an index.  `man 3 atoi` states:

```
[...]

RETURN VALUE
       The converted value or 0 on error.

[...]

BUGS
       errno is not set on error so there is no way to distinguish between 0 as
       an error and as the converted value.

[...]
```

As a first instinct, let's try an empty string.

```sh
./level2 ""
```

And we get a shell.  That's it.

For the sake of completeness, here's an explanation.

It works because `index = atoi("")` returns `0` (see man page excerpt).  The program then sets `buffer[index] = '\0'`, basically truncating it to a 0-length C string.  The subsequent call to `fread(3)` changes nothing because it writes at index 66.  Then `strcmp(buffer, argv[1])` translates to `strcmp("", "")` which fulfills the win condition.
