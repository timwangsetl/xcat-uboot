/*  File Name:  hwd_strings.h  */

#ifndef __INChwd_stringsh
#define __INChwd_stringsh

/* ==== Memory tests ==== */

#include "hwd_common_config.h"

/* Model Revision: */
MV_U8   mvUT_compare_str_modelName[] = "MV88F6281 Rev 0";

MV_U8   mvUT_compare_str_base[22];
MV_U8   mvUT_compare_str_protect[23];

/* ==== MARVELL BOARD: xCat2_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 9,10,11,12) ==================== */
MV_U8   mvUT_compare_str_protect_01[] = "Erase fail!";
MV_U8   mvUT_compare_str_protect_02[] = "Erased 1 sectors";

/* ==== MARVELL BOARD: xCat2_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 33,34,35,36) ==================== */
MV_U8   mvUT_compare_str_protect_A_02[] = "Erased 4 sectors";

/* ==== MARVELL BOARD: xCat_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 25,26,27,28) ==================== */
MV_U8   mvUT_compare_str_protect_B_02[] = "Erased 4 sectors";

/* ==== MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 21,22,23,24) ==================== */
MV_U8   mvUT_compare_str_protect_C_02[] = "Erased 4 sectors";

/* ==== MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 29,30,31,32) ==================== */
MV_U8   mvUT_compare_str_protect_D_02[] = "Erased 4 sectors";

/* ==== MARVELL BOARD: xCat_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 1,2,3,4) ==================== */
MV_U8   mvUT_compare_str_protect_E_02[] = "Erased 4 sectors";

/* ==== MARVELL BOARD: xCat_DB_24FE_Rev_0 / _1 / _2 / _3 (BoardType = 5,6,7,8) ==================== */
MV_U8   mvUT_compare_str_protect_F_02[] = "Erased 4 sectors";

MV_U8   mvUT_compare_str_cmpl[] = "Total of 1 word were the same";
MV_U8   mvUT_compare_str_cmpw[] = "Total of 1 halfword were the same";
MV_U8   mvUT_compare_str_cmpb[] = "Total of 1 byte were the same";

MV_U8   mvUT_compare_str_crc32[] = "CRC32 for";
MV_U8   mvUT_compare_str_crc32_01[] = "50";
MV_U8   mvUT_compare_str_icrc32[46];

MV_U8   mvUT_compare_str_cmpm[] = "Value:";

MV_U8   mvUT_compare_str_cpumap_01[] = "CPU Memory mapping :";
MV_U8   mvUT_compare_str_cpumap_02[] = "Cache Mode - write-back";
MV_U8   mvUT_compare_str_cpumap_03[] = "page table:";
MV_U8   mvUT_compare_str_cpumap_04[] = "Section (0x00000000 - 0x5bafffff) => (0x00000000 - 0x000fffff) Non-Cachable/Bufferable	NO_ACCESS";
MV_U8   mvUT_compare_str_cpumap_05[] = "Section (0x5bb00000 - 0x5bbfffff) => (0x01000000 - 0x010fffff) Non-Cachable/Bufferable	NO_ACCESS";
MV_U8   mvUT_compare_str_cpumap_06[] = "Section (0x5bc00000 - 0xc96fffff) => (0x00000000 - 0x000fffff) Non-Cachable/Bufferable	NO_ACCESS";
MV_U8   mvUT_compare_str_cpumap_07[] = "Section (0xc9700000 - 0xc97fffff) => (0x40000000 - 0x400fffff) Non-Cachable/Bufferable	NO_ACCESS";
MV_U8   mvUT_compare_str_cpumap_08[] = "Section (0xc9800000 - 0xffffffff) => (0x00000000 - 0x000fffff) Non-Cachable/Bufferable	NO_ACCESS";

