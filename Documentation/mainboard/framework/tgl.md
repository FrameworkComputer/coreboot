# Framework Laptop 13 (11th Gen Intel Core)

## Support Status

This port is currently not officially supported by Framework.
It is a proof of concept for internal evaluation of the technical feasibility
of coreboot at Framework.

All mainboards sold to customers ship with BootGuard enabled. While it is
possible for end users to build the image with publicly available source and
binaries, flashing will not result in a bootable system.

Everything listed under Specs has been tested and works with coreboot, unless
marked as untested or listed below. Testing was done on an i7-1185G7 board with
the Realtek ALC295 codec and a single DDR4 SO-DIMM.

## Not working

- Needs EC enablement
    - I2C HID EC media keys
    - Suspend/Modern Standby

## Specs

- CPU (full processor specs available at <https://ark.intel.com>)
      - 11th Gen Intel Core (Tiger Lake UP3), one of:
          - i5-1135G7, i7-1165G7, i7-1185G7
- EC
    - Microchip MEC1521 with Chrome EC firmware (`hx20` board)
    - Backlit keyboard, with standard PS/2 keycodes and I2C HID hotkeys
    - Battery
    - Suspend / resume (S3/S0ix, untested)
- GPU
    - Intel® Iris® Xe Graphics
    - GOP driver is recommended, VBT is provided
    - eDP panel on DDI A
    - USB-C DisplayPort video on all four ports
- Memory
    - 2x DDR4 SO-DIMM slots (one per memory controller)
- Networking
    - AX201 M.2 2230 CNVi WiFi / Bluetooth (Bluetooth over USB2, untested)
    - The M.2 Key-E slot also has a PCIe x1 lane, for
      discrete cards like the AX210
- Sound
    - IDT 92HD95 HDA Codec, or Realtek ALC295 with an ALC1306 amplifier
      depending on the mainboard revision. Both have a verb table, picked at
      runtime by the codec vendor ID. Only the ALC295 variant is tested.
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - USB-C DisplayPort audio (untested)
- Built-in devices
    - I2C HID PTP Touchpad
    - I2C HID Mediakeys from the EC
    - Optional I2C HID touch panel (Compatible with Laptop 13 Pro panel, untested)
    - Integrated Sensor Hub (ALS Sensor)
    - Nuvoton NPCT750 SPI TPM 2.0
    - Intel RAPL power reporting
    - USB Camera
    - USB Fingerprint reader
    - eSPI PS2 Keyboard
    - eSPI PS2 emulated mouse/touchpad (if OS has no I2C driver)
- Storage
    - M.2 2280 PCIe Gen4 x4 SSD
- 4 fully capable Type-C Ports
    - USB 3.2 Gen 2 (10 Gbps) and USB 2.0
    - USB4 / Thunderbolt 4, one USB4 retimer per port
      Note, the Thunderbolt NVM (firmware) run the Firmware Connection Manager
    - 60W USB PD 3.0 Charging
    - DisplayPort Alt-Mode

### Build

The following commands will build a working image:

```bash
make distclean # Note: this will remove your current config, if it exists.
touch .config
./util/scripts/config --enable VENDOR_FRAMEWORK
./util/scripts/config --enable BOARD_FRAMEWORK_TGL
make olddefconfig
make -j$(nproc)
```

If you don’t plan on using coreboot’s serial console to collect logs, you might want to disable it at this point (./util/scripts/config --disable CONSOLE_SERIAL). It should reduce the boot time by several seconds.

If you would like to boot your existing SSD with UEFI compatible Linux/Windows installation, enable EDK2 Payload (./util/scripts/config --enable PAYLOAD_EDK2).

## Flashing coreboot

```{eval-rst}
+---------------------+------------+
| Type                | Value      |
+=====================+============+
| Socketed flash      | no         |
+---------------------+------------+
| Vendor              | Winbond    |
+---------------------+------------+
| Model               | W25R256JV  |
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

Flashing example with BusPirate 5 connected to the ROM:

```
flashrom --ifd -i bios --noverify-all -w build/coreboot.rom --progress -p buspirate_spi:dev=/dev/ttyACM1,serialspeed=115200
```

## Serial Console

Serial console is available on UART2 (GPP_C20/GPP_C21) through the debug header
on the mainboard.

See [FrameworkDebugger](https://github.com/frameworkComputer/FrameworkDebugger) for details.
