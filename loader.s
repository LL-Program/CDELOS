global loader
extern kmain

MAGIC_NUMBER equ 0x1BADB002
FLAGS equ 0x0 
CHECKSUM equ -MAGIC_NUMBER

section .text:
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM


loader:
    ; has physical address of multiboot information
    ; set by bootloader
    push ebx
    ; set by gnu multiboot, eax must contain 0x2BADB002
    push eax
    call kmain

.loop:
    jmp .loop
