#include <common.h>
#include <net.h>
#include "mvTypes.h"
#include "hwd_config.h"

#include "diag_inf.h"
#include "diag_services.h"

#include "private_test_config.h"


#ifdef  SMI_TEST


MV_U8   smi_device_available_ary[DIAG_MAX_SMI_DEVICES];
int     smi_device_found = 0;


MV_STATUS smi_scan_test(void)
{
    int i;
    MV_U16 value;

    smi_device_found = 0;
    
    for(i=0; i<DIAG_MAX_SMI_DEVICES; i++)
    {
        value = 0;
  
        smi_device_available_ary[i] = 0;
        diagIfSmiRead(i, 0, DIAG_SMI_READ_WRITE_REG, (MV_U32 *)&value);
  
        if(value != 0xFFFF)
        {
            smi_device_available_ary[i] = 1;
            smi_device_found++;
            printf("\n\rsmi_scan_test: Address 0x%x , value: 0x%x\n", i, value);
        }
    }
  
    if(! smi_device_found)
    {
        printf("SMI_scan_test...Fail !!!\n");
        return MV_ERROR;
    }

    printf("\nsmi_scan_test... Pass !  Found %d devices.\n", smi_device_found);
    return MV_OK;
}



MV_STATUS smi_read_test(void)
{
  MV_STATUS status = MV_OK;
  MV_U32 address, reg, value = 0;

  printf ("\n\nEnter SMI address (hex): ");

  if (mvBspServicesInputHexNumber(&address) != MV_OK) 
    return MV_ERROR;

  printf ("\nEnter register index (hex): ");

  if (mvBspServicesInputHexNumber(&reg) != MV_OK)
    return MV_ERROR;

  /* read data from SMI device */
  status = diagIfSmiRead(address, 0, reg, &value);

  if (status != MV_OK)
  {
      printf ("\nSMI read test: Receive Error...\n");
      return MV_RCV_ERROR;
  }

  printf("\nSMI read value: 0x%x\n", value);

  return status;
}



MV_STATUS smi_write_test(void)
{
  MV_STATUS status = MV_OK;
  MV_U32 address, reg, value;

  printf ("\n\nEnter SMI address (hex): ");

  if (mvBspServicesInputHexNumber(&address) != MV_OK) 
    return MV_ERROR;

  printf ("\nEnter register index (hex): ");

  if (mvBspServicesInputHexNumber(&reg) != MV_OK)
    return MV_ERROR;

  printf ("\nEnter register value (hex): ");

  if (mvBspServicesInputHexNumber(&value) != MV_OK)
    return MV_ERROR;

  /* write data to SMI device */
  status = diagIfSmiWrite(address, 0, reg, value);

  if (status != MV_OK)
  {
      printf ("\nSMI write test: Transmit Error...\n");
      return MV_TX_ERROR;
  }

  printf("\nSMI wrote 0x%x to device\n", value);

  return status;
}



MV_STATUS smi_test(void)
{
    MV_STATUS status = MV_OK;

    if(smi_scan_test() != MV_OK)
        status = MV_ERROR;

    return status;
}


#endif  /* SMI_TEST */
