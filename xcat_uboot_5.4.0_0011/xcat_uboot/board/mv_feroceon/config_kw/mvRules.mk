# This flags will be used only by the Marvell arch files compilation.
include $(TOPDIR)/config.mk
include $(TOPDIR)/include/config.mk

# General definitions
CPU_ARCH    = ARM
CHIP        = kw
VENDOR      = Marvell
ENDIAN      = LE
LD_ENDIAN   = -EL

ifeq ($(BIG_ENDIAN),y)
ENDIAN      = BE
LD_ENDIAN   = -EB
endif

# Main directory structure
SRC_PATH           = $(TOPDIR)/board/mv_feroceon
HAL_DIR            = $(SRC_PATH)/mv_hal
COMMON_DIR         = $(SRC_PATH)/common
GEN_ERRATA_DIR     = $(SRC_PATH)/common/errata
GND_DIR            = $(SRC_PATH)/common/gnd
POOL_DIR           = $(SRC_PATH)/common/pool
MV_GEN_UT_DIR      = $(SRC_PATH)/common/mvGenUt
USP_DIR            = $(SRC_PATH)/USP
SOC_DIR            = $(SRC_PATH)/mv_$(CHIP)
CHIP_FAMILY_DIR    = $(SOC_DIR)/$(CHIP)_family
SOC_ENV_DIR        = $(CHIP_FAMILY_DIR)/ctrlEnv
SOC_SYS_DIR        = $(CHIP_FAMILY_DIR)/ctrlEnv/sys
SOC_LIB_DIR        = $(CHIP_FAMILY_DIR)/soc
SOC_CPU_DIR        = $(CHIP_FAMILY_DIR)/cpu
SOC_DEVICE_DIR     = $(CHIP_FAMILY_DIR)/device
BOARD_ERRATA_DIR   = $(CHIP_FAMILY_DIR)/boardEnv/errata
BOARD_ENV_DIR      = $(CHIP_FAMILY_DIR)/boardEnv
#USP_ETH_SWITCH_DIR = $(USP_DIR)/ethswitch

# HAL components
HAL_ETHPHY_DIR     = $(HAL_DIR)/eth-phy
HAL_FLASH_DIR      = $(HAL_DIR)/norflash
HAL_PCI_DIR        = $(HAL_DIR)/pci
HAL_PCIIF_DIR      = $(HAL_DIR)/pci-if
HAL_SFLASH_DIR     = $(HAL_DIR)/sflash
HAL_CNTMR_DIR      = $(HAL_DIR)/cntmr
HAL_GPP_DIR        = $(HAL_DIR)/gpp
HAL_PEX_DIR        = $(HAL_DIR)/pex
HAL_TWSI_DIR       = $(HAL_DIR)/twsi
HAL_TWSI_ARCH_DIR  = $(HAL_TWSI_DIR)/Arch$(CPU_ARCH)
HAL_ETH_DIR        = $(HAL_DIR)/eth
HAL_ETH_GBE_DIR    = $(HAL_ETH_DIR)/gbe
HAL_UART_DIR       = $(HAL_DIR)/uart
HAL_XOR_DIR        = $(HAL_DIR)/xor
HAL_USB_DIR        = $(HAL_DIR)/usb
HAL_MFLASH_DIR     = $(HAL_DIR)/mflash
HAL_SPI_DIR        = $(HAL_DIR)/spi
HAL_CESA_DIR	   = $(HAL_DIR)/cesa
HAL_PRESTERA_DIR   = $(HAL_DIR)/prestera
HAL_DRAGONITE_DIR  = $(HAL_DIR)/dragonite
HAL_PSS_DIR        = $(SRC_PATH)/pss
HAL_MII_DIR        = $(HAL_DIR)/mii
HAL_DRAM_DIR       = $(HAL_DIR)/ddr2
HAL_SPD_DIR        = $(HAL_DRAM_DIR)/spd
#HAL_TS_DIR	   = $(HAL_DIR)/ts
#HAL_SATA_DIR      = $(HAL_DIR)/sata
#HAL_RTC_DIR       = $(HAL_DIR)/rtc/integ_rtc
#HAL_SATA_CORE_DIR = $(HAL_DIR)/sata/CoreDriver
#HAL_IDMA_DIR      = $(HAL_DIR)/idma
#HAL_AUDIO_DIR	   = $(HAL_DIR)/audio

# OS services
OSSERVICES_DIR     = $(SRC_PATH)/uboot_oss

# Internal definitions
MV_DEFINE = -DMV_UBOOT -DMV_CPU_$(ENDIAN) -DMV_$(CPU_ARCH)

# Internal include path
HAL_PATH = -I$(HAL_DIR)								\
	-I$(HAL_PRESTERA_DIR)							\
	-I$(HAL_PRESTERA_DIR)/common						\
	-I$(HAL_PRESTERA_DIR)/hwIf						\
	-I$(HAL_PRESTERA_DIR)/sdma						\
	-I$(HAL_PRESTERA_DIR)/util						\
	-I$(HAL_PRESTERA_DIR)/vlan						\
	-I$(HAL_MII_DIR)							\
	-I$(HAL_PSS_DIR)							\
	-I$(HAL_ETH_DIR)							\
	-I$(SRC_PATH)								\
	-I$(HAL_PEX_DIR)							\
	-I$(HAL_DRAGONITE_DIR)							\
	-I$(HAL_ETHPHY_DIR)							\
	-I$(HAL_XOR_DIR)							\
	-I$(HAL_FLASH_DIR)							\
	-I$(HAL_CESA_DIR)							\
	-I$(HAL_UART_DIR)

COMMON_PATH        = -I$(COMMON_DIR)
GEN_ERRATA_PATH    = -I$(GEN_ERRATA_DIR)
GND_PATH           = -I$(GND_DIR)
POOL_PATH          = -I$(POOL_DIR)
MV_GEN_UT_PATH     = -I$(MV_GEN_UT_DIR)
OSSERVICES_PATH    = -I$(OSSERVICES_DIR)
USP_PATH           = -I$(USP_DIR)
SOC_PATH	   = -I$(CHIP_FAMILY_DIR) -I$(SOC_DIR) -I$(SOC_ENV_DIR) -I$(SOC_SYS_DIR) -I$(SOC_CPU_DIR) -I$(SOC_DEVICE_DIR) -I$(SOC_LIB_DIR)
BOARD_PATH	   = -I$(BOARD_ENV_DIR)
BOARD_ERRATA_PATH = -I$(BOARD_ERRATA_DIR)

IMPLICIT_RULES_FLAGS = $(MV_DEFINE)						\
	$(OSSERVICES_PATH)							\
	$(HAL_PATH)								\
	$(COMMON_PATH)								\
	$(GEN_ERRATA_PATH)							\
	$(GND_PATH)								\
	$(POOL_PATH)								\
	$(MV_GEN_UT_PATH)							\
	$(USP_PATH)								\
	$(SOC_PATH)								\
	$(BOARD_PATH)								\
	$(BOARD_ERRATA_PATH)						\
	$(SYS_PATH)

CFLAGS   += $(IMPLICIT_RULES_FLAGS)

AFLAGS   += $(IMPLICIT_RULES_FLAGS)

