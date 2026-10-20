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
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
}

/*
 * TEST: read back what FSP sent to pcode via the B2P mailbox: the demotion
 * disables (FSP-S) and the per-domain VR settings (FSP-M).
 */
#define PCODE_MAILBOX_RUN_BUSY			BIT(31)
#define PCODE_MAILBOX_TIMEOUT_US		1000
#define PCODE_CMD_SVID_VR_HANDLER		0x18
#define  PCODE_SUBCMD_SVID_GET_STRAP_CONFIG	0x00
#define  PCODE_SUBCMD_SVID_GET_MAX_ICC		0x05
#define  PCODE_SUBCMD_SVID_GET_FVM_ICC_LIMIT	0x24
#define PCODE_CMD_READ_VR_TDC_CONFIG		0x19
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

/* Returns the completion code, or -1 if the mailbox did not respond */
static int pcode_mailbox_read(uint8_t cmd, uint8_t param1, uint16_t param2, uint32_t *data)
{
	if (!pcode_mailbox_ready()) {
		printk(BIOS_ERR, "pcode mailbox busy before cmd 0x%x/0x%x\n", cmd, param1);
		return -1;
	}

	MCHBAR32(PCODE_MAILBOX_DATA) = 0;
	MCHBAR32(PCODE_MAILBOX_INTERFACE) = PCODE_MAILBOX_RUN_BUSY | (param2 & 0x1fff) << 16 |
					    param1 << 8 | cmd;

	if (!pcode_mailbox_ready()) {
		printk(BIOS_ERR, "pcode mailbox timeout on cmd 0x%x/0x%x\n", cmd, param1);
		return -1;
	}

	/* Completion code is returned in the command field */
	*data = MCHBAR32(PCODE_MAILBOX_DATA);
	return MCHBAR32(PCODE_MAILBOX_INTERFACE) & 0xff;
}

static void pcode_read_demotion_disable(const char *name, uint8_t subcmd)
{
	uint32_t data;
	int cc;

	cc = pcode_mailbox_read(PCODE_CMD_MISC_ALG_CONFIG, subcmd, 0, &data);
	if (cc < 0)
		return;

	printk(BIOS_INFO, "pcode %s demotion disable = 0x%x (cc 0x%x) -> %s\n", name, data,
	       cc, cc ? "read failed" : (data & 1) ? "DISABLED" : "enabled");
}

/* VR_TOPOLOGY_DATA (strap set 2 low): address [3:0] + "not SVID" bit per domain */
static const struct {
	const char *name;
	uint8_t shift;
} vr_domains[] = {
	{ "IA",   5 },
	{ "GT",   10 },
	{ "SA",   25 },
	{ "Atom", 15 },
};

static void pcode_vr_readback(void)
{
	uint32_t topology, data;
	int cc;

	cc = pcode_mailbox_read(PCODE_CMD_SVID_VR_HANDLER, PCODE_SUBCMD_SVID_GET_STRAP_CONFIG,
				0, &topology);
	if (cc) {
		printk(BIOS_ERR, "pcode VR topology read failed (cc %d)\n", cc);
		return;
	}
	printk(BIOS_INFO, "pcode VR topology = 0x%08x\n", topology);

	for (size_t i = 0; i < ARRAY_SIZE(vr_domains); i++) {
		const char *name = vr_domains[i].name;
		uint8_t addr = (topology >> vr_domains[i].shift) & 0xf;

		if (topology & BIT(vr_domains[i].shift + 4)) {
			printk(BIOS_INFO, "pcode VR %s: no SVID VR\n", name);
			continue;
		}

		cc = pcode_mailbox_read(PCODE_CMD_SVID_VR_HANDLER, PCODE_SUBCMD_SVID_GET_MAX_ICC,
					addr, &data);
		if (cc)
			printk(BIOS_INFO, "pcode VR %s (addr %u): IccMax read failed (cc 0x%x)\n",
			       name, addr, cc);
		else
			printk(BIOS_INFO, "pcode VR %s (addr %u): IccMax = %u (1/4 A) = %u A\n",
			       name, addr, data, data / 4);

		/* FSP treats a failed read as "VR does not support Fast Vmode" */
		cc = pcode_mailbox_read(PCODE_CMD_SVID_VR_HANDLER,
					PCODE_SUBCMD_SVID_GET_FVM_ICC_LIMIT, addr, &data);
		if (cc)
			printk(BIOS_INFO, "pcode VR %s: Fast Vmode NOT supported (cc 0x%x)\n",
			       name, cc);
		else
			printk(BIOS_INFO, "pcode VR %s: Fast Vmode supported, IccLimit = %u (1/4 A) = %u A%s\n",
			       name, data, data / 4, data ? "" : " (disabled)");

		cc = pcode_mailbox_read(PCODE_CMD_READ_VR_TDC_CONFIG, addr, 0, &data);
		if (cc) {
			printk(BIOS_INFO, "pcode VR %s: TDC read failed (cc 0x%x)\n", name, cc);
			continue;
		}
		/* Time window = (1 + X/4) * 2^Y ms, X = [23:22], Y = [21:17] */
		printk(BIOS_INFO, "pcode VR %s: TDC = 0x%08x: limit %u (1/8 A) = %u A, %s, "
		       "window %llu ms, IrmsSupported %u, UseIrms %u, lock %u\n",
		       name, data, data & 0x7fff, (data & 0x7fff) / 8,
		       data & BIT(15) ? "enabled" : "disabled",
		       ((4ULL + ((data >> 22) & 0x3)) << ((data >> 17) & 0x1f)) / 4,
		       !!(data & BIT(29)), !!(data & BIT(30)), !!(data & BIT(31)));
	}
}

static void pcode_readback(void *unused)
{
	pcode_read_demotion_disable("ForcePr", PCODE_SUBCMD_READ_FORCEPR_DEMOTION_DIS);
	pcode_read_demotion_disable("VrAlert", PCODE_SUBCMD_READ_VRALERT_DEMOTION_DIS);
	pcode_vr_readback();
}

/* FSP-S (which sends the disables) has run by the time devices are enumerated */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENUMERATE, BS_ON_ENTRY, pcode_readback, NULL);

struct chip_operations mainboard_ops = {.init = mainboard_init};
