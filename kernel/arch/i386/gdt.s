[BITS 32]
section .text
global setGdt

setGdt:
    push    ebp
    mov     ebp, esp

    mov     eax, [ebp + 8]   ; Load pointer to GDTR
    cli
    lgdt    [eax]

    ; Reload data segments (0x10 = GDT[2])
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax

    ; Far jump to reload CS (0x08 = GDT[1])
    jmp     0x08:.flush
.flush:
    nop
    sti
    pop     ebp
    ret
