`strcpy(3)` and `strcat(3)` do not append a NUL byte to the destination if the source contains more than `n - 1` non-NUL characters.  For example:

```c
char buffer[] = "AAAAAAAAA";

strncpy(buffer, "hello", 5);
printf("|%s|\n", buffer); // prints |helloAAAA|
```
