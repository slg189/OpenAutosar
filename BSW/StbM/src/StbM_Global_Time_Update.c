

/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/

#include "StbM_Types.h"

/* The below mentioned #defines STBM_GETCURRENTTIME_DEFINED and STBM_GETTIMEBASESTATUS_DEFINED are defined to resolve
   the double declaration warnings of the API's StbM_GetCurrentTime and StbM_GetTimeBaseStatus.
   1. When RTE is ON declarations are generated form the RTE end the #defines are defined and in source file,
   declarations are not included in StbM.h for StbM module.Since #defines are not defined for below modules ,
   the declarations are available for the below modules.
   2. When RTE is OFF declarations are not generated from the RTE end,#defines are not defined in the source file
   the declaratins of the API's are included in the StbM.h for StbM module.Since #defines are not defined for below modules ,
   the declarations are available for the below modules. */

#if (STBM_ECUC_RB_RTE_IN_USE == STD_ON)
#define STBM_GETCURRENTTIME_DEFINED
#define STBM_GETTIMEBASESTATUS_DEFINED
#endif  /*(STBM_ECUC_RB_RTE_IN_USE == STD_ON) */

#include "StbM.h"
#if (STBM_ECUC_RB_RTE_IN_USE == STD_ON)
/*The function declarations of RTE interfaced API's will be generated in the Rte_StbM.h */
#include "Rte_StbM.h"
#endif  /*(STBM_ECUC_RB_RTE_IN_USE == STD_ON) */

#if(STBM_DEV_ERROR_DETECT == STD_ON) /*Det included when Det is enabled in StbM*/
#include "Det.h"
/*#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != 4))
#error "DET AUTOSAR major version undefined or mismatched"
#elif (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != 2))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif*/

#endif /*#if(STBM_DEV_ERROR_DETECT == STD_ON)*/

#if (STBM_NVM_ENABLE == STD_ON)
#include "NvM.h"
/*#if (!defined(NVM_AR_RELEASE_MAJOR_VERSION) || (NVM_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))
#error "NVM AUTOSAR major version undefined or mismatched"
#elif (!defined(NVM_AR_RELEASE_MINOR_VERSION) || (NVM_AR_RELEASE_MINOR_VERSION != STBM_AR_RELEASE_MINOR_VERSION))
#error "NVM AUTOSAR minor version undefined or mismatched"
#endif*/

#endif /*#if(STBM_NVM_ENABLE == STD_ON)*/

#if (STBM_ETH_ENABLE == STD_ON)

/*[SWS_StbM_00246] If time stamping via Ethernet shall be supported , StbM.c shall include EthIf.h to have
* access to the interface of the EthIf module. */
#include "EthIf.h"
/*#if (!defined(ETHIF_AR_RELEASE_MAJOR_VERSION) || (ETHIF_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))
#error "ETHIF AUTOSAR major version undefined or mismatched"
#elif (!defined(ETHIF_AR_RELEASE_MINOR_VERSION) ||(ETHIF_AR_RELEASE_MINOR_VERSION != STBM_AR_RELEASE_MINOR_VERSION))
#error "ETHIF AUTOSAR minor version undefined or mismatched"
#endif*/

#endif /*#if (STBM_ETH_ENABLE == STD_ON)*/


#include "StbM_Priv.h"
#include "StbM_Inl.h"

/*
**********************************************************************************************************************
* Variables
**********************************************************************************************************************
*/


