/*  File Name:  private_diag_if.c  */

#include <common.h>
#include <command.h>

#include "mvCommon.h"
#include "mvTypes.h"
#include "diag_inf.h"
#include "mvUartLog.h"
#include "mvGenUartLogger.h"

#include "private_test_config.h"
#include "private_diag_if.h"
#include "../include/configs/mv_kw.h"
 
#include "mvEthPhy.h"
#include "mvTwsi.h"
#include "mvPciIf.h"
#include "mvPci.h"
#include "mvPex.h"


  /* CACHE Enable/Disable flags */
  static MV_U8 en_icache_flag = MV_FALSE;
  static MV_U8 en_dcache_flag = MV_FALSE;

  /* #define CFG_MAX_FLASH_BANKS 1 */

  extern flash_info_t flash_info[/*CFG_MAX_FLASH_BANKS*/];    /* info for FLASH chips */
  mvBspDiagFlashTestParamStruct flash_params;
  mvBspDiagDramTestParamStruct dram_params;
  extern MV_U32 mvBspDramSizeLocal; 

  /* I2C devices data array - address of each device on the board and
     index if it's more then one device on same address */
	 
  mvBsp_i2c_devicesStruct diagIfI2cTbl[DIAG_I2C_NUM_OF_DEVICES] =
  {
    {DIAG_I2C_SFP_MUX_PCA9544A_ADDR   ,   DIAG_I2C_SFP1_INDEX },
    {DIAG_I2C_SFP_MUX_PCA9544A_ADDR   ,   DIAG_I2C_SFP2_INDEX },
    {DIAG_I2C_SFP_MUX_PCA9544A_ADDR   ,   DIAG_I2C_SFP3_INDEX },
    {DIAG_I2C_SFP_MUX_PCA9544A_ADDR   ,   DIAG_I2C_SFP4_INDEX },
    {DIAG_I2C_TEMP_SENSE_LM96000_ADDR ,   0                   },
    {DIAG_I2C_GPIO_EXTENDER_PCA9555_ADDR, 0                   },
    {DIAG_I2C_POE_CONTR_PD63000_ADDR  ,   0                   }
  };  


  extern MV_STATUS mvBspDramTestProbe(MV_U32 *params);    /* function from dram_test.c */
  extern void sysCpldInit(void);
  extern void icache_enable (void);
  extern void icache_disable (void);
  extern void dcache_disable (void);

  /* Global Malloc pointer for U-boot function tests. */
  volatile MV_U8 *mvUTMemAlloc_PTR;	
	
  #define MV_UT_MAX_ALLOC_SIZE  1024   /* Allcate size is 1KB */


/*******************************************************************************
* FUNCTION: hwIfPrintBoardInfo
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void hwIfPrintBoardInfo(void)
{   
    printf ("\n\n\n");

    printf ("UBOOT HARDWARE DIAGNISTICS MODULE FOR:\n\n"
            "ATI8000SG MV88F5181 BOARD: ???????????????? LE \n"
            "********************************************************   \n\n");
    printf ("HW SPEC. DESIGNED INFORMATION:\n"
            "==============================\n");

    printf ("Soc: MV88F5181 Rev 3\n"
            "CPU: ARM926 (Rev 0) running @ 400Mhz\n"
            "SysClock = 200Mhz , TClock = 166Mhz\n"
            "Net:   SK98#0 [PRIME]\n\n\n");

    printf ("Memory Address Mappings (Design):\n"
            "---------------------------------\n");

    printf ("               Start       End         Size    Target          Port Size\n"
            "Component      Addr.       Addr.      (Bytes)  Interface       (Bits)\n"
            "------------------------------------------------------------------------------\n");
    printf ("DDR SDRAM      0x00000000  0x07FFFFFF  128MB   DDR_CS0         32  \n"
            "FLASH          0xFF000000  0xFFFFFFFF   16MB   BOOT_DEV_CS     16  \n");
/*
    printf ("?????????????? Packet Processors:  Vendor Id = 0x11ab ???   Device Id = 0xd91c ???\n"
            "--------------------------\n"
            "PP1            0x???????????  0x???????????  ??MB \n"
            "PP2            0x?????????  0x????????  ??MB    \n\n\n");
*/
}


