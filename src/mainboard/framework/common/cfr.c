/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <console/cfr.h>
#include <console/console.h>
#include <drivers/option/cfr_frontend.h>
#include <ec/google/chromeec/ec.h>
#include <mainboard/framework/common/board_host_command.h>
#include <mainboard/framework/common/device_switches.h>
#include <mainboard/framework/common/ec.h>
#include <stdio.h>
#include <string.h>
#include <version.h>

static const struct sm_object ps2_emulation = SM_DECLARE_BOOL({
	.opt_name	= PS2_EMULATION_OPTION_NAME,
	.ui_name	= "PS/2 Touchpad Emulation",
	.ui_helptext	= "Let the EC emulate the touchpad as PS/2 if no "
			  "Operating System driver is detected (e.g. Windows Installer).",
	.default_value	= true,
});

static const struct sm_object standalone_mode = SM_DECLARE_BOOL({
	.opt_name	= STANDALONE_MODE_OPTION_NAME,
	.ui_name	= "EC Standalone Mode",
	.ui_helptext	= "Enable EC standalone mode, for running the mainboard "
			  "without the rest of the laptop. Skips hardware checks "
			  "for chassis open, display, battery and Input Cover",
	.default_value	= false,
});

static const struct sm_enum_value fp_led_level_values[] = {
	{ "EC default",	FP_LED_LEVEL_EC_DEFAULT },
#if !CONFIG(BOARD_FRAMEWORK_SUNFLOWER)
	{ "Auto",	FP_LED_BRIGHTNESS_AUTO },
#endif
	{ "High",	FP_LED_BRIGHTNESS_HIGH },
	{ "Medium",	FP_LED_BRIGHTNESS_MEDIUM },
	{ "Low",	FP_LED_BRIGHTNESS_LOW },
	{ "Ultra low",	FP_LED_BRIGHTNESS_ULTRA_LOW },
	SM_ENUM_VALUE_END,
};

static const struct sm_object fp_led_level = SM_DECLARE_ENUM({
	.opt_name	= FP_LED_LEVEL_OPTION_NAME,
	.ui_name	= "Power Button Brightness",
	.ui_helptext	= "Set the brightness of the power button LED."
#if !CONFIG(BOARD_FRAMEWORK_SUNFLOWER)
			  "\"Auto\" follows the ambient light sensor.\n"
#endif
			  "\"EC default\" leaves the level as last configured in the EC.",
	.default_value	= FP_LED_LEVEL_EC_DEFAULT,
	.values		= fp_led_level_values,
});

static struct sm_obj_form debug = {
	.ui_name = "Debug",
	.obj_list = (const struct sm_object *[]) {
		&debug_level,
		NULL
	},
};

static const struct sm_object battery_charge_limit = SM_DECLARE_NUMBER({
	.opt_name	= BATTERY_CHARGE_LIMIT_OPTION_NAME,
	.ui_name	= "Battery Charge Limit (%)",
	.ui_helptext	= "Maximum battery charge level, to extend battery lifespan.\n"
			  "Range: 50-100%. Charging stops at the selected percentage; "
			  "100 disables the limit and charges the battery fully.\n"
			  "0 keeps the limit as last configured in the EC, e.g. one set "
			  "at runtime via the OS, so a reboot doesn't override it.",
	.default_value	= BATTERY_CHARGE_LIMIT_EC_DEFAULT,
	.min		= 0,
	.max		= 100,
	.step		= 5,
});

static const struct sm_enum_value power_on_ac_attach_values[] = {
	{ "Keep EC setting",	POWER_ON_AC_ATTACH_EC_DEFAULT },
	{ "Enabled",		1 },
	{ "Disabled",		0 },
	SM_ENUM_VALUE_END,
};

static const struct sm_object power_on_ac_attach = SM_DECLARE_ENUM({
	.opt_name	= POWER_ON_AC_ATTACH_OPTION_NAME,
	.ui_name	= "Power on AC Attach",
	.ui_helptext	= "Automatically power on the system when a charger is "
			  "attached.\n"
			  "\"Keep EC setting\" leaves the behavior as last "
			  "configured in the EC, e.g. via the OS.",
	.default_value	= POWER_ON_AC_ATTACH_EC_DEFAULT,
	.values		= power_on_ac_attach_values,
});

