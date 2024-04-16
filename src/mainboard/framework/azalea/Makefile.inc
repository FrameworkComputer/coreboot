# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += mb_gpio.c
bootblock-y += mb_ec.c

romstage-y += mb_gpio.c
romstage-y += port_descriptors.c

ramstage-y += mb_ec.c
ramstage-y += mb_gpio.c
ramstage-y += mainboard.c
ramstage-y += port_descriptors.c

smm-y += smihandler.c
