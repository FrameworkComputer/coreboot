/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c_bus.h>
#include <stdio.h>
#include <types.h>

#include "chip.h"

#define PAC194X_ACPI_HID		"MCHP1940"
/* Microchip's _DSM UUID for the PAC194x/PAC195x family (DS50003155) */
#define PAC194X_DSM_UUID		"721f1534-5d27-4b60-9df4-41a3c4b7da3a"

/* PRODUCT_ID, MANUFACTURER_ID and REVISION_ID are consecutive registers */
#define PAC194X_PRODUCT_ID_REG		0xfd
#define PAC194X_MANUFACTURER_ID		0x54

#define PAC194X_DEFAULT_ACTIVE_SPS	1024
#define PAC194X_DEFAULT_IDLE_SPS	8
#define PAC194X_DEFAULT_REFRESH_S	900
#define PAC194X_MIN_REFRESH_S		60
#define PAC194X_MAX_REFRESH_S		60000
#define PAC194X_DEFAULT_KFACTOR		1000

static const struct {
	uint8_t product_id;
	const char *name;
} pac194x_parts[] = {
	{ 0x68, "PAC1941" },
	{ 0x69, "PAC1942" },
	{ 0x6a, "PAC1943" },
	{ 0x6b, "PAC1944" },
	{ 0x6c, "PAC1941-2" },
	{ 0x6d, "PAC1942-2" },
	{ 0x78, "PAC1951" },
	{ 0x79, "PAC1952" },
	{ 0x7a, "PAC1953" },
	{ 0x7b, "PAC1954" },
	{ 0x7c, "PAC1951-2" },
	{ 0x7d, "PAC1952-2" },
};

static const char *pac194x_part_name(uint8_t product_id)
{
	for (size_t i = 0; i < ARRAY_SIZE(pac194x_parts); i++)
		if (pac194x_parts[i].product_id == product_id)
			return pac194x_parts[i].name;
	return NULL;
}

static void pac194x_init(struct device *dev)
{
	uint8_t id[3];	/* PRODUCT_ID, MANUFACTURER_ID, REVISION_ID */
	const char *part;

	if (i2c_dev_read_at(dev, id, sizeof(id), PAC194X_PRODUCT_ID_REG) != sizeof(id)) {
		printk(BIOS_INFO, "%s: no device at %s, disabling\n",
		       dev->chip_ops->name, dev_path(dev));
		dev->enabled = 0;
		return;
	}

	part = pac194x_part_name(id[0]);
	if (!part || id[1] != PAC194X_MANUFACTURER_ID)
		printk(BIOS_WARNING, "%s: unknown IDs %02x/%02x/%02x at %s\n",
		       dev->chip_ops->name, id[0], id[1], id[2], dev_path(dev));
	else
		printk(BIOS_INFO, "%s: %s rev 0x%02x at %s\n",
		       dev->chip_ops->name, part, id[2], dev_path(dev));
}

#if CONFIG(HAVE_ACPI_TABLES)

static bool pac194x_channel_used(const struct drivers_i2c_pac194x_config *config,
				 size_t channel)
{
	return config->shunt_uohm[channel] != 0;
}

static bool pac194x_channel_has_emi(const struct drivers_i2c_pac194x_config *config,
				    size_t channel)
{
	return pac194x_channel_used(config, channel) && config->rail_name[channel] &&
	       config->rail_name[channel][0] != '\0';
}

static void pac194x_write_return_package(const uint64_t *values, size_t count)
{
	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(count);
	for (size_t i = 0; i < count; i++)
		acpigen_write_integer(values[i]);
	acpigen_write_package_end();
}

/* Function 1: power rail names, "" for private channels */
static void pac194x_dsm_rail_names(void *arg)
{
	const struct drivers_i2c_pac194x_config *config = ((struct device *)arg)->chip_info;

	acpigen_emit_byte(RETURN_OP);
	acpigen_write_package(PAC194X_MAX_CHANNELS);
	for (size_t i = 0; i < PAC194X_MAX_CHANNELS; i++)
		acpigen_write_string(pac194x_channel_has_emi(config, i) ?
				     config->rail_name[i] : "");
	acpigen_write_package_end();
}

