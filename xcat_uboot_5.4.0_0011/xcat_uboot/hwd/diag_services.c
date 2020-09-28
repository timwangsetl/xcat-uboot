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

/*! General definitions */
/* #include         <mvDiagBsp/inc/defs.h> */
#include <common.h>
#include <config.h>
#include <exports.h>
#include "mvTypes.h"
#include "mvDramIf.h"

#include "diag_inf.h"
#include "diag_services.h"

/*!**************************************************************************
 *$              EXTERNAL DECLARATIONS (IMPORT AND EXPORT)
 *!**************************************************************************
 *!*/

/*!**************************************************************************
 *$              EXTERNAL DECLARATIONS (IMPORT)
 *!**************************************************************************
 *!*/
/* #include         <mvDiagBsp/inc/diag_inf.h> */

/* #include         <mvDiagBsp/app/diag_services.h> */
/* #include         <mvDiagBsp/app/diag_main.h> */
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


/*!**************************************************************************
 *$              LOCAL VARIABLE DEFINITIONS
 *!**************************************************************************
 *!*/

/*!**************************************************************************
 *$              LOCAL FUNCTION DEFINITIONS
 *!**************************************************************************
 *!*/

/*!**************************************************************************
 *$              PUBLIC FUNCTION DEFINITIONS (EXPORT)
 *!**************************************************************************
 *!*/
 
 
/*!**************************************************************************
*!
*$ FUNCTION: mvBspMemoryFill
*!
*$ GENERAL DESCRIPTION: Fill memory with data.
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
  MV_STATUS mvBspMemoryFill (
  MV_U32   addr,
  MV_U32   pattern,
  MV_U32   numOfBytes  
)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 i;
  MV_U32 TotalMemSize = 0x0;

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  /* Check for address and numOfBytes alignmenet. */
  if((addr % 4) | (numOfBytes % 4))
     return MV_ERROR;
  
  TotalMemSize = mvDramIfSizeGet();
  
  /* For debug. */
  /* printf("\n TotalMemSize = 0x%08x, numOfBytes = %d, addr = 0x%08x\n", TotalMemSize, numOfBytes, addr); */

  if((addr > TotalMemSize) || ((addr + numOfBytes) > TotalMemSize))
  {
    printf("\n addr > TotalMemSize\n");
    return MV_ERROR;
  }
	
  /* Set pattern to address. */
  for (i = 0; i < numOfBytes; i+=4)
  {
	*(volatile MV_U32 *)(addr + i) = pattern;
  }
  
  return MV_OK;
}  
 
