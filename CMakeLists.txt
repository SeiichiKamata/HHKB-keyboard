# HHKB-keyboard external USB-power RGB LED controller
zephyr_library()
zephyr_library_sources_ifdef(CONFIG_HHKB_USB_LED src/battery_led.c)
