#include <stdint.h>


/* =================================================
   IDT Entry
   ================================================= */

struct idt_entry
{
    uint16_t base_low;

    uint16_t selector;

    uint8_t zero;

    uint8_t flags;

    uint16_t base_high;
} __attribute__((packed));


/* =================================================
   IDT Pointer
   ================================================= */

struct idt_ptr
{
    uint16_t limit;

    uint32_t base;
} __attribute__((packed));


/* =================================================
   IDT
   ================================================= */

static struct idt_entry idt[256];

static struct idt_ptr idt_pointer;


/* =================================================
   Assembly interrupt handlers
   ================================================= */

extern void idt_flush(uint32_t address);

extern void timer_isr(void);

extern void keyboard_isr(void);


/* =================================================
   Port I/O
   ================================================= */

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


/* =================================================
   Set an IDT entry
   ================================================= */

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


/* =================================================
   Remap the PIC
   ================================================= */

static void pic_remap(void)
{
    /*
     * Start PIC initialization.
     */

    outb(0x20, 0x11);
    outb(0xA0, 0x11);


    /*
     * Set interrupt offsets.
     *
     * Master:
     * IRQ0 -> 0x20
     *
     * Slave:
     * IRQ8 -> 0x28
     */

    outb(0x21, 0x20);
    outb(0xA1, 0x28);


    /*
     * Tell master about slave.
     */

    outb(0x21, 0x04);

    outb(0xA1, 0x02);


    /*
     * 8086 mode.
     */

    outb(0x21, 0x01);

    outb(0xA1, 0x01);


    /*
     * Initially mask all interrupts.
     */

    outb(0x21, 0xFF);

    outb(0xA1, 0xFF);
}


/* =================================================
   Initialize interrupts
   ================================================= */

void interrupts_init(void)
{
    /*
     * Setup IDT pointer.
     */

    idt_pointer.limit =
        sizeof(idt) - 1;

    idt_pointer.base =
        (uint32_t)&idt;


    /*
     * Clear IDT.
     */

    for (int i = 0; i < 256; i++)
    {
        idt[i].base_low = 0;

        idt[i].selector = 0;

        idt[i].zero = 0;

        idt[i].flags = 0;

        idt[i].base_high = 0;
    }


    /*
     * ---------------------------------------------
     * IRQ0 - PIT Timer
     *
     * PIC IRQ0 was remapped to interrupt 0x20.
     * ---------------------------------------------
     */

    idt_set_gate(
        0x20,
        (uint32_t)timer_isr,
        0x08,
        0x8E
    );


    /*
     * ---------------------------------------------
     * IRQ1 - PS/2 Keyboard
     *
     * PIC IRQ1 was remapped to interrupt 0x21.
     * ---------------------------------------------
     */

    idt_set_gate(
        0x21,
        (uint32_t)keyboard_isr,
        0x08,
        0x8E
    );


    /*
     * Load the IDT.
     */

    idt_flush(
        (uint32_t)&idt_pointer
    );


    /*
     * Remap PIC.
     */

    pic_remap();


    /*
     * ---------------------------------------------
     * Unmask IRQ0 and IRQ1.
     *
     * IRQ0 = Timer
     * IRQ1 = Keyboard
     *
     * Binary:
     *
     * 11111100
     * ||||||||
     * |||||||└─ IRQ0 enabled
     * ||||||└── IRQ1 enabled
     * └─────── Everything else disabled
     * ---------------------------------------------
     */

    outb(0x21, 0xFC);


    /*
     * Keep slave PIC disabled for now.
     */

    outb(0xA1, 0xFF);


    /*
     * Enable CPU interrupts.
     */

    asm volatile ("sti");
}
