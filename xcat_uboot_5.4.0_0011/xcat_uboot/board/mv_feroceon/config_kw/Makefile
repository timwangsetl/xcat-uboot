
include mvRules.mk

# Objects list

COMMON_OBJS =									\
	$(COMMON_DIR)/mvCommon.o						\
	$(COMMON_DIR)/bitOps.o							\
	$(COMMON_DIR)/mvWriteLog.o						\
	$(COMMON_DIR)/mvDebug.o							\
	$(COMMON_DIR)/mvHookReg.o						\
	$(COMMON_DIR)/mvNetDrvCommon.o						\
	$(COMMON_DIR)/mvStack.o

ERRATA_OBJS =									\
	$(GEN_ERRATA_DIR)/mvErrata.o

GND_OBJS =									\
	$(GND_DIR)/mvGnd.o							\
	$(GND_DIR)/mvGndReg.o

POOL_OBJS =									\
	$(POOL_DIR)/mvGenBuffPool.o						\
	$(POOL_DIR)/mvGenPool.o							\
	$(POOL_DIR)/mvGenSyncPool.o

MV_GEN_UT_OBJS =								\
	$(MV_GEN_UT_DIR)/mvGenUtAll.o						\
	$(MV_GEN_UT_DIR)/mvGenUtUart.o

OSSERVICES_OBJS =								\
	$(OSSERVICES_DIR)/mvOs.o

BOARD_ERRATA_OBJS = \
	$(BOARD_ERRATA_DIR)/mvBoardErrata.o	\
	$(BOARD_ERRATA_DIR)/mvErrataxCatCPU.o \
	$(BOARD_ERRATA_DIR)/mvErrataxCatPP.o

BOARD_OBJS =									\
	$(BOARD_ENV_DIR)/mvBoardEnvLib.o					\
	$(BOARD_ENV_DIR)/mvBoardEnvSpec.o

HAL_OBJS = $(HAL_CNTMR_DIR)/mvCntmr.o						\
	$(HAL_SFLASH_DIR)/mvSFlash.o						\
	$(HAL_ETHPHY_DIR)/mvEthPhy.o						\
	$(HAL_TWSI_DIR)/mvTwsi.o						\
	$(HAL_UART_DIR)/mvUart.o						\
	$(HAL_UART_DIR)/mvUartLog.o						\
	$(HAL_GPP_DIR)/mvGpp.o							\
	$(HAL_SPI_DIR)/mvSpi.o							\
	$(HAL_SPI_DIR)/mvSpiCmnd.o						\
	$(HAL_USB_DIR)/mvUsb.o							\
	$(HAL_XOR_DIR)/mvXor.o							\
	$(HAL_ETH_GBE_DIR)/mvEth.o						\
	$(HAL_PCIIF_DIR)/mvPciIf.o						\
	$(HAL_ETH_GBE_DIR)/mvEthDebug.o						\
	$(HAL_PEX_DIR)/mvPex.o							\
	$(HAL_CESA_DIR)/mvMD5.o							\
	$(HAL_FLASH_DIR)/mvFlash.o						\
	$(HAL_FLASH_DIR)/mvAmdFlash.o						\
	$(HAL_FLASH_DIR)/mvIntelFlash.o						\
	$(HAL_FLASH_DIR)/mvFlashCom.o						\
	$(HAL_DRAM_DIR)/mvDramIf.o						\
	$(HAL_SPD_DIR)/mvSpd.o							\
	$(HAL_MII_DIR)/mii.o

ifeq ($(MV_PRESTERA_SWITCH), y)
HAL_OBJS +=									\
	$(HAL_PRESTERA_DIR)/mvPrestera.o					\
	$(HAL_PRESTERA_DIR)/mvPresteraEEPROM.o					\
	$(HAL_DRAGONITE_DIR)/mvDragonite.o
endif

SOC_OBJS = 									\
	$(SOC_DIR)/mv_cmd.o							\
	$(SOC_DIR)/mv_dram.o							\
	$(SOC_DIR)/mv_main.o							\
	$(SOC_ENV_DIR)/mvCtrlEnvLib.o						\
	$(SOC_ENV_DIR)/mvCtrlEnvAddrDec.o					\
	$(SOC_SYS_DIR)/mvAhbToMbus.o						\
	$(SOC_SYS_DIR)/mvCpuIf.o						\
	$(SOC_SYS_DIR)/mvSysXor.o						\
	$(SOC_SYS_DIR)/mvSysDram.o						\
	$(SOC_SYS_DIR)/mvSysGbe.o						\
	$(SOC_SYS_DIR)/mvSysPex.o						\
	$(SOC_SYS_DIR)/mvSysUsb.o						\
	$(SOC_CPU_DIR)/mvCpu.o							\
	$(SOC_DEVICE_DIR)/mvDevWriteLog.o					\
	$(SOC_DEVICE_DIR)/mvDevice.o						\
	$(SOC_LIB_DIR)/mvSocLib.o

ifneq ($(MV_TINY_IMAGE), y)
SOC_OBJS +=									\
	$(HAL_PCIIF_DIR)/pci_util/mvPciUtils.o
endif

USP_OBJS =									\
	$(USP_DIR)/mv_flash.o							\
	$(USP_DIR)/mv_serial.o							\
	$(USP_DIR)/mv_pageTable.o						\
	$(USP_DIR)/mv_mon_init.o						\
	$(USP_DIR)/mv_pci.o							\
	$(USP_DIR)/mv_nand.o							\
	$(USP_DIR)/mv_egiga.o							\
	$(USP_DIR)/mv_eth.o							\
	$(USP_DIR)/mv_loadnet.o							\
	$(USP_DIR)/mv_protectionUnit.o						\
	$(USP_DIR)/mv_i2c.o							\
	$(USP_DIR)/mv_usb.o

ifeq ($(MV_PRESTERA_SWITCH), y)
USP_OBJS +=									\
	$(USP_DIR)/mv_prestera_switch.o
SOC_OBJS +=									\
	$(SOC_DIR)/mv_prestera_cmd.o						\
	$(SOC_DIR)/mv_test_cmd.o
endif

ifeq ($(NAND_BOOT), y)
USP_OBJS +=									\
	$(USP_DIR)/nBootloader.o
endif

SOBJS =										\
	$(USP_DIR)/jump.o							\
	$(SOC_DIR)/platform.o							\
	$(SOC_DIR)/mvCpuMaxCurrent.o						\
	$(HAL_TWSI_DIR)/mvTwsiEeprom.o						\
	$(SOC_SYS_DIR)/mvCpuIfInit.o

ifeq ($(NAND_BOOT), y)
SOBJS +=									\
	../mv_kw/nBootstrap.o
endif

OBJS =										\
	$(COMMON_OBJS)								\
	$(ERRATA_OBJS)								\
	$(GND_OBJS)								\
	$(POOL_OBJS)								\
	$(OSSERVICES_OBJS)							\
	$(HAL_OBJS)								\
	$(SOC_OBJS)								\
	$(BOARD_ERRATA_OBJS)						\
	$(BOARD_OBJS)								\
	$(USP_OBJS)									\
	$(MV_GEN_UT_OBJS)

LIB =										\
	lib$(BOARD).a

all:    $(LIB)

$(LIB): .depend $(SOBJS) $(OBJS)
	$(AR) crv $@ $(SOBJS) $(OBJS)

#########################################################################

.depend:        Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c) $(USB_OBJS:.o=.c)
	$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) $(USB_OBJS:.o=.c) > $@

sinclude .depend

#########################################################################

