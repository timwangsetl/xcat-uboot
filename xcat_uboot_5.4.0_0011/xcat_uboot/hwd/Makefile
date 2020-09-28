include $(TOPDIR)/config.mk
#include $(TOPDIR)/board/88f5181/mvRules.mk
include $(TOPDIR)/board/mv_feroceon/config_kw/mvRules.mk

HWD_DIR := $(TOPDIR)/hwd

LIB	= libhwd.a

AOBJS	=

OBJS	+= $(HWD_OBJ)

# CFLAGS += -I$(HWD_DIR)

all:	$(LIB) $(AOBJS) 

$(LIB): .depend $(OBJS) 
	$(AR) crv $@ $(OBJS) 

environment.o: environment.c ../tools/envcrc
	$(CC) $(AFLAGS) -Wa,--no-warn $(CFLAGS) \
		-DENV_CRC=$(shell ../tools/envcrc) \
		-c -o $@ environment.c

../tools/envcrc:
	$(MAKE) -C ../tools $(CFLAGS)

#########################################################################

.depend:	Makefile $(AOBJS:.o=.S) $(COBJS:.o=.c)
		$(CC) -M -I$(HWD_DIR) $(CFLAGS) $(AOBJS:.o=.S) $(COBJS:.o=.c) > $@

sinclude .depend

#########################################################################




