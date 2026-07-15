

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

#if(STBM_DEV_ERROR_DETECT == STD_ON) /*Det included when Det is enabled in StbM*/
#include "Det.h"
/*#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#elif (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != STBM_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif*/

#endif  /*#if(STBM_DEV_ERROR_DETECT == STD_ON)*/

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

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
StbM_VirtualLocalTimeType StbM_VirtualLocalTimeArray_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

#if(STBM_OS_ENABLE == STD_ON )
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/* array to maintain local time when hardware reference is OS */
TickType StbM_Os_VirtualLocalTimeArray[STBM_NUM_OS_REF];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif/* #if(STBM_OS_ENABLE ==  STD_ON )*/

#if(STBM_ETH_ENABLE == STD_ON )
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/* Array to maintain last update of Virtual Local Time value if hardware reference is Eth */
StbM_TimeStampType StbM_Eth_VirtualLocalTimeArray_ast[STBM_NUM_Eth_REF];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif/* #if(STBM_ETH_ENABLE ==  STD_ON )*/

#if(STBM_GPT_ENABLE == STD_ON )
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/*Array to maintain last update of Virtual Local Time value if hardware reference is GPT */
Gpt_ValueType StbM_Gpt_VirtualLocalTimeArray[STBM_NUM_GPT_REF];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif/* #if(STBM_GPT_ENABLE ==  STD_ON )*/

#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/* Array to maintain last update of Virtual Local Time value if hardware reference is External counter reference */
TickType StbM_ExternalCounter_VirtualLocalTimeArray[STBM_NUM_EXT_REF];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_GetCurrentVirtualLocalTime_Internal
Syntax            : Std_ReturnType StbM_GetCurrentVirtualLocalTime_Internal(uint8 index,
                                      StbM_VirtualLocalTimeType * VLocalTimeStamp_Now,
                                      StbM_VirtualLocalTimeType updateHwCounter)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : timeBaseId       - time base reference
                    updateHwCounter  - parameter to represent whether to update the HW counter or not
Parameters (inout): None
Parameters (out)  : VLocalTimeStamp_Now      -  Current Virtual local Time
Return value      : Std_ReturnType   -  E_OK    : successful
                                        E_NOT_OK: failed
