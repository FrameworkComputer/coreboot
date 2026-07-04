/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef MAINBOARD_FRAMEWORK_DEVICE_SWITCHES_H
#define MAINBOARD_FRAMEWORK_DEVICE_SWITCHES_H

/*
 * CFR option names for the "Devices" setup menu. Kept in this small header
 * (rather than ec.h) so the romstage/ramstage consumers that only need the
 * option-name strings do not have to pull in the Chrome EC command headers.
 *
 * The camera/bluetooth/wifi/fingerprint switches are plain booleans (default
 * enabled) that each board applies in its mb_devtree_update() (devtree.c).
 *
 * The "typec" switch additionally has an FSP-M half that must run in romstage
 * (see the board's mainboard_disable_typec()), because the master TCSS
 * controller/port enables are FSP-M UPDs; the FSP-S per-port enables and the
 * OS-visible devices are handled in mb_devtree_update(). PD charging is
 * independent of all of these and keeps working.
 */
#define CAMERA_OPTION_NAME		"camera"
#define BLUETOOTH_OPTION_NAME		"bluetooth"
#define WIFI_OPTION_NAME		"wifi"
#define FINGERPRINT_OPTION_NAME		"fingerprint"
#define TYPEC_OPTION_NAME		"typec"

#endif /* MAINBOARD_FRAMEWORK_DEVICE_SWITCHES_H */