/*******************************************************************************
* FUNCTION: diagIfInit
*
* DESCRIPTION:
*
* INPUT:
*
* OUTPUT:
*       None.
*
* RETURN:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void diagIfInit(void)
{
    static MV_U8   one_run_flag = 0;
    if(! one_run_flag)
    {
        diagIfDramInit();
        diagIfFlashInit();

        one_run_flag = 1;
    }
    else
        diagIfFlashUnprtect();
		
	/* Allocate memory for U-boot commands tests. */
	mvUTMemAlloc_PTR = (MV_U8 *)mvBspServicesAllocMem(MV_UT_MAX_ALLOC_SIZE);
	
	if (mvUTMemAlloc_PTR == NULL)
    {
        printf("Fail to allocate memory!\n");
        return;
    }

	/* Initialze the Uart Logger function. */	
    if (mvUartLogInit(0) != MV_OK)
    {
        mvOsPrintf("\n\n %s: mvUartLogInit failed.\n", __func__);
    }

	/* Register the Uart Logger function. */
    if (mvUartLogReg(0, mvUtUartLogger) != MV_OK)
    {
        mvOsPrintf("\n\n %s: mvUartLogReg failed.\n", __func__);
    }	
}

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

void diagIfDramInit(void)
{
/*#ifdef DRAM_TEST*/
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  mvBspDramSizeLocal = 0;

  dram_params.base_address      = (MV_U32)BASE_ADDRESS      ;   /* dram base address */                    /* MV_U32  */                  
  dram_params.first_size_offset = (MV_U32)FIRST_SIZE_OFFSET ;   /* offset from base_address */             /* MV_U32  */                  
  dram_params.dev_amount        = (MV_U8) DEV_AMOUNT        ;   /* dram amount of devices */               /* MV_U8   */                  
  dram_params.data_bus_width    = (MV_U8) DRAM_DATA_BUS_WIDTH;  /* dram bus width */                       /* MV_U8   */                  
  dram_params.mem_size          = (MV_U32)MEM_SIZE          ;   /* memory size for dram test: */           /* MV_U32  */                  
                                                                /* if 0 - apply size detect algorithm */   /* mvBspDiagBooleanParamFNC* */
  dram_params.mem_probe_func    = (mvBspDiagBooleanParamFNC*)mvBspDramTestProbe;   /* memory probe function */                                               
  
/*  #endif    */
 }
/* END OF <diagIfDramInit> */





