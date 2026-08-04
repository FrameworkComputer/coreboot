/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

/* TODO: verify which codec is actually fitted on this board */
static const u32 idt_92hd95_verbs[] = {
	AZALIA_SUBVENDOR(0, 0xf1110002),
	AZALIA_RESET(1),
	AZALIA_PIN_CFG(0, 0x0a, 0x04214040),
	AZALIA_PIN_CFG(0, 0x0b, 0x04a19020),
	AZALIA_PIN_CFG(0, 0x0c, 0x40f000f0),
	AZALIA_PIN_CFG(0, 0x0d, 0x90134110),
	AZALIA_PIN_CFG(0, 0x0e, 0x90a31130),
	AZALIA_PIN_CFG(0, 0x0f, 0x40f000f0),
	AZALIA_PIN_CFG(0, 0x18, 0x40f000f0),
};

static const u32 intel_adl_hdmi_verbs[] = {
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

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "IDT 92HD95",
		.vendor_id    = 0x111d7695,
		.subsystem_id = 0xf1110002,
		.address      = 0,
		.verbs        = idt_92hd95_verbs,
		.verb_count   = ARRAY_SIZE(idt_92hd95_verbs),
	},
	{
		.name         = "Intel Alderlake-P HDMI",
		.vendor_id    = 0x8086281c,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_adl_hdmi_verbs,
		.verb_count   = ARRAY_SIZE(intel_adl_hdmi_verbs),
	},
};

AZALIA_ARRAY_SIZES;
