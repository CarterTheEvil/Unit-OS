#include "timer.h"

#include <stdint.h>


/* -------------------------------------------------
   PIT ports
   ------------------------------------------------- */

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40


/* -------------------------------------------------
   Timer state
   ------------------------------------------------- */

static volatile uint64_t timer_ticks = 0;

static uint32_t timer_frequency = 100;


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
   Read byte from hardware port
   ------------------------------------------------- */

static inline uint8_t inb(
    uint16_t port
)
{
    uint8_t value;

    asm volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}


/* -------------------------------------------------
   Initialize PIT
   ------------------------------------------------- */

void timer_init(uint32_t frequency)
{
    /*
     * Don't allow an invalid frequency.
     */

    if (frequency == 0)
        frequency = 100;


    timer_frequency = frequency;


    /*
     * Calculate PIT divisor.
     */

    uint32_t divisor =
        PIT_FREQUENCY / frequency;


    /*
     * PIT command:
     *
     * 00 = Channel 0
     * 11 = Access low byte/high byte
     * 010 = Square wave generator
     * 0 = Binary mode
     */

    uint8_t command = 0x36;


    outb(
        PIT_COMMAND,
        command
    );


    /*
     * Send divisor low byte.
     */

    outb(
        PIT_CHANNEL0,
        divisor & 0xFF
    );


    /*
     * Send divisor high byte.
     */

    outb(
        PIT_CHANNEL0,
        (divisor >> 8) & 0xFF
    );
}


/* -------------------------------------------------
   Timer interrupt handler
   ------------------------------------------------- */

void timer_handler(void)
{
    timer_ticks++;
}


/* -------------------------------------------------
   Get timer ticks
   ------------------------------------------------- */

uint64_t timer_get_ticks(void)
{
    return timer_ticks;
}


/* -------------------------------------------------
   Get uptime in seconds
   ------------------------------------------------- */

uint32_t timer_get_seconds(void)
{
    if (timer_frequency == 0)
        return 0;


    return timer_ticks / timer_frequency;
}


/* -------------------------------------------------
   Get uptime in milliseconds
   ------------------------------------------------- */

uint64_t timer_get_milliseconds(void)
{
    if (timer_frequency == 0)
        return 0;


    return
        (timer_ticks * 1000ULL)
        / timer_frequency;
}


/* -------------------------------------------------
   Sleep
   ------------------------------------------------- */

void timer_sleep(uint32_t milliseconds)
{
    uint64_t start =
        timer_get_milliseconds();


    while (
        timer_get_milliseconds() -
        start <
        milliseconds
    )
    {
        /*
         * Let the CPU wait for an interrupt
         * instead of constantly burning cycles.
         */

        asm volatile ("hlt");
    }
}