Description       : The API returns Current Virtual Local Time for the time base
**********************************************************************************************************************/
Std_ReturnType StbM_GetCurrentVirtualLocalTime_Internal(uint8 index,
                                      StbM_VirtualLocalTimeType * VLocalTimeStamp_Now,
                                      uint8 updateHwCounter)
{
    /* Local Variable declaration */
    uint64 lCurrentVirtualLocalTime_u64;
    uint64 lDeltaTime_u64;
    uint16 lTimeBaseId_u16;
    uint16 lSyncTimeBaseId_u16;
    Std_ReturnType lRet_Val_u8;
    uint8 lIndex_u8;
    uint8 lIndex_HWRef_u8;
    StbMLocalTimeHardware_ten lStbMLocalTimeHardware_ten;

#if (STBM_ETH_ENABLE == STD_ON)
    Eth_TimeStampType lTimeStampEth_st   = {STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLastTimeEth_st    = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lCurrentTimeEth_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDeltaTime_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    uint8 lCtrlIdx_u8;
    uint8 lgreaterTimeStamp_u8;
    Eth_TimeStampQualType lTimeQual_en;
#endif/* #if (STBM_ETH_ENABLE ==  STD_ON )*/
#if (STBM_GPT_ENABLE == STD_ON)
    Gpt_ChannelType lStbMLocalTimeHardware_GPTRef_ChannelId;
    Gpt_ValueType lDeltaticksGPT;
    Gpt_ValueType lLastGPT_CounterTicks;
    Gpt_ValueType lCurrentGPT_CounterTicks;
#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
#if (STBM_OS_ENABLE == STD_ON)
    TickType lOs_LocalTime;
    TickType lCounterValue_uo,l_Countervalue_uo;
#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
    TickType lCounterValueLast_uo,lCounterValue_Ext_uo,lElapsedCounterValue_Ext_uo;
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/
    /* Local variable initialization */
    lCurrentVirtualLocalTime_u64 = STBM_ZERO;
    lDeltaTime_u64 = STBM_ZERO;
    lRet_Val_u8  = E_NOT_OK;
    lStbMLocalTimeHardware_ten = STBMLOCALTIMEHARDWARE_NOT_CONFIGURED;

#if (STBM_ETH_ENABLE == STD_ON)
    lCtrlIdx_u8 = STBM_ETHIF_INVALID_CTRL_ID;
#endif/* #if (STBM_ETH_ENABLE ==  STD_ON )*/
#if (STBM_GPT_ENABLE == STD_ON)
    lStbMLocalTimeHardware_GPTRef_ChannelId = STBM_GPT_INVALID_CHANNEL_ID;
    lDeltaticksGPT = STBM_ZERO;
    lLastGPT_CounterTicks = STBM_ZERO;
    lCurrentGPT_CounterTicks = STBM_ZERO;
#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
#if (STBM_OS_ENABLE == STD_ON)
    lOs_LocalTime    = STBM_ZERO;
    lCounterValue_uo = STBM_ZERO;
    l_Countervalue_uo = STBM_ZERO;
#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
    lCounterValueLast_uo = STBM_ZERO;
    lCounterValue_Ext_uo = STBM_ZERO;
    lElapsedCounterValue_Ext_uo = STBM_ZERO;
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/

    lTimeBaseId_u16 =
            StbM_Cfg_SynchronizedTimeBaseArray_ast[index].SynchronizedTimeBaseIdentifier_u16;

    /* Fetch the index of the current timebase ID */
    lIndex_u8 = StbM_Index_au8[lTimeBaseId_u16];

    /* Check if the TimeBase is Offest TimeBase */
    if(StbM_Rb_IsOffsetTimeBase(lTimeBaseId_u16))
    {
        /* For offset timebases Localtimeclock container is not configured. Only for Synchronised timebase and Pure local Timebase
        the localtimeclock container is configured(OS/ETH/GPT/EXTERNAL). Hence the referenced sync timebase ID is taken as
        reference for Virtual Local Time calculation  */

        /* Get the referenced sync time base ID for the offset time base  */
        lSyncTimeBaseId_u16 = StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].OffsetTimeBaseRef_u8;

        /* Get the index of the referenced sync time base ID for the offset time base  */
        lIndex_u8 = StbM_Index_au8[lSyncTimeBaseId_u16];
    }
    else
    {
        /* It is a sync/pure local timebase. Current index of the timebase is used i.e.,
         * lTimeBaseId_u16 index of StbM_Index_au8 array */
    }

    /* For offset timebases Localtimeclock container is not configured. Only for Synchronised timebase and Pure local Timebase
       the localtimeclock container is configured(OS/ETH/GPT/EXTERNAL) */

    /* Fetch the Hardware reference (Eth/GPT/OS)*/
    lStbMLocalTimeHardware_ten = StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMLocalTimeHardware_e;

    /* Get the HW reference index of the timebase from StbM_Index_HWCounterRef_ast array for OS/GPT/Eth/External HW reference */
    lIndex_HWRef_u8 = StbM_Index_HWCounterRef_ast[lIndex_u8];

    /* Check last time when Virtual local time was updated*/
    if(lStbMLocalTimeHardware_ten == OS)
    {
#if (STBM_OS_ENABLE == STD_ON)

        /* Last time when Virtual local time was updated*/
        lOs_LocalTime = StbM_Os_VirtualLocalTimeArray[lIndex_HWRef_u8];

        /* [SWS_StbM_00352]
           In the scope of StbM_GetCurrentTime(), and StbM_GetCurrentTimeExtended() and StbM_BusGetCurrentTime(),
           StbM shall use the factor (StbMClockPrescaler /StbMClockFrequency) to convert the time of its local hardware
           reference clock to the actual time of the Virtual Local Time.(SRS_StbM_20065)

           Note: Rationale is that a tick duration of the hardware reference clock does not necessarily have to match
           the resolution of the Virtual Local Time.*/

        if(updateHwCounter == STBM_COUNTER_UPDATE)
        {
            lRet_Val_u8 = GetCounterValue(StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMLocalTimeHardware_OsRef,
                    &l_Countervalue_uo);

            if( lRet_Val_u8 == E_OK)
            {
                /* Store the current time value in StbM_Os_VirtualLocalTimeArray array to note the Local Time value update */
                StbM_Os_VirtualLocalTimeArray[lIndex_HWRef_u8] = l_Countervalue_uo;
            }
            else
            {
                /* QAC - do nothing */
            }
        }
        else
        {
            /* Counter value is not stored in the StbM_Os_VirtualLocalTimeArray array to note the Local Time value update */
        }

        /* Calculate the delta time from the last update of time value  */
        lRet_Val_u8 = GetElapsedValue(StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMLocalTimeHardware_OsRef,
                &lOs_LocalTime,&lCounterValue_uo);

        if (lRet_Val_u8 == E_OK)
        {
            /*Convert the ticks to nanoseconds value*/
            lDeltaTime_u64 =
                    (((uint64)lCounterValue_uo * StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMClockRatio_u32)/1000u);

        }
        else
        {
            /* Time difference is not calculated*/
        }

#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
    }
    else if(lStbMLocalTimeHardware_ten == ETHTSYN)
    {
#if (STBM_ETH_ENABLE == STD_ON)

        lCtrlIdx_u8 = (uint8)StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMLocalTimeHardware_EthIf_Ctrl_Id;

        /* Get the current time from EthIf layer */
        lRet_Val_u8 = EthIf_GetCurrentTime(lCtrlIdx_u8,&lTimeQual_en,&lTimeStampEth_st);

        if (lRet_Val_u8 == E_OK)
        {
            /* Fetch the last time when Eth's local time was updated for current time base ID */
            lLastTimeEth_st.nanoseconds = StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].nanoseconds;
            lLastTimeEth_st.seconds     = StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].seconds;
            lLastTimeEth_st.secondsHi   = StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].secondsHi;

            /*Copy the nanoseconds value into the variable*/
            lCurrentTimeEth_st.nanoseconds = lTimeStampEth_st.nanoseconds;
            lCurrentTimeEth_st.seconds     = lTimeStampEth_st.seconds;
            lCurrentTimeEth_st.secondsHi   = lTimeStampEth_st.secondsHi;

            if(updateHwCounter == STBM_COUNTER_UPDATE)
            {
                /* Store the current time value in StbM_Eth_VirtualLocalTimeArray_ast array to note the Local Time value update */
                StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].nanoseconds = lCurrentTimeEth_st.nanoseconds;
                StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].seconds     = lCurrentTimeEth_st.seconds;
                StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].secondsHi   = lCurrentTimeEth_st.secondsHi;
            }
            else
            {
                /* Counter value is not stored in the StbM_Os_VirtualLocalTimeArray array to note the Local Time value update */
            }

            /* Get time difference between last time, Local time was updated for current timebase ID and current time as returned by EthIf */
            StbM_Time_DifferenceCalculation(&lCurrentTimeEth_st,&lLastTimeEth_st,&lDeltaTime_st,&lgreaterTimeStamp_u8);

            /* Convert the ticks vae into uint64 value*/
            lDeltaTime_u64 = ((uint64)lDeltaTime_st.seconds*STBM_NS_PER_SEC) + lDeltaTime_st.nanoseconds;

        }
        else
        {
            /* Time difference is not calculated*/
        }

