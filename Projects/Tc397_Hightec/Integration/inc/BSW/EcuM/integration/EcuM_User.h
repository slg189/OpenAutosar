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


#ifndef INTEGRATION_ECUM_USER_H
#define INTEGRATION_ECUM_USER_H

typedef enum{
  NO_REASON_WU = 0,
  KL15_WU,
  CANMSG_WU,
  RESERVED
} wakeup_src_t;

FUNC( void, ECUM_CALLOUT_CODE ) EcuM_User_CheckWKSourceAll(void);
static wakeup_src_t CDD_Check_Wakeup_Src(void);

#endif /* INTEGRATION_ECUM_USER_H */
