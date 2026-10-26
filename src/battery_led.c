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

enum battery_led_color {
    BATTERY_LED_RED,
    BATTERY_LED_YELLOW,
    BATTERY_LED_GREEN
};

static enum battery_led_color get_battery_color(uint8_t level)
{
    if (level >= 50) {
        return BATTERY_LED_GREEN;
    }

    if (level >= 20) {
        return BATTERY_LED_YELLOW;
    }

    return BATTERY_LED_RED;
}

static int battery_led_init(void)
{
    uint8_t battery_level = zmk_battery_state_of_charge();

    enum battery_led_color color = get_battery_color(battery_level);

    ARG_UNUSED(color);

    return 0;
}

SYS_INIT(battery_led_init, APPLICATION, 90);
