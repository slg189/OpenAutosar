/*
 * Lin.h
 *
 *  Created on: Apr 11, 2018
 *      Author: HAD1HC
 */

#ifndef LIN_H_
#define LIN_H_

#include "Lin_17_AscLin.h"

#define LIN_AR_RELEASE_MAJOR_VERSION    LIN_17_ASCLIN_AR_RELEASE_MAJOR_VERSION
#define LIN_AR_RELEASE_MINOR_VERSION    LIN_17_ASCLIN_AR_RELEASE_MINOR_VERSION
#define LIN_AR_RELEASE_REVISION_VERSION LIN_17_ASCLIN_AR_RELEASE_PATCH_VERSION

#define LinConf_LinChannel_LinChannel_0     Lin_17_AscLinConf_LinChannel_LinChannel_0

#define Lin_Init						Lin_17_AscLin_Init
#define Lin_WakeupValidation			Lin_17_AscLin_CheckWakeup
#define Lin_GetVersionInfo				Lin_17_AscLin_GetVersionInfo
#define Lin_SendFrame					Lin_17_AscLin_SendFrame
#define Lin_GoToSleep					Lin_17_AscLin_GoToSleep
#define Lin_GoToSleepInternal			Lin_17_AscLin_GoToSleepInternal
#define Lin_Wakeup						Lin_17_AscLin_Wakeup
#define Lin_GetStatus					Lin_17_AscLin_GetStatus
#define Lin_CheckWakeup                 Lin_17_AscLin_CheckWakeup
#define Lin_WakeupInternal              Lin_17_AscLin_WakeupInternal

#endif
