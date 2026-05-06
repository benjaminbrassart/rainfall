This program is a bit confusing, maybe because two functions are named `p` and `pp`.  Let's dissect them:

`p`:
1. Prints its second argument
2. Reads 4096 bytes from stdin into a buffer
3. Replaces the first occurrence of LF by NUL
4. Copies at most 20 bytes from the read buffer into its first argument (using `strncpy(3)`)

`pp`:
1. Declares two 20-byte buffers
2. Calls `p` on the first buffer
3. Calls `p` on the second buffer
4. Copies the first buffer into its first argument (using `strcpy(3)`)
5. Appends a space the its first argument
6. Appends the second buffer to its first argument (using `strcat(3)`)

And `main` calls `pp` on 42-byte stack buffer, then prints it.  We can flatten the whole process to make it easier to grasp:

```
(pseudocode)

# main()
main_buffer = [42]byte

    # pp()
    buf1 = [20]byte
    buf2 = [20]byte

        # p()
        rbuf = read(4096)
        rbuf.replace_first('\n', '\0')

        c_string_copy(dst=rbuf, src=buf1, len=20)

        # p()
        rbuf = read(4096)
        rbuf.replace_first('\n', '\0')

        c_string_copy(dst=rbuf, src=buf2, len=20)

    c_string_copy(dst=main_buffer, src=buf1)

    main_buffer_length = c_string_length(main_buffer)

    main_buffer[main_buffer_length] = ' '
    main_buffer[main_buffer_length + 1] = '\0'

    c_string_concat(dst=main_buffer, src=buf2)
```

The exploit surface should be the combination of `strncpy(3)`, `strcpy(3)` and `strcat(3)`.

`strncpy(3)` does not append a NUL to the destination if the source contains more than `n - 1` non-NUL characters.  For example:

```c
char buffer[] = "AAAAAAAAA";

strncpy(buffer, "hello", 5);
printf("|%s|\n", buffer); // prints |helloAAAA|
```

It's not as safe as it might appear.

In our case, the first and second buffer in the `pp` stack frame are contiguous.  Having two 20-byte buffers is literally the same thing as having one 40-byte buffer:

```c
void two_buffers()
{
    char buf1[20];
    char buf2[20];

    puts(buf1);
    puts(buf2);
}

void one_buffer()
{
    char buf[40];

    puts(&buf[0]);
    puts(&buf[20]);
}
```

In this example, the two functions do exactly the same thing.  And because C strings are NUL terminated, having 20 non-NUL bytes in `buf1` then 5 non-NUL bytes and 1 NUL in `buf2` will make a 25-byte C string.

Here is what the stack frame of `main` should look like:

```
| buffer    | 42 |  0 |
| alignment |  8 | 42 |
| ebp       |  4 | 50 |
| eip       |  4 | 54 |
| <end>     |  0 | 58 |
```

XXX this does not align with working payload, calculations are wrong
