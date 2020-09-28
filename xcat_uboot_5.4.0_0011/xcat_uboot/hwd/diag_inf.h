/*===========================================================================*/
#ifndef exp_Diag_inf_PKS_INCLUDED
#define exp_Diag_inf_PKS_INCLUDED

#include "mvTypes.h"
/*!**************************************************RND Template version 4.1
*!                      P A C K A G E       S P E C I F I C A T I O N
*!==========================================================================
*$ TITLE: 
*!--------------------------------------------------------------------------
*$ FILENAME: diag_inf.h
*!--------------------------------------------------------------------------
*$ SYSTEM, SUBSYSTEM: 
*!--------------------------------------------------------------------------
*$ AUTHORS: oren-v,radlanuser
*!--------------------------------------------------------------------------
*$ LATEST UPDATE:                      
*!**************************************************************************
*!
*!**************************************************************************
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
*!
*!**************************************************************************
*!*/
/*!**************************************************************************
*$              EXTERNAL DECLARATIONS (IMPORT AND EXPORT)
*!**************************************************************************
*!*/

/*!**************************************************************************
*$              PUBLIC DECLARATIONS (EXPORT)
*!**************************************************************************
*!*/
    typedef MV_U8 mvBspDiagBooleanVoidFNC (void);          /* boolean function, no parameters */
    typedef void  mvBspDiagVoidVoidFNC (void);              /* void fnc, no params */
    typedef void  mvBspVoidParamFNC (MV_U32 *param, ... );  /* void fnc, no params */
    typedef MV_U8 mvBspDiagBooleanParamFNC (MV_U32 *param);/* boolean fnc, with param */
    typedef void  mvBspDiagBooleanFNC (MV_U8 flag);
	

