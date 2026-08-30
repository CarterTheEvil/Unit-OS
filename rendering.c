#include "rendering.h"

#include <stdint.h>


/* VGA text-mode memory */
static volatile uint16_t *const VGA_MEMORY =
    (volatile uint16_t *)0xB8000;


static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static uint8_t current_color;


/* -------------------------------------------------
   Create VGA color
   ------------------------------------------------- */

static uint8_t make_color(
    uint8_t foreground,
    uint8_t background
)
{
    return foreground | (background << 4);
}


/* -------------------------------------------------
   Create VGA character entry
   ------------------------------------------------- */

static uint16_t make_entry(
    char character,
    uint8_t color
)
{
    return (uint16_t)character |
           ((uint16_t)color << 8);
}


/* -------------------------------------------------
   Write byte to hardware port
   ------------------------------------------------- */

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


/* -------------------------------------------------
   Update hardware cursor
   ------------------------------------------------- */

static void update_cursor(void)
{
    uint16_t position =
        cursor_y * VGA_WIDTH + cursor_x;


    /* Cursor low byte */

    outb(0x3D4, 0x0F);
    outb(0x3D5, position & 0xFF);


    /* Cursor high byte */

    outb(0x3D4, 0x0E);
    outb(0x3D5, position >> 8);
}


/* -------------------------------------------------
   Scroll screen
   ------------------------------------------------- */

static void scroll(void)
{
    if (cursor_y < VGA_HEIGHT)
        return;


    /* Move every line up */

    for (uint8_t y = 1; y < VGA_HEIGHT; y++)
    {
        for (uint8_t x = 0; x < VGA_WIDTH; x++)
        {
            VGA_MEMORY[
                (y - 1) * VGA_WIDTH + x
            ] =
                VGA_MEMORY[
                    y * VGA_WIDTH + x
                ];
        }
    }


    /* Clear bottom line */

    for (uint8_t x = 0; x < VGA_WIDTH; x++)
    {
        VGA_MEMORY[
            (VGA_HEIGHT - 1) * VGA_WIDTH + x
        ] =
            make_entry(
                ' ',
                current_color
            );
    }


    cursor_y = VGA_HEIGHT - 1;
}


/* -------------------------------------------------
   Initialize renderer
   ------------------------------------------------- */

void rendering_init(void)
{
    current_color =
        make_color(
            VGA_LIGHT_GREY,
            VGA_BLACK
        );

    rendering_clear();
}


/* -------------------------------------------------
   Clear screen
   ------------------------------------------------- */

void rendering_clear(void)
{
    for (uint8_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (uint8_t x = 0; x < VGA_WIDTH; x++)
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


/* -------------------------------------------------
   Set text color
   ------------------------------------------------- */

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


/* -------------------------------------------------
   Print one character
   ------------------------------------------------- */

void rendering_putchar(char c)
{
    /* New line */

    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;

        scroll();
        update_cursor();

        return;
    }


    /* Carriage return */

    if (c == '\r')
    {
        cursor_x = 0;

        update_cursor();

        return;
    }


    /* Backspace */

    if (c == '\b')
    {
        rendering_backspace();

        return;
    }


    /* Draw character */

    VGA_MEMORY[
        cursor_y * VGA_WIDTH + cursor_x
    ] =
        make_entry(
            c,
            current_color
        );


    cursor_x++;


    /* Move to next line */

    if (cursor_x >= VGA_WIDTH)
    {
        cursor_x = 0;
        cursor_y++;

        scroll();
    }


    update_cursor();
}


/* -------------------------------------------------
   Backspace
   ------------------------------------------------- */

void rendering_backspace(void)
{
    /*
     * Don't backspace past the beginning
     * of the screen.
     */

    if (cursor_x == 0)
        return;


    cursor_x--;


    VGA_MEMORY[
        cursor_y * VGA_WIDTH + cursor_x
    ] =
        make_entry(
            ' ',
            current_color
        );


    update_cursor();
}


/* -------------------------------------------------
   Print string
   ------------------------------------------------- */

void rendering_print(const char *str)
{
    while (*str)
    {
        rendering_putchar(*str);

        str++;
    }
}


/* -------------------------------------------------
   Print string + newline
   ------------------------------------------------- */

void rendering_println(const char *str)
{
    rendering_print(str);

    rendering_putchar('\n');
}
