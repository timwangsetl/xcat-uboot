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

#include "../board/mv_feroceon/mv_kw/kw_family/boardEnv/mvBoardEnvLib.h"
#include "../board/mv_feroceon/mv_hal/eth-phy/mvEthPhy.h"

#include "private_diag_if.h"
#include "hwd_config.h"
#include "hwd_memory_strings.h"
#include "diag_inf.h"
#include "diag_services.h"
#include "test_functions.h"
#include "mvUart.h"

#include "private_test_config.h"
#include "sflash/mvSFlash.h"
#include "mvCtrlEnvRegs.h"

/* **************************************************************************
 *              PUBLIC DECLARATIONS (EXPORT)
 * **************************************************************************
 * */
    
/* **************************************************************************
 *              PUBLIC VARIABLE DEFINITIONS (EXPORT)
 * **************************************************************************
 * */
extern int db_98dx4122_detected; 
extern MV_U32 mvCpuIfTargetWinBaseLowGet(MV_TARGET target); 
extern flash_info_t flash_info[];    /* info for FLASH chips */

extern MV_U8 mvSwitchGetDevicesNum(void);
extern MV_BOOL mvPpChipIsXCat2(void);
extern MV_BOOL mvPpIsChipFE(void);

/*  fot DEBUG.  */
/* static MV_SFLASH_INFO sflash; */

#define FLASH_BANK        1

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
   #define UT_TEST_PATTERN_01  0xa5a5a5a5
   #define UT_TEST_PATTERN_02  0x12345678
   #define UT_TEST_PATTERN_03  0xFFFFFFFF
   #define UT_TEST_PATTERN_04  0xdeadface   
   
   #define UT_CRC32_PATTERN_01  0x775f2f85
   #define UT_ICRC32_PATTERN_02 0xe6793ed0

   #define UT_FLASH_ERASE_START_ADRS 0xF82C0000
   #define UT_FLASH_ERASE_END_ADRS   0xF82FFFFF   
   
   #define mvPresteraSingleDev 0x01
   #define mvPresteraBtBDev    0x02

/* **************************************************************************
 *              LOCAL FUNCTION DEFINITIONS
 * **************************************************************************
 * */
 
 extern MV_U32     mvCtrlEthMaxPortGet(MV_VOID);
 /* extern MV_32     mvBoardPhyAddrGet  (MV_U32 portNumber); */
 extern MV_STATUS  mvEthPhyPrintStatus(MV_U32 phyAddr);
 extern MV_STATUS  mvCtrlModelRevNameGet(char *pNameBuff);
 
 /* **************************************************************************
 *              PUBLIC FUNCTION DEFINITIONS (EXPORT)
 * **************************************************************************
 * */
    extern mvBspDiagDramTestParamStruct dram_params;
	
	extern volatile MV_U8 s_mvLoggerArr[];
    extern volatile MV_U32 s_mvLoggerArrI;
	
	extern volatile MV_U8 *mvUTMemAlloc_PTR;
 
/* **************************************************************************
* 
*  FUNCTION: my_atoi
*
*  GENERAL DESCRIPTION: Convert string to integer.
*
*  RETURNS: MV_U32.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS: *string has only 2 Bytes. !!!
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
volatile MV_U32 my_atoi(char *string)
{
    MV_U32 res = 0, placement = 0;
	
    while ((*string>='0' && *string <='9')||(*string>='a' && *string <='f')||(*string>='A' && *string <='F'))
    {
		if(placement >= 2)
			break;
		
		if(*string>='0' && *string <='9')
		{
			res *= 16;
			res += *string-'0';
			string++;
			placement++;			
		}
		else if(*string>='a' && *string <='f')
		{
			res *= 16;
			res += (*string-'a')+10;
			string++;
			placement++;
		}
		else if(*string>='A' && *string <='F')
		{
			res *= 16;
			res += (*string-'A')+10;
			string++;
			placement++;			
		}
    }

    return res;
}

/* **************************************************************************
* 
*  FUNCTION: my_atoi_4_char
*
*  GENERAL DESCRIPTION: Convert string to integer.
*
*  RETURNS: MV_U32.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS: *string has only 4 Bytes. !!!
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
volatile MV_U32 my_atoi_4_char(char *string)
{
    MV_U32 res = 0, placement = 0;
	
    while ((*string>='0' && *string <='9')||(*string>='a' && *string <='f')||(*string>='A' && *string <='F'))
    {
		if(placement >= 4)
			break;
		
		if(*string>='0' && *string <='9')
		{
			res *= 16;
			res += *string-'0';
			string++;
			placement++;			
		}
		else if(*string>='a' && *string <='f')
		{
			res *= 16;
			res += (*string-'a')+10;
			string++;
			placement++;
		}
		else if(*string>='A' && *string <='F')
		{
			res *= 16;
			res += (*string-'A')+10;
			string++;
			placement++;			
		}
    }

    return res;
}

/* **************************************************************************
* 
*  FUNCTION: diagIfGetMarvellBoardName
*
*  GENERAL DESCRIPTION: Get Marvell Board Name.
*
*  RETURNS: void.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS: 
*
*  REMARKS:     char *boardName, char *boardRev
*
* **************************************************************************
* */
void diagIfGetMarvellBoardName(char *boardName, char *boardRev)
{
	MV_U16 ChipRev = 0x0, devicesNum = 0x0;
	
	/* Get the number of Switch devices. */
	devicesNum = mvSwitchGetDevicesNum();

	/* For DEBUG. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
    }
	mvOsPrintf("\n devicesNum = %d\n\n", devicesNum);
		
	/* "The chip type is xCat2" */	
	if (mvPpChipIsXCat2() == MV_TRUE)
    {
		if (mvChipXCat2IsBGA() == MV_TRUE)
		{
			/* mvOsPrintf("\n XCat2IsBGA = %d\n", mvChipXCat2IsBGA()); */ /* For DEBUG. */
			if (mvPpIsChipFE() == MV_TRUE)
			{
				/* mvOsPrintf("\n IsChipFE = %d\n", mvPpIsChipFE()); */ /* For DEBUG. */
				/* "The chip package is BGA." */
				if(devicesNum == mvPresteraSingleDev)
				{
					sprintf(boardName, "%s", "xCat2_DB-24F");
				}
				else if(devicesNum == mvPresteraBtBDev)
				{
					sprintf(boardName, "%s", "xCat2_DB-48F");
				}
			}
			else /* "GE" */
			{
				/* "The chip package is BGA." */
				if(devicesNum == mvPresteraSingleDev)
				{
					sprintf(boardName, "%s", "xCat2_DB-24G");
				}
				else if(devicesNum == mvPresteraBtBDev)
				{
					sprintf(boardName, "%s", "xCat2_DB-48G");
				}			
			}
		}
		else
		{
			/* "The chip package is QFP." */
			if(devicesNum == mvPresteraSingleDev)
			{
				sprintf(boardName, "%s", "xCat2_DB-24QFP");
			}
			else if(devicesNum == mvPresteraBtBDev)
			{
				sprintf(boardName, "%s", "xCat2_DB-48QFP");
			}
		}	
	}
	else  /* "The chip type is xCat" */ /* "The chip package is BGA." */
	{		
		if (mvPpIsChipFE() == MV_TRUE)
		{			
			if(devicesNum == mvPresteraSingleDev)
			{
				sprintf(boardName, "%s", "xCat_DB-24F");
			}
			else if(devicesNum == mvPresteraBtBDev)
			{
				sprintf(boardName, "%s", "xCat_DB-48F");
			}
		}
		else /* "GE" */
		{
			if(devicesNum == mvPresteraSingleDev)
			{
				sprintf(boardName, "%s", "xCat_DB-24G");
			}
			else if(devicesNum == mvPresteraBtBDev)
			{
				sprintf(boardName, "%s", "xCat_DB-48G");
			}			
		}
	}
	
	/* Get board revision */
	ChipRev = mvBoardChipRevGet();
	if( ChipRev == 0x0)
	{
		sprintf(boardRev, "%s", "(Rev 0) ");
	}
	else if( ChipRev == 0x1)
	{
		sprintf(boardRev, "%s", "(Rev 1) ");
	}	
	else if( ChipRev == 0x2)
	{
		sprintf(boardRev, "%s", "(Rev 2) ");
	}
	else if( ChipRev == 0x3)
	{
		sprintf(boardRev, "%s", "(Rev 3) ");
	}	
	else {boardRev = "";}
	
	/* mvOsPrintf("\n ChipRev = %d\n", ChipRev); */ /* For DEBUG. */
	return;
}

/* **************************************************************************
* 
*  FUNCTION: diagIfGetMarvellBoardType
*
*  GENERAL DESCRIPTION: Get Marvell Board Type.
*
*  RETURNS: MV_U16.
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS: 
*
*  REMARKS:     
*
* **************************************************************************
* */
MV_U16 diagIfGetMarvellBoardType(void)
{
	MV_U8  *MVboardName = NULL;
	static char boardName[30];
	static char boardRev[10];	
	MV_U8  BoardType = 0x0;
	
	/* Get boardName and revision. */
	diagIfGetMarvellBoardName(boardName, boardRev);

	MVboardName = strstr((char *)boardName, "xCat_DB-24G");
	if(MVboardName != NULL)
	{
		if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
			BoardType = xCat_DB_24GE_Rev_0;   /* BT -> 1 */
		else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
			BoardType = xCat_DB_24GE_Rev_1;   /* BT -> 2 */			
		else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
			BoardType = xCat_DB_24GE_Rev_2;   /* BT -> 3 */
		else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
			BoardType = xCat_DB_24GE_Rev_3;   /* BT -> 4 */			
		else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
				BoardType = MV_INIT_ERROR;}	
	}	
	else
	{
		MVboardName = strstr((char *)boardName, "xCat_DB-24F");
		if(MVboardName != NULL)
		{
			if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
				BoardType = xCat_DB_24FE_Rev_0;    /* BT -> 5 */
			else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
				BoardType = xCat_DB_24FE_Rev_1;    /* BT -> 6 */				
			else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
				BoardType = xCat_DB_24FE_Rev_2;    /* BT -> 7 */
			else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
				BoardType = xCat_DB_24FE_Rev_3;    /* BT -> 8 */				
			else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
					BoardType = MV_INIT_ERROR;}	
		}	
		else
		{
			MVboardName = strstr((char *)boardName, "xCat2_DB-24G");
			if(MVboardName != NULL)
			{
				if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
					BoardType = xCat2_DB_24GE_Rev_0;   /* BT -> 9 */
				else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
					BoardType = xCat2_DB_24GE_Rev_1;   /* BT -> 10 */					
				else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
					BoardType = xCat2_DB_24GE_Rev_2;   /* BT -> 11 */
				else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
					BoardType = xCat2_DB_24GE_Rev_3;   /* BT -> 12 */					
				else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
						BoardType = MV_INIT_ERROR;}	
			}
			else
			{
				MVboardName = strstr((char *)boardName, "xCat2_DB-24F");
				if(MVboardName != NULL)
				{
					if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
						BoardType = xCat2_DB_24FE_Rev_0;    /* BT -> 13 */
					else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
						BoardType = xCat2_DB_24FE_Rev_1;   /* BT -> 14 */						
					else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
						BoardType = xCat2_DB_24FE_Rev_2;   /* BT -> 15 */
					else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
						BoardType = xCat2_DB_24FE_Rev_3;   /* BT -> 16 */						
					else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
							BoardType = MV_INIT_ERROR;}	
				}
				else
				{
					MVboardName = strstr((char *)boardName, "xCat2_DB-24QFP");
					if(MVboardName != NULL)
					{
						if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
							BoardType = xCat2_DB_24QFP_Rev_0;    /* BT -> 17 */
						else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
							BoardType = xCat2_DB_24QFP_Rev_1;   /* BT -> 18 */							
						else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
							BoardType = xCat2_DB_24QFP_Rev_2;   /* BT -> 19 */
						else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
							BoardType = xCat2_DB_24QFP_Rev_3;   /* BT -> 20 */							
						else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
								BoardType = MV_INIT_ERROR;}	
					}
				}
			}
		}			
	}			
	
	if(BoardType == 0x0)
	{
		/* define boardType, -  second check for 48 ports. */		
		MVboardName = strstr((char *)boardName, "xCat_DB-48G");
		if(MVboardName != NULL)
		{
			if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
				BoardType = xCat_DB_48GE_Rev_0;   /* BT -> 21 */
			else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
				BoardType = xCat_DB_48GE_Rev_1;   /* BT -> 22 */				
			else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
				BoardType = xCat_DB_48GE_Rev_2;   /* BT -> 23 */
			else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
				BoardType = xCat_DB_48GE_Rev_3;   /* BT -> 24 */				
			else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
					BoardType = MV_INIT_ERROR;}	
		}	
		else
		{
			MVboardName = strstr((char *)boardName, "xCat_DB-48F");
			if(MVboardName != NULL)
			{
				if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
					BoardType = xCat_DB_48FE_Rev_0;    /* BT -> 25 */
				else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
					BoardType = xCat_DB_48FE_Rev_1;    /* BT -> 26 */					
				else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
					BoardType = xCat_DB_48FE_Rev_2;    /* BT -> 27 */
				else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
					BoardType = xCat_DB_48FE_Rev_3;    /* BT -> 28 */					
				else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
						BoardType = MV_INIT_ERROR;}	
			}	
			else
			{
				MVboardName = strstr((char *)boardName, "xCat2_DB-48G");
				if(MVboardName != NULL)
				{
					if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
						BoardType = xCat2_DB_48GE_Rev_0;   /* BT -> 29 */
					else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
						BoardType = xCat2_DB_48GE_Rev_1;   /* BT -> 30 */						
					else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
						BoardType = xCat2_DB_48GE_Rev_2;   /* BT -> 31 */
					else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
						BoardType = xCat2_DB_48GE_Rev_3;   /* BT -> 32 */						
					else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
							BoardType = MV_INIT_ERROR;}	
				}
				else
				{
					MVboardName = strstr((char *)boardName, "xCat2_DB-48F");
					if(MVboardName != NULL)
					{
						if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
							BoardType = xCat2_DB_48FE_Rev_0;   /* BT -> 33 */
						else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
							BoardType = xCat2_DB_48FE_Rev_1;   /* BT -> 34 */							
						else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
							BoardType = xCat2_DB_48FE_Rev_2;   /* BT -> 35 */
						else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
							BoardType = xCat2_DB_48FE_Rev_3;   /* BT -> 36 */							
						else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
								BoardType = MV_INIT_ERROR;}	
					}
					else
					{
						MVboardName = strstr((char *)boardName, "xCat2_DB-48QFP");
						if(MVboardName != NULL)
						{
							if(strstr((char *)boardRev, "(Rev 0) ") != NULL)
								BoardType = xCat2_DB_48QFP_Rev_0;    /* BT -> 37 */
							else if(strstr((char *)boardRev, "(Rev 1) ") != NULL)
								BoardType = xCat2_DB_48QFP_Rev_1;    /* BT -> 38 */								
							else if(strstr((char *)boardRev, "(Rev 2) ") != NULL)
								BoardType = xCat2_DB_48QFP_Rev_2;    /* BT -> 39 */
							else if(strstr((char *)boardRev, "(Rev 3) ") != NULL)
								BoardType = xCat2_DB_48QFP_Rev_3;    /* BT -> 40 */								
							else {mvOsPrintf ("\n %s - Unknown boardRev.\n", __FUNCTION__);
									BoardType = MV_INIT_ERROR;}	
						}
					}
				}
			}			
		}			
	}	

	return BoardType;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryMeTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command me - PCI master enable.
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

MV_STATUS mvMemoryMeTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 ret_val = 0x0, compareSize = 0x0;  
  MV_U32 counter = 0x0; 
  MV_U8  mem_command[25]; 
  static volatile MV_U8  *result = NULL;  
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  me - PCI master enable command test started... \n");
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	
	/*====== STEP 01 ======*/	
	sprintf(mem_command, "me 0");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);	
		
	result     = strstr((char *)s_mvLoggerArr, mvUT_compare_str_meEnable); 
			  								
	if(result == NULL)
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
			
		mvOsPrintf ("\n Run me command (STEP 01) FAILED.\n");
		return MV_FAIL;
	}	
					  
	/*====== STEP 02 ======*/					  
	sprintf(mem_command, "me 1");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	result     = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_meExist); 
			  								
	if(result == NULL)
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
			
		mvOsPrintf ("\n Run me command (STEP 02) FAILED.\n");
		return MV_FAIL;
	}
	  
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  me - PCI master enable command test ended successfully.\n");

	return MV_OK;
}	
	
/* **************************************************************************
 *
 *  FUNCTION: mvMemoryMpTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command mp - map PCI BAR.
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

MV_STATUS mvMemoryMpTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 ret_val = 0x0, compareSize = 0x0;  
  MV_U32 counter = 0x0; 
  MV_U8  mem_command[25]; 
  static volatile MV_U8  *result = NULL, *result_01 = NULL;
  
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  mp - map PCI BAR command test started... \n");
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	
	/*====== STEP 01 ======*/	
	sprintf(mem_command, "mp");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);	
		
	result     = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_mp_01); 
	result_01  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_mp_02); 
			  								
	if((result == NULL)||(result_01 == NULL))
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
			
		mvOsPrintf ("\n Run mp command (STEP 01) FAILED.\n");
		return MV_FAIL;
	}	
					  
	/*====== STEP 02 ======*/					  
	sprintf(mem_command, "mp 0 80000000");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	result     = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_mp_01); 
	result_01  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_mp_02); 	
			  								
	if((result == NULL)||(result_01 == NULL))
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
			
		mvOsPrintf ("\n Run mp command (STEP 02) FAILED.\n");
		return MV_FAIL;
	}
	
	/*====== STEP 03 ======*/	
	sprintf(mem_command, "mp 1 80000000");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_mp_03); 
			  								
	if(result == NULL)
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
			
		mvOsPrintf ("\n Run mp command (STEP 03) FAILED.\n");
		return MV_FAIL;
	}	
  
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  mp - map PCI BAR command test ended successfully.\n");

	return MV_OK;
}	
		
/* **************************************************************************
 *
 *  FUNCTION: mvMemoryPciTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command pci - list PCI configuration space.
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

MV_STATUS mvMemoryPciTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 ret_val = 0x0, compareSize = 0x0;  
  MV_U32 counter = 0x0; 
  MV_U8  mem_command[25]; 
  static volatile MV_U8  *result = NULL, *result_01 = NULL, *result_02 = NULL, *result_03 = NULL;
  static volatile MV_U8  *result_04 = NULL, *result_05 = NULL, *result_06 = NULL;    
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  pci - list PCI configuration space command test started... \n");
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	
	/*====== STEP 01 ======*/	
	sprintf(mem_command, "pci");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);	
		                
	result     = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_pci_01); 
	result_01  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_pci_02);	
	result_02  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_pci_03); 
	result_03  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_pci_04); 
	result_04  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_pci_05); 
	result_05  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_pci_06); 
	result_06  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_pci_07);	
  			  								
	if((result == NULL)||(result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL) \
	    ||(result_05 == NULL)||(result_06 == NULL))
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
			
		mvOsPrintf ("\n Run pci command (STEP 01) FAILED.\n");
		return MV_FAIL;
	}	
					   
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  pci - list PCI configuration space command test ended successfully.\n");

	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvMemoryPhyWriteTest
*
*  GENERAL DESCRIPTION: Test the u-boot command phyWrite - write to OOB phy register.
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
MV_STATUS mvMemoryPhyWriteTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 counter = 0x0;
	MV_U32 ret_val = 0x0;  
	MV_U32 phyPort = 0x0, maxPorts = 0x0;
	MV_U32 phyAddress = 0x0;
	MV_U8  mem_command[25];    
	MV_U8  tst_value[16];  
	MV_32  restore_int = 0x0;	
	  
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  phyWrite - write value to OOB phy register command test started... \n");
	
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
	
	/*====== STEP 00 ======*/	
	/* First get the PHY address.  */
	
	maxPorts   = mvCtrlEthMaxPortGet();
	if(maxPorts > 0x1)
		maxPorts = 0x1;
	
	/* Enable print to UART. */
	/* if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	} */
	
    for( phyPort = 0 ; phyPort < maxPorts; phyPort++ )
	{
		phyAddress = mvBoardPhyAddrGet(phyPort);	
        mvOsPrintf( "PHY %d : (phyAddress = 0x%x)\n", phyPort, phyAddress);
        mvOsPrintf( "-------------------------------\n" );
		
        mvEthPhyPrintStatus(phyAddress);
        mvOsPrintf("\n");
    }
	
	/* Enable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
		return MV_FAIL;
	}	
	
	/*====== STEP 01 ======*/   
	/* Run test for phy read - (phyRead Phy_address Phy_offset) */
	/* First Read PHY page address register (register 22 (0x16)) - check that page is 0x0.  */
	sprintf(mem_command, "phyRead %d 16", phyAddress);  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
			
	/* Run test for phy read - (phyRead) */			
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, "0x0", 3);   
			
	if(ret_val != 0x0)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}	
	
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 14; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run Compare for phyRead command (STEP 01) FAILED. \n", __FUNCTION__);
		return MV_FAIL;
	}   
   
	/*====== STEP 02 ======*/   
	/* Read Copper Specific Control register-1 (register 16 (0x10)) - Address data bits 0:15 */
	sprintf(mem_command, "phyRead %d 10", phyAddress);  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;		
	tst_value[0] = '\0';
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
			
	/*====== STEP 03 ======*/			
	/* Save the value from Copper Specific Control register-1 in 'tst_value[]' */
	strncat((char *)tst_value, (MV_U8 *)(s_mvLoggerArr+2), 4);   
			
	/* Write to Copper Specific Control register-1 (register 16 (0x10)) - 0xa57e */
	sprintf(mem_command, "phyWrite %d 10 a57e", phyAddress);  
				
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
	
	/*====== STEP 04 ======*/	
	/***** VERIFY *****/
	/* Read Copper Specific Control register-1 (register 16 (0x10)) - Address data bits 0:15 */
	sprintf(mem_command, "phyRead %d 10", phyAddress);  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;		
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);	
				
	/*====== STEP 05 ======*/				
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, "0xa57e", 5);   
			
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
			
		mvOsPrintf ("\n %s - Run Compare for phyRead command FAILED.(STEP 05)\n", __FUNCTION__);
		return MV_FAIL;
	}	
	
	/*====== STEP 06 ======*/	
	/***** RESTORE *****/
	/* Write to Copper Specific Control register-1 (register 16 (0x10)) - original saved value from 'tst_value[]' */
	restore_int = my_atoi_4_char(tst_value);
	sprintf(mem_command, "phyWrite %d 10 %x", phyAddress, restore_int);   	
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);	
	
	/*====== STEP 07 ======*/	
	/***** VERIFY restore *****/
	/* Read Copper Specific Control register-1 (register 16 (0x10)) - Address data bits 0:15 */
	sprintf(mem_command, "phyRead %d 10", phyAddress);  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;		
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);	
				
	ret_val = strncmp((MV_U8 *)(s_mvLoggerArr+2), tst_value, 4);   
			
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
			
		mvOsPrintf ("\n %s - Run Compare for phyWrite command FAILED. (STEP 07), tst_value[] = %s, restore_int = %s\n", __FUNCTION__, tst_value, restore_int);
		return MV_FAIL;
	}	
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  phyWrite - write value to OOB phy register command test ended successfully. \n");
	mvOsPrintf("             (phyAddress = 0x%x).\n", phyAddress); 
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvMemoryPhyReadTest
*
*  GENERAL DESCRIPTION: Test the u-boot command phyRead - read OOB phy register.
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
MV_STATUS mvMemoryPhyReadTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 counter = 0x0;
	MV_U32 ret_val = 0x0;  
	MV_U32 phyPort = 0x0, maxPorts = 0x0;
	MV_U32 phyAddress = 0x0;
	MV_U8  mem_command[25];
	MV_U8  *result = NULL;    
	MV_U8  BoardType = 0x0;  	
	  
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  phyRead - read OOB phy register value command test started... \n");
		
	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();	
	
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }	
	
	/*====== STEP 00 ======*/	
	/* First get the PHY address.  */
	
	maxPorts   = mvCtrlEthMaxPortGet();
	if(maxPorts > 0x1)
		maxPorts = 0x1;
	
	/* Enable print to UART. */
	/* if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	} */
	
    for( phyPort = 0 ; phyPort < maxPorts; phyPort++ )
	{
		phyAddress = mvBoardPhyAddrGet(phyPort);	
        mvOsPrintf( "PHY %d : (phyAddress = 0x%x)\n", phyPort, phyAddress);
        mvOsPrintf( "-------------------------------\n" );
		
        mvEthPhyPrintStatus(phyAddress);
        mvOsPrintf("\n");
    }
	
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
		return MV_FAIL;
	}	
	
	/*====== STEP 01 ======*/   
	/* Run test for phy read - (phyRead Phy_address Phy_offset) */
	/* First Read PHY page address register (register 22 (0x16)) - check that page is 0x0.  */
	sprintf(mem_command, "phyRead %d 16", phyAddress);  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
			
	/* Run test for phy read - (phyRead) */			
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, "0x0", 3);   
			
	if(ret_val != 0x0)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}	
	
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 14; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run Compare for phyRead command (STEP 01) FAILED. \n", __FUNCTION__);
		return MV_FAIL;
	}   
   
	/*====== STEP 02 ======*/   
	/* Read PHY identifier_1 register (register 2) - Organizationally Uniqe Identifier bits 3:18 */
	/* Marvell OUI is 0x005043 --> bits 3:18 of OUI shawn in reg. 2 bits 0:15 == 0x0141 */
	sprintf(mem_command, "phyRead %d 2", phyAddress);  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
			
	/* Run test for phy read - (phyRead) */			
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, "0x141", 5);   
			
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
			
		mvOsPrintf ("\n %s - Run Compare for phyRead command (STEP 02) FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}

	/*====== STEP 03 ======*/	
	/* Read PHY identifier_2 register (register 3) - Organizationally Uniqe Identifier bits 19:24 */
	/* Marvell OUI LSB is 3 --> bits 19:24 of OUI LSB shawn in reg. 3 bits 10:15 == 0x000011 */
	sprintf(mem_command, "phyRead %d 3", phyAddress);  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
		
	/* Run test for phy read - (phyRead) */					
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		result  = strstr ((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyread);
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyread_E);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyread_B);	
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyread_A);	
	}
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyread_C);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyread_D);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_phyread_F);		
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;			
			  		
	if(result == NULL)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}	
	
		mvOsPrintf ("\n %s - ", result);
		for(counter=0x0 ; counter <= 7; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf("\n %s - Run Compare for phyRead command (STEP 03) FAILED.\n", __FUNCTION__);
		mvOsPrintf("        (phyAddress = 0x%x), BoardType = %d.\n", phyAddress, BoardType);		
		return MV_FAIL;
	}	
		
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}	
 
	mvOsPrintf("  phyRead - read OOB phy register value command test ended successfully. (BoardType = %d)\n", BoardType);
	mvOsPrintf("            (phyAddress = 0x%x).\n", phyAddress);  
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvMemoryWriteTest
*
*  GENERAL DESCRIPTION: Test the u-boot command mw - memory write.
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
MV_STATUS mvMemoryWriteTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

	MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
	MV_U32 endOffset   = startOffset + 0x20;  
	MV_U8  mem_command[25];    
	MV_U8  VarCounter = 0x0;
	register MV_U32 curVal  = 0x0;
	register MV_U32 testVal = 0x0;
	register MV_U32 adrsIndex = 0x0;
	register MV_U32 requestedVal = 0x0;
    
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

  mvOsPrintf("\n  mw - Memory Write command test started... ");
  mvOsPrintf("\n       start address is : 0x%x\n", startOffset);
  	
  for(VarCounter=0x0 ; VarCounter < 2 ; VarCounter++)
  {
		/* Clear the test buffer. */
		memset ((MV_U32 *)startOffset, 0x0, 0x20);
  
		for (adrsIndex = startOffset + dram_params.base_address;
			adrsIndex < endOffset + dram_params.base_address;
			adrsIndex += 4)
		{    
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (mw.l) */
						sprintf(mem_command, "mw.l %x %x 1", adrsIndex, adrsIndex);  
						break;
				case 1:
						/* Run test for word write - (mw.w) */
						sprintf(mem_command, "mw.w %x %x 1", adrsIndex, 0x1234);  
						break;
				case 2:
						/* Run test for byte write - (mw.b) */
						sprintf(mem_command, "mw.b %x %x 1", adrsIndex, 0xa5);  
						break;
				default:
						mvOsPrintf(" error in loop value. \n");
						return MV_FAIL;						
			}
	
			/* save the current address value */
			curVal = *(volatile MV_U32 *)adrsIndex;

			/* write to the memory address. */
			run_command(mem_command, 0);
 
			/* Read the written value */
			testVal = *(volatile MV_U32 *)adrsIndex;

			/* restore the original value to the address */
			*(volatile MV_U32 *)adrsIndex = curVal;

			switch (VarCounter)
			{
				case 0:
						requestedVal = adrsIndex;  /* for - (mw.l) */
						break;
				case 1:
						requestedVal = 0x1234;     /* for - (mw.w) */
						break;
				case 2:
						requestedVal = 0xa5;       /* for - (mw.b) */
						break;
				default:
						mvOsPrintf(" error in loop value. \n");
						return MV_FAIL;						
			}			

			if (testVal != requestedVal)
			{
				mvOsPrintf("\nMemory Write Test: At address 0x%x writen", adrsIndex);
				mvOsPrintf(" value is 0x%x but read value is 0x%x\n", requestedVal, testVal);
				return MV_FAIL;
			}
		} // for()
  } // for()
  
  mvOsPrintf("  mw - Memory Write command test ended successfully.\n");
  return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvMemoryDisplayTest
