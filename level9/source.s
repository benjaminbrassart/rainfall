main:
        .set   ARGC, [ebp + 0x8]
        .set   ARGV, DWORD PTR [ebp + 0xc]

        ;; prelude
        push   ebp
        mov    ebp, esp
        push   ebx

        ;; alignment
        and    esp, 0xfffffff0

        ;; 32 bytes of stack
        sub    esp, 32
        .set   N1, DWORD PTR [esp + 0x1c]
        .set   N2, DWORD PTR [esp + 0x18]

        cmp    DWORD PTR [ebp+0x8], 1
        ;; if (argc > 1) {
                jg     0x8048610 ; <main+0x1c>
        ;; } else {
                ;; _exit(1)
                mov    DWORD PTR [esp],0x1
                call   0x80484f0 ; <_exit@plt>
                ;; noreturn
        ;; }

        ;; N1 = new N(5)
        mov    DWORD PTR [esp], 108
        call   0x8048530 ; <operator new(unsigned int)@plt>
        mov    ebx, eax

        ;; N(5) <=> constructor($ebx, 5)
        mov    DWORD PTR [esp+0x4], 5
        mov    DWORD PTR [esp], ebx
        call   0x80486f6 ; <N::N(int)>
        mov    N1, ebx

        ;; N2 = new N(6)
        mov    DWORD PTR [esp], 108
        call   0x8048530 ; <operator new(unsigned int)@plt>
        mov    ebx, eax
        ;; N(5) <=> constructor($ebx, 6)
        mov    DWORD PTR [esp+0x4], 0x6
        mov    DWORD PTR [esp], ebx
        call   0x80486f6 ; <N::N(int)>
        mov    N2, ebx

        ;; stack[20] = N1
        mov    eax, N1
        .set   N1, [esp+0x14]
        mov    DWORD PTR N1, eax

        ;; stack[16] = N2
        mov    eax, N2
        .set   N2, DWORD PTR [esp+0x10]
        mov    DWORD PTR N2, eax

        ;; N1->setAnnotation(ARGV[1])
        mov    eax, ARGV
        add    eax, 0x4
        mov    eax, DWORD PTR [eax]
        mov    DWORD PTR [esp+0x4], eax
        mov    eax, N1
        mov    DWORD PTR [esp], eax
        call   0x804870e ; <N::setAnnotation(char*)>

        ;; $edx = **N2
        mov    eax, N2
        mov    eax, DWORD PTR [eax]
        mov    edx, DWORD PTR [eax]

        ;; ($edx)(N2, N1)
        mov    eax, N2
        mov    DWORD PTR [esp+0x4], eax
        mov    eax, N1
        mov    DWORD PTR [esp], eax
        call   edx

        ;; epilogue
        mov    ebx, DWORD PTR [ebp-0x4] ; restore saved ebx
        leave
        ret
