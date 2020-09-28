	.arch armv5t
	.fpu softvfp
	.file	"boot_linux.c"
	.text
	.align	2
	.global	boot_linux_kernel
	.type	boot_linux_kernel, %function
boot_linux_kernel:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #16
	str	r0, [fp, #-16]
	str	r1, [fp, #-20]
	str	r2, [fp, #-24]
	str	r3, [fp, #-28]
	mov     r4, r3
	ldr	r3, [fp, #-16]
	ldr	r1, [fp, #-20]
	ldr	r2, [fp, #-24]

	cmp	r4, #0 /* switch endianness ? */
	beq	.L2    /* yes - skip the code */
/*
// switch endianness
*/
#ifdef __BE
	mrc p15, 0, r0, c1, c0
	bic r0, r0, #0x80
	mcr p15, 0, r0, c1, c0
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */
	.word 0x0000a0e1 /* nop */

	.word 0x0000a0e3 /* mov r0, #0 (BE) */
	.word 0x33ff2fe1 /* blx	r3 -  no return (BE) */
#else
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

	.word 0x0000a0e3 /* mov r0, #0 (BE) */
	.word 0x33ff2fe1 /* blx	r3 -  no return (BE) */
#endif
.L2:

	mov	r0, #0
	blx	r3 /* no return */

	.size	boot_linux_kernel, .-boot_linux_kernel
	.ident	"GCC: (GNU) 3.4.4 (release) (CodeSourcery ARM 2005q3-2)"
