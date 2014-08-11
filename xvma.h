/*
 * SGI UV extended vma interface
 *
 *  Copyright (C) 2009-2012 Silicon Graphics, Inc. All rights reserved.
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
 */

#ifndef __UV_XVMA__
#define __UV_XVMA__

#include <linux/rbtree.h>
#include <linux/list.h>
#include <linux/rwsem.h>
#include <linux/completion.h>
#include <linux/spinlock.h>
#include <asm/atomic.h>

#define XVMA_DEBUGGING_ENABLED 0

#if XVMA_DEBUGGING_ENABLED
/* for testing/debugging statistics: */
extern atomic_t mmap_xmms_gotten;
extern atomic_t mmap_xmms_freed;
extern atomic_t mmap_xvmas_gotten;
extern atomic_t mmap_xvmas_freed;
#define inc_mmap_xmms_gotten		atomic_inc(&mmap_xmms_gotten)
#define inc_mmap_xmms_freed		atomic_inc(&mmap_xmms_freed)
#define inc_mmap_xvmas_gotten		atomic_inc(&mmap_xvmas_gotten)
#define inc_mmap_xvmas_freed		atomic_inc(&mmap_xvmas_freed)

#define read_mmap_xmms_gotten		atomic_read(&mmap_xmms_gotten)
#define read_mmap_xmms_freed		atomic_read(&mmap_xmms_freed)
#define read_mmap_xvmas_gotten		atomic_read(&mmap_xvmas_gotten)
#define read_mmap_xvmas_freed		atomic_read(&mmap_xvmas_freed)

#define set_mmap_xmms_gotten(x)		atomic_set(&mmap_xmms_gotten, x)
#define set_mmap_xmms_freed(x)		atomic_set(&mmap_xmms_freed, x)
#define set_mmap_xvmas_gotten(x)	atomic_set(&mmap_xvmas_gotten, x)
#define set_mmap_xvmas_freed(x)		atomic_set(&mmap_xvmas_freed, x)

#else	/* XVMA_DEBUGGING_ENABLED */

#define inc_mmap_xmms_gotten
#define inc_mmap_xmms_freed
#define inc_mmap_xvmas_gotten
#define inc_mmap_xvmas_freed

#define read_mmap_xmms_gotten		0
#define read_mmap_xmms_freed		0
#define read_mmap_xvmas_gotten		0
#define read_mmap_xvmas_freed		0

#define set_mmap_xmms_gotten(x)
#define set_mmap_xmms_freed(x)
#define set_mmap_xvmas_gotten(x)
#define set_mmap_xvmas_freed(x)

#endif	/* XVMA_DEBUGGING_ENABLED */


/* processor-addressable versus gru-only-addressable address request */
#define XVMA_TYPE_PROCESSOR		1
#define XVMA_TYPE_GRU			2
#define XVMA_TYPE_GRU_SHARED		3

/*
 * Return codes from vtop callout
 *
 * XVMA_VTOP_RETRY_UNLOCKED means our task's mmap_sem was released during
 * the fault and the caller needs to back off, acquire the mmap_sem,
 * validate and call vtop again.
 */
#define XVMA_VTOP_SUCCESS		 0
#define XVMA_VTOP_INVALID		-1
#define XVMA_VTOP_RETRY			-2
#define XVMA_VTOP_RETRY_UNLOCKED	-3
/*
 */

/* the boundary; at this point an address may only be addressed with the GRU */
#define GRU_SPACE			(1UL << 48)
#define GRU_SPACE_SHARED		(1UL << 63)

#define MBLADE_RESOURCE_FLAG 1
#define BOOTCPUSET_RESOURCE_FLAG 2
struct gru_reserve {
	int flags;
	unsigned short blade_id;
	unsigned long handle;
	atomic_t queued_count;
	struct list_head queued_superpages;
	rwlock_t inuse_lock;
	struct completion completion;
};

#define MBLADE_RESIDENT 1

/*
 * form of the 64-bit starting address that ACPI delivers to the
 * superpages driver
 */
struct xvma_range {
	unsigned int start_21:28;/* starting address >> 21 */
				 /* effectively a 49-bit address */
	unsigned int pageszindex:4; /* index to valid_size[] for below */
	unsigned int count:8;    /* number of superpages starting at start_21 */
	unsigned int node:8;     /* starts on this node */
	unsigned int numpages1:8;/* pages up to those on 2nd node */
	unsigned int numpages2:8;/* pages to end of 2nd node, if spans
				    back to 1st node */
};
/*
 * form of the 32 bits saved by the driver and provided to the configurator
 */
struct xvma_placement {
	unsigned int fill:3;	    /* to align in a 32-bit unsigned int */
	unsigned int node:9;
	unsigned int pageszindex:4; /* index to page valid_size[] for below */
	unsigned int numpages1:8;   /* pages up to those on 2nd node */
	unsigned int numpages2:8;   /* pages to end of 2nd node, if spans
				       back to 1st node */
};
/*
 * Superpage Driver's descriptor of a superpage.
 * (needed in both xvma and the driver)
 */
