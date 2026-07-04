/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <chip.h>
#include <device/device.h>
#include <devtree_update.h>
#include <mainboard/framework/common/device_switches.h>
#include <option.h>
#include <static.h>
#include <types.h>

/*
 * Apply the CFR "Devices" on/off switches before device enumeration and FSP-S.
 * The option backend returns the fallback (enabled) when no value is stored, so
 * the devicetree defaults are preserved.
 */
void mb_devtree_update(void)
{
	config_t *cfg = config_of_soc();

	/* Camera: xHCI USB2 port 7 (index 6) */
	if (get_uint_option(CAMERA_OPTION_NAME, 1) == 0)
		cfg->usb2_ports[6].enable = 0;

	/* Fingerprint reader: xHCI USB2 port 9 (index 8) */
	if (get_uint_option(FINGERPRINT_OPTION_NAME, 1) == 0)
		cfg->usb2_ports[8].enable = 0;

	/*
	 * Bluetooth: the CNVi Bluetooth core, wired to xHCI USB2 port 10
	 * (index 9). Alder Lake has no cnvi_bluetooth device, so clear the
	 * FSP-S CnviBtCore enable along with the USB2 port.
	 */
	if (get_uint_option(BLUETOOTH_OPTION_NAME, 1) == 0) {
		cfg->cnvi_bt_core = false;
		cfg->usb2_ports[9].enable = 0;
	}

	/*
	 * Wi-Fi: CNVi module in the M.2/E 2230 (JWLAN) slot. Bluetooth rides on
	 * the same CNVi, and the SoC code drops CnviBtCore when CnviMode is off,
	 * so turn the Bluetooth USB2 port off too instead of leaving a port
	 * enabled for a device that can no longer answer.
	 */
	if (get_uint_option(WIFI_OPTION_NAME, 1) == 0) {
		DEV_PTR(cnvi_wifi)->enabled = 0;
		cfg->cnvi_bt_core = false;
		cfg->usb2_ports[9].enable = 0;
	}

	/*
	 * USB-C ports: disable everything routed through the four Type-C
	 * connectors (JUSB1..4) - USB2 and SuperSpeed/DP-alt. This board has no
	 * USB4/Thunderbolt, so there are no TBT root ports or DMA engines to
	 * turn off. The FSP-M controller/port enables are cleared in romstage
	 * (mainboard_disable_typec); here we clear the FSP-S per-port enables
	 * (UsbTcPortEn, derived from the tcss_usb3_portN devices) and hide the
	 * TCSS xHCI controller from the OS. PD charging is unaffected.
	 */
	if (get_uint_option(TYPEC_OPTION_NAME, 1) == 0) {
		/* USB2 lines of the Type-C connectors (usb2_ports[1..4]) */
		cfg->usb2_ports[1].enable = 0;
		cfg->usb2_ports[2].enable = 0;
		cfg->usb2_ports[3].enable = 0;
		cfg->usb2_ports[4].enable = 0;
		/* SuperSpeed/DP-alt ports -> FSP-S UsbTcPortEn */
		DEV_PTR(tcss_usb3_port1)->enabled = 0;
		DEV_PTR(tcss_usb3_port2)->enabled = 0;
		DEV_PTR(tcss_usb3_port3)->enabled = 0;
		DEV_PTR(tcss_usb3_port4)->enabled = 0;
		/* TCSS controller (hide from OS; FSP-M init already skipped) */
		DEV_PTR(tcss_xhci)->enabled = 0;
	}
}