#endif /* #if (STBM_ETH_ENABLE ==  STD_ON )*/
    }
    else if(lStbMLocalTimeHardware_ten == GPT)
    {
#if (STBM_GPT_ENABLE == STD_ON)
        /* GPT Counter's tick for current timebase ID when the time was last updated is */
        lLastGPT_CounterTicks = StbM_Gpt_VirtualLocalTimeArray[lIndex_HWRef_u8];

        /* Get the referenced GPT channel ID */
        lStbMLocalTimeHardware_GPTRef_ChannelId =
                StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMLocalTimeHardware_GptChannel_Id;

        /* Get the current counter ticks value*/
        lCurrentGPT_CounterTicks = Gpt_GetTimeElapsed(lStbMLocalTimeHardware_GPTRef_ChannelId);

        if(updateHwCounter == STBM_COUNTER_UPDATE)
        {
            /* Store the current time value in StbM_Gpt_VirtualLocalTimeArray array to note the Local Time value update */
            StbM_Gpt_VirtualLocalTimeArray[lIndex_HWRef_u8] = lCurrentGPT_CounterTicks;
        }
        else
        {
            /* Counter value is not stored in the StbM_Os_VirtualLocalTimeArray array to note the Local Time value update */
        }

        /* [SWS_StbM_00352]
           In the scope of StbM_GetCurrentTime(), and StbM_GetCurrentTimeExtended() and StbM_BusGetCurrentTime(),
           StbM shall use the factor (StbMClockPrescaler /StbMClockFrequency) to convert the time of its local hardware
           reference clock to the actual time of the Virtual Local Time.(SRS_StbM_20065)

           Note: Rationale is that a tick duration of the hardware reference clock does not necessarily have to match
           the resolution of the Virtual Local Time.*/

        /* Get delta tick value for GPT counter from last updated time */
        lDeltaticksGPT = StbM_DeltaTimeCalculation_GPT(lLastGPT_CounterTicks, lCurrentGPT_CounterTicks);

        /* Calculate the delta time from the last update of time value  */
        lDeltaTime_u64 =
                (((uint64)lDeltaticksGPT * StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMClockRatio_u32)/1000u);

        lRet_Val_u8 = E_OK;


#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
    }
    else /*if (lStbMLocalTimeHardware_ten == EXTERNAL_COUNTER) */
    {
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)

		/* Check the counter value when last time VLT was updated*/
		lCounterValueLast_uo = StbM_ExternalCounter_VirtualLocalTimeArray[lIndex_HWRef_u8];

		/* Get the elapsed value of Virtual Local time value into a local variable */
		StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMExternalCounterGetElapsedValueCallback
		(&lCounterValueLast_uo,&lElapsedCounterValue_Ext_uo) ;

		/* Check the current time value*/
		StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMExternalCounterGetCounterValueCallback
		(&lCounterValue_Ext_uo);

		if(updateHwCounter == STBM_COUNTER_UPDATE)
		{
			/* Store the current time value in StbM_ExternalCounter_VirtualLocalTimeArray array to note the Local Time value update */
			StbM_ExternalCounter_VirtualLocalTimeArray[lIndex_HWRef_u8] = lCounterValue_Ext_uo;
		}

		/*  Call the API to convert the ticks to nanoseconds */
		lDeltaTime_u64 = StbM_Cfg_LocalTimeClockArray_ast[lIndex_u8].StbMExternalCounterTicksTonsCallback
																			(lElapsedCounterValue_Ext_uo);

		lRet_Val_u8 = E_OK;

