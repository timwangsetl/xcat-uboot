/*  File Name:  private_diag_if.h  */

#ifndef __INCprivate_diag_ifh
#define __INCprivate_diag_ifh

#include "mvCommon.h"
#include "mvTypes.h"
#include "diag_services.h"

/* Defines */

#undef IN
#define IN
#undef OUT
#define OUT
#undef INOUT
#define INOUT

/*******************************************************************************
* hwIfPrintBoardInfo
*
* DESCRIPTION:
*       
********************************************************************************/
void hwIfPrintBoardInfo(void);
/* END OF <hwIfPrintBoardInfo> */


/*******************************************************************************
* diagIfInit
*
* DESCRIPTION:
*       
********************************************************************************/
void diagIfInit(void);
/* END OF <diagIfInit> */


/*******************************************************************************
* FUNCTION: diagIfDramInit
*
* DESCRIPTION:
*       Initilize the DRAM testing parameters.
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void diagIfDramInit(void);
/* END OF <diagIfDramInit> */



/*******************************************************************************
* diagIfSmiRead
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using SMI. 
*******************************************************************************/
MV_STATUS diagIfSmiRead
    (
        IN  MV_U32  devSlvId,
        IN  MV_U32  actSmiAddr,
        IN  MV_U32  regAddr,
        OUT MV_U32 *value
    );


/*******************************************************************************
* diagIfSmiWrite
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
********************************************************************************/
MV_STATUS diagIfSmiWrite
    (
        IN MV_U32 devSlvId,
        IN MV_U32 actSmiAddr,
        IN MV_U32 regAddr,
        IN MV_U32 value
    );




/*******************************************************************************
* diagIfI2cRx
*
* DESCRIPTION:
*       Recieve ........ using I2C.
********************************************************************************/
MV_STATUS   diagIfI2cRx(
    /*!   INPUTS:   */
    IN  MV_U8     slave_address,   /* the target device slave address on I2C bus */
    IN  MV_U32    buffer_size,     /* buffer length */
    IN  MV_U8     bus_id,          /* the I2C bus id */
    /*!   OUTPUTS:  */
    OUT MV_U8    *buffer           /* received buffer */
);



/*******************************************************************************
* diagIfI2cTx
*
* DESCRIPTION:
*       Transmit ........ using I2C.
********************************************************************************/
MV_STATUS diagIfI2cTx(
    /*!   INPUTS:   */
    IN  MV_U8     slave_address,   /* the target device slave address on I2C bus */
    IN  MV_U32    buffer_size,     /* buffer length */
    IN  MV_U8     bus_id,          /* the I2C bus id if only one bus then bus_id=0*/
    /*!   OUTPUTS:  */
    OUT MV_U8    *buffer           /* transmitted buffer */
    );




/* PCI configuration space read write */

/*******************************************************************************
* diagIfPciConfigRead
*
* DESCRIPTION:
*       Transmit ........ using PCI.
********************************************************************************/
MV_STATUS diagIfPciConfigRead (
                    MV_U32 pciIf,  
                    MV_U32 bus,                            
                    MV_U32 dev,                            
                    MV_U32 func,                           
                    MV_U32 regOff, 
                    MV_U32 *data
                    );                        



/*******************************************************************************
* diagIfPciConfigWrite
*
* DESCRIPTION:
*       Transmit ........ using PCI.
********************************************************************************/
MV_STATUS diagIfPciConfigWrite(
                    MV_U32 pciIf, 
                    MV_U32 bus,                              
                    MV_U32 dev,                              
                    MV_U32 func,                             
                    MV_U32 regOff,                            
                    MV_U32 data
                    );                            




/* Flash: Erase/Read/Write */

/*******************************************************************************
* diagIfFlashInit
*
* DESCRIPTION:
*       Initilize the FLASH testing parameters.
*
********************************************************************************/
void diagIfFlashInit( void );
/* END OF <mvBspFlashInit> */



/*******************************************************************************
* diagIfFlashUnprtect
*
* DESCRIPTION:
*       Unprtect the tested FLASH.
*
********************************************************************************/
void diagIfFlashUnprtect( void );
/* END OF <diagIfFlashUnprtect> */



/*******************************************************************************
* diagIfFlashErase
*
* DESCRIPTION:
*       Erase sector given by offset.
*
********************************************************************************/
MV_STATUS diagIfFlashErase(
    /*!   INPUTS:   */
    MV_U32  offset
    );



