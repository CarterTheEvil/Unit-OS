#ifndef RENDERING_H
#define RENDERING_H

#include <stdint.h>

/* VGA text mode dimensions */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* Screen colors */
enum vga_color {
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

/* Initialize the renderer */
void rendering_init(void);

/* Clear the screen */
void rendering_clear(void);

/* Set text color */
void rendering_set_color(uint8_t foreground, uint8_t background);

/* Put one character on the screen */
void rendering_putchar(char c);

/* Print a string */
void rendering_print(const char *str);

/* Print a string followed by a newline */
void rendering_println(const char *str);

/* Move cursor */
void rendering_set_cursor(uint8_t x, uint8_t y);

#endif
