/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  ASW_DIAG
 * Description: Testcode for ASW_DIAG
 * Version         Author:       Date               Update information
 * 1.0             HAD1HC        7-Nov-2018         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        04-Mar-2021        Update for new requirements
 * 1.3             HAD1HC        13-Apr-2021        Update Memmap   
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Std_Types.h"
#include "rte_Type.h"
#include "rte.h"

#define ASW_DCM_START_SEC_VAR_INIT_8
#include "ASW_DCM_MemMap.h"
uint8 RamBlock_DID_VehicleSpeed[17] = {0};
uint8 RamBlock_DID_SystemFaultRank[4] = {0};
uint8 RamBlock_DID_IOControl[17] = {0};
uint8 RamBlock_DID_ReadWritebyAddress[4] = {0};
#define ASW_DCM_STOP_SEC_VAR_INIT_8
#include "ASW_DCM_MemMap.h"