/*******************************************************************************
* FUNCTION: diagIfSmiRead
*
* DESCRIPTION:
*       Reads a register from SMI slave.
*
* INPUTS:
*       devSlvId - Slave Device ID
*      actSmiAddr - actual smi addr to use (relevant for SX PPs)
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       valuePtr     - Data read from register.
*
* RETURNS:
*       MV_OK      - on success
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS diagIfSmiRead(
        IN  MV_U32  devSlvId,
        IN  MV_U32  actSmiAddr,
        IN  MV_U32  regAddr,
        OUT MV_U32 *value
    )
{
     mvEthPhyRegRead(devSlvId, regAddr, (MV_U16 *)value);

     return MV_OK;
}


/*******************************************************************************
* FUNCTION: diagIfSmiWrite
*
* DESCRIPTION:
*       Writes a register to an SMI slave.
*
* INPUTS:
*       devSlvId - Slave Device ID
*      actSmiAddr - actual smi addr to use (relevant for SX PPs)
*       regAddr - Register address to read from.
*       value   - data to be written.
*
* OUTPUTS:
*        None,
*
* RETURNS:
*       MV_OK      - on success
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS diagIfSmiWrite(
        IN MV_U32 devSlvId,
        IN MV_U32 actSmiAddr,
        IN MV_U32 regAddr,
        IN MV_U32 value
    )
{
    MV_STATUS ret;

    ret = mvEthPhyRegWrite(devSlvId, regAddr, (MV_U16)value);

    return ret;
}



/*
*!**************************************************************************
*!
*$ FUNCTION: diagIfI2cRx
*!
*$ GENERAL DESCRIPTION: Sequential Current Address Read by I2C.
*!                      The receive is done from random offset,
*!                      unless adjusting the start reading offset
*!                      by the HOSTG_i2c_transmit function.
*!
*$ RETURNS:
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     
*!
*!**************************************************************************
*/
MV_STATUS   diagIfI2cRx(
    /*!   INPUTS:             */
    IN  MV_U8     slave_address,   /* the target device slave address on I2C bus */
    IN  MV_U32    buffer_size,     /* buffer length */
    IN  MV_U8     bus_id,          /* the I2C bus id */
    /*!   OUTPUTS:            */
    OUT MV_U8    *buffer           /* received buffer */
    )
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
    MV_TWSI_SLAVE twsiSlave;
	MV_U8 chanNum = 0;
  
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

    twsiSlave.slaveAddr.address = (MV_U32)slave_address;
    twsiSlave.slaveAddr.type = ADDR7_BIT;
    twsiSlave.validOffset = MV_FALSE; /* app changes offset autonomously */
    twsiSlave.offset = (MV_U32)0x0;
    twsiSlave.moreThen256 = MV_FALSE;
 
/* Ry @05302007 */
    if(bus_id)
    {
       bus_id += 0x03; /* Channel selection for PCA9544 I2C MUX */
       if ( MV_OK != mvTwsiWrite(chanNum, &twsiSlave, &bus_id, 1))
           return MV_ERROR;
    }

    if(0 != mvTwsiRead (chanNum, &twsiSlave, buffer, buffer_size))
        return MV_ERROR;

    return MV_OK;
}
/*$ END OF HOSTL_i2c_receive */




/*
*!**************************************************************************
 *!
 *$ FUNCTION: diagIfI2cTx
 *!
 *$ GENERAL DESCRIPTION:  Page Write by I2C.
 *!                       The first byte held at the buffer pointer is the
 *!     start writing offset. If the buffer_size is 1, it is not written into
 *!     the device, it's only setting the start offset for the reading
 *!     by HOSTG_i2c_receive.
 *!
 *$ RETURNS:
 *!       MV_OK      - on success
 *!       MV_ERROR   - on hardware error
 *!
 *! 
 *!
 *$ ALGORITHM:   (local)
 *!
 *$ ASSUMPTIONS:
 *!
 *$ COMMENTS:     (local)
 *!
 *!**************************************************************************
 */
MV_STATUS diagIfI2cTx(
    /*!   INPUTS:   */
    IN  MV_U8     slave_address,   /* the target device slave address on I2C bus */
    IN  MV_U32    buffer_size,     /* buffer length */
    IN  MV_U8     bus_id,          /* the I2C bus id if only one bus then bus_id=0*/
    /*!   OUTPUTS:  */
    OUT MV_U8    *buffer           /* transmitted buffer */
    )
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
    MV_TWSI_SLAVE twsiSlave;
	MV_U8 chanNum = 0;	
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/

    twsiSlave.slaveAddr.address = (MV_U32)slave_address;
    twsiSlave.slaveAddr.type = ADDR7_BIT;
    twsiSlave.validOffset = MV_FALSE;
    twsiSlave.offset = (MV_U32)0x0;
    twsiSlave.moreThen256 = MV_FALSE;
 
/* Ry @05302007 */
    if(bus_id)
    {
       bus_id += 0x03; /* Channel selection for PCA9544 I2C MUX */  
       if ( MV_OK != mvTwsiWrite(chanNum, &twsiSlave, &bus_id, 1))   
           return MV_ERROR;
    }	
	
    if ( MV_OK != mvTwsiWrite(chanNum, &twsiSlave, buffer, buffer_size))   
         return MV_ERROR;
 
    return MV_OK;
}
/*$ END OF HOSTL_i2c_transmit */






