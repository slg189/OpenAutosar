

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
/*The function declarations of RTE interfaced API's will be geberated in the Rte_StbM.h */
#include "Rte_StbM.h"
#endif  /*(STBM_ECUC_RB_RTE_IN_USE == STD_ON) */

#if (STBM_TIME_NOTIFICATION ==  STD_ON )

#if(STBM_DEV_ERROR_DETECT == STD_ON) /*Det included when Det is enabled in StbM*/
#include "Det.h"
/*#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#elif (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != STBM_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif*//*#if(!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))*/

#endif /* #if(STBM_DEV_ERROR_DETECT == STD_ON) */

#include "StbM_Priv.h"
#include "StbM_Inl.h"

/*
**********************************************************************************************************************
* Global Variables
**********************************************************************************************************************
*/

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
StbMNotificationExpiryRecord_tst StbMNotificationExpiryRecordStruct_st ;
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"


/*
**********************************************************************************************************************
* Local functions declarations
**********************************************************************************************************************
*/

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

static uint16 StbM_GetCustomerIndex(StbM_SynchronizedTimeBaseType timeBaseId,
StbM_CustomerIdType customerId );

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* API to compare two TimeStamp values  */
LOCAL_INLINE uint8 StbM_CompareTimeStampStruct(const StbM_TimeStampType * timeStampOne,
const StbM_TimeStampType * timeStampTwo )
{
	if((timeStampOne->nanoseconds == timeStampTwo->nanoseconds) && (timeStampOne->seconds == timeStampTwo->seconds)
			&&(timeStampOne->secondsHi == timeStampTwo->secondsHi))
	{
		return STBM_ONE ;
	}
	else
	{
		return STBM_ZERO ;
	}
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*[SWS_StbM_00270]
On invocation of StbM_StartTimer for a Time Notification Customer of a Time
Base the StbM shall calculate the time CustomerTimerExpireTime when that
Customer Timer will expire based on the corresponding Time Base.*/

/*********************************************************************************************************************
Service name      : StbM_StartTimer
Syntax            : Std_ReturnType StbM_StartTimer( StbM_SynchronizedTimeBaseType timeBaseId,
													StbM_CustomerIdType customerId,
													const StbM_TimeStampType* expireTime )
Service ID[hex]   : 0x15
Sync/Async        : Synchronous
Reentrancy        : Non Reentrant
Parameters (in)   : timeBaseId,
					customerId,
					expireTime
Parameters (inout): None
Parameters (out)  : None
Return value      : Std_ReturnType
Description       :Sets a time value, which the Time Base value is compared against
**********************************************************************************************************************/
/* [SWS_StbM_00272] */
Std_ReturnType StbM_StartTimer (StbM_SynchronizedTimeBaseType timeBaseId,
StbM_CustomerIdType customerId,
const StbM_TimeStampType * expireTime)
{

	Std_ReturnType lRet_Val_u8; /* Return value of StbM_StartTimer() */
	Std_ReturnType lRet_GCT_u8; /* Return value from StbM_GetCurrentTime() */
	uint8 lIndex_u8;
	uint16 lcustomerIndex_u16 ;
	StbM_TimeStampType lTimeStampPtr_st={STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_TimeStampType lexpireTime_st={STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_UserDataType lUserDataPtr_st={STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbMNotificationCustomerRecord_tst * lStbMTimeNotification_ptr;

	/* Local Variable Initialization */
	lRet_Val_u8 = E_NOT_OK;
	lRet_GCT_u8 = E_NOT_OK;
	lcustomerIndex_u16 = STBM_INVALID_INDEX;
    lIndex_u8 = STBM_INVALID_INDEX;
    lStbMTimeNotification_ptr = NULL_PTR ;

	/* Check whether module is initialized */
	if(StbM_InitState_b!=FALSE)
	{
        /* Check if the TimeBaseID is in valid range */
        if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Fetch the index of the Notification Customer */
            lcustomerIndex_u16 = StbM_GetCustomerIndex ( timeBaseId, customerId );

            lStbMTimeNotification_ptr =
                            StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMNotificationCustomerRecordStruct_ptr ;

	        /* Check if the index is valid */
	        if(lcustomerIndex_u16!=STBM_INVALID_INDEX)
	        {

	            /* Check if the timeJump is NULL_PTR or not*/
	            if((expireTime != NULL_PTR))
	            {
	                /* Get the current time for particular time base */
	                lRet_GCT_u8 = StbM_GetCurrentTime(timeBaseId,&lTimeStampPtr_st,&lUserDataPtr_st);

	                if(lRet_GCT_u8 == E_OK)
	                {
	                    /* Fetch the actual time value for CustomerTimerExpireTime */
	                    lexpireTime_st = StbM_Offset_Overflow_Check(lTimeStampPtr_st,expireTime);

	                    /* Store the expireTime passed into the Time Notification Structure */
	                    ((lStbMTimeNotification_ptr->StbMNotificationCustomerInfoStruct_ptr) +
	                            lcustomerIndex_u16 )-> StbMCustomerExpireTime_st  =  lexpireTime_st ;

	                    lRet_Val_u8 = E_OK ;
	                }
	                else
	                {
	                    /* do nothing */
	                }
	            }
	            else
	            {
	                /* [SWS_StbM_00298]  If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_StartTimer() shall
                  report to DET the development error STBM_E_PARAM_POINTER, if called with an invalid pointer of parameter expireTime. */
	                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_STARTTIMER,STBM_E_PARAM_POINTER);
	            }
	        }
	        else
	        {
	            /* [SWS_StbM_00406] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_StartTimer() shall
                          report to DET the development error STBM_E_PARAM, if called with a parameter customerId, which is not configured. */

	            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_STARTTIMER,STBM_E_PARAM);
	        }
	    }
	    else
	    {
	        /* [SWS_StbM_00296] If the switch StbMDevErrorDetect is set to TRUE, StbM_StartTimer() shall report to
                     DET the development error STBM_E_PARAM, if called with a parameter timeBaseID, which
                     - is not configured or
                     - is within the reserved value range. */
	        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_STARTTIMER,STBM_E_PARAM);
	    }
	}
	else
	{
	    /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
	    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_STARTTIMER,STBM_E_NOT_INITIALIZED);
	}
	return (lRet_Val_u8);
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*[SWS_StbM_00335]:The StbM shall cyclically monitor the Time Bases and, if needed,
re-adjust in its StbM_MainFunction the expiration time CustomerTimerExpireTime for
the currently active Time Notification Customers.*/