*
*  GENERAL DESCRIPTION: Test the u-boot command md - memory display.
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
MV_STATUS mvMemoryDisplayTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR, counter = 0x0;
  MV_U32 endOffset   = startOffset + 0x20, ret_val = 0x0, compareSize = 0x0;  
  MV_U8  mem_command[25];    
  MV_U8  VarCounter = 0x0;

  MV_U32 testVal = 0x0;
  MV_U32 adrsIndex = 0x0;
  MV_U32 requestedVal = 0x0;
  
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

  mvOsPrintf("\n  md - Memory display command test started... ");
  mvOsPrintf("\n       start address is : 0x%x\n", startOffset);
  
  /* Disable print to UART. */
  if (mvUartPrintDisable(0) != MV_OK)
  {
    mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
    return MV_FAIL;
  }  
  
  for(VarCounter=0x0 ; VarCounter <= 2 ; VarCounter++)
  {
		/* Clear the test buffer. */
		memset ((MV_U32 *)startOffset, 0x0, 0x20);
					
		switch (VarCounter)
		{
			case 0:
					/* Set Compare buffer with data. */
					if(mvBspMemoryFill (startOffset, UT_TEST_PATTERN_01, 0x20) != MV_OK)
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
					if(mvBspMemoryFill (startOffset, (UT_TEST_PATTERN_01 & 0x0000ffff), 0x20) != MV_OK)
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
					if(mvBspMemoryFill (startOffset, (UT_TEST_PATTERN_01 & 0x000000ff), 0x20) != MV_OK)
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
					mvOsPrintf(" error in loop value. \n");
					return MV_FAIL;						
		}		
  
		for (adrsIndex = startOffset ; adrsIndex < endOffset ; adrsIndex += 4)
		{    
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (md.l) */
						sprintf(mem_command, "md.l %x 1", adrsIndex);  
						break;
				case 1:
						/* Run test for word write - (md.w) */
						sprintf(mem_command, "md.w %x 1", adrsIndex);  
						break;
				case 2:
						/* Run test for byte write - (md.b) */
						sprintf(mem_command, "md.b %x 1", adrsIndex);  
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
						/* Run test for long write - (md.l) */
						compareSize = sprintf(mvUT_compare_str_mdl, "%08x: %08x", adrsIndex, UT_TEST_PATTERN_01);
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_mdl, (compareSize-1));   
						break;
				case 1:
						/* Run test for word write - (md.w) */
						compareSize = sprintf(mvUT_compare_str_mdw, "%08x: %04x", adrsIndex, (UT_TEST_PATTERN_01 & 0x0000ffff));						
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_mdw, (compareSize-1));  
						break;
				case 2:
						/* Run test for byte write - (md.b) */
						compareSize = sprintf(mvUT_compare_str_mdb, "%08x: %02x", adrsIndex, (UT_TEST_PATTERN_01 & 0x000000ff));											
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_mdb, (compareSize-1)); 
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
				for(counter=0x0 ; counter <= compareSize; counter++)
					mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
				mvOsPrintf ("\n %s - Run Compare command FAILED, compareSize = %d.\n", __FUNCTION__, compareSize);
				return MV_FAIL;
			}			
 
			/* Read the written value */
			testVal = *(volatile MV_U32 *)adrsIndex;

			switch (VarCounter)
			{
				case 0:
						requestedVal = UT_TEST_PATTERN_01;  /* for - (md.l) */
						break;
				case 1:
						requestedVal = (UT_TEST_PATTERN_01 & 0x0000ffff);     /* for - (md.w) */
						break;
				case 2:
						requestedVal = (UT_TEST_PATTERN_01 & 0x000000ff);       /* for - (md.b) */
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

			if (testVal != requestedVal)
			{
				mvOsPrintf("\nMemory display Test: At address 0x%x writen", adrsIndex);
				mvOsPrintf(" value is 0x%x but read value is 0x%x\n", requestedVal, testVal);
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
  
  mvOsPrintf("  md - Memory display command test ended successfully.\n");
  return MV_OK;
}
 
/* **************************************************************************
*
*  FUNCTION: mvMemoryCopyTest
*
*  GENERAL DESCRIPTION: Test the u-boot command cp - memory copy.
*
*  RETURNS:
*
*  ALGORITHM:   (local)
*
*  ASSUMPTIONS:
*
*  REMARKS:     (local)
*
* **************************************************************************
* */
MV_STATUS mvMemoryCopyTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/
  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
  MV_U32 endOffset   = startOffset + 0x20;  
  MV_U8  mem_command[25]; 
  MV_U8  VarCounter = 0x0;  
  
  register MV_U32 testVal;
  register MV_U32 adrsIndex;
  register MV_U32 requestedVal;

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

  mvOsPrintf("\n  cp - Memory Copy command test started... ");
  mvOsPrintf("\n       start address is : 0x%x\n", startOffset);
  
  for(VarCounter=0x0 ; VarCounter <= 2 ; VarCounter++)
  {
		switch (VarCounter)
		{
			case 0:
					/* Set copy buffers with data. */
					memset ((MV_U32 *)startOffset, UT_TEST_PATTERN_01, 0x20);
					memset ((MV_U32 *)(startOffset+0x100), 0x0, 0x20);   
					break;
			case 1:
					/* Set copy buffers with data. */
					memset ((MV_U32 *)startOffset, (UT_TEST_PATTERN_01 & 0x0000ffff), 0x20);
					memset ((MV_U32 *)(startOffset+0x100), 0x0, 0x20);  
					break;
			case 2:
					/* Set copy buffers with data. */
					memset ((MV_U32 *)startOffset, (UT_TEST_PATTERN_01 & 0x000000ff), 0x20);
					memset ((MV_U32 *)(startOffset+0x100), 0x0, 0x20);  
					break;
			default:
					mvOsPrintf(" error in loop value. \n");
					return MV_FAIL;						
		}  
  
		for (adrsIndex = startOffset + dram_params.base_address;
			adrsIndex < endOffset + dram_params.base_address;
			adrsIndex += 4)
		{    
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (mw.l) */
						sprintf(mem_command, "cp.l %x %x %x", adrsIndex, (adrsIndex+0x100), 1);   
						break;
				case 1:
						/* Run test for word write - (mw.w) */
						sprintf(mem_command, "cp.w %x %x %x", adrsIndex, (adrsIndex+0x100), 1);  
						break;
				case 2:
						/* Run test for byte write - (mw.b) */
						sprintf(mem_command, "cp.b %x %x %x", adrsIndex, (adrsIndex+0x100), 1);  
						break;
				default:
						mvOsPrintf(" error in loop value. \n");
						return MV_FAIL;						
			}
	
			/* write to the memory address. */
			run_command(mem_command, 0);
 
			/* Read the written value */
			testVal = *(volatile MV_U32 *)(adrsIndex+0x100);

			switch (VarCounter)
			{
				case 0:
						requestedVal = UT_TEST_PATTERN_01;  /* for - (mw.l) */
						break;
				case 1:
						requestedVal = (UT_TEST_PATTERN_01 & 0x0000ffff); /* for - (mw.w) */
						break;
				case 2:
						requestedVal = (UT_TEST_PATTERN_01 & 0x000000ff); /* for - (mw.b) */
						break;
				default:
						mvOsPrintf(" error in loop value. \n");
						return MV_FAIL;						
			}			

			if (testVal != requestedVal)
			{
				mvOsPrintf("\nMemory Write Test: At address 0x%x writen", adrsIndex);
				mvOsPrintf(" value is 0x%x but read value is 0x%x\n", requestedVal, testVal);
				return MV_FAIL;
			}
		} // for()
  } // for()  
   
  mvOsPrintf("  cp - Memory Copy command test ended successfully.\n");

  return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryCompareTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command cmp - memory compare.
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

MV_STATUS mvMemoryCompareTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
  MV_U32 endOffset   = startOffset + 0x20, ret_val = 0x0, compareSize = 0x0;  
  MV_U32  counter = 0; 
  MV_U8  mem_command[25]; 
  MV_U8  VarCounter = 0x0;  
  MV_U32 testVal, testVal_A;
  register MV_U32 adrsIndex;
  register MV_U32 requestedVal;
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

  mvOsPrintf("\n  cmp - Memory Compare command test started... ");
  mvOsPrintf("\n        start address is : 0x%x\n", startOffset);
  
  /* Disable print to UART. */
  if (mvUartPrintDisable(0) != MV_OK)
  {
    mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
    return MV_FAIL;
  }  
  
  for(VarCounter=0x0 ; VarCounter < 3 ; VarCounter++)
  {
  		/* Clear Compare buffers. */
		memset ((MV_U32 *)startOffset, 0x0, 0x20);
		memset ((MV_U32 *)(startOffset+0x100), 0x0, 0x20);
					
		switch (VarCounter)
		{
			case 0:
					/* Set Compare buffers with data. */
					if(mvBspMemoryFill (startOffset, UT_TEST_PATTERN_02, 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (1).\n", __FUNCTION__);
					    return MV_FAIL;
					}

					if(mvBspMemoryFill ((startOffset+0x100), UT_TEST_PATTERN_02, 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (2).\n", __FUNCTION__);
					    return MV_FAIL;
					}   
					break;
			case 1:
					/* Set Compare buffers with data. */
					if(mvBspMemoryFill (startOffset, (UT_TEST_PATTERN_02 & 0x0000ffff), 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (3).\n", __FUNCTION__);
					    return MV_FAIL;
					}

					if(mvBspMemoryFill ((startOffset+0x100), (UT_TEST_PATTERN_02 & 0x0000ffff), 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (4).\n", __FUNCTION__);
					    return MV_FAIL;
					}
					break;
			case 2:
					/* Set Compare buffers with data. */
					if(mvBspMemoryFill (startOffset, (UT_TEST_PATTERN_02 & 0x000000ff), 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (5).\n", __FUNCTION__);
					    return MV_FAIL;
					}

					if(mvBspMemoryFill ((startOffset+0x100), (UT_TEST_PATTERN_02 & 0x000000ff), 0x20) != MV_OK)
					{
						if (mvUartPrintEnable(0) != MV_OK)
						{
							mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
							return MV_FAIL;
						}
						mvOsPrintf("\n %s - Memory Fill error (6).\n", __FUNCTION__);
					    return MV_FAIL;
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
  
		for (adrsIndex = startOffset; adrsIndex < endOffset; adrsIndex += 4)
		{    
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (mw.l) */
						sprintf(mem_command, "cmp.l %x %x %x", adrsIndex, (adrsIndex+0x100), 1);   
						break;
				case 1:
						/* Run test for word write - (mw.w) */
						sprintf(mem_command, "cmp.w %x %x %x", adrsIndex, (adrsIndex+0x100), 1);  
						break;
				case 2:
						/* Run test for byte write - (mw.b) */
						sprintf(mem_command, "cmp.b %x %x %x", adrsIndex, (adrsIndex+0x100), 1);  
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
	
			/* write to the memory address. */
			run_command(mem_command, 0);
									
			switch (VarCounter)
			{
				case 0:
						/* Run test for long write - (mw.l) */
						compareSize = sizeof(mvUT_compare_str_cmpl);
						/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cmpl, (compareSize-1));   
						break;
				case 1:
						/* Run test for word write - (mw.w) */
						compareSize = sizeof(mvUT_compare_str_cmpw);
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cmpw, (compareSize-1));  
						break;
				case 2:
						/* Run test for byte write - (mw.b) */
						compareSize = sizeof(mvUT_compare_str_cmpb);
						ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cmpb, (compareSize-1)); 
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
			  								
			if(ret_val != 0)
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
			
				mvOsPrintf ("\n Run Compare command FAILED.\n");
				return MV_FAIL;
			}
 
			/* Read the written value */
			testVal   = *(volatile MV_U32 *)(adrsIndex);
			testVal_A = *(volatile MV_U32 *)(adrsIndex+0x100);

			switch (VarCounter)
			{
				case 0:
						requestedVal = UT_TEST_PATTERN_02;  /* for - (mw.l) */
						break;
				case 1:
						requestedVal = (UT_TEST_PATTERN_02 & 0x0000ffff); /* for - (mw.w) */
						break;
				case 2:
						requestedVal = (UT_TEST_PATTERN_02 & 0x000000ff); /* for - (mw.b) */
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

			if ((testVal != requestedVal) || (testVal != testVal_A))
			{
				if (mvUartPrintEnable(0) != MV_OK)
				{
					mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
					return MV_FAIL;
				}
				mvOsPrintf("\nMemory Compare Test: At address 0x%x writen", (adrsIndex+0x100));
				mvOsPrintf(" value should be 0x%x but read value is 0x%x\n", requestedVal, testVal);
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
  
  mvOsPrintf("  cmp - Memory Compare command test ended successfully.\n");

  return MV_OK;

}

/*!**************************************************************************
*!
*$ FUNCTION: mvMemoryCompareValueTest
*!
*$ GENERAL DESCRIPTION: Test the u-boot command cmp - memory compare.
*!                      Compare the memory from address 'start_address'
*!                      to address 'end_address', with value 'value'
*!
*$ RETURNS:
*!
*$ ALGORITHM:
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:
*!
*!**************************************************************************
*!*/
MV_STATUS mvMemoryCompareValueTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
	MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
	MV_U32 endOffset   = startOffset + 0x20;  
	MV_U8  mem_command[25];
	register MV_U32 testVal;
	register MV_U32 adrsIndex;
	register MV_U32 requestedVal;

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

	mvOsPrintf("\n  cmpm - Memory Compare with 'value' command test started... ");
	mvOsPrintf("\n         start address is : 0x%x\n", startOffset);
  
	/* Set Compare buffers with data. */
	memset ((MV_U32 *)startOffset, UT_TEST_PATTERN_01, 0x20);
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }
  
	sprintf(mem_command, "cmpm %x %x %x", UT_TEST_PATTERN_01, startOffset, endOffset);  
	  
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	run_command(mem_command, 0);	
	
	/* cmpm return blanc if successfull, if 's_mvLoggerArr' is empty than the test fail. */
	if(strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cmpm, 6) == 0)
	{
	    /* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}					
		mvOsPrintf ("\n Run Compare command FAILED.\n");
        return MV_FAIL;
	}
    else
	{
	    /* mvOsPrintf ("\n Run Compare with value command ended successfully");	*/
		/* mvOsPrintf (", %d -\n", s_mvLoggerArrI); */
	}
	
	for (adrsIndex = startOffset; adrsIndex < endOffset; adrsIndex += 4)
	{	
		/* Read the written value */
		testVal = *(volatile MV_U32 *)(adrsIndex);

		requestedVal = UT_TEST_PATTERN_01;

		if (testVal != requestedVal)
		{
			/* Enable print to UART. */
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
			mvOsPrintf("\nMemory Compare Test: At address 0x%x writen", adrsIndex);
			mvOsPrintf(" value is 0x%x but read value is 0x%x\n", requestedVal, testVal);
			return MV_FAIL;
		}
	} // for()
 
	/* Disable print buffer update, Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  cmpm - Memory Compare with value command test ended successfully.\n");
	return MV_OK;
}

#if UBOOT_COMMANDS_MEM_TEST_DEBUG
/* **************************************************************************
 *
 *  FUNCTION: mvMemoryCpumapTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command cpumap - display CPU memory mapping settings.
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

volatile MV_STATUS mvMemoryCpumapTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/

  MV_U8  mem_command[25]; 
  MV_U8  *result = NULL, *result_01 = NULL, *result_02 = NULL, *result_03 = NULL;
  MV_U8  *result_04 = NULL, *result_05 = NULL, *result_06 = NULL, *result_07 = NULL;
      
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  cpumap - display CPU memory mapping settings command test started... \n");
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	       	   
	/* Set cpumap command */
	sprintf(mem_command, "cpumap");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run cpumap command. */
	run_command(mem_command, 0);	
					
	/* mvOsPrintf ("\n mem_command = %s, s_mvLoggerArr = %s.\n", mem_command, s_mvLoggerArr); */
	
	/*====== STEP 01 ======*/
	result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cpumap_01);
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cpumap_02);
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cpumap_03);
	result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cpumap_04);
	result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cpumap_05);	
	result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cpumap_06);
	result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cpumap_07);
	result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_cpumap_08);
		
	if((result == NULL)||(result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL) \
	    ||(result_05 == NULL)||(result_06 == NULL)||(result_07 == NULL)||(result_07 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		
		if(result == NULL)
			mvOsPrintf ("\n result = NULL.\n");
		if(result_01 == NULL)
			mvOsPrintf ("\n result_01 = NULL.\n");
		if(result_02 == NULL)
			mvOsPrintf ("\n result_02 = NULL.\n");			
		if(result_03 == NULL)
			mvOsPrintf ("\n result_03 = NULL.\n");
		if(result_04 == NULL)
			mvOsPrintf ("\n result_04 = NULL.\n");
		if(result_05 == NULL)
			mvOsPrintf ("\n result_05 = NULL.\n");
		if(result_06 == NULL)
			mvOsPrintf ("\n result_06 = NULL.\n");
		if(result_07 == NULL)
			mvOsPrintf ("\n result_07 = NULL.\n");
					
		mvOsPrintf ("\n Run cpumap (step 01) command FAILED.\n");
		return MV_FAIL;
	}
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  cpumap - display CPU memory mapping settings command test ended successfully.\n");
	return MV_OK;
}
#endif

