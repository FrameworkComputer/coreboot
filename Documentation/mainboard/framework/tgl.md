# Framework Laptop 13 (11th Gen Intel Core)

## Support Status

This port is currently not officially supported by Framework.
It is a proof of concept for internal evaluation of the technical feasability
of coreboot at Framework.

All mainboards sold to customers shipped with BootGuard enabled. While it is
possible for end users to build the image with publicly available source and
binaries, flashing will not result in a bootable system.

## Specs

- CPU (full processor specs available at <https://ark.intel.com>)
    - 11th Gen Intel Core (Tiger Lake), one of:
      - ???
- EC
    - Microchip ??? with Chrome EC firmware
    - Backlit keyboard, with standard PS/2 keycodes and I2C HID hotkeys
    - Battery
    - Suspend / resume (S0ix, not S3)
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - HDMI video
    - USB-C DisplayPort video
- Memory
    - 2x SODIMM slots for up to ??GB DDR4-??00 each
- Networking
    - AX210 M.2 2230 PCIe Gen? x? WiFi / Bluetooth
- Sound
    - Realtek ALC285 HDA Codec
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - USB-C DisplayPort/HDMI audio
- Built-in devices
  - I2C HID PTP Touchpad
  - I2C HID Mediakeys
  - ISH ALS Sensor
  - Nuvoton ??? TPM
  - Intel RAPL power reporting
  - USB Camera
  - USB Fingerprint reader
  - eSPI PS2 Keyboard
  - eSPI PS2 emulated mouse/touchpad (if OS has no I2C driver)
- Storage
    - M.2 2280 PCIe Gen? x? SSD
- 4 Type-C Ports
    - USB 3.? Gen ?
    - Thunderbolt ?
    - 100W USB PD 3.0 Charging
    - DisplayPort ?.? Alt-Mode

### Build

The following commands will build a working image:

```bash
# Select board
make nconfig

# Build
make distclean
make -j$(nproc)
```

Flashing example with BusPirate 5 connected to socketed ROM:

```
flashrom --ifd -i bios --noverify-all -w build/coreboot.rom --progress -p buspirate_spi:dev=/dev/ttyACM1,serialspeed=115200
```

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | W25Q256JV  |
+---------------------+------------+
| Size                | 32 MiB     |
+---------------------+------------+
| Package             | WSON-8     |
+---------------------+------------+
| Internal flashing   | yes        |
+---------------------+------------+
| External flashing   | yes        |
+---------------------+------------+
```
