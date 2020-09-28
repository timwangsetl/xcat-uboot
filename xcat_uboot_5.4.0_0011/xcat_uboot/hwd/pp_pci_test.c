#include <common.h>
#include "mvTypes.h"
#include "hwd_config.h"

#include "diag_inf.h"
#include "diag_services.h"

#include "private_test_config.h"


#ifdef  PP_PCI_TEST

#define MV_PP_PCI_MAX_PRINT_LEN        0x80


/*******************************************************************************
* pp_pci_express_read_test
*
* DESCRIPTION:
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
*******************************************************************************/
MV_STATUS pp_pci_express_read_test(void)
{
    MV_U32 rx_buf[MV_PP_PCI_MAX_PRINT_LEN];
    MV_U32 pp_num, pp_addr;
    MV_U32 pci_if;
    MV_U32 bus_id; 
    MV_U32 dev;
    MV_U32 reg;
    int i;

    printf ("pp_pci_express_read_test:\n"
            "-------------------------\n");

    printf ("Enter PP device num (1, 2, ...): ");

    if (mvBspServicesInputHexNumber(&pp_num) != MV_OK)
      return MV_ERROR;

    printf ("Enter PEX/PCI INTERFACE (PEX=0 or PCI=1):");
    if (mvBspServicesInputHexNumber(&pci_if) != MV_OK)
        return MV_ERROR;

    printf ("Enter PCI bus_id : ");
    if (mvBspServicesInputHexNumber(&bus_id) != MV_OK)
        return MV_ERROR;

    dev = GET_PCI_DEVICE_NUM(pp_num);


    /* PP BAR_0 Registers */

    memset (rx_buf, '\0', MV_PP_PCI_MAX_PRINT_LEN);
    reg = PCI_MEMORY_BAR_BASE_ADDR(0);

    diagIfPciConfigRead(pci_if, bus_id, dev, 0, reg, &pp_addr);

    if(pp_addr == 0xffffffff)
        return MV_ERROR;

    for (i = 0; i < (MV_PP_PCI_MAX_PRINT_LEN/2); i++) 
    { 
    rx_buf[i] = *( ((MV_U32 *)pp_addr)+i);
    } 
    printf("\n\nPP_%d: BAR_0 Registers (Pci configuration):\n"
               "------------------------------------------\n", pp_num);
    printf("Base: 0x%x", pp_addr);
    mvBspServicesDisplayMemoryTable((MV_U8*)rx_buf, (MV_PP_PCI_MAX_PRINT_LEN/2), sizeof(MV_U32), 0);


    /* PP BAR_1 Registers */

    memset (rx_buf, '\0', MV_PP_PCI_MAX_PRINT_LEN);

    #if defined(DB_MNG)
        reg = 0x18; 
    #else 
        reg = PCI_MEMORY_BAR_BASE_ADDR(1); 
    #endif

    diagIfPciConfigRead(pci_if, bus_id, dev, 0, reg, &pp_addr);

    if(pp_addr == 0xffffffff)
        return MV_ERROR;
    
    for (i = 0; i < MV_PP_PCI_MAX_PRINT_LEN; i++) 
    { 
    rx_buf[i] = *( ((MV_U32 *)pp_addr)+i);
    } 
    printf("\n\nPP_%d: BAR_1 Registers:\n"
               "----------------------\n", pp_num);
    printf("Base: 0x%x", pp_addr);
    mvBspServicesDisplayMemoryTable((MV_U8*)rx_buf, MV_PP_PCI_MAX_PRINT_LEN, sizeof(MV_U32), 0);

    return MV_OK;
}




