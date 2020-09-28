	.arch armv5t
	.fpu softvfp
	.file	"boot_linux.c"
	.text
	.align	2
	.global	go_big_endian
	.type	go_big_endian, %function
go_big_endian:
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

    /*
     * Switch CPU endianness to Big Endian.
     */
	mrc p15, 0, r0, c1, c0
	orr r0, r0, #0x80
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
	.word 0x0000a0e1 /* nop */

	.word 0x0000a0e3 /* mov r0, #0 (BE) */
	.word 0x33ff2fe1 /* blx	r3 -  no return (BE) */

	.size	go_big_endian, .-go_big_endian
	.ident	"GCC: (GNU) 3.4.4 (release) (CodeSourcery ARM 2005q3-2)"