/* **************************************************************************
* 
*  FUNCTION: mvMemoryCrc32Test
*
*  GENERAL DESCRIPTION: Test the u-boot command crc32 - checksum calculation.
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
MV_STATUS mvMemoryCrc32Test(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
  MV_U32 compareSize = 0x0;  
  MV_U8  mem_command[25];    

  register MV_U32 testVal = 0x0;
  register MV_U32 requestedVal = 0x0;
  
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  crc32 - checksum calculation command test started... ");
	mvOsPrintf("\n          start address is : 0x%x\n", startOffset);
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
    
	/* Clear the test buffers. */
	memset ((MV_U32 *)startOffset, 0x0, 0x20);
	memset ((MV_U32 *)(startOffset + 100), 0x0, 0x4);	
  
	/* Set Crc32 buffer with data. */
	if(mvBspMemoryFill (startOffset, UT_TEST_PATTERN_01, 0x20) != MV_OK)
	{
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		mvOsPrintf("\n %s - Memory Fill error.\n", __FUNCTION__);
	    return MV_FAIL;
	}   
		
	/* Set the command: crc32 address count [addr] - compute CRC32 checksum [save at addr] */
	sprintf(mem_command, "crc32 %x 20 %x", startOffset, (startOffset + 100));  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
			
	/* Set compare string */
	compareSize = sizeof(mvUT_compare_str_crc32);
	
	/* strncmp return blanc if successfull, if 's_mvLoggerArr' is empty than the test fail. */
	if(strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_crc32, (compareSize-1)) != 0)
	{
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		mvOsPrintf ("\n Run Compare command failed.");
		mvOsPrintf (", %d -\n", s_mvLoggerArrI);
		return MV_FAIL;
	}
		
	requestedVal = UT_CRC32_PATTERN_01;  /* for - (crc32) */
			
	/* Read the written value */
	testVal = *(volatile MV_U32 *)(startOffset + 100);
		
	if (testVal != requestedVal)
	{
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		mvOsPrintf("\nCrc32 Test: At address 0x%x writen", (startOffset + 100));
		mvOsPrintf(" value is 0x%x but read value is 0x%x\n", requestedVal, testVal);
		return MV_FAIL;
	}
  
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  crc32 - checksum calculation command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryDramregsTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command dramregs - show dramregs.
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

volatile MV_STATUS mvMemoryDramregsTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/

  MV_U8  mem_command[25]; 
  MV_U8  *result = NULL, *result_01 = NULL, *result_02 = NULL, *result_03 = NULL;
  MV_U8  *result_04 = NULL, *result_05 = NULL, *result_06 = NULL, *result_07 = NULL, *result_08 = NULL;
  MV_U8  *result_09 = NULL, *result_10 = NULL, *result_11 = NULL, *result_12 = NULL, *result_13 = NULL;
  MV_U8  *result_14 = NULL, *result_15 = NULL, *result_16 = NULL, *result_17 = NULL, *result_18 = NULL;
  MV_U8  *result_19 = NULL, *result_20 = NULL, *result_21 = NULL, *result_22 = NULL, *result_23 = NULL;
  MV_U8  *result_24 = NULL, *result_25 = NULL;
      
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  dramregs - show dramregs command test started... \n");
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	       	   
	/* Set dramregs command */
	sprintf(mem_command, "dramregs");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run dramregs command. */
	run_command(mem_command, 0);	
	
	/*====== STEP 01 ======*/
	result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_01);
	result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_02);
	result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_03);
	result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_04);
	result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_05);	
	result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_06);
	result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_07);
	result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_08);
	result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_09);
	
	result_09 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_10);
	result_10 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_11);
	result_11 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_12);
	result_12 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_13);
	result_13 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_14);	
	result_14 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_15);
	result_15 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_16);
	result_16 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_17);
	result_17 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_18);

	result_18 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_19);
	result_19 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_20);
	result_20 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_21);
	result_21 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_22);
	result_22 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_23);	
	result_23 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_24);
	result_24 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_25);
	result_25 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_dramregs_26);
	
	if((result == NULL)||(result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL) \
	    ||(result_05 == NULL)||(result_06 == NULL)||(result_07 == NULL)||(result_07 == NULL) \
		||(result_08 == NULL)||(result_09 == NULL)||(result_10 == NULL)||(result_11 == NULL) \
		||(result_12 == NULL)||(result_13 == NULL)||(result_13 == NULL)||(result_15 == NULL) \
		||(result_16 == NULL)||(result_17 == NULL)||(result_18 == NULL)||(result_19 == NULL) \
		||(result_20 == NULL)||(result_21 == NULL)||(result_22 == NULL)||(result_23 == NULL) \
		||(result_24 == NULL)||(result_25 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run dramregs (step 01) command FAILED.\n");
		return MV_FAIL;
	}
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  dramregs - show dramregs command test ended successfully.\n");

	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryFindValTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command fi - find value in memory.
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

MV_STATUS mvMemoryFindValTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
  MV_U32 endOffset   = startOffset + 0x80, ret_val = 0x0, compareSize = 0x0;  
  MV_U32  counter = 0; 
  MV_U8  mem_command[25], fiOffset = 0x0; 

  register MV_U32 adrsIndex;
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  fi - find value in memory command test started... ");
	mvOsPrintf("\n       start address is : 0x%x\n", startOffset);
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
  
	for (adrsIndex = startOffset, fiOffset = 0x0; adrsIndex < endOffset; adrsIndex += 0x20, fiOffset += 0x4)
	{  
		/* Clear fi buffer. */
		memset ((MV_U32 *)adrsIndex, 0x0, 0x20);
					
		/* Set fi buffer with data. */
		if(mvBspMemoryFill (adrsIndex, UT_TEST_PATTERN_02, 0x20) != MV_OK)
		{
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
			mvOsPrintf("\n %s - Memory Fill error (1).\n", __FUNCTION__);
			return MV_FAIL;
		}

		if(mvBspMemoryFill ((adrsIndex+fiOffset), UT_TEST_PATTERN_04, 4) != MV_OK)
		{
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
			mvOsPrintf("\n %s - Memory Fill error (2).\n", __FUNCTION__);
			mvOsPrintf(" address = 0x%08x, pattern = 0x%08x.\n", (adrsIndex+fiOffset), UT_TEST_PATTERN_04);
			return MV_FAIL;
		}   
      
		/* Set fi command */
		sprintf(mem_command, "fi %x %x %x", UT_TEST_PATTERN_04, adrsIndex, (adrsIndex+0x20));   
			
		/*=========================================*/
		s_mvLoggerArr[0] = '\0';
		s_mvLoggerArrI = 0x0;
	
		/* run fi command. */
		run_command(mem_command, 0);
									
		/* Run test for */
		sprintf(mvUT_compare_str_fi, "Value: %08x found at address: %x", UT_TEST_PATTERN_04, (adrsIndex+fiOffset));

		compareSize = sizeof(mvUT_compare_str_fi);
		/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
		ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_fi, (compareSize-1));   			  								
		if(ret_val != 0)
		{
			/* Enable print to UART. */
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
			
			if(strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_fi_01, (sizeof(mvUT_compare_str_fi_01)-1)) == 0x0)
			{
				mvOsPrintf ("\n To use this command enable enaMonExt. !!!\n");
			}
			else if(strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_fi_02, (sizeof(mvUT_compare_str_fi_02)-1)) == 0x0)
			{
				mvOsPrintf ("\n Value not found!!,\n Check the test buffer.\n");
			}
			else
			{					
				mvOsPrintf ("\n %d - ", ret_val);
				for(counter=0x0 ; counter <= (sizeof(mvUT_compare_str_fi_01)-1); counter++)
					mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			}
			
			mvOsPrintf ("\n Run fi command FAILED.\n");
			return MV_FAIL;
		}
	} // for()
 
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  fi - find value in memory command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
* 
*  FUNCTION: mvMemoryIcrc32Test
*
*  GENERAL DESCRIPTION: Test the u-boot command icrc32 - compute CRC32 checksum on devices at I2C channel.
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
MV_STATUS mvMemoryIcrc32Test(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/

  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
  MV_U8  *icrc32SaveBuffer = (MV_U8 *)startOffset;  
  MV_U32 compareSize = 0x0, compareVar_01 = 0x0;  
  MV_U32 nAddrsCounter = 0x0, counter = 0x0;
  MV_U8  mem_command[25], mem_command_01[25]; 
  MV_U8  ReadValue[4], ReadValue_01[4], tempValue = 0x0;  
  
  static volatile MV_U8  *result = NULL;
  
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

	mvOsPrintf("\n  icrc32 - checksum calculation command test started... ");
	mvOsPrintf("\n           start address is : 0x%x\n", startOffset);
	  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
    
	/* Clear the Icrc32 test buffer. */
	memset ((MV_U32 *)startOffset, 0x0, 0x40);

	/*====== STEP 01 ======*/	
	/* Check if I2C eeprom (address 0x50) is installed. */
	sprintf(mem_command, "iprobe");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);	
	
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_crc32_01);			  								
	if(result == NULL)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		for(counter=0x0 ; counter <= 34; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run iprobe command for I2C EEPROM (address 0x50) - FAILED.\n", __FUNCTION__);
		mvOsPrintf (" iprobe command needs I2C EEPROM installed.\n");
		return MV_FAIL;
	}	
  
	for(nAddrsCounter = 0x0 ; nAddrsCounter < 0x40 ; nAddrsCounter++)
	{
		/*====== STEP 02 ======*/
		/* Read one Byte from I2C eeprom (address 0x50) and save it to 'icrc32SaveBuffer'. */
		sprintf(mem_command_01, "imd 0 50 %x.2 1", (0x10 + nAddrsCounter));  
	
		/*=========================================*/
		s_mvLoggerArr[0] = '\0';
		s_mvLoggerArrI = 0x0;	

		/* write to the memory address. */
		run_command(mem_command_01, 0);
		
		sprintf(mem_command, "%04x: ", (0x10 + nAddrsCounter)); 
		result = strstr((MV_U8 *)s_mvLoggerArr, mem_command); 			  								
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
			
			mvOsPrintf ("\n Run strstr() command FAILED, nAddrsCounter = 0x%x, mem_command = %s.\n", nAddrsCounter, mem_command);
			return MV_FAIL;
		}
		
		/* Enable print to UART. */
		/* if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		} */
		
		ReadValue[0] = '\0';
		strncpy (ReadValue, (const char *)(result+6), 2);
		ReadValue[3] = '\0';
		/* mvOsPrintf ("%s ", ReadValue); */
			
		tempValue = (MV_U8)my_atoi(ReadValue); 
			/* mvOsPrintf ("%02x ", tempValue); */
			
		*(MV_U8 *)icrc32SaveBuffer = tempValue;
			
		/* Disable print to UART. */
		if (mvUartPrintDisable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
			return MV_FAIL;
		}
		
		icrc32SaveBuffer++;		
		
		/*====== STEP 03 ======*/
		/* Write one Byte to I2C eeprom (address 0x50). */
		sprintf(mem_command, "imw 0 50 %x.2 %x", (0x10 + nAddrsCounter), icrc32Array[nAddrsCounter]);  
	
		/* write to the memory address. */
		run_command(mem_command, 0);		
		
		/*====== STEP 04 ======*/
		/* Read one Byte from I2C eeprom to verify the write. */
		sprintf(mem_command_01, "imd 0 50 %x.2 1", (0x10 + nAddrsCounter));  
	
		/*=========================================*/
		s_mvLoggerArr[0] = '\0';
		s_mvLoggerArrI = 0x0;	

		/* write to the memory address. */
		run_command(mem_command_01, 0);
 
		sprintf(mem_command, "%04x: %02x", (0x10 + nAddrsCounter), icrc32Array[nAddrsCounter]); 
		result = strstr((MV_U8 *)s_mvLoggerArr, mem_command); 			  								
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
			
			mvOsPrintf ("\n Run imd command FAILED, nAddrsCounter = 0x%x, mem_command_01 = %s, mem_command = %s.\n", nAddrsCounter, mem_command_01, mem_command);
			return MV_FAIL;
		}
			
		ReadValue_01[0] = '\0';
		strncpy (ReadValue_01, (const char *)(result+6), 2);
		ReadValue_01[3] = '\0';
		
		compareVar_01 = my_atoi(ReadValue_01); 
				
		if((icrc32Array[nAddrsCounter] != compareVar_01))
		{
			/* Enable print to UART. */
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
					
			mvOsPrintf ("\n Comare Read Var to Write FAILED.\n");
			return MV_FAIL;
		}				
	} /* for(...) */  
			
	/*====== STEP 05 ======*/
	/* Set the command: icrc32 I2C_channel chip address[.0, .1, .2] count */
	sprintf(mem_command, "icrc32 0 50 10.2 40");  
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;			
	
	/* run the memory dispaly command. */
	run_command(mem_command, 0);
			
	/* Set compare string */
	compareSize = sprintf(mvUT_compare_str_icrc32, "CRC32 for 00000010 ... 0000004f ==> %08x", UT_ICRC32_PATTERN_02);	
	
	/* strncmp return blanc if successfull, if 's_mvLoggerArr' is empty than the test fail. */
	if(strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_icrc32, (compareSize-1)) != 0)
	{
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		mvOsPrintf ("\n Run Compare command failed.");
		mvOsPrintf ("\n %s - mvUT_compare_str_icrc32 = %s, compareSize = %d.\n", __FUNCTION__, mvUT_compare_str_icrc32, compareSize);		
		mvOsPrintf (", %d -\n", s_mvLoggerArrI);
		return MV_FAIL;
	}		
		
	/* Set pointer to the beginning of the buffer. */
	icrc32SaveBuffer = (MV_U8 *)startOffset; 
		
	for(nAddrsCounter = 0x0 ; nAddrsCounter < 0x40 ; nAddrsCounter++)
	{
		/*====== STEP 06 ======*/
		/* Write back the original Byte saved in 'icrc32SaveBuffer' to I2C eeprom (address 0x50). */
		sprintf(mem_command, "imw 0 50 %x.2 %x", (0x10 + nAddrsCounter), *(MV_U8 *)icrc32SaveBuffer);  
			
		/* write to the memory address. */
		run_command(mem_command, 0);		
		
		/*====== STEP 07 ======*/
		/* Read one Byte from I2C eeprom to verify the write. */
		sprintf(mem_command_01, "imd 0 50 %x.2 1", (0x10 + nAddrsCounter));  
	
		/*=========================================*/
		s_mvLoggerArr[0] = '\0';
		s_mvLoggerArrI = 0x0;	

		/* write to the memory address. */
		run_command(mem_command_01, 0);
 
		sprintf(mem_command, "%04x: ", (0x10 + nAddrsCounter)); 
		result = strstr((MV_U8 *)s_mvLoggerArr, mem_command); 			  								
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
			
			mvOsPrintf ("\n Run imd command FAILED, nAddrsCounter = 0x%x, mem_command_01 = %s, mem_command = %s.\n", nAddrsCounter, mem_command_01, mem_command);
			return MV_FAIL;
		}
		
		/* Enable print to UART. */
		/* if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		} */
		
		ReadValue_01[0] = '\0';
		strncpy (ReadValue_01, (const char *)(result+6), 2); 
		ReadValue_01[3] = '\0';
		/* mvOsPrintf ("%s ", ReadValue_01); */
		
		compareVar_01 = my_atoi(ReadValue_01); 
		
		/* Disable print to UART. */
		if (mvUartPrintDisable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
			return MV_FAIL;
		}		
		
		if(((*(MV_U8 *)icrc32SaveBuffer) != compareVar_01))
		{
			/* Enable print to UART. */
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
					
			mvOsPrintf ("\n Comare Read Var to saved original value FAILED.\n");
			mvOsPrintf("\nCrc32 Test: At address 0x%x writen", icrc32SaveBuffer);
			mvOsPrintf(" value should have been 0x%x but read value is 0x%x\n", (*(MV_U8 *)icrc32SaveBuffer), compareVar_01);			
			return MV_FAIL;
		}		
		icrc32SaveBuffer++;
	}
		  
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  icrc32 - checksum calculation command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryImdTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command imd - i2c memory display.
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