/*!**************************************************************************
*!
*$ FUNCTION: mvBspServicesDisplayMemoryTable
*!
*$ GENERAL DESCRIPTION: Print memory as table by offset.
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
  void mvBspServicesDisplayMemoryTable (
  /*!     INPUTS:             */
  MV_U8  * addr,
  MV_U32   numOfBytes,
  MV_U32   width,
  MV_U32   virtAddress
  /*!     INPUTS / OUTPUTS:   */

  /*!     OUTPUTS:            */

)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  int      i;
  MV_U8  *charAddr;
  MV_U16 *shortAddr;
  MV_U32 *longAddr;
  MV_U8  char_array[mvServicesMaxTableLength];

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  if(numOfBytes > mvServicesMaxTableLength)
    numOfBytes = mvServicesMaxTableLength;

  /* remove non-printable characters */
  for (i = 0; i < numOfBytes; i++)
  {
    if (*(addr + i) < mvServicesFirstPrintChar)
      char_array[i] = '.';
    else
      char_array[i] = *(addr + i);
  }

  if (width == 1)
  {
    charAddr = char_array;
    printf("\n           0  1  2  3  4  5  6  7  ");

    for ( i = 0; i < numOfBytes; i += 8 )
    {
      printf("\n%-8.8X: ", virtAddress + i);
      printf
       ("%-2.2X %-2.2X %-2.2X %-2.2X %-2.2X %-2.2X %-2.2X %-2.2X   %c%c%c%c%c%c%c%c",
                    addr[0], addr[1], addr[2], addr[3],
                    addr[4], addr[5], addr[6], addr[7],
                    charAddr[0],charAddr[1],charAddr[2],charAddr[3],
                    charAddr[4],charAddr[5],charAddr[6],charAddr[7]);

      addr += 8;
      charAddr += 8;
    }
  }

  if (width == 2)
  {
    charAddr = char_array;
    shortAddr = (MV_U16 *)addr;
    printf("\n             0    2    4    6 ");

    for ( i = 0; i < numOfBytes; i += 8 )
    {
      printf("\n%-8.8X: ", virtAddress + i);
      printf("%-4.4X %-4.4X %-4.4X %-4.4X   %c%c%c%c%c%c%c%c",
                    shortAddr[0],shortAddr[1],shortAddr[2],shortAddr[3],
                    charAddr[0],charAddr[1],charAddr[2],charAddr[3],
                    charAddr[4],charAddr[5],charAddr[6],charAddr[7]);

      shortAddr += 4;
      charAddr += 8;
    }
  }

  if (width == 4)
  {
    charAddr = char_array;
    longAddr = (MV_U32 *)addr;
    printf("\n                 0        4        8        C ");

    for ( i = 0; i < numOfBytes; i += 16)
    {
      printf("\n%-8.8X: ", virtAddress + i);

      printf
            ("%-8.8X %-8.8X %-8.8X %-8.8X   %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c",
                    longAddr[0], longAddr[1], longAddr[2], longAddr[3],
                    charAddr[0], charAddr[1], charAddr[2], charAddr[3],
                    charAddr[4], charAddr[5], charAddr[6], charAddr[7],
                    charAddr[8], charAddr[9], charAddr[10], charAddr[11],
                    charAddr[12], charAddr[13], charAddr[14], charAddr[15]);

      longAddr += 4;
      charAddr += 16;
    }
  }

  printf("\n");

}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspServicesParseHexNumber
*!
*$ GENERAL DESCRIPTION:  convert string into hex number.
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
    MV_STATUS mvBspServicesParseHexNumber (
    /*!     INPUTS:             */
    MV_U8 *src_PTR,
    MV_U32 len,         /*!*/
    /*!     INPUTS / OUTPUTS:   */
            /*!*/
    /*!     OUTPUTS:            */
    MV_U32 *dst_PTR        /*!*/
)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U8 count, digit;
  MV_U32 factor;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  if (len == 0)
    return MV_ERROR;

  factor = 1;

  *dst_PTR = 0;

  if ((strncmp (src_PTR, "0x", 2) == 0) || (strncmp (src_PTR, "0X", 2) == 0))
  {
     len -= 2;
     src_PTR += 2;
  }

  for (count = 0; count < len - 1; count++, factor *= 16);

  for (count = 0; count < len; count++, factor /= 16)
  {
    if ((digit = MV_SERVICES_DIGIT_TO_HEX_MACRO(src_PTR[count])) == 'R')
       return MV_ERROR;

    *dst_PTR += digit * factor;
  }

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspServicesAllocMem
*!
*$ GENERAL DESCRIPTION: Allocate memory for tests
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
  MV_U8 *mvBspServicesAllocMem (
  /*!     INPUTS:             */
  MV_U32 size_to_alloc
  /*!     INPUTS / OUTPUTS:   */

  /*!     OUTPUTS:            */

)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U8 *mem_pointer;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  mem_pointer = (MV_U8 *)malloc(size_to_alloc);

  return (mem_pointer);

}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspServicesFreeMem
*!
*$ GENERAL DESCRIPTION: Free memory allocated with 'mvBspServicesAllocMem'.
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
  void mvBspServicesFreeMem (
  /*!     INPUTS:             */
  MV_U8* mem_pointer
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

  free(mem_pointer);

}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspServicesPrintf
*!
*$ GENERAL DESCRIPTION:  Formated output function.
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
  void mvBspServicesPrintf (
  /*!     INPUTS:             */
  const  char   *format,
  ...
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/
  MV_U8    buffer[mvServicesPrintfBufferLength];
  va_list   argptr;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/
 
  /* reset buffer */
  memset((void *)buffer, 0, mvServicesPrintfBufferLength);

  va_start(argptr, format);
  vsprintf(buffer, format, argptr);
  va_end  (argptr);

  if ( strlen(buffer) >= mvServicesPrintfBufferLength )
  {
    printf("\n\nmvBspServicesPrintf: text is too long");
    return;
  }

  printf(buffer);
}


