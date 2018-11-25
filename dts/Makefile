all: mcp3008.dtbo mcp

mcp3008.dtbo: mcp3008.dts
	dtc -W no-unit_address_vs_reg -@ -O dtb -o mcp3008.dtbo -I dts mcp3008.dts

reloadoverlay:
	dtoverlay -r mcp3008
	dtoverlay ./mcp3008.dtbo

mcp : mcp.c
	gcc -o $@ $?

/dev/spidev0.0:
	@dtparam spi=on

/sys/bus/iio/devices/iio\:device0 : /dev/spidev0.0 reloadoverlay
	ls -l $@
