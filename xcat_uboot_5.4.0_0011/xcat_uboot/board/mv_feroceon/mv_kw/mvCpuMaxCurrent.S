/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/
.globl mvCpuMaxCurrentTest

mvCpuMaxCurrentTest:

    b       User_Code
    nop
    mcr p15, 0, r0, c7, c0, 4       @ Go to sleep at swi

User_Code:


    mov r2, #0x4000        @initiate mmu ttb
    MCR p15, 0x0, R2, c2, c0

    mov r1, #0x3            @initiate domain access
    MCR p15, 0, r1, c3, c0

    MRC p15, 0x0, R4, c1, c0, 0x0    @turn cach & mmu on
    ORR r4, r4, #0x5
    ORR r4, r4, #0x1000
    MCR p15, 0x0, R4, c1, c0, 0x0


Start_max_current_test:
    mov r10, #0x100
Loop:
    adr r1, Mat1
    adr r2, Mat2
    mov r6, #0x3
L3:
    mov r5, #0x3
L2:
    mov r4, #0x3
    mov r3, #0x0
L1:
    ldrsb r7,[r1],#+0x1
    ldrsb r8,[r2],#+0x3
    mul r9,r7,r8
    add r3,r3,r9
    subs r4,r4,#0x1
    bne L1

#    strb r3,[r0],#0x1
    sub r1,r1,#0x3
    sub r2,r2,#0x8
    subs r5,r5,#0x1
    bne L2

    add r1,r1,#0x3
    sub r2,r2,#0x3
    subs r6,r6,#0x1
    bne L3

    subs r10,r10,#0x1
    bne Loop

    mov r3, #0x0
#    strb r3,[r0],#0x1
#    strb r3,[r0],#0x1

#    mov r15, #0x8000000
    b Start_max_current_test



Mat1:    .WORD 0xa2854c0f
    .WORD 0xc8278025
    .WORD 0x71c83009
Mat2:    .WORD 0xa0a0080d
    .WORD 0x752d5770
    .WORD 0xa55afd22