#define STBM_START_SEC_VAR_CLEARED_16
#include "StbM_MemMap.h"
uint16 StbM_TimeLeap_Past_Count_u16[STBM_CFG_NUM_OF_TIMEBASES];
uint16 StbM_TimeLeap_Future_Count_u16[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_16
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_32
#include "StbM_MemMap.h"
StbM_TimeBaseNotificationType StbM_NotificationEvents_au32[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_32
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
StbM_UserDataType StbM_UserDataArray_ast[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
StbM_TimeStampType StbM_GlobalTimeTupleArray_ast[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
StbM_TimeStampType StbM_GlobalTimeReception_ast[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

/* Array added to store Received Virtual Local Time during BusSetGlobalTime Call*/
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
StbM_VirtualLocalTimeType StbM_VirtualLocalTimeReception_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

/* Array added to maintain Virtual Local Time Tupled with Global Time */
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
StbM_VirtualLocalTimeType StbM_VirtualLocalTimeTupleArray_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

/*
**********************************************************************************************************************
* Local functions decalrations
**********************************************************************************************************************
*/

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

static StbM_TimeBaseStatusType StbM_BusSetGlobalTime_StatusUpdate
(uint8 index,const StbM_TimeStampType * timeStampPtr, StbM_TimeStampType ctTimeStamp);

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
*Service name      : StbM_BusSetGlobalTime_StatusUpdate
*Syntax            : StbM_TimeBaseStatusType StbM_BusSetGlobalTime_StatusUpdate
						(uint8 index,StbM_TimeStampType * timeStampPtr)
*Service ID[hex]   : None
*Sync/Async        : None
*Reentrancy        : None
*Parameters (in)   : index                   - Index of the timebase ID
*                    timeStampPtr            - new time value received
*Parameters (inout): None
*Parameters (out)  : None
*Return value      : StbM_TimeBaseStatusType - timeBaseStatus of the timebase
*Description       : Local function to update the timeBaseStatus
***********************************************************************************************************************/
static StbM_TimeBaseStatusType StbM_BusSetGlobalTime_StatusUpdate
(uint8 index,const StbM_TimeStampType * timeStampPtr,StbM_TimeStampType ctTimeStamp)
{
	/* Local Variable declaration */
	StbM_TimeStampType lDiff_Time_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	uint8 lgreaterTimeStamp_u8    = STBM_ZERO;
	StbM_TimeBaseNotificationType lNotifyMask_u32 = STBM_ZERO ;
	StbM_TimeBaseStatusType lTimebasestatus_st = STBM_ZERO;


	/*  Copy the timebase status into a local variable */
	lTimebasestatus_st = StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus;

	/* [SWS_StbM_00182]
    For each Time Base where a Time Slave or a Time Gateway Slave Port belongs to, an invocation of
    StbM_BusSetGlobalTime() shall check, if the Global Time difference between the Received Time
    (i.e., the updated Time Base value) and the Synclocal Time (i.e., the current Time Base value) exceeds the
    configured threshold of StbMTimeLeapFutureThreshold (ECUC_StbM_00041 : ),
    i.e., TGRx - TLSync > StbMTimeLeapFutureThreshold, if at least one Time Base value has been successfully received before.
    With:
    TLSync = Global Time part of the Synclocal Time Tuple
    TGRx = Global Time part of the Received Time Tuple
    In case the threshold is exceeded the StbM shall set the TIMELEAP_FUTURE bit within timeBaseStatus of the Time Base.
    If the next StbMClearTimeleapCount updates are within the threshold of StbMTimeLeapFutureThreshold the StbM shall
    clear the TIMELEAP_FUTURE bit within timeBaseStatus of the Time Base.
    A threshold of 0 shall deactivate this check. */

	/* [SWS_StbM_00305]
     For each Time Base where a Time Slave or a Time Gateway Slave Port belongs to, an invocation of
     StbM_BusSetGlobalTime() shall check, if the Global Time difference between the Synclocal Time
     (i.e., the current Time Base value) and the Received Time (i.e., the updated Time Base value) exceeds the
     configured threshold of StbMTimeLeapPastThreshold (ECUC_StbM_00042 : ),
     i.e., TLSync - TGRx > StbMTimeLeapPastThreshold, if at least one Time Base value has been successfully received before.
    With:
    TLSync = Global Time part of the Synclocal Time Tuple
    TGRx = Global Time part of the Received Time Tuple
    In case the threshold is exceeded the StbM shall set the TIMELEAP_PAST bit within timeBaseStatus of the Time Base.
    If the next StbMClearTimeleapCount updates are within the threshold of StbMTimeLeapPastThreshold the StbM shall
    clear the TIMELEAP_PAST bit within timeBaseStatus of the Time Base.
    A threshold of 0 shall deactivate this check.*/

	/*[SWS_StbM_00279]: For each Time Base the StbM has a configurable mask StbMStatusNotificationMask , which allows to
mask individually status event notifications.*/
	/* [SWS_StbM_00278]:For Synchronized and Offset Time Bases the StbM shall use a variable NotificationEvents of type
StbM_TimeBaseNotificationType to keep track, if any status event for the referenced Time Base occurs.
If any status event occurs and the corresponding bit in the NotificationMask mask is set, the corresponding bit
in the NotificationEvents variable is set , i.e., NotificationEvents contains the bits for the events, which are
enabled within the NotificationMask mask.*/
	lNotifyMask_u32 = StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMStatusNotificationMask_u32 ;

	/* Calculate the difference between the new time and the current time */
	StbM_Time_DifferenceCalculation(timeStampPtr,&ctTimeStamp,&lDiff_Time_st,&lgreaterTimeStamp_u8);

    /* [SWS_StbM_00182]
    For each Time Domain where a Time Slave belongs to, an invocation of StbM_BusSetGlobalTime() shall check if the time
    difference between the updated Time Base and the current value exceeds the configured threshold of
    StbMTimeLeapFutureThreshold (ECUC_StbM_00041 : ), i.e. TG - TLSync > StbMTimeLeapFutureThreshold,
    if at least one Time Base value has been successfully received before.
    (where:
    -   TLSync: Value of the local instance of the Time Base before the new value of the Global Time is applied
    -   TG: Received value of the Global Time)
    A threshold of 0 shall deactivate this check.
    If the next update is within the threshold the StbM shall clear the TIMELEAP bit within timeBaseStatus of the
    Time Base */
	if(lgreaterTimeStamp_u8==STBM_ONE)
	{
		if(!((StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapFutureThreshold_sec_u32 == STBM_ZERO) &&
					(StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapFutureThreshold_ns_u32 == STBM_ZERO)))
		{
			/*  If at least one timestamp for the Time Base has been successfully received before. */
			if( StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT))
			{
				/*  check whether the difference exceeds the threshold value */
				if((lDiff_Time_st.secondsHi>STBM_ZERO)||
				((lDiff_Time_st.seconds>
				StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapFutureThreshold_sec_u32)) ||
				((lDiff_Time_st.seconds ==
				StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapFutureThreshold_sec_u32)&&
				(lDiff_Time_st.nanoseconds >
				StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapFutureThreshold_ns_u32)))
				{
					/* In case the threshold is exceeded the StbM shall set the TIMELEAP bit within timeBaseStatus of the Time Base. */
					lTimebasestatus_st = StbM_Set_Bit(lTimebasestatus_st,STBM_TIMELEAP_FUTURE_BIT,
					                     (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_TIMELEAP_FUTURE);

                    /* Load Configured ClearTimeleapCount value into Global Array, to reset TimeLeap Bit in TimeBaseStatus
                     * after the clearcount is met */
					StbM_TimeLeap_Future_Count_u16[index] =
					        StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMClearTimeleapCount_u16;

#if (STBM_TIME_CORRECTION ==  STD_ON )
			        /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
			        StbM_ResetOngoingRateCorrections(index);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */

				}
				else
				{
				    /* If the next StbMClearTimeleapCount updates are within the threshold of StbMTimeLeapFutureThreshold the StbM shall
                    clear the TIMELEAP_FUTURE bit within timeBaseStatus of the Time Base.*/
				    if(StbM_TimeLeap_Future_Count_u16[index]!= STBM_ZERO)
				    {
				        StbM_TimeLeap_Future_Count_u16[index]--;
				        if(StbM_TimeLeap_Future_Count_u16[index] == STBM_ZERO)
				        {
				            lTimebasestatus_st = StbM_Reset_Bit(lTimebasestatus_st,STBM_TIMELEAP_FUTURE_BIT,
				                 (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_TIMELEAP_FUTURE_REMOVED);
				            /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
#if (STBM_TIME_CORRECTION ==  STD_ON )
				            /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
				            StbM_ResetOngoingRateCorrections(index);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
				        }
				        else
				        {
				            /* No reset of Timeleap Bit as count is not zero */
				        }
				    }
				    else
				    {
				        /* Count is not decremented since the it is not non zero value */
				    }

				}
				/* The StbM_TimeLeap_Past_Count_u16 count should be decremented since there is no leap in the PAST*/
				if(StbM_TimeLeap_Past_Count_u16[index]!= STBM_ZERO)
				{
				    StbM_TimeLeap_Past_Count_u16[index]--;
				    if(StbM_TimeLeap_Past_Count_u16[index] == STBM_ZERO)
				    {
				        lTimebasestatus_st = StbM_Reset_Bit(lTimebasestatus_st,STBM_TIMELEAP_PAST_BIT,
				                (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_TIMELEAP_PAST_REMOVED);
				        /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
#if (STBM_TIME_CORRECTION ==  STD_ON )
				        /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
				        StbM_ResetOngoingRateCorrections(index);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
				    }
				    else
				    {
				        /* No reset of Timeleap Bit as count is not zero */
				    }
				}
				else
				{
				    /* Count is not decremented since the it is not non zero value */
				}
			}
		}
	}
    /* [SWS_StbM_00305]
     For each Time Domain where a Time Slave or a Time Gateway Slave Port belongs to, an invocation of
     StbM_BusSetGlobalTime() shall check, if the time difference between the current and the updated Time Base value
     exceeds the configured threshold of StbMTimeLeapPastThreshold (ECUC_StbM_00042 : ), if the time difference between
     the current and the updated Time Base value exceeds the configured threshold of StbMTimeLeapPastThreshold
     (ECUC_StbM_00042 : ), i.e. TLSync - TG > StbMTimeLeapPastThreshold,
     if at least one Time Base value has been successfully received before.
     (where:
     -   TLSync = Value of the local instance of the Time Base before the new value of the Global Time is applied
     -   TG = Received value of the Global Time)
     In case the threshold is exceeded the StbM shall set the TIMELEAP_PAST bit within timeBaseStatus of the Time Base*/
	else
	{
		if(!((StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapPastThreshold_sec_u32 == STBM_ZERO) &&
					(StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapPastThreshold_ns_u32 == STBM_ZERO)))
		{
			/*  If at least one timestamp for the Time Base has been successfully received before. */
			if( StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT))
			{
				/*  check whether the difference exceeds the threshold value */
				if((lDiff_Time_st.secondsHi>STBM_ZERO)||
				((lDiff_Time_st.seconds>
				StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapPastThreshold_sec_u32)) ||
				((lDiff_Time_st.seconds ==
				StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapPastThreshold_sec_u32) &&
				(lDiff_Time_st.nanoseconds >
				StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMTimeLeapPastThreshold_ns_u32)))
				{
					/* In case the threshold is exceeded the StbM shall set the TIMELEAP bit within timeBaseStatus of the Time Base. */
					lTimebasestatus_st = StbM_Set_Bit(lTimebasestatus_st,STBM_TIMELEAP_PAST_BIT,
					                  (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_TIMELEAP_PAST);

					/* Load Configured ClearTimeleapCount value into Global Array, to reset TimeLeap Bit in TimeBaseStatus
					 * after the clearcount is met */
					StbM_TimeLeap_Past_Count_u16[index] =
					        StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMClearTimeleapCount_u16;

#if (STBM_TIME_CORRECTION ==  STD_ON )
			        /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
			        StbM_ResetOngoingRateCorrections(index);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
				}
				else
				{
				    /* If the next StbMClearTimeleapCount updates are within the threshold of StbMTimeLeapPastThreshold the StbM shall
                    clear the TIMELEAP_PAST bit within timeBaseStatus of the Time Base.*/
				    if(StbM_TimeLeap_Past_Count_u16[index]!= STBM_ZERO)
				    {
				        StbM_TimeLeap_Past_Count_u16[index]--;
				        if(StbM_TimeLeap_Past_Count_u16[index] == STBM_ZERO)
				        {
				            lTimebasestatus_st = StbM_Reset_Bit(lTimebasestatus_st,STBM_TIMELEAP_PAST_BIT,
				                   (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_TIMELEAP_PAST_REMOVED);
				            /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
#if (STBM_TIME_CORRECTION ==  STD_ON )
				            /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
				            StbM_ResetOngoingRateCorrections(index);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
				        }
				        else
				        {
				            /* No reset of Timeleap Bit as count is not zero */
				        }
				    }
				    else
				    {
				        /* Count is not decremented since the it is not non zero value */
				    }
				}
				/* StbM_TimeLeap_Future_Count_u16 should be decremented since tehre is no leap in the FUTURE */
                if(StbM_TimeLeap_Future_Count_u16[index]!= STBM_ZERO)
                {
                    StbM_TimeLeap_Future_Count_u16[index]--;
                    if(StbM_TimeLeap_Future_Count_u16[index] == STBM_ZERO)
                    {
                        lTimebasestatus_st = StbM_Reset_Bit(lTimebasestatus_st,STBM_TIMELEAP_FUTURE_BIT,
                                (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_TIMELEAP_FUTURE_REMOVED);
                        /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
#if (STBM_TIME_CORRECTION ==  STD_ON )
                        /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
                        StbM_ResetOngoingRateCorrections(index);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
                    }
                    else
                    {
                        /* No reset of Timeleap Bit as count is not zero */
                    }
                }
                else
                {
                    /* Count is not decremented since the it is not non zero value */
                }
			}
		}
	}


	/* [SWS_StbM_00183]For each Time Domain where a Time Slave belongs to, the StbM shall observe a timeout.
The timeout StbMSyncLossTimeout (ECUC_StbM_00028 :) shall be measured from last invocation of
StbM_BusSetGlobalTime() If the timeout occurs, the StbM shall set the TIMEOUT bit within timeBaseStatus of the
Time Base. An invocation of StbM_BusSetGlobalTime() shall clear the bit.(Implemented in Getcurrent time API) */

	/* Check if Timeout Bit is already set. If Set, then Reset the bit */
    if(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus,STBM_TIMEOUT_BIT))
    {
        /* [SWS_StbM_00183] An invocation of StbM_BusSetGlobalTime() shall clear the bit. */
        lTimebasestatus_st = StbM_Reset_Bit(lTimebasestatus_st,STBM_TIMEOUT_BIT,
	                                       (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_TIMEOUT_REMOVED);
    }
    else
    {
        /* Don't reset the bit, as its already not set */
    }

	/* [SWS_StbM_00184]Every invocation of StbM_BusSetGlobalTime() shall set the SYNC_TO_GATEWAY bit within
timeBaseStatus of the Time Base to the value of the SYNC_TO_GATEWAY bit within timeBaseStatus of the timeStampPtr
argument passed to StbM_BusSetGlobalTime(). */

	lTimebasestatus_st = StbM_Copy_Bit(lTimebasestatus_st,STBM_SYNC_TO_GATEWAY_BIT,
	StbM_Check_Bit(timeStampPtr->timeBaseStatus,STBM_SYNC_TO_GATEWAY_BIT));
	if(StbM_Check_Bit(timeStampPtr->timeBaseStatus,STBM_SYNC_TO_GATEWAY_BIT))
	{
	    if (StbM_CheckEvent_Bit(lNotifyMask_u32,STBM_EV_SYNC_TO_SUBDOMAIN))
	    {
	        StbM_SetEvent_Bit((StbM_NotificationEvents_au32+index),STBM_EV_SYNC_TO_SUBDOMAIN);
	    }
	    else
	    {
	        /* do nothing */
	    }
	}
	else
	{
	    if (StbM_CheckEvent_Bit(lNotifyMask_u32,STBM_EV_SYNC_TO_GLOBAL_MASTER))
	    {
	        StbM_SetEvent_Bit((StbM_NotificationEvents_au32+index),STBM_EV_SYNC_TO_GLOBAL_MASTER);
	    }
	    else
	    {
	        /* do nothing */
	    }
	}
	
	/* Check if the SYNC_TO_GATEWAY bit in received TimeBaseStatus is changed */
    if(StbM_Compare_Bit(timeStampPtr->timeBaseStatus,StbM_GlobalTimeReception_ast[index].timeBaseStatus,STBM_SYNC_TO_GATEWAY_BIT))
    {
#if (STBM_TIME_CORRECTION ==  STD_ON )
        /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
        StbM_ResetOngoingRateCorrections(index);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
    }
    else
    {
        /* do nothing, as no need to discard measurement */
    }

	return (lTimebasestatus_st);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
* Service name       : StbM_SetGlobalTime
* Syntax             : Std_ReturnType StbM_SetGlobalTime( StbM_SynchronizedTimeBaseType timeBaseId,
*                      const StbM_TimeStampType* timeStamp,
*                      const StbM_UserDataType* userData )
* Service ID[hex]    : 0x0b
* Sync/Async         : Synchronous
* Reentrancy         : Non Reentrant
* Parameters (in)    : timeBaseId   - time base reference
*                      timeStamp    - New time stamp
*                      userData     - New user data (if not NULL)
* Parameters (inout) : None
* Parameters (out)   : None
* Return value       : Std_ReturnType E_OK: successful E_NOT_OK: failed
* Description        : Allows the Customers to set the new global time that has to be valid for the system,
*                      which will be sent to the busses and modify HW registers behind the providers, if supported.
*                      This function will be used if a Time Master is present in this ECU.
**********************************************************************************************************************/
/* [SWS_StbM_00213] */
/* HIS METRIC LEVEL VIOLATION IN StbM_SetGlobalTime: Since multiple functionalities are handled in the API, LEVEL is more */
Std_ReturnType StbM_SetGlobalTime(StbM_SynchronizedTimeBaseType timeBaseId,
const StbM_TimeStampType * timeStamp,
const StbM_UserDataType * userData)
{
	/* Local Variable declaration */
	Std_ReturnType lRet_Val_u8; /* Return value of StbM_SetGlobalTime() */
    Std_ReturnType lRet_GCVLTI_u8; /* Return value from StbM_GetCurrentVirtualLocalTime_Internal() */
#if (STBM_NVM_ENABLE ==  STD_ON )
	uint8 lNvm_Index_u8;
	Std_ReturnType lRet_NSRBS_u8; /* Return value from NvM_SetRamBlockStatus() */
#endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/
	uint8 lIndex_u8;
#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
    Std_ReturnType lRet_GCT_u8; /* Return value from StbM_GetCurrentTime() */
	uint8 lOffsetRef_Index_u8;
	uint16 lTimeBaseId_u16;
#endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)*/
    StbM_TimeBaseNotificationType lNotifyMask_u32 = STBM_ZERO ;
#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
	uint8 lgreaterTimeStamp_u8 = STBM_ZERO;
	StbM_TimeStampType lTimeStampPtr_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_UserDataType lUserDataPtr_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_TimeStampType lDiff_Time_st   = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
#endif /* #if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO) */
    StbM_VirtualLocalTimeType lTimeStampVLocalNow_st  = {STBM_ZERO,STBM_ZERO};
	Std_ReturnType lRet_SNCB_u8; /* Return value from StatusNotificationCallback */

	/* Local Variable Initialization */
#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
    lRet_GCT_u8 = E_NOT_OK;
	lOffsetRef_Index_u8 = STBM_INVALID_INDEX ;
	lTimeBaseId_u16     = STBM_INVALID_TIMEBASEID ;
#endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)*/
	lRet_SNCB_u8 = E_NOT_OK;
    lRet_GCVLTI_u8 = E_NOT_OK;
	lRet_Val_u8  = E_NOT_OK;
#if (STBM_NVM_ENABLE ==  STD_ON )
	lRet_NSRBS_u8 = E_NOT_OK;
#endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/

	/* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;
#if (STBM_NVM_ENABLE ==  STD_ON )
    lNvm_Index_u8 = STBM_INVALID_INDEX;
#endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/

	/* Check whether module is initialized */
	if(StbM_InitState_b!=FALSE)
	{
	    /* Check if the TimeBaseID is in valid range */
	    if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
	    {
	        /* For a timebase within the range, Index is loaded */
	        lIndex_u8 = StbM_Index_au8[timeBaseId];
#if (STBM_NVM_ENABLE ==  STD_ON )
	        lNvm_Index_u8 = StbM_Nvm_Index_au8[timeBaseId];
#endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/

	        /* Check if the timeStamp is NULL_PTR or not*/
	        if(timeStamp != NULL_PTR)
	        {
                /* Check if the nanoseconds value passed is less than 999999999 */
                if(timeStamp->nanoseconds <= STBM_MAX_NS)
                {
                    /* Check if the userDataLength passed is less than or equal to 3, when UserData is not NULL */
                    if(StbM_Rb_IsUserDataValid(userData) != STBM_USER_DATA_INVALID)
                    {
                        /*This function will be used if a Time Master is present in this ECU*/
                        if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].IsSystemWideGlobalTimeMaster_b == TRUE)
                        {

                            lNotifyMask_u32 = StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMStatusNotificationMask_u32 ;
                            /*[SWS_StbM_00304]On invocation of StbM_SetGlobalTime() or StbM_UpdateGlobalTime() for Time Domains 16 to 31 the
        StbM shall check the GLOBAL_TIME_BASE bit within timeBaseStatus of the underlying Synchronized Time Base and
        shall return E_NOT_OK if is not set.
        If the GLOBAL_TIME_BASE bit is set, the StbM:
        1.shall calculate the Offset Time by obtaining the actual Time Base value of the underlying Synchronized Time
        Base and subtract that from the Absolute Time value which is passed by StbM_SetGlobalTime() or StbM_UpdateGlobalTime()
        2.shall call StbM_SetOffset() with the calculated Offset Time value and the User Data passed by
        StbM_SetGlobalTime() or StbM_UpdateGlobalTime().*/

        #if	(STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
                            /*  Check if the TimeBase is Offset TimeBase */
                            if(StbM_Rb_IsOffsetTimeBase(timeBaseId))
                            {
                                /* Get the referenced time base ID for the offset time base  */
                                lTimeBaseId_u16 = StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].OffsetTimeBaseRef_u8;
                                lOffsetRef_Index_u8 = StbM_Index_au8[lTimeBaseId_u16];

                                /* Check if the referenced sync timebase GLOBAL TIMEBASE bit is set or not */
                                if(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[lOffsetRef_Index_u8].timeBaseStatus,
                                                                                              STBM_GLOBAL_TIME_BASE_BIT))
                                {
                                    /* Get the current time for referenced Synchronised time base */
                                    lRet_GCT_u8 = StbM_GetCurrentTime(lTimeBaseId_u16,&lTimeStampPtr_st,&lUserDataPtr_st);

                                    if(lRet_GCT_u8 == E_OK)
                                    {
                                        /* Calculate the difference between the actual Time Base value and the Absolute Time value */
                                        StbM_Time_DifferenceCalculation
                                                        (timeStamp,&lTimeStampPtr_st,&lDiff_Time_st,&lgreaterTimeStamp_u8);

                                        /*call StbM_SetOffset() with the calculated Offset Time value and the User Data passed by StbM_SetGlobalTime()*/
                                        lRet_GCT_u8 = StbM_SetOffset(timeBaseId, &lDiff_Time_st,userData);
                                    }
                                    else
                                    {

                                    }
                                }
                                else
                                {
                                    lRet_GCT_u8 = E_NOT_OK;
                                }

                            }
                            else
                            {
                                lRet_GCT_u8 = E_OK;
                            }

        #endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO) */

                            /*  Enter the resource lock */
                            SchM_Enter_StbM_Sync_OsResource();

                            /* [SWS_StbM_00181]On a valid invocation of StbM_SetGlobalTime()the StbM shall set the GLOBAL_TIME_BASE
        bit within timeBaseStatus of the corresponding Time Domain and shall clear all other bits */

                            /* [SWS_StbM_00399]For Pure Local Time Bases all bits of the Time Base status timeBaseStatus shall be set to 0,
                   except for bit GLOBAL_TIME_BASE.GLOBAL_TIME_BASE shall be set to 1, by a valid invocation of
                   StbM_SetGlobalTime() or StbM_UpdateGlobalTime() and only set to 0 by StbM_Init()  */
                            StbM_Rb_UpdateTimeBaseStatusBits(lIndex_u8, lNotifyMask_u32);

                            /* [SWS_StbM_00342] On a valid invocation of StbM_SetGlobalTime() or StbM_UpdateGlobalTime() the StbM shall
                   update the Main Time Tuple of the corresponding Synchronized Time Base.
                   Within the functions StbM_SetGlobalTime() and StbM_UpdateGlobalTime() the StbM shall retrieve the value of
                   the Virtual Local Time (as part of the Local Time tuple) as soon as possible in order to improve precision
                   of the Time Base.*/
                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds = timeStamp->nanoseconds; /* nanoseconds value */
                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds     = timeStamp->seconds;     /* seconds value */
                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi   = timeStamp->secondsHi;   /* seconds Hi value */

                            /*[SWS_StbM_00433]
                  The Main Time Tuple shall only be updated
                  ->after setting a new Global Time or a new Rate Correction value by the application
                  ->after obtaining a Received Time Tuple (i.e., a new Time Tuple) from a Timesync Module
                  ->after the Offset Correction By Rate Adaption interval (see [SWS_StbM_00353])
                  However, the Main Time Tuple may be updated if there has been no update for more than 3s.

                  [SWS_StbM_00436]
                  Although the retrieved value of the Virtual Local Time and the time which is returned by
                  StbM_GetCurrentTime(), StbM_GetCurrentTimeExtended(), and StbM_BusGetCurrentTime() form a
                  new Time Tuple [TL;TV], this tuple shall only replace the Main Time Tuple if the requirements
                  as specified in [SWS_StbM_00433] are met*/

                            /* Call the StbM_GetCurrentVirtualLocalTime_Internal to get the current Virtual local Time value along
                    with counter value update*/
                            lRet_GCVLTI_u8 = StbM_GetCurrentVirtualLocalTime_Internal
                                                                      (lIndex_u8,&lTimeStampVLocalNow_st,STBM_COUNTER_UPDATE);

                            /* Update the Virtual Local time array with the new Virtual Local time value */
                            StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsHi    = lTimeStampVLocalNow_st.nanosecondsHi;
                            StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsLo    = lTimeStampVLocalNow_st.nanosecondsLo;

                            /* Update the Virtual Local time in the Time Tuple array to make a tuple with Received Global Time */
                            StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8].nanosecondsHi = lTimeStampVLocalNow_st.nanosecondsHi;
                            StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8].nanosecondsLo = lTimeStampVLocalNow_st.nanosecondsLo;

        #if (STBM_TIME_CORRECTION ==  STD_ON )
                            if(StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr != NULL_PTR)
                            {
                                StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->
                                                   StbMVirtualLocalTimeLatest_st.nanosecondsHi = lTimeStampVLocalNow_st.nanosecondsHi;
                                StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->
                                               StbMVirtualLocalTimeLatest_st.nanosecondsLo = lTimeStampVLocalNow_st.nanosecondsLo;
                            }
                            else
                            {
                                /* If Rate Correction is not configured for the TB then array is not updated*/
                            }
        #endif /* (STBM_TIME_CORRECTION ==  STD_ON ) */

                            /* Global Array is updated to calculate Rate Corrected Time */
                            StbM_GlobalTimeReception_ast[lIndex_u8].nanoseconds = StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds; /* nanoseconds value */
                            StbM_GlobalTimeReception_ast[lIndex_u8].seconds     = StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds;     /* seconds value */
                            StbM_GlobalTimeReception_ast[lIndex_u8].secondsHi   = StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi;   /* seconds Hi value */
                            StbM_GlobalTimeReception_ast[lIndex_u8].timeBaseStatus   = StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus;   /* timeBase Status */

                            if(lRet_GCVLTI_u8 == E_OK)
                            {
                                if((StbM_Rb_IsSyncTimeBase(timeBaseId)) || (StbM_Rb_IsOffsetTimeBase(timeBaseId)))
                                {
                                    /*[SWS_StbM_00350]On a valid invocation of StbM_SetGlobalTime(), StbM_BusSetGlobalTime()
            or StbM_TriggerTimeTransmission() the StbM shall increment the timeBaseUpdateCounter
            of the corresponding Time Base by 1 (one). At 255 it shall wrap around to 0.*/
                                    (*(StbM_TimeBaseUpdateCounter_au8 + lIndex_u8))++ ;
                                }
                                else
                                {
                                    /* do nothing */
                                }

                                /* UserData can be either Null_Ptr or with Valid elements. Hence do update UserData, only if its not Null_Ptr */
                                if(StbM_Rb_IsUserDataValid(userData) != STBM_USER_DATA_NULL_PTR)
                                {
                                    /* [SWS_StbM_00398]For Pure Local Time Bases StbM_GetCurrentTime() and StbM_GetCurrentTimeExtended() shall
                            return the User Data as set by StbM_SetGlobalTime(), StbM_UpdateGlobalTime() or StbM_SetUserData() by the
                            local Pure Local Time Master  */

                                    /* Set the user data with the new data recieved based on the length provided by user as structure attribute */
                                    StbM_UserDataUpdate(lIndex_u8,userData);
                                }
                                else
                                {
                                    /* No need to update UserData, as its NULL */
                                }

                                /* Exit the resource lock , Since after this NvM API's are called , the locking time may be more. Hence the lock is exit here */
                                SchM_Exit_StbM_Sync_OsResource();

        #if (STBM_NVM_ENABLE ==  STD_ON )
                                /* Set the NvM status as changed since everytime the function is called the timebase value will be changed */
                                /* The lNvm_Index_u8 value is only valid when
                                StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StoreTimebaseNonVolatile_e==STBM_NVM_STORAGE_AT_SHUTDOWN*/
                                if (lNvm_Index_u8 != STBM_INVALID_INDEX)
                                {

                                    StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].nanoseconds    = timeStamp->nanoseconds     ;/* nanoseconds value */
                                    StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].seconds        = timeStamp->seconds         ;/* seconds value */
                                    StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].secondsHi      = timeStamp->secondsHi       ;/* seconds Hi value */

                                    lRet_NSRBS_u8 = NvM_SetRamBlockStatus (STBM_NVM_BLOCK_REFERENCE, TRUE);
                                }
                                else
                                {
                                    lRet_NSRBS_u8 = E_OK;
                                }
        #endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/

                                /* If any status event occurs and the corresponding bit in the NotificationMask mask is set,
            the StbM shall the callback function StbM_StatusNotificationCallback.If multiple status events occur simultaneously
            for the same Time Base, the StbM shall call the callback function StbM_StatusNotificationCallback only once.
            The StbM shall set the eventNotifications parameter of StbM_StatusNotificationCallback to the value of the NotificationEvents variable.*/

                                lRet_SNCB_u8 = StbM_Rb_StatusNotificationCallback(lIndex_u8);
                            }
                            else
                            {
                                /* Since the lock will not be exit if E_NOT_OK is returned, Exit resource lock is added here */
                                SchM_Exit_StbM_Sync_OsResource();
                            }
                        }
                    }
                    else
                    {
                        /* [SWS_StbM_00449] If the switch StbMDevErrorDetect is set to TRUE, StbM_SetGlobalTime()
                         * shall report to DET the development error STBM_E_PARAM_USERDATA, if called with an invalid
                         * value of parameter userData, i.e., userDataLength > 3. */
                        /* If userDataLength is invalid(greater than 3) then report error and exit the function */
                        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETGLOBALTIME,STBM_E_PARAM_USERDATA)
                    }
                }
                else
                {
                    /* [SWS_StbM_00448] If the switch StbMDevErrorDetect is set to TRUE, StbM_SetGlobalTime()
                     * shall report to DET the development error STBM_E_PARAM_TIMESTAMP, if called with a parameter
                     * timeStamp that contains invalid elements (e.g., nanoseconds part > 999999999 ns). */
                    /* If ns part in TimeStamp is greater than 999999999 then report error and exit the function */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETGLOBALTIME,STBM_E_PARAM_TIMESTAMP)
                }
	        }
	        else
	        {
	            /* [SWS_StbM_00215] If the switch STBM_DEV_ERROR_DETECT is set to True, StbM_SetGlobalTime()
	             * shall report to DET the development error STBM_E_PARAM_POINTER, if called with an invalid pointer of
	             * parameter timeStampPtr */
	            /* if Config is NULL pointer then report error and exit the function */
	            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETGLOBALTIME,STBM_E_PARAM_POINTER)
	        }
	    }
	    else
	    {
	        /* [SWS_StbM_00214] If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True,
	         * StbM_SetGlobalTime() shall report to DET the development error STBM_E_PARAM, if called with an invalid
	         * parameter timeBaseID */
	        /* To inform DET in case of invalid timebase */
	        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETGLOBALTIME,STBM_E_PARAM)
	    }
	}
	else
	{
	    /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
	    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETGLOBALTIME,STBM_E_NOT_INITIALIZED)
	}

	if((lRet_GCVLTI_u8 != E_NOT_OK)
#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
	&&(lRet_GCT_u8 != E_NOT_OK)
#endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)*/
#if (STBM_NVM_ENABLE ==  STD_ON )
	&&(lRet_NSRBS_u8 != E_NOT_OK)
#endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/
	&&(lRet_SNCB_u8 != E_NOT_OK))
	{
		lRet_Val_u8 = E_OK;
	}
	else
	{
		lRet_Val_u8 = E_NOT_OK;
	}

	return (lRet_Val_u8);
}
#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"



