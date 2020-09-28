#include <common.h>
#include "mvTypes.h"
#include "hwd_config.h"

#include "diag_inf.h"
#include "diag_services.h"

#include "private_test_config.h"


#ifdef  I2C_TEST

extern mvBsp_i2c_devicesStruct diagIfI2cTbl;


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
/* #include         <mvDiagBsp/inc/defs.h> */
 
/*!**************************************************************************
 *$              EXTERNAL DECLARATIONS (IMPORT AND EXPORT)
 *!**************************************************************************
 *!*/

/*!**************************************************************************
 *$              EXTERNAL DECLARATIONS (IMPORT)
 *!**************************************************************************
 *!*/
/*
 #include         <mvDiagBsp/inc/diag_inf.h> 

#include         <mvDiagBsp/App/diag_main.h> 
#include         <mvDiagBsp/App/diag_services.h> 

#include         <mvDiagBsp/testEnv/diag_i2c.h>
#include         <mvDiagBsp/testLow/h_i2c.h> 
*/

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
#define MV_I2C_PRINT_LEN          0x50
#define MV_I2C_PRINT_LINE_LEN     0x10
#define MV_I2C_SCAN_LEN           2
#define MV_I2C_ADDRESS_RESOLUTION 2 
#define MV_I2C_FIRST_OFFSET       0
#define MV_I2C_FIRST_ADDRESS      0x0
#define MV_I2C_LAST_ADDRESS       0x100
#define MV_I2C_DEVICE_TYPE_STRING 20

typedef  enum
{
  mvI2cModuleUnknown = 0,
  mvI2cModuleGbic = 1,
  mvI2cModuleSolered = 2,
  mvI2cModuleSfp = 3,
  mvI2cModuleUsb = 0x80,
  mvI2cNoModule = 0xFF
}  mvI2cModuleTypeEnum;

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
 *$ FUNCTION: mvI2cDeviceType
 *!
 *$ GENERAL DESCRIPTION: Check I2C module type.
 *!
 *$ RETURNS:    MV_TRUE or MV_FALSE
 *!
 *$ ALGORITHM:   (local)
 *!
 *$ ASSUMPTIONS:
 *!
 *$ REMARKS:     (local)
 *!
 *!**************************************************************************
 *!*/

MV_U8 mvI2cDeviceType(
  /*!     INPUTS:             */
  MV_U8 offset,                /* device address on bus */
  MV_U8 gbic_index,            /* i2c bus index */
  /*!     INPUTS / OUTPUTS:   */

  /*!     OUTPUTS:            */
  MV_U8 *device_type           /* return string */
)
{
/*!*************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!*************************************************************************/
  MV_U8 buffer[MV_I2C_SCAN_LEN];
  MV_U8 tx_buffer;
  MV_STATUS i2c_status = MV_OK;
  mvI2cModuleTypeEnum module_type;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  /* Clear data buffer:  */
  memset(buffer, '\0', MV_I2C_SCAN_LEN);

  tx_buffer = 0;

  i2c_status = diagIfI2cTx (offset, (MV_U32)1, gbic_index, &tx_buffer);
  if (i2c_status != MV_OK)
  {
    /* device not present */
    return MV_TX_ERROR;
  }

  i2c_status = diagIfI2cRx (offset, (MV_U32)MV_I2C_SCAN_LEN, gbic_index, buffer);
  if (i2c_status != MV_OK)
  {
    /* device not present */
    return MV_RCV_ERROR;
  }

  module_type = (mvI2cModuleTypeEnum)buffer[MV_I2C_FIRST_OFFSET];

  /* test module type */
  switch (module_type)
  {
    /* found soldered module */
    case mvI2cModuleSolered:
      strcpy(device_type,"SOLDERED");
    break;

    /* found GBIC module */
    case mvI2cModuleGbic:
      strcpy(device_type,"GBIC");
    break;

    /* found SFP module */
    case mvI2cModuleSfp:
      strcpy(device_type,"SFP");
    break;

    /* found USB module */
    case mvI2cModuleUsb:
      strcpy(device_type,"USB");
    break;

    /* empty slot or unknown module */
    default:
      strcpy(device_type,"Unknown module");
    break;
  }

  return i2c_status;

}

/*!**************************************************************************
 *$              PUBLIC FUNCTION DEFINITIONS (EXPORT)
 *!**************************************************************************
 *!*/


