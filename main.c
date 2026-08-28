#include "rendering.h"

void kernel_main(unsigned int magic, unsigned int multiboot_info)
{
    (void)magic;
    (void)multiboot_info;

    rendering_init();

    rendering_println("Welcome to Unit-OS");
    rendering_println("-------------------");
    rendering_println("");
    rendering_print("UnitOS-Main> ");

    while (1)
    {
        asm volatile ("hlt");
    }
}