/* PCI configuration space read write */

/*******************************************************************************
* FUNCTION: diagIfPciConfigRead - Read from configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit read from PCI configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions 
*       (local and over bridge). In order to read from local bus segment, use 
*       bus number retrieved from mvPciLocalBusNumGet(). Other bus numbers 
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pciIf   - PCI interface number.
*       bus     - PCI segment bus number.
*       dev     - PCI device number.
*       func    - Function number.
*       regOffs - Register offset.       
*
* OUTPUT:
*       32bit register data, 0xffffffff on error.
*
* RETURN:
*       MV_OK      - on success        
*       MV_ERROR   - on hardware error
* 
* COMMENTS:
*
*******************************************************************************/
MV_STATUS diagIfPciConfigRead (MV_U32 pciIf, MV_U32 bus, MV_U32 dev,
                               MV_U32 func, MV_U32 regOff, MV_U32 *data)
{

    *data = mvPciIfConfigRead(pciIf, bus, dev, func, regOff);
        return MV_OK;

/*  mvOsPrintf("%s: ERROR!!! Invalid pciIf %d\n", __FUNCTION__, pciIf);
        return 0; */
}

/*******************************************************************************
* FUNCTION: diagIfPciConfigWrite - Write to configuration space
*
* DESCRIPTION:
*       This function performs a 32 bit write to PCI configuration space.
*       It supports both type 0 and type 1 of Configuration Transactions 
*       (local and over bridge). In order to write to local bus segment, use 
*       bus number retrieved from mvPciLocalBusNumGet(). Other bus numbers 
*       will result configuration transaction of type 1 (over bridge).
*
* INPUT:
*       pciIf   - PCI interface number.
*       bus     - PCI segment bus number.
*       dev     - PCI device number.
*       func    - Function number.
*       regOffs - Register offset.       
*       data    - 32bit data.
*
* OUTPUT:
*       None.
*
* RETURN:
*       MV_OK - on success
*       MV_BAD_PARAM for bad parameters 
*       MV_ERROR on error  otherwise MV_OK
*
* 
* COMMENTS:
*
*******************************************************************************/
MV_STATUS diagIfPciConfigWrite(MV_U32 pciIf, MV_U32 bus, MV_U32 dev, 
                               MV_U32 func, MV_U32 regOff, MV_U32 data)
{

    return  mvPciIfConfigWrite(pciIf, bus, dev, func, regOff, data);
}



/* Flash Erase/Read/Write */


/*******************************************************************************
* FUNCTION: diagIfFlashInit
*
* DESCRIPTION:
*       Initilize the FLASH testing parameters.
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
void diagIfFlashInit(void)
{
#ifdef  FLASH_TEST
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/

/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  flash_params.base_addr = (MV_U32)FLASH_BASE_ADDR;             /* device base address */                      
  flash_params.page_size = (MV_U32)PAGE_SIZE;                   /* device page size */                         
  flash_params.overall_size = (MV_U32)OVERALL_SIZE;             /* device overall size */                      
  flash_params.tested_area_size = (MV_U32)TESTED_AREA_SIZE;     /* device tested size */                       
  flash_params.data_bus_width = (MV_U32)DATA_BUS_WIDTH;         /* device bus width */                         
  flash_params.serial_dev_amount = (MV_U32)SERIAL_DEV_AMOUNT;   /* amount of devices in serial configuration */
  flash_params.reserved_offsets[0] = (MV_U32)RESERVED_OFFSETS_0;/* offsets to not be deleted (boots area) */   
  flash_params.reserved_offsets[1] = (MV_U32)RESERVED_OFFSETS_1;                                               
  flash_params.reserved_offsets[2] = (MV_U32)RESERVED_OFFSETS_2;                                                
  flash_params.reserved_sizes[0] = (MV_U32)RESERVED_SIZES_0;    /* The size of each reserved area */           
  flash_params.reserved_sizes[1] = (MV_U32)RESERVED_SIZES_1;  
  flash_params.reserved_sizes[2] = (MV_U32)RESERVED_SIZES_2;

  diagIfFlashUnprtect();