/*[SWS_StbM_00337]To support N (N > 1) Customer Timers to run and expire within the same interval
StbMTimerStartThreshold, the StbM shall calculate all expiry points within the StbMTimerStartThreshold
interval and re-start the same GPT timer for next expiry point after the previous expiry point has been reached */
/*********************************************************************************************************************
Service name      : StbM_SetExpiryTime
Syntax            : void StbM_SetExpiryTime( StbM_SynchronizedTimeBaseType timeBaseId,
													StbM_CustomerIdType customerId)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : timeBaseId,
					customerId
Parameters (inout): None
Parameters (out)  : None
Return value      : None
Description       : Cyclically monitors the Time Bases and starts the Gpt_StartTimer if the threshold has elapsed
**********************************************************************************************************************/
/*[SWS_StbM_00257]*/
void StbM_SetExpiryTime(StbM_SynchronizedTimeBaseType timeBaseId,
StbM_CustomerIdType customerId)
{

	boolean lGptTimerRunning_b,lCompareExpiryTime_b;
	uint8 lIndex_u8;
	uint16 lCustomerIndex_u16;
	uint8 lgreaterTimeStamp_u8;
	uint64 lDeltaNotify_u64;
	StbM_TimeStampType lexpireTime_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_TimeStampType lTimeStampPtr_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO} ;
	StbM_TimeStampType lDiff_Time_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_TimeStampType lTimerDiff_Time_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_TimeStampType lStbMTimerStartThreshold_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_UserDataType lUserDataPtr_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbMNotificationCustomerRecord_tst * lStbMTimeNotification_ptr;

	/* Local Variable Initialization */
	lIndex_u8   = StbM_Index_au8[timeBaseId];
	lCustomerIndex_u16 = STBM_ZERO ;
	lDeltaNotify_u64 = STBM_ZERO ;
	lStbMTimeNotification_ptr =
	        StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMNotificationCustomerRecordStruct_ptr ;

	/*  Fetch the index for the StbM_TimeNotification Array  */
	lCustomerIndex_u16  = StbM_GetCustomerIndex(timeBaseId ,customerId );

	/* Get the expire time for the particular Customer for the timebase */
	lexpireTime_st   =
	((lStbMTimeNotification_ptr->StbMNotificationCustomerInfoStruct_ptr ) + lCustomerIndex_u16 )->
	                                                                                StbMCustomerExpireTime_st ;

	/* Check that GPT timer has already met the given expiry time*/
	if(StbM_CompareTimeStampStruct
	        (&lexpireTime_st,&StbMNotificationExpiryRecordStruct_st.StbMCustomerLastExpireTime_st) == STBM_ONE)
	{
	    lCompareExpiryTime_b = FALSE;
	}
	else
	{
	    lCompareExpiryTime_b = TRUE;
	}

	/* If the expire time for the timebase is not set or the GPT timer is already for the given expiry Time */
	if((((lexpireTime_st.nanoseconds == STBM_ZERO) && (lexpireTime_st.seconds == STBM_ZERO ) &&
	        (lexpireTime_st.secondsHi == STBM_ZERO))==FALSE)&&(lCompareExpiryTime_b))
	{
	    /* Variable to hold the state of the GPT Timer */
	    lGptTimerRunning_b = StbMNotificationExpiryRecordStruct_st.IsGptTimerRunning_b;

	    /* Get the StbMTimerStartThreshold value for particular customer of the time base */
	    lStbMTimerStartThreshold_st    =
	            ((lStbMTimeNotification_ptr->
	                    StbMNotificationCustomerInfoStruct_ptr ) + lCustomerIndex_u16 )->StbMTimerStartThreshold_st ;

	    /* Get the current time for particular time base */
	    (void)StbM_GetCurrentTime(timeBaseId,&lTimeStampPtr_st,&lUserDataPtr_st);

	    /* Calculate the difference between the CustomerTimerExpireTime and the current value of the corresponding Time Base.*/
	    StbM_Time_DifferenceCalculation(&lexpireTime_st,&lTimeStampPtr_st,&lDiff_Time_st,&lgreaterTimeStamp_u8);

	    if(lgreaterTimeStamp_u8 == STBM_ONE)
	    {
	        /* Calculate the difference between the CustomerTimerExpireTime and the current value of the corresponding Time Base and the StbMTimerStartThreshold.*/
	        StbM_Time_DifferenceCalculation
	        (&lStbMTimerStartThreshold_st,&lDiff_Time_st,&lTimerDiff_Time_st,&lgreaterTimeStamp_u8);

	        /* [SWS_StbM_00336]A time interval StbMTimerStartThreshold  before a Customer Timer expires, the StbM
			shall calculate the time difference between CustomerTimerExpireTime and the current value of the
			corresponding Time Base.The StbM shall then start a GPT timer to be notified, when the time difference has elapsed.*/
	        if(lgreaterTimeStamp_u8 == STBM_ONE )
	        {
	            /* Check If GPT timer is not in RUNNING state */
	            if(lGptTimerRunning_b == FALSE)
	            {
	                /*  Enter Resource lock */
	                SchM_Enter_StbM_Time_Source();

	                /* Set the GPT timer to RUNNING state */
	                StbMNotificationExpiryRecordStruct_st.IsGptTimerRunning_b = TRUE ;

	                /* Copy the expire time ,timebase Id and customer Id into the static global structure*/
	                StbMNotificationExpiryRecordStruct_st.StbMCustomerLastExpireTime_st = lexpireTime_st ;
	                StbMNotificationExpiryRecordStruct_st.SynchronizedTimeBaseIdentifier_u16 = timeBaseId ;
	                StbMNotificationExpiryRecordStruct_st.StbMNotificationCustomerId_u16     = customerId ;

	                /*  Exit Resource lock */
	                SchM_Exit_StbM_Time_Source();

	                /* Convert this difference timestamp to all nanoseconds value */
	                lDeltaNotify_u64 = StbM_ConvertTimestamp2Nanoseconds(lDiff_Time_st);

	            }
	            else
	            {
	                /* Check whether the expiry point for the timebase is less than the globally stored expiry time */
	                StbM_Time_DifferenceCalculation
	                (&StbMNotificationExpiryRecordStruct_st.StbMCustomerLastExpireTime_st,&lexpireTime_st,
	                                                             &lTimerDiff_Time_st,&lgreaterTimeStamp_u8);

	                if(lgreaterTimeStamp_u8 == STBM_ONE)
	                {

	                    /*  Enter Resource lock */
	                    SchM_Enter_StbM_Time_Source();

	                    /* Copy the expire time ,timebase Id and customer Id into the static global structure*/
	                    StbMNotificationExpiryRecordStruct_st.StbMCustomerLastExpireTime_st = lexpireTime_st ;
	                    StbMNotificationExpiryRecordStruct_st.SynchronizedTimeBaseIdentifier_u16 = timeBaseId ;
	                    StbMNotificationExpiryRecordStruct_st.StbMNotificationCustomerId_u16     = customerId ;

	                    /*  Exit Resource lock */
	                    SchM_Exit_StbM_Time_Source();

	                    /* Convert this difference timestamp to all nanoseconds value */
	                    lDeltaNotify_u64 = StbM_ConvertTimestamp2Nanoseconds(lDiff_Time_st);

	                    /* Stop the currently running GPT Timer */
	                    Gpt_StopTimer(STBM_GPT_TIMER_REF);

	                }
	            }

	            /* GPT timer is started */
	            Gpt_StartTimer(STBM_GPT_TIMER_REF,lDeltaNotify_u64);
	        }
	    }
	    else
	    {
	        /* do nothing */
	    }
	}
	else
	{
	    /* do nothing */
	}

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
Service name      : StbM_GetCustomerIndex
Syntax            : uint8 StbM_GetCustomerIndex( StbM_SynchronizedTimeBaseType timeBaseId,
													StbM_CustomerIdType customerId )
