/*  File Name:  diag_services.h  */

#ifndef __INCdiag_servicesh
#define __INCdiag_servicesh

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

  #define mvServicesPrintfBufferLength      256

  #define mvServicesHexNumberLength         12

  #define mvServicesFirstPrintChar          0x20

  #define mvServicesMaxTableLength          0x300


  #define MV_SERVICES_DIGIT_TO_HEX_MACRO(chr)\
        chr >= '0' && chr <= '9' ? chr - '0' :\
        chr >= 'a' && chr <= 'f' ? chr - 'a' + 10 :\
        chr >= 'A' && chr <= 'F' ? chr - 'A' + 10 : 'R'   /* R for error */


  #define       UCR         0x0D
  #define       UBKSP       0x08

 
    typedef  enum
    {
    mvBspServicesProgressBarStartEnum,
    mvBspServicesProgressBarEndEnum,
    mvBspServicesProgressBarRunningEnum
    } mvBspServicesProgressBarEnum;



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
 *!
 */
    

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
  MV_U32   patern,
  MV_U32   numOfBytes  
);
   
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
                      );

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
                /*!     OUTPUTS:            */
                MV_U32 *dst_PTR        /*!*/
                );



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
                  );



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
                  );



#if 1
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
              );

#endif  /* #if 1 */


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
                  );



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
                  );



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
                  );




#endif  /* __INCdiag_servicesh */

