/*
Tutorial device driver for a chip called bhalu that
exists in a computer. bhalu controls the color of a LED
in the front panel.

# insmod bhalu.ko debug=1
# ls /sys/bus/platform/bhalu
# cat /sys/devices/bhalu1/color
*/

#include <linux/module.h>
#include <linux/device.h>

#include <linux/platform_device.h>

static int debug = 0;

module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level, 0=silent");

static const char *color_names[] = {
#define COLOR_OFF	0
	[COLOR_OFF] = "off",
#define COLOR_RED	1
	[COLOR_RED]   = "red",
#define COLOR_GREEN	2
	[COLOR_GREEN] = "green",
#define COLOR_BLUE	3
	[COLOR_BLUE]  = "blue",
#define COLOR_WHITE	4
	[COLOR_WHITE] = "white",
};

/* 1. extend basic device and driver definitions */

struct bhalu_driver {
	struct driver *driver;
};

#define to_bhalu_driver(x)	container_of(x, struct bhalu_driver, driver)

struct bhalu_device {
	char *name;
	int  color;
	struct bhalu_driver *drv;
	struct device dev;
};

#define to_bhalu_device(x)	container_of(x, struct bhalu_device, dev)

/* 3. define getter/setter functions for each device attribute */

static ssize_t get_bhalu_name(struct device *dev, struct device_attribute *attr, char *resp)
{
	struct bhalu_device *bhalu = to_bhalu_device(dev);

	return snprintf(resp, 40, "%s\n", bhalu->name);
}

static DEVICE_ATTR(name, S_IRUGO, get_bhalu_name, NULL);

static ssize_t get_bhalu_color(struct device *dev, struct device_attribute *attr, char *resp)
{
	struct bhalu_device *bhalu = to_bhalu_device(dev);

	return snprintf(resp, 40, "%s\n", color_names[bhalu->color]);
}

static ssize_t set_bhalu_color(struct device *dev, struct device_attribute *attr, const char *newval, size_t valsize)
{
	struct bhalu_device *bhalu = to_bhalu_device(dev);
	char newcolor[10];
	int val;

	if (sscanf(newval, "%7s", newcolor) != 1)
		return -EINVAL;
	dev_alert(dev, "changing color from %s to %s ...\n", color_names[bhalu->color], newcolor);
	for (val = COLOR_OFF; val <= COLOR_WHITE; val++)
		if (strcmp(color_names[val], newcolor) == 0) {
			bhalu->color = val;
			return valsize;
		}
	return -EINVAL;
}

static DEVICE_ATTR(color, S_IRUGO|S_IWUSR, get_bhalu_color, set_bhalu_color);

static struct attribute *bhalu_attrs[] = {
	&dev_attr_name.attr,
	&dev_attr_color.attr,
	NULL
};

static struct attribute_group bhalu_basic_attributes = {
	.attrs = bhalu_attrs,
};

static const struct attribute_group *bhalu_all_attributes[] = {
 	&bhalu_basic_attributes,
 	NULL
};

static void bhalu_release(struct device *tdev)
{
	dev_alert(tdev, "releasing bhalu %s\n", tdev->kobj.name);
}

/* 2. provide register and unregister functions */

static void bhalu_register_device(struct bhalu_device *pbhalu)
{
	int err;
	struct device *dev = &pbhalu->dev;

	pr_info("registering bhalu device %s\n", pbhalu->name);
	dev->init_name = pbhalu->name;
	dev->release = bhalu_release;
	dev->groups = bhalu_all_attributes;
	err = device_register(dev);
	if (err)
		pr_err("failed to register device %s error %d\n", pbhalu->name, err);
#if 0
	else {
		/* if not using groups create file for each attribute */
		device_create_file(dev, &dev_attr_name);
		device_create_file(dev, &dev_attr_color);
	}
#endif
}

static void bhalu_unregister_device(struct bhalu_device *pbhalu)
{
	struct device *dev = &pbhalu->dev;

	dev_alert(dev, "unregistering bhalu device %s\n", pbhalu->name);
	device_remove_file(dev, &dev_attr_name);
	device_remove_file(dev, &dev_attr_color);
	device_unregister(dev);
}

static struct bhalu_device bhalu = {
	.name = "bhalu1",
};

static int __init bhalu_init(void)
{
	int ret = 0;

	pr_info("initializing module bhalu debug=%d ..\n", debug);
	bhalu_register_device(&bhalu);
	return ret;
}

static void __exit bhalu_exit(void)
{
	pr_info("exiting module bhalu debug=%d...\n", debug);
	bhalu_unregister_device(&bhalu);
}

module_init(bhalu_init);
module_exit(bhalu_exit);
MODULE_DESCRIPTION("bhalu the happy and colorful bear");
MODULE_LICENSE("GPL");
