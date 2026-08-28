#ifndef RENDERING_H
#define RENDERING_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25


enum vga_color
{
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE = 15
};


void rendering_init(void);

void rendering_clear(void);

void rendering_set_color(
    uint8_t foreground,
    uint8_t background
);

void rendering_putchar(char c);

void rendering_print(const char *str);

void rendering_println(const char *str);

void rendering_backspace(void);

#endif
