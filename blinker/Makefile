# Check $ARCH and $CROSS_COMPILE before running make
# CROSS_COMPILE should not be set for native compiler
#
ifneq ($(KERNELRELEASE),)

obj-m := blinker.o
ccflags-y := -DDEBUG

else
KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	make -C $(KDIR) M=$(PWD) modules
	make clean
	make loadit
clean:
	-rm *.mod.c *.o .*.cmd Module.symvers modules.order 2>/dev/null || :
	-rm -rf .tmp_versions 2>/dev/null || :

.PHONY: clean

loadit: blinker.ko
	insmod blinker.ko limit=5
	#find /sys/bus/platform -name 'bln*'
	-ls -l /sys/bus/platform/drivers/bln

unloadit:
	-rmmod blinker

.PHONY: loadit unloadit

endif
