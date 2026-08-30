#ifndef RENDERING_H
#define RENDERING_H

#include <stdint.h>


/* =================================================
   VGA Configuration
   ================================================= */

#define VGA_WIDTH 80
#define VGA_HEIGHT 25


/* =================================================
   VGA Colors
   ================================================= */

#define VGA_BLACK         0
#define VGA_BLUE          1
#define VGA_GREEN         2
#define VGA_CYAN          3
#define VGA_RED           4
#define VGA_MAGENTA       5
#define VGA_BROWN         6
#define VGA_LIGHT_GREY   7
#define VGA_DARK_GREY    8
#define VGA_LIGHT_BLUE   9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN  11
#define VGA_LIGHT_RED   12
#define VGA_LIGHT_MAGENTA 13
#define VGA_LIGHT_BROWN 14
#define VGA_WHITE        15


/* =================================================
   Initialization
   ================================================= */

void rendering_init(void);


/* =================================================
   Screen control
   ================================================= */

void rendering_clear(void);


/* =================================================
   Text color
   ================================================= */

void rendering_set_color(
    uint8_t foreground,
    uint8_t background
);


/* =================================================
   Character output
   ================================================= */

void rendering_putchar(char c);

void rendering_backspace(void);


/* =================================================
   String output
   ================================================= */

void rendering_print(const char *str);

void rendering_println(const char *str);


/* =================================================
   Number output
   ================================================= */

void rendering_print_number(uint32_t number);

#endif
