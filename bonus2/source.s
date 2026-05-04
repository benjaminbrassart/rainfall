        .intel_syntax noprefix

        .set LANG_EN, 0
        .set LANG_FI, 1
        .set LANG_NL, 2

        .lcomm language, 4

message_en: # 6 bytes + NUL
        .asciz "Hello "

message_nl: # 13 bytes + NUL
        .asciz "Goedemiddag! "

message_fi: # 18 bytes + NUL
        .asciz "Hyvää päivää "


        .globl main
main:
        push ebp
        mov  ebp, esp

        push edi
        push esi
        push ebx

        and esp, 0xfffffff0
        sub esp, 160

        # argc == 3
        cmp dword ptr [ebp + 8], 3
        je  .L1

        mov eax, 1
        jmp .Lret

.L1:
        # memset(stack[80], 0, 20)
        lea ebx, [esp+80]
        mov eax, 0
        mov edx, 19
        mov edi, edx
        rep stos dword ptr es:[edi], eax

        # strncpy()
        mov  eax, dword ptr [ebp + 12]
        add  eax, 4
        mov  eax, dword ptr [eax]
        mov  dword ptr [esp + 8], 40
        mov  dword ptr [esp + 4], eax
        lea  eax, [esp + 80]
        mov  dword ptr [esp] eax
        call strncpy

        # XXX missing code

.Lret:
        lea esp, [ebp + 12]
        pop ebx
        pop esi
        pop edi
        ret




greetuser:
        push ebp
        mov  ebp, esp

        sub esp, 88
        mov eax, language

        cmp eax, LANG_FI
        je  .Lgreet_fi

        cmp eax, LANG_NL
        je  .Lgreet_nl

        test eax, eax
        jne  .Lend

.Lgreet_en:
        mov   edx, message_en
        lea   eax, [ebp - 72]
        mov   ecx, dword ptr [edx]
        mov   dword ptr [eax], ecx
        movzx ecx, word ptr [edx + 4]
        mov   word ptr [eax + 4], cx
        movzx edx, byte prt [edx + 6]
        mov   byte ptr [eax + 6], dl
        jmp   .Lend

.Lgreet_fi:
        mov   edx, message_fi
        lea   eax, [ebp - 72]
        mov   ecx, dword ptr [edx]
        mov   dword ptr [eax], ecx
        mov   ecx, dword ptr [edx + 4]
        mov   dword ptr [eax + 4], ecx
        mov   ecx, dword ptr [edx + 8]
        mov   dword ptr [eax + 8], ecx
        mov   ecx, dword ptr [edx + 12]
        mov   dword ptr [eax + 12], ecx
        mov   ecx, word ptr [edx + 16]
        movzx word ptr [eax + 16], cx
        mov   edx, byte ptr [edx + 18]
        movzx byte ptr [eax + 18], dl
        jmp   .Lend

.Lgreet_nl:
        mov   edx, message_nl
        lea   eax, [ebp - 72]
        mov   ecx, dword ptr [edx]
        mov   dword ptr [eax], ecx
        mov   ecx, dword ptr [edx + 4]
        mov   dword ptr [eax + 4], ecx
        mov   ecx, dword ptr [edx + 8]
        mov   dword ptr [eax + 8], ecx
        movzx edx, word ptr [edx + 12]
        mov   word ptr [eax + 12], dx
        nop

.Lend:
        lea  eax, [ebp + 8]
        mov  dword ptr [esp + 4], eax
        lea  eax, [ebp - 72]
        mov  dword ptr [esp], eax
        call strcat

        lea  eax, [ebp - 72]
        mov  dword ptr [esp], eax
        call puts

        leave
        ret
