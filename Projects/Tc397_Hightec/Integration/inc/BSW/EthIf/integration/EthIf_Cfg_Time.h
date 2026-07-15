/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



#ifndef ETHIF_CFG_TIME_H
#define ETHIF_CFG_TIME_H

/* This template file is used to get the current time of the system is us. */
/* The source to fetch the time is project specific and can be Os, Mcu etc. */
/* The integrator after choosing the source should modify this file accordingly. */

/* Integration Hint : Include the neccesary headers here for getting the time from a particular source. */
/* No version check added as the used apis are not Autosar provided. */
#include "Mcu.h"

/* Hint : Project should include this header file. */
/* It is required for the runtime limitation of EthIf MainfunctionRx. */
/* If runtime limitation is enabled, ETHIF_RX_RUNTIME_LIMIT_ENABLED is set to STD_ON and will be included in the file EthIf.c */
/* The API EthIf_Prv_MainFunctionRx_PriorityProcessing() in EthIf.c calls a macro function for fetching the time. */
/* The macro prototype is ETHIF_GET_CURRTIME_IN_US() and return value shall be in us. */
/* If there is a wrap around in the timer value, EthIf takes care internally to calculate the correct runtime. */

/* A sample integration code is given below. */
#if ( ETHIF_RX_RUNTIME_LIMIT_ENABLED == STD_ON ) /* Compiler switch to enable/disable macro to get current time. */

/* Macro to get the current Time in us.*/
#define ETHIF_GET_CURRTIME_IN_US()                MCU_RB_TIM0TICKS_TO_US(Mcu_Rb_GetSysTimePart(TIM0))


#endif /* ( ETHIF_RX_RUNTIME_LIMIT_ENABLED == STD_ON ) */

#endif /* ETHIF_CFG_TIME_H */


