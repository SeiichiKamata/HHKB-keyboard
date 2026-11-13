/*
 * HHKB-keyboard USB-power indicator for the XIAO nRF52840 Plus.
 *
 * External RGB LED (common cathode, active HIGH), actual PCB wiring:
 *   D17 / P1.03 = red, D18 / P1.05 = green, D19 / P1.07 = blue.
 *
 * VBUS absent: blue (B 21% PWM).
 * VBUS present: green (G 21% PWM), INCLUDING when charging is complete.
 * ZMK deep sleep: LED off. When awake, check VBUS once a second.
 *
 * NOTE: The XIAO onboard RGB LED uses P0.26/P0.30/P0.06 and is not
 * controlled by this module. The external LED uses separate GPIO pins.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <nrf.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zmk/activity.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>

#define LED_PWM_PERCENT 21U
#define USB_POLL_INTERVAL K_SECONDS(1)

static const struct pwm_dt_spec led_red =
    PWM_DT_SPEC_GET(DT_NODELABEL(pwm_led_red));
static const struct pwm_dt_spec led_green =
    PWM_DT_SPEC_GET(DT_NODELABEL(pwm_led_green));
static const struct pwm_dt_spec led_blue =
    PWM_DT_SPEC_GET(DT_NODELABEL(pwm_led_blue));

static bool led_sleeping;

static bool usb_vbus_present(void)
{
    /* Detect USB 5 V, not CHG on P0.17: CHG changes at full charge. */
    return (NRF_POWER->USBREGSTATUS & POWER_USBREGSTATUS_VBUSDETECT_Msk) != 0U;
}

static int set_channel(const struct pwm_dt_spec *channel, bool on)
{
    uint32_t pulse = on ? (channel->period / 100U) * LED_PWM_PERCENT : 0U;
    return pwm_set_pulse_dt(channel, pulse);
}

static int set_led(bool red, bool green, bool blue)
{
    int ret;

    ret = set_channel(&led_red, red);
    if (ret < 0) {
        return ret;
    }

    ret = set_channel(&led_green, green);
    if (ret < 0) {
        return ret;
    }

    return set_channel(&led_blue, blue);
}

static int update_led(void)
{
    if (led_sleeping) {
        return set_led(false, false, false);
    }

    if (usb_vbus_present()) {
        return set_led(false, true, false); /* USB connected: green */
    }

    return set_led(false, false, true); /* USB disconnected: blue */
}

static void usb_led_work_handler(struct k_work *work);
K_WORK_DELAYABLE_DEFINE(usb_led_work, usb_led_work_handler);

static void usb_led_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    if (!led_sleeping) {
        (void)update_led();
        (void)k_work_reschedule(&usb_led_work, USB_POLL_INTERVAL);
    }
}

static int usb_led_init(void)
{
    if (!device_is_ready(led_red.dev) || !device_is_ready(led_green.dev) ||
        !device_is_ready(led_blue.dev)) {
        return -ENODEV;
    }

    int ret = update_led();
    if (ret < 0) {
        return ret;
    }

    (void)k_work_schedule(&usb_led_work, USB_POLL_INTERVAL);
    return 0;
}

SYS_INIT(usb_led_init, APPLICATION, 95);

static int usb_led_activity_listener(const zmk_event_t *eh)
{
    if (as_zmk_activity_state_changed(eh) == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (zmk_activity_get_state() == ZMK_ACTIVITY_SLEEP) {
        led_sleeping = true;
        (void)k_work_cancel_delayable(&usb_led_work);
        (void)set_led(false, false, false);
    } else {
        led_sleeping = false;
        (void)update_led();
        (void)k_work_reschedule(&usb_led_work, USB_POLL_INTERVAL);
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(usb_led, usb_led_activity_listener);
ZMK_SUBSCRIPTION(usb_led, zmk_activity_state_changed);
