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
#include "diag_inf.h"
#include "diag_services.h"

#include "private_test_config.h"

/* **************************************************************************
 *              PUBLIC DECLARATIONS (EXPORT)
 * **************************************************************************
 * */
    
/* **************************************************************************
 *              PUBLIC VARIABLE DEFINITIONS (EXPORT)
 * **************************************************************************
 * */
    extern mvBspDiagDramTestParamStruct dram_params;

/* **************************************************************************
 *              LOCAL DECLARATIONS
 * **************************************************************************
 * */
#define DRAM_TEST
#ifdef  DRAM_TEST

/* dram defines */
#define mvBspDramFirstTestValue     0x12345678
#define mvBspDramNextTestValue      0xA5A5A5A5
#define mvBspDramMaxSizeDetect      0x80000000    /* max of 2Gbytes */
#define mvBspDramGrant              0x00100000
#define mvBspDramGrantAlignMask     0xFFF00000

#define mvBspDramInitValue          0x55
#define mvBspDramVerifyValue        0xAA

#define mvBspDramScanLength         0x100

/* dram MACROS */   /*
#define mvBspDramLocalMemAddressMacro   dram_params.base_address
#define mvBspDramStartOffsetMacro       dram_params.first_size_offset
#define mvBspDramLocalMemSizeMacro      dram_params.mem_size
#define mvBspDramAmountOfDevicesMacro   dram_params.dev_amount
#define mvBspDramBusWidthMacro          dram_params.data_bus_width
*/

/* **************************************************************************
 *              LOCAL VARIABLE DEFINITIONS
 * **************************************************************************
 * */

/* dram variables */
/*static */MV_U32 mvBspDramSizeLocal = 0;

/* **************************************************************************
 *              LOCAL FUNCTION DEFINITIONS
 * **************************************************************************
 * */

/* **************************************************************************
*
*  FUNCTION: mvDramMemoryAreaDeviceTest
*
*  GENERAL DESCRIPTION:  DRAM area memory test
*                        Test the DRAM memory from startOffset until endOffset.
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
static MV_STATUS mvDramMemoryAreaDeviceTest(
  /*      INPUTS:             */
  MV_U32    startOffset,
  MV_U32    endOffset,
  MV_STATUS complement_pattern
  /*     INPUTS / OUTPUTS:   */
  /*     OUTPUTS:            */
)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/
  MV_U32 intMask;
  register MV_U32 curVal;
  register MV_U32 testVal;
  register MV_U32 adrsIndex;
  register MV_U32 requestedVal;
/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

  if (startOffset > endOffset)
  {
    printf ("\nDRAM Memory Device Test: wrong offset values.");
    return MV_ERROR;
  }

  for (adrsIndex = startOffset + dram_params.base_address;
       adrsIndex < endOffset + dram_params.base_address;
       adrsIndex += 4)
  {
    /* Disable interrupts */
    diagIfIntLock(&intMask);

    /* save the current address value */
    curVal = *(volatile MV_U32 *)adrsIndex;

    if (complement_pattern)
      /* write to the address its own address */
      *(volatile MV_U32 *)adrsIndex = ~adrsIndex;
    else
      /* write to the address its own complement address */
      *(volatile MV_U32 *)adrsIndex = adrsIndex;

    /* Read the written value */
    testVal = *(volatile MV_U32 *)adrsIndex;

    /* restore the original value to the address */
    *(volatile MV_U32 *)adrsIndex = curVal;

    /* Enable interrupts */
    diagIfIntUnlock(&intMask);

    if (complement_pattern)
      requestedVal = ~adrsIndex;
    else
      requestedVal = adrsIndex;

    if (testVal != requestedVal)
    {
      printf("\nDRAM Memory Device Test: At address 0x%x writen", adrsIndex);
      printf(" value is 0x%x but read value is 0x%x\n", requestedVal, testVal);
      return MV_ERROR;
    }

    mvBspServicesProgressBar (adrsIndex, mvBspDramSizeLocal,
                                    mvBspServicesProgressBarRunningEnum);
  }

  asm ( " dramTestEndLabel: " );

  return MV_OK;
}

 /* **************************************************************************
 *              PUBLIC FUNCTION DEFINITIONS (EXPORT)
 * **************************************************************************
 * */