MV_U8   mvUT_compare_str_dramregs_01[] = "dramregs at 0xf8000040:";
MV_U8   mvUT_compare_str_dramregs_02[] = "-----------------------";
MV_U8   mvUT_compare_str_dramregs_03[] = "0xffd100e0 0x1b1b1b9b";
MV_U8   mvUT_compare_str_dramregs_04[] = "0xffd20134 0x66666666";
MV_U8   mvUT_compare_str_dramregs_05[] = "0xffd20138 0x66666666";
MV_U8   mvUT_compare_str_dramregs_06[] = "0xffd01400 0x430004e0";
MV_U8   mvUT_compare_str_dramregs_07[] = "0xffd01404 0x38543000";
MV_U8   mvUT_compare_str_dramregs_08[] = "0xffd01408 0x2302433e";
MV_U8   mvUT_compare_str_dramregs_09[] = "0xffd0140c 0x00000228";
MV_U8   mvUT_compare_str_dramregs_10[] = "0xffd01410 0x00000001";
MV_U8   mvUT_compare_str_dramregs_11[] = "0xffd01414 0x00000000";
MV_U8   mvUT_compare_str_dramregs_12[] = "0xffd01418 0x00000000";
MV_U8   mvUT_compare_str_dramregs_13[] = "0xffd0141c 0x00000852";
MV_U8   mvUT_compare_str_dramregs_14[] = "0xffd01420 0x00000042";
MV_U8   mvUT_compare_str_dramregs_15[] = "0xffd01424 0x0000f17f";
MV_U8   mvUT_compare_str_dramregs_16[] = "0xffd01428 0x00085520";
MV_U8   mvUT_compare_str_dramregs_17[] = "0xffd0147c 0x00008552";
MV_U8   mvUT_compare_str_dramregs_18[] = "0xffd01504 0x0ffffff1";
MV_U8   mvUT_compare_str_dramregs_19[] = "0xffd01508 0x00000000";
MV_U8   mvUT_compare_str_dramregs_20[] = "0xffd0150c 0x00000000";
MV_U8   mvUT_compare_str_dramregs_21[] = "0xffd01514 0x00000000";
MV_U8   mvUT_compare_str_dramregs_22[] = "0xffd0151c 0x00000000";
MV_U8   mvUT_compare_str_dramregs_23[] = "0xffd01494 0x00010000";
MV_U8   mvUT_compare_str_dramregs_24[] = "0xffd01498 0x00000000";
MV_U8   mvUT_compare_str_dramregs_25[] = "0xffd0149c 0x0000e801";
MV_U8   mvUT_compare_str_dramregs_26[] = "0xffd01480 0x00000001";

MV_U8   mvUT_compare_str_fi[42];
MV_U8   mvUT_compare_str_fi_01[] = "This command can be used only if enaMonExt is set!";
MV_U8   mvUT_compare_str_fi_02[] = "Value not found!!";

