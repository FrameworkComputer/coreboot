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

	/* Camera: xHCI USB2 port 6 (index 5) */
	if (get_uint_option(CAMERA_OPTION_NAME, 1) == 0)
		cfg->usb2_ports[5].enable = 0;

	/* Fingerprint reader: xHCI USB2 port 7 (index 6) */
	if (get_uint_option(FINGERPRINT_OPTION_NAME, 1) == 0)
		cfg->usb2_ports[6].enable = 0;

	/* Bluetooth: CNVi Bluetooth core (PCI 14.7) */
	if (get_uint_option(BLUETOOTH_OPTION_NAME, 1) == 0)
		DEV_PTR(cnvi_bluetooth)->enabled = 0;

	/* Wi-Fi: onboard CNVi module (PCI 14.3) */
	if (get_uint_option(WIFI_OPTION_NAME, 1) == 0)
		DEV_PTR(cnvi_wifi)->enabled = 0;

	/*
	 * USB-C ports: disable everything routed through the four Type-C
	 * connectors (JUSBC1..4) - USB2, SuperSpeed/DP-alt and Thunderbolt/USB4.
	 * The FSP-M port policy is set to DISABLE in romstage
	 * (mainboard_disable_typec); here we clear the FSP-S per-port enable mask
	 * (config->tcss_ports[].enable) and hide the TCSS devices from the OS. PD
	 * charging is unaffected.
	 */
	if (get_uint_option(TYPEC_OPTION_NAME, 1) == 0) {
		/* USB2 lines of the Type-C connectors (usb2_ports[1..4]) */
		cfg->usb2_ports[1].enable = 0;
		cfg->usb2_ports[2].enable = 0;
		cfg->usb2_ports[3].enable = 0;
		cfg->usb2_ports[4].enable = 0;
		/* SuperSpeed/DP-alt port mask -> FSP-S UsbTcPortEn */
		cfg->tcss_ports[0].enable = 0;
		cfg->tcss_ports[1].enable = 0;
		cfg->tcss_ports[2].enable = 0;
		cfg->tcss_ports[3].enable = 0;
		/* TCSS controllers + Thunderbolt RPs (hide from OS) */
		DEV_PTR(tcss_xhci)->enabled = 0;
		DEV_PTR(tcss_dma0)->enabled = 0;
		DEV_PTR(tcss_dma1)->enabled = 0;
		DEV_PTR(tbt_pcie_rp0)->enabled = 0;
		DEV_PTR(tbt_pcie_rp1)->enabled = 0;
		DEV_PTR(tbt_pcie_rp2)->enabled = 0;
		DEV_PTR(tbt_pcie_rp3)->enabled = 0;
	}
}