/*!**************************************************************************
*!
*$ FUNCTION: mvI2cScanTest
*!
*$ GENERAL DESCRIPTION:  Scan the I2C bus to check devices from given list.
*!
*$ RETURNS:   
*       MV_OK      - on success
*       MV_ERROR   - on hardware error
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:  diagIfI2cRx at the low level must return error if
*!              no acknowledge received from the address.
*!
*$ REMARKS:     (local)
*!
*!**************************************************************************
*!*/
MV_STATUS mvI2cScanTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 device_index, num_of_devices;
  MV_U8  rx_data;
  MV_U8 device_type[MV_I2C_DEVICE_TYPE_STRING];
  mvBsp_i2c_devicesStruct *i2c_array_ptr;
  MV_STATUS ret_status = MV_OK;
  
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf ("\n\nTesting I2C bus:");
  printf   ("\n----------------\n");

  num_of_devices = DIAG_I2C_NUM_OF_DEVICES;
  i2c_array_ptr  = &diagIfI2cTbl;

  if ((num_of_devices == 0) || (i2c_array_ptr == NULL))
  {
    printf ("\nI2C devices list is empty.\n");

    return MV_ERROR;
  }

  /* check all devices in list of I2C devices from H_Pdiag */
  for(device_index = 0; device_index < num_of_devices; device_index++)
  {
    /* try to read data */
    if (diagIfI2cRx(i2c_array_ptr[device_index].i2c_device_address,
                    (MV_U32) (sizeof(MV_U8)),
                    i2c_array_ptr[device_index].i2c_device_index,
                    &rx_data ) == MV_OK)
    {
      /* clear device type string */
      memset(device_type, '\0', MV_I2C_DEVICE_TYPE_STRING);

      /* check for device type */
      if(mvI2cDeviceType(i2c_array_ptr[device_index].i2c_device_address,
                      i2c_array_ptr[device_index].i2c_device_index,
                      device_type) == MV_OK)
          printf ("Address: 0x%.2x Index: 0x%.2x Type: %s.\n",
                                    i2c_array_ptr[device_index].i2c_device_address,
                                    i2c_array_ptr[device_index].i2c_device_index,
                                    device_type);
      else
          printf ("Address: 0x%.2x Index: 0x%.2x - Device type Error !.\n",
                                    i2c_array_ptr[device_index].i2c_device_address,
                                    i2c_array_ptr[device_index].i2c_device_index);
      }
      else
      {
        printf ("Address: 0x%.2x Index: 0x%.2x Device not found.\n",
                                i2c_array_ptr[device_index].i2c_device_address,
                                i2c_array_ptr[device_index].i2c_device_index);
        ret_status = MV_ERROR;
      }
  }

  return ret_status;

}

/*!**************************************************************************
*!
*$ FUNCTION: mvI2cWriteTest
*!
*$ GENERAL DESCRIPTION: Transmit data to I2C device.
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
MV_STATUS mvI2cWriteTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U8  data[MV_I2C_PRINT_LINE_LEN];
  MV_U8  tx_data[MV_I2C_PRINT_LINE_LEN];
  MV_U32 address, bus_id, data_length;
  MV_STATUS i2c_status;
  MV_U8 parse_result;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf ("\n\nEnter device address (hex): ");

  if (mvBspServicesInputHexNumber(&address) != MV_OK) 
    return MV_ERROR;

  printf ("Enter I2C bus index: ");

  if (mvBspServicesInputHexNumber(&bus_id) != MV_OK) 
    return MV_ERROR;

  printf ("Enter how many bytes to write (0x01 - 0x%x): ",
                                                 MV_I2C_PRINT_LINE_LEN);

  if (mvBspServicesInputHexNumber(&data_length) != MV_OK) 
    return MV_ERROR;

  if(data_length > MV_I2C_PRINT_LINE_LEN)
  {
    printf ("\nError: Can't write more than %s bytes\n\n",
                                                 MV_I2C_PRINT_LINE_LEN);
    return MV_ERROR;
  }

  memset(data, '\0', MV_I2C_PRINT_LINE_LEN);
  memset(tx_data, 0, MV_I2C_PRINT_LINE_LEN);

  printf ("Enter data in hex: ");
  mvBspServicesScanStr  (MV_I2C_PRINT_LINE_LEN, data); 
  printf ("\n");

  parse_result = mvBspServicesParseHexNumber (data, strlen(data), (MV_U32 *)tx_data); 

  if ( parse_result != MV_OK )
  {
    printf ("\nError: Wrong data\n\n");
    return MV_ERROR;
  }

  i2c_status = diagIfI2cTx((MV_U8)address, (MV_U32)data_length, (MV_U8)bus_id, tx_data);

  if (i2c_status != MV_OK)
  {
      printf ("\nI2C test: Transmit Error...\n");
      return MV_TX_ERROR;
   }

  printf ("Data transmitted\n");

  return MV_OK;
}

/*!**************************************************************************
*!
*$ FUNCTION: mvI2cReadTest
*!
*$ GENERAL DESCRIPTION:   I2C test: reads the I2C device content.
*!                        Get from the user the device address and device bus
*!                        id, and call to diagIfI2cRx to receive the data.
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
MV_STATUS mvI2cReadTest (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U8  rx_buf[MV_I2C_PRINT_LEN];
  MV_U32 address, bus_id;
  MV_STATUS i2c_status;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf ("\n\nEnter device address (hex): ");

  if (mvBspServicesInputHexNumber(&address) != MV_OK) 
    return MV_ERROR;

  printf ("Enter I2C bus index: ");

  if (mvBspServicesInputHexNumber(&bus_id) != MV_OK)
    return MV_ERROR;

  memset (rx_buf, '\0', MV_I2C_PRINT_LEN);

  /* read data from I2C slave */
  i2c_status = diagIfI2cRx ((MV_U8)address,
                            (MV_U32)MV_I2C_PRINT_LEN,
                            (MV_U8)bus_id,
                             rx_buf);

  if (i2c_status != MV_OK)
  {
      printf ("\nI2C test: Receive Error...\n");
      return MV_RCV_ERROR;
  }

  /* send read data to terminal */
  mvBspServicesDisplayMemoryTable(rx_buf,MV_I2C_PRINT_LEN,sizeof(MV_U8),0); 

  return MV_OK;

}

