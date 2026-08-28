BITS 32

section .multiboot
align 4

multiboot_header:
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)


section .text

global _start
global idt_flush
global keyboard_isr

extern kernel_main
extern keyboard_handler


_start:

    cli

    ; Save Multiboot values
    mov esi, eax
    mov edi, ebx

    ; Load our GDT
    lgdt [gdt_descriptor]

    ; Reload code segment
    jmp 0x08:.reload_segments

.reload_segments:

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Setup stack
    mov esp, stack_top

    ; Call C kernel
    push edi
    push esi

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang


; ------------------------------------------------
; Load IDT
; ------------------------------------------------

idt_flush:

    mov eax, [esp + 4]
    lidt [eax]

    ret


; ------------------------------------------------
; Keyboard IRQ1
; ------------------------------------------------

keyboard_isr:

    pusha

    call keyboard_handler

    popa

    iretd


; ------------------------------------------------
; GDT
; ------------------------------------------------

section .data

gdt_start:

gdt_null:
    dq 0

gdt_code:
    dw 0xFFFF
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0

gdt_data:
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; ------------------------------------------------
; Stack
; ------------------------------------------------

section .bss

align 16

stack_bottom:
    resb 16384

stack_top:
