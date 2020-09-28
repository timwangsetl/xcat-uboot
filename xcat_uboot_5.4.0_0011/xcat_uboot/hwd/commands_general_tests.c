 /* **************************************************************************
 *
 *$ GENERAL DESCRIPTION:
 *
 *$ PROCESS AND ALGORITHM: (local)
 *
 *$ PACKAGE GLOBAL SERVICES:
 *     (A list of package global services).
 *
 *$ PACKAGE LOCAL SERVICES:  (local)
 *     (A list of package local services).
 *
 *$ PACKAGE USAGE:
 *     (How to use the package services,
 *     routines calling order, restrictions, etc.)
 *
 *$ ASSUMPTIONS:
 *
 *$ SIDE EFFECTS:
 *
 *$ RELATED DOCUMENTS:     (local)
 *
 *$ REMARKS:               (local)
 *
 ****************************************************************************/

/* General definitions */

/* **************************************************************************
 *              EXTERNAL DECLARATIONS (IMPORT AND EXPORT)
 * **************************************************************************
 * */

/* **************************************************************************
 *              EXTERNAL DECLARATIONS (IMPORT)
 * **************************************************************************
 * */
 
/* #include <vxLib.h> */

#include "private_diag_if.h"
#include "hwd_config.h"
#include "hwd_general_strings.h"
#include "diag_inf.h"
#include "diag_services.h"
#include "private_test_config.h"
#include "mvUart.h"

#include "../board/mv_feroceon/mv_hal/prestera/mvPrestera.h"
#include "../board/mv_feroceon/mv_hal/prestera/mvPresteraPriv.h"

/* **************************************************************************
 *              PUBLIC DECLARATIONS (EXPORT)
 * **************************************************************************
 * */
    
/* **************************************************************************
 *              PUBLIC VARIABLE DEFINITIONS (EXPORT)
 * **************************************************************************
 * */


/* **************************************************************************
 *              LOCAL DECLARATIONS
 * **************************************************************************
 * */
#define COMMANDS_TEST
#ifdef  COMMANDS_TEST


/* commands defines */

/* commands MACROS */


/* **************************************************************************
 *              LOCAL VARIABLE DEFINITIONS
 * **************************************************************************
 * */
 
/* commands variables */
#define UT_TEST_PATTERN_10  0xa5a5a5a5

#ifndef PRESTERA_SDMA_RX_CURR_DESC_REG_LOCAL
#define PRESTERA_SDMA_RX_CURR_DESC_REG_LOCAL   0x03800074    /*0x260C*/
#endif

static volatile MV_BOOL isPpPortsInited = MV_FALSE;

typedef MV_STATUS (*MV_SWITCH_GEN_HOOK_HW_INIT_PORTS)(MV_VOID);

/* **************************************************************************
 *              LOCAL FUNCTION DEFINITIONS
 * **************************************************************************
 * */

/* **************************************************************************
 *              PUBLIC FUNCTION DEFINITIONS (EXPORT)
 * **************************************************************************
 * */
    extern mvBspDiagDramTestParamStruct dram_params;
	
	extern volatile MV_U8 s_mvLoggerArr[];
    extern volatile MV_U32 s_mvLoggerArrI;
	
	extern volatile MV_U8 *mvUTMemAlloc_PTR;
	
	extern volatile MV_U32 my_atoi(char *string);
	extern volatile MV_U32 my_atoi_4_char(char *string);
	extern MV_U32 mvSwitchFirstPortLinkUpGet(void);
	
	extern MV_VOID   mvPpWriteReg(MV_U8 devNum, MV_U32 regAddr, MV_U32 value);
	extern MV_U32    mvPpReadReg(MV_U8 devNum, MV_U32 regAddr);
	extern MV_SWITCH_GEN_HOOK_HW_INIT_PORTS mvBoardSwitchPortsInitFuncGet(void);
	extern MV_U16 diagIfGetMarvellBoardType(void);
	
