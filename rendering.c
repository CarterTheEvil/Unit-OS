#include "rendering.h"

#include <stdint.h>


/* =================================================
   VGA Text Buffer
   ================================================= */

static volatile uint16_t *const VGA_MEMORY =
    (volatile uint16_t *)0xB8000;


/* =================================================
   Cursor
   ================================================= */

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;


/* =================================================
   Current Color
   ================================================= */

static uint8_t current_color;


/* =================================================
   Create VGA Color
   ================================================= */

static uint8_t make_color(
    uint8_t foreground,
    uint8_t background
)
{
    return foreground |
           (background << 4);
}


/* =================================================
   Create VGA Character Entry
   ================================================= */

static uint16_t make_entry(
    char character,
    uint8_t color
)
{
    return (uint16_t)character |
           ((uint16_t)color << 8);
}


/* =================================================
   Port Output
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
   Update Hardware Cursor
   ================================================= */

static void update_cursor(void)
{
    uint16_t position =
        cursor_y * VGA_WIDTH +
        cursor_x;


    /*
     * Cursor low byte.
     */

    outb(
        0x3D4,
        0x0F
    );

    outb(
        0x3D5,
        position & 0xFF
    );


    /*
     * Cursor high byte.
     */

    outb(
        0x3D4,
        0x0E
    );

    outb(
        0x3D5,
        position >> 8
    );
}


/* =================================================
   Scroll Screen
   ================================================= */

static void scroll(void)
{
    /*
     * Nothing to scroll yet.
     */

    if (cursor_y < VGA_HEIGHT)
        return;


    /*
     * Move all lines up one row.
     */

    for (
        uint8_t y = 1;
        y < VGA_HEIGHT;
        y++
    )
    {
        for (
            uint8_t x = 0;
            x < VGA_WIDTH;
            x++
        )
        {
            VGA_MEMORY[
                (y - 1) * VGA_WIDTH + x
            ] =
                VGA_MEMORY[
                    y * VGA_WIDTH + x
                ];
        }
    }


    /*
     * Clear the bottom line.
     */

    for (
        uint8_t x = 0;
        x < VGA_WIDTH;
        x++
    )
    {
        VGA_MEMORY[
            (VGA_HEIGHT - 1) *
            VGA_WIDTH +
            x
        ] =
            make_entry(
                ' ',
                current_color
            );
    }


    /*
     * Move cursor to bottom line.
     */

    cursor_y =
        VGA_HEIGHT - 1;
}


/* =================================================
   Initialize Rendering
   ================================================= */

void rendering_init(void)
{
    current_color =
        make_color(
            VGA_LIGHT_GREY,
            VGA_BLACK
        );

    rendering_clear();
}


/* =================================================
   Clear Screen
   ================================================= */

void rendering_clear(void)
{
    for (
        uint8_t y = 0;
        y < VGA_HEIGHT;
        y++
    )
    {
        for (
            uint8_t x = 0;
            x < VGA_WIDTH;
            x++
        )
        {
            VGA_MEMORY[
                y * VGA_WIDTH + x
            ] =
                make_entry(
                    ' ',
                    current_color
                );
        }
    }


    cursor_x = 0;
    cursor_y = 0;


    update_cursor();
}


/* =================================================
   Set Text Color
   ================================================= */

void rendering_set_color(
    uint8_t foreground,
    uint8_t background
)
{
    current_color =
        make_color(
            foreground,
            background
        );
}


/* =================================================
   Print Character
   ================================================= */

void rendering_putchar(char c)
{
    /*
     * Newline.
     */

    if (c == '\n')
    {
        cursor_x = 0;

        cursor_y++;

        scroll();

        update_cursor();

        return;
    }


    /*
     * Carriage return.
     */

    if (c == '\r')
    {
        cursor_x = 0;

        update_cursor();

        return;
    }


    /*
     * Backspace.
     */

    if (c == '\b')
    {
        rendering_backspace();

        return;
    }


    /*
     * Draw character.
     */

    VGA_MEMORY[
        cursor_y * VGA_WIDTH +
        cursor_x
    ] =
        make_entry(
            c,
            current_color
        );


    cursor_x++;


    /*
     * Move to next line.
     */

    if (cursor_x >= VGA_WIDTH)
    {
        cursor_x = 0;

        cursor_y++;

        scroll();
    }


    update_cursor();
}


/* =================================================
   Backspace
   ================================================= */

void rendering_backspace(void)
{
    /*
     * Don't go backwards past the
     * beginning of the current line.
     */

    if (cursor_x == 0)
        return;


    cursor_x--;


    VGA_MEMORY[
        cursor_y * VGA_WIDTH +
        cursor_x
    ] =
        make_entry(
            ' ',
            current_color
        );


    update_cursor();
}


/* =================================================
   Print String
   ================================================= */

void rendering_print(const char *str)
{
    while (*str)
    {
        rendering_putchar(*str);

        str++;
    }
}


/* =================================================
   Print String + Newline
   ================================================= */

void rendering_println(const char *str)
{
    rendering_print(str);

    rendering_putchar('\n');
}


/* =================================================
   Print Unsigned Number
   ================================================= */

void rendering_print_number(uint32_t number)
{
    char buffer[11];

    int i = 0;


    /*
     * Special case for zero.
     */

    if (number == 0)
    {
        rendering_putchar('0');

        return;
    }


    /*
     * Convert number to characters.
     */

    while (number > 0)
    {
        buffer[i++] =
            '0' + (number % 10);

        number /= 10;
    }


    /*
     * Print backwards because the
     * digits were generated backwards.
     */

    while (i > 0)
    {
        i--;

        rendering_putchar(
            buffer[i]
        );
    }
}
