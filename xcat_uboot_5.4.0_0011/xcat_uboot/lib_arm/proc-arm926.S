/*
 *  linux/arch/arm/mm/proc-arm926.S: MMU functions for ARM926EJ-S
 *
 *  Copyright (C) 1999-2001 ARM Limited
 *  Copyright (C) 2000 Deep Blue Solutions Ltd.
 *  hacked for non-paged-MM by Hyok S. Choi, 2003.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * These are the low level assembler for performing cache and TLB
 * functions on the arm926.
 *
 *  CONFIG_CPU_ARM926_CPU_IDLE -> nohlt
 */

#define PAGE_SZ 4096
#define CONFIG_L2_CACHE_ENABLE
/*
 * vm_flags in vm_area_struct, see mm_types.h.
 */
#define VM_READ		0x00000001	/* currently active flags */
#define VM_WRITE	0x00000002
#define VM_EXEC		0x00000004
#define VM_SHARED	0x00000008

/* mprotect() hardcodes VM_MAYREAD >> 4 == VM_READ, and so for r/w/x bits. */
#define VM_MAYREAD	0x00000010	/* limits for mprotect() etc */
#define VM_MAYWRITE	0x00000020
#define VM_MAYEXEC	0x00000040
#define VM_MAYSHARE	0x00000080

#define VM_GROWSDOWN	0x00000100	/* general info on the segment */
#define VM_GROWSUP	0x00000200
#define VM_PFNMAP	0x00000400	/* Page-ranges managed without "struct page", just pure PFN */
#define VM_DENYWRITE	0x00000800	/* ETXTBSY on write attempts.. */

#define VM_EXECUTABLE	0x00001000
#define VM_LOCKED	0x00002000
#define VM_IO           0x00004000	/* Memory mapped I/O or similar */

					/* Used by sys_madvise() */
#define VM_SEQ_READ	0x00008000	/* App will access data sequentially */
#define VM_RAND_READ	0x00010000	/* App will not benefit from clustered reads */

#define VM_DONTCOPY	0x00020000      /* Do not copy this vma on fork */
#define VM_DONTEXPAND	0x00040000	/* Cannot expand with mremap() */
#define VM_RESERVED	0x00080000	/* Count as reserved_vm like IO */
#define VM_ACCOUNT	0x00100000	/* Is a VM accounted object */
#define VM_NORESERVE	0x00200000	/* should the VM suppress accounting */
#define VM_HUGETLB	0x00400000	/* Huge TLB Page VM */
#define VM_NONLINEAR	0x00800000	/* Is non-linear (remap_file_pages) */
#define VM_MAPPED_COPY	0x01000000	/* T if mapped copy of data (nommu mmap) */
#define VM_INSERTPAGE	0x02000000	/* The vma has had "vm_insert_page()" done on it */
#define VM_ALWAYSDUMP	0x04000000	/* Always include in core dumps */

#define VM_CAN_NONLINEAR 0x08000000	/* Has ->fault & does nonlinear pages */
#define VM_MIXEDMAP	0x10000000	/* Can contain "struct page" and pure PFN pages */
#define VM_SAO		0x20000000	/* Strong Access Ordering (powerpc) */

#ifndef VM_STACK_DEFAULT_FLAGS		/* arch can override this */
#define VM_STACK_DEFAULT_FLAGS VM_DATA_DEFAULT_FLAGS
#endif

#ifdef CONFIG_STACK_GROWSUP
#define VM_STACK_FLAGS	(VM_GROWSUP | VM_STACK_DEFAULT_FLAGS | VM_ACCOUNT)
#else
#define VM_STACK_FLAGS	(VM_GROWSDOWN | VM_STACK_DEFAULT_FLAGS | VM_ACCOUNT)
#endif

/*
 * PSR bits
 */
#define USR26_MODE      0x00000000
#define FIQ26_MODE      0x00000001
#define IRQ26_MODE      0x00000002
#define SVC26_MODE      0x00000003
#define USR_MODE        0x00000010
#define FIQ_MODE        0x00000011
#define IRQ_MODE        0x00000012
#define SVC_MODE        0x00000013
#define ABT_MODE        0x00000017
#define UND_MODE        0x0000001b
#define SYSTEM_MODE     0x0000001f
#define MODE32_BIT      0x00000010
#define MODE_MASK       0x0000001f
#define PSR_T_BIT       0x00000020
#define PSR_F_BIT       0x00000040
#define PSR_I_BIT       0x00000080
#define PSR_A_BIT       0x00000100
#define PSR_J_BIT       0x01000000
#define PSR_Q_BIT       0x08000000
#define PSR_V_BIT       0x10000000
#define PSR_C_BIT       0x20000000
#define PSR_Z_BIT       0x40000000
#define PSR_N_BIT       0x80000000
#define PCMASK          0

