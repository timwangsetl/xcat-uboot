/*  File Name:  hwd_strings.h  */

#ifndef __INChwd_stringsh
#define __INChwd_stringsh

#include "hwd_common_config.h"

/* ==== General tests ==== */

#define PRESTERA_PORT_TEST_REG  PRESTERA_PORT_STATUS_REG

/* ================ MARVELL BOARD: xCat2_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 9,10,11,12) =======================    */
MV_U8   mvUT_compare_str_dclk[] = "TCLK 167Mhz, SYSCLK 266Mhz (UART baudrate 115200)";

/* ================ MARVELL BOARD: xCat2_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 33,34,35,36) =======================    */
MV_U8   mvUT_compare_str_dclk_A[] = "TCLK 133Mhz, SYSCLK 266Mhz (UART baudrate 115200)";

/* ================ MARVELL BOARD: xCat_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 25,26,27,28) =======================    */
MV_U8   mvUT_compare_str_dclk_B[] = "TCLK 200Mhz, SYSCLK 320Mhz (UART baudrate 115200)";

/* ================ MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 21,22,23,24) =======================    */
MV_U8   mvUT_compare_str_dclk_C[] = "TCLK 200Mhz, SYSCLK 320Mhz (UART baudrate 115200)";

/* ================ MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 29,30,31,32) =======================    */
MV_U8   mvUT_compare_str_dclk_D[] = "TCLK 200Mhz, SYSCLK 320Mhz (UART baudrate 115200)";

/* ================ MARVELL BOARD: xCat_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 1,2,3,4) =======================    */
MV_U8   mvUT_compare_str_dclk_E[] = "TCLK 200Mhz, SYSCLK 320Mhz (UART baudrate 115200)";

/* ================ MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 5,6,7,8) =======================    */
MV_U8   mvUT_compare_str_dclk_F[] = "TCLK 200Mhz, SYSCLK 200Mhz (UART baudrate 115200)";

/* ================ MARVELL BOARD: xCat2_DB_24GE_Rev_0  QFP (BoardType = 5,6,7,8) =======================    */
MV_U8   mvUT_compare_str_dclk_G[] = "TCLK 167Mhz, SYSCLK 250Mhz (UART baudrate 115200)";

MV_U8   mvUT_compare_str_echo_01[] = "This is TEST string #1";
MV_U8   mvUT_compare_str_echo_02[] = "This is TEST string #2, - The big broun fox jump over the lazy dog.";

/* ==== MARVELL BOARD: xCat2_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 9,10,11,12) ==================== */
MV_U8   mvUT_compare_str_phyr_01[]   = "00000000";
MV_U8   mvUT_compare_str_phyr_02[]   = "00000141";
MV_U8   mvUT_compare_str_phyr_03[]   = "00000dc0";

/* ==== MARVELL BOARD: xCat2_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 33,34,35,36) ==================== */
MV_U8   mvUT_compare_str_phyr_A_01[] = "00004584";
MV_U8   mvUT_compare_str_phyr_A_02[] = "00000141";
MV_U8   mvUT_compare_str_phyr_A_03[] = "00000c89";

/* ==== MARVELL BOARD: xCat_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 25,26,27,28) ==================== */
MV_U8   mvUT_compare_str_phyr_B_01[] = "00004584";
MV_U8   mvUT_compare_str_phyr_B_02[] = "00000141";
MV_U8   mvUT_compare_str_phyr_B_03[] = "00000c89";

/* ==== MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 21,22,23,24) ==================== */
MV_U8   mvUT_compare_str_phyr_C_01[] = "00000000";
MV_U8   mvUT_compare_str_phyr_C_02[] = "00000141";
MV_U8   mvUT_compare_str_phyr_C_03[] = "00000e70";

/* ==== MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 29,30,31,32)  ==================== */
MV_U8   mvUT_compare_str_phyr_D_01[] = "00004584";
MV_U8   mvUT_compare_str_phyr_D_02[] = "00000141";
MV_U8   mvUT_compare_str_phyr_D_03[] = "00000c89";

