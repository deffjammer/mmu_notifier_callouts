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
 * UV Platform xvma interface
 *
 * This inteface provides xvma handling to xpmem, superpages driver
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

static void
df_mmu_notifier_invalidate_range_start(struct mmu_notifier *mn,
			struct mm_struct *mm,
			unsigned long start, unsigned long end)
{
	return;

}



static void
df_mmu_notifier_invalidate_range_end(struct mmu_notifier *mn,
			struct mm_struct *mm,
			unsigned long start, unsigned long end)
{
	return;
}

static void
df_mmu_notifier_invalidate_page(struct mmu_notifier *mn,
			struct mm_struct *mm, unsigned long _start_address)
{
}

static void
df_mmu_notifier_release(struct mmu_notifier *mn, struct mm_struct *mm)
{
}
struct mmu_notifier_ops df_mmu_notifier_ops = {
	.release = df_mmu_notifier_release,
	.invalidate_page = df_mmu_notifier_invalidate_page,
	.invalidate_range_start = df_mmu_notifier_invalidate_range_start,
	.invalidate_range_end = df_mmu_notifier_invalidate_range_end,
};

/*
 * xvma_init
 *
 * Called at module insmod time to initialize the xvma interface.
 */
static int __init
xvma_init(void)
{
	return 0;
}

/*
 * xvma_exit
 *
 * The module is about to exit.  Release all its resources.
 */
static void __exit
xvma_exit(void)
{
}

module_init(xvma_init);
module_exit(xvma_exit);

MODULE_AUTHOR("Silicon Graphics, Inc.");
MODULE_DESCRIPTION("UV xvma interface");
MODULE_LICENSE("GPL");
MODULE_INFO(supported, "external");