/* Function 2: sense resistor values in micro-ohms, 0 for unused channels */
static void pac194x_dsm_shunts(void *arg)
{
	const struct drivers_i2c_pac194x_config *config = ((struct device *)arg)->chip_info;
	uint64_t values[PAC194X_MAX_CHANNELS];

	for (size_t i = 0; i < PAC194X_MAX_CHANNELS; i++)
		values[i] = config->shunt_uohm[i];
	pac194x_write_return_package(values, ARRAY_SIZE(values));
}

/* Function 3: EMI enable bit-mask, bit 3 = channel 1 ... bit 0 = channel 4 */
static void pac194x_dsm_emi_mask(void *arg)
{
	const struct drivers_i2c_pac194x_config *config = ((struct device *)arg)->chip_info;
	uint64_t mask = 0;

	for (size_t i = 0; i < PAC194X_MAX_CHANNELS; i++)
		if (pac194x_channel_has_emi(config, i))
			mask |= BIT(PAC194X_MAX_CHANNELS - 1 - i);
	pac194x_write_return_package(&mask, 1);
}

/* Function 4: {CFG_VS1..CFG_VS4, CFG_VB1..CFG_VB4} polarity / range codes */
static void pac194x_dsm_input_ranges(void *arg)
{
	const struct drivers_i2c_pac194x_config *config = ((struct device *)arg)->chip_info;
	uint64_t values[2 * PAC194X_MAX_CHANNELS];

	for (size_t i = 0; i < PAC194X_MAX_CHANNELS; i++) {
		values[i] = config->vsense_range[i];
		values[PAC194X_MAX_CHANNELS + i] = config->vbus_range[i];
	}
	pac194x_write_return_package(values, ARRAY_SIZE(values));
}

static uint64_t pac194x_sps(const struct device *dev, unsigned int sps, unsigned int def)
{
	if (!sps)
		return def;

	switch (sps) {
	case 1024:
	case 256:
	case 64:
	case 8:
		return sps;
	default:
		printk(BIOS_WARNING, "%s: invalid sampling rate %u sps, using %u\n",
		       dev_path(dev), sps, def);
		return def;
	}
}

/* Function 5: {active, idle} sampling rate in samples/s */
static void pac194x_dsm_sampling_rates(void *arg)
{
	const struct device *dev = arg;
	const struct drivers_i2c_pac194x_config *config = dev->chip_info;
	const uint64_t values[] = {
		pac194x_sps(dev, config->active_sps, PAC194X_DEFAULT_ACTIVE_SPS),
		pac194x_sps(dev, config->idle_sps, PAC194X_DEFAULT_IDLE_SPS),
	};

	pac194x_write_return_package(values, ARRAY_SIZE(values));
}

/* Function 6: REFRESH watchdog interval in seconds */
static void pac194x_dsm_refresh_interval(void *arg)
{
	const struct device *dev = arg;
	const struct drivers_i2c_pac194x_config *config = dev->chip_info;
	uint64_t interval = config->refresh_interval_s;

	if (!interval) {
		interval = PAC194X_DEFAULT_REFRESH_S;
	} else if (interval < PAC194X_MIN_REFRESH_S || interval > PAC194X_MAX_REFRESH_S) {
		printk(BIOS_WARNING, "%s: invalid refresh interval %llu s, using %u\n",
		       dev_path(dev), interval, PAC194X_DEFAULT_REFRESH_S);
		interval = PAC194X_DEFAULT_REFRESH_S;
	}
	pac194x_write_return_package(&interval, 1);
}