#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/
    }

    /* Convert the StbM_VirtualLocalTimeType into uint64 value*/
    lCurrentVirtualLocalTime_u64 = (((uint64)(StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsHi)) << 32) |
            (uint64)(StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsLo);

    /* Add the delta difference value to the Virtual Local Time*/
    lCurrentVirtualLocalTime_u64 = lCurrentVirtualLocalTime_u64 + lDeltaTime_u64;

    /* Convert the uint64 parameter into two uint32 paramters , high and low
                       Nanoseconds value is rightshift by 32 to get the high 32 bit value
                       Nanoseconds value is AND with 0xFFFFFFFF to get the low 32 bit value */

    VLocalTimeStamp_Now->nanosecondsHi = (uint32)(lCurrentVirtualLocalTime_u64 >> STBM_THIRTYTWO) ;

    VLocalTimeStamp_Now->nanosecondsLo = (uint32)(lCurrentVirtualLocalTime_u64 & STBM_MAX_SEC) ;

    return lRet_Val_u8;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_Rb_VirtualLocalTimeDifference
Syntax            : StbM_VirtualLocalTimeType StbM_Rb_VirtualLocalTimeDifference
                        (StbM_VirtualLocalTimeType VLocalTimeStamp_Last,
                         StbM_VirtualLocalTimeType VLocalTimeStamp_Current)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : VLocalTimeStamp_Old - Previous Virtual Local Time Value
