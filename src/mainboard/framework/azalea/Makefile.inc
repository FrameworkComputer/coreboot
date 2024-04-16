# SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
bootblock-y += mb_gpio.c

romstage-y += mb_gpio.c
romstage-y += port_descriptors.c

ramstage-y += mb_gpio.c
ramstage-y += mainboard.c
ramstage-y += port_descriptors.c
