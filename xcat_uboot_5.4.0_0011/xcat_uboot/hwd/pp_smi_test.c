#include <common.h>
#include "mvTypes.h"
#include "hwd_config.h"

#include "diag_inf.h"
#include "diag_services.h"

#include "private_test_config.h"


#ifdef  PP_SMI_TEST

/* Definition for Multi Address Mode */
#define QD_REG_SMI_COMMAND      0x0
#define QD_REG_SMI_DATA         0x1

/* Bit definition for QD_REG_SMI_COMMAND */
#define QD_SMI_BUSY             0x8000
#define QD_SMI_MODE             0x1000
#define QD_SMI_MODE_BIT         12
#define QD_SMI_OP_BIT           10
#define QD_SMI_OP_SIZE          2
#define QD_SMI_DEV_ADDR_BIT     5
#define QD_SMI_DEV_ADDR_SIZE    5
#define QD_SMI_REG_ADDR_BIT     0
#define QD_SMI_REG_ADDR_SIZE    5

#define QD_SMI_CLAUSE45         0
#define QD_SMI_CLAUSE22         1

#define QD_SMI_WRITE            0x01
#define QD_SMI_READ             0x02

#define QD_SMI_ACCESS_LOOP      1000
#define QD_SMI_TIMEOUT          2


/* speific for salsa/cheetah */
#define  SMI_WRITE_ADDRESS_MSB_REGISTER   (0x00)
#define  SMI_WRITE_ADDRESS_LSB_REGISTER   (0x01)
#define  SMI_WRITE_DATA_MSB_REGISTER      (0x02)
#define  SMI_WRITE_DATA_LSB_REGISTER      (0x03)

#define  SMI_READ_ADDRESS_MSB_REGISTER    (0x04)
#define  SMI_READ_ADDRESS_LSB_REGISTER    (0x05)
#define  SMI_READ_DATA_MSB_REGISTER       (0x06)
#define  SMI_READ_DATA_LSB_REGISTER       (0x07)

#define  SMI_STATUS_REGISTER              (0x1f)

#define SMI_STATUS_WRITE_DONE             (0x02)
#define SMI_STATUS_READ_READY             (0x01)

#define SMI_TIMEOUT_COUNTER               (1000)

/* Definitions for mixed DX SOHO systems used with 
   compilation flag DXSX_FAMILY.
   In DX SOHO system the SMI Slave device id (devSlvId) parameter 
   consists of two parts:
   - 16 LSB are SMI Slave device id in the HW
   - 16 MSB are Device type and details of SMI access to it. 
        See HW_IF_SMI_... for details. 
   E.g. 
     To access the DX device with SMI slave id N by functions 
   from this module (hwIfSmi...) the 
   devSlvId should be = (HW_IF_SMI_DX_DEVICE << 16) | N.
     To access the SOHO device with direct access (parallel SMI)
   with SMI slave id N by functions from this module (hwIfSmi...) the 
   devSlvId should be = (HW_IF_SMI_SOHO_DEVICE_DIRECT << 16) | N.   */

/* 16 MSB of devSlvId for DX devices */
#define HW_IF_SMI_DX_DEVICE               0

/* 16 MSB of devSlvId for SOHO devices with indirect SMI access */
#define HW_IF_SMI_SOHO_DEVICE_INDIRECT    1

/* 16 MSB of devSlvId for SOHO devices with direct SMI access */
#define HW_IF_SMI_SOHO_DEVICE_DIRECT      2

#if defined(SX_ONLY_DIRECT)
#define SX_FAMILY
#elif defined(SX_ONLY_INDIRECT)
#define SX_FAMILY
#elif defined(DX_ONLY)
#define DX_FAMILY
#elif defined(DX_SX_DIRECT)
#define SX_FAMILY
#define DX_FAMILY
#elif defined(DX_SX_INDIRECT)
#define SX_FAMILY
#define DX_FAMILY
#else
#endif

#define SMI_TEST_INTERNAL_ADDRESS       0x10
#define SX_DEVICE_ID_REG      0x03
#define DX_VENDOR_ID_REG      0x50
#define DX_DEVICE_ID_REG      0x4C



