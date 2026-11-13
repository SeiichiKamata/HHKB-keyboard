zephyr_library()

zephyr_library_include_directories(${APPLICATION_SOURCE_DIR}/include)

zephyr_library_sources_ifdef(CONFIG_HHKB_USB_LED src/battery_led.c)