/*******************************************************************************
* pp_pci_custom_read_test
*
* DESCRIPTION:
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
MV_STATUS pp_pci_custom_read_test(void)
{
    MV_U32 rx_buf[MV_PP_PCI_MAX_PRINT_LEN];
    MV_U32 pp_num, addr;
    MV_U32 pci_if;
    MV_U32 bus_id; 
    MV_U32 dev;
    MV_U32 reg;
    MV_U32 bar_0_base_addr, bar_0_size, bar_1_base_addr, bar_1_size;
    int i, num;

    printf ("pp_pci_custom_read_test:\n"
            "------------------------\n");

    printf ("Enter PP device num (1, 2, ...): ");
    if (mvBspServicesInputHexNumber(&pp_num) != MV_OK)
      return MV_ERROR;
    if (pp_num > MAX_PP_PCI_DEVS)
    {
        printf ("Wrong PP num...\n\n");
        return MV_ERROR;
    }

    printf ("Enter PEX/PCI INTERFACE (PEX=0 or PCI=1):");
    if (mvBspServicesInputHexNumber(&pci_if) != MV_OK)
        return MV_ERROR;

    printf ("Enter PCI bus_id : ");
    if (mvBspServicesInputHexNumber(&bus_id) != MV_OK)
        return MV_ERROR;

    dev = GET_PCI_DEVICE_NUM(pp_num);



    /* Bar_0 */

    reg = PCI_MEMORY_BAR_BASE_ADDR(0); 

    /* read base addr */
    diagIfPciConfigRead( pci_if, bus_id, dev, 0, reg, &bar_0_base_addr);

    /* write oxffffffff to the bar to get the size */
    diagIfPciConfigWrite(pci_if, bus_id, dev, 0, reg, 0xffffffff);
    /* read size */
    diagIfPciConfigRead(pci_if, bus_id, dev, 0, reg, &bar_0_size);
    bar_0_size = ~bar_0_size + 1;

    /* restore original value */
    diagIfPciConfigWrite(pci_if, bus_id, dev, 0, reg, bar_0_base_addr);


    /* Bar_1 */

    #if defined(DB_MNG)
        reg = 0x18; 
    #else 
        reg = PCI_MEMORY_BAR_BASE_ADDR(1); 
    #endif

    /* read base addr */
    diagIfPciConfigRead (pci_if, bus_id, dev, 0, reg, &bar_1_base_addr);

    /* write oxffffffff to the bar to get the size */
    diagIfPciConfigWrite(pci_if, bus_id, dev, 0, reg, 0xffffffff); 

    /* read size */
    bar_1_size = 0;
    diagIfPciConfigRead(pci_if, bus_id, dev, 0, reg, &bar_1_size);
    bar_1_size = ~bar_1_size + 1;

    /* restore original value */
    diagIfPciConfigWrite(pci_if, bus_id, dev, 0, reg, bar_1_base_addr);


    printf ("PP_%d:\n"
            "\tBAR_0 BASE = 0x%x     BAR_0 SIZE = 0x%x\n"
            "\tBAR_1 BASE = 0x%x     BAR_1 SIZE = 0x%x\n\n",
            pp_num, bar_0_base_addr, bar_0_size, 
                    bar_1_base_addr, bar_1_size);

    printf ("Enter begin address: ");
    if (mvBspServicesInputHexNumber(&addr) != MV_OK)
      return MV_ERROR;

    if ( (addr < bar_0_base_addr) || (addr > bar_1_base_addr + bar_1_size) )
    {
        printf ("Wrong Address...\n\n");
        return MV_ERROR;
    }

    printf ("Enter num of registers to read (%xh max): ", MV_PP_PCI_MAX_PRINT_LEN-1);
    if (mvBspServicesInputHexNumber(&num) != MV_OK)
      return MV_ERROR;
    if (num > MV_PP_PCI_MAX_PRINT_LEN)
    {
        printf ("Wrong num of registers...\n\n");
        return MV_ERROR;
    }

    memset (rx_buf, '\0', MV_PP_PCI_MAX_PRINT_LEN);
    for (i = 0; i < (num + (0x10 - num%0x10)); i++) 
    { 
    rx_buf[i] = *( ((MV_U32 *)addr)+i);
    } 
    printf("\n\nPP_%d: Custom read Query:\n"
               "-------------------------\n", pp_num);
    printf("First Addr.: 0x%x", addr);
    mvBspServicesDisplayMemoryTable((MV_U8*)rx_buf, num, sizeof(MV_U32), 0);

    return MV_OK;
}




