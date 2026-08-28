#include <stdint.h>


struct idt_entry
{
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));


struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));


static struct idt_entry idt[256];

static struct idt_ptr idt_pointer;


extern void idt_flush(uint32_t);


extern void keyboard_isr(void);


static inline void outb(
    uint16_t port,
    uint8_t value
)
{
    asm volatile (
        "outb %0, %1"
        :
        : "a"(value),
          "Nd"(port)
    );
}


static void idt_set_gate(
    uint8_t number,
    uint32_t base,
    uint16_t selector,
    uint8_t flags
)
{
    idt[number].base_low =
        base & 0xFFFF;

    idt[number].selector =
        selector;

    idt[number].zero = 0;

    idt[number].flags =
        flags;

    idt[number].base_high =
        (base >> 16) & 0xFFFF;
}


static void pic_remap(void)
{
    /*
     * Start initialization
     */

    outb(0x20, 0x11);
    outb(0xA0, 0x11);


    /*
     * Remap IRQs
     */

    outb(0x21, 0x20);
    outb(0xA1, 0x28);


    /*
     * Tell master/slave about each other
     */

    outb(0x21, 0x04);
    outb(0xA1, 0x02);


    /*
     * 8086 mode
     */

    outb(0x21, 0x01);
    outb(0xA1, 0x01);


    /*
     * Mask everything initially
     */

    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);
}


void interrupts_init(void)
{
    idt_pointer.limit =
        sizeof(idt) - 1;

    idt_pointer.base =
        (uint32_t)&idt;


    for (int i = 0; i < 256; i++)
    {
        idt[i].base_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].flags = 0;
        idt[i].base_high = 0;
    }


    /*
     * IRQ1 = interrupt 0x21
     */

    idt_set_gate(
        0x21,
        (uint32_t)keyboard_isr,
        0x08,
        0x8E
    );


    idt_flush(
        (uint32_t)&idt_pointer
    );


    pic_remap();


    /*
     * Unmask IRQ1 on master PIC
     */

    outb(0x21, 0xFD);


    /*
     * Enable interrupts
     */

    asm volatile ("sti");
}
