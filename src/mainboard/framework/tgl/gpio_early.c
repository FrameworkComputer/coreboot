/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <mainboard/bootblock.h>

static const struct pad_config gpio_table[] = {
	/* UART2 for console */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1), /* UART2_RXD */
	PAD_CFG_NF(GPP_H21, NONE, DEEP, NF1), /* UART2_TXD */
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
