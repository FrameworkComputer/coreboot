/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <mainboard/framework/common/device_switches.h>
#include <option.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

/*
 * Disable the TCSS (USB-C) block in FSP-M when the "USB-C Ports" CFR option is
 * turned off. These are the master controller/port enables that FSP-M commits
 * in romstage; the FSP-S per-port enables and the OS-visible devices are
 * handled separately in mb_devtree_update() (see devtree.c).
 *
 * The USB4/Thunderbolt UPDs (TcssDma*En, TcssItbtPcie*En) are left alone: this
 * board has no USB4, so the SoC code already derives them as disabled from the
 * devicetree.
 */
static void mainboard_disable_typec(FSPM_UPD *mupd)
{
	if (get_uint_option(TYPEC_OPTION_NAME, 1) != 0)
		return;

	mupd->FspmConfig.TcssXhciEn = 0;
	mupd->FspmConfig.UsbTcPortEnPreMem = 0;
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR5,
		.ect = true,
		.UserBd = BOARD_TYPE_MOBILE,
		.ddr_config = {
			.dq_pins_interleaved = false,
		},
	};

	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
		},
	};

	/* Single SODIMM slot on one physical channel */
	const bool half_populated = true;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);

	mainboard_disable_typec(mupd);
}