#include "linkage.h" /* from linux */
#include "pgtable-hwdef.h"
#include "hwcap.h"

/*
 * This is the maximum size of an area which will be invalidated
 * using the single invalidate entry instructions.  Anything larger
 * than this, and we go for the whole cache.
 *
 * This value should be chosen such that we choose the cheapest
 * alternative.
 */
#define CONFIG_MV_DCACHE_SIZE 0x4000 /* from linux */
#if (CONFIG_MV_DCACHE_SIZE == 0x4000)                         
#define CACHE_DLIMIT               8192                       
#else                                                         
#define CACHE_DLIMIT    16384              
#endif   

/*
 * the cache line size of the I and D cache
 */
#define CACHE_DLINESIZE	32

#ifdef CONFIG_ARCH_FEROCEON_ORION      
#include <asm/arch/orion_ver_macro.h>                         
#endif
#ifdef CONFIG_ARCH_FEROCEON_KW      
#include <asm/arch/kw_macro.h>
#include "../mach-feroceon-kw/kw_family/ctrlEnv/sys/mvCpuIfRegs.h"
#include "../mach-feroceon-kw/config/mvSysHwConfig.h"
#endif      
#ifdef CONFIG_ARCH_FEROCEON_MV78XX0    
#include <asm/arch/mv78xx0_macro.h>
#include "../mach-feroceon-mv78xx0/mv78xx0_family/ctrlEnv/sys/mvCpuIfRegs.h"
#include "../mach-feroceon-mv78xx0/config/mvSysHwConfig.h"
#define CPU_L2_CONFIG_REG 0x20104
#define CL2CR_L2_WT_MODE_MASK 	0x20000
#define CPU_CORE1       0x4000
#endif      

	.text
/*
 * cpu_arm926_proc_init()
 */
ENTRY(cpu_arm926_proc_init)
	mov	pc, lr


/*
 * cpu_arm926_reset(loc)
 *
 * Perform a soft reset of the system.  Put the CPU into the
 * same state as it would be if it had been reset, and branch
 * to what would be the reset vector.
 *
 * loc: location to jump to for soft reset
 */
	.align	5
ENTRY(cpu_arm926_reset)
	mov	ip, #0
	mcr	p15, 0, ip, c7, c7, 0		@ invalidate I,D caches
	mcr	p15, 0, ip, c7, c10, 4		@ drain WB
#ifdef CONFIG_MMU
	mcr	p15, 0, ip, c8, c7, 0		@ invalidate I & D TLBs
#endif
	mrc	p15, 0, ip, c1, c0, 0		@ ctrl register
	bic	ip, ip, #0x000f			@ ............wcam
	bic	ip, ip, #0x1100			@ ...i...s........
	mcr	p15, 0, ip, c1, c0, 0		@ ctrl register
	mov	pc, r0

/*
 *	flush_user_cache_all()
 *
 *	Clean and invalidate all cache entries in a particular
 *	address space.
 */
ENTRY(arm926_flush_user_cache_all)
	/* FALLTHROUGH */

/*
 *	flush_kern_cache_all()
 *
 *	Clean and invalidate the entire cache.
 */
ENTRY(arm926_flush_kern_cache_all)
	mov	r2, #VM_EXEC
	mov	ip, #0
__flush_whole_cache:
#ifdef CONFIG_CPU_DCACHE_WRITETHROUG
	mcr	p15, 0, ip, c7, c6, 0		@ invalidate D cache
#else
1:	mrc	p15, 0, r15, c7, c14, 3 	@ test,clean,invalidate
	bne	1b
#endif
	tst	r2, #VM_EXEC
	mcrne	p15, 0, ip, c7, c5, 0		@ invalidate I cache
	mcrne	p15, 0, ip, c7, c10, 4		@ drain WB
	mov	pc, lr

/*
 *	flush_user_cache_range(start, end, flags)
 *
 *	Clean and invalidate a range of cache entries in the
 *	specified address range.
 *
 *	- start	- start address (inclusive)
 *	- end	- end address (exclusive)
 *	- flags	- vm_flags describing address space
 */
ENTRY(arm926_flush_user_cache_range)
	mov	ip, #0
	sub	r3, r1, r0			@ calculate total size
	cmp	r3, #CACHE_DLIMIT
	bgt	__flush_whole_cache
1:	tst	r2, #VM_EXEC
#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D entry
	mcrne	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D entry
	mcrne	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
