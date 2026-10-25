# Laptop61 ZMK starter (build-validation scaffold)

This is a preliminary ZMK shield scaffold for a single-piece 61-key keyboard using a Seeed XIAO nRF52840 Plus.

IMPORTANT:
- This is a build-validation scaffold, not a final HHKB layout or tested firmware.
- The 61 matrix-transform positions are assigned in simple row-major order only to validate configuration/build structure. The physical key-to-matrix mapping must be finalized before PCB routing.
- The GPIO allocation is a candidate based on public XIAO ZMK examples and must be checked against the exact XIAO nRF52840 Plus schematic and PCB wiring before connecting hardware.
- Matrix diode direction is provisionally `row2col`; the PCB diode orientation must match or this setting must be changed.
- Battery ADC and always-on RGB battery indication are intentionally not included yet. The XIAO Plus battery sensing circuit requires a separate safe implementation and validation.

## Use
1. Create a repository from https://github.com/zmkfirmware/unified-zmk-config-template.
2. Copy the files/folders in this archive into the repository, preserving paths.
3. Commit/push to GitHub.
4. Open the repository's Actions tab and inspect the build job. Do not flash the output until the build result and pin assignment are reviewed.

Board identifier `seeeduino_xiao_ble` is used because public XIAO nRF52840 Plus ZMK projects use this board target. If the current template/toolchain rejects it, check the build log before changing the target.
