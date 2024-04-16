/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/amd_pci_util.h>
#include <commonlib/helpers.h>
#include <device/device.h>

#include "mb_gpio.h"

/*
 * This controls the device -> IRQ routing.
 *
 */
static const struct fch_irq_routing fch_irq_map[] = {
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

static void mainboard_init(void *chip_info)
{
	mainboard_program_gpios();
}

static void mainboard_enable(struct device *dev)
{
	/* Unmask eSPI IRQ 1 (Keyboard) */
	pm_write32(PM_ESPI_INTR_CTRL, PM_ESPI_DEV_INTR_MASK & ~(BIT(1)));
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