#if CONFIG(FRAMEWORK_INPUT_DECK)
static const struct sm_enum_value input_deck_mode_values[] = {
	{ "Require Modules",	INPUT_DECK_MODE_AUTO },
	{ "Force off",		INPUT_DECK_MODE_FORCE_OFF },
	{ "Force on",		INPUT_DECK_MODE_FORCE_ON },
	SM_ENUM_VALUE_END,
};

static const struct sm_object input_deck_mode = SM_DECLARE_ENUM({
	.opt_name	= INPUT_DECK_MODE_OPTION_NAME,
	.ui_name	= "Input Deck Mode",
	.ui_helptext	= "Require Modules will check if all input modules are correctly "
			  "installed before enabling power.",
	.default_value	= INPUT_DECK_MODE_AUTO,
	.values		= input_deck_mode_values,
});
#endif

#if CONFIG(FRAMEWORK_TOUCHSCREEN_STYLUS)
static const struct sm_enum_value stylus_protocol_values[] = {
	{ "MPP 2.0",	STYLUS_PROTOCOL_MPP },
	{ "USI 2.0",	STYLUS_PROTOCOL_USI },
	SM_ENUM_VALUE_END,
};

static const struct sm_object stylus_protocol = SM_DECLARE_ENUM({
	.opt_name	= STYLUS_PROTOCOL_OPTION_NAME,
	.ui_name	= "Touchscreen Stylus Protocol",
	.ui_helptext	= "Use this setting to change the touchscreen stylus protocol "
			  "to match what your stylus supports.\n\n"
			  "Framework Stylus supports both protocols.\n\n"
			  "Modern Microsoft Surface compatible styluses support MPP.\n"
			  "Modern Chromebook compatible styluses support USI.",
	.default_value	= STYLUS_PROTOCOL_MPP,
	.values		= stylus_protocol_values,
});
#endif

static const struct sm_enum_value kbd_swap_ctrl_fn_values[] = {
	{ "Do nothing",	KBD_REMAP_NO_CHANGE },
	{ "Normal",	KB_SCANSET_LEFT_CTRL },
	{ "Swapped",	KB_SCANSET_FN },
	SM_ENUM_VALUE_END,
};

static const struct sm_object kbd_swap_ctrl_fn = SM_DECLARE_ENUM({
	.opt_name	= KBD_SWAP_CTRL_FN_OPTION_NAME,
	.ui_name	= "Swap Left Ctrl and Fn",
	.ui_helptext	= "Swap the left Ctrl and Fn keys.\n"
			  "\"Swapped\" makes the bottom-left key act as Fn and Fn act "
			  "as left Ctrl.\n"
			  "\"Normal\" restores the default layout.\n"
			  "\"Do nothing\" leaves the key mapping as configured in the EC.",
	.default_value	= KBD_REMAP_NO_CHANGE,
	.values		= kbd_swap_ctrl_fn_values,
});

static const struct sm_enum_value kbd_copilot_values[] = {
	{ "Do nothing",		KBD_REMAP_NO_CHANGE },
	{ "Right Ctrl",		KB_SCANSET_RIGHT_CTRL },
	{ "Copilot key",	KB_SCANSET_COPILOT },
	SM_ENUM_VALUE_END,
};

static const struct sm_object kbd_copilot = SM_DECLARE_ENUM({
	.opt_name	= KBD_COPILOT_OPTION_NAME,
	.ui_name	= "Copilot Key",
	.ui_helptext	= "Configure the bottom-right key.\n"
			  "\"Right Ctrl\" makes it a normal right Ctrl key.\n"
			  "\"Copilot key\" makes it act as the Windows Copilot key.\n"
			  "\"Do nothing\" leaves the key mapping as configured in the EC.",
	.default_value	= KBD_REMAP_NO_CHANGE,
	.values		= kbd_copilot_values,
});

static const struct sm_object battery_disconnect = SM_DECLARE_BOOL({
	.opt_name	= BATTERY_DISCONNECT_OPTION_NAME,
	.ui_name	= "Disconnect Battery",
	.ui_helptext	= "Disconnect the battery (ship mode) on the next boot. The "
			  "system powers off and stays off until a charger is "
			  "connected. This is a one-shot action that resets itself "
			  "automatically, so it does not stay enabled.",
	.default_value	= false,
});

/*
 * Onboard device on/off switches. The payload writes the selected value to the
 * option backend (UEFI variable store / SMMSTORE); each board reads it back in
 * mb_devtree_update() (see the board's devtree.c) and disables the matching
 * device before FSP-S runs. Boards that lack one of these devices simply do not
 * act on its option.
 */