Parameters (inout): None
Parameters (out)  : VLocalTimeStamp_Current - Current Virtual Local Time Value
Return value      : StbM_VirtualLocalTimeType - The Virtual Local Time difference
Description       : Local function to calculate the timevalue difference between the two virtual local time.
 **********************************************************************************************************************/
StbM_VirtualLocalTimeType StbM_Rb_VirtualLocalTimeDifference
        (StbM_VirtualLocalTimeType VLocalTimeStamp_Last,
         StbM_VirtualLocalTimeType VLocalTimeStamp_Current)
{
    /* Local Variable declaration */
    uint64 lVirtualLocalTimeCurrent_u64 = STBM_ZERO;
    uint64 lVirtualLocalTimeLast_u64 = STBM_ZERO;
    uint64 lVirtualLocalTimeDiff_u64 = STBM_ZERO;
    StbM_VirtualLocalTimeType lStbM_VirtualLocalTimeDiff_st = {STBM_ZERO,STBM_ZERO};

    /* Convert the StbM_VirtualLocalTimeType into uint64 value*/
    lVirtualLocalTimeCurrent_u64 = (((uint64)(VLocalTimeStamp_Last.nanosecondsHi)) << 32) |
                              (uint64)(VLocalTimeStamp_Last.nanosecondsLo);

    /* Convert the StbM_VirtualLocalTimeType into uint64 value*/
    lVirtualLocalTimeLast_u64 = (((uint64)(VLocalTimeStamp_Current.nanosecondsHi)) << 32) |
                              (uint64)(VLocalTimeStamp_Current.nanosecondsLo);

    if(lVirtualLocalTimeLast_u64>lVirtualLocalTimeCurrent_u64)
    {
        lVirtualLocalTimeDiff_u64 = lVirtualLocalTimeLast_u64 - lVirtualLocalTimeCurrent_u64;
    }
    else
    {
        lVirtualLocalTimeDiff_u64 = lVirtualLocalTimeCurrent_u64 - lVirtualLocalTimeLast_u64;
    }

    /* Convert the uint64 parameter into two uint32 paramters , high and low
                   Nanoseconds value is rightshift by 32 to get the high 32 bit value
                   Nanoseconds value is AND with 0xFFFFFFFF to get the low 32 bit value */

    lStbM_VirtualLocalTimeDiff_st.nanosecondsHi = (uint32)(lVirtualLocalTimeDiff_u64 >> STBM_THIRTYTWO) ;

    lStbM_VirtualLocalTimeDiff_st.nanosecondsLo = (uint32)(lVirtualLocalTimeDiff_u64 & STBM_MAX_SEC) ;

    return(lStbM_VirtualLocalTimeDiff_st);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
*Service name      : StbM_VirtualLocalTime_Init
*Syntax            : Std_ReturnType StbM_VirtualLocalTime_Init
                     (uint8 index)
*Service ID[hex]   : None
*Sync/Async        : None
*Reentrancy        : None
*Parameters (in)   : index                - Index of the timebase ID
*Parameters (inout): None
*Parameters (out)  : None
*Return value      : None
*Description       : This function is used to start/initilaise the Virtual Local Time
***********************************************************************************************************************/
void StbM_VirtualLocalTime_Init(uint8 index)
{
    /* Local Variable declaration */
    uint8 lIndex_HWRef_u8;
    StbMLocalTimeHardware_ten lStbMLocalTimeHardware_ten;
#if ((STBM_OS_ENABLE == STD_ON) || (STBM_ETH_ENABLE == STD_ON))
    Std_ReturnType lRet_Val_u8;
#endif

#if (STBM_OS_ENABLE == STD_ON)
    TickType l_Countervalue_uo;
#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
    TickType lCounterValue_Ext_uo;
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/
#if (STBM_GPT_ENABLE == STD_ON)
    Gpt_ChannelType lStbMLocalTimeHardware_GPTRef_ChannelId;
    Gpt_ValueType lGPT_CounterTicks;
#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
#if (STBM_ETH_ENABLE == STD_ON)
    Eth_TimeStampQualType lTimeQual_en;
    uint8 lStbMLocalTimeHardware_EthRef_CtrlIdx_u8;
    Eth_TimeStampType lTimeStamp_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO};
#endif/* #if (STBM_ETH_ENABLE ==  STD_ON )*/

    /* Local Variable Initialization */
#if (STBM_OS_ENABLE == STD_ON)
    l_Countervalue_uo = STBM_ZERO;
#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
    lCounterValue_Ext_uo = STBM_ZERO;
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/
#if (STBM_ETH_ENABLE == STD_ON)
    lTimeQual_en = ETH_UNCERTAIN;
    lStbMLocalTimeHardware_EthRef_CtrlIdx_u8 = STBM_ETHIF_INVALID_CTRL_ID;
#endif/* #if (STBM_ETH_ENABLE ==  STD_ON )*/
#if (STBM_GPT_ENABLE == STD_ON)
    lGPT_CounterTicks = STBM_ZERO;
    lStbMLocalTimeHardware_GPTRef_ChannelId = STBM_GPT_INVALID_CHANNEL_ID;
#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
#if ((STBM_OS_ENABLE == STD_ON) || (STBM_ETH_ENABLE == STD_ON))
    lRet_Val_u8 = E_NOT_OK;
#endif

    /* Get the type of the HW reference taht is used for the timebase */
    lStbMLocalTimeHardware_ten = StbM_Cfg_LocalTimeClockArray_ast[index].StbMLocalTimeHardware_e;

    /* Get the index of LocalTime_ast array from StbM_Index_HWCounterRef_ast array for OS/GPT/Eth HW reference */
    lIndex_HWRef_u8 = StbM_Index_HWCounterRef_ast[index];

#if (STBM_OS_ENABLE == STD_ON)
        if(lStbMLocalTimeHardware_ten == OS)
        {
            /* Check the current invocation time of the API */
            lRet_Val_u8 = GetCounterValue(StbM_Cfg_LocalTimeClockArray_ast[index].StbMLocalTimeHardware_OsRef,
                    &l_Countervalue_uo);

            if( lRet_Val_u8 == E_OK)
            {
                /* Store the current time value in StbM_Os_VirtualLocalTimeArray array to note the Virtual Local Time value update */
                StbM_Os_VirtualLocalTimeArray[lIndex_HWRef_u8] = l_Countervalue_uo;
            }
            else
            {
                /* Array is not updated */
            }
        }
        else
#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
#if (STBM_ETH_ENABLE == STD_ON)
        if(lStbMLocalTimeHardware_ten == ETHTSYN)
        {
            /* Get Ethernet control ID */
            lStbMLocalTimeHardware_EthRef_CtrlIdx_u8 =
                    (uint8)StbM_Cfg_LocalTimeClockArray_ast[index].StbMLocalTimeHardware_EthIf_Ctrl_Id ;

            /* Get current time for that control ID */
            lRet_Val_u8 = EthIf_GetCurrentTime(lStbMLocalTimeHardware_EthRef_CtrlIdx_u8,&lTimeQual_en,&lTimeStamp_st);

            if( lRet_Val_u8 == E_OK)
            {
                /* Store the current time value in StbM_Eth_VirtualLocalTimeArray_ast array to note the Local Time value update */
                StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].nanoseconds = lTimeStamp_st.nanoseconds;
                StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].seconds     = lTimeStamp_st.seconds;
                StbM_Eth_VirtualLocalTimeArray_ast[lIndex_HWRef_u8].secondsHi   = lTimeStamp_st.secondsHi;
            }
            else
            {
                /* Time value is not stored */
            }
        }
        else
