;===================================================================
default rel

;===================================================================
section .rodata
align 32
mask_table:
    ; 0
    db 0FFh
    times 31 db 0

    ; 1
    times 2 db 0FFh
    times 30 db 0

    ; 2
    times 3 db 0FFh
    times 29 db 0

    ; 3
    times 4 db 0FFh
    times 28 db 0

    ; 4
    times 5 db 0FFh
    times 27 db 0

    ; 5
    times 6 db 0FFh
    times 26 db 0

    ; 6
    times 7 db 0FFh
    times 25 db 0

    ; 7
    times 8 db 0FFh
    times 24 db 0

    ; 8
    times 9 db 0FFh
    times 23 db 0

    ; 9
    times 10 db 0FFh
    times 22 db 0

    ; 10
    times 11 db 0FFh
    times 21 db 0

    ; 11
    times 12 db 0FFh
    times 20 db 0

    ; 12
    times 13 db 0FFh
    times 19 db 0

    ; 13
    times 14 db 0FFh
    times 18 db 0

    ; 14
    times 15 db 0FFh
    times 17 db 0

    ; 15
    times 16 db 0FFh
    times 16 db 0

    ; 16
    times 17 db 0FFh
    times 15 db 0

    ; 17
    times 18 db 0FFh
    times 14 db 0

    ; 18
    times 19 db 0FFh
    times 13 db 0

    ; 19
    times 20 db 0FFh
    times 12 db 0

    ; 20
    times 21 db 0FFh
    times 11 db 0

    ; 21
    times 22 db 0FFh
    times 10 db 0

    ; 22
    times 23 db 0FFh
    times 9 db 0

    ; 23
    times 24 db 0FFh
    times 8 db 0

    ; 24
    times 25 db 0FFh
    times 7 db 0

    ; 25
    times 26 db 0FFh
    times 6 db 0

    ; 26
    times 27 db 0FFh
    times 5 db 0

    ; 27
    times 28 db 0FFh
    times 4 db 0

    ; 28
    times 29 db 0FFh
    times 3 db 0

    ; 29
    times 30 db 0FFh
    times 2 db 0

    ; 30
    times 31 db 0FFh
    times 1 db 0

    ; 31
    times 32 db 0FFh

section .text
    global KDS_HM_CmpString

KDS_HM_CmpString:
    ; rdi - (input) string from hash
    ; rsi - (input) out string
    ; dl  - (input) len string from hash

.circle:
    xor eax, eax
    cmp dl, 31
    ja .not_equal

    vmovdqu ymm0, [rdi]
    vmovdqu ymm1, [rsi]

    movzx r8d, dl
    shl r8, 5
    lea r9, [rel mask_table]
    vmovdqu ymm2, [r9 + r8]

    vpxor ymm0, ymm0, ymm1
    vpand  ymm0, ymm0, ymm2

    vptest ymm0, ymm0
    jnz .not_equal

    vzeroupper
    ret

.not_equal:
    mov eax, 1
    vzeroupper
    ret