static const struct sm_object camera = SM_DECLARE_BOOL({
	.opt_name	= CAMERA_OPTION_NAME,
	.ui_name	= "Camera",
	.ui_helptext	= "Enable or disable the internal webcam.",
	.default_value	= true,
});

static const struct sm_object bluetooth = SM_DECLARE_BOOL({
	.opt_name	= BLUETOOTH_OPTION_NAME,
	.ui_name	= "Bluetooth",
	.ui_helptext	= "Enable or disable the internal Bluetooth radio.",
	.default_value	= true,
});

static const struct sm_object wifi = SM_DECLARE_BOOL({
	.opt_name	= WIFI_OPTION_NAME,
	.ui_name	= "Wi-Fi",
	.ui_helptext	= "Enable or disable the onboard Wi-Fi module in the "
			  "M.2/E 2230 (WLAN) slot.",
	.default_value	= true,
});

static const struct sm_object fingerprint = SM_DECLARE_BOOL({
	.opt_name	= FINGERPRINT_OPTION_NAME,
	.ui_name	= "Fingerprint Reader",
	.ui_helptext	= "Enable or disable the fingerprint reader in the power button.",
	.default_value	= true,
});

static const struct sm_object typec = SM_DECLARE_BOOL({
	.opt_name	= TYPEC_OPTION_NAME,
	.ui_name	= "USB-C Ports",
	.ui_helptext	= "Enable or disable all USB-C ports, including USB data, "
			  "DisplayPort Alt Mode and Thunderbolt/USB4. Charging over "
			  "USB-C is unaffected and keeps working.\n"
			  "Disabling turns off external display output through the "
			  "USB-C ports and blocks Thunderbolt/USB4 (including DMA), "
			  "which can be useful in high-security environments.",
	.default_value	= true,
});

static struct sm_obj_form devices = {
	.ui_name = "Devices",
	.obj_list = (const struct sm_object *[]) {
		&camera,
		&bluetooth,
		&wifi,
		&fingerprint,
		&typec,
		NULL
	},
};

static struct sm_obj_form ec = {
	.ui_name = "Embedded Controller",
	.obj_list = (const struct sm_object *[]) {
		&ps2_emulation,
		&standalone_mode,
		&fp_led_level,
		&battery_charge_limit,
#if CONFIG(FRAMEWORK_INPUT_DECK)
		&input_deck_mode,
#endif
#if CONFIG(FRAMEWORK_TOUCHSCREEN_STYLUS)
		&stylus_protocol,
#endif
		&battery_disconnect,
		&kbd_swap_ctrl_fn,
		&kbd_copilot,
		&power_on_ac_attach,
		NULL
	},
};

/*
 * Read-only version strings shown in the setup menu, populated from the EC at
 * table-generation time. The buffers must stay alive until the CFR table has
 * been serialized, so they are static.
 */
static int ec_query(uint16_t code, uint8_t version, void *out, size_t out_size)
{
	struct chromeec_command cmd = {
		.cmd_code	= code,
		.cmd_version	= version,
		.cmd_data_in	= NULL,
		.cmd_size_in	= 0,
		.cmd_data_out	= out,
		.cmd_size_out	= out_size,
		.cmd_dev_index	= 0,
	};

	return google_chromeec_command(&cmd);
}

static void update_ec_version(struct sm_object *new)
{
	static char buf[sizeof(((struct ec_response_get_custom_version *)0)->simple_version) + 1];
	struct ec_response_get_custom_version r;

	if (ec_query(EC_CMD_GET_SIMPLE_VERSION, 0, &r, sizeof(r)) < 0) {
		new->sm_varchar.default_value = "Unknown";
		return;
	}

	/* simple_version is not guaranteed to be NUL-terminated */
	memcpy(buf, r.simple_version, sizeof(r.simple_version));
	buf[sizeof(r.simple_version)] = '\0';
	new->sm_varchar.default_value = buf;
}

static const struct sm_object ec_version = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "ec_version",
	.ui_name	= "EC Version",
}, WITH_CALLBACK(update_ec_version));

static const struct sm_object coreboot_ver = SM_DECLARE_VARCHAR({
	.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
	.opt_name	= "coreboot_version",
	.ui_name	= "coreboot Version",
	.default_value	= coreboot_version,
});

