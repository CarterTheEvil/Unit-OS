#include "keyboard.h"

#include <stdint.h>


#define KEYBOARD_DATA_PORT 0x60


static volatile int key_buffer[256];

static volatile uint16_t buffer_read = 0;
static volatile uint16_t buffer_write = 0;


static int shift_pressed = 0;
static int caps_lock = 0;
static int extended_key = 0;


static inline uint8_t inb(uint16_t port)
{
    uint8_t value;

    asm volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}


static void buffer_push(int key)
{
    uint16_t next =
        (buffer_write + 1) % 256;

    if (next != buffer_read)
    {
        key_buffer[buffer_write] = key;
        buffer_write = next;
    }
}


int keyboard_getkey(void)
{
    if (buffer_read == buffer_write)
        return 0;

    int key = key_buffer[buffer_read];

    buffer_read =
        (buffer_read + 1) % 256;

    return key;
}


/*
 * US keyboard scan code -> ASCII
 */

static const char normal_map[128] =
{
    0,
    27,

    '1','2','3','4','5','6','7','8','9','0',
    '-','=',
    '\b',

    '\t',
    'q','w','e','r','t','y','u','i','o','p',
    '[',']',

    '\n',

    0,

    'a','s','d','f','g','h','j','k','l',
    ';','\'','`',

    0,

    '\\',

    'z','x','c','v','b','n','m',
    ',','.','/',

    0,

    '*',

    0,

    ' '
};


/*
 * Shifted keyboard characters
 */

static const char shift_map[128] =
{
    0,
    27,

    '!','@','#','$','%','^','&','*','(',')',
    '_','+',
    '\b',

    '\t',

    'Q','W','E','R','T','Y','U','I','O','P',

    '{','}',

    '\n',

    0,

    'A','S','D','F','G','H','J','K','L',

    ':','"','~',

    0,

    '|',

    'Z','X','C','V','B','N','M',

    '<','>','/',

    0,

    '*',

    0,

    ' '
};


void keyboard_handler(void)
{
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);


    /*
     * Extended key
     */

    if (scancode == 0xE0)
    {
        extended_key = 1;
        return;
    }


    /*
     * Key released
     */

    if (scancode & 0x80)
    {
        uint8_t released = scancode & 0x7F;

        if (released == 0x2A ||
            released == 0x36)
        {
            shift_pressed = 0;
        }

        extended_key = 0;

        return;
    }


    /*
     * Extended keys
     */

    if (extended_key)
    {
        extended_key = 0;

        if (scancode == 0x48)
        {
            buffer_push(KEY_UP);
            return;
        }

        if (scancode == 0x50)
        {
            buffer_push(KEY_DOWN);
            return;
        }

        if (scancode == 0x4B)
        {
            buffer_push(KEY_LEFT);
            return;
        }

        if (scancode == 0x4D)
        {
            buffer_push(KEY_RIGHT);
            return;
        }

        return;
    }


    /*
     * Shift
     */

    if (scancode == 0x2A ||
        scancode == 0x36)
    {
        shift_pressed = 1;
        return;
    }


    /*
     * Caps Lock
     */

    if (scancode == 0x3A)
    {
        caps_lock = !caps_lock;
        return;
    }


    /*
     * Ignore invalid scan codes
     */

    if (scancode >= 128)
        return;


    char character;


    /*
     * Letters
     */

    if (
        (scancode >= 0x10 && scancode <= 0x19) ||
        (scancode >= 0x1E && scancode <= 0x26) ||
        (scancode >= 0x2C && scancode <= 0x32)
       )
    {
        character = normal_map[scancode];

        if (caps_lock ^ shift_pressed)
        {
            if (character >= 'a' &&
                character <= 'z')
            {
                character -= 'a' - 'A';
            }
        }
    }
    else
    {
        if (shift_pressed)
            character = shift_map[scancode];
        else
            character = normal_map[scancode];
    }


    if (character != 0)
        buffer_push(character);
}


void keyboard_init(void)
{
    buffer_read = 0;
    buffer_write = 0;

    shift_pressed = 0;
    caps_lock = 0;
    extended_key = 0;
}
