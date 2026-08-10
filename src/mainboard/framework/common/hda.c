/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Laptop 13 has two different chassis available, which use different speakers
 * and have different audio behavior. Therefore the audio tuning needs to be
 * different for optimal performance and speaker protection. The OS detects the
 * chassis by checking the HDA codec PCIe SSID.
 *
 * The different chassis fit different audio boards, so we can detect it by the
 * audio board ID.
 *
 * In standalone mode (bare mainboard) everything newer than Sakura defaults to
 * Pro SSID.
 *
 * ADL and Iris don't need an override, they use a Tempo codec, which can
 * handle both chassis.
 *
 * | Platform  | Classic Chassis | Pro Chassis | Default |
 * |-----------|-----------------|-------------|---------|
 * | Tigerlake | 0x0001          | 0x0012      | Classic |
 * | Alderlake | 0x0002          | N/A         | Classic |
 * | Iris      | 0x0003          | N/A         | Classic |
 * | Azalea    | 0x0006          | 0x0013      | Classic |
 * | Marigold  | 0x0009          | 0x0014      | Classic |
 * | Lilac     | 0x000B          | 0x010B      | Classic |
 * | Sakura    | 0x010F          | 0x000F      | Pro     |
 *
 */

#include <console/console.h>
#include <device/azalia_device.h>
#include <ec/google/chromeec/ec.h>
#include <mainboard/framework/common/board_host_command.h>
#include <mainboard/framework/common/hda.h>
#include <types.h>

/* Board ID the EC reports when it could not measure the strap */
#define BOARD_ID_UNKNOWN	-1

static int audio_board_id(void)
{
	const struct ec_params_read_boardid params = {
		.board_id_type = HC_BOARD_ID_AUDIO_BOARD,
	};
	struct ec_response_read_boardid resp;
	struct chromeec_command cmd = {
		.cmd_code	= EC_CMD_READ_BOARDID,
		.cmd_version	= 0,
		.cmd_data_in	= &params,
		.cmd_size_in	= sizeof(params),
		.cmd_data_out	= &resp,
		.cmd_size_out	= sizeof(resp),
		.cmd_dev_index	= 0,
	};

	if (google_chromeec_command(&cmd)) {
		printk(BIOS_ERR, "HDA: Failed to read the audio board ID\n");
		return BOARD_ID_UNKNOWN;
	}

	return resp.board_id;
}

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	const struct framework_audio_board_codec *codec = &framework_audio_board_codec;

	/* Every other codec, e.g. the HDMI, keeps its SSID. They don't control
	 * the built-in speakers. */
	if (viddid != codec->vendor_id)
		return;

	const int board_id = audio_board_id();
	const u32 ssid = (board_id == FRAMEWORK_AUDIO_BOARD_ID_PRO) ? codec->ssid_pro
								   : codec->ssid_classic;
	const u32 verbs[] = {
		AZALIA_SUBVENDOR(codec->address, ssid),
	};

	printk(BIOS_DEBUG, "HDA: Audio board ID %d, subsystem ID 0x%08x\n", board_id, ssid);

	if (azalia_program_verb_table(base, verbs, ARRAY_SIZE(verbs)) < 0)
		printk(BIOS_WARNING, "HDA: Failed to program the subsystem ID\n");
}
