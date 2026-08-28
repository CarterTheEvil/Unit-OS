#include "rendering.h"

#include <stdint.h>

/*
 * VGA text-mode memory starts at 0xB8000.
 *
 * Each character takes two bytes:
 *
 * byte 0 = character
 * byte 1 = color
 */

static volatile uint16_t *const VGA_MEMORY =
    (volatile uint16_t *)0xB8000;

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static uint8_t current_color =
    (VGA_LIGHT_GREY | (VGA_BLACK << 4));


/* Create a VGA color byte */
static uint8_t make_color(uint8_t foreground, uint8_t background)
{
    return foreground | (background << 4);
}


/* Create a VGA screen entry */
static uint16_t make_entry(char character, uint8_t color)
{
    return (uint16_t)character | ((uint16_t)color << 8);
}


/* Update the hardware cursor */
static void update_cursor(void)
{
    uint16_t position =
        cursor_y * VGA_WIDTH + cursor_x;

    /* VGA controller index register */
    asm volatile (
        "outb %0, %1"
        :
        : "a"((uint8_t)0x0F), "Nd"((uint16_t)0x3D4)
    );

    asm volatile (
        "outb %0, %1"
        :
        : "a"((uint8_t)(position & 0xFF)),
          "Nd"((uint16_t)0x3D5)
    );

    asm volatile (
        "outb %0, %1"
        :
        : "a"((uint8_t)0x0E), "Nd"((uint16_t)0x3D4)
    );

    asm volatile (
        "outb %0, %1"
        :
        : "a"((uint8_t)((position >> 8) & 0xFF)),
          "Nd"((uint16_t)0x3D5)
    );
}


/* Scroll the screen up one line */
static void scroll(void)
{
    if (cursor_y < VGA_HEIGHT)
        return;

    for (uint8_t y = 1; y < VGA_HEIGHT; y++)
    {
        for (uint8_t x = 0; x < VGA_WIDTH; x++)
        {
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] =
                VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }

    /* Clear last line */
    for (uint8_t x = 0; x < VGA_WIDTH; x++)
    {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
            make_entry(' ', current_color);
    }

    cursor_y = VGA_HEIGHT - 1;
}


/* Initialize rendering */
void rendering_init(void)
{
    cursor_x = 0;
    cursor_y = 0;

    current_color =
        make_color(VGA_LIGHT_GREY, VGA_BLACK);

    rendering_clear();
}


/* Clear screen */
void rendering_clear(void)
{
    for (uint8_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (uint8_t x = 0; x < VGA_WIDTH; x++)
        {
            VGA_MEMORY[y * VGA_WIDTH + x] =
                make_entry(' ', current_color);
        }
    }

    cursor_x = 0;
    cursor_y = 0;

    update_cursor();
}


/* Change text color */
void rendering_set_color(uint8_t foreground, uint8_t background)
{
    current_color =
        make_color(foreground, background);
}


/* Set cursor position */
void rendering_set_cursor(uint8_t x, uint8_t y)
{
    if (x >= VGA_WIDTH)
        x = VGA_WIDTH - 1;

    if (y >= VGA_HEIGHT)
        y = VGA_HEIGHT - 1;

    cursor_x = x;
    cursor_y = y;

    update_cursor();
}


/* Print one character */
void rendering_putchar(char c)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;

        scroll();
        update_cursor();
        return;
    }

    if (c == '\r')
    {
        cursor_x = 0;
        update_cursor();
        return;
    }

    if (c == '\b')
    {
        if (cursor_x > 0)
        {
            cursor_x--;

            VGA_MEMORY[
                cursor_y * VGA_WIDTH + cursor_x
            ] = make_entry(' ', current_color);
        }

        update_cursor();
        return;
    }

    VGA_MEMORY[
        cursor_y * VGA_WIDTH + cursor_x
    ] = make_entry(c, current_color);

    cursor_x++;

    if (cursor_x >= VGA_WIDTH)
    {
        cursor_x = 0;
        cursor_y++;

        scroll();
    }

    update_cursor();
}


/* Print string */
void rendering_print(const char *str)
{
    while (*str)
    {
        rendering_putchar(*str);
        str++;
    }
}


/* Print string + newline */
void rendering_println(const char *str)
{
    rendering_print(str);
    rendering_putchar('\n');
}
