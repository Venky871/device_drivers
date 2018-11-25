# Check $ARCH and $CROSS_COMPILE before running make
# CROSS_COMPILE should not be set for native compiler
#
#
ifneq ($(KERNELRELEASE),)
# make -C .. M= ... will reach here. Why?

obj-m := xbus.o xbdrv.o
ccflags-y := -DDEBUG

else
# ------------------------------------------------
# Outermost make. saves typing

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	make -C $(KDIR) M=$(PWD) modules
	make clean

%.ko:
	make -C $(KDIR) M=$(PWD) $@

clean:
	-rm *.mod.c *.o .*.cmd Module.symvers modules.order 2>/dev/null || :
	-rm -rf .tmp_versions 2>/dev/null || :

.PHONY: clean

endif