/*******************************************************************************
* pp_pci_write_test
*
* DESCRIPTION:
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
MV_STATUS pp_pci_write_reg(void)
{
    MV_U32 pp_num, offset_addr;
    MV_U32 pci_if;
    MV_U32 bus_id; 
    MV_U32 dev;
    MV_U32 reg;
    MV_U32 bar_0_base_addr, bar_0_size, bar_1_base_addr, bar_1_size;
    int val = 0, mask = 0xFFFFFFFF;
    char next;
    MV_U32 * reg_addr;

    printf ("pp_pci_write_reg:\n"
            "-----------------\n");

    printf ("Enter PP device num (1, 2, ...): ");
    if (mvBspServicesInputHexNumber(&pp_num) != MV_OK)
      return MV_ERROR;
    if (pp_num > MAX_PP_PCI_DEVS)
    {
        printf ("Wrong PP num...\n\n");
        return MV_ERROR;
    }

    printf ("Enter PEX/PCI INTERFACE (PEX=0 or PCI=1):");
    if (mvBspServicesInputHexNumber(&pci_if) != MV_OK)
        return MV_ERROR;

    printf ("Enter PCI bus_id : ");
    if (mvBspServicesInputHexNumber(&bus_id) != MV_OK)
        return MV_ERROR;

    dev = GET_PCI_DEVICE_NUM(pp_num); 


    /* Bar_0 */

    reg = PCI_MEMORY_BAR_BASE_ADDR(0); 

    /* read base addr */
    diagIfPciConfigRead(pci_if, bus_id, dev, 0, reg, &bar_0_base_addr);

    /* write oxffffffff to the bar to get the size */
    diagIfPciConfigWrite(pci_if, bus_id, dev, 0, reg, 0xffffffff);

    /* read size */
    diagIfPciConfigRead(pci_if, bus_id, dev, 0, reg, &bar_0_size); 
    bar_0_size = ~bar_0_size + 1;

    /* restore original value */
    diagIfPciConfigWrite(pci_if, bus_id, dev, 0, reg, bar_0_base_addr);


    /* Bar_1 */

    #if defined(DB_MNG) 
        reg = 0x18;  
    #else  
        reg = PCI_MEMORY_BAR_BASE_ADDR(1); 
    #endif 

    /* read base addr */
    diagIfPciConfigRead( pci_if, bus_id, dev, 0, reg, &bar_1_base_addr);

    /* write oxffffffff to the bar to get the size */
    reg = PCI_MEMORY_BAR_BASE_ADDR(1);
    diagIfPciConfigWrite(pci_if, bus_id, dev, 0, reg, 0xffffffff);

    /* read size */
    bar_1_size = 0;
    diagIfPciConfigRead(pci_if, bus_id, dev, 0, reg, &bar_1_size);
    bar_1_size = ~bar_1_size + 1;

    /* restore original value */
    diagIfPciConfigWrite(pci_if, bus_id, dev, 0, reg, bar_1_base_addr);


    printf ("PP_%d:\n"
            "\tBAR_0 BASE = 0x%x     BAR_0 SIZE = 0x%x\n"
            "\tBAR_1 BASE = 0x%x     BAR_1 SIZE = 0x%x\n\n",
            pp_num, bar_0_base_addr, bar_0_size, 
                    bar_1_base_addr, bar_1_size);
    next = 'Y';
    do
    {
    printf ("Enter Register Offset Addr. (BAR_1 Only): ");
    if (mvBspServicesInputHexNumber(&offset_addr) != MV_OK)
      return MV_ERROR; 
    if (offset_addr > bar_1_size)
    {
        printf ("Wrong Offset Addr. (Too big)...\n\n");
        return MV_ERROR;
    } 

    printf ("Enter Register value (32 bit): ");
    if (mvBspServicesInputHexNumber(&val) != MV_OK)
      return MV_ERROR;  

    printf ("Enter Register Mask (Default = 0xFFFFFFFF): ");
    if (mvBspServicesInputHexNumber(&mask) != MV_OK)
      return MV_ERROR;
    if (!mask)
        mask = 0xFFFFFFFF;

    reg_addr = (MV_U32 *) (bar_1_base_addr + offset_addr);
    * reg_addr = (val & mask); 

    printf ("More ?  Type \"q\" to quit: ");
    next = getc();
    printf("\n\n");
    }
    while( (next != 'Q') && (next != 'q') );

        return MV_OK;
}




#endif  /* PP_PCI_TEST */
