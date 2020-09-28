#include <common.h>
#include "mvTypes.h"
#include "hwd_config.h"

#include "private_test_config.h"

#ifdef  LED_TEST


#if UBOOT_HWD_LET_TEST_DEBUG

/* SMI Address of Salsa 98DX24x */
#define DEV_ID 0x10

/* Indication Class Description*/
#define Class0	0
#define Class1	1
#define Class2	2
#define Class3	3
#define Class4	4
#define Class5	5

/* Set up a correspondence between Class # and LED state*/
#define Speed_1Gbps   Class0
#define Speed_100Mbps Class1
#define Full_Duplex   Class2
#define Link_up       Class3
#define Activity      Class4
#define Half_Duplex   Class5

/* LED Interface 0 Control Register for port 0-11 */
#define LED_Interface0_CTRL_REG	0x04004100
/* LED Interface 1 Control Register for port 12-23 */
#define LED_Interface1_CTRL_REG	0x05004100
/* LED Interface 0 Manipulation Register for Calss 0-1 port 0-11*/
#define LED_Interface0_Class_01	0x04004108 
/* LED Interface 1 Manipulation Register for Calss 0-1 port 12-23*/
#define LED_Interface1_Class_01	0x05004108
/* LED Interface 0 Manipulation Register for Calss 2-3 port 0-11*/
#define LED_Interface0_Class_23	0x04804108
/* LED Interface 1 Manipulation Register for Calss 2-3 port 12-23*/
#define LED_Interface1_Class_23	0x05804108
/* LED Interface 0 Manipulation Register for Calss 4 port 0-11*/
#define LED_Interface0_Class_4	0x0400410C
/* LED Interface 1 Manipulation Register for Calss 4 port 12-23*/
#define LED_Interface1_Class_4	0x0500410C
/* LED Interface 0 Manipulation Register for Calss 5 port 0-11*/
#define LED_Interface0_Class_5	0x0480410C
/* LED Interface 1 Manipulation Register for Calss 5 port 12-23*/
#define LED_Interface1_Class_5	0x0580410C

#endif


MV_STATUS led_test(void)
{
    MV_STATUS  LED_Test = MV_OK;

#if UBOOT_HWD_LET_TEST_DEBUG
    MV_U32  wrValue, data, regData=0;
    volatile MV_U32 timeout;

	diagIfReadReg(DEV_ID, LED_Interface0_CTRL_REG, &regData);
	for(timeout = 0; timeout < 1000; timeout++);
	diagIfReadReg(DEV_ID, LED_Interface1_CTRL_REG, &regData);

	data = 0x1FFF; /*Speed_1Gbps*/
	wrValue = (1<<12)|data;
	diagIfWriteReg(DEV_ID, LED_Interface0_Class_01, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Speed_100Mbps*/
	wrValue = (1<<28)|(data<<16);
	diagIfWriteReg(DEV_ID, LED_Interface0_Class_01, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Full_Duplex*/
	wrValue = (1<<12)|data;
	diagIfWriteReg(DEV_ID, LED_Interface0_Class_23, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Link_up*/
	wrValue = (1<<28)|(data<<16);
	diagIfWriteReg(DEV_ID, LED_Interface0_Class_23, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Activity*/
	wrValue = (1<<12)|data;
	diagIfWriteReg(DEV_ID, LED_Interface0_Class_4, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Half_Duplex*/
	wrValue = (1<<12)|data;
	diagIfWriteReg(DEV_ID, LED_Interface0_Class_5, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);
/*!-----------------------------------------------------------*/
	data = 0x1FFF; /*Speed_1Gbps*/
	wrValue = (1<<12)|data;
	diagIfWriteReg(DEV_ID, LED_Interface1_Class_01, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Speed_100Mbps*/
	wrValue = (1<<28)|(data<<16);
	diagIfWriteReg(DEV_ID, LED_Interface1_Class_01, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Full_Duplex*/
	wrValue = (1<<12)|data;
	diagIfWriteReg(DEV_ID, LED_Interface1_Class_23, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Link_up*/
	wrValue = (1<<28)|(data<<16);
	diagIfWriteReg(DEV_ID, LED_Interface1_Class_23, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Activity*/
	wrValue = (1<<12)|data;
	diagIfWriteReg(DEV_ID, LED_Interface1_Class_4, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);

	data = 0x1FFF; /*Half_Duplex*/
	wrValue = (1<<12)|data;
	diagIfWriteReg(DEV_ID, LED_Interface1_Class_5, wrValue);
	for(timeout = 0; timeout < 1000; timeout++);
        /*printf("PHY Address:%d Port:%d Status:0x%x\n",phyAddressesR,r+j,dataReaded&0xFFFF);*/
#endif

	printf("\nLED Test: Not implemented yet...\n");
    LED_Test = MV_FAIL;

    return LED_Test;
}

#endif  /* LED_TEST */
