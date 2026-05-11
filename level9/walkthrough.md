# level9

Pull the executable:

```sh
sshpass -f ../level8/flag scp level9@rainfall:level9 .
```

See `source.cpp` for source reconstitution.

This one is simiar to level7.

This program:
- allocates and initializes two `N` objects (`N1` and `N2`)
- calls `N1->setAnnotation(argv[1])`
- calls the function pointer located at offset 0 of `N2` with N1 as parameter (like `N2->func(N1)`)

`N1` is `0x804a008` and `N2` is `0x804a078`.  They are 112 bytes apart.  `sizeof(N)` is 108 bytes, so we have 4 bytes of what is either heap metadata or alignment (doesn't really matter).

The `setAnnotation` member function copies its first argument as a string (with `strlen` + `memcpy`, which is basically the same thing as `strcpy`) into the buffer at offset 4 of the `N` object.

We have an attack surface: `N2` is allocated next to `N1` and `N1->setAnnotation(argv[1])` performs a copy without bounds checking.  We can make a simple heap buffer overflow that will overwrite the address at `&N2[0]`.

Here is a representation of our heap:

```
| Element        | Size | Offset |
+----------------+------+--------+
| N1             |    0 |      0 |
| N1::func       |    4 |      0 |
| N1::annotation |  100 |      4 |
| N1::value      |    4 |    104 |
| metadata       |    4 |    108 |
+----------------+------+--------+
| N2             |    0 |    108 |
| N2::func       |    4 |    108 |
| N2::annotation |  100 |    208 |
| N2::value      |    4 |    212 |
| metadata       |    4 |    216 |
```

Since `N::setAnnotation` writes to `N::annotation`, we need to subtract the offset of `N::annotation` (which is 4) from the difference between `N1` and `N2`.  `112 - 4 = 108`, so we need to write 108 bytes before reaching `N2::func`.  As usual, we will include a shellcode and some padding.

We do have one tiny problem with the function pointer:

```asm
        mov    eax, N2
        mov    eax, DWORD PTR [eax]
        mov    edx, DWORD PTR [eax]

        ;; ($edx)(N2, N1)
        mov    eax, N2
        mov    DWORD PTR [esp+0x4], eax
        mov    eax, N1
        mov    DWORD PTR [esp], eax
        call   edx
```

It is dereferenced twice, so we need two levels of indirection.  Basically, doing `N2::func -> &N2::annotation[0] -> &N2::annotation[4]` should do the trick.

```sh
./level9 "$(cat /tmp/payload-level9.bin)"
cat ~bonus0/.pass
```