#endif /* #if (STBM_ETH_ENABLE ==  STD_ON )*/
#if (STBM_GPT_ENABLE == STD_ON)
        if(lStbMLocalTimeHardware_ten == GPT)
        {

            /* Get the GPT channel reference */
            lStbMLocalTimeHardware_GPTRef_ChannelId =
                    StbM_Cfg_LocalTimeClockArray_ast[index].StbMLocalTimeHardware_GptChannel_Id;

            /* Get GPT tick value */
            lGPT_CounterTicks = Gpt_GetTimeElapsed(lStbMLocalTimeHardware_GPTRef_ChannelId);

            /* Store the current time value in StbM_Gpt_VirtualLocalTimeArray array to note the Local Time value update */
            StbM_Gpt_VirtualLocalTimeArray[lIndex_HWRef_u8] = lGPT_CounterTicks;
        }
        else
#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
        /*lStbMLocalTimeHardware_ten == EXTERNAL_COUNTER*/
        if (lStbMLocalTimeHardware_ten == EXTERNAL_COUNTER)
        {
            StbM_Cfg_LocalTimeClockArray_ast[index].StbMExternalCounterGetCounterValueCallback(&lCounterValue_Ext_uo);

            /* Store the current time value in StbM_ExternalCounter_VirtualLocalTimeArray array to note the Local Time value update */
            StbM_ExternalCounter_VirtualLocalTimeArray[lIndex_HWRef_u8] = lCounterValue_Ext_uo;

        }
        else
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/
        {
            /* Local Hardware reference is not configured for the timebase. Hence no time value is updated */
        }

        /* Initialise the Virtual local time array with zero  */
        StbM_VirtualLocalTimeArray_ast[index].nanosecondsHi    = STBM_ZERO;
        StbM_VirtualLocalTimeArray_ast[index].nanosecondsLo    = STBM_ZERO;

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
 *Service name       : StbM_GetCurrentVirtualLocalTime
 *Syntax             : Std_ReturnType StbM_GetCurrentVirtualLocalTime( StbM_SynchronizedTimeBaseType timeBaseId,
 *                                                       StbM_VirtualLocalTimeType* localTimePtr)
 *Service ID[hex]    : 0x1e
 *Sync/Async         : Synchronous
 *Reentrancy         : Non Reentrant
 *Parameters (in)    : timeBaseId       -  time base reference
 *Parameters (inout) : None
 *Parameters (out)   : localTimePtr     -  Current Virtual Local Time value
 *Return value       : Std_ReturnType   -  E_OK: successful
                                           E_NOT_OK: failed
 *Description        : Returns the Virtual Local Time of the referenced Time Base.
 **********************************************************************************************************************/
