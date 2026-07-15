

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

#if (STBM_TIME_CORRECTION ==  STD_ON )

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
 * Local functions decalrations
 **********************************************************************************************************************
 */

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

static void StbM_StartRateCorrectionMeasurement(uint8 Index,
        const StbM_VirtualLocalTimeType * VLocalTimeStart,
        const StbM_TimeStampType * GlobalTimeStart);

static void StbM_EndRateCorrectionMeasurement(uint8 Index,
        const StbM_VirtualLocalTimeType * VLocalTimeStart,
        const StbM_TimeStampType * globalTimeStart,
        const StbM_TimeStampType * globalTimeEnd,
        const StbM_VirtualLocalTimeType * VLocalTimeEnd);

static void StbM_CheckOffsetCorrection(uint8 index,
        StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_TimeStampType * timeStampPtr);

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* [SWS_StbM_00378] */
/*********************************************************************************************************************
Service name      : StbM_GetRateDeviation
Syntax            : Std_ReturnType StbM_GetRateDeviation( StbM_SynchronizedTimeBaseType timeBaseId,
                                                          StbM_RateDeviationType* rateDeviation )
Service ID[hex]   : 0x11
Sync/Async        : Synchronous
Reentrancy        : Reentrant
Parameters (in)   : timeBaseId
Parameters (inout): None
Parameters (out)  : rateDeviation
Return value      : Std_ReturnType
Description       : Returns value of the current rate deviation of a Time Base
 **********************************************************************************************************************/
/* RFC 77938 :
   SWS_StbM_00397] For Time Bases with StbMSynchronizedTimeBaseIdentifier 0 to 31 and StbMIsSystemWideGlobalTimeMaster
   = False, the StbM shall return on invocation of StbM_GetRateDeviation() the rate deviation, which
   has been calculated for that Time Base (i.e., rrc -1 for Synchronized Time Bases or rorc - 1 for Offset Time Bases).
   If no rate deviation has been calculated, StbM_GetRateDeviation() shall return E_NOT_OK. */
/* [SWS_StbM_00xxxx] For Time Bases with StbMSynchronizedTimeBaseIdentifier 32 to 127 and for Time Bases with
   StbMSynchronizedTimeBaseIdentifier 0 to 31 and StbMIsSystemWideGlobalTimeMaster = True, the StbM shall return on
   invocation of StbM_GetRateDeviation() the rate deviation that has been set by StbM_SetRateCorrection() for that Time
   Base. If no rate deviation has been set, StbM_GetRateDeviation() shall return E_NOT_OK. */
Std_ReturnType StbM_GetRateDeviation(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_RateDeviationType * rateDeviation)
    {
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

             /* Check if the rateDeviation is NULL_PTR or not*/
             if(rateDeviation != NULL_PTR)
             {
                 /* Check if the Time Correction feature is configured for the timebase */
                 if(StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].TimeCorrectionType_ten != STBM_TIME_CORRECTION_NOT_CONFIGURED)
                 {
                     /* Check, if we have already calculated some rate or not, if no rate has been calculated then E_NOT_OK
       has to be returned */
                     if(StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->RateCurrent_u32
                                                                                                          != STBM_ZERO)
                     {
                         /* If the passed timebase ID is pure local time and configured as slave then E_NOT_OK has to be returned */
                         if(!((StbM_Rb_IsPureLocalTimeBase(timeBaseId))&&
                                 (StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].IsSystemWideGlobalTimeMaster_b
                                                                                                            == FALSE)))
                         {
                             /*MR12 RULE 10.3,10.4,10.8 VIOLATION: The conversion from unsigned to signed is necessary in this case
                 Since the value returned should be of type StbM_RateDeviationType */
                             *rateDeviation = (StbM_RateDeviationType)(((StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->
                                      RateCurrent_u32)) - STBM_MILLION);
                             lRet_Val_u8 = E_OK;
                         }
                         else
                         {
                             /* Passed timebase ID is pure local and is configured as a SLAVE. E_NOT_OK will be returned. */
                             /* Do nothing*/
                         }
                     }
                     else
                     {
                         /* No rate has been calculated yet. E_NOT_OK will be returned. */
                         /* Do Nothing */
                     }
                 }
                 else
                 {
                     /* Return E_NOT_OK since the Time Correction feature is not configured for the timebase*/
                 }
             }
             else
             {
                 /* [SWS_StbM_00380] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_GetRateDeviation()
                    shall report to DET the development error STBM_E_PARAM_POINTER, if called with a NULL pointer of parameter
                    rateDeviation. */
                 STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETRATEDEVIATION,STBM_E_PARAM_POINTER)
             }
         }
         else
         {
             /* RFC : 77938
                [SWS_StbM_00379] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_GetRateDeviation()
                shall report to DET the development error STBM_E_PARAM, if called with a parameter timeBaseID, which
                -is not configured or
                -is within the reserved value range. */
             STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETRATEDEVIATION,STBM_E_PARAM)
         }
     }
     else
     {
         /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
         STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETRATEDEVIATION,STBM_E_NOT_INITIALIZED)
     }

     return (lRet_Val_u8);
    }


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* [SWS_StbM_00390] */
/*********************************************************************************************************************
Service name      : StbM_SetRateCorrection
Syntax            : Std_ReturnType StbM_SetRateCorrection( StbM_SynchronizedTimeBaseType timeBaseId,
                                                           StbM_RateDeviationType rateDeviation )
Service ID[hex]   : 0x12
Sync/Async        : Synchronous
Reentrancy        : Reentrant
Parameters (in)   : timeBaseId, rateDeviation
Parameters (inout): None
Parameters (out)  : None
Return value      : Std_ReturnType
Description       : Allows to set the rate of a Synchronized Time Base (being either a Pure Local Time Base or not).
 **********************************************************************************************************************/