/*
 * The PD controller version fields are built at runtime because the number of
 * controllers is board specific (CONFIG_FRAMEWORK_EC_PD_CHIP_COUNT). All
 * buffers must outlive cfr_write_setup_menu(), hence they are static.
 */
#define PD_CHIP_COUNT	CONFIG_FRAMEWORK_EC_PD_CHIP_COUNT

static struct sm_object pd_version_objs[PD_CHIP_COUNT];
static char pd_version_value[PD_CHIP_COUNT][16];
static char pd_version_name[PD_CHIP_COUNT][sizeof("PD 255 Version")];
static char pd_version_opt[PD_CHIP_COUNT][sizeof("pd_version_255")];

/* coreboot version + EC version + PD_CHIP_COUNT fields + NULL terminator */
static const struct sm_object *versions_obj_list[PD_CHIP_COUNT + 3];

static struct sm_obj_form versions = {
	.ui_name = "Firmware Versions",
};

/*
 * Format a Cypress CCGx PD controller version. The application firmware
 * version (and its two-character application type) is the user-facing one.
 */
static const char *format_pd_version(char *buf, size_t size, const uint8_t *v)
{
	/* All-zero means no PD controller is present at this index */
	if (!v[4] && !v[5] && !v[6] && !v[7]) {
		strncpy(buf, "Not present", size);
		return buf;
	}

	snprintf(buf, size, "%x.%x.%x",
		 (v[7] & 0xf0) >> 4, v[7] & 0x0f, v[6]);
	return buf;
}

static void build_versions_form(void)
{
	struct ec_response_read_pd_version_v1_4x r1;
	struct ec_response_read_pd_version_v0 r0;
	const uint8_t (*pd_versions)[8] = NULL;
	unsigned int pd_count = 0;
	size_t n = 0;

	/*
	 * Newer EC firmware supports READ_PD_VERSION v1 (a controller count
	 * plus that many 8-byte version blobs); older firmware only has v0
	 * (exactly two fixed blobs). Try v1 first and fall back to v0, like
	 * framework_tool does.
	 */
	if (ec_query(EC_CMD_READ_PD_VERSION, 1, &r1, sizeof(r1)) >= 0) {
		pd_versions = r1.pd_versions;
		pd_count = MIN(r1.pd_chip_count, ARRAY_SIZE(r1.pd_versions));
	} else if (ec_query(EC_CMD_READ_PD_VERSION, 0, &r0, sizeof(r0)) >= 0) {
		/* The two blobs are contiguous, so treat them as an array. */
		pd_versions = (const uint8_t (*)[8])&r0;
		pd_count = 2;
	}
	printk(BIOS_DEBUG, "CFR: EC reported %u PD controller version(s)\n", pd_count);

	versions_obj_list[n++] = &coreboot_ver;
	versions_obj_list[n++] = &ec_version;

	for (unsigned int i = 0; i < PD_CHIP_COUNT; i++) {
		const char *value;

		if (pd_versions && i < pd_count)
			value = format_pd_version(pd_version_value[i],
						  sizeof(pd_version_value[i]),
						  pd_versions[i]);
		else
			value = "Unknown";

		snprintf(pd_version_name[i], sizeof(pd_version_name[i]), "PD %u Version", i + 1);
		snprintf(pd_version_opt[i], sizeof(pd_version_opt[i]), "pd_version_%u", i);

		/*
		 * struct sm_object has a const member, so it cannot be
		 * assigned directly; copy from a compound literal instead.
		 */
		const struct sm_object obj = SM_DECLARE_VARCHAR({
			.flags		= CFR_OPTFLAG_READONLY | CFR_OPTFLAG_VOLATILE,
			.opt_name	= pd_version_opt[i],
			.ui_name	= pd_version_name[i],
			.default_value	= value,
		});
		memcpy(&pd_version_objs[i], &obj, sizeof(obj));
		versions_obj_list[n++] = &pd_version_objs[i];
	}

	versions_obj_list[n] = NULL;
	versions.obj_list = versions_obj_list;
}

static struct sm_obj_form *sm_root[] = {
	&debug,
	&devices,
	&ec,
	&versions,
	NULL
};

void mb_cfr_setup_menu(struct lb_cfr *cfr_root)
{
	build_versions_form();
	cfr_write_setup_menu(cfr_root, sm_root);
}