/*!**************************************************************************
*!
*$ FUNCTION: mvI2cScanBus
*!
*$ GENERAL DESCRIPTION:  Scan the I2C bus to detect connected devices.
*!
*$ RETURNS:
*!
*$ ALGORITHM:   (local)
*!
*$ ASSUMPTIONS:  diagIfI2cRx at the low level must return error if
*!              no acknowledge received from the address.
*!
*$ REMARKS:     (local)
*!
*!**************************************************************************
*!*/
MV_STATUS mvI2cScanBus (void)
{
/*!****************************************************************************/
/*! L O C A L   D E C L A R A T I O N S   A N D   I N I T I A L I Z A T I O N */
/*!****************************************************************************/
  MV_U32 dev_address;
  MV_U8  rx_data;
  MV_U8 found, dev_found;
  MV_STATUS ret_status = MV_OK;
/*!*************************************************************************/
/*!                      F U N C T I O N   L O G I C                       */
/*!*************************************************************************/

  printf ("\n\nScaning I2C bus:\n");
  printf     ("----------------\n");
  printf ("\nAttention! It can be several devices on same address.\n");

  found = MV_FALSE;

  for (dev_address = MV_I2C_FIRST_ADDRESS;
       dev_address < MV_I2C_LAST_ADDRESS;
       dev_address += MV_I2C_ADDRESS_RESOLUTION)
  {
    dev_found = MV_FALSE;


    /* try to read data */
    // Ry@ if (diagIfI2cRx((MV_U8)dev_address,
    if (diagIfI2cTx((MV_U8)dev_address,
                    (MV_U32) (sizeof (MV_U8)),
                     0,
                     &rx_data ) == MV_OK)
    {
      if (! found) /* if this is the first device to be found */
      {
        printf ("\nFound devices:");
        printf ("\n--------------");
        found = MV_TRUE;
      }

      dev_found = MV_TRUE;
      printf ("\nAddress: 0x%.2x.\n", dev_address);
    }
  }

  printf ("\nI2C bus test");
  printf ("........................");

  if (!found)
  {
    printf ("No I2C devices found\n");
    ret_status = MV_ERROR;
    printf ("FAIL\n");
  }
  else
    printf ("PASS\n");

  return ret_status;
}

/*$ END OF <module_name> */



MV_STATUS i2c_test(void)
{
    MV_STATUS status = MV_OK;

    if(mvI2cScanBus() != MV_OK)
	  status = MV_ERROR;
    
    /* I2C auto test includes 2 tests - scan and serch devices */
	/* Read & Write are NOT auto tests and therefore should not be called here! */
    if(mvI2cScanTest() != MV_OK)
	  status = MV_ERROR;

    return status;
}
#endif  /* i2c_test */