/*******************************************************************************
* hwIfSmiMultichipWrite
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*       Supports SOHO single-chip, multi-cuip and DX protocols.
*
* INPUTS:
*       devSlvId - 16 MSB: SMI protocol type
*                  16 LSB: Slave Device ID
*       regAddr  - Register address to read from.
*       value    - Data write to register.
*
* OUTPUTS:
*        None,
*
* RETURNS:
*       MV_OK               - on success
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*******************************************************************************/
static MV_STATUS hwIfSmiMultichipWrite
(
    IN MV_U32 devSlvId,
    IN MV_U32 regAddr,
    IN MV_U32 value
)
{
    MV_STATUS rc;
    MV_U16 smiMode;        /* SOHO Single / multi-chip  or DX*/
    MV_U32 regSmiDevId;
    MV_U32 regSmiRegAddr;
    volatile MV_U32 timeOut; /* in 100MS units */
    volatile MV_32  i;
    MV_U32 smiReg;
    MV_U32 msb;
    MV_U32 lsb;

    /* get internal info for register */
    regSmiRegAddr = regAddr & 0xffff;
    regSmiDevId   = regAddr >> 16;
    smiMode       = devSlvId >> 16;
    devSlvId &= 0xFFFF;    /* send only device addr to BSP */
    
    switch(smiMode)
    {
      case HW_IF_SMI_SOHO_DEVICE_DIRECT:     /******** SOHO DIRECT ********/
        rc = diagIfSmiWrite(devSlvId, regSmiDevId, regSmiRegAddr, value);
        break;

      case HW_IF_SMI_SOHO_DEVICE_INDIRECT:   /******* SOHO INDIRECT *******/
        /* first check that it is not busy */
        rc = diagIfSmiRead(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
        if (rc != MV_OK) 
          return MV_FAIL;

        timeOut = QD_SMI_ACCESS_LOOP; /* initialize the loop count */

        if(smiReg & QD_SMI_BUSY) 
        {
          for(i = 0 ; i < QD_SMI_TIMEOUT ; i++);
          do 
          {
            if(timeOut-- < 1 ) 
              return MV_TIMEOUT;

            rc = diagIfSmiRead(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
            if (rc != MV_OK) 
              return MV_FAIL;

          } while (smiReg & QD_SMI_BUSY);
        }

        /* the SMI is not busy, write SMI data register */
        rc = diagIfSmiWrite(devSlvId,0,QD_REG_SMI_DATA,value);
        if (rc != MV_OK) 
          return MV_FAIL;

        /*   build data for SMI command register */
        smiReg = QD_SMI_BUSY                          | 
                 (QD_SMI_CLAUSE22 << QD_SMI_MODE_BIT) |
                 (QD_SMI_WRITE << QD_SMI_OP_BIT)      | 
                 (regSmiDevId << QD_SMI_DEV_ADDR_BIT) | 
                 (regSmiRegAddr << QD_SMI_REG_ADDR_BIT);

        /* write SMI Command register */
        rc = diagIfSmiWrite(devSlvId, 0, QD_REG_SMI_COMMAND, smiReg);
        if (rc != MV_OK) 
          return MV_FAIL;
        break;

      case HW_IF_SMI_DX_DEVICE:              /******** DX INDIRECT ********/

        /* write addr to write */
        msb = regAddr >> 16;
        lsb = regAddr & 0xFFFF;
        rc = diagIfSmiWrite(devSlvId, 0, SMI_WRITE_ADDRESS_MSB_REGISTER, msb);
        if (rc != MV_OK)
          return rc;

        rc = diagIfSmiWrite(devSlvId, 0, SMI_WRITE_ADDRESS_LSB_REGISTER, lsb);
        if (rc != MV_OK)
          return rc;

        /* write data to write */
        msb = value >> 16;
        lsb = value & 0xFFFF;
        rc = diagIfSmiWrite(devSlvId, 0, SMI_WRITE_DATA_MSB_REGISTER, msb);
        if (rc != MV_OK)
          return rc;

        rc = diagIfSmiWrite(devSlvId, 0, SMI_WRITE_DATA_LSB_REGISTER, lsb);
        if (rc != MV_OK)
          return rc;
        break;

      default:
        return MV_NOT_SUPPORTED;
        break;
    }
  return rc;
}



/*******************************************************************************
* hwIfSmiMultichipRead
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*       Supports SOHO single-chip, multi-cuip and DX protocols.
*
* INPUTS:
*       devSlvId - 16 MSB: SMI protocol type
*                  16 LSB: Slave Device ID
*       regAddr  - Register address to read from.
*
* OUTPUTS:
*        valuePtr  - Data read from register.
*
* RETURNS:
*       MV_OK               - on success
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*******************************************************************************/
static MV_STATUS hwIfSmiMultichipRead
(
    IN  MV_U32 devSlvId,
    IN  MV_U32 regAddr,
    OUT MV_U32 *valuePtr
)
{
    MV_STATUS rc;
    MV_U16 smiMode;        /* SOHO Single / multi-chip  or DX*/
    MV_U32 regSmiDevId;
    MV_U32 regSmiRegAddr;
    volatile MV_U32 timeOut; /* in 100MS units */
    volatile MV_32  i;
    MV_U32 smiReg;
    MV_U32 msb;
    MV_U32 lsb;

    /* get internal info for register */
    regSmiRegAddr = regAddr & 0xffff;
    regSmiDevId   = regAddr >> 16;
    smiMode       = devSlvId >> 16;
    
    switch(smiMode)
    {
      case HW_IF_SMI_SOHO_DEVICE_DIRECT:     /******** SOHO DIRECT ********/
        devSlvId &= 0xFFFF;    /* send only device addr to BSP */
        rc = diagIfSmiRead(devSlvId, regSmiDevId, regSmiRegAddr, valuePtr);
        break;

      case HW_IF_SMI_SOHO_DEVICE_INDIRECT:   /******* SOHO INDIRECT *******/
        /* first check that it is not busy */
        rc = diagIfSmiRead(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
        if (rc != MV_OK) 
          return MV_FAIL;

        timeOut = QD_SMI_ACCESS_LOOP; /* initialize the loop count */

        if(smiReg & QD_SMI_BUSY) 
        {
          for(i = 0 ; i < QD_SMI_TIMEOUT ; i++);
          do 
          {
            if(timeOut-- < 1 ) 
              return MV_TIMEOUT;

            rc = diagIfSmiRead(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
            if (rc != MV_OK) 
              return MV_FAIL;

          } while (smiReg & QD_SMI_BUSY);
        }

    /* the SMI is not busy, 
       build data for SMI command register */
        smiReg = QD_SMI_BUSY                          | 
                 (QD_SMI_CLAUSE22 << QD_SMI_MODE_BIT) |
                 (QD_SMI_READ << QD_SMI_OP_BIT)       | 
                 (regSmiDevId << QD_SMI_DEV_ADDR_BIT) | 
                 (regSmiRegAddr << QD_SMI_REG_ADDR_BIT);

        /* write SMI Command register */
        rc = diagIfSmiWrite(devSlvId, 0, QD_REG_SMI_COMMAND, smiReg);
        if (rc != MV_OK) 
          return MV_FAIL;

        /* initialize the loop count */    
        timeOut = QD_SMI_ACCESS_LOOP;
        
        /* Read SMI command register to check BUSY bit */
        rc = diagIfSmiRead(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
        if (rc != MV_OK) 
          return MV_FAIL;

        timeOut = QD_SMI_ACCESS_LOOP; /* initialize the loop count */

        if(smiReg & QD_SMI_BUSY) 
        {
          for(i = 0 ; i < QD_SMI_TIMEOUT ; i++);
          do 
          {
            if(timeOut-- < 1 ) 
              return MV_TIMEOUT;

            rc = diagIfSmiRead(devSlvId, 0, QD_REG_SMI_COMMAND, &smiReg);
            if (rc != MV_OK) 
              return MV_FAIL;

          } while (smiReg & QD_SMI_BUSY);
        }

        /* The Data is ready in the SMI Data register */
        rc = diagIfSmiRead(devSlvId, 0, QD_REG_SMI_DATA, valuePtr);
        if (rc != MV_OK) 
            return MV_FAIL;

        break;

      case HW_IF_SMI_DX_DEVICE:              /******** DX INDIRECT ********/
         /* write addr to write */
        msb = regAddr >> 16;
        lsb = regAddr & 0xFFFF;
        rc = diagIfSmiWrite(devSlvId, 0, SMI_READ_ADDRESS_MSB_REGISTER, msb);
        if (rc != MV_OK)
          return rc;

        rc = diagIfSmiWrite(devSlvId, 0, SMI_READ_ADDRESS_LSB_REGISTER, lsb);
        if (rc != MV_OK)
          return rc;

       /* write data to write */
        rc = diagIfSmiRead(devSlvId, 0, SMI_READ_DATA_MSB_REGISTER, &msb);
        if (rc != MV_OK)
          return rc;

        rc = diagIfSmiRead(devSlvId, 0, SMI_READ_DATA_LSB_REGISTER, &lsb);
        if (rc != MV_OK)
          return rc;

        *valuePtr = ((msb & 0xFFFF) << 16) | (lsb & 0xFFFF);
        break;

      default:
        return MV_NOT_SUPPORTED;
        break;
    }
  return rc;
}



/*******************************************************************************
* hwIfSmiWriteReg
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MV_OK               - on success
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*       
*      For SOHO PPs the regAddr consist of 2 parts:
*      - 16 MSB is SMI device ID for register
*      - 16 LSB is register address within SMI device ID for register
*
*******************************************************************************/
MV_STATUS hwIfSmiWriteReg
(
    IN MV_U32 devSlvId,
    IN MV_U32 regAddr,
    IN MV_U32 value
)
{
#if defined(SX_ONLY_DIRECT)
    devSlvId |= (HW_IF_SMI_SOHO_DEVICE_DIRECT << 16);
    return hwIfSmiMultichipWrite(devSlvId, regAddr, value);

#elif defined(SX_ONLY_INDIRECT)
    devSlvId |= (HW_IF_SMI_SOHO_DEVICE_INDIRECT << 16);
    return hwIfSmiMultichipWrite(devSlvId, regAddr, value);

#elif defined(DX_ONLY)
    devSlvId |= (HW_IF_SMI_DX_DEVICE << 16);
    return hwIfSmiMultichipWrite(devSlvId, regAddr, value);

#elif defined(DX_SX_DIRECT)
    if( (devSlvId & 0xFFFF) == DX_SMI_ADDR)
      devSlvId |= (HW_IF_SMI_DX_DEVICE << 16);
	else
      devSlvId |= (HW_IF_SMI_SOHO_DEVICE_DIRECT << 16);

    return hwIfSmiMultichipWrite(devSlvId, regAddr, value);

#elif defined(DX_SX_INDIRECT)
    if( (devSlvId & 0xFFFF) == DX_SMI_ADDR)
      devSlvId |= (HW_IF_SMI_DX_DEVICE << 16);
	else
      devSlvId |= (HW_IF_SMI_SOHO_DEVICE_INDIRECT << 16);

    return hwIfSmiMultichipWrite(devSlvId, regAddr, value);

#else /* no valid option */
    return MV_ERROR;
#endif
}



/*******************************************************************************
* hwIfSmiReadReg
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using SMI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       MV_OK               - on success
*       MV_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
MV_STATUS hwIfSmiReadReg
(               
    IN  MV_U32  devSlvId,
    IN  MV_U32  regAddr,
    OUT MV_U32 *valuePtr
)
{
#if defined(SX_ONLY_DIRECT)
    devSlvId |= (HW_IF_SMI_SOHO_DEVICE_DIRECT << 16);
    return hwIfSmiMultichipRead(devSlvId, regAddr, valuePtr);

#elif defined(SX_ONLY_INDIRECT)
    devSlvId |= (HW_IF_SMI_SOHO_DEVICE_INDIRECT << 16);
    return hwIfSmiMultichipRead(devSlvId, regAddr, valuePtr);

#elif defined(DX_ONLY)
    devSlvId |= (HW_IF_SMI_DX_DEVICE << 16);
    return hwIfSmiMultichipRead(devSlvId, regAddr, valuePtr);

#elif defined(DX_SX_DIRECT)
    if( (devSlvId & 0xFFFF) == DX_SMI_ADDR)
      devSlvId |= (HW_IF_SMI_DX_DEVICE << 16);
	else
      devSlvId |= (HW_IF_SMI_SOHO_DEVICE_DIRECT << 16);

    return hwIfSmiMultichipRead(devSlvId, regAddr, valuePtr);

#elif defined(DX_SX_INDIRECT)
    if( (devSlvId & 0xFFFF) == DX_SMI_ADDR)
      devSlvId |= (HW_IF_SMI_DX_DEVICE << 16);
	else
      devSlvId |= (HW_IF_SMI_SOHO_DEVICE_INDIRECT << 16);

    return hwIfSmiMultichipRead(devSlvId, regAddr, valuePtr);

#else /* no valid option */
    return MV_ERROR;
#endif
}



MV_STATUS pp_smi_scan_test(void)
{
    MV_STATUS status = MV_OK;
    MV_U32 value;
    int i = 0;

#ifdef SX_FAMILY
    MV_U32 smi_temp;
    
    for(i=SX_SMI_ADDR_FIRST; i<SX_SMI_ADDR_LAST; i++)
    {
        value = 0;
		smi_temp = SX_DEVICE_ID_REG | (SMI_TEST_INTERNAL_ADDRESS << 16);
        hwIfSmiReadReg(i, smi_temp, &value);
  
        if(value != 0xFFFF)
        {
            printf("\n\rSX Id at SMI Address 0x%x: 0x%x\n", i, value);
        }
		else
        {
            status = MV_ERROR;
            printf("\n\rFailed to read SX in SMI Address 0x%x\n", i);
        }
    }
#endif

#ifdef DX_FAMILY
    hwIfSmiReadReg(DX_SMI_ADDR, DX_VENDOR_ID_REG, &value);
 
    if(value != 0xFFFF)
    {
        printf("\n\rDX Id at SMI Address 0x%x: 0x%x\n", i, value);
    }
    else
    {
        status = MV_ERROR;
        printf("\n\rFailed to read DX in SMI Address 0x%x\n", i);
    }
#endif
  
    if(status != MV_OK)
    {
        printf("SMI scan test...Fail !!!\n");
        return MV_ERROR;
    }

    printf("\nSMI scan test... Pass\n");
    return MV_OK;
}



MV_STATUS pp_smi_read_test(void)
{
  MV_STATUS status = MV_OK;
  MV_U32 reg, value;
  MV_U32 smi_addr = 0 ;
  #ifdef SX_FAMILY
  MV_U32 internal_addr = 0;
  #endif  /* SX_FAMILY */

  printf ("\n\nEnter PP_SMI address (hex): ");

  if (mvBspServicesInputHexNumber(&smi_addr) != MV_OK) 
    return MV_ERROR;

#ifdef SX_FAMILY
  printf ("\n\nEnter SX internal SMI address (relevant to SX only)(hex): ");

  if (mvBspServicesInputHexNumber(&internal_addr) != MV_OK) 
    return MV_ERROR;
#endif  /* SX_FAMILY */

  printf ("\nEnter register index (hex): ");

  if (mvBspServicesInputHexNumber(&reg) != MV_OK)
    return MV_ERROR;

  /* read data from PP */
  status = hwIfSmiReadReg(smi_addr, reg, &value);

  if (status != MV_OK)
  {
      printf ("\nPP_SMI read test: Receive Error...\n");
      return MV_RCV_ERROR;
  }

  printf("\nPP_SMI Read value: 0x%x\n", value);

  return status;
}



MV_STATUS pp_smi_write_test(void)
{
  MV_STATUS status = MV_OK;
  MV_U32 smi_addr, reg, value;
  #ifdef SX_FAMILY
  MV_U32 internal_addr
  #endif  /* SX_FAMILY */

  printf ("\n\nEnter PP_SMI address (hex): ");

  if (mvBspServicesInputHexNumber(&smi_addr) != MV_OK) 
    return MV_ERROR;

#ifdef SX_FAMILY
  printf ("\n\nEnter SX internal SMI address (relevant to SX only)(hex): ");

  if (mvBspServicesInputHexNumber(&internal_addr) != MV_OK) 
    return MV_ERROR;
#endif  /* SX_FAMILY */

  printf ("\nEnter register index (hex): ");

  if (mvBspServicesInputHexNumber(&reg) != MV_OK)
    return MV_ERROR;

  printf ("\nEnter register value (hex): ");

  if (mvBspServicesInputHexNumber(&value) != MV_OK)
    return MV_ERROR;

  /* write to PP */
  status = hwIfSmiWriteReg(smi_addr, reg, value);

  if (status != MV_OK)
  {
      printf ("\nPP_SMI read test: Transmit Error...\n");
      return MV_RCV_ERROR;
  }

  printf("\nSMI Wrote 0x%x to PP\n", value);

  return status;
}



MV_STATUS pp_smi_test(void)
{
    MV_STATUS status = MV_OK;

    if(pp_smi_scan_test() != MV_OK)
        status = MV_ERROR;

    return status;
}


#endif  /* PP_SMI_TEST */