#else
#$!@#$
	mcr	p15, 0, r0, c7, c14, 1		@ clean and invalidate D entry
	mcrne	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
	mcr	p15, 0, r0, c7, c14, 1		@ clean and invalidate D entry
	mcrne	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
#endif
	cmp	r0, r1
	blo	1b
	tst	r2, #VM_EXEC
	mcrne	p15, 0, ip, c7, c10, 4		@ drain WB
	mov	pc, lr

/*
 *	coherent_kern_range(start, end)
 *
 *	Ensure coherency between the Icache and the Dcache in the
 *	region described by start, end.  If you have non-snooping
 *	Harvard caches, you need to implement this function.
 *
 *	- start	- virtual start address
 *	- end	- virtual end address
 */
ENTRY(arm926_coherent_kern_range)
	/* FALLTHROUGH */

/*
 *	coherent_user_range(start, end)
 *
 *	Ensure coherency between the Icache and the Dcache in the
 *	region described by start, end.  If you have non-snooping
 *	Harvard caches, you need to implement this function.
 *
 *	- start	- virtual start address
 *	- end	- virtual end address
 */
ENTRY(arm926_coherent_user_range)
	bic	r0, r0, #CACHE_DLINESIZE - 1
1:	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
	mcr	p15, 0, r0, c7, c5, 1		@ invalidate I entry
	add	r0, r0, #CACHE_DLINESIZE
	cmp	r0, r1
	blo	1b
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

/*
 *	flush_kern_dcache_page(void *page)
 *
 *	Ensure no D cache aliasing occurs, either with itself or
 *	the I cache
 *
 *	- addr	- page aligned address
 */
ENTRY(arm926_flush_kern_dcache_page)
	add	r1, r0, #PAGE_SZ
1:	mcr	p15, 0, r0, c7, c14, 1		@ clean+invalidate D entry
	add	r0, r0, #CACHE_DLINESIZE
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0		@ invalidate I cache
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

/*
 *	dma_inv_range(start, end)
 *
 *	Invalidate (discard) the specified virtual address range.
 *	May not write back any entries.  If 'start' or 'end'
 *	are not cache line aligned, those lines must be written
 *	back.
 *
 *	- start	- virtual start address
 *	- end	- virtual end address
 *
 * (same as v4wb)
 */
ENTRY(arm926_dma_inv_range)
#if !defined(CONFIG_CPU_DCACHE_WRITETHROUGH) || !defined(CONFIG_CPU_L2_DCACHE_WRITETHROUGH)
	tst	r0, #CACHE_DLINESIZE - 1
  #ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcrne	p15, 0, r0, c7, c10, 1		@ clean D entry
  #endif
 #ifdef CONFIG_L2_CACHE_ENABLE
    #ifndef CONFIG_CPU_L2_DCACHE_WRITETHROUGH
	mcrne	p15, 1, r0, c15, c9, 1		@ clean L2 entry
   #endif
 #endif

mcrne	p15, 0, r0, c7, c10, 4		@ drain WB

	tst	r1, #CACHE_DLINESIZE - 1
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcrne	p15, 0, r1, c7, c10, 1		@ clean D entry
#endif
#ifdef CONFIG_L2_CACHE_ENABLE
  #ifndef CONFIG_CPU_L2_DCACHE_WRITETHROUGH
	mcrne	p15, 1, r1, c15, c9, 1		@ clean L2 entry
  #endif
#endif
mcrne	p15, 0, r0, c7, c10, 4		@ drain WB
#endif

 	cmp	r1, r0
	subne	r1, r1, #1			@ Prevent cleaning of top address 
						@ cache line when top is cache line aligned
	mrs	r2, cpsr
	orr	r3, r2, #PSR_F_BIT | PSR_I_BIT
	msr	cpsr_c, r3			@ Disable interrupts
        mcr     p15, 5, r0, c15, c14, 0         @ L1 invalidation zone start addr
        mcr     p15, 5, r1, c15, c14, 1         @ L1 invalidation zone end addr and
 #ifdef CONFIG_L2_CACHE_ENABLE
        mcr     p15, 1, r0, c15, c11, 4         @ L2 invalidation zone start addr
        mcr     p15, 1, r1, c15, c11, 5         @ L2 invalidation zone end addr and 
                                                @ invalidate procedure trigger
 #endif
	msr	cpsr_c, r2			@ Restore interrupts
	mov	pc, lr

/*
 *	dma_clean_range(start, end)
 *
 *	Clean the specified virtual address range.
 *
 *	- start	- virtual start address
 *	- end	- virtual end address
 *
 * (same as v4wb)
 */
