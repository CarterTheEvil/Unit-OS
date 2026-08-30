#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

/*
 * Unit-OS PIT timer
 *
 * The PIT normally runs at approximately
 * 1,193,182 Hz.
 */

#define PIT_FREQUENCY 1193182

/*
 * Initialize the PIT.
 *
 * frequency = desired timer interrupts per second.
 */
void timer_init(uint32_t frequency);

/*
 * Called by the timer interrupt.
 */
void timer_handler(void);

/*
 * Return the number of timer ticks since
 * Unit-OS started.
 */
uint64_t timer_get_ticks(void);

/*
 * Return uptime in seconds.
 */
uint32_t timer_get_seconds(void);

/*
 * Return uptime in milliseconds.
 */
uint64_t timer_get_milliseconds(void);

/*
 * Simple blocking delay.
 *
 * milliseconds = amount of time to wait.
 */
void timer_sleep(uint32_t milliseconds);

#endif
