/* *****************************************************************************
 * BEGIN: Banner
 *-----------------------------------------------------------------------------
 *                                 ETAS GmbH
 *                      D-70469 Stuttgart, Borsigstr. 14
 *-----------------------------------------------------------------------------
 *    Administrative Information (automatically filled in by ISOLAR)         
 *-----------------------------------------------------------------------------
 * Project :    ETAS Entry Platfrom
 * Component:  SensorActuator_DIO
 * Description: Testcode for SensorActuator_DIO
 * Version         Author        Date               Update information
 * 1.0             WZO4SGH       9-Nov-2020         Create software
 * 1.1             AGT1HC        19-Nov-2021        Update the Banner
 * 1.2             HAD1HC        13-Apr-2021        Update Memmap
 *-----------------------------------------------------------------------------
 * END: Banner
 ******************************************************************************/

#include "Rte_SensorActuator_DIO.h"

#define SENSORACTUATOR_DIO_START_SEC_VAR_INIT_8
#include "SensorActuator_DIO_MemMap.h"
boolean DIO_Read[2]={0,0};
boolean DIO_Write_One = 255 ;
#define SENSORACTUATOR_DIO_STOP_SEC_VAR_INIT_8
#include "SensorActuator_DIO_MemMap.h"
#define SENSORACTUATOR_DIO_START_SEC_CODE
#include "SensorActuator_DIO_MemMap.h"
FUNC (void, SensorActuator_DIO_CODE) DIO_Set_func/* return value & FctID */
(
		void
)
{

	boolean dio_status1;
	Std_ReturnType syncCall1;
	boolean dio_status2;
	Std_ReturnType syncCall2;
	Std_ReturnType retValue = RTE_E_OK;
	/*  -------------------------------------- Data Read -----------------------------------------  */

	/*  -------------------------------------- Server Call Point  --------------------------------  */
	if(DIO_Write_One==1)
	{

		syncCall1 = Rte_Call_RPort_DIO_Set_1_IoHwAb_IO_Set(1);

		syncCall2 = Rte_Call_RPort_DIO_Set_0_IoHwAb_IO_Set(1);
	}
	
	if(DIO_Write_One==0)
	{

		syncCall1 = Rte_Call_RPort_DIO_Set_1_IoHwAb_IO_Set(0);

		syncCall2 = Rte_Call_RPort_DIO_Set_0_IoHwAb_IO_Set(0);
	}
}

#define SENSORACTUATOR_DIO_STOP_SEC_CODE
#include "SensorActuator_DIO_MemMap.h"
#define SENSORACTUATOR_DIO_START_SEC_CODE
#include "SensorActuator_DIO_MemMap.h"
FUNC (void, SensorActuator_DIO_CODE) DIO_Get_func/* return value & FctID */
(
		void
)
{
	boolean dio_status3;
	Std_ReturnType syncCall1;
	boolean dio_status4;
	Std_ReturnType syncCall2;
	Std_ReturnType retValue = RTE_E_OK; 

	syncCall1 = Rte_Call_RPort_DIO_Get_0_IoHwAb_IO_Get(&DIO_Read[0]);
	syncCall2 = Rte_Call_RPort_DIO_Get_1_IoHwAb_IO_Get(&DIO_Read[1]);
}

#define SENSORACTUATOR_DIO_STOP_SEC_CODE
#include "SensorActuator_DIO_MemMap.h"
