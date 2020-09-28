#
# (C) Copyright 2004
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk
include $(TOPDIR)/board/mv_feroceon/config_kw/mvRules.mk

LIB	= libcommon.a

AOBJS	= boot_vxworks.o

COBJS	= main.o bedbug.o circbuf.o \
	  cmd_autoscript.o \
	  cmd_bdinfo.o cmd_bedbug.o cmd_boot.o cmd_bootm.o \
	  cmd_cache.o cmd_console.o \
	  cmd_date.o cmd_diag.o cmd_display.o cmd_doc.o \
	  cmd_eeprom.o cmd_elf.o cmd_ext2.o \
	  cmd_fat.o cmd_flash.o \
	  cmd_i2c.o cmd_ide.o cmd_itest.o cmd_jffs2.o \
	  cmd_load.o cmd_log.o \
	  cmd_mem.o cmd_mii.o cmd_misc.o \
	  cmd_nand.o cmd_net.o cmd_nvedit.o \
	  cmd_pci.o \
	  cmd_reginfo.o cmd_reiser.o cmd_spi.o \
	  cmd_usb.o cmd_vfd.o \
	  command.o console.o devices.o dlmalloc.o docecc.o \
	  environment.o env_common.o \
	  env_nand.o env_dataflash.o env_flash.o env_eeprom.o \
	  env_nvram.o env_nowhere.o \
	  exports.o \
	  flash.o ft_build.o \
	  hush.o kgdb.o lists.o \
	  memsize.o miiphyutil.o \
	  s_record.o serial.o soft_i2c.o soft_spi.o \
	  usb.o usb_kbd.o usb_storage.o

OBJS	= $(AOBJS) $(COBJS)

CPPFLAGS += -I..

all:	$(LIB) $(AOBJS)

$(LIB): .depend $(OBJS)
	$(AR) crv $@ $(OBJS)

environment.o: environment.c ../tools/envcrc
	$(CC) $(AFLAGS) -Wa,--no-warn \
		-DENV_CRC=$(shell ../tools/envcrc) \
		-c -o $@ environment.c

../tools/envcrc:
	$(MAKE) -C ../tools

#########################################################################

.depend:	Makefile $(AOBJS:.o=.S) $(COBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(AOBJS:.o=.S) $(COBJS:.o=.c) > $@

sinclude .depend

#########################################################################