/* Function 7: VBUS multiplication factors, 1000 * Vrail / Vbus, >= 1000 */
static void pac194x_dsm_vbus_kfactors(void *arg)
{
	const struct device *dev = arg;
	const struct drivers_i2c_pac194x_config *config = dev->chip_info;
	uint64_t values[PAC194X_MAX_CHANNELS];

	for (size_t i = 0; i < PAC194X_MAX_CHANNELS; i++) {
		values[i] = config->vbus_kfactor[i];
		if (!values[i]) {
			values[i] = PAC194X_DEFAULT_KFACTOR;
		} else if (values[i] < PAC194X_DEFAULT_KFACTOR) {
			printk(BIOS_WARNING, "%s: invalid VBUS K-factor %llu on channel %zu, using %u\n",
			       dev_path(dev), values[i], i + 1, PAC194X_DEFAULT_KFACTOR);
			values[i] = PAC194X_DEFAULT_KFACTOR;
		}
	}
	pac194x_write_return_package(values, ARRAY_SIZE(values));
}

static void pac194x_fill_ssdt(const struct device *dev)
{
	struct drivers_i2c_pac194x_config *config = dev->chip_info;
	const char *scope = acpi_device_scope(dev);
	void (*dsm_callbacks[])(void *) = {
		NULL,	/* Function 0: generated bit-mask of supported functions */
		pac194x_dsm_rail_names,
		pac194x_dsm_shunts,
		pac194x_dsm_emi_mask,
		pac194x_dsm_input_ranges,
		pac194x_dsm_sampling_rates,
		pac194x_dsm_refresh_interval,
		pac194x_dsm_vbus_kfactors,
	};
	struct acpi_i2c i2c = {
		.address = dev->path.i2c.device,
		.mode_10bit = dev->path.i2c.mode_10bit,
		.speed = config->bus_speed ? config->bus_speed : I2C_SPEED_FAST,
		.resource = scope,
	};

	if (!scope)
		return;

	if (!config->uid)
		printk(BIOS_WARNING, "%s: _UID must be set to a unique value >= 1\n",
		       dev_path(dev));

	acpigen_write_scope(scope);
	acpigen_write_device(acpi_device_name(dev));
	acpigen_write_name_string("_HID", PAC194X_ACPI_HID);
	acpigen_write_name_integer("_UID", config->uid);
	if (config->desc)
		acpigen_write_name_string("_DDN", config->desc);
	/* Lowest D-state supported while the system is in S0 */
	acpigen_write_name_integer("_S0W", ACPI_DEVICE_SLEEP_D3);
	acpigen_write_STA(acpi_device_status(dev));

	acpigen_write_name("_CRS");
	acpigen_write_resourcetemplate_header();
	acpi_device_write_i2c(&i2c);
	acpigen_write_resourcetemplate_footer();

	acpigen_write_dsm(PAC194X_DSM_UUID, dsm_callbacks, ARRAY_SIZE(dsm_callbacks),
			  (void *)dev);

	acpigen_pop_len(); /* Device */
	acpigen_pop_len(); /* Scope */

	printk(BIOS_INFO, "%s: %s at %s\n", acpi_device_path(dev),
	       config->desc ? config->desc : dev->chip_ops->name, dev_path(dev));
}

static const char *pac194x_acpi_name(const struct device *dev)
{
	const struct drivers_i2c_pac194x_config *config = dev->chip_info;
	static char name[ACPI_NAME_BUFFER_SIZE];

	if (config->name)
		return config->name;

	snprintf(name, sizeof(name), "PA%02X", dev->path.i2c.device);
	return name;
}

#endif /* CONFIG(HAVE_ACPI_TABLES) */

static struct device_operations pac194x_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	.init			= pac194x_init,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name		= pac194x_acpi_name,
	.acpi_fill_ssdt		= pac194x_fill_ssdt,
#endif
};

static void pac194x_enable(struct device *dev)
{
	dev->ops = &pac194x_ops;
}

struct chip_operations drivers_i2c_pac194x_ops = {
	.name = "Microchip PAC194x/PAC195x power monitor",
	.enable_dev = pac194x_enable,
};
