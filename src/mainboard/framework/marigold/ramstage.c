/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <ec/ec.h>
#include <mainboard/ramstage.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/*
	 * Disable C10 reporting over eSPI to prevent LED flicker
	 * during S0ix entry/exit on MTL laptops.
	 */
	params->PchEspiHostC10ReportEnable = 0;

	/* CNVi RF_RESET# and CLKREQ are on GPP_F4/F5, not the default GPP_A8/A9 */
	params->CnviRfResetPinMux = 0x194CE404;
	params->CnviClkreqPinMux = 0x394CE605;

	/* Auto-notify iGPU of display changes on USB-C (for hotplug) */
	params->TcNotifyIgd = 2;
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
}

struct chip_operations mainboard_ops = {.init = mainboard_init};