MV_STATUS mvMemoryImdTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 ret_val = 0x0, compareSize = 0x0;  
  MV_U32  counter = 0; 
  MV_U8  mem_command[25]; 
  static volatile MV_U8  *result = NULL, *result_01 = NULL, *result_02 = NULL, *result_03 = NULL;
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  imd - i2c memory display command test started... \n");
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	
	/*====== STEP 01 ======*/	
	sprintf(mem_command, "iprobe");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);	
	
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_imd_01);			  								
	if(result == NULL)
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
			
		mvOsPrintf ("\n %s - Run iprobe command for I2C EEPROM (address 0x50) - FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}	
  
	sprintf(mem_command, "imd 0 50 10.2 40");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	result     = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_imd_02); 
	result_01  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_imd_03); 
	result_02  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_imd_04); 
	result_03  = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_imd_05); 	
			  								
	if((result == NULL)||(result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL))
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
			
		mvOsPrintf ("\n Run imd command FAILED.\n");
		return MV_FAIL;
	}
 
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  imd - i2c memory display command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryImwTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command imw - i2c memory write.
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

MV_STATUS mvMemoryImwTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 counter = 0x0, nAddrsCounter = 0x0; 
  MV_U8  mem_command[25], mem_command_01[25]; 
  MV_U8  ReadValue[4], ReadValue_01[4];
  MV_U8  compareVar = 0x0, compareVar_01 = 0x0;
  static volatile MV_U8  *result = NULL;  
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  imw - i2c memory write command test started... \n");
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	
	/*====== STEP 01 ======*/	
	/* Check if I2C eeprom (address 0x50) is installed. */
	sprintf(mem_command, "iprobe");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);	
	
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_imw_01);			  								
	if(result == NULL)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		for(counter=0x0 ; counter <= 34; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n %s - Run iprobe command for I2C EEPROM (address 0x50) - FAILED.\n", __FUNCTION__);
		mvOsPrintf (" iprobe command needs I2C EEPROM installed.\n");
		return MV_FAIL;
	}	
  
	for(nAddrsCounter = 0x0 ; nAddrsCounter < 0x40 ; nAddrsCounter++)
	{
		/*====== STEP 02 ======*/
		/* Read one Byte from I2C eeprom (address 0x50) and save it to 'compareVar'. */
		sprintf(mem_command_01, "imd 0 50 %x.2 1", (0x10 + nAddrsCounter));  
	
		/*=========================================*/
		s_mvLoggerArr[0] = '\0';
		s_mvLoggerArrI = 0x0;	

		/* write to the memory address. */
		run_command(mem_command_01, 0);
 
		sprintf(mem_command, "%04x: ", (0x10 + nAddrsCounter)); 
		result = strstr((MV_U8 *)s_mvLoggerArr, mem_command); 			  								
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
			
			mvOsPrintf ("\n Run imd command FAILED, nAddrsCounter = 0x%x, mem_command_01 = %s, mem_command = %s.\n", nAddrsCounter, mem_command_01, mem_command);
			return MV_FAIL;
		}
		
		/* Enable print to UART. */
		/* if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		} */
		
		ReadValue[0] = '\0';
		strncpy (ReadValue, (const char *)(result+6), 2);
		ReadValue[3] = '\0';
		/* mvOsPrintf ("%s ", ReadValue); */
		
		compareVar = my_atoi(ReadValue); 
		/* mvOsPrintf ("%02x ", compareVar); */
		
		/* Disable print to UART. */
		if (mvUartPrintDisable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
			return MV_FAIL;
		}
		
		/*====== STEP 03 ======*/
		/* Write one Byte to I2C eeprom (address 0x50). */
		sprintf(mem_command, "imw 0 50 %x.2 %x", (0x10 + nAddrsCounter), nAddrsCounter);  
	
		/* write to the memory address. */
		run_command(mem_command, 0);		
		
		/*====== STEP 04 ======*/
		/* Read one Byte from I2C eeprom to verify the write. */
		sprintf(mem_command_01, "imd 0 50 %x.2 1", (0x10 + nAddrsCounter));  
	
		/*=========================================*/
		s_mvLoggerArr[0] = '\0';
		s_mvLoggerArrI = 0x0;	

		/* write to the memory address. */
		run_command(mem_command_01, 0);
 
		sprintf(mem_command, "%04x: ", (0x10 + nAddrsCounter)); 
		result = strstr((MV_U8 *)s_mvLoggerArr, mem_command); 			  								
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
			
			mvOsPrintf ("\n Run imd command FAILED, nAddrsCounter = 0x%x, mem_command_01 = %s, mem_command = %s.\n", nAddrsCounter, mem_command_01, mem_command);
			return MV_FAIL;
		}
		
		/* Enable print to UART. */
		/* if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		} */
		
		ReadValue_01[0] = '\0';
		strncpy (ReadValue_01, (const char *)(result+6), 2);
		ReadValue_01[3] = '\0';
		/* mvOsPrintf ("%s ", ReadValue_01); */
		
		compareVar_01 = my_atoi(ReadValue_01); 
		
		/* mvOsPrintf ("%02x ", compareVar_01);
		   if(!(nAddrsCounter % 0x10))
			   mvOsPrintf ("\n"); */
			
		/* Disable print to UART. */
		if (mvUartPrintDisable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
			return MV_FAIL;
		}			
		
		if((nAddrsCounter != compareVar_01))
		{
			/* Enable print to UART. */
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
					
			mvOsPrintf ("\n Comare Read Var to Write FAILED.\n");
			return MV_FAIL;
		}		
		
		/*====== STEP 05 ======*/
		/* Write back the original Byte saved in 'compareVar' to I2C eeprom (address 0x50). */
		sprintf(mem_command, "imw 0 50 %x.2 %x", (0x10 + nAddrsCounter), compareVar);  
	
		/* write to the memory address. */
		run_command(mem_command, 0);		
		
		/*====== STEP 06 ======*/
		/* Read one Byte from I2C eeprom to verify the write. */
		sprintf(mem_command_01, "imd 0 50 %x.2 1", (0x10 + nAddrsCounter));  
	
		/*=========================================*/
		s_mvLoggerArr[0] = '\0';
		s_mvLoggerArrI = 0x0;	

		/* write to the memory address. */
		run_command(mem_command_01, 0);
 
		sprintf(mem_command, "%04x: ", (0x10 + nAddrsCounter)); 
		result = strstr((MV_U8 *)s_mvLoggerArr, mem_command); 			  								
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
			
			mvOsPrintf ("\n Run imd command FAILED, nAddrsCounter = 0x%x, mem_command_01 = %s, mem_command = %s.\n", nAddrsCounter, mem_command_01, mem_command);
			return MV_FAIL;
		}
		
		/* Enable print to UART. */
		/* if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		} */
		
		ReadValue_01[0] = '\0';
		strncpy (ReadValue_01, (const char *)(result+6), 2); 
		ReadValue_01[3] = '\0';
		/* mvOsPrintf ("%s ", ReadValue_01); */
		
		compareVar_01 = my_atoi(ReadValue_01); 
		
		/* Disable print to UART. */
		if (mvUartPrintDisable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
			return MV_FAIL;
		}		
		
		if((compareVar != compareVar_01))
		{
			/* Enable print to UART. */
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
					
			mvOsPrintf ("\n Comare Read Var to saved original value FAILED.\n");
			return MV_FAIL;
		}		

	} /* for(...) */
	
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  imw - i2c memory write command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryIprobeTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command iprobe - discover valid I2C chip addresses.
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

MV_STATUS mvMemoryIprobeTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 ret_val = 0x0, compareSize = 0x0;  
  MV_U32  counter = 0; 
  MV_U8  mem_command[25]; 
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  iprobe - discover valid I2C chip addresses command test started... \n");
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
  
	sprintf(mem_command, "iprobe");  
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;	

	/* write to the memory address. */
	run_command(mem_command, 0);
 
	compareSize = sizeof(mvUT_compare_str_iprobe);
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_iprobe, (compareSize-1));   			  								
	if(ret_val != 0x0)
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
			
		mvOsPrintf ("\n Run iprobe command FAILED, compareSize = %d\n", compareSize);
		return MV_FAIL;
	}
 
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  iprobe - discover valid I2C chip addresses command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryBaseTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command base - print or set address offset for memory commands.
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