Std_ReturnType StbM_SetRateCorrection(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_RateDeviationType rateDeviation)
{
    Std_ReturnType lRet_Val_u8;
    uint8 lIndex_u8;
    StbM_RateDeviationType rateDeviationTemp;
    StbM_VirtualLocalTimeType lVirtualLocalTimeDiff_st  = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocalLast_st = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocalNow_st  = {STBM_ZERO,STBM_ZERO};

    /* Local Variable Initialization */
    lRet_Val_u8 = E_NOT_OK;
    rateDeviationTemp = rateDeviation;

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

            /* [SWS_StbM_00395] If StbMAllowMasterRateCorrection equals TRUE, an invocation of StbM_SetRateCorrection()
            shall set the rate correction value. Otherwise StbM_SetRateCorrection() shall do nothing and return E_NOT_OK. */
            if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMAllowMasterRateCorrection_b == TRUE)
            {

                /* [SWS_StbM_00396] If the absolute value of the rate correction parameter rateDeviation, which is passed to
                StbM_SetRateCorrection(), is greater than StbMMasterRateDeviationMax, StbM_SetRateCorrection shall set the
                actually applied rate correction value to either (StbMMasterRateDeviationMax) or
                (-StbMMasterRateDeviationMax), depending on sign of rateDeviation. */
                if(rateDeviation < STBM_SIGNED_ZERO)
                {
                    /* rateDeviation passed is negative */
                    rateDeviationTemp = rateDeviationTemp * STBM_VAL_MINUS_ONE;
                    if(rateDeviationTemp >
                    StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMMasterRateDeviationMax_u16)
                    {
                    /*MR12 RULE 10.3 VIOLATION: The conversion from unsigned to signed is necessary in this case
                    Since the value returned should be of type StbM_RateDeviationType */
                    rateDeviationTemp =
                    StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMMasterRateDeviationMax_u16
                    * STBM_VAL_MINUS_ONE;
                    }
                }
                else /* (rateDeviation > STBM_SIGNED_ZERO)*/
                {
                    /* rateDeviation passed is positive */
                    if(rateDeviationTemp >
                    StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMMasterRateDeviationMax_u16)
                    {
                    /*MR12 RULE 10.3 VIOLATION: The conversion from unsigned to signed is necessary in this case
                    Since the value returned should be of type StbM_RateDeviationType */
                    rateDeviationTemp =
                    StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMMasterRateDeviationMax_u16;
                    }
                }

                /* [SWS_StbM_00411] The StbM shall apply rate correction to a Time Base, if StbMAllowMasterRateCorrection
                equals TRUE and a valid rate correction value has been set by StbM_SetRateCorrection(). */

                /* [SWS_StbM_00396] :The actual applied resulting rate will be
                for Synchronized Time Bases: rateDeviation + 1 (= rrc as given in [SWS_StbM_00424])
                for Offset Time Bases: rateDeviation (= rorc - 1 as given in [SWS_StbM_00424])
                */

                /*  Enter Resource lock */
                SchM_Enter_StbM_Time_Source();

                /* Check if the timebase is a Synchronized or Pure local timebase*/
                if((StbM_Rb_IsSyncTimeBase(timeBaseId)) || (StbM_Rb_IsPureLocalTimeBase(timeBaseId)))
                {
                    /*MR12 RULE 10.4 VIOLATION: The conversion from unsigned to signed is necessary in this case
                    Since the value returned should be of type uint32 */
                    StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->RateCurrent_u32 =
                    rateDeviationTemp + STBM_MILLION;
                }
                else
                {
                    /*MR12 RULE 10.3 VIOLATION: The conversion from unsigned to signed is necessary in this case
                    Since the value returned should be of type uint32 */
                    StbM_Cfg_TimeCorrectionArray_ast[lIndex_u8].RateCorrectionInfoStruct_ptr->RateCurrent_u32 =
                    rateDeviationTemp;
                }

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

                [SWS_StbM_00442]
                For Synchronized Time Bases the Main Time Tuple shall be updated according to
                [SWS_StbM_00441] and [SWS_StbM_00342].
                In case of StbM_SetRateCorrection() the StbM shall calculate a temporary Time Tuple according to
                [SWS_StbM_00424] and replace the Main Time Tuple by this temporary Time Tuple

                [SWS_StbM_00443]
                For Offset Time Bases the Main Time Tuple shall be updated according to [SWS_StbM_00441], [SWS_StbM_00190]
                and [SWS_StbM_00304].
                In case of StbM_SetRateCorrection() the StbM shall calculate a temporary Time Tuple according to
                [SWS_StbM_00424] and replace the Main Time Tuple by this temporary Time Tuple */

                /* Load the previous VLT*/
                lTimeStampVLocalLast_st = StbM_VirtualLocalTimeArray_ast[lIndex_u8];

                /* Call the StbM_GetCurrentVirtualLocalTime_Internal to get the current Virtual local Time value along
                with counter value update*/
                lRet_Val_u8 =
                StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocalNow_st,STBM_COUNTER_UPDATE);

                /* Update the Virtual Local time array with the new Virtual Local time value */
                StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsHi    = lTimeStampVLocalNow_st.nanosecondsHi;
                StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsLo    = lTimeStampVLocalNow_st.nanosecondsLo;

                /*  Exit Resource lock */
                SchM_Exit_StbM_Time_Source();

                if(lRet_Val_u8 == E_OK)
                {
                    /*Check if the Virtual time value */
                    lVirtualLocalTimeDiff_st =
                    StbM_Rb_VirtualLocalTimeDifference(lTimeStampVLocalLast_st,lTimeStampVLocalNow_st);

                    /*Call the API to update the Global Time Tuple*/
                    StbM_Rb_GlobalTimeTupleUpdate(timeBaseId,lTimeStampVLocalNow_st,lVirtualLocalTimeDiff_st);
                }
                else
                {

                }
            }
            else
            {
            /* [SWS_StbM_00392] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_SetRateCorrection()
            shall report to DET the development error STBM_E_SERVICE_DISABLED, if StbMAllowMasterRateCorrection is set to
            FALSE for the corresponding Time Base, i.e., it is not allowed to call StbM_SetRateCorrection().*/
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETRATECORRECTION,STBM_E_SERVICE_DISABLED)
            }
        }
        else
        {
        /* [SWS_StbM_00391] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_SetRateCorrection()
        shall report to DET the development error STBM_E_PARAM, if called with a parameter timeBaseID, which
        - is not configured or
        - is within the reserved value range. */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETRATECORRECTION,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETRATECORRECTION,STBM_E_NOT_INITIALIZED)
    }

    return lRet_Val_u8;

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"


/* [SWS_StbM_00366] At the start of a Rate Correction measurement, the StbM shall store the Received Time Tuple that is
   passed in the scope of the function StbM_BusSetGlobalTime().
   The elements of the stored Time Tuple have the following denotation:
   TGStart - Global Time part of the Received Time Tuple
   TVStart - Virtual Local Time part of the Received Time Tuple */
/*********************************************************************************************************************
Service name      : StbM_StartRateCorrectionMeasurement
Syntax            : void StbM_StartRateCorrectionMeasurement(uint8 Index,
                                               StbM_TimeStampType * VLocalTimeStart,
                                               const StbM_TimeStampType * globalTimeStart)
Parameters (in)   : Index : index of timebase
                    VLocalTimeStart : Virtual local time now
                    globalTimeStart : Global time now
Parameters (inout): None
Parameters (out)  : None
Return value      : void
Description       : Local API to start the Rate correction measurement.
 **********************************************************************************************************************/
/* Local function for Rate Calculation  */

