/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <ec/ec.h>
#include <mainboard/ramstage.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/* The SSD hangs off the CPU PCIe root port, mark it as a slot */
	params->CpuPcieRpLtrEnable[0] = 1;
	params->CpuPcieRpSlotImplemented[0] = 1;

	/*
	 * Disable C10 reporting over eSPI. The EC does not consume it and it
	 * only generates extra eSPI traffic on every S0ix entry/exit.
	 */
	params->PchEspiHostC10ReportEnable = 0;
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
}

struct chip_operations mainboard_ops = {.init = mainboard_init};
