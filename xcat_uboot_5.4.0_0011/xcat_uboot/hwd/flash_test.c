/*!**************************************************************************
 *!
 *$ GENERAL DESCRIPTION:
 *!
 *$ PROCESS AND ALGORITHM: (local)
 *!
 *$ PACKAGE GLOBAL SERVICES:
 *!     (A list of package global services).
 *!
 *$ PACKAGE LOCAL SERVICES:  (local)
 *!     (A list of package local services).
 *!
 *$ PACKAGE USAGE:
 *!     (How to use the package services,
 *!     routines calling order, restrictions, etc.)
 *!
 *$ ASSUMPTIONS:
 *!
 *$ SIDE EFFECTS:
 *!
 *$ RELATED DOCUMENTS:     (local)
 *!
 *$ REMARKS:               (local)
 *!
 *!**************************************************************************
 *!*/

/*$ <module_name> BODY */

/*! General definitions */

/*!**************************************************************************
 *$              EXTERNAL DECLARATIONS (IMPORT AND EXPORT)
 *!**************************************************************************
 *!*/

#include <common.h>
#include <flash.h>
#include "hwd_config.h"
#include "diag_inf.h"
#include "diag_services.h"

#include "private_test_config.h"

/*!**************************************************************************
 *$              EXTERNAL DECLARATIONS (IMPORT)
 *!**************************************************************************
 *!*/
    extern mvBspDiagFlashTestParamStruct flash_params;
/*!**************************************************************************
 *$              PUBLIC DECLARATIONS (EXPORT)
 *!**************************************************************************
 *!*/

/*!**************************************************************************
 *$              PUBLIC VARIABLE DEFINITIONS (EXPORT)
 *!**************************************************************************
 *!*/

/*!**************************************************************************
 *$              LOCAL DECLARATIONS
 *!**************************************************************************
 *!*/

#ifdef  FLASH_TEST


#define MV_FLASH_INIT_VAL            0x55
#define MV_FLASH_VERIFY_VAL          0x01
#define MV_FLASH_START_INDEX         0x01

#define MV_FLASH_MAX_SECTOR_SIZE     0x80000
#define MV_FLASH_SCAN_LEN            0x100

/*!**************************************************************************
 *$              LOCAL VARIABLE DEFINITIONS
 *!**************************************************************************
 *!*/

/*!**************************************************************************
 *$              LOCAL FUNCTION DEFINITIONS
 *!**************************************************************************
 *!*/



/*!**************************************************************************
*!
*$ FUNCTION: mvFlashCheckReservedOffsets
*!
*$ GENERAL DESCRIPTION:  Checking is current flash sector can be deleted.
*!
*$ RETURNS: MV_OK if given offset not reserved from erase/write operations.
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!**************************************************************************
*!*/
static BOOLEAN mvFlashCheckReservedOffsets (
  /*!     INPUTS:             */
  MV_U32 offset
  /*!     INPUTS / OUTPUTS:   */

  /*!     OUTPUTS:            */

)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  if (((offset >= flash_params.reserved_offsets[0]) &&
      (offset < (flash_params.reserved_offsets[0] + flash_params.reserved_sizes[0]))) ||
      ((offset >= flash_params.reserved_offsets[1]) &&
      (offset < (flash_params.reserved_offsets[1] + flash_params.reserved_sizes[1]))) ||
      ((offset >= flash_params.reserved_offsets[2]) &&
      (offset < (flash_params.reserved_offsets[2] + flash_params.reserved_sizes[2]))))
    return MV_ERROR;

  return MV_OK;
}

/*!**************************************************************************
 *$              PUBLIC FUNCTION DEFINITIONS (EXPORT)
 *!**************************************************************************
 *!*/