struct spg_desc {
	struct spg_desc *next;
	struct list_head list_link;
	void *addr;	/* address of the superpage */
	/* location of node/blade nearest the superpage - for GRU requests */
	unsigned short node;
	unsigned short blade_id;
	int flags;
	struct gru_reserve *gru_resource;
	long pages_in_set; /* nonzero only in the first of a set */
	long page_size;	/* initially all superpages will be the same size */
	int page_shift;	/* log 2 of page size */
	struct page *alloc_page;
	atomic_t ref_count;
	unsigned long uvaddr;
	unsigned long vaddr;
	unsigned int placement;
	struct rb_node rb_node;
};

enum xvma_type{XVMA_XPMEM, XVMA_SP}; /* cpw: not currently used */

struct xvma_struct;

struct xvma_operations_struct {
	/* convert a virtual address (within this xvma) to physical */
	int (*vtop)(struct mm_struct *mm, struct xvma_struct *xvma, unsigned long vaddr,
		int write, int atomic, unsigned long *paddr, int *writeable,
		int *pageshift);
};

/*
 * The xmm_struct is the head of a list of xvma_struct's, as mm_struct
 * is to vm_area_struct's.
 * 	Note: xmm_mm is NULL for shared high space
 */
struct xmm_struct {
	pid_t xmm_tgid;
	struct xmm_struct *xmm_shared_xmm;
	struct mm_struct *xmm_mm;
	atomic_t xmm_refcount;
	/* number of superpages currently mapped in the page table */
	atomic_t xmm_pagetable_mappings;
	struct rb_node xmm_rb_node;
	struct rb_root xvma_rb_root;
	unsigned long gru_space_hole_start; /* used to locate beginning of search region. */
	void *xmm_gru_private_data;
	void (*xmm_invalidate_high_range)(struct xmm_struct *xmm, unsigned long start, unsigned long end);
	void (*xmm_free_shared_gms)(struct xmm_struct *xmm);
	struct rw_semaphore xmm_high_space_mmap_sem;
	struct rw_semaphore xmm_xvma_list_sem;
	/* -- shared xmm -- */
	struct hlist_head xmm_shared_head;
	/* -- private xmm -- */
	struct rw_semaphore xmm_xvma_shared_list_sem;
	struct hlist_node xmm_shared_node;
};

/*
 * The extended vma's are logically parallel to the regular vma's.  They are
 * protected with mm->mmap_sem, the same as regular vma's.
 */
struct xvma_struct {
	struct xmm_struct *xvma_xmm;
	struct mm_struct *xvma_mm;
	struct vm_area_struct *xvma_vma;
	unsigned long xvma_start;
	unsigned long xvma_end;
	/* tree of xvma areas per task, sorted by address */
	struct rb_node xvma_rb;
	void *xvma_private;  /* to the page descriptor */
	struct xvma_operations_struct *xvma_ops;
	struct rb_root spg_rb_root;
	struct spg_desc *spg_cache;
	char xvma_type; /* xpmem vs. superpage */
};

/*
 * Test if xmm is a shared xmm.
 */
static inline int is_shared_xmm(struct xmm_struct *xmm)
{
	return !xmm->xmm_mm;
}

/* Interface functions: */
struct xvma_struct *
create_xvma(struct xmm_struct *, struct vm_area_struct *vma, unsigned long vaddr, long bytes,
	    long align, int type, struct xvma_operations_struct *ops);
int
delete_this_xvma(struct xvma_struct *);
int
delete_xvma(struct xmm_struct *, unsigned long vaddr, long bytes);
struct xvma_struct *
find_xvma(struct xmm_struct *xmm, unsigned long vaddr);
struct xvma_struct *
find_xvma_lock(struct xmm_struct *xmm, unsigned long vaddr);
struct xmm_struct *
find_xmm(struct task_struct *tsk);
int
clone_xmm(struct task_struct *parent, struct task_struct *child);
struct xmm_struct *
find_xmm(struct task_struct *tsk);
struct xmm_struct *
get_xmm(struct task_struct *tsk);
int
put_xmm(struct xmm_struct *xmm);
struct xmm_struct *
create_shared_xmm(struct task_struct *tsk, pid_t shared_tgid);
struct xmm_struct *
join_shared_xmm(struct task_struct *tsk, pid_t shared_tgid);
int
leave_shared_xmm(struct task_struct *tsk);
struct xmm_struct *
get_shared_xmm(struct task_struct *tsk);
struct xmm_struct *
find_shared_xmm(struct task_struct *tsk);
void
zap_xvma_ptes(struct xvma_struct *, unsigned long start, unsigned long size);

int gru_try_readlock_shared_xvmas(struct xmm_struct *xmm, struct xmm_struct *shared_xmm);
void gru_readlock_shared_xvmas(struct xmm_struct *xmm, struct xmm_struct *shared_xmm);
void gru_readunlock_shared_xvmas(struct xmm_struct *xmm, struct xmm_struct *shared_xmm);

/* for debugging: (also used by the superpages driver) */
void
show_xmm(struct xmm_struct *xmm);
/* statistical counters used by the superpages driver for debugging: */
extern atomic_t mmap_xmms_gotten;
extern atomic_t mmap_xmms_freed;
extern atomic_t mmap_xvmas_gotten;
extern atomic_t mmap_xvmas_freed;

static inline struct xvma_struct *
find_xvma_exact(struct xmm_struct *xmm, unsigned long vaddr)
{
	struct xvma_struct *xvma = find_xvma(xmm, vaddr);

	if (xvma == NULL || xvma->xvma_start > vaddr)
		return NULL;

	return xvma;
}

#endif