ENTRY(arm926_dma_clean_range)
#if !defined(CONFIG_CPU_DCACHE_WRITETHROUGH) || !defined(CONFIG_CPU_L2_DCACHE_WRITETHROUGH)
	cmp	r1, r0
	subne	r1, r1, #1			@ Prevent cleaning of top address 
						@ cache line when top is cache line aligned
	mrs	r2, cpsr
	orr	r3, r2, #PSR_F_BIT | PSR_I_BIT
	msr	cpsr_c, r3			@ Disable interrupts
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
        mcr     p15, 5, r0, c15, c13, 0         @ L1 DCache clean zone start addr
        mcr     p15, 5, r1, c15, c13, 1         @ L1 DCache clean zone end addr and
#endif 
#ifndef CONFIG_CPU_L2_DCACHE_WRITETHROUGH
        mcr     p15, 1, r0, c15, c9, 4          @ L2 Cache clean zone start addr
        mcr     p15, 1, r1, c15, c9, 5          @ L2 Cache clean zone end addr and 
                                                @ invalidate procedure trigger
#endif
	msr	cpsr_c, r2			@ Restore interrupts
#endif
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

/*
 *	dma_flush_range(start, end)
 *
 *	Clean and invalidate the specified virtual address range.
 *
 *	- start	- virtual start address
 *	- end	- virtual end address
 */
ENTRY(arm926_dma_flush_range)
	cmp	r1, r0
	subne	r1, r1, #1			@ Prevent cleaning of top address 
						@ cache line when top is cache line aligned
	mrs	r2, cpsr
	orr	r3, r2, #PSR_F_BIT | PSR_I_BIT
	msr	cpsr_c, r3			@ Disable interrupts
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
        mcr     p15, 5, r0, c15, c15, 0         @ L1 DCache clean+inv zone start addr
        mcr     p15, 5, r1, c15, c15, 1         @ L1 DCache clean+inv zone end addr and 
#else
        mcr     p15, 5, r0, c15, c14, 0         @ L1 invalidation zone start addr
        mcr     p15, 5, r1, c15, c14, 1         @ L1 invalidation zone end addr and
#endif
#ifdef CONFIG_L2_CACHE_ENABLE
  #ifndef CONFIG_CPU_L2_DCACHE_WRITETHROUGH
        mcr     p15, 1, r0, c15, c9, 4          @ L2 Cache clean zone start addr
        mcr     p15, 1, r1, c15, c9, 5          @ L2 Cache clean zone end addr and
  #endif 
        mcr     p15, 1, r0, c15, c11, 4         @ L2 invalidation zone start addr
        mcr     p15, 1, r1, c15, c11, 5         @ L2 invalidation zone end addr and 
                                                @ invalidate procedure trigger
#endif
	msr	cpsr_c, r2			@ Restore interrupts
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

ENTRY(arm926_cache_fns)
	.long	arm926_flush_kern_cache_all
	.long	arm926_flush_user_cache_all
	.long	arm926_flush_user_cache_range
	.long	arm926_coherent_kern_range
	.long	arm926_coherent_user_range
	.long	arm926_flush_kern_dcache_page
	.long	arm926_dma_inv_range
	.long	arm926_dma_clean_range
	.long	arm926_dma_flush_range

ENTRY(cpu_arm926_dcache_clean_area)
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
1:	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
#ifdef CONFIG_L2_CACHE_ENABLE
@#$@!#4
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mcr	p15, 1, r0, c15, c9, 1		@ clean L2 entry
#endif
	add	r0, r0, #CACHE_DLINESIZE
	subs	r1, r1, #CACHE_DLINESIZE
	bhi	1b
#endif
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mov	pc, lr

/* =============================== PageTable ============================== */

/*
 * cpu_arm926_switch_mm(pgd)
 *
 * Set the translation base pointer to be as described by pgd.
 *
 * pgd: new page tables
 */
	.align	5
ENTRY(cpu_arm926_switch_mm)
#ifdef CONFIG_MMU
	mov	ip, #0
#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcr	p15, 0, ip, c7, c6, 0		@ invalidate D cache
#else
@ && 'Clean & Invalidate whole DCache'
#ifdef CONFIG_ARCH_FEROCEON
        mv_flush_all r1, r2, r3, ip
        mov     ip, #0
#else
1:	mrc	p15, 0, r15, c7, c14, 3 	@ test,clean,invalidate
	bne	1b
#endif
#endif
	mcr	p15, 0, ip, c7, c5, 0		@ invalidate I cache
	mcr	p15, 0, ip, c7, c10, 4		@ drain WB
	mcr	p15, 0, r0, c2, c0, 0		@ load page table pointer
	mcr	p15, 0, ip, c8, c7, 0		@ invalidate I & D TLBs