/* **************************************************************************
* 
*  FUNCTION: mvGeneralDclkTest
*
*  GENERAL DESCRIPTION: Test the u-boot command dclk - Display the MV device CLKs.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralDclkTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 ret_val = 0xdeadface, compareSize = 0x0;  
	MV_U32 counter = 0; 	
	MV_U8  mem_command[25];    	
	MV_U8  BoardType = 0x0;	

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  dclk - Display the MV device CLKs command test started... \n"); 
	
	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();
		
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }
  
	sprintf(mem_command, "dclk");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 	
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		compareSize = sizeof(mvUT_compare_str_dclk);
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dclk, (compareSize-1)); 
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		compareSize = sizeof(mvUT_compare_str_dclk);
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dclk_E, (compareSize-1));
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		compareSize = sizeof(mvUT_compare_str_dclk);
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dclk_B, (compareSize-1));
	}
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		compareSize = sizeof(mvUT_compare_str_dclk);
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dclk_A, (compareSize-1));
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		compareSize = sizeof(mvUT_compare_str_dclk);
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dclk_C, (compareSize-1));	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		compareSize = sizeof(mvUT_compare_str_dclk);
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dclk_D, (compareSize-1));	
	}
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		compareSize = sizeof(mvUT_compare_str_dclk);
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dclk_F, (compareSize-1));	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		compareSize = sizeof(mvUT_compare_str_dclk);
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dclk_G, (compareSize-1));	
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	
						  																														
	if(ret_val == 0x0)
	{
		/* mvOsPrintf ("\n Run dclk command ended successfully");	*/
		/* mvOsPrintf (", %d -\n", s_mvLoggerArrI); */
	}	
	else if(ret_val == 0xdeadface)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Board type not supported 1. (BoardType = %d)\n", BoardType);
		return MV_FAIL;
	}	
	else
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= compareSize; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n Run dclk command FAILED. (BoardType = %d)\n", BoardType);
		return MV_FAIL;
	} 
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  dclk - Display the MV device CLKs command test ended successfully. (BoardType = %d)\n", BoardType);
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralEchoTest
*
*  GENERAL DESCRIPTION: Test the u-boot command echo - echo args to console.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralEchoTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 ret_val = 0x0, compareSize = 0x0;  
	MV_U32 counter = 0; 	
	MV_U8  mem_command[100];    

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  echo - echo args to console command test started... \n");
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	/*====== STEP 01 ======*/
	/* Set echo command #1 */  
	mem_command[0] = '\0';
	sprintf(mem_command, "echo %s", mvUT_compare_str_echo_01);  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	compareSize = sizeof(mvUT_compare_str_echo_01);
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_echo_01, (compareSize-1));   
			  								
	if(ret_val == 0)
	{
		/* mvOsPrintf ("\n Run echo command ended successfully");	*/
		/* mvOsPrintf (", %d -\n", s_mvLoggerArrI); */
	}
	else
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= compareSize; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n Run echo command FAILED.\n");
		return MV_FAIL;
	}

	/*====== STEP 02 ======*/
	/* Set echo command #2 */ 
	mem_command[0] = '\0';
	sprintf(mem_command, "echo %s", mvUT_compare_str_echo_02);  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	compareSize = sizeof(mvUT_compare_str_echo_02);
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_echo_02, (compareSize-1));   
			  								
	if(ret_val == 0)
	{
		/* mvOsPrintf ("\n Run echo command ended successfully");	*/
		/* mvOsPrintf (", %d -\n", s_mvLoggerArrI); */
	}
	else
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= compareSize; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n Run echo command FAILED.\n");
		return MV_FAIL;
	}	
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  echo - echo args to console command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralPingTest
*
*  GENERAL DESCRIPTION: Test the u-boot command ping - send ICMP ECHO_REQUEST to network host.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralPingTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 counter = 0x0; 	
	MV_U8  mem_command[100];    
	MV_U8  ReadValue[16];
	MV_U8 delay = 0x0;

	MV_U8  *result_01 = NULL, *result_02 = NULL, *result_03 = NULL, *result_04 = NULL;
	MV_U8  *result_05 = NULL, *result_06 = NULL;
	static volatile MV_U8  *result = NULL;

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  ping - send ICMP ECHO_REQUEST to network host command test started... \n");
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	/*====== STEP 01 ======*/
	/* Set ping command #1 */  
	mem_command[0] = '\0';
	sprintf(mem_command, "printenv serverip");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_ping_01); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		for(counter=0x0 ; counter <= 6; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n Run strstr() command FAILED, mem_command = %s.\n", mem_command);
		return MV_FAIL;
	}
			
	ReadValue[0] = '\0';
	strncpy (ReadValue, (const char *)(result+9), 15);
	ReadValue[15] = '\0';

	/*====== STEP 02 ======*/
	/* Set ping command */ 
	mem_command[0] = '\0';
	sprintf(mem_command, "ping %s", ReadValue);  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
		    
    delay = (USER_PING_DELAY / SW_1_SEC_CNS);

    while(delay)
    {
        udelay(SW_1_SEC_CNS);
        delay--; 
    }
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
					
	/* mvOsPrintf ("\n mem_command = %s, s_mvLoggerArr = %s.\n", mem_command, s_mvLoggerArr); */
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }		
 
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_ping_02);
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_ping_03);
	result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_ping_04);
	result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_ping_05);
		
	if((result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run ping command (STEP 01) FAILED.\n");
		return MV_FAIL;
	}	
	
	result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_ping_06);
	result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_ping_07);
		
	if((result_05 != NULL)||(result_06 != NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run ping command (STEP 02) FAILED.\n");
		return MV_FAIL;
	}		
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
 
	mvOsPrintf("  ping - send ICMP ECHO_REQUEST to network host command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralSeTest
*
*  GENERAL DESCRIPTION: Test the u-boot command se - PCI slave enable.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralSeTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U8  mem_command[100], mem_command_01[25];    
	MV_U8  *result_01 = NULL, *result_02 = NULL, *result_03 = NULL, *result_04 = NULL;
	
	MV_U8  maxNumberOfPex = 0x0, maxBusNumber = 0x6, maxDevNumber = 0x4;	
	MV_U8  pciNumber = 0x0;
	MV_U8  busNumber = 0x0;	
	MV_U8  devNumber = 0x0;	
	
	MV_U8  nPexCounter, nBusNumberCounter, nDevNumberCounter;

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  se - PCI slave enable command test started... \n");
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	/*====== STEP 01 ======*/
	/* Set command */  
	mem_command[0] = '\0';
	sprintf(mem_command, "socChipFeaturesPrint");  
	 
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 		 
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_se_01);		
	if(result_01 == NULL)
	{
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_se_02);
		if((result_02 == NULL))
		{
			/* Enable print to UART. */
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
					
			mvOsPrintf ("\n Run se command (STEP 01) FAILED.\n");
			return MV_FAIL;			
		}
		else
		{
			maxNumberOfPex = 0x2;
		}
	}
	else 
	{
		maxNumberOfPex = 0x1;
	}

	/*====== STEP 02 ======*/	
	for(nPexCounter = 0x0, pciNumber = 0x0 ; nPexCounter < maxNumberOfPex ; nPexCounter++)
	{
		for(nBusNumberCounter = 0x0, busNumber = 0x0 ; nBusNumberCounter < maxBusNumber ; nBusNumberCounter++)
		{
			for(nDevNumberCounter = 0x0, devNumber = 0x0 ; nDevNumberCounter < maxDevNumber ; nDevNumberCounter++)
			{
				/* Set se command */  
				mem_command[0] = '\0';	
				sprintf(mem_command, "se %d %d %d", pciNumber, busNumber, devNumber);  

				/*=========================================*/
				s_mvLoggerArr[0] = '\0';
				s_mvLoggerArrI = 0x0;	

				/* write to the memory address. */
				run_command(mem_command, 0);
	
				sprintf(mem_command_01, "PCI %d Bus %d Slave 0x%x enabled", pciNumber, busNumber, devNumber); 
		
				result_03 = strstr((MV_U8 *)s_mvLoggerArr, mem_command_01);
				result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_se_03);
				
				if(result_03 == NULL)
				{
					/* Enable print to UART. */
					if (mvUartPrintEnable(0) != MV_OK)
					{
						mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
						return MV_FAIL;
					}
					
					mvOsPrintf ("\n Run se command (STEP 02) FAILED.\n");
					return MV_FAIL;
				}
				else if(result_04 != NULL)
				{
					/* Enable print to UART. */
					if (mvUartPrintEnable(0) != MV_OK)
					{
						mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
						return MV_FAIL;
					}
					
					mvOsPrintf ("\n Run se command (STEP 02) FAILED - PCI 1 doesn't exist.\n");
					return MV_FAIL;
				}
				devNumber++;
					
				/* for debug... */
				/* mvOsPrintf ("\n Run se command (STEP 02): pciNumber = %d, busNumber = %d, devNumber = %d.\n", pciNumber, busNumber, devNumber); */
			}
			busNumber++;
		}			
		pciNumber++;
	}
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  se - PCI slave enable command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralSgTest
*
*  GENERAL DESCRIPTION: Test the u-boot command sg - scanning the PHY's status.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralSgTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U8  mem_command[100];    
	MV_U8  *result_01 = NULL, *result_02 = NULL, *result_03 = NULL, *result_04 = NULL;
	MV_U8  *result_05 = NULL, *result_06 = NULL, *result_07 = NULL, *result_08 = NULL;
	MV_U8  *result_09 = NULL, *result_10 = NULL, *result_11 = NULL, *result_12 = NULL;	

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  sg - scanning the PHY's status command test started... \n");
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	/*====== STEP 01 ======*/
	/* Set sg command */  
	mem_command[0] = '\0';
	sprintf(mem_command, "sg");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 		 
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_01);
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_02);
	result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_03);
	result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_04);
	result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_05);
		
	if((result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL)||(result_05 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run sg command (STEP 01) FAILED.\n");
		return MV_FAIL;
	}

	/*====== STEP 02 ======*/	
	result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_06);
	result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_07);	
	result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_08);	
		
	if((result_06 == NULL)&&(result_07 == NULL)&&(result_08 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run sg command (STEP 02) FAILED.\n");
		return MV_FAIL;
	}	
	
	/*====== STEP 03 ======*/	
	result_09 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_09);
	result_10 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_10);	
	
	if((result_09 == NULL)&&(result_10 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run sg command (STEP 03) FAILED.\n");
		return MV_FAIL;
	}
	
	/*====== STEP 04 ======*/	
	result_11 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_11);	
	result_12 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sg_12);	
	
	if((result_11 == NULL)&&(result_12 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run sg command (STEP 04) FAILED.\n");
		return MV_FAIL;
	}	
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  sg - scanning the PHY's status command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralSpTest
*
*  GENERAL DESCRIPTION: Test the u-boot command sp - scan PCI bus.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralSpTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U8  mem_command[100];    
	MV_U8  BoardType = 0x0;	
	MV_U8  *result_01 = NULL, *result_02 = NULL, *result_03 = NULL, *result_04 = NULL;
	MV_U8  *result_05 = NULL, *result_06 = NULL;
	
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  sp - send scan PCI bus command test started... \n");
	
	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();
		
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	/*====== STEP 01 ======*/
	/* Set sp command */  
	mem_command[0] = '\0';
	sprintf(mem_command, "sp");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 		 	
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_01);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_02);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_03);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_04);
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_05);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_06); 
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_E_01); 
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_E_02);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_E_03);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_E_04);
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_E_05);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_E_06);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_B_01);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_B_02);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_B_03);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_B_04);
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_B_05);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_B_06);
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_A_01);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_A_02);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_A_03);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_A_04);
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_A_05);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_A_06);
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_C_01);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_C_02);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_C_03);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_C_04);
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_C_05);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_C_06);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_D_01);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_D_02);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_D_03);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_D_04);
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_D_05);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_D_06);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_F_01);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_F_02);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_F_03);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_F_04);
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_F_05);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_sp_F_06);	
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	
							
	if((result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL) \
	    ||(result_05 == NULL)||(result_06 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run sp command FAILED. (BoardType = %d)\n", BoardType);
		return MV_FAIL;
	}	
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  sp - scan PCI bus command test ended successfully. (BoardType = %d)\n", BoardType);
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralVersionTest
*
*  GENERAL DESCRIPTION: Test the u-boot command version - print U-boot version.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralVersionTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U8  mem_command[100];    
	MV_U8  *result_01 = NULL, *result_02 = NULL, *result_03 = NULL;

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  version - print U-boot version command test started... \n");
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	/*====== STEP 01 ======*/
	/* Set version command */  
	mem_command[0] = '\0';
	sprintf(mem_command, "version");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 		 
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_version_01);
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_version_02);
	result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_version_03);
		
	if((result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run version command FAILED.\n");
		return MV_FAIL;
	}	
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  version - print U-boot version command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralTestsmiTest
*
*  GENERAL DESCRIPTION: Test the u-boot command test_smi - test the SMI interface.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralTestsmiTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U8  mem_command[100];    
	MV_U8  *result_01 = NULL, *result_02 = NULL;

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  test_smi - test the SMI interface command test started... \n");
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	/*====== STEP 01 ======*/
	/* Set test_smi command */  
	mem_command[0] = '\0';
	sprintf(mem_command, "test_smi");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 		 
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_smi_01);
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_smi_02);
		
	if((result_01 == NULL)||(result_02 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run test_smi command FAILED.\n");
		return MV_FAIL;
	}	
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  test_smi - test the SMI interface command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralPhyrTest
*
*  GENERAL DESCRIPTION: Test the u-boot command phyr - read the phy value connected to the switch.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralPhyrTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 counter = 0x0;
	MV_U32 ret_val = 0xdeadface;  
	MV_U8  mem_command[25];    	
	MV_U8  BoardType = 0x0;	
	
	MV_SWITCH_GEN_HOOK_HW_INIT_PORTS fP;
  
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  phyr - display switch read phy internal register value command test started... \n");

	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();
		
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
	
	/*====== STEP 00 ======*/	
	/* First Init SMI interface (Init PP --> Init SMI).  */
	if (isPpPortsInited == MV_FALSE)
    {
		fP = mvBoardSwitchPortsInitFuncGet();

		/* if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		} */
        mvOsPrintf("\nInitializing switch ports...");
		
		if (fP != NULL)
        {
            if (fP() != MV_OK)
            {
                mvOsPrintf("PP-EEPROM simulation failed.\n"); 
                return MV_FAIL; /* failure */
            }
            mvOsPrintf("Done.\n");
			isPpPortsInited = MV_TRUE;
        }
        else
        {
            mvOsPrintf("PP-EEPROM simulation failed (no function found).\n");
            return MV_FAIL; /* failure */
        }	
	}
	
	/*====== STEP 01 ======*/   
	/* Run test for phy read - (phyr [devid0-1][smd0-1] [phyId 0-16 ] [reg]) */
	/* First Read PHY page address register (register 22 (0x16)) - check that page is 0x0.  */
	sprintf(mem_command, "phyr 0 0 4 16");  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
	
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_01, 8); 
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_E_01, 8);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_B_01, 8);
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_A_01, 8);
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_C_01, 8);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_D_01, 8);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_F_01, 8);	
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	

	if(ret_val == 0xdeadface)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Board type not supported 2. (BoardType = %d)\n", BoardType);
		return MV_FAIL;
	}			
	else if(ret_val != 0x0)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}	
	
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 7; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run Compare for phyr command FAILED. (STEP 01).\n", __FUNCTION__);
		return MV_FAIL;
	}   
   
	/*====== STEP 02 ======*/   
	/* Read PHY identifier_1 register (register 2) - Organizationally Uniqe Identifier bits 3:18 */
	/* Marvell OUI is 0x005043 --> bits 3:18 of OUI shawn in reg. 2 bits 0:15 == 0x0141 */
	sprintf(mem_command, "phyr 0 0 4 2");  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
					 			
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_02, 8); 
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_E_02, 8);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_B_02, 8);
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_A_02, 8);
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_C_02, 8);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_D_02, 8);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_F_02, 8);	
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;				
											
	if(ret_val != 0x0)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}	
	
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 7; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run Compare for phyr command FAILED. (STEP 02). (BoardType = %d)\n", __FUNCTION__, BoardType);
		return MV_FAIL; 
	}

	/*====== STEP 03 ======*/	
	/* Read PHY identifier_2 register (register 3) - Organizationally Uniqe Identifier bits 19:24 */
	/* Marvell OUI LSB is 3 --> bits 19:24 of OUI LSB shawn in reg. 3 bits 10:15 == 0x000011 */
	sprintf(mem_command, "phyr 0 0 4 3");    
			 
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
		
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_03, 8); 
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_E_03, 8);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_B_03, 8);
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_A_03, 8);
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_C_03, 8);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_D_03, 8);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{ 
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_F_03, 8);	
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;
						   			
	if(ret_val != 0x0)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}	
	
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 7; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run Compare for phyr command FAILED. (STEP 03). (BoardType = %d)\n", __FUNCTION__, BoardType);
		return MV_FAIL;
	}	
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  phyr - display switch read phy internal register value command test ended successfully.");
	mvOsPrintf("\n       (BoardType = %d)\n", BoardType); 
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralPhywTest
*
*  GENERAL DESCRIPTION: Test the u-boot command phyw - write the phy value connected to the switch.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralPhywTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 counter = 0x0;
	MV_U32 ret_val = 0xdeadface;  
	MV_U8  mem_command[25];    
	MV_U8  tst_value[16];  
	MV_U8  restore_value[8];
	MV_32  restore_int = 0x0;	
	MV_U8  BoardType = 0x0;	

	MV_SWITCH_GEN_HOOK_HW_INIT_PORTS fP;  	
  
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  phyw - write to switch phy internal register command test started... \n");
	
	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();
		
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
	
	/*====== STEP 00 ======*/	
	/* First Init SMI interface (Init PP --> Init SMI).  */
	if (isPpPortsInited == MV_FALSE)
    {
		fP = mvBoardSwitchPortsInitFuncGet();

		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
        mvOsPrintf("\nInitializing switch ports...");
		
		if (fP != NULL)
        {
            if (fP() != MV_OK)
            {
                mvOsPrintf("PP-EEPROM simulation failed.\n");
                return MV_FAIL; /* failure */
            }
            mvOsPrintf("Done.\n");
			isPpPortsInited = MV_TRUE;
        }
        else
        {
            mvOsPrintf("PP-EEPROM simulation failed (no function found).\n");
            return MV_FAIL; /* failure */
        }	
	}	
   
	/*====== STEP 01 ======*/   
	/* Run test for phy write - (phyw  [devid0-1][smd0-1] [phyId 0-16 ] [reg] [value]) */
	/* First Read PHY page address register (register 22 (0x16)) - check that page is 0x0.  */
	sprintf(mem_command, "phyr 0 0 4 16");  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
				
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_01, 8); 
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_E_01, 8);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_B_01, 8);
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_A_01, 8);
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_C_01, 8);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_D_01, 8);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyr_F_01, 8);	
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	
		
	if(ret_val == 0xdeadface)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Board type not supported 3. (BoardType = %d)\n", BoardType);
		return MV_FAIL;
	}		
	else if(ret_val != 0x0)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
	
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 7; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run Compare for phyw command FAILED. (STEP 01), (BoardType = %d)\n", __FUNCTION__, BoardType);
		return MV_FAIL;
	}   	
   
	/*====== STEP 02 ======*/   
	/* Read Copper Specific Control register-1 (register 16 (0x10)) - Address data bits 0:15 */
	sprintf(mem_command, "phyr 0 0 4 10");  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;		
	tst_value[0] = '\0';
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
			
	/*====== STEP 03 ======*/			
	/* Save the value from Copper Specific Control register-1 in 'tst_value[]' */
	strncat((char *)tst_value, (MV_U8 *)s_mvLoggerArr, 8);   
			
	/* Write to Copper Specific Control register-1 (register 16 (0x10)) - 0xa57e */
	sprintf(mem_command, "phyw 0 0 4 10 a57e");  
				
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
	
	/*====== STEP 04 ======*/	
	/***** VERIFY *****/
	/* Read Copper Specific Control register-1 (register 16 (0x10)) - Address data bits 0:15 */
	sprintf(mem_command, "phyr 0 0 4 10");  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;		
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);	
				
	/*====== STEP 05 ======*/				
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		/* Run test for phy read - (phyw) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyw_01, 8); 
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyw_E_01, 8);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		/* Run test for phy read - (phyw) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyw_B_01, 8);
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		/* Run test for phy read - (phyw) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyw_A_01, 8);
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyw_C_01, 8);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyw_D_01, 8);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		/* Run test for phy read - (phyr) */			
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyw_F_01, 8);	
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	
								
	if(ret_val != 0x0)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
	
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 7; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run Compare for phyr command FAILED.(STEP 05), (BoardType = %d)\n", __FUNCTION__, BoardType);
		return MV_FAIL;
	}	
	
	/*====== STEP 06 ======*/	
	/***** RESTORE *****/
	/* Write to Copper Specific Control register-1 (register 16 (0x10)) - original saved value from 'tst_value[]' */
	restore_value[0] = '\0';
	strncpy((char *)restore_value, (char *)(tst_value+4), 4); 
	restore_int = my_atoi_4_char(restore_value);
	sprintf(mem_command, "phyw 0 0 4 10 %x", restore_int);   	
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);	
	
	/*====== STEP 07 ======*/	
	/***** VERIFY restore *****/
	/* Read Copper Specific Control register-1 (register 16 (0x10)) - Address data bits 0:15 */
	sprintf(mem_command, "phyr 0 0 4 10");  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;		
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);	
				
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, tst_value, 8);   
			
	if(ret_val != 0x0)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
	
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 7; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run Compare for phyw command FAILED. (STEP 07), tst_value[] = %s, restore_value = %s\n", __FUNCTION__, tst_value, restore_value);
		return MV_FAIL;
	}	
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  phyw - write to switch phy internal register command test ended successfully. (BoardType = %d)\n", BoardType);
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralSmdTest
*
*  GENERAL DESCRIPTION: Test the u-boot command smd - display switch internal register.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralSmdTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR, counter = 0x0;
	MV_U32 endOffset   = (PRESTERA_PORT_TEST_REG + (0x20 * 4)), ret_val = 0x0, compareSize = 0x0;  
	MV_U8  mem_command[25];    
	MV_U8  VarCounter = 0x0;
	MV_U32 dev = 0x0;
	MV_U32 testVal = 0x0;
	MV_U32 adrsIndex = 0x0;
  
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  smd - display switch internal register command test started... \n");
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	/*====== STEP 01 ======*/		 
	for(VarCounter=0x0 ; VarCounter <= 2 ; VarCounter++)
	{
		/* Clear the test buffer. */
		memset ((MV_U32 *)startOffset, 0x0, 0x80);
					
		switch (VarCounter)
		{
			case 0:
					/* Set Compare buffer with data. */
					for(counter=0x0 ; counter < 0x80 ; counter+=4)
					{
						testVal = mvPpReadReg (dev, (PRESTERA_PORT_TEST_REG + counter));	
						
						/* Write the value from prestera register to the test buffer. */
						*(volatile MV_U32 *)(startOffset+counter) = testVal;
					}   
					break;
			case 1:
					/* Set Compare buffer with data. */
					for(counter=0x0 ; counter < 0x80 ; counter+=4)
					{
						testVal = mvPpReadReg (dev, (PRESTERA_PORT_TEST_REG + counter));
						
						/* Write the value from prestera register to the test buffer. */
						*(volatile MV_U32 *)(startOffset+counter) = (testVal & 0x0000ffff);
					}					
					break;
			case 2:
					/* Set Compare buffer with data. */
					for(counter=0x0 ; counter < 0x80 ; counter+=4)
					{
						testVal = mvPpReadReg (dev, (PRESTERA_PORT_TEST_REG + counter));
						
						/* Write the value from prestera register to the test buffer. */
						*(volatile MV_U32 *)(startOffset+counter) = (testVal & 0x000000ff);
					}
					break;
			default:
					if (mvUartPrintEnable(0) != MV_OK)
					{
						mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
						return MV_FAIL;
					}
					mvOsPrintf(" error in loop value. \n");
					return MV_FAIL;						
		}		
  
		for (adrsIndex = PRESTERA_PORT_TEST_REG, counter=0x0 ; adrsIndex < endOffset ; adrsIndex += 4, counter += 4)
		{    
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (smd.l) */
						sprintf(mem_command, "smd.l %x 1", adrsIndex);  
						break;
				case 1:
						/* Run test for word write - (smd.w) */
						sprintf(mem_command, "smd.w %x 1", adrsIndex);  
						break;
				case 2:
						/* Run test for byte write - (smd.b) */
						sprintf(mem_command, "smd.b %x 1", adrsIndex);  
						break;
				default:
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf(" error in loop value. \n");
						return MV_FAIL;						
			}
			
			/*=========================================*/
			s_mvLoggerArr[0] = '\0';
			s_mvLoggerArrI = 0x0;			
	
			/* run the memory dispaly command. */
			run_command(mem_command, 0);
			
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (smd.l) */
						compareSize = sprintf(mvUT_compare_str_smdl, "0x%08X: %08x", adrsIndex, (*(volatile MV_U32 *)(startOffset+counter)));
						/* mvOsPrintf ("\n mvUT_compare_str_smdl = %s\n", mvUT_compare_str_smdl); */
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_smdl, (compareSize-2));   
						break;
				case 1:
						/* Run test for word write - (smd.w) */
						compareSize = sprintf(mvUT_compare_str_smdw, "0x%08X: %04x", adrsIndex, ((*(volatile MV_U32 *)(startOffset+counter)) & 0xffff0000));
						/* mvOsPrintf ("\n mvUT_compare_str_smdw = %s\n", mvUT_compare_str_smdw); */
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_smdw, (compareSize-2));  
						break;
				case 2:
						/* Run test for byte write - (smd.b) */
						compareSize = sprintf(mvUT_compare_str_smdb, "0x%08X: %02x", adrsIndex, ((*(volatile MV_U32 *)(startOffset+counter)) & 0x00ff0000));											
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_smdb, (compareSize-2)); 
						break;
				default:
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf(" error in loop value. \n");
						return MV_FAIL;						
			}			
			
			if(ret_val != 0x0)
			{
				/* Enable print to UART. */
				if (mvUartPrintEnable(0) != MV_OK)
				{
					mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
					return MV_FAIL;
				}
		
				mvOsPrintf ("\n %d - ", ret_val);
				for(counter=0x0 ; counter <= compareSize-1; counter++)
					mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
				mvOsPrintf ("\n %s - Run Compare for smd command FAILED, compareSize = %d, startOffset = 0x%08x.\n", __FUNCTION__, compareSize, startOffset);
				return MV_FAIL;
			}			
		} // for()
	} // for()	
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  smd - display switch internal register command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralSmwTest
*
*  GENERAL DESCRIPTION: Test the u-boot command smw - modify switch internal register.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralSmwTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR, counter = 0x0, counter_b = 0x0;
	MV_U32 endOffset   = (PRESTERA_SDMA_RX_CURR_DESC_REG_LOCAL + (0x20 * 1)), ret_val = 0x0, compareSize = 0x0;  
	MV_U8  mem_command[25];    
	MV_U8  VarCounter = 0x0;
	MV_U32 dev = 0x0;
	MV_U32 testVal = 0x0;
	MV_U32 adrsIndex = 0x0;

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  smw - modify switch internal register command test started...");
	mvOsPrintf("\n        start address is : 0x%x\n", startOffset);	
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
  
	for(VarCounter=0x0 ; VarCounter <= 2 ; VarCounter++)
	{		
		/*====== STEP 01 ======*/
		/* Clear the test buffers. */
		memset ((MV_U32 *)startOffset, 0x0, 0x20);
		memset ((MV_U32 *)(startOffset+0x100), 0x0, 0x20);		
	
		switch (VarCounter)
		{
			case 0:
					/* Set backup buffer with data from prestera switch. */
					for(counter=0x0 ; counter < 0x20 ; counter+=4)
					{
						testVal = mvPpReadReg (dev, (PRESTERA_SDMA_RX_CURR_DESC_REG_LOCAL + counter));	
						
						/* Write the value from prestera register to the test buffer. */
						*(volatile MV_U32 *)(startOffset+counter) = testVal;
					}   
					break;
			case 1:
					/* Set backup buffer with data from prestera switch. */
					for(counter=0x0 ; counter < 0x20 ; counter+=4)
					{
						testVal = mvPpReadReg (dev, (PRESTERA_SDMA_RX_CURR_DESC_REG_LOCAL + counter));
						
						/* Write the value from prestera register to the test buffer. */
						*(volatile MV_U32 *)(startOffset+counter) = (testVal & 0x0000ffff);
					}					
					break;
			case 2:
					/* Set backup buffer with data from prestera switch. */
					for(counter=0x0 ; counter < 0x20 ; counter+=4)
					{
						testVal = mvPpReadReg (dev, (PRESTERA_SDMA_RX_CURR_DESC_REG_LOCAL + counter));
						
						/* Write the value from prestera register to the test buffer. */
						*(volatile MV_U32 *)(startOffset+counter) = (testVal & 0x000000ff);
					}
					break;
			default:
					if (mvUartPrintEnable(0) != MV_OK)
					{
						mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
						return MV_FAIL;
					}
					mvOsPrintf(" error in loop value (01). \n");
					return MV_FAIL;						
		}		
		
		switch (VarCounter)
		{
			case 0:
					/* Set Compare buffer with data. */
					if(mvBspMemoryFill ((startOffset+0x100), UT_TEST_PATTERN_10, 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (1).\n", __FUNCTION__);
					    return MV_FAIL;
					}   
					break;
			case 1:
					/* Set Compare buffer with data. */
					if(mvBspMemoryFill ((startOffset+0x100), (UT_TEST_PATTERN_10 & 0x0000ffff), 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (3).\n", __FUNCTION__);
					    return MV_FAIL;
					}
					break;
			case 2:
					/* Set Compare buffer with data. */
					if(mvBspMemoryFill ((startOffset+0x100), (UT_TEST_PATTERN_10 & 0x000000ff), 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (5).\n", __FUNCTION__);
					    return MV_FAIL;
					}
					break;
			default:
					if (mvUartPrintEnable(0) != MV_OK)
					{
						mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
						return MV_FAIL;
					}
					mvOsPrintf(" error in loop value (02). \n");
					return MV_FAIL;						
		}		

		for (adrsIndex = PRESTERA_SDMA_RX_CURR_DESC_REG_LOCAL, counter=0x0 ; adrsIndex < endOffset ; adrsIndex += 4, counter += 4)
		{			
			/*====== STEP 02 - write the test pattern to the switch using smw command. ======*/
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (smw.l) */
						sprintf(mem_command, "smw.l %x %x 1", adrsIndex, UT_TEST_PATTERN_10);  
						break;
				case 1:
						/* Run test for word write - (smw.w) */
						sprintf(mem_command, "smw.w %x %x 1", adrsIndex, (UT_TEST_PATTERN_10 & 0x0000ffff));  
						break;
				case 2:
						/* Run test for byte write - (smw.b) */
						sprintf(mem_command, "smw.b %x %x 1", adrsIndex, (UT_TEST_PATTERN_10 & 0x000000ff));  
						break;
				default:
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf(" error in loop value (03). \n");
						return MV_FAIL;						
			}
			
			/* run the memory write command. */
			run_command(mem_command, 0);
			
			/*====== STEP 03 - read the test pattern from the switch using smd command to verify write.. ======*/			
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (smd.l) */
						sprintf(mem_command, "smd.l %x 1", adrsIndex);  
						break;
				case 1:
						/* Run test for word write - (smd.w) */
						sprintf(mem_command, "smd.w %x 1", adrsIndex);  
						break;
				case 2:
						/* Run test for byte write - (smd.b) */
						sprintf(mem_command, "smd.b %x 1", adrsIndex);  
						break;
				default:
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf(" error in loop value (04). \n");
						return MV_FAIL;						
			}
			
			/*=========================================*/
			s_mvLoggerArr[0] = '\0';
			s_mvLoggerArrI = 0x0;			
	
			/* run the memory dispaly command. */
			run_command(mem_command, 0);
			
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (smd.l) */
						compareSize = sprintf(mvUT_compare_str_smdl, "0x%08X: %08x", adrsIndex, (*(volatile MV_U32 *)(startOffset+0x100+counter)));
						/* mvOsPrintf ("\n mvUT_compare_str_smdl = %s\n", mvUT_compare_str_smdl); */
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_smdl, (compareSize-2));   
						break;
				case 1:
						/* Run test for word write - (smd.w) */
						compareSize = sprintf(mvUT_compare_str_smdw, "0x%08X: %04x", adrsIndex, ((*(volatile MV_U32 *)(startOffset+0x100+counter)) & 0xffff0000));
						/* mvOsPrintf ("\n mvUT_compare_str_smdw = %s\n", mvUT_compare_str_smdw); */
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_smdw, (compareSize-2));  
						break;
				case 2:
						/* Run test for byte write - (smd.b) */
						compareSize = sprintf(mvUT_compare_str_smdb, "0x%08X: %02x", adrsIndex, ((*(volatile MV_U32 *)(startOffset+0x100+counter)) & 0x00ff0000));											
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_smdb, (compareSize-2)); 
						break;
				default:
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf(" error in loop value (05). \n");
						return MV_FAIL;						
			}			
			
			if(ret_val != 0x0)
			{
				/* Enable print to UART. */
				if (mvUartPrintEnable(0) != MV_OK)
				{
					mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
					return MV_FAIL;
				}
					
				mvOsPrintf ("\n %d - ", ret_val);
				for(counter_b=0x0 ; counter_b <= compareSize-1; counter_b++)
					mvOsPrintf ("%c", s_mvLoggerArr[counter_b]);
		
				mvOsPrintf ("\n %s - Run Compare for smd command FAILED, compareSize = %d, Offset = 0x%08x.\n", __FUNCTION__, compareSize, (startOffset+0x100+counter));		
				return MV_FAIL;
			}		
			
			/*====== STEP 04 - restore the data to the switch. ======*/						
			/* Write the value from the test buffer back to prestera registers. */
			testVal = *(volatile MV_U32 *)(startOffset+counter);
			mvPpWriteReg(dev, (PRESTERA_SDMA_RX_CURR_DESC_REG_LOCAL + counter), testVal);
			
		} // for()		
	} // for()	
			
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
 
	mvOsPrintf("  smw - modify switch internal register command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvGeneralSwitchMibReadTest
*
*  GENERAL DESCRIPTION: Test the u-boot command switchMibRead - read switch MIB counters for specified port.
*
*  RETURNS: MV_OK or MV_FAIL.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvGeneralSwitchMibReadTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U8  mem_command[25], mem_command_01[50]; 
	static volatile MV_U8  *result = NULL;
	MV_U8  *result_01 = NULL, *result_02 = NULL, *result_03 = NULL;
	MV_U8  *result_05 = NULL, *result_06 = NULL;
	MV_U8  ReadValue[4], ReadValue_01[8], ReadValue_02[8], tempValue = 0x0;
	MV_U8  delay = 0x0;	
	MV_U32 switchPortNum = 0x88;
	MV_U32 counter = 0x0; 

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  switchMibRead - read switch MIB counters command test started... \n");
	
	/* Disable print to UART. */
    if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }
	
	/*====== STEP 00 - A ======*/
	/* Get ethprime device and save it. */  
	mem_command[0] = '\0';
	sprintf(mem_command, "printenv ethprime");  
	
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_01); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		for(counter=0x0 ; counter <= 15; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run strstr() command FAILED, mem_command = %s.\n", __FUNCTION__, mem_command);
		return MV_FAIL;
	}
			
	ReadValue_01[0] = '\0';
	strncpy (ReadValue_01, (const char *)(result+9), 6);
	ReadValue_01[7] = '\0';

	/*====== STEP 00 - B ======*/
	/* Get ethact device and save it. */  
	mem_command[0] = '\0';
	sprintf(mem_command, "printenv ethact");  
	
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_02); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		for(counter=0x0 ; counter <= 15; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run strstr() command for 'printenv ethact' FAILED, mem_command = %s.\n", __FUNCTION__, mem_command);
		return MV_FAIL;
	}
			
	ReadValue_02[0] = '\0';
	strncpy (ReadValue_02, (const char *)(result+7), 6);
	ReadValue_02[7] = '\0';	
	
	/* Set ethprime command for using ppsdma device. */  
	mem_command[0] = '\0';
	sprintf(mem_command, "setenv ethprime ppsdma");  
	
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 	
	/*====== STEP 01 ======*/
	/* Set printenv serverip command */  
	mem_command[0] = '\0';
	sprintf(mem_command, "printenv serverip");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_03); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		for(counter=0x0 ; counter <= 24; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
			
		mvOsPrintf ("\n %s - Run strstr() command for 'printenv serverip' FAILED, mem_command = %s.\n", __FUNCTION__, mem_command);
		return MV_FAIL;
	}
	
	ReadValue[0] = '\0';
	strncpy (ReadValue, (const char *)(result+9), 15);
	ReadValue[15] = '\0';	
				
	/*====== STEP 02 ======*/
	/* Set ethact ppsdma command - Initialize the switch by ping. */  
	mem_command[0] = '\0';
	sprintf(mem_command, "setenv ethact ppsdma");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 								
	/*====== STEP 03 ======*/
	/* Set ping command - ping the HOST to see if it's alive. */ 
	mem_command[0] = '\0';
	sprintf(mem_command, "ping %s", ReadValue);  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
		    
    delay = (USER_PING_DELAY / SW_1_SEC_CNS);

    while(delay)
    {
        udelay(SW_1_SEC_CNS);
        delay--; 
    }
						
	/* fot debug... */
	/* mvOsPrintf ("\n mem_command = %s, s_mvLoggerArr = %s.\n", mem_command, s_mvLoggerArr); */
			 
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_04);
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_05);
	result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_06);
		
	if((result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);			
					
		mvOsPrintf ("\n %s - Run ping command (STEP 03) FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}	
	
	result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_07);
	result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_08);
		
	if((result_05 != NULL)||(result_06 != NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);			
					
		mvOsPrintf ("\n %s - Run ping command (STEP 03) FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}	
	
	/*====== STEP 04 ======*/
	/* Get Switch First Port Link Up */  
    switchPortNum = mvSwitchFirstPortLinkUpGet();
	
	if(switchPortNum == -1)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
						
		mvOsPrintf ("\n Run mvSwitchFirstPortLinkUpGet() return (-1).\n");
		mvOsPrintf ("\n First Initialize switch ports...\n");
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
		
		return MV_FAIL;
	}	
	
	/*====== STEP 05 ======*/
	/* Set switchMibRead command - First clear the MIB counters for the port. */  
	mem_command[0] = '\0';
	sprintf(mem_command, "switchMibRead %d", switchPortNum);  
	
	/* write to the memory address. */
	run_command(mem_command, 0);	
   
	/*====== STEP 06 ======*/
	/* Set ping command - ping the HOST to set some traffic after clear. */ 
	mem_command[0] = '\0';
	sprintf(mem_command, "ping %s", ReadValue);  
	
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
		    
    delay = (USER_PING_DELAY / SW_1_SEC_CNS);

    while(delay)
    {
        udelay(SW_1_SEC_CNS);
        delay--; 
    }
						
	/* fot debug... */
	/* mvOsPrintf ("\n mem_command = %s, s_mvLoggerArr = %s.\n", mem_command, s_mvLoggerArr); */
			 
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_09);	 
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_10);
	result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_11);
		
	if((result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);			
					
		mvOsPrintf ("\n %s - Run ping command (STEP 06) FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}	
	
	result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_12);
	result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_13);
		
	if((result_05 != NULL)||(result_06 != NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);			
					
		mvOsPrintf ("\n %s - Run ping command (STEP 02) FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}   
   
	/*====== STEP 07 ======*/
	/* Set switchMibRead command - Read the MIB counters for the port. */  
	mem_command[0] = '\0';
	sprintf(mem_command, "switchMibRead %d", switchPortNum);  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
	
	/* Set switchMibRead command */  
	mem_command_01[0] = '\0';
	sprintf(mem_command_01, "Port #%d MIB Counters (Port %d Device", switchPortNum, switchPortNum);	
 		 
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mem_command_01);
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_14);
		
	if((result_01 == NULL)||(result_02 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		mvOsPrintf ("\n - ");
		for(counter=0x0 ; counter <= 24; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);		
			
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);			
					
		mvOsPrintf ("\n Run switchMibRead command FAILED.\n");
		mvOsPrintf ("\n switchPortNum = %d, mem_command_01 = %s.\n", switchPortNum, mem_command_01);
		return MV_FAIL;
	}	
		
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_15); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);			
					
		mvOsPrintf ("\n %s - Run strstr() command FAILED, GoodFramesReceived... not found.\n", __FUNCTION__);
		return MV_FAIL;
	}
			
	ReadValue[0] = '\0';
	strncpy (ReadValue, (const char *)(result+30), 1);
	ReadValue[1] = '$';
	ReadValue[2] = '\0';
	/* mvOsPrintf ("%s ", ReadValue); */
			
	tempValue = (MV_U8)my_atoi(ReadValue); 
		/* mvOsPrintf ("%02x ", tempValue); */	
		
	if(tempValue == 0x0)	
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);			
						
		mvOsPrintf ("\n %s - Run switchMibRead command FAILED.\n", __FUNCTION__);
		mvOsPrintf ("\n GoodFramesReceived == 0, tempValue = %d.\n", tempValue);
		return MV_FAIL;
	}
	
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_16); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
					
		mvOsPrintf ("\n %s - Run strstr() command FAILED, BroadcastFramesReceived... not found.\n", __FUNCTION__);
		return MV_FAIL;
	}
			
	ReadValue[0] = '\0';
	strncpy (ReadValue, (const char *)(result+30), 2);
	ReadValue[2] = '\0';
	/* mvOsPrintf ("%s ", ReadValue); */
			
	tempValue = (MV_U8)my_atoi(ReadValue); 
		/* mvOsPrintf ("%02x ", tempValue); */	
		
	if(tempValue == 0x0)	
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
						
		mvOsPrintf ("\n %s - Run switchMibRead command FAILED.\n", __FUNCTION__);
		mvOsPrintf ("\n BroadcastFramesReceived == 0, tempValue = %d.\n", tempValue);
		return MV_FAIL;
	}	
	
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_17); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
					
		mvOsPrintf ("\n %s - Run strstr() command FAILED, MulticastFramesReceived... not found.\n", __FUNCTION__);
		return MV_FAIL;
	}
			
	ReadValue[0] = '\0';
	strncpy (ReadValue, (const char *)(result+30), 1);
	ReadValue[1] = '$';
	ReadValue[2] = '\0';
	/* mvOsPrintf ("%s ", ReadValue); */
			
	tempValue = (MV_U8)my_atoi(ReadValue); 
		/* mvOsPrintf ("%02x ", tempValue); */	
		
	if(tempValue == 0x0)	
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
						
		mvOsPrintf ("\n %s - Run switchMibRead command FAILED.\n", __FUNCTION__);
		mvOsPrintf ("\n MulticastFramesReceived == 0, tempValue = %d.\n", tempValue);
		return MV_FAIL;
	}

	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_18); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
					
		mvOsPrintf ("\n %s - Run strstr() command FAILED, GoodOctetsReceived... not found.\n", __FUNCTION__);
		return MV_FAIL;
	}
			
	ReadValue[0] = '\0';
	strncpy (ReadValue, (const char *)(result+45), 3);
	ReadValue[3] = '\0';
	/* mvOsPrintf ("%s ", ReadValue); */
			
	tempValue = (MV_U8)my_atoi(ReadValue); 
		/* mvOsPrintf ("%02x ", tempValue); */	
		
	if(tempValue == 0x0)	
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
						
		mvOsPrintf ("\n %s - Run switchMibRead command FAILED.\n", __FUNCTION__);
		mvOsPrintf ("\n GoodOctetsReceived == 0, tempValue = %d.\n", tempValue);
		return MV_FAIL;
	}

	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_19); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
					
		mvOsPrintf ("\n %s - Run strstr() command FAILED, GoodFramesSent... not found.\n", __FUNCTION__);
		return MV_FAIL;
	}
			
	ReadValue[0] = '\0';
	strncpy (ReadValue, (const char *)(result+30), 1);
	ReadValue[1] = '$';	
	ReadValue[2] = '\0';
	/* mvOsPrintf ("%s ", ReadValue); */
			
	tempValue = (MV_U8)my_atoi(ReadValue); 
		/* mvOsPrintf ("%02x ", tempValue); */	
		
	if(tempValue == 0x0)	
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
						
		mvOsPrintf ("\n %s - Run switchMibRead command FAILED.\n", __FUNCTION__);
		mvOsPrintf ("\n GoodFramesSent == 0, tempValue = %d.\n", tempValue);
		return MV_FAIL;
	}

	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SwitchMibRead_20); 			  								
	if((result == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
					
		mvOsPrintf ("\n %s - Run strstr() command FAILED, BroadcastFramesSent... not found.\n", __FUNCTION__);
		return MV_FAIL;
	}
			
	ReadValue[0] = '\0';
	strncpy (ReadValue, (const char *)(result+30), 1);
	ReadValue[1] = '$';	
	ReadValue[2] = '\0';
	/* mvOsPrintf ("%s ", ReadValue); */
			
	tempValue = (MV_U8)my_atoi(ReadValue); 
		/* mvOsPrintf ("%02x ", tempValue); */	
		
	if(tempValue == 0x0)	
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/* Set ethprime command for using ppsdma device. */  
		mem_command[0] = '\0';
		sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
			mvOsPrintf ("\n Set ethprime and ethact back to default value. --> %s\n", mem_command);			
		run_command(mem_command, 0);
		sprintf(mem_command, "setenv ethact %s", ReadValue_02);
		run_command(mem_command, 0);		
						
		mvOsPrintf ("\n %s - Run switchMibRead command FAILED.\n", __FUNCTION__);
		mvOsPrintf ("\n BroadcastFramesSent == 0, tempValue = %d.\n", tempValue);
		return MV_FAIL;
	}	
	
	/* Set ethprime command for using ppsdma device. */  
	mem_command[0] = '\0';
	sprintf(mem_command, "setenv ethprime %s", ReadValue_01);
	run_command(mem_command, 0);	
	sprintf(mem_command, "setenv ethact %s", ReadValue_02);
	run_command(mem_command, 0);	
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
 
	mvOsPrintf("  switchMibRead - read switch MIB counters command test ended successfully.\n");
	return MV_OK;
} 

/* **************************************************************************
 *
 *  FUNCTION: mvGeneralDummyTest
 *
 *  GENERAL DESCRIPTION: A Dummy Test command.
 *
 *  RETURNS:
 *
 *  ALGORITHM:
 *
 *  ASSUMPTIONS:
 *
 *  REMARKS:  (local)
 *
 * **************************************************************************
 * */

MV_STATUS mvGeneralDummyTest(void)
{

	return MV_OK;
}

#endif  /* COMMANDS_TEST */

/*$ END OF <module_name> */


