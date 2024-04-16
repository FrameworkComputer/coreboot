/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "mb_gpio.h"

/* GPIO pins used by coreboot should be initialized in bootblock */
static const struct soc_amd_gpio gpio_set_stage_reset[] = {

	/* ESPI */
	/* ESPI_RST# */
	PAD_NF(GPIO_21, ESPI_RESET_L, PULL_NONE),
	/* ESPI_ALERT */
	PAD_NF(GPIO_22, ESPI_ALERT_D1, PULL_NONE),
	/* ESPI_CS_L */
	PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* ESPI_CLK */
	PAD_NF(GPIO_77, SPI1_CLK, PULL_NONE),
	/* ESPI1_DATA0 */
	PAD_NF(GPIO_81, SPI1_DAT0, PULL_NONE),
	/* ESPI1_DATA1 */
	PAD_NF(GPIO_80, SPI1_DAT1, PULL_NONE),
	/* ESPI1_DATA2 */
	PAD_NF(GPIO_68, SPI1_DAT2, PULL_NONE),
	/* ESPI1_DATA3 */
	PAD_NF(GPIO_69, SPI1_DAT3, PULL_NONE),

	/* Enable UART 0 */
	/* UART0_RXD */
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART0_TXD */
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),

	/* Deassert PCIe Reset lines */
	/* APU_PCIE0_RST# */
	PAD_NFO(GPIO_26, PCIE_RST0_L, HIGH),
	/* APU_PCIE1_RST# */
	PAD_NFO(GPIO_27, PCIE_RST1_L, HIGH),
	/* SSD1_RST# */
	PAD_GPO(GPIO_78, HIGH),

	/* APU_EC_INT# */
	PAD_INT(GPIO_84, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),

};

static const struct soc_amd_gpio gpio_set_stage_rom[] = {

/* Enable touchscreen, hold in reset */
/* TS_RST */
PAD_GPO(GPIO_3, LOW),

/* TS_EN_Q */
PAD_GPO(GPIO_4, HIGH),
};


/*
 * GPIO pins used only by the OS should be initialized at ramstage.
 */