/* ==== MARVELL BOARD: xCat_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 1,2,3,4) ==================== */
MV_U8   mvUT_compare_str_phyr_E_01[] = "00000000";
MV_U8   mvUT_compare_str_phyr_E_02[] = "00000141";
MV_U8   mvUT_compare_str_phyr_E_03[] = "00000e70";

/* ==== MARVELL BOARD: xCat_DB_24FE_Rev_0 / _1 / _2 / _3 (BoardType = 5,6,7,8) ==================== */
MV_U8   mvUT_compare_str_phyr_F_01[] = "00004584";
MV_U8   mvUT_compare_str_phyr_F_02[] = "00000141";
MV_U8   mvUT_compare_str_phyr_F_03[] = "00000c87";

/* ==== MARVELL BOARD: xCat2_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 9,10,11,12) ==================== */
MV_U8   mvUT_compare_str_phyw_01[]   = "0000a57e";

/* ==== MARVELL BOARD: xCat2_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 33,34,35,36) ==================== */
MV_U8   mvUT_compare_str_phyw_A_01[] = "0000a57e";

/* ==== MARVELL BOARD: xCat_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 25,26,27,28) ==================== */
MV_U8   mvUT_compare_str_phyw_B_01[] = "0000a57e";

/* ==== MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 21,22,23,24) ==================== */
MV_U8   mvUT_compare_str_phyw_C_01[] = "0000a57e";

/* ==== MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 29,30,31,32) ==================== */
MV_U8   mvUT_compare_str_phyw_D_01[] = "0000a57e";

/* ==== MARVELL BOARD: xCat_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 1,2,3,4) ==================== */
MV_U8   mvUT_compare_str_phyw_E_01[] = "0000a57e";

/* ==== MARVELL BOARD: xCat_DB_24FE_Rev_0 / _1 / _2 / _3 (BoardType = 5,6,7,8) ==================== */
MV_U8   mvUT_compare_str_phyw_F_01[] = "0000a57e";

MV_U8   mvUT_compare_str_ping_01[] = "serverip=";
MV_U8   mvUT_compare_str_ping_02[] = "Using";
MV_U8   mvUT_compare_str_ping_03[] = "device";
MV_U8   mvUT_compare_str_ping_04[] = "host";
MV_U8   mvUT_compare_str_ping_05[] = "is alive";
MV_U8   mvUT_compare_str_ping_06[] = "ping failed; ";
MV_U8   mvUT_compare_str_ping_07[] = "is not alive";

MV_U8   mvUT_compare_str_se_01[] = "numOfPex          = 1.";
MV_U8   mvUT_compare_str_se_02[] = "numOfPex          = 2.";
MV_U8   mvUT_compare_str_se_03[] = "PCI 1 doesn't exist";

MV_U8   mvUT_compare_str_sg_01[] = "PHY 0 :";
MV_U8   mvUT_compare_str_sg_02[] = "Auto negotiation: Enabled";
MV_U8   mvUT_compare_str_sg_03[] = "Speed:";
MV_U8   mvUT_compare_str_sg_04[] = "Duplex: ";
MV_U8   mvUT_compare_str_sg_05[] = "Link:";

MV_U8   mvUT_compare_str_sg_06[] = "1000 Mbps";
MV_U8   mvUT_compare_str_sg_07[] = "100 Mbps";
MV_U8   mvUT_compare_str_sg_08[] = "10 Mbps";

MV_U8   mvUT_compare_str_sg_09[] = "Half";
MV_U8   mvUT_compare_str_sg_10[] = "Full";
MV_U8   mvUT_compare_str_sg_11[] = "down";
MV_U8   mvUT_compare_str_sg_12[] = "up";

/* ================ xCat2_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 9,10,11,12) =======================    */
MV_U8   mvUT_compare_str_sp_01[] = "Bus: 0 Device: 0 Func: 0 Vendor ID: 11ab Device ID: e5ca";
MV_U8   mvUT_compare_str_sp_02[] = "Class: Network controller";
MV_U8   mvUT_compare_str_sp_03[] = "PCI_BAR0 (Mem-64bit) base: 0f1000000";
MV_U8   mvUT_compare_str_sp_04[] = "size: 1048576 bytes";
MV_U8   mvUT_compare_str_sp_05[] = "PCI_BAR1 (Mem-64bit) base: 000000000";
MV_U8   mvUT_compare_str_sp_06[] = "size: 134217728 bytes";

