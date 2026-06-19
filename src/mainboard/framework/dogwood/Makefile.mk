# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += port_descriptors.c

ramstage-y += mainboard.c
ramstage-y += port_descriptors.c

APCB_SOURCES          = $(src)/mainboard/$(MAINBOARDDIR)/APCB/APCB_dogwood_LPDDR5.apcb
APCB_SOURCES_RECOVERY = $(src)/mainboard/$(MAINBOARDDIR)/APCB/APCB_dogwood_LPDDR5.apcb
