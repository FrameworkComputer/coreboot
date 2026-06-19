# Framework Desktop AMD Ryzen AI 300 (Dogwood)

## Support Status

This port is currently not officially supported by Framework.
It is a proof of concept for internal evaluation of the technical feasability
of coreboot at Framework.

## Specs

- CPU
    - FP11 Strix Halo
- EC
    - Nuvoton NPCX 993 with Framework Chrome EC firmware
    - Suspend / resume (S0ix, not S3)
- GPU
    - AMD Ryzen Graphics
- Memory
    - Soldered LPDDR5x x8
    - Options for 32GB, 64GB, 128GB
- Networking
    - RZ616/RZ717 M.2 2230 PCIe Gen4 x1 WiFi / Bluetooth
- Sound
    - Realtek ALC285 HDA Codec
    - Internal speakers
    - Internal microphone
    - Combined headphone / microphone 3.5-mm jack
    - USB-C DisplayPort/HDMI audio
- Internal Slots
    - M.2 2280 PCIe Gen4 x4 SSD (Port 12~15 / CLK Port 2)
    - M.2 2280 PCIe Gen4 x4 SSD (Port 0~3 / CLK Port 0)
    - PCIe Gen4 x 4 (Port 4~7 / CLK Port 1)
    - M.2 2230 PCIe Gen4 x1 WiFi/BT (Port 10~11 / CLK Port 3, USB Port 5)
- Backside Ports
    - 5G Ethernet (RTL8126, PCIe Gen3 x1, Port 8 / CLK Port 6)
    - HDMI 2.1 (DP1)
    - DP 2.0 (UHBR10) (DP0)
    - DP 2.0 (UHBR10) (DP4)
    - Combo Jack from ALC623 (HP1)
    - Type-C USB4/DP-Altmode
      - USB2 IP0(Port0)
      - USB4 IP0 / DP2
    - Type-C USB4/DP-Altmode
      - USB2 IP1(Port1)
      - USB4 IP1 / DP3
    - 2x USB-A connectors through RTS5424 hub (IP4, Port4)

- 2x Front Type-C Ports
   - GL3590 USB Hub
   - USB3.2 IP3(Port3)
   - USB2.0 IP3(Port3)
- Audio Header
    - HP2/MIC2 from ALC623

#### USB Table

| Controller  | Port  | Type     |
|-------------|-------|----------|
| Controller2 | Port0 | USBC1    |
| Controller3 | Port1 | USBC2    |
| Controller0 | Port2 |          |
| Controller0 | Port3 | USBC Hub |
| Controller1 | Port4 | USBA Hub |
| Controller0 | Port5 | BT       |
| Controller0 | Port6 |          |
| Controller0 | Port7 |          |
| Controller2 | Port0 | USBC1    |
| Controller3 | Port1 | USBC2    |
| Controller0 | Port3 | USBC Hub |
| Controller1 | Port4 | USBA Hub |
| Controller0 | Port5 | BT       |

#### PCIE Port Table

| Port     | Type         | PCIe CLK        |
|----------|--------------|-----------------|
| PCIE #0  | SSD2         | CLK0 & CLKREQ#0 |
| PCIE #1  | SSD2         | CLK0 & CLKREQ#0 |
| PCIE #2  | SSD2         | CLK0 & CLKREQ#0 |
| PCIE #3  | SSD2         | CLK0 & CLKREQ#0 |
| PCIE #4  | PCIe x4 Slot | CLK1 & CLKREQ#1 |
| PCIE #5  | PCIe x4 Slot | CLK1 & CLKREQ#1 |
| PCIE #6  | PCIe x4 Slot | CLK1 & CLKREQ#1 |
| PCIE #7  | PCIe x4 Slot | CLK1 & CLKREQ#1 |
| PCIE #8  | LAN          | CLK6 & CLKREQ#6 |
| PCIE #9  | N/A          | N/A             |
| PCIE #10 | WLAN         | CLK3 & CLKREQ#3 |
| PCIE #11 | WLAN         | CLK3 & CLKREQ#3 |
| PCIE #12 | SSD1         | CLK2 & CLKREQ#2 |
| PCIE #13 | SSD1         | CLK2 & CLKREQ#2 |
| PCIE #14 | SSD1         | CLK2 & CLKREQ#2 |
| PCIE #15 | SSD1         | CLK2 & CLKREQ#2 |

