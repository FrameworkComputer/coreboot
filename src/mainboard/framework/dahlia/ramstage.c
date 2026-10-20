/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootstate.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <ec/ec.h>
#include <intelblocks/systemagent.h>
#include <mainboard/ramstage.h>
#include <soc/ramstage.h>
#include <soc/systemagent.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/*
	 * Disable C10 reporting over eSPI to prevent LED flicker
	 * during S0ix entry/exit on laptops.
	 */
	params->PchEspiHostC10ReportEnable = 0;

	/* Auto-notify iGPU of display changes on USB-C (for hotplug) */
	params->TcNotifyIgd = 2;

	/*
	 * CNVi RF_RESET# and CLKREQ routed to GPP_F04/GPP_F05, not the FSP
	 * default of GPP_A8/GPP_A9, see FspsUpd.h for defaults and values.
	 */
	params->CnviRfResetPinMux = 0x194CE404; /* GPP_F04 */
	params->CnviClkreqPinMux = 0x394CE605;  /* GPP_F05 */

	/* TEST: disable VR_ALERT# throttling and ForcePr/VrAlert demotion */
	params->PchPmVrAlert = 0;
	params->VrAlertDemotion = 0;
	params->ForcePrDemotion = 0;
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
}

/* TEST: read back the pcode demotion disables FSP-S sent via B2P mailbox */
#define PCODE_MAILBOX_RUN_BUSY			BIT(31)
#define PCODE_MAILBOX_TIMEOUT_US		1000
#define PCODE_CMD_MISC_ALG_CONFIG		0x2b
#define  PCODE_SUBCMD_READ_FORCEPR_DEMOTION_DIS	0x0c
#define  PCODE_SUBCMD_READ_VRALERT_DEMOTION_DIS	0x0e

static bool pcode_mailbox_ready(void)
{
	for (size_t i = 0; i < PCODE_MAILBOX_TIMEOUT_US; i++) {
		if (!(MCHBAR32(PCODE_MAILBOX_INTERFACE) & PCODE_MAILBOX_RUN_BUSY))
			return true;
		udelay(1);
	}
	return false;
}

static void pcode_read_demotion_disable(const char *name, uint8_t subcmd)
{
	uint32_t status, data;

	if (!pcode_mailbox_ready()) {
		printk(BIOS_ERR, "pcode mailbox busy before %s readback\n", name);
		return;
	}

	MCHBAR32(PCODE_MAILBOX_DATA) = 0;
	MCHBAR32(PCODE_MAILBOX_INTERFACE) = PCODE_MAILBOX_RUN_BUSY | subcmd << 8 |
					    PCODE_CMD_MISC_ALG_CONFIG;

	if (!pcode_mailbox_ready()) {
		printk(BIOS_ERR, "pcode mailbox timeout on %s readback\n", name);
		return;
	}

	/* Completion code is returned in the command field */
	status = MCHBAR32(PCODE_MAILBOX_INTERFACE) & 0xff;
	data = MCHBAR32(PCODE_MAILBOX_DATA);

	printk(BIOS_INFO, "pcode %s demotion disable = 0x%x (cc 0x%x) -> %s\n", name, data,
	       status, status ? "read failed" : (data & 1) ? "DISABLED" : "enabled");
}

static void pcode_demotion_readback(void *unused)
{
	pcode_read_demotion_disable("ForcePr", PCODE_SUBCMD_READ_FORCEPR_DEMOTION_DIS);
	pcode_read_demotion_disable("VrAlert", PCODE_SUBCMD_READ_VRALERT_DEMOTION_DIS);
}

/* FSP-S (which sends the disables) has run by the time devices are enumerated */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, pcode_demotion_readback, NULL);

struct chip_operations mainboard_ops = {.init = mainboard_init};
