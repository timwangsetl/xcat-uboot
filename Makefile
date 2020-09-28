
ifndef TOPDIR
export TOPDIR = $(shell (cd ./ && pwd -P))
endif

export PATH = /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/sbin:/opt/arm-mv5sft-linux-gnueabi_SW3.2/bin

ifndef BINPATH
export BINPATH=$(TOPDIR)/../binfile
endif

ifndef BOOTPATH
export BOOTPATH=$(TOPDIR)/xcat_uboot_5.4.0_0011/xcat_uboot
endif

all: boot_build

build: 
	@echo making bootloader
	cd $(BOOTPATH) && make 
	cp $(BOOTPATH)/u-boot-1.1.4-xCat98DX-320MHz-spi.bin $(BINPATH)/u-boot.bin 

boot_build: 
	@echo making bootloader
	cd $(BOOTPATH) && make mrproper
	cd $(BOOTPATH) && make distclean
	cd $(BOOTPATH) && make xCat_total_build_spi TOOLCHAIN=3.2
	cd $(BOOTPATH) && make dep
	cd $(BOOTPATH) && make clean
	cd $(BOOTPATH) && make 
	cp $(BOOTPATH)/u-boot-1.1.4-xCat98DX-320MHz-spi.bin $(BINPATH)/u-boot.bin 

clean: 
	cd $(BOOTPATH) && make distclean