/*      ========================= MARVELL BOARD: xCat2_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 9,10,11,12) ===========================    */
MV_U8   mvUT_compare_str_flinfo_01[] = "Bank # 1:";
MV_U8   mvUT_compare_str_flinfo_02[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_flinfo_03[] = "Flash Model         : ST M25P128";
MV_U8   mvUT_compare_str_flinfo_04[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_flinfo_05[] = "Device Id           : 0x2018";
MV_U8   mvUT_compare_str_flinfo_06[] = "Sector Size         : 256K";
MV_U8   mvUT_compare_str_flinfo_07[] = "Number of sectors   : 64";
MV_U8   mvUT_compare_str_flinfo_08[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_flinfo_09[] = "Write Protection    :";

MV_U8   mvUT_compare_str_flinfo_10[] = "Linux/vxWorks partitions on flash";
MV_U8   mvUT_compare_str_flinfo_11[] = "Flash size           : 16MB";
MV_U8   mvUT_compare_str_flinfo_12[] = "u-boot               : offset=0xf8000000, size=0x00100000 (1024K)";
MV_U8   mvUT_compare_str_flinfo_13[] = "kernel/vxWorks-image : offset=0xf8100000, size=0x001c0000 (1792K)";
MV_U8   mvUT_compare_str_flinfo_14[] = "Linux rootfs         : offset=0xf82c0000, size=0x00d40000 (13568K)";
MV_U8   mvUT_compare_str_flinfo_15[] = "Single Image         : offset=0xf8100000, size=0x00f00000 (15360K)";

/*      ========================= MARVELL BOARD: xCat2_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 33,34,35,36) ===========================    */
MV_U8   mvUT_compare_str_flinfo_A_01[] = "Bank # 1:";
MV_U8   mvUT_compare_str_flinfo_A_02[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_flinfo_A_03[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_flinfo_A_04[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_flinfo_A_05[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_flinfo_A_06[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_flinfo_A_07[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_flinfo_A_08[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_flinfo_A_09[] = "Write Protection    :";

MV_U8   mvUT_compare_str_flinfo_A_10[] = "Linux/vxWorks partitions on flash";
MV_U8   mvUT_compare_str_flinfo_A_11[] = "Flash size           : 8MB";
MV_U8   mvUT_compare_str_flinfo_A_12[] = "u-boot               : offset=0xf8000000, size=0x00100000 (1024K)";
MV_U8   mvUT_compare_str_flinfo_A_13[] = "kernel/vxWorks-image : offset=0xf8100000, size=0x001c0000 (1792K)";
MV_U8   mvUT_compare_str_flinfo_A_14[] = "Linux rootfs         : offset=0xf82c0000, size=0x00540000 (5376K)";
MV_U8   mvUT_compare_str_flinfo_A_15[] = "Single Image         : offset=0xf8100000, size=0x00700000 (7168K)";

/*      ========================= MARVELL BOARD: xCat_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 25,26,27,28) ===========================    */
MV_U8   mvUT_compare_str_flinfo_B_01[] = "Bank # 1:";
MV_U8   mvUT_compare_str_flinfo_B_02[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_flinfo_B_03[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_flinfo_B_04[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_flinfo_B_05[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_flinfo_B_06[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_flinfo_B_07[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_flinfo_B_08[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_flinfo_B_09[] = "Write Protection    :";

MV_U8   mvUT_compare_str_flinfo_B_10[] = "Linux/vxWorks partitions on flash";
MV_U8   mvUT_compare_str_flinfo_B_11[] = "Flash size           : 8MB";
MV_U8   mvUT_compare_str_flinfo_B_12[] = "u-boot               : offset=0xf8000000, size=0x00100000 (1024K)";
MV_U8   mvUT_compare_str_flinfo_B_13[] = "kernel/vxWorks-image : offset=0xf8100000, size=0x001c0000 (1792K)";
MV_U8   mvUT_compare_str_flinfo_B_14[] = "Linux rootfs         : offset=0xf82c0000, size=0x00540000 (5376K)";
MV_U8   mvUT_compare_str_flinfo_B_15[] = "Single Image         : offset=0xf8100000, size=0x00700000 (7168K)";

/*      ========================= MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 21,22,23,24) ===========================    */
MV_U8   mvUT_compare_str_flinfo_C_01[] = "Bank # 1:";
MV_U8   mvUT_compare_str_flinfo_C_02[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_flinfo_C_03[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_flinfo_C_04[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_flinfo_C_05[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_flinfo_C_06[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_flinfo_C_07[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_flinfo_C_08[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_flinfo_C_09[] = "Write Protection    :";

MV_U8   mvUT_compare_str_flinfo_C_10[] = "Linux/vxWorks partitions on flash";
MV_U8   mvUT_compare_str_flinfo_C_11[] = "Flash size           : 8MB";
MV_U8   mvUT_compare_str_flinfo_C_12[] = "u-boot               : offset=0xf8000000, size=0x00100000 (1024K)";
MV_U8   mvUT_compare_str_flinfo_C_13[] = "kernel/vxWorks-image : offset=0xf8100000, size=0x001c0000 (1792K)";
MV_U8   mvUT_compare_str_flinfo_C_14[] = "Linux rootfs         : offset=0xf82c0000, size=0x00540000 (5376K)";
MV_U8   mvUT_compare_str_flinfo_C_15[] = "Single Image         : offset=0xf8100000, size=0x00700000 (7168K)";

/*      ========================= MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 29,30,31,32) ===========================    */
MV_U8   mvUT_compare_str_flinfo_D_01[] = "Bank # 1:";
MV_U8   mvUT_compare_str_flinfo_D_02[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_flinfo_D_03[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_flinfo_D_04[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_flinfo_D_05[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_flinfo_D_06[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_flinfo_D_07[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_flinfo_D_08[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_flinfo_D_09[] = "Write Protection    :";

MV_U8   mvUT_compare_str_flinfo_D_10[] = "Linux/vxWorks partitions on flash";
MV_U8   mvUT_compare_str_flinfo_D_11[] = "Flash size           : 8MB";
MV_U8   mvUT_compare_str_flinfo_D_12[] = "u-boot               : offset=0xf8000000, size=0x00100000 (1024K)";
MV_U8   mvUT_compare_str_flinfo_D_13[] = "kernel/vxWorks-image : offset=0xf8100000, size=0x001c0000 (1792K)";
MV_U8   mvUT_compare_str_flinfo_D_14[] = "Linux rootfs         : offset=0xf82c0000, size=0x00540000 (5376K)";
MV_U8   mvUT_compare_str_flinfo_D_15[] = "Single Image         : offset=0xf8100000, size=0x00700000 (7168K)";

/*      ========================= MARVELL BOARD: xCat_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 1,2,3,4) ===========================    */
MV_U8   mvUT_compare_str_flinfo_E_01[] = "Bank # 1:";
MV_U8   mvUT_compare_str_flinfo_E_02[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_flinfo_E_03[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_flinfo_E_04[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_flinfo_E_05[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_flinfo_E_06[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_flinfo_E_07[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_flinfo_E_08[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_flinfo_E_09[] = "Write Protection    :";

MV_U8   mvUT_compare_str_flinfo_E_10[] = "Linux/vxWorks partitions on flash";
MV_U8   mvUT_compare_str_flinfo_E_11[] = "Flash size           : 8MB";
MV_U8   mvUT_compare_str_flinfo_E_12[] = "u-boot               : offset=0xf8000000, size=0x00100000 (1024K)";
MV_U8   mvUT_compare_str_flinfo_E_13[] = "kernel/vxWorks-image : offset=0xf8100000, size=0x001c0000 (1792K)";
MV_U8   mvUT_compare_str_flinfo_E_14[] = "Linux rootfs         : offset=0xf82c0000, size=0x00540000 (5376K)";
MV_U8   mvUT_compare_str_flinfo_E_15[] = "Single Image         : offset=0xf8100000, size=0x00700000 (7168K)";

/*      ========================= MARVELL BOARD: xCat_DB_24FE_Rev_0 / _1 / _2 / _3 (BoardType = 5,6,7,8) ===========================    */
MV_U8   mvUT_compare_str_flinfo_F_01[] = "Bank # 1:";
MV_U8   mvUT_compare_str_flinfo_F_02[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_flinfo_F_03[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_flinfo_F_04[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_flinfo_F_05[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_flinfo_F_06[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_flinfo_F_07[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_flinfo_F_08[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_flinfo_F_09[] = "Write Protection    :";

MV_U8   mvUT_compare_str_flinfo_F_10[] = "Linux/vxWorks partitions on flash";
MV_U8   mvUT_compare_str_flinfo_F_11[] = "Flash size           : 8MB";
MV_U8   mvUT_compare_str_flinfo_F_12[] = "u-boot               : offset=0xf8000000, size=0x00100000 (1024K)";
MV_U8   mvUT_compare_str_flinfo_F_13[] = "kernel/vxWorks-image : offset=0xf8100000, size=0x001c0000 (1792K)";
MV_U8   mvUT_compare_str_flinfo_F_14[] = "Linux rootfs         : offset=0xf82c0000, size=0x00540000 (5376K)";
MV_U8   mvUT_compare_str_flinfo_F_15[] = "Single Image         : offset=0xf8100000, size=0x00700000 (7168K)";

MV_U8   mvUT_compare_str_mdl[18];
MV_U8   mvUT_compare_str_mdw[18];
MV_U8   mvUT_compare_str_mdb[18];

MV_U8   mvUT_compare_str_meEnable[] = "PCI 0 Master enabled.";
MV_U8   mvUT_compare_str_meExist[]  = "Master 1 doesn't exist";

/* ======== MARVELL BOARD: xCat2_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 9,10,11,12) ========== */
MV_U8   mvUT_compare_str_phyread[]  = "0xe40";

/* ======== MARVELL BOARD: xCat2_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 33,34,35,36) ========== */
MV_U8   mvUT_compare_str_phyread_A[]  = "0xe40";

/* ======== MARVELL BOARD: xCat_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 25,26,27,28) ========== */
MV_U8   mvUT_compare_str_phyread_B[]  = "0xe40";

/* ======== MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 21,22,23,24) ========== */
MV_U8   mvUT_compare_str_phyread_C[]  = "0xcc2";

/* ======== MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 29,30,31,32) ========== */
MV_U8   mvUT_compare_str_phyread_D[]  = "0xe40";

/* ======== MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 1,2,3,4) ========== */
MV_U8   mvUT_compare_str_phyread_E[]  = "0xcc2";

/* ======== MARVELL BOARD: xCat_DB_24FE_Rev_0 / _1 / _2 / _3 (BoardType = 5,6,7,8) ========== */
MV_U8   mvUT_compare_str_phyread_F[]  = "0xcc2";

/*      ========================= MARVELL BOARD: xCat2_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 9,10,11,12) ===========================    */
MV_U8   mvUT_compare_str_SFlash_01[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_SFlash_02[] = "Flash Model         : ST M25P128";
MV_U8   mvUT_compare_str_SFlash_03[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_SFlash_04[] = "Device Id           : 0x2018";
MV_U8   mvUT_compare_str_SFlash_05[] = "Sector Size         : 256K";
MV_U8   mvUT_compare_str_SFlash_06[] = "Number of sectors   : 64";
MV_U8   mvUT_compare_str_SFlash_07[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_SFlash_08[] = "Write Protection    : Off";
MV_U8   mvUT_compare_str_SFlash_09[] = "Write Protection    : All";

/*      ========================= MARVELL BOARD: xCat2_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 33,34,35,36) ===========================    */
MV_U8   mvUT_compare_str_SFlash_A_01[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_SFlash_A_02[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_SFlash_A_03[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_SFlash_A_04[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_SFlash_A_05[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_SFlash_A_06[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_SFlash_A_07[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_SFlash_A_08[] = "Write Protection    : Off";
MV_U8   mvUT_compare_str_SFlash_A_09[] = "Write Protection    : All";

/*      ========================= MARVELL BOARD: xCat_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 25,26,27,28) ===========================    */
MV_U8   mvUT_compare_str_SFlash_B_01[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_SFlash_B_02[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_SFlash_B_03[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_SFlash_B_04[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_SFlash_B_05[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_SFlash_B_06[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_SFlash_B_07[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_SFlash_B_08[] = "Write Protection    : Off";
MV_U8   mvUT_compare_str_SFlash_B_09[] = "Write Protection    : All";

/*      ========================= MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 21,22,23,24) ===========================    */
MV_U8   mvUT_compare_str_SFlash_C_01[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_SFlash_C_02[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_SFlash_C_03[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_SFlash_C_04[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_SFlash_C_05[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_SFlash_C_06[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_SFlash_C_07[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_SFlash_C_08[] = "Write Protection    : Off";
MV_U8   mvUT_compare_str_SFlash_C_09[] = "Write Protection    : All";

/*      ========================= MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 29,30,31,32) ===========================    */
MV_U8   mvUT_compare_str_SFlash_D_01[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_SFlash_D_02[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_SFlash_D_03[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_SFlash_D_04[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_SFlash_D_05[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_SFlash_D_06[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_SFlash_D_07[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_SFlash_D_08[] = "Write Protection    : Off";
MV_U8   mvUT_compare_str_SFlash_D_09[] = "Write Protection    : All";

/*      ========================= MARVELL BOARD: xCat_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 1,2,3,4) ===========================    */
MV_U8   mvUT_compare_str_SFlash_E_01[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_SFlash_E_02[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_SFlash_E_03[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_SFlash_E_04[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_SFlash_E_05[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_SFlash_E_06[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_SFlash_E_07[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_SFlash_E_08[] = "Write Protection    : Off";
MV_U8   mvUT_compare_str_SFlash_E_09[] = "Write Protection    : All";

/*      ========================= MARVELL BOARD: xCat_DB_24FE_Rev_0 / _1 / _2 / _3 (BoardType = 5,6,7,8) ===========================    */
MV_U8   mvUT_compare_str_SFlash_F_01[] = "Flash Base Address  : 0xf8000000";
MV_U8   mvUT_compare_str_SFlash_F_02[] = "Flash Model         : ST M25P64";
MV_U8   mvUT_compare_str_SFlash_F_03[] = "Manufacturer ID     : 0x20";
MV_U8   mvUT_compare_str_SFlash_F_04[] = "Device Id           : 0x2017";
MV_U8   mvUT_compare_str_SFlash_F_05[] = "Sector Size         : 64K";
MV_U8   mvUT_compare_str_SFlash_F_06[] = "Number of sectors   : 128";
MV_U8   mvUT_compare_str_SFlash_F_07[] = "Page Size           : 256";
MV_U8   mvUT_compare_str_SFlash_F_08[] = "Write Protection    : Off";
MV_U8   mvUT_compare_str_SFlash_F_09[] = "Write Protection    : All";

MV_U8   mvUT_compare_str_mp_01[] = "mapping pci 0 to address 0x";
MV_U8   mvUT_compare_str_mp_02[] = "PCI 0 Access base address : 90000000";
MV_U8   mvUT_compare_str_mp_03[] = "PCI 1 doesn't exist";

MV_U8   mvUT_compare_str_pci_01[] = "Scanning PCI devices on bus 0";
MV_U8   mvUT_compare_str_pci_02[] = "BusDevFun";
MV_U8   mvUT_compare_str_pci_03[] = "VendorId";
MV_U8   mvUT_compare_str_pci_04[] = "DeviceId";
MV_U8   mvUT_compare_str_pci_05[] = "Device";
MV_U8   mvUT_compare_str_pci_06[] = "Class";
MV_U8   mvUT_compare_str_pci_07[] = "Sub-Class";

MV_U8   mvUT_compare_str_imd_01[] = "50";
MV_U8   mvUT_compare_str_imd_02[] = "0010: ";
MV_U8   mvUT_compare_str_imd_03[] = "0020: ";
MV_U8   mvUT_compare_str_imd_04[] = "0030: ";
MV_U8   mvUT_compare_str_imd_05[] = "0040: ";

MV_U8   mvUT_compare_str_imw_01[] = "50";

MV_U8   mvUT_compare_str_iprobe[] = "Valid chip addresses: 18";

MV_U8 icrc32Array[] =\
{\
0x00,0x00,0x00,0xf0,\
0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0xf0,\
0x00,0x00,0x00,0x01,\
0x00,0x00,0x00,0xf0,\
0x00,0x00,0x00,0x02,\
0x00,0x00,0x00,0xf0,\
0x00,0x00,0x00,0x03,\
0x00,0x00,0x00,0xf0,\
0x00,0x00,0x00,0x04,\
0x00,0x00,0x00,0xf0,\
0x00,0x00,0x00,0x05,\
0x00,0x00,0x00,0xf0,\
0x00,0x00,0x00,0x06,\
0x00,0x00,0x00,0xf0,\
0x00,0x00,0x00,0x07\
};


#endif /*__INChwd_stringsh*/