/*===========================================================================*/
/*!**************************************************RND Template version 4.1
 *!          S T R U C T U R E   T Y P E   D E F I N I T I O N
 *!==========================================================================
 *$ TITLE: Generic diagnostic structures.
 *!--------------------------------------------------------------------------
 *$ FILENAME: 
 *!--------------------------------------------------------------------------
 *$ SYSTEM, SUBSYSTEM: 
 *!--------------------------------------------------------------------------
 *$ AUTHORS: AharonG,oren-v,OrenV
 *!--------------------------------------------------------------------------
 *$ LATEST UPDATE: 19-Jan-2004, 10:50 AM CREATION DATE: 15-Jun-2003
 *!**************************************************************************
 *!
 *!**************************************************************************
 *!
 *$ GENERAL DESCRIPTION:
 *!
 *$ INCLUDE REQUIRED:
 *!
 *$ REMARKS:
 *!
 *!**************************************************************************
 *!*/
 
  /* void function for string input */
  typedef MV_U8 mvBspDiagBoolThreeParamFNC(MV_U32 *param_1,
                                           MV_U32 *param_2,
                                           MV_U32 *param_3);

  typedef struct mvBspDiagDramProbeParamStructT
  {
    MV_U32   address;        /* address to probe */
    MV_U32   size;           /* size to probe */
    MV_U8   operation;     /* 0 = write; 1 = read */
    MV_U32   *value_ptr;     /* pointer to value to write or read */
  } mvBspDiagDramProbeParamStruct;

  typedef struct mvBspDiagDramTestParamStructT
  {
    MV_U32   base_address;      /* dram base address */
    MV_U32   first_size_offset; /* offset from base_address */
    MV_U8    dev_amount;        /* dram amount of devices */
    MV_U8    data_bus_width;    /* dram bus width */
    MV_U32   mem_size;          /* memory size for dram test, if 0 - apply size detect algorithm */
    mvBspDiagBooleanParamFNC* mem_probe_func; /* memory probe function */
  } mvBspDiagDramTestParamStruct;

  typedef struct mvBspDiagFlashTestParamStructT
  {
    MV_U32 base_addr;            /* device base address */
    MV_U32 page_size;            /* device page size */
    MV_U32 overall_size;         /* device overall size */
    MV_U32 tested_area_size;     /* device tested size */
    MV_U32 data_bus_width;       /* device bus width */
    MV_U32 serial_dev_amount;    /* amount of devices in serial configuration */
    MV_U32 reserved_offsets[3];  /* offsets to not be deleted (boots area) */
    MV_U32 reserved_sizes[3];    /* The size of each reserved area */
  } mvBspDiagFlashTestParamStruct;

  typedef struct mvBspDiagNvramTestParamStructT
  {
    MV_U32 base_addr;            /* base address */
    MV_U32 nvram_size;           /* device size */
    MV_U32 data_bus_width;       /* device bus width */
    MV_U32 serial_dev_amount;    /* amount of devices in serial configuration */
  } mvBspDiagNvramTestParamStruct;

  typedef struct mvBspDiagEthernetTestParamStructT
  {
    MV_U8 int_loopback_supported;      /* internal loopback supported param */
    MV_U8 ext_loopback_supported;      /* external loopback supported param */
    MV_U8 phy_10_loopback_supported;   /* phy 10MB loopback supported param */
    MV_U8 phy_100_loopback_supported;  /* phy 100Mb loopback supported param */
    MV_U8 phy_auto_loopback_supported; /* phy autoneg loopback supported param */
  } mvBspDiagEthernetTestParamStruct;

  typedef struct mvBspDiagCacheFunctionStructT
  {
    mvBspDiagBooleanFNC* cache_enable_fnc;      /* enable/disable cache function */
    mvBspDiagBooleanFNC* inst_cache_enable_fnc; /* enable/disable inst. cache function*/
    mvBspDiagBooleanFNC* data_cache_enable_fnc; /* enable/disable data cache function*/
  } mvBspDiagCacheFunctionStruct;

  typedef struct mvBspDiagPciTestParamsStructT
  {
    mvBspDiagBooleanVoidFNC*  pci_bus_test_fnc;     /* test pci bus function */
    mvBspDiagBooleanVoidFNC*  pci_find_dev_fnc;     /* list of PCI devices on board function */
    mvBspDiagBooleanVoidFNC* pci_connect_to_dev_fnc;/* PCI config cycle function */
  } mvBspDiagPciTestParamsStruct;

  typedef struct mvBspCpldTestParamsStructT
  {
    mvBspDiagBooleanVoidFNC* sfp_test_fnc;    /* sfp presence test function */
    mvBspDiagBooleanVoidFNC* led_test_fnc;    /* led test function */
    mvBspDiagBooleanVoidFNC* power_test_fnc;  /* power supply test function */
    mvBspDiagBooleanVoidFNC* fans_test_fnc;   /* fans test function */
    mvBspDiagBooleanVoidFNC* hw_data_test_fnc;/* read hw information function */
  }mvBspCpldTestParamsStruct;

  typedef struct mvBspWatchDogTestParamsStructT
  {
    mvBspDiagVoidVoidFNC* watchdog_stop_strobe_fnc; /* watchdog test function */
    MV_U32 watchdog_reset_time; /* time in sec from last strobe to device reset */
  }mvBspWatchDogTestParamsStruct;

  typedef struct mvBspTimersTestParamsStructT
  {
    mvBspDiagBooleanVoidFNC* main_timers_test_fnc; /* CPU timers test function */
    mvBspDiagBooleanVoidFNC* aux_timers_test_fnc;  /* Aux timers test function */
  }mvBspTimersTestParamsStruct;

  typedef struct mvBsp_i2c_devicesStructure
  {
    MV_U8 i2c_device_address;                    /* i2c device address   */
    MV_U8 i2c_device_index;                      /* i2c device bus index */
  }mvBsp_i2c_devicesStruct;

  typedef struct mvBspI2cTestParamsStructT
  {
    mvBsp_i2c_devicesStruct* i2c_devices_array; /* array of i2c devices */
    MV_U32 i2c_devices_number;                  /* number of i2c device */
  }mvBspI2cTestParamsStruct;

  typedef struct mvBspPhyDevicesStructT
  {
    MV_U32 asic_pci_id;                          /* asic's pci id select */
    MV_U32 phys_number;                          /* number of phys for this asic */
    MV_U32 ports_per_phy;                        /* number of ports per phy */
  }mvBspPhyDevicesStruct;

  typedef struct mvBspPhyTestParamsStructT
  {
    mvBspPhyDevicesStruct* devices_array;      /* array of asics */
    MV_U32 asics_number;                       /* number of asics on board */
    mvBspDiagBoolThreeParamFNC* phy_read_fnc;  /* phy read function */
    mvBspDiagBoolThreeParamFNC* phy_write_fnc; /* phy write function */
  }mvBspPhyTestParamsStruct;

  #if UBOOT_HWD_DIAG_INF_DEBUG
  typedef struct mvBspSpecialTestsStructT
  {
    mvBspVoidParamFNC* special_test_fnc;        /* special test function */
    char* special_test_name;                    /* special test name */
    mvBspDiagTestComponentsEnum special_test_type; /* special test type */
    MV_U8 include_in_test_all;
  }mvBspSpecialTestsStruct;
  #endif 

/*===========================================================================*/


#endif
/*$ END OF Diag_inf */