/*******************************************************************************
* diagIfFlashRead
*
* DESCRIPTION:
*       read amount of bytes from flash to given buffer.
*
********************************************************************************/
MV_STATUS diagIfFlashRead(
    /*!  INPUTS:     */
    MV_U32  offset,
    MV_U32  size,
    /*!  OUTPUTS:    */
     void   *dest_ptr
    );



/*******************************************************************************
* diagIfFlashWrite
*
* DESCRIPTION:
*       write amount of bytes to flash from given buffer.
*
********************************************************************************/
MV_STATUS diagIfFlashWrite (
      /*!   INPUTS:     */
      MV_U32       offset,
      MV_U32       size,
      void          *src_ptr
      );


 
/*******************************************************************************
* diagIfFlashPageIsEmpty
*
* DESCRIPTION:
*       Check if sector given by offset is empty.
*
********************************************************************************/
MV_STATUS diagIfFlashPageIsEmpty(
      /*  INPUTS:   */
      MV_U32    offset
      );
/* END OF <mvBspFlashPageIsEmpty> */





/* INSTRUCTION CACHE Enable/Disable */

MV_STATUS   diagIfIcacheEnable(MV_U8 arg);
MV_STATUS   diagIfIcacheDisable(MV_U8 arg);


/* DATA CACHE Enable/Disable */

MV_STATUS   diagIfDcacheEnable(MV_U8 arg);
MV_STATUS   diagIfDcacheDisable(MV_U8 arg);



/* Timer test interface functions */

/*******************************************************************************
* diagIfStartTimer
*
* DESCRIPTION:
*       
********************************************************************************/
MV_STATUS diagIfStartTimer(void);
/* END OF <diagIfStartTimer> */


/*******************************************************************************
* diagIfStopTimer
*
* DESCRIPTION:
*       
********************************************************************************/
MV_STATUS diagIfStopTimer(void);
/* END OF <diagIfStopTimer> */


/*******************************************************************************
* diagIfSetTimer
*
* DESCRIPTION:
*       
********************************************************************************/
MV_STATUS diagIfSetTimer(MV_U32 time);
/* END OF <diagIfSetTimer> */


/*******************************************************************************
* diagIfGetTimer
*
* DESCRIPTION:
*       
********************************************************************************/
MV_U32 diagIfGetTimer(void);
/* END OF <diagIfGetTimer> */





/* HW Watchdog test interface functions */

/*******************************************************************************
* diagIfWdInit
*
* DESCRIPTION:  Init WatchDog device.
*       
********************************************************************************/
void diagIfWdInit (void);
/* END OF <diagIfWdInit> */


/*******************************************************************************
* diagIfWdStart
*
* DESCRIPTION:  Enable the WatchDog device.
*       
********************************************************************************/
void diagIfWdStart(void);
/* END OF <diagIfWdStart> */


/*******************************************************************************
* diagIfWdStop
*
* DESCRIPTION:  Disable the WatchDog device.
*       
********************************************************************************/
void diagIfWdStop(void);
/* END OF <diagIfWdStop> */


/*******************************************************************************
* diagIfWdStrobeDevice
*
* DESCRIPTION:  Send strobe to WatchDog device to keep system alive.
*       
********************************************************************************/
void diagIfWdStrobeDevice(void);
/* END OF <diagIfWdStrobeDevice> */



/* SW Watchdog test interface functions */

/*******************************************************************************
* diagIfSoftRSTSet
*
* DESCRIPTION:  Mask & Enable the Software WatchDog registers.
*       
********************************************************************************/
void diagIfSoftRSTSet(void);
/* END OF <diagIfSoftRSTSet> */




/*  Interrupts lock/unlock interface functions:
 *  These functins are not nessesary for u-boot functionality,
 *  but must be implemented for other OS  
 */

/*******************************************************************************
* diagIfIntLock
*
* DESCRIPTION:  Interrupt Lock (disable interrupts).
*       
********************************************************************************/
void diagIfIntLock(MV_U32* intMask);
/* END OF <diagIfIntLock> */


/*******************************************************************************
* diagIfIntUnlock
*
* DESCRIPTION:  Interrupt Lock (disable interrupts).
*       
********************************************************************************/
void diagIfIntUnlock(MV_U32* intMask);
/* END OF <diagIfIntUnlock> */



#endif  /* __INCprivate_diag_ifh */

