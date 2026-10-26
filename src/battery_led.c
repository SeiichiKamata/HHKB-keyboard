/*
 * HHKB-keyboard
 * Battery RGB LED controller
 *
 * Target:
 * - Green: 50-100%
 * - Yellow: 20-49%
 * - Red: 0-19%
 * - PWM duty cycle: 21%
 * - LED off during sleep
 */
#include <zephyr/kernel.h>
#include <zmk/battery.h>

static int battery_led_init(void)
{
    return 0;
}

SYS_INIT(battery_led_init, APPLICATION, 90);
