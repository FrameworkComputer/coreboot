/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* IDT 92HD95 */
	0x111d7695, /* Vendor ID */
	0xf1110002, /* Subsystem ID */
	9, /* Number of entries */
	AZALIA_SUBVENDOR(0, 0xf1110002),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x0a, 0x04214040),
	AZALIA_PIN_CFG(0, 0x0b, 0x04a19020),
	AZALIA_PIN_CFG(0, 0x0c, 0x40f000f0),
	AZALIA_PIN_CFG(0, 0x0d, 0x90134110),
	AZALIA_PIN_CFG(0, 0x0e, 0x90a31130),
	AZALIA_PIN_CFG(0, 0x0f, 0x40f000f0),
	AZALIA_PIN_CFG(0, 0x18, 0x40f000f0),

	/* Intel Alderlake-P HDMI */
	0x8086281c, /* Vendor ID */
	0x80860101, /* Subsystem ID */
	10, /* Number of entries */
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),
};

const u32 pc_beep_verbs[] = {};

AZALIA_ARRAY_SIZES;
