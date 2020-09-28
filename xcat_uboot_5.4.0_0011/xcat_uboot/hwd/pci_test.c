#include <common.h>
#include "hwd_config.h"
#include "diag_services.h"

#include "private_test_config.h"


#ifdef  PCI_TEST



MV_STATUS pci_scan_test(void)
{
    MV_U32 pciIf, bus, dev, func;
    MV_U32 value;
    MV_U32 pci_device_found = 0;
    
    printf ("\npci_scan_test:\n"
            "--------------\n");

	/* Scan Interface 0 (Pex)*/
	for(pciIf=MV_PEX_START_IF; pciIf < MV_PEX_START_IF + MV_PEX_MAX_IF; pciIf++)
	{
		for(bus=0; bus<DIAG_MAX_PCI_BUSSES; bus++)
		{
		    for(dev=0; dev<DIAG_MAX_PCI_DEVICES; dev++)
		    {
				func = 0;
		        value = 0;
		  
		        diagIfPciConfigRead (pciIf, bus, dev, func, DIAG_PCI_READ_WRITE_REG, &value);
		  
		        if(value != 0xFFFFFFFF)
		        {
		            pci_device_found++;
		            printf ("%d.  If = Pex   BusNum=0x%x   Dev.Num=0x%x.  Dev.ID=0x%x   Ven.ID=0x%x\n",
		                          pci_device_found, bus, dev, (MV_U16)(value >> 16), (MV_U16)value);
		        }
	    	}
		}
	}

	/* Scan Interface 1 (Pci)*/
	for(pciIf=MV_PCI_START_IF; pciIf < MV_PCI_START_IF + MV_PCI_MAX_IF; pciIf++)
	{
		for(bus=0; bus<DIAG_MAX_PCI_BUSSES; bus++)
		{
		    for(dev=0; dev<DIAG_MAX_PCI_DEVICES; dev++)
		    {
				func = 0;
		        value = 0;

		        diagIfPciConfigRead (pciIf, bus, dev, func, DIAG_PCI_READ_WRITE_REG, &value);
		  
		        if(value != 0xFFFFFFFF)
		        {
		            pci_device_found++;
		            printf ("%d.  If = Pci   BusNum=0x%x   Dev.Num=0x%x.  Dev.ID=0x%x   Ven.ID=0x%x\n",
		                          pci_device_found, bus, dev, (MV_U16)(value >> 16), (MV_U16)value);
		        }
		    }
		}
	}
  
    if(! pci_device_found)
    {
        printf("PCI Test: pci_scan_test...Fail !!!\n");
        return MV_ERROR;
    }

    printf("PCI Test: pci_scan_test... Pass!  Found %d devices.\n", pci_device_found);
    return MV_OK;
}


MV_STATUS pci_config_read_test(void)
{
    MV_STATUS status;
    MV_U32 data;
    MV_U32 pci_if;
    MV_U32 bus_id; 
    MV_U32 dev_num;
    MV_U32 regOff;

    printf ("Enter PCI/PEX INTERFACE (0 or 1):");
    if (mvBspServicesInputHexNumber(&pci_if) != MV_OK)
        return MV_ERROR;

    printf ("Enter PCI bus_id : ");
    if (mvBspServicesInputHexNumber(&bus_id) != MV_OK)
        return MV_ERROR;

    printf ("\nEnter PCI Device Num (hex): ");
    if (mvBspServicesInputHexNumber(&dev_num) != MV_OK) 
        return MV_ERROR;

    printf ("\nEnter reg. offset address (hex): ");
    if (mvBspServicesInputHexNumber(&regOff) != MV_OK) 
        return MV_ERROR;

    status = diagIfPciConfigRead (pci_if, bus_id, dev_num, 0, regOff, &data);
    printf ("\npci_config_read_test: data (hex) = 0x%x\n", data);

    return status;
}



MV_STATUS pci_config_write_test(void)
{
    MV_STATUS status;
    MV_U32 data;
    MV_U32 pci_if;
    MV_U32 bus_id; 
    MV_U32 dev_num;
    MV_U32 regOff;

    printf ("Enter PCI/PEX INTERFACE (0 or 1):");
    if (mvBspServicesInputHexNumber(&pci_if) != MV_OK)
        return MV_ERROR;

    printf ("Enter PCI bus_id : ");
    if (mvBspServicesInputHexNumber(&bus_id) != MV_OK)
        return MV_ERROR;

    printf ("\nEnter PCI Device Num (hex): ");
    if (mvBspServicesInputHexNumber(&dev_num) != MV_OK) 
        return MV_ERROR;

    printf ("\nEnter reg. offset address (hex): ");
    if (mvBspServicesInputHexNumber(&regOff) != MV_OK) 
        return MV_ERROR;

    printf ("\nEnter data (hex): ");
    if (mvBspServicesInputHexNumber(&data) != MV_OK) 
        return MV_ERROR;

    status = diagIfPciConfigWrite(pci_if, bus_id, dev_num, 0, regOff, data);
     
    return status;
}



MV_STATUS pci_test(void)
{
    MV_STATUS status = MV_OK;

    if(pci_scan_test() != MV_OK)
        status = MV_ERROR;

	return status;
}


#endif  /* PCI_TEST */
