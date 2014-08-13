/*
 *  Copyright (C) 2009-2012 Silicon Graphics, Inc.  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

/*
 * UV Platform df_mmu interface
 *
 * This inteface provides df_mmu handling to df_mmu, superpages driver
 * and the GRU driver.
 */

#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/mmu_notifier.h>
#include <linux/rculist.h>
#include <linux/spinlock.h>
#include <linux/kallsyms.h>
#include <linux/string.h>
#include <linux/version.h>

#define DF_MMU_MODULE_NAME "df_mmu"

struct df_group {
	pid_t  mmu_registered_pid;		/* tg's tgid */
	struct mmu_notifier mmu_notifier;
	struct mm_struct *mm_for_mmu_notifier_unreg_only;
};

static int
df_mmu_file_open(struct inode *inode, struct file *file)
{ 
	return 0;
}
#if 0
static int
df_mmu_file_write(struct inode *inode, struct file *file)
{ 
	return 0;
}

static int
df_mmu_file_read(struct inode *inode, struct file *file)
{ 
	return 0;
}
#endif 
static int
df_mmu_file_release(struct inode *inode, struct file *file)
{ 
	return 0;
}

struct file_operations df_mmu_fops = {
//       .read = df_mmu_file_read,
//       .write = df_mmu_file_write,
       .open = df_mmu_file_open,
       .release = df_mmu_file_release
};
#if 0
static struct miscdevice df_mmu_dev_handle = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DF_MMU_MODULE_NAME,
	.fops = &df_mmu_fops
};
#endif 
static void
df_mmu_notifier_invalidate_range_start(struct mmu_notifier *mn,
			struct mm_struct *mm,
			unsigned long start, unsigned long end)
{
	printk("invalidate_range_start mm=%p, start=%#018lx, end=%#018lx\n", mm,start,end);
	return;

}



static void
df_mmu_notifier_invalidate_range_end(struct mmu_notifier *mn,
			struct mm_struct *mm,
			unsigned long start, unsigned long end)
{
	printk("invalidate_range_end mm=%p, start=%#018lx, end=%#018lx\n", mm,start,end);
	return;
}

static void
df_mmu_notifier_invalidate_page(struct mmu_notifier *mn,
			struct mm_struct *mm, unsigned long _start_address)
{
	printk("invalidate_page mm=%p, start_address=%#018lx\n", mm,_start_address);
}

static void
df_mmu_notifier_release(struct mmu_notifier *mn, struct mm_struct *mm)
{
	printk("release mm=%p \n", mm);
}
struct mmu_notifier_ops df_mmu_notifier_ops = {
	.release = df_mmu_notifier_release,
	.invalidate_page = df_mmu_notifier_invalidate_page,
	.invalidate_range_start = df_mmu_notifier_invalidate_range_start,
	.invalidate_range_end = df_mmu_notifier_invalidate_range_end,
};

/*
 * df_mmu_init
 *
 * Called at module insmod time to initialize the df_mmu interface.
 */
static int __init
df_mmu_init(void)
{
	static int major;

	printk("df-mmu init\n");
	major = register_chrdev(0, "df_mmu", &df_mmu_fops);

   	if (major < 0) {
     		printk ("Registering the character device failed with %d\n", major);
     		return major;
   	}
	return 0
;
}

/*
 * df_mmu_exit
 *
 * The module is about to exit.  Release all its resources.
 */
static void __exit
df_mmu_exit(void)
{
	printk("df-mmu exit\n");
}

module_init(df_mmu_init);
module_exit(df_mmu_exit);

MODULE_AUTHOR("Silicon Graphics, Inc.");
MODULE_DESCRIPTION("UV df_mmu interface");
MODULE_LICENSE("GPL");
MODULE_INFO(supported, "external");


