	.arch armv5t
	.fpu softvfp
	.file	"boot_vxworks.c"
	.text
	.align	2
	.global	boot_vxworks_image
	.type	boot_vxworks_image, %function
boot_vxworks_image:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #8
	mov     r3, r0
	mov     r4, r1
	
	cmp	r4, #1 /* big endian ? */
	bne	.L2    /* no - skip the code */
/*
// switch to BE
*/
	mrc p15, 0, r0, c1, c0
	orr r0, r0, #0x80 
	mcr p15, 0, r0, c1, c0

	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 
	.word 0x0000a0e1 
	.word 0x0000a0e1 
	.word 0x0000a0e1 
	.word 0x0000a0e1 
	.word 0x0000a0e1 
	.word 0x0000a0e1 
	.word 0x0000a0e1 
	.word 0x0000a0e1 
	
	.word 0x33ff2fe1 /* blx	r3 - no return (BE) */
.L2:	
	blx	r3 /* no return */
	
	.size	boot_vxworks_image, .-boot_vxworks_image
	.ident	"GCC: (GNU) 3.4.4 (release) (CodeSourcery ARM 2005q3-2)"