static const struct soc_amd_gpio gpio_set_stage_ram[] = {
	/* PBTN_OUT# */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_NONE),
	/* SYS_RST# */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),
	/* PCIE_WAKE# */
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_NONE, EDGE_LOW),

	/* Enable touchscreen, hold in reset */
	/* TS_RST */
	PAD_GPO(GPIO_3, HIGH),


	/* APU_EC_INT2# */
	PAD_GPI(GPIO_5, PULL_UP),
	PAD_WAKE(GPIO_5, PULL_UP, EDGE_LOW, S0i3_S4_S5),
	/* WLAN_WAKE# */
	PAD_GPI(GPIO_6, PULL_UP),
	PAD_WAKE(GPIO_6, PULL_UP, EDGE_LOW, S0i3_S4_S5),
	/* EC_SCI# */
	PAD_SCI(GPIO_7, PULL_UP, EDGE_LOW),
	/* I2C_INT#_TP */
	PAD_SCI(GPIO_8, PULL_UP, EDGE_LOW),
	/* Unused */
	PAD_NC(GPIO_9),
	/* APU_BT_ON_S5 TODO: Verify this pin*/
	PAD_GPO(GPIO_10, HIGH),
	/* Unused */
	PAD_NC(GPIO_11),
	/* TS_INT# */
	PAD_INT(GPIO_12, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* GPIO_13 - GPIO_15: Not available */
	/* JUSBC_1_2_OC# */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* JUSBC_3_4_OC# */
	PAD_NF(GPIO_17, USB_OC1_L, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_18),
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* ESPI_RST# - initialized in bootblock */
	//PAD_NF(GPIO_21, ESPI_RESET_L, PULL_NONE),
	/* ESPI_ALERT - initialized in bootblock */
	//PAD_NF(GPIO_22, ESPI_ALERT_D1, PULL_NONE),
	/* HW_ACAV_IN_L */
	PAD_NF(GPIO_23, AC_PRES, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_24),
	/* GPIO_25: Not available */
	/* APU_PCI_RST0_L - initialized in bootblock */
	//PAD_NFO(GPIO_26, PCIE_RST0_L, HIGH),
	/* APU_PCI_RST1_L - initialized in bootblock */
	//PAD_NFO(GPIO_27, PCIE_RST1_L, HIGH),
	/* GPIO_28: Not available */
	/* SPI_TOM_CS# */
	PAD_NF(GPIO_29, SPI_TPM_CS_L, PULL_NONE),
	/* ESPI_EC_CS# - initialized in bootblock*/
	//PAD_NF(GPIO_30, ESPI_CS_L, PULL_NONE),
	/* INT_CLKREQ_L */
	PAD_INT(GPIO_31, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* SPI_TPM_RST# */
	PAD_GPO(GPIO_32, HIGH),
	/* GPIO_33 - GPIO_37: Not available */
	/* Unused */
	PAD_NC(GPIO_38),
	/* CLKREQ_PCIW#6 */
	PAD_NF(GPIO_39, CLK_REQ6_L, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_40),
	/* GPIO_41: Not available */
	/* TS_EN_Q */
	PAD_GPO(GPIO_42, HIGH),
	/* GPIO_43 - GPIO_66: Not available */
	/* Unused */
	PAD_NC(GPIO_67),
	/* ESPI_EC_DAT2 - initialized in bootblock */
	// PAD_NF(GPIO_68, SPI1_DAT2, PULL_NONE),
	/* ESPI_EC_DAT3 - initialized in bootblock */
	// PAD_NF(GPIO_69, SPI1_DAT3, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_70),
	/* GPIO_71 - GPIO_73: Not available */
	/* Unused */
	PAD_NC(GPIO_74),
	/* Unused */
	PAD_NC(GPIO_75),
	/* Unused */
	PAD_NC(GPIO_76),
	/* ESPI_CLK - Initialized in bootblock */
	//PAD_NF(GPIO_77, SPI1_CLK, PULL_NONE),
	/* SSD1_RST# - Initialized in bootblock */
	//PAD_GPO(GPIO_78, HIGH),
	/* Unused */
	PAD_NC(GPIO_79),
	/* ESPI1_DATA1 - Initialized in bootblock */
	//PAD_NF(GPIO_80, SPI1_DAT1, PULL_NONE),
	/* ESPI1_DATA0 - Initialized in bootblock*/
	//PAD_NF(GPIO_81, SPI1_DAT0, PULL_NONE),
	/* GPIO_82 - GPIO_83: Not available */
	/* APU_EC_INT# - Initialized in bootblock */
	//PAD_INT(GPIO_84, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* Unused */
	PAD_NC(GPIO_85),
	/* Not available on package */
	PAD_NC(GPIO_86),
	/* GPIO_87 - GPIO_88: Not available */
	/* Unused */
	PAD_NC(GPIO_89),
	/* TPM SPI_PIRQ# */
	PAD_SCI(GPIO_90, PULL_UP, EDGE_LOW),
	/* SPKR */
	PAD_NF(GPIO_91, SPKR, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_92),
	/* GPIO_93 - GPIO_103: Not available */
	/* Unused */
	PAD_NC(GPIO_104),
	/* Unused */
	PAD_NC(GPIO_105),
	/* Unused */
	PAD_NC(GPIO_106),
	/* Unused */
	PAD_NC(GPIO_107),
	/* GPIO_108 - GPIO_112: Not available */
	/* SMB_0_SCL */
	PAD_NF(GPIO_113, SMBUS0_SCL, PULL_NONE),
	/* SMB_0_SDA */
	PAD_NF(GPIO_114, SMBUS0_SDA, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_115),
	/* Unused */
	PAD_NC(GPIO_116),
	/* GPIO_117 - GPIO_129: Not available */
	/* Unused */
	PAD_NC(GPIO_130),
	/* Unused */
	PAD_NC(GPIO_131),
	/* CLKREQ_PCIE#4 */
	PAD_NF(GPIO_132, CLK_REQ4_L, PULL_NONE),
	/* GPIO_133 - GPIO_134: Not available */
	/* Unused */
	PAD_NC(GPIO_135),
	/* Unused */
	PAD_NC(GPIO_136),
	/* Unused */
	PAD_NC(GPIO_137),
	/* Unused */
	PAD_NC(GPIO_138),
	/* Unused */
	PAD_NC(GPIO_139),
	/* UART_0_CTS# */
	PAD_NF(GPIO_140, UART0_CTS_L, PULL_NONE),
	/* UART_0_ARXD_DTXD - Initialized in bootblock */
	//PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* UART_0_RTS# */
	PAD_NF(GPIO_142, UART0_RTS_L, PULL_NONE),
	/* UART_0_ATXD_DRXD - Initialized in bootblock */
	//PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
	/* Unused */
	PAD_NC(GPIO_144),
	/* APU_I2C_CK0 */
	PAD_NF(GPIO_145, I2C0_SCL, PULL_NONE),
	/* APU_I2C_DA0 */
	PAD_NF(GPIO_146, I2C0_SDA, PULL_NONE),
	/* TS SCL_Q */
	PAD_NF(GPIO_147, I2C1_SCL, PULL_NONE),
	/* TS SDA_Q */
	PAD_NF(GPIO_148, I2C1_SDA, PULL_NONE),
	/* GPIO_149 - GPIO_152: Not available */
	/* Unused */
	PAD_NC(GPIO_153),
	/* Unused */
	PAD_NC(GPIO_154),
	/* APU_BT_ON_S3 TODO: Verify this pin */
	PAD_GPO(GPIO_155, HIGH),
	/* Unused */
	PAD_NC(GPIO_156),
	/* Unused */
	PAD_NC(GPIO_157),
};


void mainboard_program_early_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_reset, ARRAY_SIZE(gpio_set_stage_reset));
}

void romstage_program_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_rom, ARRAY_SIZE(gpio_set_stage_rom));
}

void mainboard_program_gpios(void)
{
	gpio_configure_pads(gpio_set_stage_ram, ARRAY_SIZE(gpio_set_stage_ram));
}