/*!**************************************************************************
*!
*$ FUNCTION: mvBspFlashMemoryDeviceTest
*!
*$ GENERAL DESCRIPTION:
*!
*$ RETURNS:
*!
*$ ALGORITHM:
*!          The Memory Device test purpose is to confirm that every bit in the
*!          device is capable of holding both 0 and 1. This test performs
*!          "increment/decrement test".
*!          The data value should change with (but is not equivalent to) the
*!          address.
*!
*!          The steps are:
*!          1. Erase the Flash sectors.
*!          2. Write increment data values to all Flash memory addresses.
*!          3. Verify that all address set correctly.
*!          4. Erase the Flash sectors.
*!          5. Write decrement data values to all Flash memory addresses.
*!          6. Verify that address set correctly.
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:
*!**************************************************************************
*!*/
MV_STATUS mvBspFlashMemoryDeviceTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 adrsIndex, startIndex, readValue, countTotalSize, progressCount;
  MV_U32 *flash_adrs_value_tbl = NULL;
  float   one_percent;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf("\n\nFLASH Memory Device Test\n");
  printf("------------------------\n");
  printf("\nTested size:0x%x .\n",flash_params.tested_area_size);
  flash_adrs_value_tbl =
         (MV_U32 *)mvBspServicesAllocMem(MV_FLASH_MAX_SECTOR_SIZE);

  if (flash_adrs_value_tbl == NULL)
  {
    printf("..........................FAIL\n");
    printf("\nMemory allocation failed.");
    return MV_ERROR;
  }

  countTotalSize = 100; /* 100 % */

  one_percent = 100 / (float)(flash_params.tested_area_size/flash_params.page_size);

  /* intiate table with test values */
  for (adrsIndex = 0; adrsIndex < flash_params.page_size;
                                                  adrsIndex += sizeof (MV_U32))
    flash_adrs_value_tbl[adrsIndex / sizeof(MV_U32)] = adrsIndex;

  printf ("\nFirst pattern test in progress...\n");
  printf ("\nErasing Flash sectors:     ");

  progressCount = 0;

  /* reset the percent function with flag param set to MV_OK */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                           mvBspServicesProgressBarStartEnum);

  /* erase all flash */
  for (adrsIndex = 0; adrsIndex < flash_params.tested_area_size; adrsIndex +=
                                                   flash_params.page_size)
  {
    if (mvFlashCheckReservedOffsets(adrsIndex) == MV_OK)
    {
      if (diagIfFlashErase (adrsIndex) != MV_OK)
      {
        printf("..........................FAIL\n");
        printf ("Flash Erase Error at offset 0x%X\n",adrsIndex);
        return MV_ERROR;
      }
    }
    mvBspServicesProgressBar ((MV_U32)(one_percent * (progressCount++)),
                     countTotalSize, mvBspServicesProgressBarRunningEnum);
  }

  /* print 100 % */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                          mvBspServicesProgressBarEndEnum);

  printf("\n\nWriting and verifying:     ");

  progressCount = 0;

  /* reset the percent function with flag param set to MV_OK */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                          mvBspServicesProgressBarStartEnum);

  /* step through all flash sectors */
  for (adrsIndex = 0x0; adrsIndex < flash_params.tested_area_size;
                                      adrsIndex += flash_params.page_size)
  {
    /* write test data to all sector */
    if (mvFlashCheckReservedOffsets(adrsIndex) == MV_OK)
    {
      if (diagIfFlashWrite(adrsIndex, flash_params.page_size,
                          flash_adrs_value_tbl) != MV_OK)
      {
        printf("..........................FAIL\n");
        printf("\nFlash write error 1 at sector start offset 0x%X\n", adrsIndex);
        return MV_ERROR;
      }

      /* read all sector and confirm data was written OK */
      for (startIndex = 0; startIndex<flash_params.page_size;startIndex+=4)
      {
        diagIfFlashRead(adrsIndex + startIndex, sizeof (MV_U32),
                                                                    &readValue);
        if (readValue != startIndex)
        {
          printf("..........................FAIL\n");
          printf ("\nFlash Memory Test error: address 0x%X ",
                                                                     adrsIndex);
          printf ("written value is 0x%X - read value is 0x%X\n",
                                                          adrsIndex, readValue);
        return MV_ERROR;
        }
      }
    }

    mvBspServicesProgressBar ((MV_U32)(one_percent * (progressCount++)),
                     countTotalSize, mvBspServicesProgressBarRunningEnum);
  }

  /* initiate test table with inversed data */
  for (adrsIndex = 0; adrsIndex < flash_params.page_size;
                                                  adrsIndex += sizeof (MV_U32))
    flash_adrs_value_tbl[adrsIndex / sizeof(MV_U32)] = ~adrsIndex;

  /* print 100 % */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                          mvBspServicesProgressBarEndEnum);

  printf ("\n\nSecond pattern test in progress...\n");
  printf ("\nErasing Flash sectors:     ");

  progressCount = 0;

  /* reset the percent function with flag param set to MV_OK */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                     mvBspServicesProgressBarStartEnum);

  /* erase all flash */
  for (adrsIndex = 0; adrsIndex < flash_params.tested_area_size; adrsIndex +=
                                              flash_params.page_size)
  {
    if (mvFlashCheckReservedOffsets(adrsIndex) == MV_OK)
      if (diagIfFlashErase(adrsIndex) != MV_OK)
      {
        printf("..........................FAIL\n");
        printf ("Flash Erase Error at offset 0x%X\n",adrsIndex);
        return MV_ERROR;
      }

    mvBspServicesProgressBar ((MV_U32)(one_percent * (progressCount++)),
                countTotalSize, mvBspServicesProgressBarRunningEnum);
  }

  /* print 100 % */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                       mvBspServicesProgressBarEndEnum);

  printf ("\n\nWriting and verifying:     ");

  progressCount = 0;

  /* reset the percent function with flag param set to MV_OK */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                     mvBspServicesProgressBarStartEnum);

  /* repeat all stages with inversed data */
  for (adrsIndex = 0x0; adrsIndex < flash_params.tested_area_size;
                                      adrsIndex += flash_params.page_size)
  {
    if (mvFlashCheckReservedOffsets(adrsIndex)==MV_OK)
    {
      if (diagIfFlashWrite(adrsIndex, flash_params.page_size,
                         flash_adrs_value_tbl) != MV_OK)
      {
        printf("..........................FAIL\n");
        printf
          ("\nFlash write error 2 at sector start offset 0x%X\n", adrsIndex);
        return MV_ERROR;
      }

      for (startIndex = 0; startIndex < flash_params.page_size;
                                                                startIndex += 4)
      {
        diagIfFlashRead(adrsIndex + startIndex, sizeof(MV_U32),
                                                                    &readValue);
        if (readValue != ~startIndex)
        {
          printf("..........................FAIL\n");
          printf
                ("\nFlash Memory Device Test error: At address 0x%X written",
                                                                     adrsIndex);
          printf (" value is 0x%X but read value is 0x%X\n",
                                                          adrsIndex, readValue);
          return MV_ERROR;
        }
      }
    }

    mvBspServicesProgressBar ((MV_U32)(one_percent * (progressCount++)),
                countTotalSize, mvBspServicesProgressBarRunningEnum);
  }

  /* print 100 % */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                       mvBspServicesProgressBarEndEnum);

  printf("..........................PASS\n");

  printf ("\n\nClearing Flash device:     ");

  progressCount = 0;

  /* reset the percent function with flag param set to MV_OK */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                     mvBspServicesProgressBarStartEnum);

  /* erase all flash */
  for (adrsIndex = 0; adrsIndex < flash_params.tested_area_size; adrsIndex +=
                                              flash_params.page_size)
  {
    if (mvFlashCheckReservedOffsets(adrsIndex) == MV_OK)
      if (diagIfFlashErase (adrsIndex) != MV_OK)
      {
          printf("..........................FAIL\n");
          printf ("Flash Erase Error at offset 0x%X\n",adrsIndex);
          return MV_ERROR;
      }

    mvBspServicesProgressBar ((MV_U32)(one_percent * (progressCount++)),
                countTotalSize, mvBspServicesProgressBarRunningEnum);
  }

  /* print 100 % */
  mvBspServicesProgressBar (progressCount, countTotalSize,
                                       mvBspServicesProgressBarEndEnum);
  printf("..........................PASS\n");

  mvBspServicesFreeMem((MV_U8 *)flash_adrs_value_tbl);

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspFlashAddrBusTest
*!
*$ GENERAL DESCRIPTION:
*!
*$ RETURNS:
*!
*$ ALGORITHM:
           The Address bus test purpose is to confirm that there is no
           overlapping in memory locations. This test check address
           combinations of "power-of-two" address, which is similar to the
           "walking 1's test".

           The steps are:
           1.  Initial all "power-of-two" addresses and address 0x00h with 0x55h.
           2.  Write 0x01h to the first offset.
           3.  Verify that the initial data value is still stored at every other
               "power-of-two" offset (that are bigger than tested offset).
           4.  If found a location that contains 0x01 there is a problem with
               the current address bit and software should display a message on
               screen: "Bits X1 and X2 FAILED the test". (Where X1 is the
               tested bit and X2 is the verified bit).
           5.  Write 0x01 to the next "power of-two" offset, verify.
           6.  Repeat step 5 until the MSB is set.

