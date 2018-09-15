#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/interrupt.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/types.h>
#include <linux/major.h>
#include <linux/file.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/buffer_head.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/mm.h>

#define PARTITION_NAME "/dev/sda1"

#define DELAY(); \
	printk(KERN_INFO "filter: FUNCTION=%s LINE=%d\n", __FUNCTION__, __LINE__); \
		set_current_state(TASK_UNINTERRUPTIBLE); \
		schedule_timeout(HZ * 5);

struct block_device *bd;

make_request_fn *orig_mrqfn;

static void filter_mrqfn(struct request_queue *rq, struct bio *b)
{
	orig_mrqfn(rq, b); /* calling the original make_request_fn() */
}

/*
 * set_mrqfn() - Change the original make_request_fn() to our
 * modules request function
 */
static void set_mrqfn(void)
{
	int ret;
	struct super_block *sb =  bd->bd_super;
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	DELAY();
	/* lock filesystem to prevent any further changes */
	sb = freeze_bdev(bd);
	if (sb)
		printk(KERN_INFO "filter: freez block device\n");
	DELAY();

	if (bd->bd_disk->queue->make_request_fn == filter_mrqfn) {
		printk(KERN_INFO "filter: modules request function is already active\n");
	} else {
		/* save the pointer to the original make_request_fn() */
		orig_mrqfn = bd->bd_disk->queue->make_request_fn;
		/* replace the original with our modules request function */
		bd->bd_disk->queue->make_request_fn = filter_mrqfn;
	}

	/* unlock filesystem */
	thaw_bdev(bd, sb);
}

/*
 * restore_mrqfn() - Restore the original make_request_fn()
 */
static void restore_mrqfn(void)
{
	struct super_block *sb =  bd->bd_super;
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	if (orig_mrqfn) {
		/* lock filesystem to prevent any further changes */
		freeze_super(sb);

		/* restore the original request function */
		bd->bd_disk->queue->make_request_fn = orig_mrqfn;

		/* unlock filesystem */
		thaw_super(sb);
	}
	orig_mrqfn = NULL;
}

static int __init filter_init(void)
{
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	orig_mrqfn = NULL;

	// Read block device from path
	bd = blkdev_get_by_path(PARTITION_NAME, FMODE_READ, NULL);

	if (IS_ERR(bd)) {
		printk(KERN_ERR "filter: failed to get block device\n");
		return 0;
	}

	set_mrqfn();

	return 0;
}

static void __exit filter_exit(void)
{
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	restore_mrqfn();

	if (!IS_ERR(bd)) {
		blkdev_put(bd, FMODE_READ);
	}
}

module_init(filter_init);
module_exit(filter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hackboxlive");