MV_STATUS mvMemoryBaseTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
  MV_U32 ret_val = 0x0, compareSize = 0x0;  
  MV_U32 counter = 0; 
  MV_U8  mem_command[25]; 
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  base - print or set address offset command test started... ");
	mvOsPrintf("\n         start address is : 0x%x\n", startOffset);
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
       
	/*====== STEP 01 ======*/
	/* Set base command */
	sprintf(mem_command, "base %x", startOffset);   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run base command. */
	run_command(mem_command, 0);
									
	/* Run test for */
	sprintf(mvUT_compare_str_base, "Base Address: 0x%08x", startOffset);

	compareSize = sizeof(mvUT_compare_str_base);
	/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_base, (compareSize-1));   			  								
	if(ret_val != 0)
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
			
		mvOsPrintf ("\n Run base command FAILED.\n");
		return MV_FAIL;
	}
 
 	/*====== STEP 02 ======*/
	/* Set base command */
	sprintf(mem_command, "base");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run base command. */
	run_command(mem_command, 0);
									
	/* Run test for */
	sprintf(mvUT_compare_str_base, "Base Address: 0x%08x", startOffset);

	compareSize = sizeof(mvUT_compare_str_base);
	/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_base, (compareSize-1));   			  								
	if(ret_val != 0)
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
			
		mvOsPrintf ("\n Run base command FAILED.\n");
		return MV_FAIL;
	}
	
	/*====== STEP 03 ======*/
	/* Set base command back to it's default value. */
	sprintf(mem_command, "base %x", 0x00000000);   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run base command. */
	run_command(mem_command, 0);
									
	/* Run test for */
	sprintf(mvUT_compare_str_base, "Base Address: 0x%08x", 0x00000000);

	compareSize = sizeof(mvUT_compare_str_base);
	/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_base, (compareSize-1));   			  								
	if(ret_val != 0)
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
			
		mvOsPrintf ("\n Run base command FAILED.\n");
		return MV_FAIL;
	}	
 
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  base - print or set address offset command test ended successfully.\n");
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryEraseFlashTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command erase - erase FLASH memory.
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

MV_STATUS mvMemoryEraseFlashTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
  MV_U32 ret_val = 0x0, compareSize = 0x0;  
  MV_U32 writeBufferSize = 0x100; 
  MV_U32 counter = 0; 
  MV_U8  mem_command[25]; 
  MV_U8  *result = NULL;
  MV_U8  BoardType = 0x0;  
  
  register MV_U32 testVal = 0x0;
  register MV_U32 adrsIndex = startOffset;  
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  erase - erase FLASH memory command test started... ");
	mvOsPrintf("\n          start address is : 0x%x\n", startOffset);
	
	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();
  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	
	/* Clear erase buffer. */
	memset ((MV_U32 *)adrsIndex, 0x0, writeBufferSize);
					
	/* Set erase buffer with data. */
	if(mvBspMemoryFill (adrsIndex, UT_TEST_PATTERN_02, writeBufferSize) != MV_OK)
	{
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		mvOsPrintf("\n %s - Memory Fill error (1).\n", __FUNCTION__);
		return MV_FAIL;
	}	
       	   
	/*====== STEP 01 ======*/
	/* Set protect command */
	sprintf(mem_command, "protect off all");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run protect command. */
	run_command(mem_command, 0);	
	
	/* Run test for */
	sprintf(mvUT_compare_str_protect, "Un-Protect Flash Bank # 1");

	compareSize = sizeof(mvUT_compare_str_protect);
	/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect, (compareSize-1));
	if(ret_val != 0) /*Try different number of sectors*/
	  ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_02, (compareSize-1));
	if(ret_val != 0) /*Try different number of sectors*/
	  ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_C_02, (compareSize-1));
	
	if(ret_val != 0)
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
			
		mvOsPrintf ("\n Run protect off all command FAILED.\n");
		return MV_FAIL;
	}

	/* Read the value in UT_FLASH_ERASE_START_ADRS */
	testVal = *(volatile MV_U32 *)(UT_FLASH_ERASE_START_ADRS);
	
	if(0xFFFFFFFF == testVal)
	{
			/* Set write to FLASH command. */
			sprintf(mem_command, "sflash write %08x %08x %x", adrsIndex, (UT_FLASH_ERASE_START_ADRS - DEVICE_SPI_BASE), writeBufferSize);
				
			/* run write to FLASH command. */
			run_command(mem_command, 0);			
			
			/* Read the value in UT_FLASH_ERASE_START_ADRS */
			testVal = *(volatile MV_U32 *)(UT_FLASH_ERASE_START_ADRS);
	
			if(0xFFFFFFFF == testVal)
			{			
				/* Enable print to UART. */
				if (mvUartPrintEnable(0) != MV_OK)
				{
					mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
					return MV_FAIL;
				}
					
				mvOsPrintf ("\n %s - Run write to FLASH command FAILED.\n", __FUNCTION__);
				mvOsPrintf ("   - mem_command is: %s.\n", mem_command);
				return MV_FAIL;
			}
	}	
	
 	/*====== STEP 02 ======*/
	/* Set erase command */
	sprintf(mem_command, "erase %08x %08x", UT_FLASH_ERASE_START_ADRS, UT_FLASH_ERASE_END_ADRS);   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run erase command. */
	run_command(mem_command, 0);
									
	compareSize = sizeof(mvUT_compare_str_protect_02);
	/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
		
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		result  = strstr ((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_02);
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_E_02);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_B_02);	
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_A_02);	
	}
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_C_02);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_D_02);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_F_02);	
	}	
	else if(BoardType == MV_INIT_ERROR)	 
			return MV_FAIL;			
					
	if(result == NULL)
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
			
		mvOsPrintf ("\n Run erase (01) command FAILED.\n");
		return MV_FAIL;
	}
			
	/*====== STEP 03 ======*/ 
	/* Set protect command */
	sprintf(mem_command, "protect on all");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run protect command. */
	run_command(mem_command, 0);

	/*====== STEP 04 ======*/ 	
	/* Read the value in UT_FLASH_ERASE_START_ADRS */
	testVal = *(volatile MV_U32 *)(UT_FLASH_ERASE_START_ADRS);
	
	if(0xFFFFFFFF != testVal)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n %s - erase FLASH FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}		
 
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  erase - erase FLASH memory command test ended successfully. (BoardType = %d)\n", BoardType);
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryFlinfoFlashTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command flinfo - print information for all FLASH memory banks.
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

volatile MV_STATUS mvMemoryFlinfoFlashTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/

  MV_U8  mem_command[25]; 
  MV_U8  BoardType = 0x0;  
  MV_U8  *result = NULL, *result_01 = NULL, *result_02 = NULL, *result_03 = NULL;
  MV_U8  *result_04 = NULL, *result_05 = NULL, *result_06 = NULL, *result_07 = NULL, *result_08 = NULL;  
       
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  flinfo - print info for SPI FLASH command test started... \n");

#if UBOOT_COMMANDS_MEM_TEST_DEBUG
	sflash.baseAddr         = mvCpuIfTargetWinBaseLowGet(SPI_CS);
	sflash.sectorNumber     = flash_info[FLASH_BANK-1].sector_count;
	sflash.sectorSize       = flash_info[FLASH_BANK-1].size/flash_info[FLASH_BANK-1].sector_count;

    mvOsPrintf("Base: 0x%08X\nSectors: 0x%08X\nSector Size: 0x%08X\nFlashSize: 0x%08X\n",
			sflash.baseAddr,sflash.sectorNumber,sflash.sectorSize,sflash.sectorNumber*sflash.sectorSize);  
#endif	
	
	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();
			  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	       	   
	/* Set sflash command */
	sprintf(mem_command, "flinfo");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run sflash command. */
	run_command(mem_command, 0);	
	
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{
		/*====== STEP 01 ======*/
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_07);
		result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_08);
		result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_09);
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_07);
		result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_08);
		result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_E_09);
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_07);
		result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_08);
		result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_09);	
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		/*====== STEP 01 ======*/
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_07);
		result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_08);
		result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_09);
	}
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_07);
		result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_08);
		result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_09);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_07);
		result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_08);
		result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_09);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_07);
		result_07 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_08);
		result_08 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_09);
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	
								
	if((result == NULL)||(result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL) \
	    ||(result_05 == NULL)||(result_06 == NULL)||(result_07 == NULL)||(result_07 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run flinfo (step 01) command FAILED.\n");
		return MV_FAIL;
	}
	
	/*====== STEP 02 ======*/	
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_10);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_11);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_12);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_13);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_14);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_15);
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_10);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_11);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_12);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_13);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_14);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_15);
	}		
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_10);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_11);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_12);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_13);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_14);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_B_15);	
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_10);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_11);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_12);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_13);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_14);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_A_15);	
	}
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_10);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_11);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_12);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_13);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_14);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_C_15);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_10);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_11);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_12);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_13);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_14);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_D_15);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_10);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_11);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_12);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_13);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_14);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_flinfo_F_15);
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	
					
	if((result == NULL)||(result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL) \
	    ||(result_05 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run flinfo (step 02) command FAILED.\n");
		return MV_FAIL;
	}	
 
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  flinfo - print info for SPI FLASH command test ended successfully. (BoardType = %d)\n", BoardType);
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryProtectTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command protect - enable or disable FLASH write protection.
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

MV_STATUS mvMemoryProtectTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 ret_val = 0x0, compareSize = 0x0;  
  MV_U32 counter = 0; 
  MV_U8  mem_command[25]; 
  MV_U8  *result = NULL;  
  MV_U8  BoardType = 0x0;  
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  protect - enable or disable FLASH write protection command test started... \n");
	
	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();
		  
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
       
	/*====== STEP 01 ======*/
	/* Set protect command */
	sprintf(mem_command, "protect on all");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run protect command. */
	run_command(mem_command, 0);
									
	/* Run test for */
	sprintf(mvUT_compare_str_protect, "Protect Flash Bank # 1");

	compareSize = sizeof(mvUT_compare_str_protect);
	/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect, (compareSize-1));   			  								
	if(ret_val != 0)
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
			
		mvOsPrintf ("\n Run protect on all command FAILED.\n");
		return MV_FAIL;
	}

 	/*====== STEP 02 ======*/
	/* Set erase command */
	sprintf(mem_command, "erase %08x %08x", UT_FLASH_ERASE_START_ADRS, UT_FLASH_ERASE_END_ADRS);   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run erase command. */
	run_command(mem_command, 0);
									
	compareSize = sizeof(mvUT_compare_str_protect_01);
	/* mvOsPrintf("\n compareSize = %d\n", compareSize); */

	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_01);			  								
	if(result == NULL)
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
			
		mvOsPrintf ("\n Run erase (01) command FAILED.\n");
		return MV_FAIL;
	}
	
	/*====== STEP 03 ======*/
	/* Set protect command */
	sprintf(mem_command, "protect off all");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run protect command. */
	run_command(mem_command, 0);	
	
	/* Run test for */
	sprintf(mvUT_compare_str_protect, "Un-Protect Flash Bank # 1");

	compareSize = sizeof(mvUT_compare_str_protect);
	/* mvOsPrintf("\n compareSize = %d\n", compareSize); */
	ret_val = strncmp((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect, (compareSize-1));   			  								
	if(ret_val != 0)
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
			
		mvOsPrintf ("\n Run protect off all command FAILED.\n");
		return MV_FAIL;
	}	
	
	/*====== STEP 04 ======*/
	/* Set erase command again. */
	sprintf(mem_command, "erase %08x %08x", UT_FLASH_ERASE_START_ADRS, UT_FLASH_ERASE_END_ADRS);   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run erase command. */
	run_command(mem_command, 0);
									
	compareSize = sizeof(mvUT_compare_str_protect_02);
	/* mvOsPrintf("\n compareSize = %d, s_mvLoggerArr = 0x%08x\n", compareSize, &s_mvLoggerArr); */

	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_02);
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_E_02);	
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_B_02);		
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_A_02);	
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_C_02);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_D_02);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_protect_F_02);	
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;
											
	if(result == NULL)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		mvOsPrintf ("\n %s - ", result);
		for(counter=0x0 ; counter <= compareSize; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n Run erase (02) command FAILED.\n");
		return MV_FAIL;
	}	
	
	/*====== STEP 05 ======*/ 
	/* Set protect command */
	sprintf(mem_command, "protect on all");   
			
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run protect command. */
	run_command(mem_command, 0);	
 
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  protect - enable or disable FLASH write protection command test ended successfully. (BoardType = %d)\n", BoardType);
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemorySFlashTest
 *
 *  GENERAL DESCRIPTION: Test the u-boot command sflash - read/write or erase SPI FLASH.
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