#endif  /*  */
}
/* END OF <diagIfFlashInit> */



/*******************************************************************************
* FUNCTION: diagIfFlashUnprtect
*
* DESCRIPTION:
*       Unprtect the tested FLASH.
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
void diagIfFlashUnprtect(void)
{
#ifdef  FLASH_TEST
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/
  flash_info_t *info;
  ulong bank;
  int i, protect_flag, rcode = 0;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

#ifdef UBOOT_FLASH_TEST_DEBUG
  for (bank=1; bank<=CFG_MAX_FLASH_BANKS; ++bank)
  {
      info = &flash_info[bank-1];
      if (info->flash_id == FLASH_UNKNOWN) {
          printf("\nError: Unknown flash ID.\n");
          continue;
      }
  
      printf ("Un-Protect Flash Bank # %ld ...\n", bank);
      flash_protect (FLAG_PROTECT_CLEAR,
                     info->start[0],
                     info->start[0] + info->size - 1,
                     &flash_info[bank-1]); 
      /*
      flash_protect (FLAG_PROTECT_CLEAR,
                     FLASH_BASE_ADDR,
                     FLASH_BASE_ADDR + OVERALL_SIZE - 1,
                     &flash_info[bank-1]); */
  }
  puts ("done\n\n");
#endif

  info = &flash_info[1];
  
  for (bank=1; bank<=CFG_MAX_FLASH_BANKS; ++bank)
  {
      info = &flash_info[bank-1];
      if (info->flash_id == FLASH_UNKNOWN) {
          printf("\nError: Unknown flash ID.\n");
          continue;
      }

      protect_flag = 0;
      printf ("\nUn-Protect Flash Bank # %ld ", bank);
      for (i=0; i<info->sector_count; ++i) 
      {
          #if defined(CFG_FLASH_PROTECTION)
          if (flash_real_protect(info, i, protect_flag))
              rcode = 1;
          putc ('.');
          #else
          info->protect[i] = protect_flag;
          #endif	/* CFG_FLASH_PROTECTION */
      }
  }

#if defined(CFG_FLASH_PROTECTION)
		if (!rcode) puts ("\n...... done\n\n");
#endif	/* CFG_FLASH_PROTECTION */

 

#endif  /*  */
}
/* END OF <diagIfFlashUnprtect> */


/*******************************************************************************
* FUNCTION: diagIfFlashErase
*
* DESCRIPTION:
*       Erase sector given by offset.
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       MV_OK      - on success (if sector erased successfully)
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS diagIfFlashErase(
    /*!   INPUTS:   */
    MV_U32  offset
    )
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/
  MV_U32 sectorNumber;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/
  
  if (offset >= flash_params.tested_area_size)
    return MV_ERROR;

  sectorNumber = (offset / flash_params.page_size);

  /* Erasing only 1 sector */
  return ( (MV_STATUS) flash_erase(&flash_info[1], sectorNumber, sectorNumber) );

/*   return ( FlashEraseSector(sectorNumber) ); */  /* the old func */
}
/* END OF <diagIfFlashErase> */



/*******************************************************************************
* FUNCTION: diagIfFlashRead
*
* DESCRIPTION:
*       read amount of bytes from flash to given buffer.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       MV_OK      - on success
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS diagIfFlashRead(
    /*!  INPUTS:     */
    MV_U32  offset,
    MV_U32  size,
    /*!  OUTPUTS:    */
     void   *dest_ptr
    )
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/
  MV_U8 *pos;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!*************************************************************************/
  
  if (offset + size > flash_params.tested_area_size)
    return MV_ERROR;

  for (pos = (MV_U8 *)(offset + flash_params.base_addr);
       pos < (MV_U8 *)(offset + flash_params.base_addr + size);
       pos++)
  {
      *((MV_U8*)dest_ptr) = *pos;
      dest_ptr++;

  }

    return MV_OK;
}