Service ID[hex]   : None
Sync/Async        : Synchronous
Reentrancy        : Non Reentrant
Parameters (in)   : timeBaseId,
					customerId
Parameters (inout): None
Parameters (out)  : None
Return value      : uint16
Description       : Returns the index of the Time Notification Customer in StbM_NotificationArray
**********************************************************************************************************************/
/* Local function which returns the index of the Time Notification Customer in StbM_NotificationArray  */
static uint16 StbM_GetCustomerIndex(StbM_SynchronizedTimeBaseType timeBaseId,
StbM_CustomerIdType customerId )
{
	/* Variable Declaration */
	uint8 lIndex_u8;
	uint16 lCustomerIndex_u16;
	uint16 lCount_u16;
	StbMNotificationCustomerRecord_tst * lStbMTimeNotification_ptr;

	/* Local Variable Initialization */
	lCount_u16   = STBM_ZERO ;
	lCustomerIndex_u16 = STBM_INVALID_INDEX ;
    lIndex_u8 = STBM_INVALID_INDEX;
    lStbMTimeNotification_ptr = NULL_PTR ;

    /* Check if the TimeBaseID is in valid range */
    if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
    {
        /* For a timebase within the range, Index is loaded */
        lIndex_u8 = StbM_Index_au8[timeBaseId];

        lStbMTimeNotification_ptr =
                StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMNotificationCustomerRecordStruct_ptr ;

		if(lStbMTimeNotification_ptr != NULL_PTR)
		{
			/* Loop through all the Notification Customers for the timebase ID */
			for(lCount_u16 = STBM_ZERO ; lCount_u16 < lStbMTimeNotification_ptr->StbMNotificationCustomerSize_u8 ;
			    lCount_u16++)
			{
				if(((lStbMTimeNotification_ptr->
				    StbMNotificationCustomerInfoStruct_ptr ) + lCount_u16)->StbMNotificationCustomerId_u16 == customerId)
				{
					lCustomerIndex_u16 = lCount_u16 ;
					break ;
				}
				else
				{
					/*Do nothing*/
				}
			}
		}
		else
		{
			/*Do nothing*/
		}
	}
	else
	{
		/*Do nothing*/
	}
	return lCustomerIndex_u16 ;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*[SWS_StbM_00271] Upon invocation of StbM_TimerCallback, the StbM shall calculate the time difference between CustomerTimerExpireTime
and the current value of the corresponding Time Base.The StbM shall then call <Customer>_TimeNotificationCallback<TimeBase>() to
inform the corresponding Time Notification Customer and return the value of the calculated time difference by parameter deviationTime.*/
/*********************************************************************************************************************
Service name      : StbM_TimerCallback
Syntax            : void StbM_TimerCallback( void )
Service ID[hex]   : None
Sync/Async        : Synchronous
Reentrancy        : Non Reentrant
Parameters (in)   : None
Parameters (inout): None
Parameters (out)  : None
Return value      : None
Description       : Notifies the StbM, that the GPT timer, which is used to trigger the StbM_TimeNotificationCallback has expired.
**********************************************************************************************************************/
/*[SWS_StbM_00257]*/
void StbM_TimerCallback(void)
{
  /* To meet Note in requirement SWS_StbM_00271, when StbMTimeNotificationCallback is not null, the configured TimeNotificationCallback
   * function will be called in Interrupt context, in StbM_TimerCallback_Wrapper().
   * otherwise, the operation NotifyTime will be called in Task context in StbM_TimerCallback_Wrapper() via Rte_IRtrigger
   * and so EventToTaskMapping is needed for RE StbM_TimerCallback_Wrapper in StbM SW-Component, to decouple from
   * Interrupt context to Task context.
   */
  #if (STBM_ECUC_RB_RTE_IN_USE != STD_ON)
    StbM_TimerCallback_Wrapper();
  #else
    /* Call Rte_IrTrigger_StbM_TimerCallback_ITP_TimerNotificationCallBack API to change the context from Interrupt to Task  */
    Rte_IrTrigger_StbM_TimerCallback_ITP_TimerNotificationCallBack();
  #endif
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
Service name      : StbM_TimerCallback_Wrapper
Syntax            : void StbM_TimerCallback_Wrapper( void )
Service ID[hex]   : None
Sync/Async        : Synchronous
Reentrancy        : Non Reentrant
Parameters (in)   : None
Parameters (inout): None
Parameters (out)  : None
Return value      : None
Description       : Calculates the deviation time and informs the customer that the timer has elapsed.
**********************************************************************************************************************/
void StbM_TimerCallback_Wrapper(void)
{

	/* Local Variable declaration */
	uint16 lCustomerIndex_u16;
	uint8 lgreaterTimeStamp_u8;
	uint8 lIndex_u8;
	StbM_SynchronizedTimeBaseType ltimeBaseId_u16;
	StbM_CustomerIdType lCustomerId_u16;
	StbM_TimeDiffType lDeviation_Time_s32;
	uint64 lDeltaNotify_u64;
	StbM_TimeStampType lexpireTime_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_TimeStampType lTimeStampPtr_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbM_UserDataType lUserDataPtr_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO} ;
	StbM_TimeStampType lDiffTime_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
	StbMNotificationCustomerRecord_tst * lStbMTimeNotification_ptr;

	/* Local Variable initialization */
	lgreaterTimeStamp_u8 = STBM_ZERO;
	lDeltaNotify_u64 = STBM_ZERO ;
	lDeviation_Time_s32 = STBM_SIGNED_ZERO ;

	/* Get the expire time for the particular Customer for the timebase */
	lexpireTime_st   =  StbMNotificationExpiryRecordStruct_st.StbMCustomerLastExpireTime_st;
	/* Fetch the timebase ID for which the GPT timer was running */
	ltimeBaseId_u16  =  StbMNotificationExpiryRecordStruct_st.SynchronizedTimeBaseIdentifier_u16;
	/* Fetch the customer ID of the particular timebase for which the GPT timer was running */
	lCustomerId_u16   =  StbMNotificationExpiryRecordStruct_st.StbMNotificationCustomerId_u16 ;

	/*  Get the index of the timebase ID */
	lIndex_u8   = StbM_Index_au8[ltimeBaseId_u16];

	lStbMTimeNotification_ptr =
	        StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMNotificationCustomerRecordStruct_ptr ;

	/*  Fetch the index for the StbM_TimeNotification Array  */
	lCustomerIndex_u16  = StbM_GetCustomerIndex(ltimeBaseId_u16 ,lCustomerId_u16 );

	/* Get the current time for particular time base */
	(void)StbM_GetCurrentTime(ltimeBaseId_u16,&lTimeStampPtr_st,&lUserDataPtr_st);

	/* Calculate the difference between the CustomerTimerExpireTime and the current value of the corresponding Time Base.*/
	StbM_Time_DifferenceCalculation(&lTimeStampPtr_st,&lexpireTime_st,&lDiffTime_st,&lgreaterTimeStamp_u8);

	/* Convert this difference timestamp to all nanoseconds value */
	lDeltaNotify_u64 = StbM_ConvertTimestamp2Nanoseconds(lDiffTime_st);

	/* If current value is greater than the Expiry time */
	if(lgreaterTimeStamp_u8 == STBM_ONE)
	{
	    /* Check for deviation time overflow  */
	    if(lDeltaNotify_u64 > (uint32) STBM_INT_MAX32 )
	    {
	        lDeviation_Time_s32 = (sint32) STBM_INT_MAX32 ;
	    }
	    else
	    {
	        /* Typecast is done since deviation time is within the sint32 range */
	        lDeviation_Time_s32 = (sint32) lDeltaNotify_u64 ;
	    }
	}
	else
	{
	    /* Check for deviation time overflow  */
	    if(lDeltaNotify_u64 > (uint32) STBM_INT_MAX32)
	    {
	        /* MR12 RULE 10.8,10.4,10.3 VIOLATION: The conversion of signed int to unsingned int is necessary in this case */
	        lDeviation_Time_s32 = (sint32) STBM_INT_MIN32 ;
	    }
	    else
	    {
	        /*  Typecast is done since deviation time is within the sint32 range */
	        lDeviation_Time_s32 = -(sint32) lDeltaNotify_u64 ;
	    }
	}

	/* Call <Customer>_TimeNotificationCallback<TimeBase>() to inform the Customer and pass the value of the deviation time calculated*/
	(void)((lStbMTimeNotification_ptr->StbMNotificationCustomerInfoStruct_ptr ) +
	                lCustomerIndex_u16)->StbMTimeNotificationCallback(lDeviation_Time_s32);

	/* Reset the flag to indicate that GPT timer is stopped */
	StbMNotificationExpiryRecordStruct_st.IsGptTimerRunning_b = FALSE ;

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#endif /* #if (STBM_TIME_NOTIFICATION ==  STD_ON )*/
/*
**********************************************************************************************************************
* End of the file
**********************************************************************************************************************
*/
