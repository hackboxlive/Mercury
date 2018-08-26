#include <linux/init.h>
#include <linux/module.h>

#include "device_file.h"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Sarv Shakti Singh, hackboxlive");

static int my_init(void)
{
	int result = 0;
	printk( KERN_NOTICE "Mercury: Initialization started" );
	result = register_device();
	return 0;
}

static void my_exit(void)
{
	return;
}

module_init(my_init);
module_exit(my_exit);


