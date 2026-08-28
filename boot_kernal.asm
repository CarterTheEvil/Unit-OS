; Unit-OS boot kernel
; GRUB Multiboot v1 compatible
; 32-bit x86

BITS 32

section .multiboot
align 4

multiboot_header:
    dd 0x1BADB002                  ; Multiboot magic
    dd 0x00000003                  ; Flags
    dd -(0x1BADB002 + 0x00000003)  ; Checksum

section .text
global _start
extern kernel_main

_start:
    cli

    ; Set up our stack
    mov esp, stack_top

    ; Pass Multiboot information to C
    ; EAX = magic
    ; EBX = multiboot info structure
    push ebx
    push eax

    call kernel_main

    ; If kernel_main returns, halt forever
.hang:
    cli
    hlt
    jmp .hang


section .bss
align 16

stack_bottom:
    resb 16384                     ; 16 KB stack
stack_top:
