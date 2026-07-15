/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  IoHwAb_DIO
 * Description: Testcode for IoHwAb_DIO
 * Version         Author:       Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_IoHwAb_DIO.h"

#include "Dio.h"

#define IOHWAB_DIO_START_SEC_CODE
#include "IoHwAb_DIO_MemMap.h"
FUNC (void , IoHwAb_DIO_CODE) IoHwAb_DIO_Set_0_func/* return value & FctID */
(
		VAR(boolean, AUTOMATIC) dio_status
)
{
	Std_ReturnType retValue = RTE_E_OK;
	Dio_WriteChannel((Dio_ChannelType)DioConf_DioChannel_DIO_Write0,(Dio_LevelType)dio_status);
}

#define IOHWAB_DIO_STOP_SEC_CODE
#include "IoHwAb_DIO_MemMap.h"
#define IOHWAB_DIO_START_SEC_CODE
#include "IoHwAb_DIO_MemMap.h"

FUNC (void , IoHwAb_DIO_CODE) IoHwAb_DIO_Get_1_func/* return value & FctID */
(
		P2VAR(boolean, AUTOMATIC, RTE_APPL_DATA) dio_status
)
{
	Std_ReturnType retValue = RTE_E_OK;
	*dio_status	=Dio_ReadChannel(DioConf_DioChannel_DIO_Read1);
}

#define IOHWAB_DIO_STOP_SEC_CODE
#include "IoHwAb_DIO_MemMap.h"
#define IOHWAB_DIO_START_SEC_CODE
#include "IoHwAb_DIO_MemMap.h"
FUNC (void , IoHwAb_DIO_CODE) IoHwAb_DIO_Get_0_func/* return value & FctID */
(
		P2VAR(boolean, AUTOMATIC, RTE_APPL_DATA) dio_status
)
{
	Std_ReturnType retValue = RTE_E_OK;
	*dio_status	=Dio_ReadChannel(DioConf_DioChannel_DIO_Read0);
}

#define IOHWAB_DIO_STOP_SEC_CODE
#include "IoHwAb_DIO_MemMap.h"
#define IOHWAB_DIO_START_SEC_CODE
#include "IoHwAb_DIO_MemMap.h"
FUNC (void , IoHwAb_DIO_CODE) IoHwAb_DIO_Set_1_func/* return value & FctID */
(
		VAR(boolean, AUTOMATIC) dio_status
)
{
	Std_ReturnType retValue = RTE_E_OK;
	Dio_WriteChannel((Dio_ChannelType)DioConf_DioChannel_DIO_Write1,(Dio_LevelType)dio_status);
}

#define IOHWAB_DIO_STOP_SEC_CODE
#include "IoHwAb_DIO_MemMap.h"