/*!**************************************************************************
*!
*$ FUNCTION: mvBspServicesScanStr
*!
*$ GENERAL DESCRIPTION:  Scan string from terminal.
*!
*$ RETURNS:     void
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!**************************************************************************
*!*/
  void mvBspServicesScanStr (
  /*!     INPUTS:             */
  unsigned int        max_str_len,
  /*!     OUTPUTS:            */
  char                *str_PTR
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/
  MV_U32 count = 0;
  MV_U8  ch;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/

  str_PTR[0] = '\0';

  ch = (MV_U8)getc();

  /* Wating for  */
  while (count < max_str_len)
  {
    if((ch == '\n') || (ch == '\r'))
        break;

    if (ch == UBKSP)
    {
      if (count > 0)
      {
        printf("\b \b");
        count--;
      }
    }
    else
    {
      str_PTR[count++] = ch;
      printf("%c", ch);
    }

    ch = (MV_U8)getc();
  }
  str_PTR[count] = '\0';

}



/*!**************************************************************************
*!
*$ FUNCTION: mvBspServicesProgressBar
*!
*$ GENERAL DESCRIPTION:  This function calculate the progress of a loop
*!                       function given the total size to measure and the
*!                       current counter, for tracking purpose 'reset_progress'
*$                       variable signals if it's the first run or not.
*$ RETURNS:
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS: Cannot be called from more then one test simultaneously!
*!**************************************************************************
*!*/
  void mvBspServicesProgressBar (
  /*!     INPUTS:             */
  MV_U32 counter,
  MV_U32 totalSize,
  mvBspServicesProgressBarEnum progress
  /*!     INPUTS / OUTPUTS:   */

  /*!     OUTPUTS:            */
)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 totalCounter;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  if (progress == mvBspServicesProgressBarStartEnum)
  {
    printf("\b\b0%%");
    return;
  }

  if (progress == mvBspServicesProgressBarEndEnum)
  {
    printf("\b\b\b\b100%%");
    return;
  }

  if (progress != mvBspServicesProgressBarRunningEnum)
    return;

  if (counter % (totalSize / 100) == 0)
  {
    totalCounter = (counter / (totalSize / 100)) + 1;
    printf("\b");
    if (totalCounter < 10)
      printf("\b%d%%", totalCounter);
    else if (totalCounter < 100)
      printf("\b\b%d%%", totalCounter);
    else if (totalCounter == 100)
      printf("\b\b\b100%%");
  }
}

/*!**************************************************************************
*!
*$ FUNCTION: mvBspServicesInputHexNumber
*!
*$ GENERAL DESCRIPTION: Get hexadecimal number from user as string and convert it.
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
  MV_STATUS mvBspServicesInputHexNumber (
  /*!     INPUTS:             */
  MV_U32 *hex_number
  /*!     INPUTS / OUTPUTS:   */

  /*!     OUTPUTS:            */

)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U8  data[mvServicesHexNumberLength];
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  memset(data, '\0', mvServicesHexNumberLength);

  mvBspServicesScanStr (mvServicesHexNumberLength, data);
  printf("\n");

  if (mvBspServicesParseHexNumber(data, strlen(data), hex_number) == MV_ERROR)
  {
    printf ("\nError: Wrong Input.\n\n");
    return MV_ERROR;
  }

  return MV_OK;

}


/*$ END OF <D_Services> */