/* **************************************************************************
* 
*  FUNCTION: mvBspDramFreeMemoryReadTest
*
*  GENERAL DESCRIPTION:
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
MV_STATUS mvBspDramFreeMemoryReadTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/
  MV_U32 parse_address, parse_data_size, parse_units_num;

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

  printf("\n\nPlease notice: It is the user responsibility to set here ");
  printf("a valid address.\nOtherwise the system might get an exception!");
  printf("\nIt is also the user responsibility to make sure that the ");
  printf("requested address is\naligned with its size (byte, short or word)");

  printf("\n\nEnter address in hex: ");

  if (mvBspServicesInputHexNumber(&parse_address) != MV_OK)
    return MV_ERROR;

  printf("Enter unit size in bytes (1,2 or 4): ");

  if (mvBspServicesInputHexNumber(&parse_data_size) != MV_OK)
    return MV_ERROR;

  if ((parse_data_size != 1) && (parse_data_size != 2) && (parse_data_size != 4))
  {
    printf ("\nError: Wrong size. Must be 1,2 or 4\n");

    return MV_ERROR;
  }

  printf("Enter number of units to display: ");

  if (mvBspServicesInputHexNumber(&parse_units_num) != MV_OK)
    return MV_ERROR;

  mvBspServicesDisplayMemoryTable((MV_U8 *)parse_address,
                              parse_units_num ,parse_data_size, parse_address);

  return MV_OK;
}

/* **************************************************************************
*
*  FUNCTION: mvBspDramFreeMemoryWriteTest
*
*  GENERAL DESCRIPTION:
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
MV_STATUS mvBspDramFreeMemoryWriteTest(void)
{
/* ****************************************************************************/
/*  L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/* ****************************************************************************/
  MV_U8  data[mvBspDramScanLength];
  MV_U32 parse_address, parse_data_size, parse_data;
  MV_STATUS parse_result;

/* *************************************************************************/
/*                       F U N C T I O N   L O G I C                       */
/* *************************************************************************/

  printf("\n\nPlease notice: It is the user responsibility to set here ");
  printf("a valid address.\nOtherwise the system might get an exception!");
  printf("\nIt is also the user responsibility to make sure that the ");
  printf("requested address is\naligned with its size (byte, short or word)");

  printf("\n\nEnter address in hex: ");

  if (mvBspServicesInputHexNumber(&parse_address) != MV_OK)
    return MV_ERROR;

  printf("Enter unit size in bytes (1,2 or 4): ");

  if (mvBspServicesInputHexNumber(&parse_data_size) != MV_OK)
    return MV_ERROR;

  if ((parse_data_size != 1) && (parse_data_size != 2) && (parse_data_size != 4))
  {
    printf ("\nError: Wrong size. Must be 1,2 or 4\n");

    return MV_ERROR;
  }

  memset (data, 0, mvBspDramScanLength);

  printf ("Enter data in hex: ");
  mvBspServicesScanStr  (mvBspDramScanLength, data);
  printf ("\n");

  parse_data = 0;

  parse_result = mvBspServicesParseHexNumber ((MV_U8 *)data,
                                            (MV_U8) strlen(data), &parse_data);
  if ( parse_result != MV_OK )
  {
    printf ("\nError: Wrong data.");
    return MV_ERROR;
  }

  if(parse_data_size == 1)
    *(MV_U8 *)parse_address = (MV_U8)(parse_data) & 0xFF;
  else if(parse_data_size == 2)
    *(MV_U16 *)parse_address = (MV_U16)(parse_data) & 0xFFFF;
  else
    *(MV_U32 *)parse_address = parse_data;

  printf("\n");

  return MV_OK;
}

/* **************************************************************************
 *
 *  FUNCTION: mvBspDramSizeTest
 *
 *  GENERAL DESCRIPTION:
 *
 *  RETURNS:
 *
 *  ALGORITHM:
 *       Read (and store) the data from the base location of the memory.
 *       Write 0x12345678 to this location and verify.
 *       Change the address to its next value.
 *       Store the data and write 0xa5a5a5a5. Verify this operation.
 *       Read the base location. If it will contain 0x12345678 return the
 *       stored data and continue to next address and perform the previous
 *       step again. If the first location is 0xa5a5a5a5 write in the first
 *       location its original value and return (display) the memory size.
 *
 *  ASSUMPTIONS:
 *
 *  REMARKS:  The diagnostic code and the OS sits on the tested DRAM, so
 *            the start test address does not begin from 0x0, but from
 *            HOSTP_gdiag_dram_startOffset_MACRO - offset defined in P_HOST
 *
 * **************************************************************************
 * */

