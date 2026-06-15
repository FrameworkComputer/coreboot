/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/azalia_device.h>

/*
 * Verb tables transcribed from the vendor firmware (PlatformInitAdvancedPostMem).
 * The vendor installs both analog codec tables unconditionally and the matching
 * one is picked at runtime by the codec vendor ID, so no board ID or strap is
 * involved.
 */

/* Board variant 1: IDT 92HD95 */
static const u32 idt_92hd95_verbs[] = {
	AZALIA_SUBVENDOR(0, 0xf1110001),

	/* Pin widget configurations */
	/* Port-A: Headphone jack (combo) */
	AZALIA_PIN_CFG(0, 0x0a, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
		AZALIA_HP_OUT,
		AZALIA_STEREO_MONO_1_8,
		AZALIA_GREEN,
		AZALIA_JACK_PRESENCE_DETECT,
		4, 0
	)),
	/* Port-B: Microphone jack (combo) */
	AZALIA_PIN_CFG(0, 0x0b, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_RIGHT,
		AZALIA_MIC_IN,
		AZALIA_STEREO_MONO_1_8,
		AZALIA_PINK,
		AZALIA_JACK_PRESENCE_DETECT,
		2, 0
	)),
	AZALIA_PIN_CFG(0, 0x0c, 0x40f000f0),	/* Port-C: NC */
	/* Port-D: Internal speakers */
	AZALIA_PIN_CFG(0, 0x0d, AZALIA_PIN_DESC(
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_SPEAKER,
		AZALIA_ATAPI_INTERNAL,
		AZALIA_GREEN,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		1, 0
	)),
	/* Port-E: Internal microphone */
	AZALIA_PIN_CFG(0, 0x0e, AZALIA_PIN_DESC(
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_MIC_IN,
		AZALIA_ATAPI_INTERNAL,
		AZALIA_BLACK,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		3, 0
	)),
	AZALIA_PIN_CFG(0, 0x0f, 0x40f000f0),	/* Port-F: NC */
	AZALIA_PIN_CFG(0, 0x18, 0x40f000f0),	/* Digital mic: NC */

	/* Pin widget control: HP + output enable on Port-A, output enable on Port-D */
	AZALIA_VERB_12B(0, 0x0a, 0x707, 0xc0),
	AZALIA_VERB_12B(0, 0x0d, 0x707, 0x40),

	/* IDT vendor-specific verbs (undocumented, copied verbatim from vendor) */
	AZALIA_VERB_12B(0, 0x01, 0x7c6, 0x03),
	AZALIA_VERB_12B(0, 0x1a, 0x79b, 0x03),
	AZALIA_VERB_12B(0, 0x01, 0x7ee, 0x31),
	AZALIA_VERB_12B(0, 0x01, 0x780, 0x04),
};

/* Board variant 2: Realtek ALC295 (the ALC1306 amplifier needs no verbs) */
static const u32 realtek_alc295_verbs[] = {
	AZALIA_SUBVENDOR(0, 0xf1110001),

	/* Reset all registers back to default */
	0x0205001a, 0x0204c003, 0x0205001a, 0x02048003,

	AZALIA_RESET(1),

	/* Pin widget configurations */
	/* DMIC: Internal microphone */
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_DESC(
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_MIC_IN,
		AZALIA_OTHER_DIGITAL,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		3, 0
	)),
	AZALIA_PIN_CFG(0, 0x13, 0x40000000),		/* DMIC: NC */
	/* Port-D: Internal speakers */
	AZALIA_PIN_CFG(0, 0x14, AZALIA_PIN_DESC(
		AZALIA_INTEGRATED,
		AZALIA_INTERNAL,
		AZALIA_SPEAKER,
		AZALIA_OTHER_ANALOG,
		AZALIA_COLOR_UNKNOWN,
		AZALIA_NO_JACK_PRESENCE_DETECT,
		1, 0
	)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),	/* NPC */
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),	/* I2S OUT */
	AZALIA_PIN_CFG(0, 0x18, AZALIA_PIN_CFG_NC(0)),	/* I2S IN */
	/*
	 * MIC2 (Port-F) is NC like on the vendor firmware. Linux turns it into
	 * the headset mic through ALC295_FIXUP_FRAMEWORK_LAPTOP_MIC_NO_PRESENCE,
	 * which matches on the codec SSID programmed above.
	 */
	AZALIA_PIN_CFG(0, 0x19, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1a, AZALIA_PIN_CFG_NC(0)),	/* NPC */
	AZALIA_PIN_CFG(0, 0x1b, AZALIA_PIN_CFG_NC(0)),	/* LINE2 (Port-E) */
	/*
	 * BEEP-IN: not a pin description. Realtek codecs use this pin's
	 * configuration default as a SKU word (valid, checksum 4, laptop,
	 * no external amp, PC beep disabled).
	 */
	AZALIA_PIN_CFG(0, 0x1d, 0x40641a05),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),	/* S/PDIF-OUT */
	/* HP1-OUT (Port-I): Headphone jack */
	AZALIA_PIN_CFG(0, 0x21, AZALIA_PIN_DESC(
		AZALIA_JACK,
		AZALIA_EXTERNAL_PRIMARY_CHASSIS | AZALIA_FRONT,
		AZALIA_HP_OUT,
		AZALIA_STEREO_MONO_1_8,
		AZALIA_BLACK,
		AZALIA_JACK_PRESENCE_DETECT,
		2, 0
	)),

	/* Realtek processing coefficients (NID 0x20), copied verbatim from vendor */
	0x02050063, 0x0204cf00, 0x0205004a, 0x02042830,

	/* ClassD reset */
	0x0205003c, 0x0204f254, 0x0205003c, 0x0204f214,

	/* ClassD gain */
	0x02050038, 0x02048909, 0x02050038, 0x02048909,
};

