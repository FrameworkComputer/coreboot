/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <ec/ec.h>
#include <mainboard/ramstage.h>
#include <memory_info.h>
#include <smbios.h>
#include <soc/ramstage.h>
#include <stdio.h>

/*
 * Marigold has two DDR5 SODIMM slots, each on its own memory controller.
 * FSP reports both as ChannelId 0 / DimmId 0, so the generic locator (which
 * only uses channel_num and dimm_num) labels both "Channel-0-DIMM-0". Fold in
 * ctrlr_num to give each DIMM a distinct locator.
 */
void smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	char locator[40];

	snprintf(locator, sizeof(locator), "Controller%d-Channel%d-DIMM%d",
		dimm->ctrlr_num, dimm->channel_num, dimm->dimm_num);
	t->device_locator = smbios_add_string(t->eos, locator);

	snprintf(locator, sizeof(locator), "BANK %d", dimm->bank_locator);
	t->bank_locator = smbios_add_string(t->eos, locator);
}

void smbios_fill_dimm_asset_tag(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	char buf[40];

	snprintf(buf, sizeof(buf), "Controller%d-Channel%d-DIMM%d-AssetTag",
		dimm->ctrlr_num, dimm->channel_num, dimm->dimm_num);
	t->asset_tag = smbios_add_string(t->eos, buf);
}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/*
	 * Disable C10 reporting over eSPI to prevent LED flicker
	 * during S0ix entry/exit on MTL laptops.
	 */
	params->PchEspiHostC10ReportEnable = 0;

	/* Auto-notify iGPU of display changes on USB-C (for hotplug) */
	params->TcNotifyIgd = 2;
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
}

struct chip_operations mainboard_ops = {.init = mainboard_init};
