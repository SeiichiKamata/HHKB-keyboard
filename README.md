# Laptop61 ZMK starter (build-validation scaffold)

This is a preliminary ZMK shield scaffold for a single-piece 61-key keyboard using a Seeed XIAO nRF52840 Plus.

IMPORTANT:
- This is a build-validation scaffold, not a final HHKB layout or tested firmware.
- The 61 matrix-transform positions are assigned in simple row-major order only to validate configuration/build structure. The physical key-to-matrix mapping must be finalized before PCB routing.
- The GPIO allocation is a candidate based on public XIAO ZMK examples and must be checked against the exact XIAO nRF52840 Plus schematic and PCB wiring before connecting hardware.
- Matrix diode direction is provisionally `row2col`; the PCB diode orientation must match or this setting must be changed.
- The battery voltage measurement devicetree node is present but has not been hardware-validated. This revision adds USB-VBUS-based white/green indication for the *external common-cathode RGB LED*; its physical output and power draw still require hardware validation.

## Use
1. Create a repository from https://github.com/zmkfirmware/unified-zmk-config-template.
2. Copy the files/folders in this archive into the repository, preserving paths.
3. Commit/push to GitHub.
4. Open the repository's Actions tab and inspect the build job. Do not flash the output until the build result and pin assignment are reviewed.

Board identifier `seeeduino_xiao_ble` is used because public XIAO nRF52840 Plus ZMK projects use this board target. If the current template/toolchain rejects it, check the build log before changing the target.


## USB power indicator (this revision)

The custom source file `src/battery_led.c` is included by the Zephyr module via
`zephyr/module.yml` (`build.cmake: .`) and `Kconfig`. The `zmk-rgbled-widget` dependency was
removed so two controllers cannot both drive the same RGB pins.

- **No USB VBUS**: external RGB LED white (R/G/B each 21% PWM).
- **USB VBUS present**: external RGB LED green (G 21% PWM), even after charge completion.
- **ZMK deep sleep** (when enabled): external RGB LED off.
- **Active or idle**: VBUS polled every second; change may take about 1 second.

Hardware assumption: *external* 4-pin **common-cathode** LED, connected via
resistors to P0.26 (R), P0.30 (G), P0.06 (B), with cathode connected to GND.
Active-high PWM is intentional. The **XIAO onboard RGB** shares those pins and
is wired active-low, so it may show the inverse colors. This cannot be controlled
independently in firmware. The white balance depends on the physical LED and
series resistors; actual brightness and color need hardware testing.

Source and pin mapping are prepared for ZMK `v0.3` / `seeeduino_xiao_ble` /
`laptop61`. Build on GitHub Actions and **review the build result before
flashing**. A successful build is not proof that the PCB wiring is correct.
The keyboard keymap and matrix map have not been changed in this revision.
