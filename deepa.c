
#include <linux/module.h>

static int flag =100 ;
module_param(flag , int, 0);
MODULE_PARM_DESC(flag, "Simple flag value");

static int __init simple_init(void)
{
	pr_info("Namaste %s %d ..\n", module_name(THIS_MODULE), flag);
	return 0;
}

static void __exit simple_exit(void)
{
	pr_info("Shubham %s %d...\n", module_name(THIS_MODULE), flag);
}

module_init(simple_init);
module_exit(simple_exit);
MODULE_LICENSE("GPL");