#### Display Table

| Port | Type      |
|------|-----------|
| DP0  | DP Port 1 |
| DP1  | HDMI      |
| DP2  | USBC1     |
| DP3  | USBC2     |
| DP4  | DP Port 2 |

#### I2C Address Table

| Port | Device           | Address   |
|------|------------------|-----------|
| I2C0 | PS8835 Retimer   | 0x10-0x2F |
| I2C0 | PS8835 Retimer   | 0x10-0x2F |
| I2C1 | EC I2C HID       | Unused    |
| I2C2 | RTS5424 USBA Hub | 0x6A      |
| I2C3 | PCIe x4 Slot     |           |

#### GPIOs

| GPIO    | Name/Function        | Device | Level      |
|---------|----------------------|--------|------------|
| GPIO77  | ESPI_CLK             |        | S0_1.8V    |
| GPIO81  | ESPI_DAT0            |        | S0_1.8V    |
| GPIO80  | ESPI_DAT1            |        | S0_1.8V    |
| GPIO68  | ESPI_DAT2            |        | S0_1.8V    |
| GPIO69  | ESPI_DAT3            |        | S0_1.8V    |
| GPIO78  | SSD1_RST#            |        | S5_1.8V    |
| GPIO79  | SSD2_RST#            |        | S5_1.8V    |
| GPIO29  | MEM_ID1              |        | S5_1.8V    |
| GPIO32  | MEM_ID2              |        | S5_1.8V    |
| GPIO143 | UART0 TX             |        | S0_1.8V    |
| GPIO141 | UART0 RX             |        | S0_1.8V    |
| GPIO92  | CLKREQ_PCIE#0        | SSD2   | S0_3.3V    |
| GPIO115 | CLKREQ_PCIE#1        | PCIe   | S0_3.3V    |
| GPIO116 | CLKREQ_PCIE#2        | SSD1   | S0_3.3V    |
| GPIO131 | CLKREQ_PCIE#3        | WLAN   | S0_3.3V    |
| GPIO39  | CLKREQ_PCIE#6        | LAN    | S0_3.3V    |
|         | RTCCLK               |        | S5_1.8V    |
| GPIO16  | `USBC1_2_PD_OC#_APU` |        | S5_1.8V    |
| GPIO17  | `USB_OC1#`           |        | S5_1.8V    |
| GPIO18  | `WLAN_WAKE#_R`       |        | S5_1.8V    |
| GPIO24  | `LAN_WAKE#_R`        |        | S5_1.8V    |
| GPIO145 | I2C0 SCL             |        | S0_1.8V OD |
| GPIO146 | I2C0 SDA             |        | S0_1.8V OD |
| GPIO147 | I2C1 SCL             |        | S0_1.8V OD |
| GPIO148 | I2C1 SDA             |        | S0_1.8V OD |
| GPIO113 | I2C2 SCL             |        | S0_1.8V OD |
| GPIO114 | I2C2 SDA             |        | S0_1.8V OD |
| GPIO19  | I2C3 SCL             |        | S5_1.8V OD |
| GPIO20  | I2C3 SDA             |        | S5_1.8V OD |
| GPIO3   | MEM_ID0 (on PVT!)    |        | S5_1.8V    |
| GPIO6   | ES_SCI#              |        | S5_3.3V    |
| GPIO7   | API_VR_LP#           |        | S5_3.3V    |
| GPIO40  | HUBA_INT#            |        | S5_3.3V    |
| GPIO84  | APU_EC_INT#          |        | S0_1.8V    |
| GPIO85  | APU_EC_INT2#         |        | S0_1.8V    |
| GPIO26  | APU_PCIE0_RST#_R     |        | S5_3.3V    |
| GPIO27  | APU_PCIE1_RST#_R     |        | S5_3.3V    |
|         | EC_RSMRST#           |        | S5_1.8V    |
| GPIO0   | PWR_BTN_L            |        | S5_1.8V    |
|         | PWR_GOOD             |        | S5_1.8V    |
| GPIO1   | SYS_RESET_L          |        | S5_3.3V    |
| GPIO2   | PCIE_WAKE#           |        | S5_1.8V    |
|         | SLP_S3_L             |        | S5_3.3V    |
|         | SLP_S5_L             |        | S5_3.3V    |
| GPIO10  | VDD_MEM_VID0         |        | S5_1.8V    |
| GPIO23  | NC                   |        |            |
| GPIO42  | VDD_MEM_VID1         |        | S5_1.8V    |
|         | AZ_BITLK             | HDA    |            |
|         | AZ_SDIN0             | HDA    |            |
|         | AZ_SDIN1             | HDA    |            |
|         | AZ_SDIN2             | HDA    |            |
|         | AZ_RST_L             | HDA    |            |
|         | AZ_SYNC              | HDA    |            |
|         | AZ_SDOUT             | HDA    |            |

