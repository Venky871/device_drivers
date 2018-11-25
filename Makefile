# Check $ARCH and $CROSS_COMPILE before running make
# CROSS_COMPILE should not be set for native compiler
#
#
ifneq ($(KERNELRELEASE),)
# make -C .. M= ... will reach here. Why?

obj-m := sachin.o deepa.o bhalu.o

else
# ------------------------------------------------
# Outermost make. saves typing

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	make -C $(KDIR) M=$(PWD) modules
	echo $(modules)
	make clean

%.ko:
	make -C $(KDIR) M=$(PWD) $@
	 echo $@
clean:
	-rm *.mod.c *.o .*.cmd Module.symvers modules.order || :
	-rm -rf .tmp_versions || :

.PHONY: clean

endif
