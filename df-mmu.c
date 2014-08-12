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
 * This inteface provides df_mmu handling to xpmem, superpages driver
 * and the GRU driver.
 */

#include <linux/module.h>
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

struct df_group {
	pid_t  mmu_registered_pid;		/* tg's tgid */
	struct mmu_notifier mmu_notifier;
	struct mm_struct *mm_for_mmu_notifier_unreg_only;
};

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

	printk("df-mmu init\n");
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


