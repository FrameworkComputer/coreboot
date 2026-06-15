/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <mainboard/bootblock.h>

static const struct pad_config gpio_table[] = {
	/* LPSS UART2 for console (debug header) */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),	/* UART2_RXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),	/* UART2_TXD */

	/* SMBus, needed in romstage to read the DDR4 SO-DIMM SPDs */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),	/* SMBCLK */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),	/* SMBDATA */
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