/* ================ MARVELL BOARD: xCat2_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 33,34,35,36) =======================    */
MV_U8   mvUT_compare_str_sp_A_01[] = "Bus: 0 Device: 0 Func: 0 Vendor ID: 11ab Device ID: e5a7";
MV_U8   mvUT_compare_str_sp_A_02[] = "Class: Network controller";
MV_U8   mvUT_compare_str_sp_A_03[] = "PCI_BAR0 (Mem-64bit) base: 0f1000000";
MV_U8   mvUT_compare_str_sp_A_04[] = "size: 1048576 bytes";
MV_U8   mvUT_compare_str_sp_A_05[] = "PCI_BAR1 (Mem-64bit) base: 000000000";
MV_U8   mvUT_compare_str_sp_A_06[] = "size: 268435456 bytes";

/* ================ MARVELL BOARD: xCat_DB_48FE_Rev_0 / _1 / _2 / _3 (BoardType = 25,26,27,28) =======================    */
MV_U8   mvUT_compare_str_sp_B_01[] = "Bus: 0 Device: 0 Func: 0 Vendor ID: 11ab Device ID: e5a7";
MV_U8   mvUT_compare_str_sp_B_02[] = "Class: Network controller";
MV_U8   mvUT_compare_str_sp_B_03[] = "PCI_BAR0 (Mem-64bit) base: 0f1000000";
MV_U8   mvUT_compare_str_sp_B_04[] = "size: 1048576 bytes";
MV_U8   mvUT_compare_str_sp_B_05[] = "PCI_BAR1 (Mem-64bit) base: 000000000";
MV_U8   mvUT_compare_str_sp_B_06[] = "size: 268435456 bytes";

/* ================ MARVELL BOARD: xCat_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 21,22,23,24) =======================    */
MV_U8   mvUT_compare_str_sp_C_01[] = "Bus: 0 Device: 0 Func: 0 Vendor ID: 11ab Device ID: 6281";
MV_U8   mvUT_compare_str_sp_C_02[] = "Class: Memory controller";
MV_U8   mvUT_compare_str_sp_C_03[] = "PCI_BAR0 (Mem-64bit) base: 0f1000000";
MV_U8   mvUT_compare_str_sp_C_04[] = "size: 1048576 bytes";
MV_U8   mvUT_compare_str_sp_C_05[] = "PCI_BAR1 (Mem-64bit) base: 000000000";
MV_U8   mvUT_compare_str_sp_C_06[] = "size: 134217728 bytes";

/* ================ MARVELL BOARD: xCat2_DB_48GE_Rev_0 / _1 / _2 / _3 (BoardType = 29,30,31,32) =======================    */
MV_U8   mvUT_compare_str_sp_D_01[] = "Bus: 0 Device: 0 Func: 0 Vendor ID: 11ab Device ID: 6281";
MV_U8   mvUT_compare_str_sp_D_02[] = "Class: Network controller";
MV_U8   mvUT_compare_str_sp_D_03[] = "PCI_BAR0 (Mem-64bit) base: 0f1000000";
MV_U8   mvUT_compare_str_sp_D_04[] = "size: 1048576 bytes";
MV_U8   mvUT_compare_str_sp_D_05[] = "PCI_BAR1 (Mem-64bit) base: 000000000";
MV_U8   mvUT_compare_str_sp_D_06[] = "size: 134217728 bytes";

/* ================ MARVELL BOARD: xCat_DB_24GE_Rev_0 / _1 / _2 / _3 (BoardType = 1,2,3,4) =======================    */
MV_U8   mvUT_compare_str_sp_E_01[] = "Bus: 0 Device: 0 Func: 0 Vendor ID: 11ab Device ID: 6281";
MV_U8   mvUT_compare_str_sp_E_02[] = "Class: Memory controller";
MV_U8   mvUT_compare_str_sp_E_03[] = "PCI_BAR0 (Mem-64bit) base: 0f1000000";
MV_U8   mvUT_compare_str_sp_E_04[] = "size: 1048576 bytes";
MV_U8   mvUT_compare_str_sp_E_05[] = "PCI_BAR1 (Mem-64bit) base: 000000000";
MV_U8   mvUT_compare_str_sp_E_06[] = "size: 134217728 bytes";