/*[SWS_StbM_00233]*/
Std_ReturnType StbM_GetCurrentVirtualLocalTime(
        StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_VirtualLocalTimeType * localTimePtr)
{
    /* Local Variable declaration */
    StbM_VirtualLocalTimeType lTimeStampVLocalNow_st  = {STBM_ZERO,STBM_ZERO};
    Std_ReturnType lRet_Val_u8;
    uint8 lIndex_u8;

    /* Local Variable Initialization */
    lRet_Val_u8 = E_NOT_OK;
    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBaseID is in valid range */
        if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the userdata is NULL_PTR or not*/
            if((localTimePtr != NULL_PTR))
            {
                /* [SWS_StbM_00437]
           StbM_GetCurrentVirtualLocalTime() shall return the value of the Virtual Local Time of the associated
           Time Base. If the Virtual Local Time could not be determined (e.g., the underlying hardware counter was not
           yet activated), StbM_GetCurrentVirtualLocalTime() shall return E_NOT_OK */

                /*Call the StbM_GetCurrentVirtualLocalTime_Internal to read the current Virtual Local Time */
                lRet_Val_u8 =
                StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocalNow_st,STBM_NO_COUNTER_UPDATE);

                if(lRet_Val_u8 == E_OK)
                {
                    /* Assign the converted time value to the output parameter */
                    localTimePtr->nanosecondsHi = lTimeStampVLocalNow_st.nanosecondsHi;
                    localTimePtr->nanosecondsLo = lTimeStampVLocalNow_st.nanosecondsLo;
                }
                else
                {
                    lRet_Val_u8 = E_NOT_OK;
                }
            }
            else
            {
                /* [SWS_StbM_00444] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                   StbM_GetCurrentVirtualLocalTime () shall report to DET the development error STBM_E_PARAM_POINTER,
                   if called with a NULL pointer for parameter localTimePtr */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTVIRTUALLOCALTIME,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00445] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
               StbM_GetCurrentVirtualLocalTime () shall report to DET the development error STBM_E_PARAM,
               if called with a parameter timeBaseId, which
               1) is not configured or
               2) is within the reserved value range  */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTVIRTUALLOCALTIME,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTVIRTUALLOCALTIME,STBM_E_NOT_INITIALIZED)
    }

    return (lRet_Val_u8);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