*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:  To write 0x01 on a 0x55 value, there is no need to erase the Flash
*!           sectors, due to the fact that writing to a Flash device decrements
*!           1's to 0's.
*!
*!           Erase, write and read operations must be done directly from
*!           P_HOST to avoid skipping sectors.
*!
*!**************************************************************************
*!*/
MV_STATUS mvBspFlashAddrBusTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
   MV_U32 adrsIndex, curIndex, startIndex, errorNum = 0;
   MV_U8 initValue, verifyValue, readValue;
   MV_U8 testValue;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  initValue   = MV_FLASH_INIT_VAL;
  verifyValue = MV_FLASH_VERIFY_VAL;
  startIndex  = MV_FLASH_START_INDEX;

  printf ("\n\nFLASH Address Bus Test");

  printf("\n\nFLASH Address Bus Test in progress.");
  printf(" Please wait.\n");

  /* erase first sector */
  if (diagIfFlashPageIsEmpty(startIndex) != MV_OK)
    if (diagIfFlashErase (startIndex) != MV_OK)
    {
      printf ("............................FAIL\n");
      printf ("\nFlash erase error 1.\n");
      return MV_ERROR;
    }

  /* erase all sectors that have multiply of two addresses in them */
  for (adrsIndex = startIndex; adrsIndex < flash_params.tested_area_size; adrsIndex<<=1)
  {
    if (mvFlashCheckReservedOffsets(adrsIndex))
      if (adrsIndex >= flash_params.page_size)
        if (diagIfFlashErase (adrsIndex) != MV_OK)
        {
          printf ("............................FAIL\n");
          printf ("\nFlash erase error 2.\n");
          return MV_ERROR;
        }
  }

  /* In order to check all the "HW arch flash assembly" we
        need to have a minimum of 8 bytes - (4 x 16b).
        checking addresses 0,1,2,3 ...7 */
  /* write 0x55 to the 8 first addresses */
  for (adrsIndex = 0; adrsIndex < 8; adrsIndex++)
  {
    if (mvFlashCheckReservedOffsets(adrsIndex))
    {
      if (diagIfFlashWrite (adrsIndex, sizeof (MV_U8), &initValue)
                                                != MV_OK)
      {
        printf ("............................FAIL\n");
        printf("\nFlash write error 1.\n");
        return MV_ERROR;
      }

      /* Verify the value written to the flash address */
      diagIfFlashRead (adrsIndex, sizeof (MV_U8),&testValue);
      if (testValue != initValue)
      {
        printf ("............................FAIL\n");
        printf("\nFlash write error to address offset 0x%X\n",
                                                                     adrsIndex);
        /* In order to check all the "HW arch flash assembly" we need to have
           a minimum of 8 bytes - (4 x 16b) */
        if (++errorNum == 8)
          return MV_ERROR;
      }
    }
  }

  errorNum = 0;

  /* write 0x55 to all multiply of two addresses */
  for (adrsIndex = startIndex; adrsIndex < flash_params.tested_area_size; adrsIndex<<=1)
  {
    if (mvFlashCheckReservedOffsets(adrsIndex))
    {
      if (diagIfFlashWrite (adrsIndex, sizeof (MV_U8), &initValue)
                                                != MV_OK)
      {
        printf ("............................FAIL\n");
        printf("\n Flash write error 2.\n");
        return MV_ERROR;
      }

      /* Verify the value written to the flash address */
      diagIfFlashRead (adrsIndex, sizeof (MV_U8),&testValue);
      if (testValue != initValue)
      {
        printf ("............................FAIL\n");
        printf("\nFlash write error to address offset 0x%X\n",
                                                                     adrsIndex);

        /* In order to check all the "HW arch flash assembly" we need to have
           a minimum of 8 bytes - (4 x 16b) */
        if (++errorNum == 8)
          return MV_ERROR;
      }
    }
  }

  for (curIndex = startIndex; curIndex<(flash_params.tested_area_size>>1); curIndex<<=1)
  {
    if (mvFlashCheckReservedOffsets(curIndex))
    {
      /* write 0x01 to current multiply of two address */
      if (diagIfFlashWrite (curIndex, sizeof (MV_U8), &verifyValue)
                                                != MV_OK)
      {
        printf ("............................FAIL\n");
        printf ("\nFlash write error 3.\n");
        return MV_ERROR;
      }

      /* read all multiply of two addresses above current one and make sure they
         still have initial value (0x55) and weren't overlap by previous writing */
      for (adrsIndex = curIndex<<1; adrsIndex<flash_params.tested_area_size; adrsIndex<<=1)
      {
        diagIfFlashRead (adrsIndex, sizeof (MV_U8), &readValue);

        if (readValue == MV_FLASH_VERIFY_VAL)
        {
          printf ("............................FAIL\n");
          printf ("Flash Address Bus Test Error: Writing 0x%X to",
                                                                   verifyValue);
          printf (" address 0x%X overlapped in address 0x%X.\n",
                                                           curIndex, adrsIndex);
          return MV_ERROR;
        }
      }
    }
  }

  printf ("............................PASS\n");

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspFlashDataBusTest
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
*!          1. Erasing the first Flash sector.
*!          2. Starting with value of 0x80 (bit 7 is set).
*!          3. Initialize the start offset into 0.
*!          4. Writing the start value a cast format related to
*!             the value of HOSTP_gdiag_flash_data_bus_width into the start offset.
*!          5. Make a right shift to the start value, increment the start value
*!             by HOSTP_gdiag_flash_data_bus_width and jumping to step 4.
*!          6. The start value is always incremented in order not to erase
*!             the Flash sector after each single step.
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:
*!
*!**************************************************************************
*!*/
MV_STATUS mvBspFlashDataBusTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 adrs, curOffset, baseOffset;
  MV_U16 flashIndex, widthIndex;
  MV_U8  data, verifyData, curData, dataBusWidth;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf ("\n\nFLASH data bus test");

  /* erase third sector (the test can be done on any flash sector, but sectors
     one & two are used for the address test and we want to decrease the number
     of time they are cleared) */
  baseOffset  = flash_params.page_size * 2;

  dataBusWidth = flash_params.data_bus_width;

  if (diagIfFlashErase (baseOffset) != MV_OK)
  {
    printf("............................FAIL\n");
    printf ("\nFlash erase error.\n");
    return MV_ERROR;
  }

  /* for each byte of the data bus width  */
  for (widthIndex = 0; widthIndex < dataBusWidth * 8; widthIndex +=dataBusWidth)
    /* for each 1 bit of the data bus width */
    for (data = 0x80, adrs = 0; data != 0; data >>= 1, adrs += dataBusWidth / 8)
      /* for each 1 bit of bytes amount in the data bus width */
      for (flashIndex = 0; flashIndex < dataBusWidth / 8; flashIndex++)
      {
        curOffset = widthIndex + flashIndex + adrs;

        if (flashIndex % 8 == widthIndex / dataBusWidth)
           curData = data;
        else
           curData = 0x0;

        if (diagIfFlashWrite (baseOffset + curOffset, 1, & curData) != MV_OK)
        {
          printf("............................FAIL\n");
          printf("\nFlash write error.\n");
          return MV_ERROR;
        }

        diagIfFlashRead (baseOffset + curOffset, 1, &verifyData);

        if (verifyData != curData)
        {
          printf ("............................FAIL\n");

          printf("\nData bus error at offset 0x%X.\n",
                                                                     curOffset);
          printf("Written val: 0x%X, Read val: 0x%X\n",
                                                              data, verifyData);
          return MV_ERROR;
        }
      }

  printf ("............................PASS\n");

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspFlashWriteByOffsetTest
*!
*$ GENERAL DESCRIPTION:  Write byte/short/word to the Flash by the user.
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
MV_STATUS mvBspFlashWriteByOffsetTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U8  data[MV_FLASH_SCAN_LEN];
  MV_U8  parse_data[MV_FLASH_SCAN_LEN];
  MV_U32 parseOffset, parse_data_size;
  BOOLEAN parse_result;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf("\n\nEnter offset: ");

  if (mvBspServicesInputHexNumber(&parseOffset) != MV_OK)
    return MV_ERROR;

  printf("Enter size in bytes (0x1 - 0x4): ");

  if (mvBspServicesInputHexNumber(&parse_data_size) != MV_OK)
    return MV_ERROR;

  if ((parse_data_size < 1) || (parse_data_size > MV_FLASH_SCAN_LEN))
  {
    printf ("\nError: Wrong size.\n\n");
    return MV_ERROR;
  }

  memset(data, '\0', MV_FLASH_SCAN_LEN);
  memset(parse_data, 0xFF, MV_FLASH_SCAN_LEN);

  printf ("Enter data in hex: ");
  mvBspServicesScanStr  (MV_FLASH_SCAN_LEN, data);
  printf ("\n");

  parse_result = mvBspServicesParseHexNumber(data, strlen(data),
                                                         (MV_U32 *)parse_data);
  if ( parse_result != MV_OK )
  {
    printf ("\nError: Wrong data.\n\n");
    return MV_ERROR;
  }

  if (mvFlashCheckReservedOffsets(parseOffset) != MV_OK)
  {
    printf ("Error: Writing to this sector is forbidden.\n\n");
    return MV_ERROR;
  }

  if (diagIfFlashWrite (parseOffset, parse_data_size, parse_data) != MV_OK)
  {
    printf ("Error: Write failed.\n\n");
    return MV_ERROR;
  }

  printf ("Data written successfully.\n\n");
  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspFlashReadByOffsetTest
*!
*$ GENERAL DESCRIPTION:  Read some bytes from the Flash by the user
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
MV_STATUS mvBspFlashReadByOffsetTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 parseOffset;
  MV_U8  data[MV_FLASH_SCAN_LEN];
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf("\n\nEnter offset in hex: ");

  if (mvBspServicesInputHexNumber(&parseOffset) != MV_OK)
    return MV_ERROR;

  memset(data, '\0', MV_FLASH_SCAN_LEN);

  /* read from flash & send to terminate */
  diagIfFlashRead (parseOffset, MV_FLASH_SCAN_LEN, data);

  mvBspServicesDisplayMemoryTable(data,MV_FLASH_SCAN_LEN,
                                                   sizeof(MV_U32),parseOffset);

  printf ("\n");

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspFlashEraseByOffsetTest
*!
*$ GENERAL DESCRIPTION:  Erase Flash sector by the user.
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
MV_STATUS mvBspFlashEraseByOffsetTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 parseOffset;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf("\n\nEnter offset: ");

  if (mvBspServicesInputHexNumber(&parseOffset) != MV_OK)
    return MV_ERROR;

  if (mvFlashCheckReservedOffsets(parseOffset) != MV_OK)
  {
    printf ("Error: Erasing this sector is forbidden.\n\n");
    return MV_ERROR;
  }

  if (diagIfFlashErase (parseOffset) != MV_OK)
  {
    printf ("Error: Erase failed.\n\n");
    return MV_ERROR;
  }

  printf ("Sector erased successfully.\n\n");
  return MV_OK;
}
/*$ END OF <module_name> */


/**********************************************************************************/


MV_STATUS flash_test(void)
{
MV_STATUS status = MV_OK;

    if( (MV_OK != mvBspFlashAddrBusTest()) ||
        (MV_OK != mvBspFlashDataBusTest()) ||
        (MV_OK != mvBspFlashMemoryDeviceTest()) )
        status = MV_ERROR;

    return status;
}


#endif  /* FLASH_TEST */