/* ================ MARVELL BOARD: xCat_DB_24FE_Rev_0 / _1 / _2 / _3 (BoardType = 5,6,7,8) =======================    */
MV_U8   mvUT_compare_str_sp_F_01[] = "Bus: 0 Device: 0 Func: 0 Vendor ID: 11ab Device ID: 6281";
MV_U8   mvUT_compare_str_sp_F_02[] = "Class: Memory controller";
MV_U8   mvUT_compare_str_sp_F_03[] = "PCI_BAR0 (Mem-64bit) base: 0f1000000";
MV_U8   mvUT_compare_str_sp_F_04[] = "size: 1048576 bytes";
MV_U8   mvUT_compare_str_sp_F_05[] = "PCI_BAR1 (Mem-64bit) base: 000000000";
MV_U8   mvUT_compare_str_sp_F_06[] = "size: 268435456 bytes";

MV_U8   mvUT_compare_str_smdl[21];
MV_U8   mvUT_compare_str_smdw[21];
MV_U8   mvUT_compare_str_smdb[21];

MV_U8   mvUT_compare_str_smwl[30];
MV_U8   mvUT_compare_str_smww[30];
MV_U8   mvUT_compare_str_smwb[30];

MV_U8   mvUT_compare_str_smi_01[] = "Checking for port 0";
MV_U8   mvUT_compare_str_smi_02[] = "Test Passed";

MV_U8   mvUT_compare_str_SwitchMibRead_01[] = "ethprime=";
MV_U8   mvUT_compare_str_SwitchMibRead_02[] = "ethact=";
MV_U8   mvUT_compare_str_SwitchMibRead_03[] = "serverip=";

MV_U8   mvUT_compare_str_SwitchMibRead_04[] = "Using ppsdma device";
MV_U8   mvUT_compare_str_SwitchMibRead_05[] = "host";
MV_U8   mvUT_compare_str_SwitchMibRead_06[] = "is alive";

MV_U8   mvUT_compare_str_SwitchMibRead_07[] = "ping failed; ";
MV_U8   mvUT_compare_str_SwitchMibRead_08[] = "is not alive";
MV_U8   mvUT_compare_str_SwitchMibRead_09[] = "Using ppsdma device";
MV_U8   mvUT_compare_str_SwitchMibRead_10[] = "host";
MV_U8   mvUT_compare_str_SwitchMibRead_11[] = "is alive";
MV_U8   mvUT_compare_str_SwitchMibRead_12[] = "ping failed; ";
MV_U8   mvUT_compare_str_SwitchMibRead_13[] = "is not alive";
MV_U8   mvUT_compare_str_SwitchMibRead_14[] = "Port MIB base address:";
MV_U8   mvUT_compare_str_SwitchMibRead_15[] = "GoodFramesReceived          = ";
MV_U8   mvUT_compare_str_SwitchMibRead_16[] = "BroadcastFramesReceived     = ";
MV_U8   mvUT_compare_str_SwitchMibRead_17[] = "MulticastFramesReceived     = ";
MV_U8   mvUT_compare_str_SwitchMibRead_18[] = "GoodOctetsReceived          = ";
MV_U8   mvUT_compare_str_SwitchMibRead_19[] = "GoodFramesSent              = ";
MV_U8   mvUT_compare_str_SwitchMibRead_20[] = "BroadcastFramesSent         = ";

MV_U8   mvUT_compare_str_version_01[] = "U-Boot 1.1.4";
MV_U8   mvUT_compare_str_version_02[] = "5.3.4_0026";
MV_U8   mvUT_compare_str_version_03[] = "Marvell version:";

/* MV_U8   mvUT_compare_str_vxburn_path[] = "\\galileo101\sw\Release\Prestera-Release\Pss\BullsEye\v2.2\v2.2.7"; */


#endif /*__INChwd_stringsh*/
