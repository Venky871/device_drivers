#include <linux/device.h>
#include <linux/mod_devicetable.h> /* of_* */

struct xbus_device {
	int 	id;
	struct device dev;
	/* device resources */
};

#define to_xbus_device(p)	container_of(p, struct xbus_device, dev)

/* dummy functions */
#define xbus_device_register(pdev)	device_register(&(pdev)->dev)
#define xbus_device_unregister(pdev)	device_unregister(&(pdev)->dev)

struct xbus_driver {
	struct device_driver drv;
	/* driver state */
};


extern struct bus_type xbus_bus_type;

#define to_xbus_driver(p)	container_of(p, struct xbus_driver, drv)

#define xbus_driver_register(pdrv)	_xbus_driver_register((pdrv), THIS_MODULE)
extern int _xbus_driver_register(struct xbus_driver *d, struct module *m);

/* dummy functions */
#define xbus_driver_unregister(pdrv)	driver_unregister(&(pdrv)->drv)
