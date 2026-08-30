BITS 32


; =================================================
; Unit-OS Boot Kernel
; 32-bit x86
; GRUB Multiboot v1
; =================================================


; =================================================
; Multiboot header
; =================================================

section .multiboot

align 4

multiboot_header:

    dd 0x1BADB002

    dd 0x00000003

    dd -(0x1BADB002 + 0x00000003)


; =================================================
; Kernel code
; =================================================

section .text

global _start

global idt_flush

global timer_isr

global keyboard_isr


extern kernel_main

extern timer_handler

extern keyboard_handler


; =================================================
; Kernel entry point
; =================================================

_start:

    cli


    ; ---------------------------------------------
    ; Save Multiboot information
    ; ---------------------------------------------

    mov esi, eax
    mov edi, ebx


    ; ---------------------------------------------
    ; Load our GDT
    ; ---------------------------------------------

    lgdt [gdt_descriptor]


    ; ---------------------------------------------
    ; Reload segment registers
    ; ---------------------------------------------

    jmp 0x08:.reload_segments


.reload_segments:

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax


    ; ---------------------------------------------
    ; Setup kernel stack
    ; ---------------------------------------------

    mov esp, stack_top


    ; ---------------------------------------------
    ; Call C kernel
    ;
    ; kernel_main(
    ;     multiboot_magic,
    ;     multiboot_info
    ; )
    ; ---------------------------------------------

    push edi
    push esi

    call kernel_main


    ; ---------------------------------------------
    ; Kernel should never return
    ; ---------------------------------------------

.hang:

    cli

    hlt

    jmp .hang


; =================================================
; Load IDT
; =================================================

idt_flush:

    mov eax, [esp + 4]

    lidt [eax]

    ret


; =================================================
; PIT Timer IRQ0
; =================================================

timer_isr:

    pusha


    ; Call C timer handler

    call timer_handler


    ; ---------------------------------------------
    ; Send End Of Interrupt to master PIC
    ; ---------------------------------------------

    mov al, 0x20

    out 0x20, al


    popa

    iretd


; =================================================
; PS/2 Keyboard IRQ1
; =================================================

keyboard_isr:

    pusha


    ; Call C keyboard handler

    call keyboard_handler


    ; ---------------------------------------------
    ; Send End Of Interrupt to master PIC
    ; ---------------------------------------------

    mov al, 0x20

    out 0x20, al


    popa

    iretd


; =================================================
; GDT
; =================================================

section .data


gdt_start:


; ---------------------------------------------
; Null descriptor
; ---------------------------------------------

gdt_null:

    dq 0


; ---------------------------------------------
; Code segment
; ---------------------------------------------

gdt_code:

    dw 0xFFFF

    dw 0

    db 0

    db 10011010b

    db 11001111b

    db 0


; ---------------------------------------------
; Data segment
; ---------------------------------------------

gdt_data:

    dw 0xFFFF

    dw 0

    db 0

    db 10010010b

    db 11001111b

    db 0


gdt_end:


; ---------------------------------------------
; GDT descriptor
; ---------------------------------------------

gdt_descriptor:

    dw gdt_end - gdt_start - 1

    dd gdt_start


; =================================================
; Kernel stack
; =================================================

section .bss

align 16


stack_bottom:

    resb 16384


stack_top:
