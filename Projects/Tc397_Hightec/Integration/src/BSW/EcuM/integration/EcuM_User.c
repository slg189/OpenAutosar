/***********************************************************************************************************************
 *
 * COPYRIGHT RESERVED, ETAS GmbH, 2020. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
 * Component 	: EcuM_User.h
 * Created on	: Apr 15, 2021
 * Version   	: 1.0
 * Author		 Nguyen Thanh Binh
 *  * This file is implement user call out for wakeup source
 **********************************************************************************************************************/

/*
 **************************************************************************************************
 * Includes
 **************************************************************************************************
 */

#include "EcuM.h" 
#include "EcuM_User.h"

/*
 **************************************************************************************************
 * Variables
 **************************************************************************************************
 */


/**********************************************************************************
  Function name     :   EcuM_User_CheckWKSourceAll
  Description       :   The ECU Manager Module calls EcuM_User_CheckWKSourceAll to check what is the wakeup source by calling the CDD API
  Parameter (in)    :   None
  Parameter (inout) :   None
  Parameter (out)   :   None
  Return value      :   None
  Remarks           :   The CDD API for checking wakeup reason is a stub. The user shall implement the complete API in CDD.
***********************************************************************************/
#define ECUM_START_SEC_CALLOUT_CODE
#include "EcuM_MemMap.h"
FUNC( void, ECUM_CALLOUT_CODE ) EcuM_User_CheckWKSourceAll(void)
{
  wakeup_src_t wakeup_src;
  wakeup_src = CDD_Check_Wakeup_Src();
  switch (wakeup_src) 
  {
    case KL15_WU:
      EcuM_SetWakeupEvent(EcuMConf_EcuMWakeupSource_ECUM_WKSOURCE_KL15);
      break;
    case CANMSG_WU:
      EcuM_SetWakeupEvent(EcuMConf_EcuMWakeupSource_ECUM_WKSOURCE_CANMSG);
      break;
    default:
      break;
  }
}

/**********************************************************************************
  Function name     :   CDD_Check_Wakeup_Src
  Description       :   This is a stub of API to check wakeup reason in CDD
  Parameter (in)    :   None
  Parameter (inout) :   None
  Parameter (out)   :   None
  Return value      :   None
  Remarks           :   The CDD API for checking wakeup reason is a stub. The user shall implement the complete API in CDD.
***********************************************************************************/
static wakeup_src_t CDD_Check_Wakeup_Src(void)
{
  static volatile wakeup_src_t wakeup_src = KL15_WU; //by default is KL15 wakeup
  /* code to check wakeup source */
  return wakeup_src;
}
#define ECUM_STOP_SEC_CALLOUT_CODE
#include "EcuM_MemMap.h"



