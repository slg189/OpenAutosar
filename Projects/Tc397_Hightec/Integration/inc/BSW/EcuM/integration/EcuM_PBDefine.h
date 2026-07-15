/*AGT1HC: This file is created to map the data configuration between BSW and MCAL*/
/*
 * EcuM_PBDefine.h
 *
 *  Created on: Mar - 15 -2018
 *      Author: AGT1HC
 */

#ifndef BASICSOFTWARE_SRC_BSW_GEN_ECUM_INTEGRATION_ECUM_PBDEFINE_H_
#define BASICSOFTWARE_SRC_BSW_GEN_ECUM_INTEGRATION_ECUM_PBDEFINE_H_


#include "Mcu.h"
#include "Port.h"
// #include "Gpt.h"
#include "Can.h"
#include "Smu.h"
// #include "Gpt.h"
#include "Wdg_17_Scu.h"




#define Mcu_Config                  Mcu_Config
#define Port_Config                 Port_Config
#define Can_ConfigPtr               Can_17_McmCan_Config
#define Dio_Config                  Dio_Config
#define Fls_Config                  Fls_17_Dmu_Config
#define Smu_Config                  Smu_Config
#define Wdg_17_Scu_Config           Wdg_17_Scu_Config_0
#define Gpt_Config                  Gpt_Config

extern void McuFunc_InitializeClock(void);

#endif /* BASICSOFTWARE_SRC_BSW_GEN_ECUM_INTEGRATION_ECUM_PBDEFINE_H_ */