/*
 * The vendor firmware does not program the display audio codec: the pin
 * defaults read back from Linux (0x18560010 on every pin) are the hardware
 * reset values. This table keeps that parity instead of the per-port
 * associations from soc/intel/tigerlake/display_audio.c.
 */
#define TGL_HDMI_PIN_DESC AZALIA_PIN_DESC(	\
	AZALIA_JACK,				\
	AZALIA_DIGITAL_DISPLAY,			\
	AZALIA_DIGITAL_OTHER_OUT,		\
	AZALIA_OTHER_DIGITAL,			\
	AZALIA_COLOR_UNKNOWN,			\
	AZALIA_JACK_PRESENCE_DETECT,		\
	1, 0					\
)

static const u32 intel_tgl_hdmi_verbs[] = {
	AZALIA_SUBVENDOR(2, 0x80860101),

	/* Nine digital display pin widgets */
	AZALIA_PIN_CFG(2, 0x04, TGL_HDMI_PIN_DESC),
	AZALIA_PIN_CFG(2, 0x06, TGL_HDMI_PIN_DESC),
	AZALIA_PIN_CFG(2, 0x08, TGL_HDMI_PIN_DESC),
	AZALIA_PIN_CFG(2, 0x0a, TGL_HDMI_PIN_DESC),
	AZALIA_PIN_CFG(2, 0x0b, TGL_HDMI_PIN_DESC),
	AZALIA_PIN_CFG(2, 0x0c, TGL_HDMI_PIN_DESC),
	AZALIA_PIN_CFG(2, 0x0d, TGL_HDMI_PIN_DESC),
	AZALIA_PIN_CFG(2, 0x0e, TGL_HDMI_PIN_DESC),
	AZALIA_PIN_CFG(2, 0x0f, TGL_HDMI_PIN_DESC),
};

const u32 pc_beep_verbs[] = {};

struct azalia_codec mainboard_azalia_codecs[] = {
	{
		.name         = "IDT 92HD95",
		.vendor_id    = 0x111d7695,
		.subsystem_id = 0xf1110001,
		.address      = 0,
		.verbs        = idt_92hd95_verbs,
		.verb_count   = ARRAY_SIZE(idt_92hd95_verbs),
	},
	{
		.name         = "Realtek ALC295",
		.vendor_id    = 0x10ec0295,
		.subsystem_id = 0xf1110001,
		.address      = 0,
		.verbs        = realtek_alc295_verbs,
		.verb_count   = ARRAY_SIZE(realtek_alc295_verbs),
	},
	{
		.name         = "Intel Tiger Lake HDMI",
		.vendor_id    = 0x80862812,
		.subsystem_id = 0x80860101,
		.address      = 2,
		.verbs        = intel_tgl_hdmi_verbs,
		.verb_count   = ARRAY_SIZE(intel_tgl_hdmi_verbs),
	},
};

AZALIA_ARRAY_SIZES;