static void StbM_StartRateCorrectionMeasurement(uint8 Index,
        const StbM_VirtualLocalTimeType * VLocalTimeStart,
        const StbM_TimeStampType * GlobalTimeStart)
    {
    uint8 lgreaterTimeStamp_u8;
    uint8 lgreaterTimeStamp1_u8;
    uint16 lIter_u16;
    boolean lStbM_StartMeasurement_b;
    uint16 lLocation_u16;
    StbM_TimeCorrectionTimeSnapshot_tst * lRateCorrectionTimeMaintainenceArray;
    StbM_TimeStampType lMinOffsetRCCycle_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDiffTimeCurrentLast  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDiffTimeDeltaOffset  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType StbMRCLastCycleStartTime  = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lVLocalTimeStart_st = {STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalTimeStart  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lRCLastCycleStartTime  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};

    /* Initialize the local variables */
    lgreaterTimeStamp_u8   = STBM_ZERO;
    lgreaterTimeStamp1_u8  = STBM_ZERO;
    lStbM_StartMeasurement_b = FALSE;
    lLocation_u16 = STBM_ZERO;

    StbMRCLastCycleStartTime.nanosecondsHi   = StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->
                                                               StbMRateCorrectionLastCycleStartTime_st.nanosecondsHi;
    StbMRCLastCycleStartTime.nanosecondsLo   = StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->
                                                               StbMRateCorrectionLastCycleStartTime_st.nanosecondsLo;

    lMinOffsetRCCycle_st.secondsHi       = StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->
                                                                    StbMRateCorrectionMinimumOffset_st.secondsHi;
    lMinOffsetRCCycle_st.seconds         = StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->
                                                                    StbMRateCorrectionMinimumOffset_st.seconds;
    lMinOffsetRCCycle_st.nanoseconds     = StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->
                                                                    StbMRateCorrectionMinimumOffset_st.nanoseconds;

    /* Get the pointer to the relevant array where all the time values are stored for current timebase */
    lRateCorrectionTimeMaintainenceArray = StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->
                                                                    RateCorrectionTimeSnapshotArray_ptr;

    /* Check, if there are any ongoing measurements or not? */
    if(StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->RateCorrectionCycleCount_u16 > STBM_ZERO)
    {
        /* Yes, there are some measurements ongoing for this timebase, we need to check for the time differences */

        /* Calculate time difference between virtual local time now (when the BSGT API is called) and
           the last time the measurement started for current timebase */

        /* Copy the Virtual Local Time into a local variable*/
        lVLocalTimeStart_st.nanosecondsHi = VLocalTimeStart->nanosecondsHi;
        lVLocalTimeStart_st.nanosecondsLo = VLocalTimeStart->nanosecondsLo;

        StbM_Rb_ConvertVirtualLocalTimeToTimestampType(lVLocalTimeStart_st,&lLocalTimeStart);

        StbM_Rb_ConvertVirtualLocalTimeToTimestampType(StbMRCLastCycleStartTime,&lRCLastCycleStartTime);

        StbM_Time_DifferenceCalculation
                  (&lLocalTimeStart,&lRCLastCycleStartTime,&lDiffTimeCurrentLast,&lgreaterTimeStamp_u8);

        if(lgreaterTimeStamp_u8 == STBM_ONE)
        {
            /* [SWS_StbM_00368] The StbM shall perform as many simultaneous Rate Correction measurements as configured
               by parameter: StbMRateCorrectionsPerMeasurementDuration */
            /* [SWS_StbM_00367] Simultaneous Rate Correction measurements shall be started with a defined offset (ton)
               to yield Rate Corrections evenly distributed over the measurement duration.
               ton = n * (StbMRateCorrectionMeasurementDuration / StbMRateCorrectionsPerMeasurementDuration)
               (where "n" is the zero-based index of the current measurement) */

            /* Check if this difference is greater than the expected minimum offset as per the configuration for
               current timebase */
            StbM_Time_DifferenceCalculation(&lDiffTimeCurrentLast,&lMinOffsetRCCycle_st,
                                            &lDiffTimeDeltaOffset,&lgreaterTimeStamp1_u8);

            if(lgreaterTimeStamp1_u8 == STBM_ONE)
            {
                /* The time difference between Virtual local time now and the last time when the measurement started is
                   greater than the minimum offset, so the measurement can be started, store the global and local
                   timestamps and increment cycle count variable */

                /* To store the timestamp values in array loop thorugh the array and store the values at location where
                   StbM_ValidTimeStamp_b flag is FALSE */

                for(lIter_u16 = STBM_ZERO;
                    lIter_u16 < StbM_Cfg_TimeCorrectionArray_ast[Index].StbMRateCorrectionsPerMeasurementDuration_u16;
                    lIter_u16++)
                {
                    if(((lRateCorrectionTimeMaintainenceArray + lIter_u16)->StbM_ValidTimeStamp_b) == FALSE)
                    {
                        /* We just found out an empty space to store our time values */
                        lStbM_StartMeasurement_b = TRUE;
                        lLocation_u16 = lIter_u16;

                        /* We have already found an empty location, so break out of the search for an empty location */
                        break;
                    }
                    else
                    {
                        /* This index has a valid time stored, so move ahead, to next array location or index */
                    }
                }
            }
            else
            {
                /* The time difference between Virtual local time now and the last time when the measurement started is
                   less than the minimum offset, measurement cannot be started */

                /* Do Nothing */
            }
        }
    }
    else
    {
        /* There are no measurements ongoing for this timebase, so not need to check for any time differences, directly
           start the measurement */

        lStbM_StartMeasurement_b = TRUE;

        /* lLocation_u16 is already STBM_ZERO in initialization */
    }

    /* Do we have to start any measurement ? */
    if(lStbM_StartMeasurement_b)
    {
        /* Store the variables in the index as mentioned by lLocation_u16 parameter */

        /* As we are going to store some valid time value here, so mark the mentioned index as valid now */
        (lRateCorrectionTimeMaintainenceArray + lLocation_u16)->StbM_ValidTimeStamp_b = TRUE;

        /* Store Global time Values */
        (lRateCorrectionTimeMaintainenceArray + lLocation_u16) ->RateCorrectionGlobalStartTime_st.nanoseconds =
                                                                                         GlobalTimeStart -> nanoseconds;
        (lRateCorrectionTimeMaintainenceArray + lLocation_u16) ->RateCorrectionGlobalStartTime_st.seconds     =
                                                                                         GlobalTimeStart -> seconds;
        (lRateCorrectionTimeMaintainenceArray + lLocation_u16) ->RateCorrectionGlobalStartTime_st.secondsHi   =
                                                                                         GlobalTimeStart -> secondsHi;

        /* Store Local time Values */
        (lRateCorrectionTimeMaintainenceArray + lLocation_u16) ->RateCorrectionVirtualLocalStartTime_st.nanosecondsHi =
                                                                                       VLocalTimeStart -> nanosecondsHi;
        (lRateCorrectionTimeMaintainenceArray + lLocation_u16) ->RateCorrectionVirtualLocalStartTime_st.nanosecondsLo =
                                                                                       VLocalTimeStart -> nanosecondsLo;

        /* Increment cycle count */
        StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->RateCorrectionCycleCount_u16++;

        /* store last time when the Rate correction cycle began */
        StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->
                               StbMRateCorrectionLastCycleStartTime_st.nanosecondsHi = VLocalTimeStart -> nanosecondsHi;
        StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->
                               StbMRateCorrectionLastCycleStartTime_st.nanosecondsLo = VLocalTimeStart -> nanosecondsLo;
    }

}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/* [SWS_StbM_00364] At the end of the Rate Correction measurement, the StbM shall store the Received Time Tuple that is
   passed in the scope of the function StbM_BusSetGlobalTime(). The elements of the stored Time Tuple have the
   following denotation:
   TGStop - Global Time part of the Received Time Tuple
   TVStop - Virtual Local Time part of the Received Time Tuple
   This is equivalent to an atomic Time Tuple assigment: [TGStop;TVStop] = [TGRx;TVRx]*/
/* [SWS_StbM_00361] At the end of a Rate Correction measurement, the StbM shall calculate the resulting correction rate
   (rrc) for Synchronized Time Bases as shown:
   rrc = (TGStop - TGStart) / (TVStop - TVStart) */
/* [SWS_StbM_00360] At the end of a Rate Correction measurement, the StbM shall calculate the resulting correction
   rate (rorc) for Offset Time Bases as shown:

   rorc = (TGStop - TGStart) / (TVStop - TVStart) + 1 */
/*********************************************************************************************************************
Service name      : StbM_EndRateCorrectionMeasurement
Syntax            : void StbM_EndRateCorrectionMeasurement(uint8 Index,
                                                  StbM_TimeStampType * VLocalTimeStart,
                                                  StbM_TimeStampType * globalTimeStart,
                                                  const StbM_TimeStampType * globalTimeEnd,
                                                  StbM_TimeStampType * VLocalTimeEnd)
Parameters (in)   : Index : index of timebase
                    VLocalTimeStart : Virtual Local Start time of the measurement
                    globalTimeStart : Global Start time of the measurement
                    globalTimeEnd   : Global end time of the measurement
                    VLocalTimeEnd   : Virtual Local end time of the measurement
Parameters (inout): None
Parameters (out)  : None
Return value      : void
Description       : Local API to end the Rate correction measurement.
 **********************************************************************************************************************/
/* Local function for Rate Calculation  */

