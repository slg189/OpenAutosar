

#ifndef CAN_H
#define CAN_H

#include "Can_17_McmCan.h"


#define CAN_AR_RELEASE_MAJOR_VERSION	4
#define CAN_AR_RELEASE_MINOR_VERSION	2	/* CIX2SGH */
#define CAN_AR_RELEASE_REVISION_VERSION   3

#define Can_Init								Can_17_McmCan_Init

#if (CAN_CHANGE_BAUDRATE_API == STD_ON)
	#define Can_CheckBaudrate(a,b) 				Can_17_McmCan_CheckBaudrate(a,b)
#else
	#define Can_CheckBaudrate(a,b)     			ERROR_CanChangeBaudrateApi_NOT_SELECTED
#endif /* #if (CAN_CHANGE_BAUDRATE_API == STD_ON) */

#if (CAN_CHANGE_BAUDRATE_API == STD_ON)
	#define CanP_ChangeBaudrate(a,b)      		Can_17_McmCan_ChangeBaudrate(a,b)
#else
	#define CanP_ChangeBaudrate(a,b)      		ERROR_CanChangeBaudrateApi_NOT_SELECTED
#endif /* #if (CAN_CHANGE_BAUDRATE_API == STD_ON) */

#define Can_SetControllerMode(a,b) 				Can_17_McmCan_SetControllerMode(a,b)
#define Can_DisableControllerInterrupts(a) 		Can_17_McmCan_DisableControllerInterrupts(a)
#define Can_EnableControllerInterrupts(a) 		Can_17_McmCan_EnableControllerInterrupts(a)
#define Can_Write(a,b) 							Can_17_McmCan_Write(a,b)

#define Can_MainFunction_Wakeup()  				Can_17_McmCan_MainFunction_Wakeup()
#define Can_MainFunction_Mode() 				Can_17_McmCan_MainFunction_Mode()
#define Can_CheckWakeup(a)  					Can_17_McmCan_CheckWakeup(a)
#define Can_IsrBusOffHandler(a)  				Can_17_McmCan_IsrBusOffHandler(a)
#define Can_IsrReceiveHandler(a)  				Can_17_McmCan_IsrReceiveHandler(a)
#define Can_IsrTransmitHandler(a) 				Can_17_McmCan_IsrTransmitHandler(a)

#define Can_MainFunction_Write(void) 			Can_17_McmCan_MainFunction_Write(void)
#define Can_MainFunction_Read(void) 			Can_17_McmCan_MainFunction_Read(void)
#define Can_MainFunction_BusOff(void)			Can_17_McmCan_MainFunction_BusOff(void)

#define CanConf_CanController_Can_Network_CANNODE_0   (Can_17_McmCanConf_CanController_Can_Network_CANNODE_0)
#define CanConf_CanController_Can_Network_CANNODE_1    (Can_17_McmCanConf_CanController_Can_Network_CANNODE_1)

#endif /* CAN_H */
