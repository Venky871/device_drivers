# Check $ARCH and $CROSS_COMPILE before running make
# CROSS_COMPILE should not be set for native compiler
#

ifneq ($(KERNELRELEASE),)

obj-m := goonj.o

else
KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	make -C $(KDIR) M=$(PWD) modules
	        make clean 
	make /run/udev/rules.d/99-goonj.rules	
	make loadit
	make  add 
clean:
	-rm *.mod.c *.o .*.cmd Module.symvers modules.order 2>/dev/null || :
	-rm -rf .tmp_versions 2>/dev/null || :
.PHONY: clean

/run/udev/rules.d/99-goonj.rules: 99-goonj.rules
	test -d ${@D} || sudo mkdir -p ${@D} || :
	cp $? $@

loadit: goonj.ko /run/udev/rules.d/99-goonj.rules
	insmod goonj.ko
	find /sys -name 'goonj*'
	-ls -l /dev/goonj1 /dev/gpio/gp1
	-ls -l /sys/devices/virtual/misc/goonj1/

unloadit:
	-rmmod goonj
	-rm /run/udev/rules.d/99-goonj.rules

.PHONY: loadit unloadit

change add remove : /sys/devices/virtual/misc/goonj1/uevent
	echo $@ >$<

.PHONY: change add remove

endif
