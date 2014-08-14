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
static int df_mmu_open(struct inode *, struct file *);
ssize_t df_mmu_ioctl(struct file *, unsigned int, unsigned long);
static int df_mmu_release(struct inode *, struct file *);
static int df_mmu_flush(struct file *, fl_owner_t);
struct mmu_notifier_ops df_mmu_notifier_ops;

struct df_mmu_group {
	pid_t  mmu_registered_pid;		/* df_g's df_gid */
	struct mmu_notifier mmu_notifier;
	struct mm_struct *mm_for_mmu_notifier_unreg_only;
};

struct kmem_cache *df_mmu_df_g_cache;

static inline int
df_mmu_cache_create_df_g(void)
{
	df_mmu_df_g_cache = kmem_cache_create("df_mmu_group",
		sizeof(struct df_mmu_group), 0, SLAB_HWCACHE_ALIGN, NULL);

	if (df_mmu_df_g_cache == NULL)
		return -ENOMEM;

	return 0;
}

static inline void
df_mmu_cache_destroy_df_g(void)
{
	kmem_cache_destroy(df_mmu_df_g_cache);
}

static inline struct df_mmu_group *
df_mmu_g_alloc(void)
{
	struct df_mmu_group *df_g;

	df_g = kmem_cache_alloc(df_mmu_df_g_cache, GFP_KERNEL);
	if (df_g != NULL)
		memset(df_g, 0, sizeof(struct df_mmu_group));

	return df_g;
}

static inline void
df_mmu_g_free(struct df_mmu_group *df_g)
{
	kmem_cache_free(df_mmu_df_g_cache, df_g);
}

static inline int
df_mmu_cache_create(void)
{
	int err;

	err = df_mmu_cache_create_df_g();
	if (err)
		goto err_1;

	df_mmu_cache_destroy_df_g();
err_1:
	return err;
}

struct file_operations df_mmu_fops = {
        .open = 	  df_mmu_open,
        .flush = 	  df_mmu_flush,
        .release = 	  df_mmu_release,
        .unlocked_ioctl = df_mmu_ioctl,
};

#if 1
static struct miscdevice df_mmu_dev_handle = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DF_MMU_MODULE_NAME,
	.fops = &df_mmu_fops
};
#endif 


static int
df_mmu_open(struct inode *inode, struct file *file)
{ 
	struct df_mmu_group *df_g;

	df_g = df_mmu_g_alloc();
	if (df_g == NULL)
		return -ENOMEM;
	
	df_g->mmu_notifier.ops = &df_mmu_notifier_ops;

	return 0;
}

ssize_t
df_mmu_ioctl(struct file *file, unsigned int flag, unsigned long lflag)
{ 
	return 0;
}
static int
df_mmu_flush(struct file *file, fl_owner_t owner)
{
	struct df_mmu_group *df_g = file->private_data;

	if (df_g == NULL)
		return 0;

	df_mmu_g_free(df_g);

	return 0;
}
static int
df_mmu_release(struct inode *inode, struct file *file)
{ 
	struct df_mmu_group *df_g = file->private_data;

	if (df_g == NULL)
		return 0;

	df_mmu_g_free(df_g);

	return 0;
}


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
	int ret;

	printk("df-mmu init\n");

	ret = df_mmu_cache_create();
	if (ret)
		return ret;


	/* create the df_mmu character device (/dev/df_mmu) */
	ret = misc_register(&df_mmu_dev_handle);
	if (ret != 0){
     		printk ("Registering the character device failed with %d\n", ret);
     		return ret;
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
	df_mmu_cache_destroy_df_g();
	misc_deregister(&df_mmu_dev_handle);
}

module_init(df_mmu_init);
module_exit(df_mmu_exit);

MODULE_AUTHOR("Silicon Graphics, Inc.");
MODULE_DESCRIPTION("UV df_mmu interface");
MODULE_LICENSE("GPL");
MODULE_INFO(supported, "external");


