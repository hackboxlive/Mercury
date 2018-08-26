#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#include "device_file.h"

/*
	struct file_operations {
	struct module *owner;
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
	int (*readdir) (struct file *, void *, filldir_t);
	unsigned int (*poll) (struct file *, struct poll_table_struct *);
	int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, struct dentry *, int datasync);
	int (*fasync) (int, struct file *, int);
	int (*lock) (struct file *, int, struct file_lock *);
	ssize_t (*readv) (struct file *, const struct iovec *, unsigned long, loff_t *);
	ssize_t (*writev) (struct file *, const struct iovec *, unsigned long, loff_t *);
	};
*/

static const char    g_s_Hello_World_string[] = "Hello world from kernel mode!\n\0";
static const ssize_t g_s_Hello_World_size = sizeof(g_s_Hello_World_string);

static ssize_t device_file_read( struct file *file_ptr, char __user *user_buffer, size_t count, loff_t *position)
{
	printk( KERN_NOTICE "Simple-driver: Device file is read at offset = %i, read bytes count = %u", (int)*position, (unsigned int)count );
	/* If position is behind the end of a file we have nothing to read */
	if( *position >= g_s_Hello_World_size )
		return 0;
	/* If a user tries to read more than we have, read only as many bytes as we have */
	if( *position + count > g_s_Hello_World_size )
		count = g_s_Hello_World_size - *position;
	if( copy_to_user(user_buffer, g_s_Hello_World_string + *position, count) != 0 )
		return -EFAULT;    
	/* Move reading position */
	*position += count;
	return count;
}

static struct file_operations simple_driver_fops =
{
	.owner = THIS_MODULE,
	.read = device_file_read,
};

static int device_file_major_number = 0;
static const char device_name[] = "Simple-driver";

int register_device(void)
{
	int result = 0;
	printk( KERN_NOTICE "Mercury: register_device() is called" );
	result = register_chrdev( 0, device_name, &simple_driver_fops );
	if(result < 0)
	{
		printk( KERN_WARNING "Mercury: can\'t register character device with errorcode = %i", result );
		return result;
	}
	device_file_major_number = result;
	printk( KERN_NOTICE "Mercury: registered character device with major number = %i and minor numbers 0...255", device_file_major_number);
	return 0;
}

void unregister_device(void)
{
	printk( KERN_NOTICE "Mercury: unregister_device() is called" );
	if(device_file_major_number != 0)
	{
		unregister_chrdev( device_file_major_number, device_name );
	}
}


