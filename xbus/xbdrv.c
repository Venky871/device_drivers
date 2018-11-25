/*
 * Demonstration of timer handling in a linux driver module
 */
#include <linux/module.h>
#include <linux/timer.h>
#include "xbus_device.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Driver for xbing devices on xbus");

#define XBLINKER_DRIVER_NAME "xb"

static int limit = 5;
module_param(limit, int, 0);
MODULE_PARM_DESC(limit, "wakeup limit");

static unsigned int interval = 2;

static struct timer_list xbtimer;
static int counter = 0;

#define secs_to_jiffies(i)	(msecs_to_jiffies((i)*1000))

static void xbdrv_timeout(unsigned long dummy)
{
	static bool level = 0;

	counter++;
	level = 1 - level; /* flip state */

	pr_devel("xb: wakeup count %d, level %d\n", counter, level);
	if (counter < limit)
		mod_timer(&xbtimer, jiffies+secs_to_jiffies(interval));
	else
		pr_info("xb: wakeup limit reached\n");
}

static int xbdrv_probe(struct device *pdev)
{
	struct xbus_device *pxb = to_xbus_device(pdev);

	dev_dbg(pdev, "id %d\n", pxb->id);
	return 0;
}

static int xbdrv_remove(struct device *pdev)
{
	struct xbus_device *pxb = to_xbus_device(pdev);

	dev_dbg(pdev, "id %d\n", pxb->id);
	return 0;
}

static struct of_device_id of_xb_match[] = {
	{ .compatible	= "xb,led", },
	{ .compatible	= "xb", },
	{},
};
MODULE_DEVICE_TABLE(of, of_xb_match);

static struct xbus_driver xb_driver = {
	.drv = {
		.probe  = xbdrv_probe,
		.remove = xbdrv_remove,

		.name = XBLINKER_DRIVER_NAME,
		.of_match_table = of_xb_match,
	}
};

static int __init xbdrv_load(void)
{
	int err;

	pr_devel("xb: loading limit=%d interval=%d\n", limit, interval);
	err = xbus_driver_register(&xb_driver);
	if (unlikely(err)) {
		pr_err("xb: loading error %d...\n", err);
		return err;
	}
	setup_timer(&xbtimer, xbdrv_timeout, 0);
	err = mod_timer(&xbtimer, secs_to_jiffies(interval));
	if (unlikely(err)) {
		pr_err("xb: timer error %d...\n", err);
		xbus_driver_unregister(&xb_driver);
		del_timer(&xbtimer);
		return err;
	}
	return 0;
}
module_init(xbdrv_load);

static void __exit xbdrv_unload(void)
{
	int err;

	pr_devel("xb: unloading limit=%d\n", limit);
	xbus_driver_unregister(&xb_driver);
	err = del_timer(&xbtimer);
	if (err)
		pr_err("xb: timer delete error %d...\n", err);
}
module_exit(xbdrv_unload);