*Service name      : StbM_Rb_ConvertVirtualLocalTimeToTimestampType
*Syntax            : void StbM_Rb_ConvertVirtualLocalTimeToTimestampType
                         (const StbM_VirtualLocalTimeType * localTimePtr,
                          StbM_TimeStampType * VirtuallocalTimePtr)
*Service ID[hex]   : None
*Sync/Async        : None
*Reentrancy        : None
*Parameters (in)   : localTimePtr  - Virtual local time value that is passed
*Parameters (inout): None
*Parameters (out)  : VirtuallocalTimePtr - Virtual local time value that is converted into StbM_TimeStampType format
*Return value      : Std_ReturnType E_OK: successful E_NOT_OK: failed
*Description       : API to convert the Virtual local time into StbM_TimeStampType format
***********************************************************************************************************************/
 void StbM_Rb_ConvertVirtualLocalTimeToTimestampType
                           (StbM_VirtualLocalTimeType localTimePtr,
                            StbM_TimeStampType * VirtuallocalTimePtr)
{
    /* Local Variable declaration */
    uint64 lVirtualLocalTime_u64;

    /* Local Variable Initialization */
    lVirtualLocalTime_u64 = STBM_ZERO;

    /* Convert the StbM_VirtualLocalTimeType into uint64 value*/
    lVirtualLocalTime_u64 = (((uint64)(localTimePtr.nanosecondsHi)) << 32) | (uint64)(localTimePtr.nanosecondsLo);

    /* Convert this to Timestamp format */
    VirtuallocalTimePtr->secondsHi      = (uint16)((lVirtualLocalTime_u64/(STBM_S_PER_SECHI))/(STBM_NS_PER_SEC));
    VirtuallocalTimePtr->seconds        = (uint32)((lVirtualLocalTime_u64 -
                                             (VirtuallocalTimePtr->secondsHi * (STBM_S_PER_SECHI) * (STBM_NS_PER_SEC)))
                                                   /(STBM_NS_PER_SEC));
    VirtuallocalTimePtr->nanoseconds    = (uint32)
                                            (lVirtualLocalTime_u64 -
                                            (VirtuallocalTimePtr->secondsHi * (STBM_S_PER_SECHI) * (STBM_NS_PER_SEC)) -
                                            (VirtuallocalTimePtr->seconds * (STBM_NS_PER_SEC)));

    VirtuallocalTimePtr->timeBaseStatus = STBM_ZERO;
}
#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