static void StbM_EndRateCorrectionMeasurement(uint8 Index,
        const StbM_VirtualLocalTimeType * VLocalTimeStart,
        const StbM_TimeStampType * globalTimeStart,
        const StbM_TimeStampType * globalTimeEnd,
        const StbM_VirtualLocalTimeType * VLocalTimeEnd)
{
    /* Rate will be calculated in this API */
    StbM_TimeStampType lDiffStartStop_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalTimeStart_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalTimeEnd_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lVLocalTimeStart_st = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lVLocalTimeEnd_st = {STBM_ZERO,STBM_ZERO};
    uint64 lDeltaGlobal_u64;
    uint64 lDeltaVLocal_u64;
    uint64 lRateTemp_u64;
    uint16 lTimeBaseId_u16;
    uint8 lgreaterTimeStamp_u8;

    lDeltaGlobal_u64     = STBM_ZERO;
    lDeltaVLocal_u64     = STBM_ZERO;
    lTimeBaseId_u16      = STBM_ZERO;
    lgreaterTimeStamp_u8 = STBM_ZERO;

    /* Copy the configured timebase ID into a local array */
    lTimeBaseId_u16 =
            StbM_Cfg_SynchronizedTimeBaseArray_ast[Index].SynchronizedTimeBaseIdentifier_u16;

    /* Get the difference between the 2 global time stamps */
    StbM_Time_DifferenceCalculation(globalTimeEnd,globalTimeStart,&lDiffStartStop_st,&lgreaterTimeStamp_u8);

    /* Convert this difference timestamp to all nanoseconds value */
    lDeltaGlobal_u64 = StbM_ConvertTimestamp2Nanoseconds(lDiffStartStop_st);

    /* Copy the Virtual Local Time into a local variable*/
    lVLocalTimeStart_st.nanosecondsHi = VLocalTimeStart->nanosecondsHi;
    lVLocalTimeStart_st.nanosecondsLo = VLocalTimeStart->nanosecondsLo;

    /* Copy the Virtual Local Time into a local variable*/
    lVLocalTimeEnd_st.nanosecondsHi = VLocalTimeEnd->nanosecondsHi;
    lVLocalTimeEnd_st.nanosecondsLo = VLocalTimeEnd->nanosecondsLo;

    StbM_Rb_ConvertVirtualLocalTimeToTimestampType(lVLocalTimeStart_st,&lLocalTimeStart_st);

    StbM_Rb_ConvertVirtualLocalTimeToTimestampType(lVLocalTimeEnd_st,&lLocalTimeEnd_st);

    /* Get the difference between the 2 virtual local time stamps */
    StbM_Time_DifferenceCalculation(&lLocalTimeEnd_st,&lLocalTimeStart_st,&lDiffStartStop_st,&lgreaterTimeStamp_u8);

    /* Convert this difference timestamp to all nanoseconds value */
    lDeltaVLocal_u64 = StbM_ConvertTimestamp2Nanoseconds(lDiffStartStop_st);

    /* Check if the TimeBase is Synchronized TimeBase */
    if(StbM_Rb_IsSyncTimeBase(lTimeBaseId_u16))
    {
        /* Calculate the rate - stored in format ppm */
        lRateTemp_u64 = (lDeltaGlobal_u64 * STBM_MILLION)/lDeltaVLocal_u64;
    }
    /*Offset timebases*/
    else
    {
        /* Calculate the rate - stored in format ppm
         Note: +1 is added for formal reasons in the formula for rorc.
         This is to have in [SWS_StbM_00397] and [SWS_StbM_00412] aligned value ranges for rate correction rorc and rrc
         and the corresponding rate deviation values */
        lRateTemp_u64 = ((lDeltaGlobal_u64 * STBM_MILLION)/lDeltaVLocal_u64) + STBM_MILLION;
    }

    /* Check if the rate exceeds the limit or not */
    if(lRateTemp_u64 > STBM_RATE_MAX_PPM)
    {
        lRateTemp_u64 = STBM_RATE_MAX_PPM;
    }
    else if(lRateTemp_u64 < STBM_RATE_MIN_PPM)
    {
        lRateTemp_u64 = STBM_RATE_MIN_PPM;
    }
    else
    {
        /* Nothing to do, rate is in correct value range (968000 - 1032000 ppm)*/
    }

    /* Store the rate in the array - ppm format */
    StbM_Cfg_TimeCorrectionArray_ast[Index].RateCorrectionInfoStruct_ptr->RateCurrent_u32 = (uint32)lRateTemp_u64;

    /* Set the event */
    StbM_SetEvent_Bit((StbM_NotificationEvents_au32+Index),STBM_EV_RATECORRECTION);
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/* [SWS_StbM_00374] For Rate Correction measurements, the StbM shall evaluate state changes of the TIMEOUT flag.
   The StbM shall discard measurements when the state changes during a measurement. */
/* [SWS_StbM_00375] For Rate Correction measurements, the StbM shall evaluate state changes of the SYNC_TO_GATEWAY flag
   during measurements. The StbM shall discard the measurement if the flag state changes. */
/* [SWS_StbM_00376] For Rate Correction measurements, the StbM shall evaluate state changes of the TIMELEAP_FUTURE and
   TIMELEAP_PAST flags during measurements. The StbM shall discard the measurement if the flag state changes. */
/*********************************************************************************************************************
Service name      : StbM_ResetOngoingRateCorrections
Syntax            : void StbM_ResetOngoingRateCorrections(uint8 index)
Parameters (in)   : Index : index of timebase
Parameters (inout): None
Parameters (out)  : None
Return value      : Std_ReturnType
Description       : API to discard all the ongoing measurements.
 **********************************************************************************************************************/
/* Local function to reset all the ongoing rate correction measurements current timebase */
void StbM_ResetOngoingRateCorrections(uint8 index)
{
    uint16 lCount;

    /* Check if the time correction container is configured for current timebase */
    if(StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten != STBM_TIME_CORRECTION_NOT_CONFIGURED)
    {
       /* Time correction container has been configured for current timeBase */

        /* Reset the variable that stores the count of simultaneos ongoing rate corrections for current timebase */
        StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->RateCorrectionCycleCount_u16 = STBM_ZERO;

        /* Reset Valid flag for snapshot array for mentioned timebase */
        for(lCount = STBM_ZERO;
            lCount < StbM_Cfg_TimeCorrectionArray_ast[index].StbMRateCorrectionsPerMeasurementDuration_u16;
            lCount++)
        {
            ((StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
                                          RateCorrectionTimeSnapshotArray_ptr) + lCount)->StbM_ValidTimeStamp_b = FALSE;
        }
    }
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"


/*********************************************************************************************************************
Service name      : StbM_GetRateCorrectedTime
Syntax            : StbM_TimeStampType StbM_GetRateCorrectedTime(uint8 index,
                                                    StbM_SynchronizedTimeBaseType timeBaseId,
                                                    const StbM_TimeStampType * VLocalTimeNow)
Parameters (in)   : Index : index of timebase
                    timeBaseId : Timebase ID
                    VLocalTimeNow : Current value of virtual local time
Parameters (inout): None
Parameters (out)  : None
Return value      : StbM_TimeStampType
Description       : Local API to get the Rate correction time.
 **********************************************************************************************************************/
/* Local function for Rate Calculation  */

StbM_TimeStampType StbM_GetRateCorrectedTime(uint8 index,
        StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_VirtualLocalTimeType * VLocalTimeNow)
{
    StbM_VirtualLocalTimeType lVLocalTimeLastSync_st = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lVLocalTimeNow_st   = {STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalTimeLastSync_st       = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lVLocalTimeLastSyncNow_st   = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lAdaptionInterval_st      = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDiffAdapIntVLocalTime_st   = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lRateCorrectedVLocalTime_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lReturnLocalTime            = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalTimeNow_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalTimeLast_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType * lLocalTimeLastSyncPtr;
    uint64 lDeltaVLocal_u64;
    uint32 lRate;
    uint8 lgreaterTimeStamp_u8;

    /* Local Variable Initialization */
    lgreaterTimeStamp_u8  = STBM_ZERO;
    lDeltaVLocal_u64      = STBM_ZERO;
    lRate                 = StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->RateCurrent_u32;
    lLocalTimeLastSyncPtr = &lLocalTimeLastSync_st;

    /*
     [SWS_StbM_00424]
      The StbM shall calculate the (rate corrected) time (TL) of its local instance of the Time Base as:
      TL = TGSync + (TV - TVSync) * r
      With:
      TV = Current value of the Virtual Local Time
      TVSync = Virtual Local Time part of the Main Time Tuple
      TGSync = Global Time part of the Main Time Tuple
      r = Rate for correcting the Time Base with r = rrc for Synchronized Time Bases r = rorc -1 for Offset Time Bases

      If StbMAllowMasterRateCorrection (ECUC_StbM_00043 : ) equals FALSE r shall be set to
       1 for Synchronized Time Bases
       0 for Offset Time Bases
       (i.e., no rate correction is applied).

       Time correction Formulas
       1. Jump Correction/Rate Correction
          Synchronized Timebase : TL = TG + (TV - TVSync) * rrc
          Offset Timebase       : TL = TG + (TV - TVSync) * (rorc -1)
       2. Rate Adaption
          TL = TLSync + (TV - TVSync) * (rrc + roc)
    */

    /* [SWS_StbM_00362] The StbM shall use the same value for rrc and rorc until a new value has been calculated. */

    /* [SWS_StbM_00412] The StbM shall use rrc = 1 for a Synchronized Time Base, if a valid correction rate (rrc) has
       not yet been calculated or is not being calculated (refer [SWS_StbM_00377]) but shall be applied
       (refer [SWS_StbM_00355], [SWS_StbM_00354]).
       The StbM shall use rorc = 1 for an Offset Time Base, if a valid correction rate (rorc) has not yet been
       calculated or is not being calculated (refer [SWS_StbM_00377]) but shall be applied. */

    if(lRate == STBM_ZERO)
    {
        /* Till now no Rate has been calculated, so default rate will be set to 1 (or 1000000 ppm) (as per RFC 77376) */
        lRate = STBM_MILLION;
    }

    /*
      [SWS_StbM_00440]
      For Synchronized Time Bases and if rate correction is enabled (see [SWS_StbM_00377]) and if the
      absolute value of the time offset between the Received Time and the Synclocal Time (abs(TGRx - TLSync))
      is equal or greater than StbMOffsetCorrectionJumpThreshold (ECUC_StbM_00056 : ), the StbM shall use the
      factor rrc for the rate correction term r:
      r = rrc
      Otherwise r shall be set to 1, unless r shall be set accordingly to [SWS_StbM_00356] or [SWS_StbM_00353].

      [SWS_StbM_00441]
      For Offset Time Bases and if rate correction is enabled (see [SWS_StbM_00377]) and if the absolute value
      of the time offset between the Received Time and the Synclocal Time (abs(TGRx - TLSync)) is equal or greater
      than StbMOffsetCorrectionJumpThreshold (ECUC_StbM_00056 : ),
      the StbM shall use the factor rorc for the rate correction term r:
      r = rorc - 1
      Otherwise r shall be set to 0, unless r shall be set accordlingly to [SWS_StbM_00356] or [SWS_StbM_00355]


      [SWS_StbM_00424]
      The StbM shall calculate the (rate corrected) time (TL) of its local instance of the Time Base as:
      TL = TGSync + (TV - TVSync) * r
      With:
      TV = Current value of the Virtual Local Time
      TVSync = Virtual Local Time part of the Main Time Tuple
      TGSync = Global Time part of the Main Time Tuple
      r = Rate for correcting the Time Base with r = rrc for Synchronized Time Bases r = rorc -1 for Offset Time Bases

      If StbMAllowMasterRateCorrection (ECUC_StbM_00043 : ) equals FALSE r shall be set to
      1 for Synchronized Time Bases
      0 for Offset Time Bases
      (i.e., no rate correction is applied)

    If the TimeBase is a Synchronized timebase, then r = r = rrc, lRate = lRate
     */

    /* Check if the TimeBase is Offest TimeBase */
    if(StbM_Rb_IsOffsetTimeBase(timeBaseId))
    {
         /* r = rorc -1 */
         lRate = lRate - STBM_MILLION;
     }
     else
     {

     }


     /* If StbMAllowMasterRateCorrection (ECUC_StbM_00043 : ) equals FALSE r shall be set to
        1 for Synchronized Time Bases
        0 for Offset Time Bases
        (i.e., no rate correction is applied)*/

     if(StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMAllowMasterRateCorrection_b == FALSE)
     {
         /*
         If the TimeBase is a Synchronized timebase, then lRate = lRate
         }*/

         /* Check if the TimeBase is Offest TimeBase */
         if(StbM_Rb_IsOffsetTimeBase(timeBaseId))
         {
             lRate = lRate - STBM_MILLION;
         }
         else
         {

         }

     }
     else
     {

     }

    /* What was the virtual Local time when time was synchronized last in the scope of bus set global time (TVsync)*/
    lVLocalTimeLastSync_st.nanosecondsHi = StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
                                                                        StbMVirtualLocalTimeLatest_st.nanosecondsHi;
    lVLocalTimeLastSync_st.nanosecondsLo     = StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
                                                                        StbMVirtualLocalTimeLatest_st.nanosecondsLo;

    /* What is the current virtual local time now -> timeStamp */
    /* Calculate the delta between virtual local when last time the timebase was synchronized and
       virtual local time now */

    /* Copy the Virtual Local Time into a local variable*/
    lVLocalTimeNow_st.nanosecondsHi = VLocalTimeNow->nanosecondsHi;
    lVLocalTimeNow_st.nanosecondsLo = VLocalTimeNow->nanosecondsLo;

    StbM_Rb_ConvertVirtualLocalTimeToTimestampType(lVLocalTimeNow_st,&lLocalTimeNow_st);

    StbM_Rb_ConvertVirtualLocalTimeToTimestampType(lVLocalTimeLastSync_st,&lLocalTimeLast_st);

    StbM_Time_DifferenceCalculation(&lLocalTimeNow_st,&lLocalTimeLast_st,
                                    &lVLocalTimeLastSyncNow_st,&lgreaterTimeStamp_u8);

    if((StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr != NULL_PTR)&&
       (StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten == STBM_RATE_ADAPTION))
    {

        /* Check if this difference is greater than configured Adaption interval */
        lAdaptionInterval_st.nanoseconds = StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                                                                    StbMOffsetCorrectionAdaptionInterval_st.nanoseconds;
        lAdaptionInterval_st.seconds = StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                                                                    StbMOffsetCorrectionAdaptionInterval_st.seconds;
        lAdaptionInterval_st.secondsHi = StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                                                                    StbMOffsetCorrectionAdaptionInterval_st.secondsHi;

        StbM_Time_DifferenceCalculation(&lVLocalTimeLastSyncNow_st,&lAdaptionInterval_st,
                                        &lDiffAdapIntVLocalTime_st,&lgreaterTimeStamp_u8);

        if(lgreaterTimeStamp_u8 == STBM_ZERO)
        {
            /* Correct the time by Rate Adaption, additional rate will be applied */
            if(StbM_Cfg_TimeCorrectionArray_ast[index].
                                                     OffsetCorrectionInfoStruct_ptr->AdditionalRateType_b == STBM_ACCELERATE_RATE)
            {
                lRate = lRate + StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                                                                                                     AdditionalRate_u32;
            }
            else /* AdditionalRateType_b == STBM_DEACCELERATE_RATE */
            {

                if(lRate > StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->AdditionalRate_u32)
                {
                    lRate = lRate - StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                                                                                                     AdditionalRate_u32;
                }
                else
                {
                    /* This is done because if the additional Rate is greater than the lRate value ie Calculated Rate
                       (1million when no rate has been calculated yet), then in that case difference will become
                       negative and will become +ve max */
                    lRate = STBM_RATE_MIN_PPM;
                }
            }

            /* Check if the new rate exceeds the limit or not */
            if(lRate > STBM_RATE_MAX_PPM)
            {
                lRate = STBM_RATE_MAX_PPM;
            }
            else if(lRate < STBM_RATE_MIN_PPM)
            {
                lRate = STBM_RATE_MIN_PPM;
            }
            else
            {
                /* Nothing to do, rate is in correct value range */
            }
        }
        else /* if(lgreaterTimeStamp_u8 == STBM_ONE) */
        {
            /* [SWS_StbM_00353] If the absolute time offset between the Global Time Base and the local instance of the
               Time Base (abs(TG - TL)) is smaller than StbMOffsetCorrectionJumpThreshold, the StbM shall calculate the
               corrected time (TL) of its local instance of the Time Base after the period of
               StbMOffsetCorrectionAdaptionInterval as specified in [SWS_StbM_00355]. */

            /* We have alreday cross the Rate Adaption interval, do it by traditional Rate correction way, so no
               Additional rate will be applied, unless next call to StbM_BusSetGlobalTiem API changes this to
               Rate Adaption */
            StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten = STBM_JUMP_CORRECTION;
        }
    }

    /* Calculation :
          lDeltaVLocal_u64 maximum value is Uint64 max (18,446,744,073,709,551,615)
          lRate maximum value is 1032000

          Uint64 max = ( x * 1032000 )/ 1000000
          x = Uint64 max/1.032 = 18446744073709551615/1.032
            = 17874752009408

          Maximum value lDeltaVLocal_u8 can hold is 17874752009408 so that next instruction
          can be executed without an overflow
          => 17874752009408 ns
          => 17874 sec + 752009408 ns
    */

    if((lVLocalTimeLastSyncNow_st.secondsHi == STBM_ZERO)&&
            ((lVLocalTimeLastSyncNow_st.seconds < 17874u)||
                ((lVLocalTimeLastSyncNow_st.seconds == 17874u)&&
                 (lVLocalTimeLastSyncNow_st.nanoseconds <= 752009408u))))
    {
        /* Convert difference time to 1 unit (nanosecond) so that we can multiply it with rate value */
        lDeltaVLocal_u64 = StbM_ConvertTimestamp2Nanoseconds(lVLocalTimeLastSyncNow_st);

        /* Rate corrected delta is Difference * rate */
        lDeltaVLocal_u64 = (lDeltaVLocal_u64 * lRate)/STBM_MILLION;

        /* Convert this to Timestamp format */
        /*MR12 RULE 10.3 VIOLATION: The conversion to lower data type is necessary in this case , since the seconds is extracted */
        lRateCorrectedVLocalTime_st.seconds     = lDeltaVLocal_u64/STBM_NS_PER_SEC;
        /*MR12 RULE 10.3 VIOLATION: The conversion to lower data type is necessary in this case , since nanoseconds value is extracted */
        lRateCorrectedVLocalTime_st.nanoseconds = lDeltaVLocal_u64 -
                                                                (lRateCorrectedVLocalTime_st.seconds * STBM_NS_PER_SEC);


        if((StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten == STBM_JUMP_CORRECTION)||
           (StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten == STBM_RATE_CORRECTION))
        {
            /* [SWS_StbM_00355]
               The StbM shall calculate the current value of a Time Base based on the Main Tuple and the current rate
               correction term according to:
               TL = TGSync + (TV - TVSync) * r
               With:
               TL = Current value of the Time Base
               TV = Current value of the Virtual Local Time
               TVSync = Virtual Local Time part of the Main Time Tuple
               TGSync = Global Time part of the Main Time Tuple
               r = Current rate for correcting the local instance of the Time Base */

            /*[SWS_StbM_00353]
              If an additional rate has been applied (Offset Correction By Rate Adaption according to [SWS_StbM_00356]),
              the StbM shall after the period of StbMOffsetCorrectionAdaptionInterval (i.e., (TV - TVSync)
              (see [SWS_StbM_00355]) is larger or equal than StbMOffsetCorrectionAdaptionInterval) insert the following
              two steps if it needs to calculate the current value of a Time Base as defined by [SWS_StbM_00355]:
              1. It shall first calculate a temporary Time Tuple [TLTemp;TVTemp] using the formula in [SWS_StbM_00355]
              with TV = TVTemp = TVSync + StbMOffsetCorrectionAdaptionInterval
              r = rrc + roc (for Synchronized Time Bases)
              r = (rorc - 1) + roc (for Offset Time Bases)
              TLTemp shall be set to the resulting value TL
             2. Afterwards the Main Time Tuple [TGSync;TVSync] shall be set by an atomic operation to the values of
             the temporary Time Tuple [TLTemp;TVTemp]. */

            /* Add this Rate corrected Virtual Local Time to Global Time */
            lReturnLocalTime = StbM_Overflow_Check(index,lRateCorrectedVLocalTime_st,
                    STBM_GLOBAL_TIME_VALUE_RATE_CORRECTION);
        }
        else /* (StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten == STBM_RATE_ADAPTION) */
        {
            /* [SWS_StbM_00354] If the absolute time offset between Global Time Base and local instance of the Time Base
               (abs(TG - TLSync)) is smaller than StbMOffsetCorrectionJumpThreshold, the StbM shall calculate the
               corrected time (TL) of its local instance of the Time Base within the period of
               StbMOffsetCorrectionAdaptionInterval as shown:
               TL = TLSync + (TV - TVSync) * (rrc + roc)
               (Where:
               - TLSync = Corrected current value of the local instance of the Time Base
               - TV = Current value of the Virtual Local Time of the Time Base
               - TVSync = Value of the Virtual Local Time as defined in [SWS_StbM_00359]
               - rrc = Actual rate for correcting the local instance of the Time Base
               - roc = Rate for time offset elimination via Rate Adaption
               This correction shall be done whenever the time is read in the scope of these functions:
               - StbM_GetCurrentTime()
               - StbM_GetCurrentTimeExtended()
               This correction shall also be done when the StbM needs to determine the time of the local instance of
               the Time Base.*/

            /* What was the Local time when time was synchronized last in the scope of bus set global time (TLsync)*/
            lLocalTimeLastSync_st.nanoseconds = StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
                                                                                     StbMLocalTimeLatest_st.nanoseconds;
            lLocalTimeLastSync_st.seconds     = StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
                                                                                     StbMLocalTimeLatest_st.seconds;
            lLocalTimeLastSync_st.secondsHi   = StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
                                                                                     StbMLocalTimeLatest_st.secondsHi;

            /* lLocalTimeLastSyncPtr = &lLocalTimeLastSync_st (Already done in initialization)*/
            /* Add this Rate corrected Virtual Local Time to TLsync */
            /* Already existing API is used to add 2 timestamp values */
            lReturnLocalTime = StbM_Offset_Overflow_Check(lRateCorrectedVLocalTime_st, lLocalTimeLastSyncPtr);
        }
    }
    else /* lVLocalTimeLastSyncNow_st is greater than 0,17874,752009408 ns */
    {
        /* Return the value without applying the rate correction */
        lReturnLocalTime = StbM_Overflow_Check(index,lVLocalTimeLastSyncNow_st,
                                                                          STBM_GLOBAL_TIME_VALUE_RATE_CORRECTION);
    }

    /* Timeout check has to be done if the state of the StbM is SLAVE / GATEWAY  */
    if(StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMState_e != STBM_MASTER)
    {
        /*  Call the local function to Check whether the timeout has occurred  */
        StbM_GetCurrentTime_Timeout_Check(index,&lVLocalTimeNow_st);
    }
    else
    {
        /* Do nothing - QAC */
    }

    /* Update the Timeout bit status in lReturnLocalTime */
    lReturnLocalTime.timeBaseStatus  = StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus;

    return lReturnLocalTime;
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_CheckOffsetCorrection
Syntax            : void StbM_CheckOffsetCorrection(uint8 index,
                                                    StbM_SynchronizedTimeBaseType timeBaseId,
                                                    const StbM_TimeStampType * timeStampPtr)
Parameters (in)   : Index : index of timebase
                    timeBaseId : passed timebase
                    VLocalTimeNow : Current value of virtual local time
Parameters (inout): None
Parameters (out)  : None
Return value      : None
Description       : Local API to check the offset correction feature in Time correction.
 **********************************************************************************************************************/
/* Local function for Rate Calculation  */

static void StbM_CheckOffsetCorrection(uint8 index,
                                               StbM_SynchronizedTimeBaseType timeBaseId,
                                               const StbM_TimeStampType * timeStampPtr)
{

    StbM_UserDataType lUserDataPtrTC_st         = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lTimeStampPtrLocal_st     = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDiffGlobalLocalTime      = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lJumpThresholdConfigured  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDiffDeltaMeasDuration    = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    uint64 lDeltaGlobal_u64;

    uint8 lgreaterTimeStamp1_u8;
    uint8 lgreaterTimeStamp2_u8;
    lgreaterTimeStamp1_u8 = STBM_ZERO;
    lDeltaGlobal_u64 = STBM_ZERO;

    /*  If at least one timestamp for the Time Base has been successfully received before. */
    if( StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT))
    {

        /* Check if the Adaption Interval configured is greater than 0 */
        if(StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                StbMOffsetCorrectionAdaptionIntervalTotalNs_u64 > STBM_ZERO)
        {
            /* [SWS_StbM_00359]
           The StbM shall calculate the Global Time offset (i.e., difference) between the Received Time and
           the Synclocal Time upon each, except the very first, valid invocation of  StbM_BusSetGlobalTime().
           The elements of the Time Tuples used for calculating the Global Time offset have the following denotation:
           -   TLSync = Global Time part of the Synclocal Time Tuple
           -   TGRx = Global Time part of the Received Time Tuple */

            /* Take Local Time snapshot - Time with Rate correction applied. (TLsync) */
            (void)StbM_GetCurrentTime(timeBaseId,&lTimeStampPtrLocal_st,&lUserDataPtrTC_st);

            /* Get the absolute delta between the passed global time and local time now (TGRx - TLsync)*/
            StbM_Time_DifferenceCalculation(timeStampPtr,&lTimeStampPtrLocal_st,
                    &lDiffGlobalLocalTime,&lgreaterTimeStamp1_u8);

            /*[SWS_StbM_00400] If StbMOffsetCorrectionJumpThreshold is set to 0, Offset Correction shall be performed by
          Jump Correction only.*/
            /* Check if this delta is greater than the configured offset correction Jump threshold */
            lJumpThresholdConfigured.nanoseconds =
            StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
            StbMOffsetCorrectionJumpThreshold_st.nanoseconds;
            lJumpThresholdConfigured.seconds     =
            StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
            StbMOffsetCorrectionJumpThreshold_st.seconds;
            lJumpThresholdConfigured.secondsHi   =
            StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
            StbMOffsetCorrectionJumpThreshold_st.secondsHi;

            StbM_Time_DifferenceCalculation(&lDiffGlobalLocalTime,&lJumpThresholdConfigured,
                    &lDiffDeltaMeasDuration,&lgreaterTimeStamp2_u8);

            if(lgreaterTimeStamp2_u8 == STBM_ONE)
            {
                /* [SWS_StbM_00355]
                   The StbM shall calculate the current value of a Time Base based on the Main Tuple and
                   the current rate correction term according to:
                   TL = TGSync + (TV - TVSync) * r
                   With:
                   TL = Current value of the Time Base
                   TV = Current value of the Virtual Local Time
                   TVSync = Virtual Local Time part of the Main Time Tuple
                   TGSync = Global Time part of the Main Time Tuple
                   r = Current rate for correcting the local instance of the Time Base */

                /* Absolute time difference between global time and local time is greater than
               Jump Threshold set in configuration */
                StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten = STBM_JUMP_CORRECTION;
            }
            else
            {
                /* [SWS_StbM_00356]
                   If rate correction is enabled (see [SWS_StbM_00377]) and if the absolute value of the time offset
                   between the Received Time and the Synclocal Time (abs(TGRx - TLSync)) is smaller than
                   StbMOffsetCorrectionJumpThreshold (ECUC_StbM_00056 : ),
                   the StbM shall correct the time offset by temporarily applying an additional rate (roc) to r:
                   r = rrc + roc (for Synchronized Time Bases)
                   r = (rorc - 1) + roc (for Offset Time Bases)
                   This rate correction term shall be applied for the duration defined by parameter
                   StbMOffsetCorrectionAdaptionInterval (ECUC_StbM_00057 : ),
                   starting when obtaining the Received Time Tuple (i.e., it shall be applied as long as (TV - TVSync)
                   (see [SWS_StbM_00355]) is smaller than StbMOffsetCorrectionAdaptionInterval).
                   roc shall be calculated as shown:
                   roc = (TGRx - TLSync) / (TCorrInt)
                   With:
                   TCorrInt = StbMOffsetCorrectionAdaptionInterval
                   TLSync = Global Time part of the Synclocal Time Tuple
                   TGRx = Global Time part of the Received Time Tuple*/

                /* Absolute time difference between global time and local time is less than Jump Threshold
               set in configuration */
                StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten = STBM_RATE_ADAPTION;

                /* Convert the delta between global time and local time into 1 unit format (all nanoseconds) */
                lDeltaGlobal_u64 = StbM_ConvertTimestamp2Nanoseconds(lDiffGlobalLocalTime);

                /* Calculate the additional Rate (Roc) that will be applied on top of already calculated Rrc over
               the period as defined in configuration as offset Adaption period */
                /*MR12 RULE 10.3 VIOLATION: The conversion to lower data type is necessary in this case , since additional rate is calculated */
                StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->AdditionalRate_u32 =
                        (lDeltaGlobal_u64 * STBM_MILLION)/
                        (StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                                StbMOffsetCorrectionAdaptionIntervalTotalNs_u64);

                /* Check if we need to make additional rate positive or negative */
                if(lgreaterTimeStamp1_u8 == STBM_ONE)
                {
                    /* The Global time passed is greater than the rate corrected time calculated in the system,
                   So, we need to speed up. */
                    StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                    AdditionalRateType_b = STBM_ACCELERATE_RATE;
                }
                else /* lgreaterTimeStamp1_u8 == STBM_ZERO */
                {
                    /* The Global time passed is less than the rate corrected time calculated in the system,
                   So, we need to slow down. */
                    StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr->
                    AdditionalRateType_b = STBM_DEACCELERATE_RATE;
                }
            }
        }
        else
        {
            /* Adaption Interval is configured as 0, so Offset correction will always happen via Jump Corection,
           which is the case by default. */
        }
    }
    else
    {
        /* Calculation is not done since Global time is not updated even once i.e  it is the first call of StbM_BusSetGlobalTime()*/
    }

}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"



#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_TimeCorrectionMeasurement
Syntax            : void StbM_TimeCorrectionMeasurement(uint8 index,
                                                   StbM_SynchronizedTimeBaseType timeBaseId,
                                                   StbM_TimeStampType timeStamp_Global,
                                                   StbM_TimeStampType timeStamp_CurrentGlobal,
                                                   StbM_VirtualLocalTimeType timeStamp_VLocal
                                                   )
Parameters (in)   : Index                 : index of timebase
                    timeBaseId            : Timebase ID
                    timeStamp_Global      : Value of the local instance of the Time Base before the new value
                                            of the Global Time is applied
                    timeStamp_CurrentGlobal : The global time revieved via the bus.
                    timeStamp_VLocal        : Value of the Virtual Local Time before the new value
                                            of the Global Time is applied
Parameters (inout): None
Parameters (out)  : None
Return value      : None
Description       : Local API where Time correction measurement will be taken care.
 **********************************************************************************************************************/
/* Local function for Rate Calculation  */
/* HIS METRIC LEVEL VIOLATION IN StbM_TimeCorrectionMeasurement: This is highly optimized and proven code. */
void StbM_TimeCorrectionMeasurement(uint8 index,
        StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampType timeStamp_Global,
        StbM_TimeStampType timeStamp_CurrentGlobal,
        StbM_VirtualLocalTimeType timeStamp_VLocal
)
{
    StbM_VirtualLocalTimeType lVLocalStartCurCycle_st = {STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalTimeNow_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lVLocalStartTempRC_st   = {STBM_MAX_SEC,STBM_MAX_SEC};
    StbM_TimeStampType lGlobalStartTempRC_st   = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDiffStartCurrent_st    = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDiffDeltaMeasDuration  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lMeasDuration           = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lStartCurCycle_st       = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lLocalStartTempRC_st       = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeCorrectionTimeSnapshot_tst * lRateCorrectionTimeMaintainenceArray;
    boolean lStbM_RateCalculated_b;
    uint16 lRCCycleCountTemp_u16;
    uint16 lRCCycleValidCount_u16;
    uint8 lgreaterTimeStamp_u8;
    uint16 lRateCorrectionCycleCountNow;

    lStbM_RateCalculated_b = FALSE;
    lgreaterTimeStamp_u8 = STBM_ZERO;
    lRCCycleValidCount_u16 = STBM_ZERO;
    lRateCorrectionCycleCountNow = STBM_ZERO;

    /* [SWS_StbM_00377] The StbM shall not perform Rate Correction when the measurement duration
       StbMRateCorrectionMeasurementDuration is set to zero. */
    /* [SWS_StbM_00372] The StbM shall perform Rate Correction measurements to determine the rate deviation of each
       configured Time Base. */
    /* [SWS_StbM_00371] The StbM shall perform Rate Correction measurements continuously. The end of a measurement
       marks the start of the next measurement. The start and end of measurements is always triggered by and
       aligned to the reception of time values for Synchronized or Offset Time Bases. */

    /* Check if the time correction container is configured for current timebase */
    if(StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten != STBM_TIME_CORRECTION_NOT_CONFIGURED)
    {
        /* Time correction container is configured for current timebase */

        /* Check if the current timebase is a synchronized timebase, and also if the offset correction is configured in
           configuration */
        if((StbM_Rb_IsSyncTimeBase(timeBaseId)) &&
           (StbM_Cfg_TimeCorrectionArray_ast[index].OffsetCorrectionInfoStruct_ptr != NULL_PTR))
        {
            /* Offset Correction */
            StbM_CheckOffsetCorrection(index, timeBaseId,&timeStamp_CurrentGlobal);
        }

        /* Store the value of Virtual Local Time (TVsync) */
        StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
        StbMVirtualLocalTimeLatest_st.nanosecondsHi    = timeStamp_VLocal.nanosecondsHi;
        StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
        StbMVirtualLocalTimeLatest_st.nanosecondsLo        = timeStamp_VLocal.nanosecondsLo;

        /* Store the value of Local Time (TLsync) */
        StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->StbMLocalTimeLatest_st.nanoseconds    =
                timeStamp_Global.nanoseconds;
        StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->StbMLocalTimeLatest_st.seconds        =
                timeStamp_Global.seconds;
        StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->StbMLocalTimeLatest_st.secondsHi      =
                timeStamp_Global.secondsHi;
        StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->StbMLocalTimeLatest_st.timeBaseStatus =
                timeStamp_Global.timeBaseStatus;


        /* Process to end an ongoing measurement */
        /* Check do we have any measurement running for current timebase ID */
        /* RateCorrectionCycleCount_u16, which is used to maintain the cycle count will be set back to 0,
           if the timebase status changes bit values of TIMELEAP_FUTURE, TIMELEAP_PAST, SYNC_TO_GATEWAY or TIMEOUT */
        if(StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
                                                                               RateCorrectionCycleCount_u16 > STBM_ZERO)
        {
            /* Yes, one or more than one measurements are running for current time base ID */

            /* Get the pointer to the array where time snapshots (both global and local) are maintained */
            lRateCorrectionTimeMaintainenceArray = StbM_Cfg_TimeCorrectionArray_ast[index].
                                                      RateCorrectionInfoStruct_ptr->RateCorrectionTimeSnapshotArray_ptr;

            /* Get the current cycle count before ending any measurement */
            lRateCorrectionCycleCountNow = StbM_Cfg_TimeCorrectionArray_ast[index].
                                                      RateCorrectionInfoStruct_ptr->RateCorrectionCycleCount_u16;

            /* Check current virtual local timestamp with all ongoing measurements to find if now we have an
               End of measurement or not*/
            for(lRCCycleCountTemp_u16 = STBM_ZERO;
                    lRCCycleCountTemp_u16 <
                                 StbM_Cfg_TimeCorrectionArray_ast[index].StbMRateCorrectionsPerMeasurementDuration_u16;
                    lRCCycleCountTemp_u16++)
            {
                /* Calculate the delta, start time of the measurement wrt current time now, only for valid array indexes */
                /* Also, loop only if we have not covered all the valid indexes */
                if(((lRateCorrectionTimeMaintainenceArray + lRCCycleCountTemp_u16)->StbM_ValidTimeStamp_b)&&
                    (lRCCycleValidCount_u16 < lRateCorrectionCycleCountNow))
                {
                    lRCCycleValidCount_u16 ++;

                    /* Get Virtual Local time snapshot for current cycle under observation */
                    lVLocalStartCurCycle_st.nanosecondsHi      =
                                                       (lRateCorrectionTimeMaintainenceArray + lRCCycleCountTemp_u16)->
                                                        RateCorrectionVirtualLocalStartTime_st.nanosecondsHi;
                    lVLocalStartCurCycle_st.nanosecondsLo        =
                                                       (lRateCorrectionTimeMaintainenceArray + lRCCycleCountTemp_u16)->
                                                        RateCorrectionVirtualLocalStartTime_st.nanosecondsLo;
                    /* Check the difference between Virtual local start time of current cycle under observation
                       and current virtual local timestamp */

                    StbM_Rb_ConvertVirtualLocalTimeToTimestampType(timeStamp_VLocal,&lLocalTimeNow_st);

                    StbM_Rb_ConvertVirtualLocalTimeToTimestampType(lVLocalStartCurCycle_st,&lStartCurCycle_st);

                    StbM_Time_DifferenceCalculation(&lLocalTimeNow_st,&lStartCurCycle_st,
                            &lDiffStartCurrent_st,&lgreaterTimeStamp_u8);

                    if(lgreaterTimeStamp_u8 == STBM_ONE)
                    {
                        /* [SWS_StbM_00370] During runtime the StbM shall determine the timespan of a Rate Correction
                           measurement on the basis of the Virtual Local Time. */

                        /* Is this difference greater than or equal to configured measurement duration */
                        lMeasDuration.nanoseconds = StbM_Cfg_TimeCorrectionArray_ast[index].
                                                                   StbMRateCorrectionMeasurementDuration_st.nanoseconds;
                        lMeasDuration.seconds     = StbM_Cfg_TimeCorrectionArray_ast[index].
                                                                   StbMRateCorrectionMeasurementDuration_st.seconds;
                        lMeasDuration.secondsHi   = StbM_Cfg_TimeCorrectionArray_ast[index].
                                                                   StbMRateCorrectionMeasurementDuration_st.secondsHi;

                        StbM_Time_DifferenceCalculation(&lDiffStartCurrent_st,&lMeasDuration,
                                                        &lDiffDeltaMeasDuration,&lgreaterTimeStamp_u8);

                        if(lgreaterTimeStamp_u8 == STBM_ONE)
                        {
                            /* Delta between the start of current cycle as represented by lRCCycleCountTemp_u16 and
                               virtual local time now is greater than configured measurement duration and therefore it
                               should end now*/

                            /* Check if start time of current measurement cycle is less than other cycle that are ending
                               if there are multiple cycle ends at this particular instant of BSGT API call */

                            /* Reinitialize variable so that can be used again below. */
                            lgreaterTimeStamp_u8 = STBM_ONE;

                            StbM_Rb_ConvertVirtualLocalTimeToTimestampType(lVLocalStartTempRC_st,&lLocalStartTempRC_st);

                            StbM_Time_DifferenceCalculation(&lLocalStartTempRC_st,&lStartCurCycle_st,
                                                            &lDiffDeltaMeasDuration,&lgreaterTimeStamp_u8);

                            if(lgreaterTimeStamp_u8 == STBM_ONE)
                            {
                                /* Start of current cycle is less than the start of previously ended cycle,
                                   so this will be used for Rate calculation at the end*/

                                /* Store the start of virtual Local Time for Rate correction measurement later on */
                                lVLocalStartTempRC_st.nanosecondsHi      = lVLocalStartCurCycle_st.nanosecondsHi;
                                lVLocalStartTempRC_st.nanosecondsLo      = lVLocalStartCurCycle_st.nanosecondsLo;

                                /* Store the start of Global Time for Rate correction measurement later on */
                                lGlobalStartTempRC_st.secondsHi      =
                                                        (lRateCorrectionTimeMaintainenceArray + lRCCycleCountTemp_u16)->
                                                         RateCorrectionGlobalStartTime_st.secondsHi;
                                lGlobalStartTempRC_st.seconds        =
                                                        (lRateCorrectionTimeMaintainenceArray + lRCCycleCountTemp_u16)->
                                                         RateCorrectionGlobalStartTime_st.seconds;
                                lGlobalStartTempRC_st.nanoseconds    =
                                                        (lRateCorrectionTimeMaintainenceArray + lRCCycleCountTemp_u16)->
                                                         RateCorrectionGlobalStartTime_st.nanoseconds;
                                lGlobalStartTempRC_st.timeBaseStatus =
                                                        (lRateCorrectionTimeMaintainenceArray + lRCCycleCountTemp_u16)->
                                                         RateCorrectionGlobalStartTime_st.timeBaseStatus;
                            }
                            else
                            {
                                /* Start of this cycle is greater than the start of previously ended cycle,
                                   hence we need not consider its time value */
                            }

                            /* Mark current cycle as FALSE now, as this has ended, this is done so that this array
                               location can be used to store time values as the sequence in which the measurements will
                               end is arbitary */
                            (lRateCorrectionTimeMaintainenceArray+lRCCycleCountTemp_u16)->StbM_ValidTimeStamp_b = FALSE;

                            /* Decrement the variable that is maintaining the number of measurements going on for
                               current timebase */
                            StbM_Cfg_TimeCorrectionArray_ast[index].RateCorrectionInfoStruct_ptr->
                                                                                         RateCorrectionCycleCount_u16--;

                            /* This is to notify that for current timebase Rate has to be calculated as one or more
                               measurement cycles are ending now */
                            lStbM_RateCalculated_b = TRUE;
                        }
                        else
                        {
                            /* Delta between the start of current cycle as represented by lRCCycleCountTemp_u16 and
                               virtual local time now is less than configured measurement duration and therefore we
                               can't do anything now. Check in next step if any measurement can be started */
                        }
                    }
                }
            }

            /* Do we have to calculate the rate now for this call of BSGT API */
            if(lStbM_RateCalculated_b == TRUE)
            {
                /* Now we have the start time (both global and local of measurement which started first.
                   In case multiple measurements are ending) */

                /* End the measurement */
                StbM_EndRateCorrectionMeasurement(index,&lVLocalStartTempRC_st,&lGlobalStartTempRC_st,
                        &timeStamp_CurrentGlobal,&timeStamp_VLocal);
            }
        }
        else
        {
            /* No measurements ongoing, so nothing to end. Move forward to next step to start a new measurement
               (if applicable). */
        }
        /* ~ Process to end an ongoing measurement */


        /* [SWS_StbM_00373] For Rate Correction, the StbM shall evaluate the TIMELEAP_FUTURE/TIMELEAP_PAST flags
           during the start of a measurement. The StbM shall not start a Rate Correction measurement when the state
           of any of the flags equals "Set". */
        /* [SWS_StbM_00371] The StbM shall perform Rate Correction measurements continuously. The end of a measurement
           marks the start of the next measurement. The start and end of measurements is always triggered by and
           aligned to the reception of time values for Synchronized or Offset Time Bases. */

        if(!(StbM_Check_Bit(timeStamp_CurrentGlobal.timeBaseStatus,STBM_TIMELEAP_FUTURE_BIT)))
        {
            if(!(StbM_Check_Bit(timeStamp_CurrentGlobal.timeBaseStatus,STBM_TIMELEAP_PAST_BIT)))
            {
                /* Start a new measurement */

                /* All the checks required to be verified before starting a measurement will be checked
               inside this function */
                StbM_StartRateCorrectionMeasurement(index,&timeStamp_VLocal,&timeStamp_CurrentGlobal);
            }
            else
            {
               /* No measurement will start*/
            }
        }
        else
        {

        }
    }
    else
    {
        /* Nothing to do */
        /* Time correction is not configured for this timebase */
    }
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