#endif
	mov	pc, lr

/*
 * cpu_arm926_set_pte_ext(ptep, pte, ext)
 *
 * Set a PTE and flush it out
 */
	.align	5
ENTRY(cpu_arm926_set_pte_ext)
#ifdef CONFIG_MMU
	str	r1, [r0], #-2048		@ linux version

	eor	r1, r1, #L_PTE_PRESENT | L_PTE_YOUNG | L_PTE_WRITE | L_PTE_DIRTY

	bic	r2, r1, #PTE_SMALL_AP_MASK
	bic	r2, r2, #PTE_TYPE_MASK
	orr	r2, r2, #PTE_TYPE_SMALL

	tst	r1, #L_PTE_USER			@ User?
	orrne	r2, r2, #PTE_SMALL_AP_URO_SRW

	tst	r1, #L_PTE_WRITE | L_PTE_DIRTY	@ Write and Dirty?
	orreq	r2, r2, #PTE_SMALL_AP_UNO_SRW

	tst	r1, #L_PTE_PRESENT | L_PTE_YOUNG	@ Present and Young?
	movne	r2, #0

#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
	eor	r3, r2, #0x0a			@ C & small page?
	tst	r3, #0x0b
	biceq	r2, r2, #4
#endif
	str	r2, [r0]			@ hardware version
	mov	r0, r0
#ifndef CONFIG_CPU_DCACHE_WRITETHROUGH
	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
#ifdef CONFIG_L2_CACHE_ENABLE
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
	mcr	p15, 1, r0, c15, c9, 1		@ clean L2 entry
#endif
#endif
	mcr	p15, 0, r0, c7, c10, 4		@ drain WB
#endif
	mov	pc, lr

	.type	__arm926_setup, #function
__arm926_setup:
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7		@ invalidate I,D caches on v4
#ifdef CONFIG_L2_CACHE_ENABLE
//	mcr	p15, 1, r0, c15, c11, 0		@ invalidate L2 caches
#endif
	mcr	p15, 0, r0, c7, c10, 4		@ drain write buffer on v4
#ifdef CONFIG_MMU
	mcr	p15, 0, r0, c8, c7		@ invalidate I,D TLBs on v4
#endif


#ifdef CONFIG_CPU_DCACHE_WRITETHROUGH
	mov	r0, #4				@ disable write-back on caches explicitly
	mcr	p15, 7, r0, c15, c0, 0
#endif 

	adr	r5, arm926_crval
	ldmia	r5, {r5, r6}
#if defined(CONFIG_ARCH_FEROCEON_KW) || defined(CONFIG_ARCH_FEROCEON_MV78XX0)
	mrc	p15, 1, r0, c15, c1, 0		@ Marvell extra features register
#ifdef CONFIG_L2_CACHE_ENABLE
	ldr	r8, =INTER_REGS_BASE
	ldr	r7, =CPU_L2_CONFIG_REG	
#if defined(CONFIG_ARCH_FEROCEON_MV78XX0)	
	mrc	p15, 1, r0, c15, c1, 0	 @ get core Id
	tst 	r0, #CPU_CORE1
	addne	r7, r7, #0x4000		
#endif
	orr	r8, r8, r7
	ldr 	r7, [r8]
#ifndef CONFIG_CPU_L2_DCACHE_WRITETHROUGH	
	bic 	r7, r7, #CL2CR_L2_WT_MODE_MASK	@ set L2 wb mode
#else
	orr 	r7, r7, #CL2CR_L2_WT_MODE_MASK	@ set L2 wt mode
#endif
	orr	r7, r7, #CL2CR_L2_ECC_EN_MASK	@ enable L2 ECC
	str 	r7, [r8]

	orr	r0, r0, #0x01000000		@ disable L2 prefetching
 	orr	r0, r0, #0x00400000		@ enable L2 
#else
	bic	r0, r0, #0x00400000		@ disable L2 cache
#endif
	mcr	p15, 1, r0, c15, c1, 0		
#endif
	mrc	p15, 0, r0, c1, c0		@ get control register v4
	bic	r0, r0, r5
	orr	r0, r0, r6
#ifdef CONFIG_CPU_CACHE_ROUND_ROBIN
	orr	r0, r0, #0x4000			@ .1.. .... .... ....
#endif


	mov	pc, lr
	.size	__arm926_setup, . - __arm926_setup

	/*
	 *  R
	 * .RVI ZFRS BLDP WCAM
	 * .011 0001 ..11 0101
	 * 
	 */
	.type	arm926_crval, #object
arm926_crval:
