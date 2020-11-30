/*
 *  8devices Rambutan board support
 *
 *  Copyright (C) 2017 Mantas Pucka <mantas@8devices.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <linux/platform/ar934x_nfc.h>
#include <linux/ar8216_platform.h>
#include <linux/platform_data/i2c-designware.h>
#include <linux/platform_data/phy-at803x.h>
#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-nfc.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

#define QCA955X_I2C_BASE (AR71XX_APB_BASE + 0x00018000)
#define QCA955X_I2C_SIZE 0x100

static struct at803x_platform_data rambutan_ar8032_data = {
	.has_reset_gpio = 1,
	.reset_gpio = 17,
};

static struct mdio_board_info rambutan_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.mdio_addr = 0,
		.platform_data = &rambutan_ar8032_data,
	},
};

static struct at803x_platform_data rambutan_ar8033_data = {
	.has_reset_gpio = 1,
	.override_sgmii_aneg = 1,
	.reset_gpio = 23,
};

static struct mdio_board_info rambutan_mdio1_info[] = {
	{
		.bus_id = "ag71xx-mdio.1",
		.mdio_addr = 0,
		.platform_data = &rambutan_ar8033_data,
	},
};

static struct resource rambutan_i2c_resources[] = {
	{
		.start  = QCA955X_I2C_BASE,
		.end    = QCA955X_I2C_BASE + QCA955X_I2C_SIZE - 1,
		.flags  = IORESOURCE_MEM,
	},
	{
		.start  = ATH79_MISC_IRQ(24),
		.end    = ATH79_MISC_IRQ(24),
		.flags  = IORESOURCE_IRQ,
	},
};

static struct dw_i2c_platform_data rambutan_i2c_pdata = {
	.i2c_scl_freq	= 100000,
};

static struct platform_device rambutan_i2c_device = {
	.name		= "i2c_designware",
	.id		= -1,
	.resource	= rambutan_i2c_resources,
	.num_resources	= ARRAY_SIZE(rambutan_i2c_resources),
	.dev = {
		.platform_data	= &rambutan_i2c_pdata,
	},
};

static struct spi_board_info rambutan_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "spidev",
	},
	{
		.bus_num	= 0,
		.chip_select	= 1,
		.max_speed_hz   = 25000000,
		.modalias	= "spidev",
	},
	{
		.bus_num	= 0,
		.chip_select	= 2,
		.max_speed_hz   = 25000000,
		.modalias	= "spidev",
	}
};

static struct ath79_spi_platform_data rambutan_spi_data = {
		.bus_num	= 0,
		.num_chipselect	= 3,
};

static struct resource rambutan_uart1_resources[] = {
	{
		.start	= QCA955X_UART1_BASE,
		.end	= QCA955X_UART1_BASE + QCA955X_UART1_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= ATH79_MISC_IRQ(6),
		.end	= ATH79_MISC_IRQ(6),
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device rambutan_uart1_device = {
	.name		= "ar933x-uart",
	.id		= -1,
	.resource	= rambutan_uart1_resources,
	.num_resources	= ARRAY_SIZE(rambutan_uart1_resources),
};

static void __init rambutan_setup(void)
{
	ath79_nfc_set_ecc_mode(AR934X_NFC_ECC_HW);
	ath79_register_nfc();
	ath79_register_usb();
	ath79_register_pci();
	ath79_register_wmac_simple();
	platform_device_register(&rambutan_uart1_device);
	platform_device_register(&rambutan_i2c_device);
	ath79_register_spi(&rambutan_spi_data, rambutan_spi_info, 3);

	mdiobus_register_board_info(rambutan_mdio0_info,
				    ARRAY_SIZE(rambutan_mdio0_info));
	mdiobus_register_board_info(rambutan_mdio1_info,
				    ARRAY_SIZE(rambutan_mdio1_info));
	ath79_register_mdio(0, 0x0);
	ath79_register_mdio(1, 0x0);

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_register_eth(0);

	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_SGMII;
	ath79_eth1_data.phy_mask = BIT(0);
	ath79_eth1_data.mii_bus_dev = &ath79_mdio1_device.dev;
	ath79_eth1_data.enable_sgmii_fixup = 1;
	ath79_eth1_pll_data.pll_1000 = 0x17000000;
	ath79_eth1_pll_data.pll_10 = 0x1313;
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_RAMBUTAN, "RAMBUTAN", "8devices Rambutan board",
	     rambutan_setup);
