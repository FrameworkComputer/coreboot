/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <mainboard/framework/common/device_switches.h>
#include <option.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

/*
 * Disable the whole TCSS (USB-C) block in FSP-M when the "USB-C Ports" CFR
 * option is turned off. These are the master controller/port enables that FSP-M
 * commits in romstage; the FSP-S per-port enables and the OS-visible devices are
 * handled separately in mb_devtree_update() (see devtree.c).
 */
static void mainboard_disable_typec(FSPM_UPD *mupd)
{
	if (get_uint_option(TYPEC_OPTION_NAME, 1) != 0)
		return;

	mupd->FspmConfig.TcssXhciEn = 0;
	mupd->FspmConfig.TcssXdciEn = 0;
	mupd->FspmConfig.TcssDma0En = 0;
	mupd->FspmConfig.TcssDma1En = 0;
	mupd->FspmConfig.UsbTcPortEnPreMem = 0;
	mupd->FspmConfig.TcssItbtPcie0En = 0;
	mupd->FspmConfig.TcssItbtPcie1En = 0;
	mupd->FspmConfig.TcssItbtPcie2En = 0;
	mupd->FspmConfig.TcssItbtPcie3En = 0;
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR5,
		.ect = true,
		.UserBd = BOARD_TYPE_ULT_ULX,
		.ddr_config = {
			.dq_pins_interleaved = false,
		},
	};

	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
			[1] = { .addr_dimm[0] = 0x52, },
		},
	};

	const bool half_populated = false;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);

	mainboard_disable_typec(mupd);
}
