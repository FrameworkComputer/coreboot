/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR4,
		.ect = true,
		/* The board follows Intel's non-interleaved (NIL) DDR4 routing */
		.ddr4_config = {
			.dq_pins_interleaved = false,
		},
	};

	/*
	 * Two DDR4 SO-DIMM sockets, one per memory controller:
	 *   Controller0-ChannelA-DIMM0 (SMBus 0x50)
	 *   Controller1-ChannelA-DIMM0 (SMBus 0x52)
	 */
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
			[1] = { .addr_dimm[0] = 0x52, },
		},
	};

	const bool half_populated = false;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);
}
