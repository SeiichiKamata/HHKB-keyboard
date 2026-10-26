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
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define LED_RED_NODE DT_ALIAS(led_red)
#define LED_GREEN_NODE DT_ALIAS(led_green)
#define LED_BLUE_NODE DT_ALIAS(led_blue)

static const struct gpio_dt_spec led_red =
    GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);

static const struct gpio_dt_spec led_green =
    GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);

static const struct gpio_dt_spec led_blue =
    GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

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

static int set_battery_led_color(enum battery_led_color color)
{
    bool red_on = false;
    bool green_on = false;

    switch (color) {
    case BATTERY_LED_GREEN:
        green_on = true;
        break;

    case BATTERY_LED_YELLOW:
        red_on = true;
        green_on = true;
        break;

    case BATTERY_LED_RED:
        red_on = true;
        break;

    default:
        return -EINVAL;
    }

    int ret = gpio_pin_set_dt(&led_red, red_on);
    if (ret < 0) {
        return ret;
    }

    ret = gpio_pin_set_dt(&led_green, green_on);
    if (ret < 0) {
        return ret;
    }

    return gpio_pin_set_dt(&led_blue, 0);
}

static int battery_led_init(void)
{
    if (!gpio_is_ready_dt(&led_red) ||
        !gpio_is_ready_dt(&led_green) ||
        !gpio_is_ready_dt(&led_blue)) {
        return -ENODEV;
    }

    int ret;

    ret = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    ret = gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    ret = gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    uint8_t battery_level = zmk_battery_state_of_charge();
    enum battery_led_color color = get_battery_color(battery_level);

    ARG_UNUSED(color);

    return set_battery_led_color(color);
}

SYS_INIT(battery_led_init, APPLICATION, 90);
