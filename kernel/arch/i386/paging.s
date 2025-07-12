;  paging.asm — NASM-syntax, single file
;  -------------------------------------
;  void loadPageDirectory(uint32_t *pd_phys);
;  void enablePaging(void);
;
;  Assemble:   nasm -f elf32 paging.asm
;  Link (ELF): ld -m elf_i386 -Ttext 0x1000 -o paging.o paging.asm
;  -------------------------------------

        bits 32                ; we are in 32-bit protected mode
        default rel            ; (optional, keeps RIP-rel happy on 64-bit NASM)

section .text
        global  loadPageDirectory
        global  enablePaging

; ------------------------------------------------------------
;  loadPageDirectory(pd_physical_addr)
;  ------------------------------------------------------------
loadPageDirectory:
        push    ebp
        mov     ebp, esp

        mov     eax, [ebp + 8]     ; first C argument
        mov     cr3, eax           ; CR3 ← physical address of page directory

        pop     ebp
        ret

; ------------------------------------------------------------
;  enablePaging()
;  ------------------------------------------------------------
enablePaging:
        push    ebp
        mov     ebp, esp

        mov     eax, cr0           ; read CR0
        or      eax, 0x80000000    ; set PG (bit 31)
        mov     cr0, eax           ; write back to CR0

        pop     ebp
        ret