MV_STATUS mvBspDramSizeTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
   MV_U32               grant;
   volatile MV_U32      *first_PTR;
   volatile MV_U32      first_val, cur_val, tmp_ptr;
   volatile MV_U32      value;

   mvBspDiagDramProbeParamStruct probe_params;

/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

  printf ("\n");

  /* cache off */
  diagIfIcacheDisable(MV_FALSE);
  diagIfDcacheDisable(MV_FALSE);

  /* this is the accuary of the test (test in steps of "grant" each) */
  grant = mvBspDramGrant;

  /* first_PTR should be the first address after code & OS data so we dont
     overlap with it during tests.  */
  first_PTR = (MV_U32 *)(dram_params.first_size_offset + dram_params.base_address);
  first_val = *first_PTR;

  *first_PTR = mvBspDramFirstTestValue;

  /* try to write to first address */
  if (*first_PTR != mvBspDramFirstTestValue)
  {
    printf ("\nRam Size Test failed on first read\n");
    printf ("Address read 0x%x", (int)first_PTR);
    return MV_ERROR;
  }

  /* start scan from grant aligned adress  */
  tmp_ptr = (MV_U32)(((MV_U32)first_PTR + mvBspDramGrant) &
                                             mvBspDramGrantAlignMask);

  /* scan all RAM until we cant write so no RAM is available */
  for ( ; tmp_ptr <= (MV_U32)first_PTR + mvBspDramMaxSizeDetect;
                                                             tmp_ptr += grant)
  {
    if(dram_params.mem_probe_func != NULL)
    {
      probe_params.address = tmp_ptr;
      probe_params.size = sizeof (MV_U32);
      probe_params.operation = 1;
      probe_params.value_ptr = (MV_U32 *)&cur_val;

      if(dram_params.mem_probe_func( (MV_U32 *)&probe_params) != MV_TRUE )
        break;
    }
    else
      cur_val = *(MV_U32 *)tmp_ptr;

    /* vrite test value to new address */
    value = mvBspDramNextTestValue;
    *(MV_U32 *)tmp_ptr = value;

    /* this is to make sure we dont have duplications - when we think the RAM
       is still valid here but actualy it is virtually duplicated */
    if (*first_PTR != mvBspDramFirstTestValue)
    {
      tmp_ptr -= dram_params.first_size_offset;
      break;
    }

    /* have we reached end of RAM? */
    if ((*(MV_U32 *)tmp_ptr != mvBspDramNextTestValue))
      break;

    *(MV_U32 *)tmp_ptr = cur_val;
  }

  /* cache on */
    diagIfIcacheEnable(MV_TRUE);
    diagIfDcacheEnable(MV_TRUE);

 /* convert address to size and update detected size to global parameter */
  mvBspDramSizeLocal = tmp_ptr - dram_params.base_address;

  printf ("\nDetected DRAM memory size: %d KB.", (int)mvBspDramSizeLocal >> 10);

  if ((dram_params.mem_size != 0) &&
      (dram_params.mem_size != mvBspDramSizeLocal))
  {
    printf ("\nDeclared DRAM memory size %d KB.",
                         (int)(dram_params.mem_size) >> 10);
    return MV_ERROR;
  }
  else
    dram_params.mem_size = mvBspDramSizeLocal;

  *first_PTR = first_val;

  return MV_OK;

}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspDramMemoryDeviceTest
*!
*$ GENERAL DESCRIPTION:
*!          The Memory Device test purpose is to confirm that every bit in the
*!          device is capable of holding both 0 and 1. This test performs
*!          "increment/decrement test".
*!          The data value should change with (but is not equivalent to) the
*!          address.
*!
*$ RETURNS:
*!
*$ ALGORITHM:
*!
*!          1. Write increment data values to all memory addresses.
*!          2. Verify that all address set correctly.
*!          3. Write decrement data values to all memory addresses.
*!          4. Verify that address set correctly.
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:
*!
*!**************************************************************************
*!*/
MV_STATUS mvBspDramMemoryDeviceTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_STATUS status = MV_OK;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  if (mvBspDramSizeLocal == 0)
    mvBspDramSizeTest();

  printf ("\nDRAM Memory Device Test");

  printf("\nFirst  pattern test in progress       ");

  /* reset the percent function with flag param set to one */
  mvBspServicesProgressBar (0, mvBspDramSizeLocal,
                                    mvBspServicesProgressBarStartEnum);

  /* disable data cache to allow access the address not defined in the MMU */
  diagIfDcacheDisable(0);
  /* first patern test, write the address as data */
  /* status = mvDramMemoryAreaDeviceTest (
                                0,
                                (MV_U32)mvDramMemoryAreaDeviceTest,
                                MV_FALSE);

  if (status == MV_OK) */
    status = mvDramMemoryAreaDeviceTest(
                                dram_params.first_size_offset,
                                dram_params.first_size_offset + mvBspDramGrant,
                                MV_FALSE);

  printf ("\nFirst pattern test");

  if (status == MV_OK)
  {
    printf ("................................PASS");
  }
  else
  {
    printf ("................................FAIL");
    return status;
  }

  /* print 100 % */
  mvBspServicesProgressBar (0, mvBspDramSizeLocal,
                                      mvBspServicesProgressBarEndEnum);

  printf("\nSecond pattern test in progress       ");

  /* reset the percent calc function */
  mvBspServicesProgressBar (0, mvBspDramSizeLocal,
                                    mvBspServicesProgressBarStartEnum);

  /* second pattern test write address negative value */
  /*
  status = mvDramMemoryAreaDeviceTest (
                            0,
                            (MV_U32)mvDramMemoryAreaDeviceTest,
                            MV_TRUE); 

  if (status == MV_OK) */
    status = mvDramMemoryAreaDeviceTest (
                            dram_params.first_size_offset,
                            dram_params.first_size_offset + mvBspDramGrant,
                            MV_TRUE);

  /* Enabling the data cache */
  diagIfDcacheEnable(1);

  /* print 100 % */
  mvBspServicesProgressBar (0, mvBspDramSizeLocal,
                                      mvBspServicesProgressBarEndEnum);

  printf ("\nSecond pattern test");

  if (status == MV_OK)
  {
    printf ("...............................PASS\n");
  }
  else
  {
    printf ("...............................FAIL\n");
  }

  return status;
}


