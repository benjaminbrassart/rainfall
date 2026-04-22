# level0

Pull the binary:

```sh
sshpass -plevel0 scp level0@rainfall:level0 .
```

Using Ghidra, we can get a reconstitution of what the C code would have looked like from the machine code of the binary.  See `source.c` for the source code reconstitution.

There are two interesting things in this program.  First, we see that the password is `423`.  Second, we see `setresuid` and `setresgid` which respectively set real, effective and saved UID/GID for the program.  The program sets them to effective UID and GID.  Combined with the setuid and setgid bits in the program file's permissions, this effectively changes the user to the owner of the file, i.e. `level1`.

```sh
./level0 423
```

We now get a shell as `level1:level1`.

```sh
cat ~level1/.pass
```

We get `1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a`.
