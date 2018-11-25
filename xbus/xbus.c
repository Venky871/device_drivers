/*
 * Example - Creating a new bus
 */
#include <linux/module.h>
#include <linux/device.h> /* device model defs */
#include <linux/string.h>
#include "xbus_device.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("xbus example");

static int xbus_match(struct device *dev, struct device_driver *drv)
{
	pr_devel("xbus match dev %s drv %s\n", dev_name(dev), drv->name);
	return sysfs_streq(dev_name(dev), drv->name);
}

struct bus_type xbus_bus_type = {
	.name = "xbus",
	.match = xbus_match,
};

EXPORT_SYMBOL_GPL(xbus_bus_type);

int _xbus_driver_register(struct xbus_driver *xdrv, struct module *owner)
{
	xdrv->drv.owner = owner;
	xdrv->drv.bus = &xbus_bus_type;
	return driver_register(&xdrv->drv);
}
EXPORT_SYMBOL_GPL(_xbus_driver_register);

static int __init xbus_init(void)
{
	pr_devel("xbus inited\n");
	bus_register(&xbus_bus_type);
	return 0;
}
module_init(xbus_init);

static void __exit xbus_exit(void)
{
	pr_devel("xbus exiting..");
	bus_unregister(&xbus_bus_type);
}
module_exit(xbus_exit);