/*******************************************************************************
* FUNCTION: diagIfFlashWrite
*
* DESCRIPTION:
*       write amount of bytes to flash from given buffer.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       MV_OK      - on success (if data writen to flash successfully)
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS diagIfFlashWrite (
      /*!   INPUTS:     */
      MV_U32       offset,
      MV_U32       size,
      void         *src_ptr
      )
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N*/
/*!*************************************************************************/
  /* MV_U32 actualWritten; */
  MV_STATUS status;
  MV_U8 *pos;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/
  
printf("diagIfFlashWrite: offset=0x%X size=0x%X\n", offset, size);

  if (offset + size > flash_params.tested_area_size)
    return MV_ERROR;
/*
    actualWritten = FlashWriteBlock( offset, size, (char *)src_PTR);
    if ( actualWritten != size )
*/

  for (pos = (MV_U8 *)(flash_params.base_addr + offset);
       pos < (MV_U8 *)(flash_params.base_addr + offset + size);
       pos++)
  {
    if (*pos != (MV_U8)MV_FLASH_EMPTY_32BIT)
    {
      printf("The area to Write into is NOT EMPTY... Please Erase before Writing !\n");
      return MV_ERROR;
    }
  }

printf("flash_write: offset=0x%X size=0x%X\n", flash_params.base_addr + offset, size);

  status = flash_write( (uchar *)src_ptr, 
                        flash_params.base_addr + offset, 
                        size );
/*   if( status != MV_OK ) */
      /* return MV_ERROR;        */

  return status;  
}
/* END OF <diagIfFlashWrite> */



/*******************************************************************************
* FUNCTION: diagIfFlashPageIsEmpty
*
* DESCRIPTION:
*       Check if sector given by offset is empty.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       MV_OK      - on success (if Empty)
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS diagIfFlashPageIsEmpty(
    /*!     INPUTS:             */
  MV_U32     offset
    /*!     INPUTS / OUTPUTS:   */
    /*!     OUTPUTS:            */
)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 *pos;
  MV_U32 sectorNumber;
  MV_U32 sectorOffset;
/*!****************************************************************************/
/*!                      F U N C T I O N   L O G I C                          */
/*!****************************************************************************/
  
  if ((offset + flash_params.page_size) > flash_params.tested_area_size)
    return MV_ERROR;

  sectorNumber = (offset / flash_params.page_size);
  sectorOffset = sectorNumber * flash_params.page_size;
  for (pos = (MV_U32 *)(flash_params.base_addr + sectorOffset);
       pos < (MV_U32 *)(flash_params.base_addr + sectorOffset + flash_params.page_size);
       pos++)
  {
    if (*pos != (MV_U32)MV_FLASH_EMPTY_32BIT)
    {
      printf("The Checked Page is NOT EMPTY !\n");
      return MV_ERROR;
    }
  }
  
  return MV_OK;
}
/* END OF <diagIfFlashPageIsEmpty> */




/* INSTRUCTION CACHE Enable/Disable */

MV_STATUS   diagIfIcacheEnable(MV_U8 arg)
{
    if (en_icache_flag == MV_FALSE)
    {
        icache_enable();
        en_icache_flag = MV_TRUE;
    }

    return MV_OK;
}


MV_STATUS   diagIfIcacheDisable(MV_U8 arg)
{
    if (en_icache_flag == MV_TRUE)
    {
        icache_disable();
        en_icache_flag = MV_FALSE;
    }

    return MV_OK;
}


/* DATA CACHE Enable/Disable */

MV_STATUS   diagIfDcacheEnable(MV_U8 arg)
{
return MV_ERROR; /*for some reason, the function 'dcache_enable' is not implemented  for the926ejs. TBD. AdiM 22/8/06*/    
    
    if (en_dcache_flag == MV_FALSE)
    {
    /*dcache_enable();*/
    en_dcache_flag = MV_TRUE;
    }

    return MV_OK;
}