volatile MV_STATUS mvMemorySFlashTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 startOffset = (MV_U32)mvUTMemAlloc_PTR;
  MV_U32 ret_val = 0x0;  
  MV_U32 writeBufferSize = 0x100; 
  MV_U32 counter = 0; 
  MV_U8  mem_command[25]; 
  MV_U8  *result = NULL, *result_01 = NULL, *result_02 = NULL, *result_03 = NULL;
  MV_U8  *result_04 = NULL, *result_05 = NULL, *result_06 = NULL;    
  MV_U8  BoardType = 0x0;  
  
  register MV_U32 testVal = 0x0;
  register MV_U32 adrsIndex = startOffset;  
    
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

	mvOsPrintf("\n  sflash - read/write or erase SPI FLASH command test started... ");
	mvOsPrintf("\n           start address is : 0x%x\n", startOffset);
	
	/* Get boardType. */
	BoardType = diagIfGetMarvellBoardType();
		 
	/* Disable print to UART. */
	if (mvUartPrintDisable(0) != MV_OK)
    {
        mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
        return MV_FAIL;
    }  
	       	   
	/*====== STEP 01 ======*/
	/* Set sflash command */
	sprintf(mem_command, "sflash info");   
	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run sflash command. */
	run_command(mem_command, 0);	

	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_07);
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_E_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_E_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_E_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_E_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_E_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_E_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_E_07);		
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_B_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_B_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_B_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_B_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_B_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_B_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_B_07);		
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_A_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_A_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_A_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_A_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_A_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_A_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_A_07);
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_C_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_C_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_C_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_C_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_C_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_C_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_C_07);	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_D_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_D_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_D_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_D_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_D_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_D_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_D_07);	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		result    = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_F_01);
		result_01 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_F_02);
		result_02 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_F_03);
		result_03 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_F_04);
		result_04 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_F_05);	
		result_05 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_F_06);
		result_06 = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_F_07);
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	
							
	if((result == NULL)||(result_01 == NULL)||(result_02 == NULL)||(result_03 == NULL)||(result_04 == NULL) \
	    ||(result_05 == NULL)||(result_06 == NULL))
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n Run sflash info command FAILED.\n");
		return MV_FAIL;
	}

 	/*====== STEP 02 ======*/
	/* Set sflash command */
	sprintf(mem_command, "sflash protect off");  

	/* run sflash command. */
	run_command(mem_command, 0);			
	
	/* Set sflash command */
	sprintf(mem_command, "sflash info"); 	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run sflash command. */
	run_command(mem_command, 0);
									
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_08);			  								
	if(result == NULL)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 24; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n Run sflash protect off command FAILED.\n");
		return MV_FAIL;
	}
				
	/*====== STEP 03 ======*/ 	
	
	/* Clear erase buffer. */
	memset ((MV_U32 *)adrsIndex, 0x0, writeBufferSize);
					
	/* Set erase buffer with data. */
	if(mvBspMemoryFill (adrsIndex, UT_TEST_PATTERN_02, writeBufferSize) != MV_OK)
	{
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		mvOsPrintf("\n %s - Memory Fill error (1).\n", __FUNCTION__);
		return MV_FAIL;
	}	
	
	/* Read the value in UT_FLASH_ERASE_START_ADRS */
	testVal = *(volatile MV_U32 *)(UT_FLASH_ERASE_START_ADRS);
	
	if(0xFFFFFFFF == testVal)
	{
		/* Set write to FLASH command. */
		sprintf(mem_command, "sflash write %08x %08x %x", adrsIndex, (UT_FLASH_ERASE_START_ADRS - DEVICE_SPI_BASE), writeBufferSize);
				
		/* run write to FLASH command. */
		run_command(mem_command, 0);			
			
		/* Read the value in UT_FLASH_ERASE_START_ADRS */
		testVal = *(volatile MV_U32 *)(UT_FLASH_ERASE_START_ADRS);
	
		if(0xFFFFFFFF == testVal)
		{			
			/* Enable print to UART. */
			if (mvUartPrintEnable(0) != MV_OK)
			{
				mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
				return MV_FAIL;
			}
					
			mvOsPrintf ("\n %s - Run write to FLASH command FAILED.\n", __FUNCTION__);
			mvOsPrintf ("   - mem_command is: %s.\n", mem_command);
			return MV_FAIL;
		}
	}	
	
 	/*====== STEP 03b ======*/
	/* Set sflash command */
	sprintf(mem_command, "sflash protect off");  

	/* run sflash command. */
	run_command(mem_command, 0);	
	
 	/*====== STEP 04 ======*/
	if((BoardType == xCat2_DB_24GE_Rev_0)||(BoardType == xCat2_DB_24GE_Rev_1)||
	   (BoardType == xCat2_DB_24GE_Rev_2)||(BoardType == xCat2_DB_24GE_Rev_3)) /* BT -> 9,10,11,12 */
	{	
		/* Set erase command (for sector 11) */
		sprintf(mem_command, "sflash erase 11-12");
	}
	else if((BoardType == xCat_DB_24GE_Rev_0)||(BoardType == xCat_DB_24GE_Rev_1)||
	        (BoardType == xCat_DB_24GE_Rev_2)||(BoardType == xCat_DB_24GE_Rev_3)) /* BT -> 1,2,3,4 */
	{
		/* Set erase command (for sector 44,45) */
		sprintf(mem_command, "sflash erase 44-45");
	}	
	else if((BoardType == xCat_DB_48FE_Rev_0)||(BoardType == xCat_DB_48FE_Rev_1)||
	        (BoardType == xCat_DB_48FE_Rev_2)||(BoardType == xCat_DB_48FE_Rev_3)) /* BT -> 25,26,27,28 */
	{
		/* Set erase command (for sector 44,45) */
		sprintf(mem_command, "sflash erase 44-45");		
	}	
	else if((BoardType == xCat2_DB_48FE_Rev_0)||(BoardType == xCat2_DB_48FE_Rev_1)||
	        (BoardType == xCat2_DB_48FE_Rev_2)||(BoardType == xCat2_DB_48FE_Rev_3)) /* BT -> 33,34,35,36 */
	{
		/* Set erase command (for sector 44,45) */
		sprintf(mem_command, "sflash erase 44-45");
	}	
	else if((BoardType == xCat_DB_48GE_Rev_0)||(BoardType == xCat_DB_48GE_Rev_1)||
	        (BoardType == xCat_DB_48GE_Rev_2)||(BoardType == xCat_DB_48GE_Rev_3)) /* BT -> 21,22,23,24 */
	{
		/* Set erase command (for sector 44,45) */
		sprintf(mem_command, "sflash erase 44-45");	
	}	
	else if((BoardType == xCat2_DB_48GE_Rev_0)||(BoardType == xCat2_DB_48GE_Rev_1)||
	        (BoardType == xCat2_DB_48GE_Rev_2)||(BoardType == xCat2_DB_48GE_Rev_3)) /* BT -> 29,30,31,32 */
	{
		/* Set erase command (for sector 44,45) */
		sprintf(mem_command, "sflash erase 44-45");	
	}	
	else if((BoardType == xCat_DB_24FE_Rev_0)||(BoardType == xCat_DB_24FE_Rev_1)||
	        (BoardType == xCat_DB_24FE_Rev_2)||(BoardType == xCat_DB_24FE_Rev_3)) /* BT -> 5,6,7,8 */
	{
		/* Set erase command (for sector 44,45) */
		sprintf(mem_command, "sflash erase 44-45");
	}	
	else if(BoardType == MV_INIT_ERROR)	
			return MV_FAIL;	
																
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run erase command. */
	run_command(mem_command, 0);
									
	/* Read the value in UT_FLASH_ERASE_START_ADRS */
	testVal = *(volatile MV_U32 *)(UT_FLASH_ERASE_START_ADRS);
	
	if(0xFFFFFFFF != testVal)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n %s - Run sflash erase command FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}	
	
	/*====== STEP 05 ======*/ 	
	/* Set write to FLASH command. */
	sprintf(mem_command, "sflash write %08x %08x %x", adrsIndex, (UT_FLASH_ERASE_START_ADRS - DEVICE_SPI_BASE), writeBufferSize);
				
	/* run write to FLASH command. */
	run_command(mem_command, 0);			
			
	/* Read the value in UT_FLASH_ERASE_START_ADRS */
	testVal = *(volatile MV_U32 *)(UT_FLASH_ERASE_START_ADRS);
	
	if(0xFFFFFFFF == testVal)
	{			
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n %s - Run write to FLASH command FAILED.\n", __FUNCTION__);
		mvOsPrintf ("   - mem_command is: %s.\n", mem_command);
		return MV_FAIL;
	}

	/*====== STEP 06 ======*/ 	
	/* Clear read buffer. */
	memset ((MV_U32 *)adrsIndex, 0x0, writeBufferSize);	

	/* Read the value from read buffer. */
	testVal = *(volatile MV_U32 *)(adrsIndex);
	
	if(0x0 != testVal)
	{			
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n %s - Run clear read buffer command FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}	
	
	/* Set read from FLASH command. */
	sprintf(mem_command, "sflash read %08x %08x %x", (UT_FLASH_ERASE_START_ADRS - DEVICE_SPI_BASE), adrsIndex, writeBufferSize);
				
	/* run write to FLASH command. */
	run_command(mem_command, 0);			
			
	testVal = memcmp((MV_U32 *)adrsIndex, (MV_U32 *)UT_FLASH_ERASE_START_ADRS, (writeBufferSize / 4));
	
	if(0x0 != testVal)
	{			
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
					
		mvOsPrintf ("\n %s - Run memcmp after sflash read buffer command FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}	
	
	/*====== STEP 07 ======*/ 
	/* Set protect command */
	sprintf(mem_command, "sflash protect on");   
			
	/* run sflash command. */
	run_command(mem_command, 0);
	
	/* Set sflash command */
	sprintf(mem_command, "sflash info"); 	
	/*=========================================*/
	s_mvLoggerArr[0] = '\0';
	s_mvLoggerArrI = 0x0;
	
	/* run sflash command. */
	run_command(mem_command, 0);
									
	result = strstr((MV_U8 *)s_mvLoggerArr, mvUT_compare_str_SFlash_09);			  								
	if(result == NULL)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
		mvOsPrintf ("\n %d - ", ret_val);
		for(counter=0x0 ; counter <= 24; counter++)
			mvOsPrintf ("%c", s_mvLoggerArr[counter]);
			
		mvOsPrintf ("\n Run sflash protect off command FAILED.\n");
		return MV_FAIL;
	}	
 
	/* Enable print to UART. */
	if (mvUartPrintEnable(0) != MV_OK)
	{
		mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
		return MV_FAIL;
	}
  
	mvOsPrintf("  sflash - read/write or erase SPI FLASH command test ended successfully. (BoardType = %d)\n", BoardType);
	return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvMemoryDummyTest
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

MV_STATUS mvMemoryDummyTest(void)
{

	return MV_OK;
}

/* ================================================= */
/* ================================================= */
/* ================================================= */

MV_STATUS commands_test(void)
{
    MV_STATUS status = MV_OK;
	static volatile MV_STATUS retStatus = MV_OK;

	/* ============================================== */
	/*             Memory Tests.                      */
	/* ============================================== */

    if( MV_OK != mvMemoryBaseTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[base]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
		
	status = MV_OK;
    if( MV_OK != mvMemoryCopyTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[cp]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));		

	status = MV_OK;
    if( MV_OK != mvMemoryCompareTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[cmp]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvMemoryCompareValueTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[cmpm]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));		
#if UBOOT_COMMANDS_MEM_TEST_DEBUG
	status = MV_OK;
    if( MV_OK != mvMemoryCpumapTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[cpumap]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
#endif	
	status = MV_OK;
    if( MV_OK != mvMemoryCrc32Test() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[crc32]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;
    if( MV_OK != mvMemoryDramregsTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[dramregs]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;
    if( MV_OK != mvMemoryDisplayTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[md]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
		
	status = MV_OK;
    if( MV_OK != mvMemoryFindValTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[find]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
		
	status = MV_OK;	
    if( MV_OK != mvMemoryWriteTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[mw]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;		
    if( MV_OK != mvMemoryPhyReadTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[phyRead]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;		
    if( MV_OK != mvMemoryPhyWriteTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[phyWrite]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
		
	/* ============================================== */
	/*             PCI / PEX Tests.                   */
	/* ============================================== */	
		
	status = MV_OK;
    if( MV_OK != mvMemoryMpTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[mp]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
		
	/* ============================================== */
	/*             I2C Tests.                         */
	/* ============================================== */	
		
	status = MV_OK;
    if( MV_OK != mvMemoryIcrc32Test() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[icrc32]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
		
	status = MV_OK;
    if( MV_OK != mvMemoryImdTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[imd]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvMemoryImwTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[imw]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvMemoryIprobeTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[iprobe]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));		
	
	/* ============================================== */
	/*             FLASH Tests.                       */
	/* ============================================== */	
		
	status = MV_OK;	
    if( MV_OK != mvMemoryEraseFlashTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[erase]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

#if UBOOT_COMMANDS_MEM_TEST_DEBUG
	status = MV_OK;
    if( MV_OK != mvMemoryFlinfoFlashTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[flinfo]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
#endif

	status = MV_OK;
    if( MV_OK != mvMemoryProtectTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[protect]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	

#if UBOOT_COMMANDS_MEM_TEST_DEBUG
	status = MV_OK;
    if( MV_OK != mvMemorySFlashTest() )
	{
		mvOsPrintf(" retVal != MV_OK\n");
        status = retStatus = MV_FAIL;
	}
	else
	{
		if( MV_OK != status )
			mvOsPrintf(" status != MV_OK\n");
	}
	mvOsPrintf("[sflash]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));		
#endif	
	/* ============================================== */
	/*             General Tests.                     */
	/* ============================================== */	
#if UBOOT_COMMANDS_MEM_TEST_DEBUG
	status = MV_OK;
    if( MV_OK != mvGeneralDclkTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[dclk]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
#endif	
	status = MV_OK;
    if( MV_OK != mvGeneralEchoTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[echo]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;
    if( MV_OK != mvGeneralPingTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[ping]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;	
    if( MV_OK != mvGeneralSeTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[se]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;
    if( MV_OK != mvGeneralSgTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[sg]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvGeneralSpTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[sp]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	

#if UBOOT_COMMANDS_MEM_TEST_DEBUG
	status = MV_OK;	
    if( MV_OK != mvGeneralVersionTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[version]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
#endif

	status = MV_OK;		
    if( MV_OK != mvGeneralTestsmiTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[test_smi]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	/* ======== SWITCH commands ========= */
	
	status = MV_OK;	
    if( MV_OK != mvGeneralPhyrTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[phyr]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;	
    if( MV_OK != mvGeneralPhywTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[phyw]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;	
    if( MV_OK != mvGeneralSmdTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[smd]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;	
    if( MV_OK != mvGeneralSmwTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[smw]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;	
    if( MV_OK != mvGeneralSwitchMibReadTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[switchMibRead]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
    return retStatus;
}

MV_STATUS commands_test_until_fail(void)
{	
	/* ============================================== */
	/*             Memory Tests.                      */
	/* ============================================== */

    if( MV_OK != mvMemoryBaseTest() )
        return MV_FAIL;
	mvOsPrintf("[base]-%s\n\n", "PASS");

    if( MV_OK != mvMemoryCopyTest() )
        return MV_FAIL;
	mvOsPrintf("[cp]-%s\n\n", "PASS");	

    if( MV_OK != mvMemoryCompareTest() )
        return MV_FAIL;
	mvOsPrintf("[cmp]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryCompareValueTest() )
        return MV_FAIL;
	mvOsPrintf("[cmpm]-%s\n\n", "PASS");
#if UBOOT_COMMANDS_MEM_TEST_DEBUG
    if( MV_OK != mvMemoryCpumapTest() )
        return MV_FAIL;
	mvOsPrintf("[cpumap]-%s\n\n", "PASS");
#endif	
    if( MV_OK != mvMemoryCrc32Test() )
        return MV_FAIL;
	mvOsPrintf("[crc32]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemoryDramregsTest() )
        return MV_FAIL;
	mvOsPrintf("[dramregs]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemoryDisplayTest() )
        return MV_FAIL;
	mvOsPrintf("[md]-%s\n\n", "PASS");	
		
    if( MV_OK != mvMemoryFindValTest() )
        return MV_FAIL;
	mvOsPrintf("[find]-%s\n\n", "PASS");	
		
    if( MV_OK != mvMemoryIprobeTest() )
        return MV_FAIL;
	mvOsPrintf("[iprobe]-%s\n\n", "PASS");		
	
    if( MV_OK != mvMemoryWriteTest() )
        return MV_FAIL;
	mvOsPrintf("[mw]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemoryPhyReadTest() )
        return MV_FAIL;
	mvOsPrintf("[phyRead]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemoryPhyWriteTest() )
        return MV_FAIL;
	mvOsPrintf("[phyWrite]-%s\n\n", "PASS");	
	
	/* ============================================== */
	/*             PCI / PEX Tests.                   */
	/* ============================================== */	
		
    if( MV_OK != mvMemoryMpTest() )
        return MV_FAIL;
	mvOsPrintf("[mp]-%s\n\n", "PASS");
	
	/* ============================================== */
	/*             I2C Tests.                         */
	/* ============================================== */
	
    if( MV_OK != mvMemoryIcrc32Test() )
        return MV_FAIL;
	mvOsPrintf("[icrc32]-%s\n\n", "PASS");
		
    if( MV_OK != mvMemoryImdTest() )
        return MV_FAIL;
	mvOsPrintf("[imd]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryImwTest() )
        return MV_FAIL;
	mvOsPrintf("[imw]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryIprobeTest() )
        return MV_FAIL;
	mvOsPrintf("[iprobe]-%s\n\n", "PASS");	
	
	/* ============================================== */
	/*             FLASH Tests.                       */
	/* ============================================== */	
	
    if( MV_OK != mvMemoryEraseFlashTest() )
        return MV_FAIL;
	mvOsPrintf("[erase]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryFlinfoFlashTest() )
        return MV_FAIL;
	mvOsPrintf("[flinfo]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryProtectTest() )
        return MV_FAIL;
	mvOsPrintf("[protect]-%s\n\n", "PASS");		
	
    if( MV_OK != mvMemorySFlashTest() )
        return MV_FAIL;
	mvOsPrintf("[sflash]-%s\n\n", "PASS");	
	
	/* ============================================== */
	/*             General Tests.                     */
	/* ============================================== */	
	
    if( MV_OK != mvGeneralDclkTest() )
        return MV_FAIL;
	mvOsPrintf("[dclk]-%s\n\n", "PASS");

    if( MV_OK != mvGeneralEchoTest() )
        return MV_FAIL;
	mvOsPrintf("[echo]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralPingTest() )
        return MV_FAIL;
	mvOsPrintf("[ping]-%s\n\n", "PASS");
	
    if( MV_OK != mvGeneralSeTest() )
        return MV_FAIL;
	mvOsPrintf("[se]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralSgTest() )
        return MV_FAIL;
	mvOsPrintf("[sg]-%s\n\n", "PASS");
	
    if( MV_OK != mvGeneralSpTest() )
        return MV_FAIL;
	mvOsPrintf("[sp]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralVersionTest() )
        return MV_FAIL;
	mvOsPrintf("[version]-%s\n\n", "PASS");		
	
    if( MV_OK != mvGeneralTestsmiTest() )
        return MV_FAIL;
	mvOsPrintf("[test_smi]-%s\n\n", "PASS");
	
	/* ======== SWITCH commands ========= */
	
    if( MV_OK != mvGeneralPhyrTest() )
        return MV_FAIL;
	mvOsPrintf("[phyr]-%s\n\n", "PASS");

    if( MV_OK != mvGeneralPhywTest() )
        return MV_FAIL;
	mvOsPrintf("[phyw]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralSmdTest() )
        return MV_FAIL;
	mvOsPrintf("[smd]-%s\n\n", "PASS");

    if( MV_OK != mvGeneralSmwTest() )
        return MV_FAIL;
	mvOsPrintf("[smw]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralSwitchMibReadTest() )
        return MV_FAIL;
	mvOsPrintf("[switchMibRead]-%s\n\n", "PASS");	
	
    return MV_OK;
}

MV_STATUS commands_memory_test(void)
{
    MV_STATUS status = MV_OK;
	static volatile MV_STATUS retStatus = MV_OK;	
	
#if UBOOT_COMMANDS_MEM_TEST_DEBUG
    static volatile MV_U8  *result = NULL;  	
	static char boardName[30];
	static char boardRev[10];
    
    char name[20];

	name[0] = '\0';
    mvCtrlModelRevNameGet(name);
	
	result  = strstr((char *)name, mvUT_compare_str_modelName); 			  								
	if(result == NULL)
	{
		/* Enable print to UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
			
        mvOsPrintf("\n Soc: %s",  name);			
		mvOsPrintf("\n %s - Run cmvCtrlModelRevNameGet() FAILED.\n", __FUNCTION__);
		return MV_FAIL;
	}	
	else 
	{
        /* Enable print wto UART. */
		if (mvUartPrintEnable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintEnable failed.\n", __func__);
			return MV_FAIL;
		}
		mvOsPrintf("\n Soc: %s\n\n",  name);
		if (mvUartPrintDisable(0) != MV_OK)
		{
			mvOsPrintf("%s: mvUartPrintDisable failed.\n", __func__);
			return MV_FAIL;
		}
	}
	
	/* Get Marvell Board Name. */
	diagIfGetMarvellBoardName(boardName, boardRev);	
	
	mvOsPrintf("\n");
	#if defined(MV_CPU_BE)
		mvOsPrintf(" ** MARVELL BOARD: %s %sBE (%s)\n",
			boardName, boardRev, (db_98dx4122_detected)?"detected and changed from RD-98DX3121-24G" : "configured");
		mvOsPrintf(" ** Linux   - BE support\n");
		mvOsPrintf(" ** vxWorks - BE support\n");
	#else
		mvOsPrintf(" ** MARVELL BOARD: %s %sLE (%s)\n",
			boardName, boardRev, (db_98dx4122_detected)?"detected and changed from RD-98DX3121-24G" : "configured");
		mvOsPrintf(" ** Linux        - LE/BE support\n");
		mvOsPrintf(" ** vxWorks(elf) - LE/BE support\n");
	#endif
	mvOsPrintf("\n");	
#endif 	/* For DEBUG only.  */	
	
	/* ============================================== */
	/*             Memory Tests.                      */
	/* ============================================== */
		
    if( MV_OK != mvMemoryBaseTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[base]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;
    if( MV_OK != mvMemoryCopyTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[cp]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	

	status = MV_OK;
    if( MV_OK != mvMemoryCompareTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[cmp]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvMemoryCompareValueTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[cmpm]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
#if UBOOT_COMMANDS_MEM_TEST_DEBUG
	status = MV_OK;
    if( MV_OK != mvMemoryCpumapTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[cpumap]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
#endif	
	status = MV_OK;
    if( MV_OK != mvMemoryCrc32Test() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[crc32]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;
    if( MV_OK != mvMemoryDramregsTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[dramregs]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;
    if( MV_OK != mvMemoryDisplayTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[md]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
		
	status = MV_OK;
    if( MV_OK != mvMemoryFindValTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[find]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
		
	status = MV_OK;
    if( MV_OK != mvMemoryWriteTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[mw]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;		
    if( MV_OK != mvMemoryPhyReadTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[phyRead]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));		
	
	status = MV_OK;		
    if( MV_OK != mvMemoryPhyWriteTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[phyWrite]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	/* ============================================== */
	/*             PCI / PEX Tests.                   */
	/* ============================================== */	
		
	status = MV_OK;		
    if( MV_OK != mvMemoryMpTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[mp]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	/* ============================================== */
	/*             I2C Tests.                         */
	/* ============================================== */	
	
	status = MV_OK;
    if( MV_OK != mvMemoryIcrc32Test() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[icrc32]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
		
	status = MV_OK;
    if( MV_OK != mvMemoryImdTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[imd]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvMemoryImwTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[imw]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvMemoryIprobeTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[iprobe]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	/* ============================================== */
	/*             FLASH Tests.                       */
	/* ============================================== */	
	
	status = MV_OK;
    if( MV_OK != mvMemoryEraseFlashTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[erase]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvMemoryFlinfoFlashTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[flinfo]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;
    if( MV_OK != mvMemoryProtectTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[protect]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;
    if( MV_OK != mvMemorySFlashTest() )
	{
		mvOsPrintf(" retVal != MV_OK\n");
        status = MV_FAIL;
		retStatus = MV_FAIL;
	}
	else
	{
		if( MV_OK != status )
			mvOsPrintf(" status != MV_OK\n");
	}
	mvOsPrintf("[sflash]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
		
    return retStatus;
}

MV_STATUS commands_memory_test_until_fail(void)
{	
	/* ============================================== */
	/*             Memory Tests.                      */
	/* ============================================== */

    if( MV_OK != mvMemoryBaseTest() )
        return MV_FAIL;
	mvOsPrintf("[base]-%s\n\n", "PASS");

    if( MV_OK != mvMemoryCopyTest() )
        return MV_FAIL;
	mvOsPrintf("[cp]-%s\n\n", "PASS");	

    if( MV_OK != mvMemoryCompareTest() )
        return MV_FAIL;
	mvOsPrintf("[cmp]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryCompareValueTest() )
        return MV_FAIL;
	mvOsPrintf("[cmpm]-%s\n\n", "PASS");
#if UBOOT_COMMANDS_MEM_TEST_DEBUG
    if( MV_OK != mvMemoryCpumapTest() )
        return MV_FAIL;
	mvOsPrintf("[cpumap]-%s\n\n", "PASS");	
#endif	
    if( MV_OK != mvMemoryCrc32Test() )
        return MV_FAIL;
	mvOsPrintf("[crc32]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryDramregsTest() )
        return MV_FAIL;
	mvOsPrintf("[dramregs]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemoryDisplayTest() )
        return MV_FAIL;
	mvOsPrintf("[md]-%s\n\n", "PASS");	
		
    if( MV_OK != mvMemoryFindValTest() )
        return MV_FAIL;
	mvOsPrintf("[find]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemoryIprobeTest() )
        return MV_FAIL;
	mvOsPrintf("[iprobe]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemoryWriteTest() )
        return MV_FAIL;
	mvOsPrintf("[mw]-%s\n\n", "PASS");

    if( MV_OK != mvMemoryPhyReadTest() )
        return MV_FAIL;
	mvOsPrintf("[phyRead]-%s\n\n", "PASS");		
	
    if( MV_OK != mvMemoryPhyWriteTest() )
        return MV_FAIL;
	mvOsPrintf("[phyWrite]-%s\n\n", "PASS");	
	
	/* ============================================== */
	/*             PCI / PEX Tests.                   */
	/* ============================================== */	
		
    if( MV_OK != mvMemoryMpTest() )
        return MV_FAIL;
	mvOsPrintf("[mp]-%s\n\n", "PASS");	
	
	/* ============================================== */
	/*             I2C Tests.                         */
	/* ============================================== */	
	
    if( MV_OK != mvMemoryIcrc32Test() )
        return MV_FAIL;
	mvOsPrintf("[icrc32]-%s\n\n", "PASS");	
		
    if( MV_OK != mvMemoryImdTest() )
        return MV_FAIL;
	mvOsPrintf("[imw]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryImwTest() )
        return MV_FAIL;
	mvOsPrintf("[imw]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryIprobeTest() )
        return MV_FAIL;
	mvOsPrintf("[iprobe]-%s\n\n", "PASS");	
	
	/* ============================================== */
	/*             FLASH Tests.                       */
	/* ============================================== */	
	
    if( MV_OK != mvMemoryEraseFlashTest() )
        return MV_FAIL;
	mvOsPrintf("[erase]-%s\n\n", "PASS");
	
    if( MV_OK != mvMemoryFlinfoFlashTest() )
        return MV_FAIL;
	mvOsPrintf("[flinfo]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemoryProtectTest() )
        return MV_FAIL;
	mvOsPrintf("[protect]-%s\n\n", "PASS");	
	
    if( MV_OK != mvMemorySFlashTest() )
        return MV_FAIL;
	mvOsPrintf("[sflash]-%s\n\n", "PASS");	
		
    return MV_OK;
}

MV_STATUS commands_general_test(void)
{
    MV_STATUS status = MV_OK;
	static volatile MV_STATUS retStatus = MV_OK;
	
	/* ============================================== */
	/*             General Tests.                     */
	/* ============================================== */	
	
    if( MV_OK != mvGeneralDclkTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[dclk]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;
    if( MV_OK != mvGeneralEchoTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[echo]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;
    if( MV_OK != mvGeneralPingTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[ping]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;	
    if( MV_OK != mvGeneralSeTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[se]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;
    if( MV_OK != mvGeneralSgTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[sg]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;
    if( MV_OK != mvGeneralSpTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[sp]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;	
    if( MV_OK != mvGeneralVersionTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[version]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));
	
	status = MV_OK;		
    if( MV_OK != mvGeneralTestsmiTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[test_smi]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	/* ======== SWITCH commands ========= */
	
	status = MV_OK;	
    if( MV_OK != mvGeneralPhyrTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[phyr]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;	
    if( MV_OK != mvGeneralPhywTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[phyw]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;	
    if( MV_OK != mvGeneralSmdTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[smd]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));

	status = MV_OK;	
    if( MV_OK != mvGeneralSmwTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[smw]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
	status = MV_OK;	
    if( MV_OK != mvGeneralSwitchMibReadTest() )
        status = retStatus = MV_FAIL;
	mvOsPrintf("[switchMibRead]-%s\n\n", ((status!=MV_OK) ? "FAIL" : "PASS"));	
	
    return retStatus;
}

MV_STATUS commands_general_test_until_fail(void)
{	

	/* ============================================== */
	/*             General Tests.                     */
	/* ============================================== */	
	
    if( MV_OK != mvGeneralDclkTest() )
        return MV_FAIL;
	mvOsPrintf("[dclk]-%s\n\n", "PASS");

    if( MV_OK != mvGeneralEchoTest() )
        return MV_FAIL;
	mvOsPrintf("[echo]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralPingTest() )
        return MV_FAIL;
	mvOsPrintf("[ping]-%s\n\n", "PASS");
	
    if( MV_OK != mvGeneralSeTest() )
        return MV_FAIL;
	mvOsPrintf("[se]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralSgTest() )
        return MV_FAIL;
	mvOsPrintf("[sg]-%s\n\n", "PASS");
	
    if( MV_OK != mvGeneralSpTest() )
        return MV_FAIL;
	mvOsPrintf("[sp]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralVersionTest() )
        return MV_FAIL;
	mvOsPrintf("[version]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralTestsmiTest() )
        return MV_FAIL;
	mvOsPrintf("[test_smi]-%s\n\n", "PASS");
	
	/* ======== SWITCH commands ========= */
		
    if( MV_OK != mvGeneralPhyrTest() )
        return MV_FAIL;
	mvOsPrintf("[phyr]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralPhywTest() )
        return MV_FAIL;
	mvOsPrintf("[phyw]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralSmdTest() )
        return MV_FAIL;
	mvOsPrintf("[smd]-%s\n\n", "PASS");

    if( MV_OK != mvGeneralSmwTest() )
        return MV_FAIL;
	mvOsPrintf("[smw]-%s\n\n", "PASS");	
	
    if( MV_OK != mvGeneralSwitchMibReadTest() )
        return MV_FAIL;
	mvOsPrintf("[switchMibRead]-%s\n\n", "PASS");	
	
    return MV_OK;
}

#endif  /* COMMANDS_TEST */


/*$ END OF <module_name> */