### Memory ID Table

Always 8 soldered down chips of each.
There is no SPD, so memory is configured GPIOs with 10k pullup/down to
1.8ALW_APU or to GND. SPD is stored in APCB configuration binary exctracted from vendor BIOS.
APCB contains the memory strap GPIOs, so they do not need to be read by coreboot.

| Vendor   | Schematic Partnum  | APCB Partnum        | Size  | ID2 | ID1 | ID0 |
|----------|--------------------|---------------------|-------|-----|-----|-----|
| Samsung  | K3KL8L80DM-MGCU    | Same                |  4GB  | 0   | 0   | 0   |
| Samsung  | K3KL9L90DM-MGCU    | Same                |  8GB  | 0   | 0   | 1   |
| Samsung  | K3KLALA0DM-MGCU    | K3KLALA0EM-MGCV     | 16GB  | 0   | 1   | 0   |
| SK Hynix | H58G56CK8BX146     | H58G56CK8BX146N     |  4GB  | 0   | 1   | 1   |
| SK Hynix | H58G56CK8BX146     | H58G66CK8BX147N     |  8GB  | 1   | 0   | 0   |
| SK Hynix | H58G56CK8BX146     | H58G78CK8BX185N     | 16GB  | 1   | 0   | 1   |
| Micron   | MT62F4G32D8DV-0231 | MT62F4G32D8DV-023   | 16GB  | 1   | 1   | 0   |

The APCB also contains validated configs that are **not** wired to a strap in
the table above:

| Vendor | APCB Partnum        | Size | Note                         |
|--------|---------------------|------|------------------------------|
| Micron | MT62F1G32D2DS-023   |  4GB | no matching strap row        |
| Micron | MT62F2G32D4DS-023   |  8GB | no matching strap row        |
| Micron | MT62F4G32D8DV-020   | 16GB | alternate revision of `-023` |

### Build

The following commands build a `build/coreboot.rom` for dogwood:

```bash
make distclean # Note: this will remove your current config, if it exists.
touch .config
./util/scripts/config --enable VENDOR_FRAMEWORK
./util/scripts/config --enable BOARD_FRAMEWORK_DOGWOOD
make olddefconfig
make -j$(nproc)
```

The image produced by the steps above is **not bootable on its own** - it has
no FSP binaries and no payload (the build prints warnings to that effect). To
get a bootable image:

- Add the FSP binaries. The Strix Halo FSP-M/FSP-S blobs are not in the public
  `amd_blobs`; once you have `Fsp_M.fd` / `Fsp_S.fd`, point coreboot at them:

  ```bash
  ./util/scripts/config --enable ADD_FSP_BINARIES
  ./util/scripts/config --set-str FSP_M_FILE /path/to/Fsp_M.fd
  ./util/scripts/config --set-str FSP_S_FILE /path/to/Fsp_S.fd
  ```

- Add a payload, e.g. edk2, to boot an existing SSD with a UEFI-compatible
  Linux/Windows installation:

  ```bash
  ./util/scripts/config --enable PAYLOAD_EDK2
  ```

Re-run `make olddefconfig && make -j$(nproc)` after changing the config.

If you don’t plan on using coreboot’s serial console to collect logs, you might
want to disable it (`./util/scripts/config --disable CONSOLE_SERIAL`). It should
reduce the boot time by several seconds.

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

Flashing example with BusPirate 5 connected to socketed ROM:

```
flashrom --ifd -i bios --noverify-all -w build/coreboot.rom --progress -p buspirate_spi:dev=/dev/ttyACM1,serialspeed=115200
```

## Serial Console

Serial console is available through JECDB header (unpopulated on MP) or Type-C CCD UART on the front back bottom Type-C port.

See [FrameworkDebugger](https://github.com/frameworkComputer/FrameworkDebugger) for details.