#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
* Service name       : StbM_UpdateGlobalTime
* Syntax             : Std_ReturnType StbM_UpdateGlobalTime( StbM_SynchronizedTimeBaseType timeBaseId,
*                      const StbM_TimeStampType* timeStamp,
*                      const StbM_UserDataType* userData )
* Service ID[hex]    : 0x10
* Sync/Async         : Synchronous
* Reentrancy         : Non Reentrant
* Parameters (in)    : timeBaseId   - time base reference
*                      timeStamp - New time stamp
*                      userData  - New user data (if not NULL)
* Parameters (inout) : None
* Parameters (out)   : None
* Return value       : Std_ReturnType E_OK: successful E_NOT_OK: failed
* Description        : Allows the Customers to set the Global Time that will be sent to the buses.
This function will be used if a Time Master is present in this ECU.
Using UpdateGlobalTime will not lead to an immediate transmission of the Global Time.
**********************************************************************************************************************/
/* [SWS_StbM_00385] */
/* HIS METRIC LEVEL VIOLATION IN StbM_UpdateGlobalTime: Since multiple functionalities are handled in the API, LEVEL is more */
Std_ReturnType StbM_UpdateGlobalTime(StbM_SynchronizedTimeBaseType timeBaseId,
const StbM_TimeStampType * timeStamp,
const StbM_UserDataType * userData)
{
	/* Local Variable declaration */
	Std_ReturnType lRet_Val_u8; /* Return value of StbM_UpdateGlobalTime() */
    Std_ReturnType lRet_GCVLTI_u8; /* Return value from StbM_GetCurrentVirtualLocalTime_Internal() */
#if (STBM_NVM_ENABLE ==  STD_ON )
	uint8 lNvm_Index_u8;
	Std_ReturnType lRet_NSRBS_u8; /* Return value from NvM_SetRamBlockStatus() */
#endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/
	uint8 lIndex_u8;

	Std_ReturnType lRet_SNCB_u8; /* Return value from StatusNotificationCallback */
    StbM_TimeBaseNotificationType lNotifyMask_u32 = STBM_ZERO ;
#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
    Std_ReturnType lRet_GCT_u8; /* Return value from StbM_GetCurrentTime() */
	uint16 lTimeBaseId_u16;
	uint8 lOffsetRef_Index_u8;
	uint8 lgreaterTimeStamp_u8 = STBM_ZERO;
	StbM_TimeStampType lTimeStampPtr_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_UserDataType lUserDataPtr_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_TimeStampType lDiff_Time_st   = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
#endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO) */
    StbM_VirtualLocalTimeType lTimeStampVLocalNow_st  = {STBM_ZERO,STBM_ZERO};

	/* Local Variable Initialization */
#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
    lRet_GCT_u8  = E_NOT_OK;
    lOffsetRef_Index_u8 = STBM_INVALID_INDEX ;
    lTimeBaseId_u16     = STBM_INVALID_TIMEBASEID ;
#endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)*/

	lRet_SNCB_u8 = E_NOT_OK;
	lRet_Val_u8  = E_NOT_OK;
	lRet_GCVLTI_u8  = E_NOT_OK;
#if  (STBM_NVM_ENABLE ==  STD_ON )
    lRet_NSRBS_u8 = E_NOT_OK;
#endif /*#if  (STBM_NVM_ENABLE ==  STD_ON )*/

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;
#if  (STBM_NVM_ENABLE ==  STD_ON )
    lNvm_Index_u8 = STBM_INVALID_INDEX;
#endif /*#if  (STBM_NVM_ENABLE ==  STD_ON )*/

	/* Check whether module is initialized */
	if(StbM_InitState_b!=FALSE)
	{
        /* Check if the TimeBaseID is in valid range */
        if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
	    {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];
#if  (STBM_NVM_ENABLE ==  STD_ON )
            lNvm_Index_u8 = StbM_Nvm_Index_au8[timeBaseId];
#endif /*#if  (STBM_NVM_ENABLE ==  STD_ON )*/

	        /* Check if the timeStamp is NULL_PTR or not*/
	        if(timeStamp != NULL_PTR)
	        {
                /* Check if the nanoseconds value passed is less than 999999999 */
                if(timeStamp->nanoseconds <= STBM_MAX_NS)
                {
                    /* Check if the userDataLength passed is less than or equal to 3 when UserData is not NULL */
                    if(StbM_Rb_IsUserDataValid(userData) != STBM_USER_DATA_INVALID)
                    {
                        /*This function will be used if a Time Master is present in this ECU*/
                        if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].IsSystemWideGlobalTimeMaster_b == TRUE)
                        {
                            lNotifyMask_u32 = StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMStatusNotificationMask_u32 ;
                            /*[SWS_StbM_00304]On invocation of StbM_SetGlobalTime() or StbM_UpdateGlobalTime() for Time Domains 16 to 31 the
        StbM shall check the GLOBAL_TIME_BASE bit within timeBaseStatus of the underlying Synchronized Time Base and
        shall return E_NOT_OK if is not set.
        If the GLOBAL_TIME_BASE bit is set, the StbM:
        1.shall calculate the Offset Time by obtaining the actual Time Base value of the underlying Synchronized Time
        Base and subtract that from the Absolute Time value which is passed by StbM_SetGlobalTime() or StbM_UpdateGlobalTime()
        2.shall call StbM_SetOffset() with the calculated Offset Time value and the User Data passed by
        StbM_SetGlobalTime() or StbM_UpdateGlobalTime().*/

        #if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
                            /*  Check if the TimeBase is Offset TimeBase */
                            if(StbM_Rb_IsOffsetTimeBase(timeBaseId))
                            {
                                /* Get the referenced time base ID for the offset time base  */
                                lTimeBaseId_u16 = StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].OffsetTimeBaseRef_u8;
                                lOffsetRef_Index_u8 = StbM_Index_au8[lTimeBaseId_u16];
                                /* Check if the referenced sync timebase GLOBAL TIMEBASE bit is set or not */
                                if(StbM_Check_Bit
                                      (StbM_GlobalTimeTupleArray_ast[lOffsetRef_Index_u8].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT))
                                {
                                    /* Get the current time for referenced Synchronised time base */
                                    lRet_GCT_u8 = StbM_GetCurrentTime(lTimeBaseId_u16,&lTimeStampPtr_st,&lUserDataPtr_st);

                                    if(lRet_GCT_u8 == E_OK)
                                    {
                                        /* Calculate the difference between the actual Time Base value and the Absolute Time value */
                                        StbM_Time_DifferenceCalculation
                                                        (timeStamp,&lTimeStampPtr_st,&lDiff_Time_st,&lgreaterTimeStamp_u8);

                                        /*call StbM_SetOffset() with the calculated Offset Time value and the User Data passed by StbM_SetGlobalTime()*/
                                        lRet_GCT_u8 = StbM_SetOffset(timeBaseId, &lDiff_Time_st,userData);
                                    }
                                    else
                                    {

                                    }
                                }
                                else
                                {
                                    lRet_GCT_u8 = E_NOT_OK;
                                }
                            }
                            else
                            {
                                lRet_GCT_u8 = E_OK;
                            }

        #endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)*/

                            /*  Enter the resource lock */
                            SchM_Enter_StbM_Sync_OsResource();
                            /* [SWS_StbM_00181]On a valid invocation of StbM_SetGlobalTime()or StbM_UpdateGlobalTime() the StbM
                 shall set the GLOBAL_TIME_BASE bit within timeBaseStatus of the corresponding
                  Time Domain and shall clear all other bits */

                            /* [SWS_StbM_00399]For Pure Local Time Bases all bits of the Time Base status timeBaseStatus shall be set to 0,
                except for bit GLOBAL_TIME_BASE.GLOBAL_TIME_BASE shall be set to 1, by a valid invocation of
                StbM_SetGlobalTime() or StbM_UpdateGlobalTime() and only set to 0 by StbM_Init()  */

                            StbM_Rb_UpdateTimeBaseStatusBits(lIndex_u8, lNotifyMask_u32);

                            /* [SWS_StbM_00342] On a valid invocation of StbM_SetGlobalTime() or StbM_UpdateGlobalTime() the StbM shall
                   update the Main Time Tuple of the corresponding Synchronized Time Base.
                   Within the functions StbM_SetGlobalTime() and StbM_UpdateGlobalTime() the StbM shall retrieve the value of
                   the Virtual Local Time (as part of the Local Time tuple) as soon as possible in order to improve precision
                   of the Time Base.*/
                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds    = timeStamp->nanoseconds;     /* nanoseconds value */
                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds        = timeStamp->seconds;         /* seconds value */
                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi      = timeStamp->secondsHi;       /* seconds Hi value */

                            /*[SWS_StbM_00433]
                  The Main Time Tuple shall only be updated
                  ->after setting a new Global Time or a new Rate Correction value by the application
                  ->after obtaining a Received Time Tuple (i.e., a new Time Tuple) from a Timesync Module
                  ->after the Offset Correction By Rate Adaption interval (see [SWS_StbM_00353])
                  However, the Main Time Tuple may be updated if there has been no update for more than 3s.

                  [SWS_StbM_00436]
                  Although the retrieved value of the Virtual Local Time and the time which is returned by
                  StbM_GetCurrentTime(), StbM_GetCurrentTimeExtended(), and StbM_BusGetCurrentTime() form a
                  new Time Tuple [TL;TV], this tuple shall only replace the Main Time Tuple if the requirements
                  as specified in [SWS_StbM_00433] are met*/

                            /* Call the StbM_GetCurrentVirtualLocalTime_Internal to get the current Virtual local Time value along
                    with counter value update*/
                            lRet_GCVLTI_u8 =
                            StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocalNow_st,STBM_COUNTER_UPDATE);

                            /* Update the Virtual  Local time array with the new Virtual Local time value */
                            StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsHi    = lTimeStampVLocalNow_st.nanosecondsHi;
                            StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsLo    = lTimeStampVLocalNow_st.nanosecondsLo;

                            /* Update the Virtual Local time in the Time Tuple array to make a tuple with Received Global Time */
                            StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8].nanosecondsHi = lTimeStampVLocalNow_st.nanosecondsHi;
                            StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8].nanosecondsLo = lTimeStampVLocalNow_st.nanosecondsLo;

        #if (STBM_TIME_CORRECTION ==  STD_ON )
                            if(StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr != NULL_PTR)
                            {
                                StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->
                                                   StbMVirtualLocalTimeLatest_st.nanosecondsHi = lTimeStampVLocalNow_st.nanosecondsHi;
                                StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->
                                               StbMVirtualLocalTimeLatest_st.nanosecondsLo = lTimeStampVLocalNow_st.nanosecondsLo;
                            }
                            else
                            {
                                /* If Rate Correction is not configured for the TB then array is not updated*/
                            }
        #endif /* (STBM_TIME_CORRECTION ==  STD_ON ) */

                            /* Global Array is updated to calculate Rate Corrected Time */
                            StbM_GlobalTimeReception_ast[lIndex_u8].nanoseconds = StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds; /* nanoseconds value */
                            StbM_GlobalTimeReception_ast[lIndex_u8].seconds     = StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds;     /* seconds value */
                            StbM_GlobalTimeReception_ast[lIndex_u8].secondsHi   = StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi;   /* seconds Hi value */
                            StbM_GlobalTimeReception_ast[lIndex_u8].timeBaseStatus   = StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus;   /* timeBase Status */

                            if(lRet_GCVLTI_u8 == E_OK)
                            {
                                /* UserData can be either Null_Ptr or with Valid elements. Hence do update UserData, only if its not Null_Ptr */
                                if(StbM_Rb_IsUserDataValid(userData) != STBM_USER_DATA_NULL_PTR)
                                {
                                    /* Set the user data with the new data recieved based on the length provided by user as structure attribute */
                                    StbM_UserDataUpdate(lIndex_u8,userData);
                                }
                                else
                                {
                                    /* No need to update UserData, as its NULL */
                                }

                                /* Exit the resource lock , Since after this NvM API's are called , the locking time may be more. Hence the lock is exit here */
                                SchM_Exit_StbM_Sync_OsResource();

        #if (STBM_NVM_ENABLE ==  STD_ON )
                                /* Set the NvM status as changed since everytime the function is called the timebase value will be changed */
                                /* The lNvm_Index_u8 value is only valid when
                                StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StoreTimebaseNonVolatile_e==STBM_NVM_STORAGE_AT_SHUTDOWN*/
                                if (lNvm_Index_u8 != STBM_INVALID_INDEX)
                                {
                                    StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].nanoseconds    = timeStamp->nanoseconds     ;/* nanoseconds value */
                                    StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].seconds        = timeStamp->seconds         ;/* seconds value */
                                    StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].secondsHi      = timeStamp->secondsHi       ;/* seconds Hi value */

                                    lRet_NSRBS_u8 = NvM_SetRamBlockStatus (STBM_NVM_BLOCK_REFERENCE, TRUE);
                                }
                                else
                                {
                                    lRet_NSRBS_u8 = E_OK;
                                }
        #endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/

                                /* If any status event (refer to [SWS_StbM_00284]) occurs and the corresponding bit in the NotificationMask mask is set,
            the StbM shall the callback function StbM_StatusNotificationCallback.If multiple status events occur simultaneously
            for the same Time Base, the StbM shall call the callback function StbM_StatusNotificationCallback only once.
            The StbM shall set the eventNotifications parameter of StbM_StatusNotificationCallback to the value of the NotificationEvents variable.*/

                                lRet_SNCB_u8 = StbM_Rb_StatusNotificationCallback(lIndex_u8);
                            }
                            else
                            {
                                /* Since the lock will not be exit if E_NOT_OK is returned, Exit resource lock is added here */
                                SchM_Exit_StbM_Sync_OsResource();
                            }
                        }
                    }
                    else
                    {
                        /* [SWS_StbM_00452] If the switch StbMDevErrorDetect is set to TRUE, StbM_UpdateGlobalTime()
                         * shall report to DET the development error STBM_E_PARAM_USERDATA, if called with an invalid
                         * value of parameter userData, i.e., userDataLength > 3. */
                        /* If userDataLength is invalid(greater than 3) then report error and exit the function */
                        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_UPDATEGLOBALTIME,STBM_E_PARAM_USERDATA)
                    }
                }
                else
                {
                    /* [SWS_StbM_00451] If the switch StbMDevErrorDetect is set to TRUE, StbM_UpdateGlobalTime()
                     * shall report to DET the development error STBM_E_PARAM_TIMESTAMP, if called with a parameter
                     * timeStamp that contains invalid elements (e.g., nanoseconds part > 999999999 ns). */
                    /* If ns part in TimeStamp is greater than 999999999 then report error and exit the function */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_UPDATEGLOBALTIME,STBM_E_PARAM_TIMESTAMP)
                }
	        }
	        else
	        {
	            /* [SWS_StbM_00341] If the switch StbMDevErrorDetect is set to TRUE, StbM_UpdateGlobalTime() shall
	             * report to DET the development error STBM_E_PARAM_POINTER, if called with a NULL pointer of parameter
	             * timeStamp. */
	            /* If Config is NULL pointer then report error and exit the function */
	            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_UPDATEGLOBALTIME,STBM_E_PARAM_POINTER)
	        }
	    }
	    else
	    {
	        /* [SWS_StbM_00340] If the switch StbMDevErrorDetect(ECUC_StbM_00012 : ) is set to TRUE,
	         * StbM_UpdateGlobalTime() shall report to DET the development error STBM_E_PARAM, if called with a
	         * parameter timeBaseID, which is not configured or within the reserved value range */
	        /* To inform DET in case of invalid timebase */
	        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_UPDATEGLOBALTIME,STBM_E_PARAM)
	    }
	}
	else
	{
	    /* [SWS_StbM_00198]DET error, if StbM is not intialised */
	    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_UPDATEGLOBALTIME,STBM_E_NOT_INITIALIZED)
	}

	if((lRet_GCVLTI_u8 != E_NOT_OK)
#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)
	&&(lRet_GCT_u8 != E_NOT_OK)
#endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)*/
#if (STBM_NVM_ENABLE ==  STD_ON )
	&&(lRet_NSRBS_u8 != E_NOT_OK)
#endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/
	&&(lRet_SNCB_u8 != E_NOT_OK))
	{
		lRet_Val_u8 = E_OK;
	}
	else
	{
		lRet_Val_u8 = E_NOT_OK;
	}

	return (lRet_Val_u8);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/**********************************************************************************************************************
Service name     :StbM_BusSetGlobalTime
Syntax           :Std_ReturnType StbM_BusSetGlobalTime( StbM_SynchronizedTimeBaseType timeBaseId,
const StbM_TimeStampType* timeStampPtr, const StbM_UserDataType* userDataPtr,
boolean syncToTimeBase )
Service ID[hex]  :0x0f
Sync/Async       :Synchronous
Reentrancy       :Non Reentrant
Parameters (in)  :timeBaseId      time base reference
timeStampPtr    New time stamp
userDataPtr     New user data (if not NULL)
syncToTimeBase  SYNC to GTM (0) clear the SYNC_TO_GATEWAY bit within the status
SYNC to sub-domain (1) set the SYNC_TO_GATEWAY bit within the status
Parameters(inout):None
Parameters (out) :None
Return value     :Std_ReturnType E_OK: successful
E_NOT_OK: failed
Description      :Allows the Timebase Provider Modules to forward a new Global Time to the StbM,
which has been received from different busses.
**********************************************************************************************************************/
/* [SWS_StbM_00233] */
/*MR12 RULE 2.7,8.3 VIOLATION: The parameter is used within the switch.Hence the warning */
/* HIS METRIC v(G) VIOLATION IN StbM_BusSetGlobalTime: Since multiple functionalities are handled in the API, v(G) is more */
/* HIS METRIC LEVEL VIOLATION IN StbM_BusSetGlobalTime: Since multiple functionalities are handled in the API, LEVEL is more */
Std_ReturnType StbM_BusSetGlobalTime(StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_TimeStampType * globalTimePtr,
        const StbM_UserDataType * userDataPtr,
        const StbM_MeasurementType * measureDataPtr,
        const StbM_VirtualLocalTimeType * localTimePtr)
{
	/* Local Variable declaration */
    StbM_VirtualLocalTimeType lTimeStampVLocalNow_st  = {STBM_ZERO,STBM_ZERO};
	StbM_TimeBaseStatusType lTimebaseStatus_st = STBM_ZERO;
	Std_ReturnType lRet_Val_u8; /* Return value of StbM_BusSetGlobalTime() */
	Std_ReturnType lRet_GCVLTI_u8; /* Return value from StbM_GetCurrentVirtualLocalTime_Internal() */
    Std_ReturnType lRet_GCTI_u8; /* Return value from StbM_GetCurrentTime_Internal() */
	StbM_UserDataType lUserData_st        = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalTimeStamp_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeBaseNotificationType lNotifyMask_u32 = STBM_ZERO ;
#if (STBM_TIME_CORRECTION ==  STD_ON )
    StbM_TimeStampType lTimeStampCurrentGlobal_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
#if  (STBM_NVM_ENABLE ==  STD_ON )
	uint8 lNvm_Index_u8;
	Std_ReturnType lRet_NSRBS_u8; /* Return value from NvM_SetRamBlockStatus() */
#endif /*#if  (STBM_NVM_ENABLE ==  STD_ON )*/

#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
	StbM_SyncRecordTableBlockType * lsyncTimeBaseBlockRecordArray;
	StbM_OffsetRecordTableBlockType * loffsetTimeBaseBlockRecordArray;
	uint16 lRecordTableBlockCount_u16;
	uint16 lrecordTableBlockCountCurrent_u16;
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/

	uint8 lIndex_u8;
	Std_ReturnType lRet_SNCB_u8; /* Return value from StatusNotificationCallback */

	/* Local Variable Initialization */
	lRet_Val_u8 = E_NOT_OK;
	lRet_SNCB_u8 = E_NOT_OK;
	lRet_GCVLTI_u8  = E_NOT_OK;
	lRet_GCTI_u8 = E_NOT_OK;
#if  (STBM_NVM_ENABLE ==  STD_ON )
    lRet_NSRBS_u8 = E_OK;
#endif /*#if  (STBM_NVM_ENABLE ==  STD_ON )*/

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;
#if  (STBM_NVM_ENABLE ==  STD_ON )
    lNvm_Index_u8 = STBM_INVALID_INDEX;
#endif /*#if  (STBM_NVM_ENABLE ==  STD_ON )*/

#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
    lrecordTableBlockCountCurrent_u16 = STBM_ZERO;
	lRecordTableBlockCount_u16        = STBM_ZERO;
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /*[SWS_StbM_00428]
          The StbM shall do Global Time precision measurement only for Synchronized Time Bases and Offset Time Bases,
          for which StbMIsSystemWideGlobalTimeMaster (ECUC_StbM_00036 : : ) is set to FALSE.*/
        /* Check if the TimeBase is Synchronized or Offest TimeBase */
        if((StbM_Rb_IsSyncTimeBase(timeBaseId)) || (StbM_Rb_IsOffsetTimeBase(timeBaseId)))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];