MV_STATUS   diagIfDcacheDisable(MV_U8 arg)
{
    if (en_dcache_flag == MV_TRUE)
    {
        dcache_disable();
        en_dcache_flag = MV_FALSE;
    }

    return MV_OK;
}



/* Timer test interface functions */

MV_STATUS diagIfStartTimer(void)
{

  return MV_OK;
}


MV_STATUS diagIfStopTimer(void)
{

  return MV_OK;
}


MV_STATUS diagIfSetTimer(MV_U32 time)
{

  return MV_OK;
}


MV_U32  diagIfGetTimer(void)
{
  MV_U32 time = 0;	

  return time;
}



/* HW Watchdog test interface functions */

/*!**************************************************************************
*!
*$ FUNCTION: HOSTL_wd_init
*!
*$ GENERAL DESCRIPTION: Init WatchDog device.
*!
*$ RETURNS: WD Interval Delay
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!***************************************************************************
*!*/
void diagIfWdInit(void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  volatile MV_U32 * ptr;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  /* Setting the WD Timer Reload Reg value */
  ptr = (MV_U32 *)CPU_WD_TIMER_RELOAD_REG_CNS;
  *ptr = (MV_U32)(0xFFFFFFF);

  /* Setting the WD mask Reg value */
  ptr = (MV_U32 *)WD_MASK_REG_CNS;  
  *ptr = (MV_U32) (WD_DEFAULT_MASK_CNS);  
}
/* END OF <diagIfWdInit> */


/*!**************************************************************************
*!
*$ FUNCTION: diagIfWdStart
*!
*$ GENERAL DESCRIPTION: Enable the WatchDog device.
*!
*$ RETURNS: void
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!***************************************************************************
*!*/
void diagIfWdStart(void)
{
/*!***************************************************************************/
/*! L O C A L   D E C L A R A T I O N S  A N D  I N I T I A L I Z A T I O N  */
/*!***************************************************************************/
  volatile MV_U32 * ptr;
  MV_U32 ctrl_reg_tmp_val;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  /* Setting the cpu timers control reg. value to enable the WD */
  ptr = (MV_U32 *)CPU_TIMERS_CONTROL_REG_CNS;
  ctrl_reg_tmp_val = *ptr;
  *ptr = (ctrl_reg_tmp_val | CPU_WD_TIMER_EN_CNS);
}
/* END OF <diagIfWdStart> */


/*!**************************************************************************
*!
*$ FUNCTION: diagIfWdStop
*!
*$ GENERAL DESCRIPTION: Disable the WatchDog device.
*!
*$ RETURNS: void
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!**************************************************************************
*!*/
void diagIfWdStop(void)
{
/*!***************************************************************************/
/*! L O C A L   D E C L A R A T I O N S  A N D  I N I T I A L I Z A T I O N  */
/*!***************************************************************************/
  volatile MV_U32 * ptr;
  MV_U32 ctrl_reg_tmp_val;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  /* Setting the cpu timers control reg. value to disable the WD */
  ptr = (MV_U32 *)CPU_TIMERS_CONTROL_REG_CNS;
  ctrl_reg_tmp_val = *ptr;
  *ptr = ( ctrl_reg_tmp_val & 0xFFFFFFEF );   /* (~CPU_WD_TIMER_EN_CNS) */

  /* Masking the mask reg. to disable RSTOUT */
  ptr = (MV_U32 *)WD_MASK_REG_CNS;
  *ptr = (MV_U32) 0x0;      
}
/* END OF <diagIfWdStop> */


