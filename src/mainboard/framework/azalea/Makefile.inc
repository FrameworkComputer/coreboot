# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += mb_gpio.c
bootblock-y += mb_ec.c

romstage-y += mb_gpio.c
romstage-y += port_descriptors.c

ramstage-y += hda_verb.c
ramstage-y += mb_ec.c
ramstage-y += mb_gpio.c
ramstage-y += mainboard.c
ramstage-y += port_descriptors.c

smm-y += smihandler.c

ifneq ($(wildcard $(MAINBOARD_BLOBS_DIR)/APCB_FP7_DDR5.bin),)
APCB_SOURCES = $(MAINBOARD_BLOBS_DIR)/APCB_FP7_DDR5.bin
APCB_SOURCES_RECOVERY = $(MAINBOARD_BLOBS_DIR)/APCB_FP7_DDR5_DefaultRecovery.bin
else
files_added:: warn_no_apcb
endif