#if  (STBM_NVM_ENABLE ==  STD_ON )
            lNvm_Index_u8 = StbM_Nvm_Index_au8[timeBaseId];
#endif /*#if  (STBM_NVM_ENABLE ==  STD_ON )*/

            /* Check if the timeStamp is NULL_PTR or not*/
            if(globalTimePtr != NULL_PTR)
            {
                /* Check if the measureDataPtr is NULL_PTR or not*/
                if(measureDataPtr != NULL_PTR)
                {
                    /* Check if the localTimePtr is NULL_PTR or not*/
                    if(localTimePtr != NULL_PTR)
                    {
                        /* Check if the nanoseconds value passed is less than 999999999 */
                        if(globalTimePtr->nanoseconds <= STBM_MAX_NS)
                        {
                            /* Check if the userDataLength passed is less than or equal to 3 when UserData is not NULL */
                            if(StbM_Rb_IsUserDataValid(userDataPtr) != STBM_USER_DATA_INVALID)
                            {
                                /*[SWS_StbM_00428]
                  The StbM shall do Global Time precision measurement only for Synchronized Time Bases and Offset Time Bases,
                  for which StbMIsSystemWideGlobalTimeMaster (ECUC_StbM_00036 : : ) is set to FALSE.

                  StbM_BusSetGlobalTime will be called only when timebase is SLAVE or GATEWAY */
                                if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].IsSystemWideGlobalTimeMaster_b == FALSE )
                                {
                                    /*  Enter the resource lock */
                                    SchM_Enter_StbM_Sync_OsResource();

                                    lRet_GCVLTI_u8 = StbM_GetCurrentVirtualLocalTime_Internal (lIndex_u8,&lTimeStampVLocalNow_st,STBM_COUNTER_UPDATE);

                                    /* Update the Virtual Local time array with the new Virtual Local time value */
                                    StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsHi    = lTimeStampVLocalNow_st.nanosecondsHi;
                                    StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsLo    = lTimeStampVLocalNow_st.nanosecondsLo;

                                    /* Read the Local Time  before synchronization */
                                    lRet_GCTI_u8 = StbM_GetCurrentTime_Internal (lIndex_u8, &lLocalTimeStamp_st, localTimePtr, &lUserData_st);

                                    /* Update the Received Virtual Local time in the Time Tuple array to make a tuple with Received Global Time */
                                    StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8].nanosecondsHi = localTimePtr->nanosecondsHi;
                                    StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8].nanosecondsLo = localTimePtr->nanosecondsLo;

                                    /* [SWS_StbM_00179]
                       For Time Bases 0 to 15 each invocation of StbM_BusSetGlobalTime() shall update the corresponding
                       Main Time Tuple and set the User Data and the Time Base Status accordingly.
                       Note: To update the Main Time Tuple does not mean to automatically overwrite the Main Time Tuple with the
                       Received Time Tuple */
                                    /* Compute the Status TIME_LEAP_PAST and TIME_LEAP_FUTURE  based on the Global Time Recieved
                       and the Local Time */
                                    lTimebaseStatus_st = StbM_BusSetGlobalTime_StatusUpdate(lIndex_u8,globalTimePtr,lLocalTimeStamp_st);
                                    /* [SWS_StbM_00438]
                       The StbM shall determine for Time Bases 0 to 15 on each invocation of StbM_BusSetGlobalTime()
                       the Synclocal Time Tuple [TLSync;TVSync] by using the value of the Virtual Local Time of the
                       Received Time Tuple as reference (i.e., TVRx is used for TV when calculating TL in [SWS_StbM_00355]).
                       The Synclocal Time Tuple shall be determined using the Main Time Tuple before the
                       Main Time Tuple itself is updated.

                       [SWS_StbM_00439]
                       The StbM shall determine for Time Bases 16 to 31 on each invocation of StbM_BusSetGlobalTime()
                       the Synclocal Time Tuple [TLSync;TVSync] by using the value of the Virtual Local Time of the
                       Received Time Tuple as reference (i.e., TVRx is used for TV when calculating TL in [SWS_StbM_00355]).
                       The Synclocal Time Tuple shall be determined using the Main Time Tuple before the
                       Main Time Tuple is updated.
                                     */

                                    /* Calculate the current VLT and Global Time value [TLSync;TVSync] based on the recieved VLT TVRx */

        #if (STBM_TIME_CORRECTION ==  STD_ON )
                                    if(StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr != NULL_PTR)
                                    {
                                        /* Update the recieved VLT into the Global array for calculating current time value with rate correction*/
                                        StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->
                                        StbMVirtualLocalTimeLatest_st.nanosecondsHi    = localTimePtr->nanosecondsHi;
                                        StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->
                                        StbMVirtualLocalTimeLatest_st.nanosecondsLo    = localTimePtr->nanosecondsLo;
                                    }
                                    else
                                    {
                                        /* If Rate Correction is not configured for the TB then array is not updated*/
                                    }
        #endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */

                                    /* Note : This updation of the record table has to be done before updating the local time base by global time base.*/
                                    /* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
        #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
                                    /* [SWS_StbM_00312] If StbMTimeRecordingSupport is set to TRUE, on an invocation of StbM_BusSetGlobalTime() the StbM shall update
                       all elements of the block of the recording table. If all blocks have been written and no notification via
                       StbM_SyncTimeRecordBlockCallback() or StbM_OffsetTimeRecordBlockCallback() did release all blocks with their elements,
                       the StbM shall again overwrite the Block index 0 (zero) with the incoming measurement data. */

                                    lrecordTableBlockCountCurrent_u16 = StbM_RecordTableBlockCountCurrent_ast[lIndex_u8];

                                    lRecordTableBlockCount_u16 =
                                            StbM_Cfg_TimeBaseBlockRecordArray_ast[lIndex_u8].RecordTableBlockCount_u16;

                                    /* Check if time recording feature support is on for this timebase ID */
                                    if(lRecordTableBlockCount_u16 != STBM_ZERO)
                                    {
                                        if(lrecordTableBlockCountCurrent_u16 == lRecordTableBlockCount_u16)
                                        {
                                            StbM_RecordTableBlockCountCurrent_ast[lIndex_u8] = STBM_ZERO;
                                            lrecordTableBlockCountCurrent_u16 = STBM_ZERO;
                                        }
                                        else
                                        {

                                        }

                                        /* Check if the TimeBase is Synchronized TimeBase */
                                        if(StbM_Rb_IsSyncTimeBase(timeBaseId))
                                        {
                                            lsyncTimeBaseBlockRecordArray = StbM_Cfg_TimeBaseBlockRecordArray_ast[lIndex_u8].
                                                                            SynchronizedTimeBaseBlockRecordArray_ptr;

                                            /* [SWS_StbM_00313] : For Synchronized Time Bases, if Global Time Precision Measurement is enabled
                           (refer to [SWS_StbM_00428] and [SWS_StbM_00307]) for the Time Base, on an invocation of
                           StbM_BusSetGlobalTime() the StbM shall write the block elements LocSeconds and LocNanoSeconds to
                           the related measurement recording table before updating the Main Time Tuple
                           (i.e., updating the Local Time Base by the Global Time Base). LocSeconds and LocNanoSeconds are the
                           elements of the Global Time part of the Synclocal Time Tuple (i.e., TLSync , see [SWS_StbM_00438])*/

                                            /* Update the calculated currrent time value into the array */
                                            (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->LocSeconds =
                                                    lLocalTimeStamp_st.seconds;
                                            (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->LocNanoSeconds
                                                    = lLocalTimeStamp_st.nanoseconds;
                                        }
                                        else
                                        {

                                        }
                                    }
                                    else
                                    {

                                    }
        #endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/

                                    /* [SWS_StbM_00179]
                       For Time Bases 0 to 15 each invocation of StbM_BusSetGlobalTime() shall update the corresponding
                       Main Time Tuple and set the User Data and the Time Base Status accordingly.
                       Note: To update the Main Time Tuple does not mean to automatically overwrite the Main Time Tuple with the
                       Received Time Tuple

                       [SWS_StbM_00393]
                       For Time Bases 16 to 31 each invocation of StbM_BusSetGlobalTime() shall update the corresponding
                       Main Time Tuple and set the User Data and the Time Base Status accordingly.

                       Note: To update the Main Time Tuple does not mean to automatically overwrite the Main Time Tuple with
                       the Received Time Tuple.*/

                                    StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus = lTimebaseStatus_st;
                                    StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds    = globalTimePtr->nanoseconds;    /* nanoseconds value */
                                    StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds        = globalTimePtr->seconds;        /* seconds value */
                                    StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi      = globalTimePtr->secondsHi;      /* seconds Hi value */

                                    /*[SWS_StbM_00433]
                       The Main Time Tuple shall only be updated
                       ->after setting a new Global Time or a new Rate Correction value by the application
                       ->after obtaining a Received Time Tuple (i.e., a new Time Tuple) from a Timesync Module
                       ->after the Offset Correction By Rate Adaption interval (see [SWS_StbM_00353])
                       However, the Main Time Tuple may be updated if there has been no update for more than 3s.

                       [SWS_StbM_00436]
                       Although the retrieved value of the Virtual Local Time and the time which is returned by
                       StbM_GetCurrentTime(), StbM_GetCurrentTimeExtended(), and StbM_BusGetCurrentTime() form a
                       new Time Tuple [TL;TV], this tuple shall only replace the Main Time Tuple if the requirements
                       as specified in [SWS_StbM_00433] are met.

                       [SWS_StbM_00393]
                       For Time Bases 16 to 31 each invocation of StbM_BusSetGlobalTime() shall update the corresponding
                       Main Time Tuple and set the User Data and the Time Base Status accordingly.

                       Note: To update the Main Time Tuple does not mean to automatically overwrite the Main Time Tuple with
                       the Received Time Tuple*/

                                    /* A copy of Received Virtual local time is stored for Timeout Calculation */
                                    StbM_VirtualLocalTimeReception_ast[lIndex_u8].nanosecondsHi = localTimePtr->nanosecondsHi;
                                    StbM_VirtualLocalTimeReception_ast[lIndex_u8].nanosecondsLo = localTimePtr->nanosecondsLo;

                                    /* A copy of Received Global time is stored for TimeLeap Calculation and to calculate Rate Corrected Time */
                                    StbM_GlobalTimeReception_ast[lIndex_u8].timeBaseStatus =
                                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus;
                                    StbM_GlobalTimeReception_ast[lIndex_u8].nanoseconds    =
                                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds   ;
                                    StbM_GlobalTimeReception_ast[lIndex_u8].seconds        =
                                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds    ;
                                    StbM_GlobalTimeReception_ast[lIndex_u8].secondsHi      =
                                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi  ;

                                    /* STBM_EV_RESYNC event bit is set to indicate that resynchronization has occurred and a new time value has been applied*/
                                    if(StbM_CheckEvent_Bit (StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].
                                                                   StbMStatusNotificationMask_u32,STBM_EV_RESYNC))
                                    {
                                        StbM_SetEvent_Bit((StbM_NotificationEvents_au32+lIndex_u8),STBM_EV_RESYNC);
                                    }
                                    else
                                    {
                                        /* do nothing */
                                    }

                                    if(lRet_GCVLTI_u8 == E_OK)
                                    {

        #if (STBM_TIME_CORRECTION ==  STD_ON )
                                        /* Copy the current Global Time value into a variable*/
                                        lTimeStampCurrentGlobal_st.timeBaseStatus =
                                                StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus;
                                        lTimeStampCurrentGlobal_st.nanoseconds    =
                                                globalTimePtr->nanoseconds;
                                        lTimeStampCurrentGlobal_st.seconds        =
                                                globalTimePtr->seconds;
                                        lTimeStampCurrentGlobal_st.secondsHi      =
                                                globalTimePtr->secondsHi;

                                        /* Perform Time Correction Measurements */
                                        StbM_TimeCorrectionMeasurement(lIndex_u8, timeBaseId, lLocalTimeStamp_st,
                                                                 lTimeStampCurrentGlobal_st,(*localTimePtr));
        #endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */

                                        /*[SWS_StbM_00279]: For each Time Base the StbM has a configurable mask StbMStatusNotificationMask , which allows to
                                    mask individually status event notifications.*/
                                        /* [SWS_StbM_00278]:For Synchronized and Offset Time Bases the StbM shall use a variable NotificationEvents of type
                                    StbM_TimeBaseNotificationType to keep track, if any status event for the referenced Time Base occurs.
                                    If any status event occurs and the corresponding bit in the NotificationMask mask is set, the corresponding bit
                                    in the NotificationEvents variable is set , i.e., NotificationEvents contains the bits for the events, which are
                                    enabled within the NotificationMask mask.*/
                                        lNotifyMask_u32 = StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMStatusNotificationMask_u32 ;

                                        /* AR4.2.2 Slave   : [SWS_StbM_00185]Every invocation of StbM_BusSetGlobalTime() shall set the GLOBAL_TIME_BASE bit
                                    within timeBaseStatus of the Time Base. Once set, the bit is never cleared */
                                        /* AR4.2.2 Gateways: [SWS_StbM_00189]Every invocation of StbM_BusSetGlobalTime() shall set the GLOBAL_TIME_BASE bit
                                    within timeBaseStatus of the Time Base. Once set, the bit is never cleared */
                                        /* AR4.3 : [SWS_StbM_00185]For each Time Domain where a Time Slave or a Time Gateway Slave Port belongs to an
                                    invocation of StbM_BusSetGlobalTime() shall set the GLOBAL_TIME_BASE bit within timeBaseStatus of the Time Base.
                                    Once set, the bit is never cleared*/

                                        /* Check if GLOBAL_TIME_BASE bit is not set. If not set, Set it */
                                        if(!(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT)))
                                        {
                                            StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus = StbM_Set_Bit(lTimebaseStatus_st,STBM_GLOBAL_TIME_BASE_BIT,
                                                                         (StbM_NotificationEvents_au32+lIndex_u8),lNotifyMask_u32,STBM_EV_GLOBAL_TIME_BASE);
                                        }
                                        else
                                        {
                                            /* Don't set the bit, as its already set */
                                        }

                                        /*[SWS_StbM_00350]On a valid invocation of StbM_SetGlobalTime(), StbM_BusSetGlobalTime() or StbM_TriggerTimeTransmission()
                    the StbM shall increment the timeBaseUpdateCounterof the corresponding Time Base by 1 (one).
                    At 255 it shall wrap around to 0.*/
                                        (*(StbM_TimeBaseUpdateCounter_au8 + lIndex_u8))++ ;

                                        /* userDataPtr can be either Null_Ptr or with Valid elements. Hence do update UserData, only if its not Null_Ptr */
                                        if(StbM_Rb_IsUserDataValid(userDataPtr) != STBM_USER_DATA_NULL_PTR)
                                        {
                                            /* Set the user data with the new data recieved based on the length provided by user as structure attribute */
                                            StbM_UserDataUpdate(lIndex_u8,userDataPtr);
                                        }
                                        else
                                        {
                                            /* No need to update UserData, as its NULL */
                                        }

                                        /* Exit the resource lock , Since after this NvM API's are called , the locking time may be more. Hence the lock is exit here */
                                        SchM_Exit_StbM_Sync_OsResource();

        #if  (STBM_NVM_ENABLE ==  STD_ON )
                                        /* Set the NvM status as changed since every time the function is called the timebase value will be changed */
                                        /* The lNvm_Index_u8 value is only valid when
                                        StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StoreTimebaseNonVolatile_e==STBM_NVM_STORAGE_AT_SHUTDOWN*/
                                        if (lNvm_Index_u8 != STBM_INVALID_INDEX)
                                        {

                                            StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].nanoseconds    =
                                                    globalTimePtr->nanoseconds     ;/* nanoseconds value */
                                            StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].seconds        =
                                                    globalTimePtr->seconds         ;/* seconds value */
                                            StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].secondsHi      =
                                                    globalTimePtr->secondsHi       ;/* seconds Hi value */

                                            lRet_NSRBS_u8 = NvM_SetRamBlockStatus (STBM_NVM_BLOCK_REFERENCE, TRUE);
                                        }
                                        else
                                        {
                                            /* do nothing */
                                        }
        #endif /* #if (STBM_NVM_ENABLE ==  STD_ON )*/

                                        /* Note : This updation of the record table has to be done after updating the local time base by global time base*/
                                        /* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
        #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )

                                        /* Check if time recording feature support is on for this timebase ID */
                                        if(lRecordTableBlockCount_u16 != STBM_ZERO)
                                        {
                                            /* Check if the TimeBase is Synchronized TimeBase */
                                            if(StbM_Rb_IsSyncTimeBase(timeBaseId))
                                            {
                                                lsyncTimeBaseBlockRecordArray=StbM_Cfg_TimeBaseBlockRecordArray_ast[lIndex_u8].
                                                SynchronizedTimeBaseBlockRecordArray_ptr;

                                                /* [SWS_StbM_00314] For Synchronized Time Bases, if Global Time Precision Measurement is enabled
                               (refer to [SWS_StbM_00428] and [SWS_StbM_00307]) for the Time Base, on an invocation of
                               StbM_BusSetGlobalTime() the StbM shall write the block elements GlbSeconds, GlbNanoSeconds,
                               VirtualLocalTimeLow, RateDeviation, TimeBaseStatus and PathDelay to the related measurement
                               recording table after updating the Main Time Tuple
                               (i.e., after updating the Local Time Base by the Global Time Base). GlbSeconds, GlbNanoSeconds
                               are the elements of the Global Time part of the Received Time Tuple (i.e., TGRx);
                               VirtualLocalTimeLow is the nanosecondsLo element of the Virtual Local Time part of the Received
                                Time Tuple (i.e., TVRx).

                               GlbSeconds, GlbNanoSeconds
                               are the elements of the Global Time part of the Received Time Tuple (i.e., TGRx) */
                                                (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                        GlbSeconds = globalTimePtr->seconds;
                                                (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                        GlbNanoSeconds = globalTimePtr->nanoseconds;
                                                (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                        TimeBaseStatus = StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus;


                                                /* VirtualLocalTimeLow is the nanosecondsLo element of the Virtual Local Time part of the Received
                                Time Tuple (i.e., TVRx) */
                                                (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                        VirtualLocalTimeLow = localTimePtr->nanosecondsLo;

                                                /* Calculated Rate Deviation directly after rate deviation measurement */
        #if (STBM_TIME_CORRECTION ==  STD_ON )
                                                if(StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr
                                                                                                                != NULL_PTR)
                                                {
                                                    /* Check if the rate is already calculated or not */
                                                    if(StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].
                                                            RateCorrectionInfoStruct_ptr->RateCurrent_u32 == STBM_ZERO)
                                                    {
                                                        /* Till now no Rate has been calculated, Hence the rate devaition is ZERO */
                                                        (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                                RateDeviation = STBM_SIGNED_ZERO;
                                                    }
                                                    else
                                                    {
                                                        /* Copy the calculated rate into the block */
                                                        /* MR12 RULE 10.8,10.4,10.3 VIOLATION: The conversion of unsigned int to singned int is necessary in this case */
                                                        (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)
                                                                ->RateDeviation =
                                                          (StbM_RateDeviationType)((StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].
                                                              RateCorrectionInfoStruct_ptr->RateCurrent_u32) - STBM_MILLION);
                                                    }
                                                }
                                                else
                                                {
                                                    (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                            RateDeviation = STBM_SIGNED_ZERO;
                                                }
        #else
                                                (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                        RateDeviation = STBM_SIGNED_ZERO;
        #endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
                                                /* Current propagation delay in nanoseconds */
                                                (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->PathDelay
                                                        = measureDataPtr->pathDelay;
                                            }
                                            else
                                            {
                                                loffsetTimeBaseBlockRecordArray =
                                                StbM_Cfg_TimeBaseBlockRecordArray_ast[lIndex_u8].
                                                                                           OffsetTimeBaseBlockRecordArray_ptr;

                                                /* [SWS_StbM_00388] For Offset Time Bases, if StbMTimeRecordingSupport is set to TRUE, on an invocation of
            StbM_BusSetGlobalTime() the StbM shall write the block elements GlbSeconds, GlbNanoSeconds and
            TimeBaseStatus to the related measurement recording table. */

                                                /* Seconds, Nanoseconds and Time Base Status of the Local Time Base directly after synchronization
            with the Global Time Base */

                                                (loffsetTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                        GlbSeconds = StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds;
                                                (loffsetTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                        GlbNanoSeconds = StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds;
                                                (loffsetTimeBaseBlockRecordArray + lrecordTableBlockCountCurrent_u16)->
                                                        TimeBaseStatus = StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus;


                                            }

                                            /* Update the current block count after the update is done for particular timebase */
                                            StbM_RecordTableBlockCountCurrent_ast[lIndex_u8]++ ;
                                        }
                                        else
                                        {


                                        }
        #endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/
                                    }
                                    else
                                    {
                                        /* Since the lock will not be exit if E_NOT_OK is returned, Exit resource lock is added here */
                                        SchM_Exit_StbM_Sync_OsResource();
                                    }
                                    /* If any status event occurs and the corresponding bit in the NotificationMask mask is set,
                       the StbM shall the callback function StbM_StatusNotificationCallback.
                       If multiple status events occur simultaneously for the same Time Base, the StbM shall call the callback
                       function StbM_StatusNotificationCallback only once.The StbM shall set the eventNotifications parameter
                       of StbM_StatusNotificationCallback to the value of the NotificationEvents variable.*/

                                    lRet_SNCB_u8 = StbM_Rb_StatusNotificationCallback(lIndex_u8);
                                }
                                else
                                {
                                    /*Since the configured timebase is MASTER the functionality is not performed */
                                }
                            }
                            else
                            {
                                /* [SWS_StbM_00456] If the switch StbMDevErrorDetect is set to TRUE, StbM_BusSetGlobalTime()
                                 * shall report to DET the development error STBM_E_PARAM_USERDATA, if called with an invalid
                                 * value of parameter userData, i.e., userDataLength > 3. */
                                /* If userDataLength is invalid(greater than 3) then report error and exit the function */
                                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSSETGLOBALTIME,STBM_E_PARAM_USERDATA)
                            }
                        }
                        else
                        {
                            /* [SWS_StbM_00455] If the switch StbMDevErrorDetect is set to TRUE, StbM_BusSetGlobalTime()
                             * shall report to DET the development error STBM_E_PARAM_TIMESTAMP, if called with a
                             * parameter timeStamp that contains invalid elements (e.g., nanoseconds part > 999999999 ns). */
                            /* If ns part in TimeStamp is greater than 999999999 then report error and exit the function */
                            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSSETGLOBALTIME,STBM_E_PARAM_TIMESTAMP)
                        }
                    }
                    else
                    {
                        /* [SWS_StbM_00235]  If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                         * StbM_BusSetGlobalTime() shall report to DET the development error STBM_E_PARAM_POINTER,
                         * if called with a NULL pointer of parameter localTimePtr. */
                        /* if localTimePtr is NULL pointer then report error and exit the function */
                        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSSETGLOBALTIME,STBM_E_PARAM_POINTER)
                    }
                }
                else
                {
                    /* [SWS_StbM_00235]  If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                     * StbM_BusSetGlobalTime() shall report to DET the development error STBM_E_PARAM_POINTER, if called
                     *  with a NULL pointer of parameter measureDataPtr. */
                    /* if measureDataPtr is NULL pointer then report error and exit the function */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSSETGLOBALTIME,STBM_E_PARAM_POINTER)
                }
            }
            else
            {
                /* [SWS_StbM_00235]  If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                 * StbM_BusSetGlobalTime() shall report to DET the development error STBM_E_PARAM_POINTER, if called
                 * with a NULL pointer of parameter globalTimePtr. */
                /* if globalTimePtr is NULL pointer then report error and exit the function */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSSETGLOBALTIME,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00234]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True,
             * StbM_BusSetGlobalTime() shall report to DET the development error STBM_E_PARAM, if called with an
             * invalid parameter timeBaseID */
            /* To inform DET in case of invalid timebase */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSSETGLOBALTIME,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSSETGLOBALTIME,STBM_E_NOT_INITIALIZED)
    }

    if ((lRet_GCVLTI_u8 != E_NOT_OK)
#if  (STBM_NVM_ENABLE ==  STD_ON )
            && (lRet_NSRBS_u8 != E_NOT_OK)
#endif
            && (lRet_GCTI_u8 != E_NOT_OK)
            && (lRet_SNCB_u8 != E_NOT_OK))
    {
        lRet_Val_u8 = E_OK;
    }
    else
    {
        lRet_Val_u8 = E_NOT_OK;
    }

	return (lRet_Val_u8);
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

/*
**********************************************************************************************************************
* End of the file
**********************************************************************************************************************
*/