/*!**************************************************************************
*!
*$ FUNCTION: mvBspDramDataBusTest
*!
*$ GENERAL DESCRIPTION:
*!           The Data bus test purpose is to confirm that the memory device at
*!          the other end correctly receives any value placed on the data bus by
*!          the processor. This test performs the so-called "walking 1's test".
*!          The "walking 1's test" Algorithm is setting each time one bit to '1'
*!          and "walked" through the entire data word. The number of data value
*!          to test is the same as the width of the data bus.
*!
*$ RETURNS:
*!
*$ ALGORITHM:
*!
*!          1. Choose any address within the memory device. (If the Data bus
*!             splits to more then one chip, this test should be performed to
*!             one address in each chip).
*!          2. Write 0x00 to the memory and verify it by reading it back.
*!          3. Write 0x01 to the same place and verify it by reading it back.
*!          4. Write the next value by shifting left the previous value (0x02),
*!             verify.
*!          5. Repeat step 4 until the MSB is set.
*!
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:  The diagnostic code and the OS sits on the tested DRAM, so
*!           the start test address does not begin from 0x0, but from
*!           HOSTP_gdiag_dram_startOffset_MACRO - offset defined in P_HOST
*!
*!**************************************************************************
*!*/
MV_STATUS mvBspDramDataBusTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
   MV_U32 data, curData, adrs;
   MV_U16 dramIndex, widthIndex;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  if (mvBspDramSizeLocal == 0)
    mvBspDramSizeTest();

  printf ("\n");

  /* scan all sdram devices */
  for (dramIndex = 0, adrs = dram_params.base_address;
       dramIndex < dram_params.dev_amount;
       dramIndex++, adrs += mvBspDramSizeLocal/dram_params.dev_amount)
  {
     /* bit0 is '1', others are '0' */
     data = 1;

     /* save the original address value */
     curData = *(MV_U32 *)adrs;

     /* step through all bus width */
     for (widthIndex = 0;
          widthIndex < dram_params.data_bus_width;
          widthIndex++)
     {
        /* 1 bit of 32 bit is writen with value of 1 */
        *(MV_U32 *)adrs = data;

        /* verify the writen value */
        if (*(MV_U32 *)adrs != data)
        {
          printf("\nRAM test: data bus error, %s: 0x%x, %s: 0x%x",
                         "writen value", data, "read value", *(MV_U32 *)adrs);
          printf ("\nDRAM Data Bus Test");
          printf ("................................FAIL");

          return MV_ERROR;
        }
        /* next bit gets the '1' value */
        data <<= 1;
     }

     /* restore the original address value */
     *(MV_U32 *)adrs = curData;
  }

  printf ("\nDRAM Data Bus Test");
  printf ("................................PASS");

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspDramReadByOffset
*!
*$ GENERAL DESCRIPTION:  Read data from NVRAM.
*!
*$ RETURNS:
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!**************************************************************************
*!*/
MV_STATUS mvBspDramReadByOffset(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 parseOffset;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  /* if RAM size unknown detect ram size */
  if (dram_params.mem_size == 0)
  {
    printf("\nMemory size detection in progress.");

    if (mvBspDramSizeTest() == MV_ERROR)
    {
      printf("\nError in memory size detection.");

      return MV_ERROR;
    }

    printf("\nMemory size detected - %d KB.",
                                  (int)(dram_params.mem_size) >> 10);
  }

  printf("\nEnter offset in hex: ");

  if (mvBspServicesInputHexNumber(&parseOffset) != MV_OK)
    return MV_ERROR;

  if (parseOffset >= dram_params.mem_size)
  {
    printf("\nError: Wrong offset. Must be from 0x0 to 0x%x\n",
                                            dram_params.mem_size - 1);
    return MV_ERROR;
  }

  mvBspServicesDisplayMemoryTable((MV_U8 *)parseOffset,
                      (mvBspDramScanLength) ,sizeof(MV_U32), parseOffset);

  return MV_OK;

}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspDramWriteByOffset
*!
*$ GENERAL DESCRIPTION:  Write data to NVRAM.
*!
*$ RETURNS:
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!**************************************************************************
*!*/
MV_STATUS mvBspDramWriteByOffset(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U8  data[mvBspDramScanLength];
  MV_U32 parseOffset, parse_data_size, parse_data;
  MV_STATUS parse_result;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  /* if RAM size unknown detect ram size */
  if (dram_params.mem_size == 0)
  {
    printf("\nMemory size detection in progress.");

    if (mvBspDramSizeTest() == MV_ERROR)
    {
      printf("\nError in memory size detection.");

      return MV_ERROR;
    }

    printf("\nMemory size detected - %d KB.",
                                  (int)(dram_params.mem_size) >> 10);
  }

  printf("\n\nEnter offset in hex: ");

  if (mvBspServicesInputHexNumber(&parseOffset) != MV_OK)
    return MV_ERROR;

  if (parseOffset >= dram_params.mem_size)
  {
    printf("\nError: Wrong offset. Must be from 0x0 to 0x%X\n",
                                            dram_params.mem_size - 1);
    return MV_ERROR;
  }

  printf("Enter size in bytes (0x1 - 0x4): ");

  if (mvBspServicesInputHexNumber(&parse_data_size) != MV_OK)
    return MV_ERROR;

  if ((parse_data_size < 1)||
      (parse_data_size > 4))
  {
    printf ("\nError: Wrong size.Must be from 0x1 to 0x4");

    return MV_ERROR;
  }

  memset (data, 0, mvBspDramScanLength);

  printf ("Enter data in hex: ");
  mvBspServicesScanStr  (mvBspDramScanLength, data);
  printf ("\n");

  parse_data = 0;

  parse_result = mvBspServicesParseHexNumber ((MV_U8 *)data,
                                                     (MV_U8) strlen(data), &parse_data);
  if ( parse_result != MV_OK )
  {
    printf ("\nError: Wrong data.");
    return MV_ERROR;
  }

  memcpy ((MV_U8 *)parseOffset ,(MV_U8 *)&parse_data, parse_data_size);

  printf("\n");

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspDramAddrBusTest
*!
*$ GENERAL DESCRIPTION:
*!           The Address bus test purpose is to confirm that there is no
*!           overlapping in memory locations. This test check address
*!           combinations of "power-of-two" address, which is similar to the
*!           "walking 1's test".
*!
*$ RETURNS:
*!
*$ ALGORITHM:
*!
*!         1.  Initial all "power-of-two" addresses and address 0x00h with 0x55h.
*!         2.  Write 0xAAh to the first offset.
*!         3.  Verify that the initial data value is still stored at every other
*!             "power-of-two" offset (that are bigger than tested offset).
*!         4.  If found a location that contains 0xAA there is a problem with
*!             the current address bit and software should display a message on
*!             screen: "Bits X1 and X2 FAILED the test". (Where X1 is the
*!            tested bit and X2 is the verified bit).
*!         5.  Write 0xAA to the next "power of-two" offset, verify.
*!         6.  Repeat step 5 until the MSB is set.
*!
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:  The diagnostic code and the OS sits on the tested DRAM, so
*!           the start test address does not begin from 0x0, but from
*!           HOSTP_gdiag_dram_startOffset_MACRO - offset defined in P_HOST
*!
*!**************************************************************************
*!*/
MV_STATUS mvBspDramAddrBusTest(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
   MV_U32 adrsIndex, curIndex;
   MV_U32 oldVals[64];
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  if (mvBspDramSizeLocal == 0)
    mvBspDramSizeTest();

  printf ("\n");

  /* initial all "power-of-two" addresses and address 0x00h with 0x55 */
  for (curIndex = 0, adrsIndex = dram_params.base_address + dram_params.first_size_offset;
       adrsIndex < dram_params.base_address + mvBspDramSizeLocal;
       adrsIndex <<= 1, curIndex++)
  {
     /* save address original value */
     oldVals [curIndex] = *(MV_U8 *)adrsIndex;
     /* write initial value pattern */
     *(MV_U8 *)adrsIndex = mvBspDramInitValue;
  }

  for (curIndex = dram_params.first_size_offset + dram_params.base_address;
       curIndex < mvBspDramSizeLocal >> 1; curIndex <<= 1)
  {
     /* write the verification value */
     *(MV_U8 *)curIndex = mvBspDramVerifyValue;
     /* checks that the initial value still exists at every other
        "power-of-two" offset */
     for (adrsIndex = curIndex << 1; adrsIndex < mvBspDramSizeLocal;
                                                               adrsIndex <<= 1)
        if (*(MV_U8 *)adrsIndex == mvBspDramVerifyValue) 
        { 
           printf("\nAddress Bus Test Error: Writing 0x%x to address ",
                                                    mvBspDramVerifyValue);
           printf (" 0x%x overlapped in address 0x%x.\n", curIndex, adrsIndex);

           printf ("\nDRAM Address Bus Test");

           printf (".............................FAIL");

           return MV_ERROR;
        } 
  }

  /* restore the orginal addresses values */
  for (curIndex = 0, adrsIndex = dram_params.first_size_offset + dram_params.base_address;
       adrsIndex < mvBspDramSizeLocal + dram_params.base_address;
       adrsIndex <<= 1, curIndex++)
     *(MV_U8 *)adrsIndex = oldVals [curIndex];

  printf ("\nDRAM Address Bus Test");
  printf (".............................PASS");

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspDramTestProbe
*!
*$ GENERAL DESCRIPTION:
*!
*$ RETURNS:
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!**************************************************************************
*!*/
MV_STATUS mvBspDramTestProbe(
  /*!     INPUTS:             */
  MV_U32 *params
)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  mvBspDiagDramProbeParamStruct *probe_params;
  #if UBOOT_HWD_DRAM_TEST_DBG
  int operation_type;
  #endif
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  probe_params = (mvBspDiagDramProbeParamStruct *)params;

  if(probe_params->size != 1 && probe_params->size != 2 &&
                           probe_params->size != 4 && probe_params->size != 8)
    return MV_ERROR;

#if UBOOT_HWD_DRAM_TEST_DBG
  if(probe_params->operation == 0)
    operation_type = VX_WRITE;
  else
    operation_type = VX_READ;

  if(vxMemProbe ((char *)probe_params->address, probe_params->operation,
                  probe_params->size, (char *)probe_params->value_ptr) == MV_ERROR)
    return MV_ERROR;
  else
#endif

    return MV_FAIL;     /* TBD */
}



MV_STATUS dram_test(void)
{
    MV_STATUS status = MV_OK;

    /*
    if( (MV_OK != mvBspDramAddrBusTest())   ||
        (MV_OK != mvBspDramDataBusTest())  ||
        (MV_OK != mvBspDramSizeTest())    ||
        (MV_OK != mvBspDramMemoryDeviceTest()) )
        status = MV_ERROR;
    */
    if( MV_OK != mvBspDramAddrBusTest() )
        status = MV_ERROR;

    if( MV_OK != mvBspDramDataBusTest() )
        status = MV_ERROR;;

    if( MV_OK != mvBspDramSizeTest() )
        status = MV_ERROR;;

    if( MV_OK != mvBspDramMemoryDeviceTest() )
        status = MV_ERROR;;

    return status;
}

#endif  /* DRAM_TEST */


/*$ END OF <module_name> */


