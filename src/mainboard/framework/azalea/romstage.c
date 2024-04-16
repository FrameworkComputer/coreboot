/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/platform_descriptors.h>

#include "mb_gpio.h"

void mb_pre_fspm(FSP_M_CONFIG *mcfg)
{
	romstage_program_gpios();
}
