/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * Derived from drivers/spi/mpc8xxx_spi.c
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */
#include <common.h>
#if defined(CONFIG_CMD_SPI) || defined(CONFIG_CMD_SF)
#include <malloc.h>
#include <spi.h>
#include "mvSysSpiApi.h"
#include "mvOs.h"
#include "spi/mvSpi.h"
#include "spi/mvSpiSpec.h"
//#include <asm/arch/kirkwood.h>
//#include <asm/arch/spi.h>
//#include <asm/arch/mpp.h>
// spi_claim_bus
// spi_xfer
// spi_release_bus
// spi_free_slave
//
// spi_init
// spi_setup_slave

//static struct kwspi_registers *spireg = (struct kwspi_registers *)KW_SPI_BASE;

void spi_init (void)
{
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				unsigned int max_hz, unsigned int mode)
{
	struct spi_slave *slave;
//	u32 data;

	if (!spi_cs_is_valid(bus, cs))
		return NULL;

	slave = malloc(sizeof(struct spi_slave));
	if (!slave)
		return NULL;

	slave->bus = bus;
	slave->cs = cs;

// 	writel(~KWSPI_CSN_ACT | KWSPI_SMEMRDY, &spireg->ctrl);
//
// 	/* calculate spi clock prescaller using max_hz */
// 	data = ((CONFIG_SYS_TCLK / 2) / max_hz) & KWSPI_CLKPRESCL_MASK;
// 	data |= 0x10;
//
// 	/* program spi clock prescaller using max_hz */
// 	writel(KWSPI_ADRLEN_3BYTE | data, &spireg->cfg);
// 	debug("data = 0x%08x \n", data);
//
// 	writel(KWSPI_SMEMRDIRQ, &spireg->irq_cause);
// 	writel(KWSPI_IRQMASK, spireg->irq_mask);

	mvSysSpiInit(0,max_hz);

	return slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	free(slave);
}

int spi_claim_bus(struct spi_slave *slave)
{
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
}

#ifndef CONFIG_SPI_CS_IS_VALID
/*
 * you can define this function board specific
 * define above CONFIG in board specific config file and
 * provide the function in board specific src file
 */
int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
    return ((bus == CONFIG_ENV_SPI_BUS) && (cs == CONFIG_ENV_SPI_CS));
//|| cs == 1)
}
#endif

void spi_cs_activate(struct spi_slave *slave)
{
	mvSpiCsAssert(0);
	//writel(readl(&spireg->ctrl) | KWSPI_IRQUNMASK, &spireg->ctrl);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	mvSpiCsDeassert(0);
	//writel(readl(&spireg->ctrl) & KWSPI_IRQMASK, &spireg->ctrl);
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout,
		 void *din, unsigned long flags)
{
	MV_STATUS ret;
	MV_U8* pdout = (MV_U8*)dout;
	MV_U8* pdin = (MV_U8*)din;
	int tmp_bitlen = bitlen;
    //
	//printf("spi_xfer: slave %u:%u dout %08X din %08X bitlen %u\n",
	  //    slave->bus, slave->cs, dout, din, bitlen);

	if (flags & SPI_XFER_BEGIN)
		spi_cs_activate(slave);

	/* Verify that the SPI mode is in 8bit mode */
	MV_REG_BIT_RESET(MV_SPI_IF_CONFIG_REG(0), MV_SPI_BYTE_LENGTH_MASK);

	/* TX/RX in 8bit chanks */

	while (tmp_bitlen > 0)
	{
		/* Transmitted and wait for the transfer to be completed */
		if ((ret = mvSpi8bitDataTxRx(0,*pdout, pdin)) != MV_OK)
			return ret;

		/* increment the pointers */
		//printf("in=[0x%x]",*pdin);
		if (pdin)
			pdin++;
		//printf("out=[0x%x]",*pdout);
		if (pdout)
			pdout++;

		tmp_bitlen-=8;
	}

	if (flags & SPI_XFER_END)
		spi_cs_deactivate(slave);

	return 0;
}
#endif
