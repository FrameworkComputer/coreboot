/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef MAINBOARD_FRAMEWORK_HDA_H
#define MAINBOARD_FRAMEWORK_HDA_H

#include <types.h>

#define FRAMEWORK_AUDIO_BOARD_ID_PRO	8

/* The codec on the audio board, provided by the mainboard */
struct framework_audio_board_codec {
	/* HDA vendor/device ID of the codec, as in struct azalia_codec */
	u32 vendor_id;
	/* Codec address (SDI link) the codec responds on */
	u8 address;
	/* Subsystem IDs to program for the classic and pro chassis (audio board) */
	u32 ssid_classic;
	u32 ssid_pro;
};

extern const struct framework_audio_board_codec framework_audio_board_codec;

#endif /* MAINBOARD_FRAMEWORK_HDA_H */