/*!**************************************************************************
*!
*$ FUNCTION: diagIfWdStrobeDevice
*!
*$ GENERAL DESCRIPTION: Send strobe to WatchDog device to keep system alive.
*!
*$ RETURNS: void
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!***************************************************************************
*!*/
void diagIfWdStrobeDevice(void)
{
/*!**************************************************************************/
/*! L O C A L   D E C L A R A T I O N S  A N D  I N I T I A L I Z A T I O N */
/*!**************************************************************************/
  volatile MV_U32 * ptr;
  MV_U32 ctrl_reg_tmp_val;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  /* Clearing the WD timer */
  /* ptr = (MV_U32 *) WD_MASK_REG_CNS; */
  /* *ptr = (MV_U32) WD_CLEAR_BIT_CNS; */

   /* Setting the cpu timers control reg. value to enable the WD */
  ptr = (MV_U32 *)CPU_TIMERS_CONTROL_REG_CNS;
  ctrl_reg_tmp_val = *ptr;
  *ptr = (ctrl_reg_tmp_val | CPU_WD_TIMER_EN_CNS);
}
/*$ END OF  <diagIfWdStrobeDevice> */



/* SW Watchdog test interface functions */

/*!**************************************************************************
*!
*$ FUNCTION: diagIfSoftRSTSet
*!
*$ GENERAL DESCRIPTION: Send strobe to WatchDog device to keep system alive.
*!
*$ RETURNS: void
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!***************************************************************************
*!*/
void diagIfSoftRSTSet(void)
{
/*!**************************************************************************/
/*! L O C A L   D E C L A R A T I O N S  A N D  I N I T I A L I Z A T I O N */
/*!**************************************************************************/

    /* volatile MV_U32 * ptr; */
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/
    /*
    ptr = (MV_U32 *) WD_MASK_REG_CNS; 
    *ptr = (MV_U32) SW_RST_MASK_BIT_CNS; 

    ptr = (MV_U32 *) SW_RST_EN_REG_CNS;  
    *ptr = (MV_U32) SW_RST_EN_BIT_CNS;
    */
         
        #define CRMR_SOFT_RST_OUT_OFFS          2
        #define CRMR_SOFT_RST_OUT_MASK          BIT2
        #define CRMR_SOFT_RST_OUT_ENABLE        (1 << CRMR_SOFT_RST_OUT_OFFS)
        #define CRMR_SOFT_RST_OUT_DISBALE       (0 << CRMR_SOFT_RST_OUT_OFFS)

        #define CSSRR_SYSTEM_SOFT_RST           BIT0
        
        /* System Software Reset Register (CSSRR)*/
        #define CPU_SYS_SOFT_RST_REG         0x2010C 
        #define CPU_RSTOUTN_MASK_REG         0x20108 

        /* Enable SoftRstOutEn in RSTOUTn Mask Register */
    	MV_REG_WRITE (CPU_RSTOUTN_MASK_REG, CRMR_SOFT_RST_OUT_ENABLE); 
    	/* Set SystemSoftRst signal */
    	MV_REG_WRITE (CPU_SYS_SOFT_RST_REG, CSSRR_SYSTEM_SOFT_RST); 

        while (1); 
}
/*$ END OF  <diagIfSoftRSTSet> */




/*  Interrupts lock/unlock interface functions:
 *  These functins are not nessesary for u-boot functionality,
 *  but must be implemented for other OS  
 */

/*!**************************************************************************
*!
*$ FUNCTION: diagIfIntLock
*!
*$ GENERAL DESCRIPTION: Interrupt Lock (disable interrupts).
*!
*$ RETURNS: void
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!***************************************************************************
*!*/
void diagIfIntLock(MV_U32* intMask)
{
/*!**************************************************************************/
/*! L O C A L   D E C L A R A T I O N S  A N D  I N I T I A L I Z A T I O N */
/*!**************************************************************************/
  
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/


}
/*$ END OF  <diagIfIntLock> */



/*!**************************************************************************
*!
*$ FUNCTION: diagIfIntUnlock
*!
*$ GENERAL DESCRIPTION: Interrupt UnLock (Enable interrupts).
*!
*$ RETURNS: void
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:
*!
*$ REMARKS:     (local)
*!
*!***************************************************************************
*!*/
void diagIfIntUnlock(MV_U32* intMask)
{
/*!**************************************************************************/
/*! L O C A L   D E C L A R A T I O N S  A N D  I N I T I A L I Z A T I O N */
/*!**************************************************************************/
  
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/


}
/*$ END OF  <diagIfIntUnlock> */

