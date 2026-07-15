/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  IoHwAb_ADC
 * Description: Testcode for IoHwAb_ADC
 * Version         Author:       Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#ifndef IOHWAB_ICU_H
#define IOHWAB_ICU_H
#include "Std_Types.h"

#define IOHWAB_ADC_START_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"
 extern FUNC (void, IoHwAb_ADC_CODE) IoHwAb_ADC_Init_func(void);
#define IOHWAB_ADC_STOP_SEC_CODE
#include "IoHwAb_ADC_MemMap.h"
#endif