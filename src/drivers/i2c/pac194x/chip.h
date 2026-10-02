/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_I2C_PAC194X_CHIP_H__
#define __DRIVERS_I2C_PAC194X_CHIP_H__

#include <device/i2c_simple.h>

#define PAC194X_MAX_CHANNELS	4

/*
 * VSENSE / VBUS input range configuration codes, as defined by the
 * Microchip "PAC194X/5X Device Driver User's Guide" (DS50003155).
 */
enum pac194x_input_range {
	PAC194X_RANGE_UNIPOLAR_FSR = 0,
	PAC194X_RANGE_BIPOLAR_FSR = 1,
	PAC194X_RANGE_BIPOLAR_HALF_FSR = 2,
};

/*
 * Microchip PAC194x/PAC195x multi-channel power monitor.
 *
 * The device is described to the OS with the Microchip ACPI _HID "MCHP1940"
 * and the configuration _DSM (UUID 721f1534-5d27-4b60-9df4-41a3c4b7da3a)
 * that both the Windows EMI driver and the Linux pac1944 IIO driver consume.
 *
 * At init the driver reads the product/manufacturer ID registers; if the chip
 * does not answer, the device is disabled and no ACPI node is generated, so
 * the entry is safe on boards where the part is optionally populated.
 */
struct drivers_i2c_pac194x_config {
	/* ACPI device name, "PAxx" from the I2C address if not set */
	const char *name;
	/* Device description (_DDN) */
	const char *desc;
	/* ACPI _UID; must be unique per PAC device in the system, >= 1 */
	unsigned int uid;
	/* I2C bus speed, I2C_SPEED_FAST if not set */
	enum i2c_speed bus_speed;

	/*
	 * Power rail name per channel (_DSM function 1). An unset or empty
	 * name marks a "private" channel without an EMI interface.
	 */
	const char *rail_name[PAC194X_MAX_CHANNELS];
	/* Sense resistor per channel in micro-ohms (_DSM function 2), 0 = unused */
	unsigned int shunt_uohm[PAC194X_MAX_CHANNELS];
	/* VSENSE / VBUS polarity and full scale range per channel (_DSM function 4) */
	enum pac194x_input_range vsense_range[PAC194X_MAX_CHANNELS];
	enum pac194x_input_range vbus_range[PAC194X_MAX_CHANNELS];
	/*
	 * VBUS multiplication factor per channel for low-side (-2) parts whose
	 * VBUS pins sit behind a voltage divider (_DSM function 7):
	 * 1000 * Vrail / Vbus. 0 selects the default of 1000.
	 */
	unsigned int vbus_kfactor[PAC194X_MAX_CHANNELS];

	/* Sampling rate in samples/s while active and idle (_DSM function 5): 1024, 256, 64 or 8 */
	unsigned int active_sps;	/* 0 = 1024 */
	unsigned int idle_sps;		/* 0 = 8 */
	/* REFRESH watchdog interval in seconds, 60..60000 (_DSM function 6), 0 = 900 */
	unsigned int refresh_interval_s;
};

#endif /* __DRIVERS_I2C_PAC194X_CHIP_H__ */
