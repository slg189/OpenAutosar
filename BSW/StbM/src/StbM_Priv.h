

#ifndef STBM_PRIV_H
#define STBM_PRIV_H

/**
***************************************************************************************************
* \moduledescription
*                      The file contains decalrations that is used only in StbM module
*
* \scope               This is included only by StbM module
***************************************************************************************************
*/

/*
**********************************************************************************************************************
* Includes
**********************************************************************************************************************
*/

#include "StbM_Cfg_SchM.h"


/*
 **********************************************************************************************************************
 * Define/Macros
 **********************************************************************************************************************
 */

/*#defines used in StbM module*/

#define STBM_MAX_NS                          999999999u          /*Nanoseconds max value*/
#define STBM_MAX_SEC                         4294967295u         /*MAX uint32 value*/
#define STBM_MAX_NUM_TIMEBASES               128u                /*MAX number of timebases(Sync & offset timebasesand pure local timebases)*/
#define STBM_SYNC_TIMEBASES_MAX              15u                 /*MAX ID Range for Sync timebases*/
#define STBM_SYNC_TIMEBASES_MIN              0u                  /*MIN ID Range for Sync timebases*/
#define STBM_OFFSET_TIMEBASES_MAX            31u                 /*MAX ID Range for Offset timebases*/
#define STBM_OFFSET_TIMEBASES_MIN            16u                 /*MIN ID Range for Offset timebases*/
#define STBM_PURELOCAL_TIMEBASES_MAX         127u                /*MAX ID Range for Pure Local timebases*/
#define STBM_PURELOCAL_TIMEBASES_MIN         32u                 /*MIN ID Range for Pure Local timebases*/
#define STBM_INT_MAX32                       2147483647          /*MAX sint32 value*/
#define STBM_INT_MIN32                      -2147483648          /*MIN sint32 value*/
#define STBM_NS_PER_SEC                      1000000000u         /*Number of Nanoseconds per Second */
#define STBM_S_PER_SECHI                     4294967296u         /*Number of Seconds per SecondsHigh */
#define STBM_MAX_UINT32                      4294967295u         /* Maximum value for uint32 variable */
#define STBM_MILLION                         1000000u
#define STBM_RATE_MAX_PPM                    1032000u            /* Rate in ppm corresponding to maximum value of Rate deviation */
#define STBM_RATE_MIN_PPM                    968000u             /* Rate in ppm corresponding to minimum value of Rate deviation */
#define STBM_USERDATA_LENGTH_MAX             3u                  /* Userdata maximum length */


#define STBM_TIMEOUT_BIT                     0u                   /*TIMEOUT bit in timeBaseStatus*/
#define STBM_SYNC_TO_GATEWAY_BIT             2u                   /*SYNC_TO_GATEWAY bit in timeBaseStatus*/
#define STBM_GLOBAL_TIME_BASE_BIT            3u                   /*GLOBAL_TIME_BASE bit in timeBaseStatus*/
#define STBM_TIMELEAP_FUTURE_BIT             4u                   /*TIMELEAP_FUTURE bit in timeBaseStatus*/
#define STBM_TIMELEAP_PAST_BIT               5u                   /*TIMELEAP_PAST bit in timeBaseStatus*/

/*Events for timebasestatus change*/
#define STBM_EV_GLOBAL_TIME_BASE                  0u                   /* Event for GLOBAL_TIME_BASE bit set */
#define STBM_EV_TIMEOUT_OCCURED                   1u                   /* Event for TIMEOUT bit set */
#define STBM_EV_TIMEOUT_REMOVED                   2u                   /* Event for TIMEOUT bit reset */
#define STBM_EV_TIMELEAP_FUTURE                   3u                   /* Event for TIMELEAP_FUTURE bit set */
#define STBM_EV_TIMELEAP_FUTURE_REMOVED           4u                   /* Event for TIMELEAP_FUTURE  bit reset */
#define STBM_EV_TIMELEAP_PAST                     5u                   /* Event for TIMELEAP_PAST bit set */
#define STBM_EV_TIMELEAP_PAST_REMOVED             6u                   /* Event for TIMELEAP_PAST bit reset */
#define STBM_EV_SYNC_TO_SUBDOMAIN                 7u                   /* Event for SYNC_TO_GATEWAY bit set */
#define STBM_EV_SYNC_TO_GLOBAL_MASTER             8u                   /* Event for SYNC_TO_GATEWAY bit reset */
#define STBM_EV_RESYNC                            9u                   /* Event for RESYNCHRONISATION */
#define STBM_EV_RATECORRECTION                   10u                   /* Event for RATE CORRECTION */

#define STBM_ZERO                            0u                   /*#define for 0*/
#define STBM_SIGNED_ZERO                     0                    /*#define for 0*/
#define STBM_ONE                             1u                   /*#define for 1*/
#define STBM_VAL_MINUS_ONE                  -1                    /*#define for -1*/
#define STBM_TWO                             2u                   /*#define for 2*/
#define STBM_THIRTYTWO                       0x20                 /*#define for 32*/

#define STBM_OS_TIMEREF_OFF                                   NULL_PTR                      /*Local Timeref is not configured for Timebase*/
#define STBM_ETH_GLOBAL_DOMAINREF_ON                          TRUE                          /*Ethref is configured for timebase*/
#define STBM_ETH_GLOBAL_DOMAINREF_OFF                         FALSE                         /*Ethref is not configured for timebase*/
#define STBM_OFFSET_TIMEBASE_OFF                              STD_OFF                       /*Offset time base is not configured for timebase*/
#define STBM_EXTERNAL_COUNTERREF_ON                           TRUE                         /*Externalcounterref is configured for timebase*/
#define STBM_EXTERNAL_COUNTERREF_OFF                          FALSE                        /*Externalcounterref is not configured for timebase*/
#define STBM_ETHIF_INVALID_CTRL_ID                            0xFF                          /*EthIf ctrlid for the timebase is invalid*/
#define STBM_GPT_INVALID_CHANNEL_ID                           0xFF                          /*EthIf ctrlid for the timebase is invalid*/
#define STBM_INVALID_INDEX                                    0xFF                          /*Index for the timebase is invalid*/
#define STBM_ALLOW_MASTER_RATE_CORRECTION_ON                  TRUE                          /* TimeCorrection is configured for the timebase */
#define STBM_ALLOW_MASTER_RATE_CORRECTION_OFF                 FALSE                         /* TimeCorrection is not configured for the timebase*/
#define STBM_ALLOW_SYSTEM_WIDE_GLOBAL_TIME_MASTER_ON          TRUE                          /* Global Time Master is configured to act as a
																								system-wide source of time */
#define STBM_ALLOW_SYSTEM_WIDE_GLOBAL_TIME_MASTER_OFF         FALSE                         /* Global Time Master is not configured to act as a
																								system-wide source of time */
#define STBM_INVALID_TIMEBASEID              65535u               /*Invalid Timebase id*/

#define STBM_ACCELERATE_RATE                 TRUE                 /* The additional rate needs to be added to origanally calculated rate */
#define STBM_DEACCELERATE_RATE               FALSE                /* The additional rate needs to be substracted from origanally calculated rate */
#define STBM_MAX_SEC_LIMIT                   3u                  /* Maximum seconds limit to update the Main Time Tuple*/
#define STBM_NO_COUNTER_UPDATE               0u                  /* Macro to indicate not to update the counter*/
#define STBM_COUNTER_UPDATE                  1u                  /* Macro to indicate to update the counter*/

#define STBM_USER_DATA_NULL_PTR           0u                  /* Macro to indicate to if UserData has to be checked if its valid and update */
#define STBM_USER_DATA_INVALID            1u                  /* Macro to indicate to if UserData has to be directly updated */
#define STBM_USER_DATA_VALID              2u                  /* Macro to indicate to if UserData has to be directly updated */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
/* Local structure to link timebase record with corresponding block of record table*/
typedef struct
{
    /* This represents the recording block size for particular timebase */
    const uint16 RecordTableBlockCount_u16;
    /* This represents the pointer to record block array for synchronized timebase */
    StbM_SyncRecordTableBlockType * SynchronizedTimeBaseBlockRecordArray_ptr;
    /* This represents the pointer to record block array for Offset timebase */
    StbM_OffsetRecordTableBlockType * OffsetTimeBaseBlockRecordArray_ptr;
    /* This represents the function pointer to callback function for synchronized timebase */
    Std_ReturnType (*StbMSyncTimeRecordBlockCallback) (const StbM_SyncRecordTableBlockType * SyncRecordTableBlock);
    /* This represents the function pointer to callback function for offset timebase */
    Std_ReturnType (*StbMOffsetTimeRecordBlockCallback) (const StbM_OffsetRecordTableBlockType * OffsetRecordTableBlock);
}StbM_TimeBaseBlockRecord_tst;

/* Local structure to link timebase record with corresponding Header of record table*/
typedef struct
{
    /* This represents the header for synchronized timebase */
    const StbM_SyncRecordTableHeadType SynchronizedTimeBaseHeaderRecord_st;
    /* This represents the header for Offset timebase */
    const StbM_OffsetRecordTableHeadType OffsetTimeBaseHeaderRecord_st;
}StbM_TimeBaseHeaderRecord_tst;

#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/

typedef struct
{
#if (STBM_OS_ENABLE == STD_ON)
    /* "StbMLocalTimeHardware" for Os configuration*/
    const CounterType StbMLocalTimeHardware_OsRef;
    #endif  /*(STBM_OS_ENABLE == STD_ON) */
#if ((STBM_OS_ENABLE == STD_ON)||(STBM_GPT_ENABLE == STD_ON))
    /*Represents the Clock ratio used in converstion of ticks to nanoseconds*/
    const uint32 StbMClockRatio_u32;
#endif /*(STBM_OS_ENABLE == STD_ON)||(STBM_GPT_ENABLE == STD_ON)*/
    /*Type of the "StbMLocalTimeHardware" */
    const StbMLocalTimeHardware_ten StbMLocalTimeHardware_e;
#if (STBM_ETH_ENABLE == STD_ON)
    /*"StbMLocalTimeHardware" for Eth configuration*/
    const uint8 StbMLocalTimeHardware_EthIf_Ctrl_Id;
#endif  /*(STBM_ETH_ENABLE == STD_ON) */
#if (STBM_GPT_ENABLE == STD_ON)
    /*"StbMLocalTimeHardware" for Gpt configuration*/
    const Gpt_ChannelType StbMLocalTimeHardware_GptChannel_Id;
#endif  /*(STBM_GPT_ENABLE == STD_ON) */
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
    /*Mandatory parameter which is true in case the local time shall be accessed via external counter*/
    const boolean ExternalCounterRef_b;
    /* StbMRbExternalCounterGetCounterValueCallback Callback for ExternalCounterGetCounter */
    void (*StbMExternalCounterGetCounterValueCallback) (TickType * ticks);
    /* StbMRbExternalCounterGetElapsedValueCallback Callback for ExternalCounterGetCounter  */
    void (*StbMExternalCounterGetElapsedValueCallback) (const TickType * lastTick,
                                                                          TickType * deltaTick);
    /* StbMRbExternalCounterGetElapsedValueCallback  Callback for ExternalCounterGetCounter  */
    uint32 (*StbMExternalCounterTicksTonsCallback) (TickType ticks);
#endif  /*(STBM_EXT_COUNTER_ENABLE == STD_ON) */
}StbM_LocalTimeClock_tst;

#if (STBM_TIME_CORRECTION ==  STD_ON )

/* Local structure to maintain Time value snapshots for Rate correction */
typedef struct
{
    /* Flag variable to check if the current measurement is valid or not */
    boolean StbM_ValidTimeStamp_b;
    /* Global time when the measurement was started */
    StbM_TimeStampType RateCorrectionGlobalStartTime_st;
    /* Virtual Local time when the measurement was started */
    StbM_VirtualLocalTimeType RateCorrectionVirtualLocalStartTime_st;
}StbM_TimeCorrectionTimeSnapshot_tst;

/* Local structure to store Rate Correction related information */
typedef struct
{
    /* Count of ongoing Rate Corrections */
    uint16 RateCorrectionCycleCount_u16;
    /* Calculated Rate in ppm */
    uint32 RateCurrent_u32;
    /* Minimum Offset between simultaneous Rate Correction cycles */
    const StbM_TimeStampType StbMRateCorrectionMinimumOffset_st;
    /* Last time when Rate Correction cycles began */
    StbM_VirtualLocalTimeType StbMRateCorrectionLastCycleStartTime_st;
    /* Value of Virtual Local Time  when last time BSGT API was called */
    StbM_VirtualLocalTimeType StbMVirtualLocalTimeLatest_st;
    /* Value of Local Time  when last time BSGT API was called */
    StbM_TimeStampType StbMLocalTimeLatest_st;
    /* Pointer to array where Rate correction time value snapshots are stored */
    StbM_TimeCorrectionTimeSnapshot_tst * RateCorrectionTimeSnapshotArray_ptr;
}StbM_RateCorrection_tst;

/* Local structure to store Offset Correction related information */
typedef struct
{
    /* Additional Rate sign, if this needs to be added or substracted from original rate */
    boolean AdditionalRateType_b;
    /* Additional Rate, to be used in case of Rate Adaption (Roc) in ppm */
    uint32 AdditionalRate_u32;
    /* Offset Correction Adaption Interval : Common : Used in offset correction */
    const uint64 StbMOffsetCorrectionAdaptionIntervalTotalNs_u64;
    /* Offset Correction Adaption Interval */
    const StbM_TimeStampType StbMOffsetCorrectionAdaptionInterval_st;
    /* Offset Correction Jump Threshold */
    const StbM_TimeStampType StbMOffsetCorrectionJumpThreshold_st;
}StbM_OffsetCorrection_tst;

/* Local structure to maintain Time Correction information */
typedef struct
{
    /* Time correction offset correction is done via Rate Correction or Offset Correction*/
    StbM_TimeCorrectionType_ten TimeCorrectionType_ten;
    /* Maximum Rate Corrections Per Measurement Duration */
    const uint16 StbMRateCorrectionsPerMeasurementDuration_u16;
    /* Rate Correction Measurement Duration */
    const StbM_TimeStampType StbMRateCorrectionMeasurementDuration_st;
    /* Pointer to Offset Correction structure */
    StbM_OffsetCorrection_tst * OffsetCorrectionInfoStruct_ptr;
    /* Pointer to Rate Correction structure */
    StbM_RateCorrection_tst * RateCorrectionInfoStruct_ptr;
}StbM_TimeCorrection_tst;

#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */

/*
 **********************************************************************************************************************
 * Local Inline Functions
 **********************************************************************************************************************
 */
LOCAL_INLINE uint64 StbM_ConvertTimestamp2Nanoseconds(StbM_TimeStampType TimeStamp );

/* Local Inline Function to convert TimeStamp to Nanoseconds */
LOCAL_INLINE uint64 StbM_ConvertTimestamp2Nanoseconds(StbM_TimeStampType TimeStamp )
{
    uint64 lNanoSecTime;
    lNanoSecTime = STBM_ZERO;

    lNanoSecTime = (uint64)((((uint64)TimeStamp.secondsHi)* STBM_S_PER_SECHI * STBM_NS_PER_SEC) +
            (((uint64)TimeStamp.seconds)* STBM_NS_PER_SEC) +
            TimeStamp.nanoseconds) ;

    return lNanoSecTime;
}

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

/*
**********************************************************************************************************************
* Global Variables
**********************************************************************************************************************
*/

#define STBM_START_SEC_VAR_INIT_UNSPECIFIED
#include "StbM_MemMap.h"
extern boolean StbM_InitState_b;
#define STBM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_INIT_8
#include "StbM_MemMap.h"
/* Array to maintain the understanding of index for each timebase */
extern uint8 StbM_Index_au8[STBM_MAX_NUM_TIMEBASES];
#define STBM_STOP_SEC_VAR_INIT_8
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_INIT_8
#include "StbM_MemMap.h"
/* Array to maintain the understanding of index of NvM configured Timebases */
extern uint8 StbM_Nvm_Index_au8[STBM_MAX_NUM_TIMEBASES];
#define STBM_STOP_SEC_VAR_INIT_8
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_8
#include "StbM_MemMap.h"
extern uint8 StbM_TimeBaseUpdateCounter_au8[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_8
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_16
#include "StbM_MemMap.h"
extern uint16 StbM_TimeLeap_Past_Count_u16[STBM_CFG_NUM_OF_TIMEBASES];
extern uint16 StbM_TimeLeap_Future_Count_u16[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_16
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_32
#include "StbM_MemMap.h"
#if STBM_CFG_TRIGGERED_CUSTOMER
extern uint32 StbM_TrigCusPeriod_u32[STBM_CFG_TRIGGERED_CUSTOMER];
#endif /*#if STBM_CFG_TRIGGERED_CUSTOMER*/
#define STBM_STOP_SEC_VAR_CLEARED_32
#include "StbM_MemMap.h"

#if (STBM_TIME_NOTIFICATION ==  STD_ON )
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
extern StbMNotificationExpiryRecord_tst StbMNotificationExpiryRecordStruct_st ;
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif /*#if (STBM_TIME_NOTIFICATION ==  STD_ON ) */

#define STBM_START_SEC_VAR_CLEARED_32
#include "StbM_MemMap.h"
extern StbM_TimeBaseNotificationType StbM_NotificationEvents_au32[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_32
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
extern StbM_UserDataType StbM_UserDataArray_ast   [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
extern StbM_TimeStampType StbM_GlobalTimeTupleArray_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
extern StbM_TimeStampType StbM_GlobalTimeReception_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

#if (STBM_NVM_ENABLE == STD_ON)
#define STBM_START_SEC_VAR_SAVED_ZONE_UNSPECIFIED
#include "StbM_MemMap.h"
extern StbM_TimeStampType StbM_TimesourceArray_Nvm_ast [STBM_CFG_NUM_OF_TIMEBASES_NVM];
#define STBM_STOP_SEC_VAR_SAVED_ZONE_UNSPECIFIED
#include "StbM_MemMap.h"
#endif /*#if (STBM_NVM_ENABLE == STD_ON)*/

#if (STBM_TIME_RECORDING_SUPPORT == STD_ON)
#define STBM_START_SEC_VAR_CLEARED_16
#include "StbM_MemMap.h"
extern uint16 StbM_RecordTableBlockCountCurrent_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_16
#include "StbM_MemMap.h"
#endif /*#if (STBM_TIME_RECORDING_SUPPORT == STD_ON)*/

#define STBM_START_SEC_VAR_CLEARED_8
#include "StbM_MemMap.h"
extern uint8 StbM_Index_HWCounterRef_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_8
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/* Array to maintain the Virtual Local Time value*/
extern StbM_VirtualLocalTimeType StbM_VirtualLocalTimeArray_ast[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/* Array to maintain the Virtual Local Time value*/
extern StbM_VirtualLocalTimeType StbM_VirtualLocalTimeTupleArray_ast[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

/* Array added to maintain Virtual Local Time after the BusSetGlobalTime Call*/
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
extern StbM_VirtualLocalTimeType StbM_VirtualLocalTimeReception_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"

#if(STBM_OS_ENABLE == STD_ON )
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/* Array to maintain last update of Virtual Local Time value if hardware reference is OS */
extern TickType StbM_Os_VirtualLocalTimeArray[STBM_NUM_OS_REF];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif/* #if(STBM_OS_ENABLE ==  STD_ON )*/

#if(STBM_ETH_ENABLE == STD_ON )
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/* Array to maintain last update of Virtual Local Time value if hardware reference is Eth */
extern StbM_TimeStampType StbM_Eth_VirtualLocalTimeArray_ast[STBM_NUM_Eth_REF];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif/* #if(STBM_ETH_ENABLE ==  STD_ON )*/

#if(STBM_GPT_ENABLE == STD_ON )
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/*Array to maintain last update of Virtual Local Time value if hardware reference is GPT */
extern Gpt_ValueType StbM_Gpt_VirtualLocalTimeArray[STBM_NUM_GPT_REF];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif/* #if(STBM_GPT_ENABLE ==  STD_ON )*/

#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
#define STBM_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
/* Array to maintain last update of Virtual Local Time value if hardware reference is External counter reference */
extern TickType StbM_ExternalCounter_VirtualLocalTimeArray[STBM_NUM_EXT_REF];
#define STBM_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "StbM_MemMap.h"
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/

#define STBM_START_SEC_CONST_UNSPECIFIED
#include "StbM_MemMap.h"

extern const StbM_SynchronizedTimeBase_tst StbM_Cfg_SynchronizedTimeBaseArray_ast[STBM_CFG_NUM_OF_TIMEBASES];

/* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
#if (STBM_TIME_RECORDING_SUPPORT == STD_ON)
extern const StbM_TimeBaseHeaderRecord_tst StbM_Cfg_TimeBaseHeaderRecordArray_ast[STBM_CFG_NUM_OF_TIMEBASES];
#endif /* #if (STBM_TIME_RECORDING_SUPPORT == STD_ON) */

#if STBM_CFG_TRIGGERED_CUSTOMER
extern const StbM_TriggeredCustomer_tst StbM_Cfg_TriggeredCustomerArray_ast[STBM_CFG_TRIGGERED_CUSTOMER];
#endif

extern const StbM_LocalTimeClock_tst StbM_Cfg_LocalTimeClockArray_ast[STBM_CFG_NUM_OF_TIMEBASES];

#define STBM_STOP_SEC_CONST_UNSPECIFIED
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_INIT_UNSPECIFIED
#include "StbM_MemMap.h"

/* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
#if (STBM_TIME_RECORDING_SUPPORT == STD_ON)
extern StbM_TimeBaseBlockRecord_tst StbM_Cfg_TimeBaseBlockRecordArray_ast[STBM_CFG_NUM_OF_TIMEBASES];
#endif /* #if (STBM_TIME_RECORDING_SUPPORT == STD_ON) */

#define STBM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "StbM_MemMap.h"

#if (STBM_TIME_CORRECTION ==  STD_ON )

#define STBM_START_SEC_VAR_INIT_UNSPECIFIED
#include "StbM_MemMap.h"
extern StbM_TimeCorrection_tst StbM_Cfg_TimeCorrectionArray_ast[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "StbM_MemMap.h"

#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */

/*
**********************************************************************************************************************
* Local function declarations
**********************************************************************************************************************
*/

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Function to returns a time value in standard format (Local Time Base derived from Global Time Base) based on Virtual
 * Local Time passed */
extern Std_ReturnType StbM_GetCurrentTime_Internal(uint8 index,
        StbM_TimeStampType * timeStamp,
        const StbM_VirtualLocalTimeType * vlocalTime,
        StbM_UserDataType * userData);

/* Function to calculate the difference between two time values  */
extern void StbM_Time_DifferenceCalculation
(const StbM_TimeStampType * timeStampPtr_current,
const StbM_TimeStampType * timeStampPtr_last,
StbM_TimeStampType * timeStampPtr,
uint8 * greaterTimeStamp
);

/* Function to update the userdata bytes depending on the length of userdata length */
extern void StbM_UserDataUpdate( uint8 Index_u8,
                                                const StbM_UserDataType * userData );

/* Function to calculate the current time for Synchronised Timebase */
extern  void StbM_GetCurrentTime_Synchronised_Timebase(
        StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_VirtualLocalTimeType virtualLocalTimeNow,
        StbM_VirtualLocalTimeType virtualLocalTimeDiff,
       StbM_TimeStampType * timeStampPtr);

/* Function to check the overflow when two time values are added */
extern StbM_TimeStampType StbM_Overflow_Check (uint8 index,
                                                                StbM_TimeStampType timestampIn,
                                                                StbM_TimeValueType_ten timevaluetype);

/* Function to calculate the sum of Offset time basevalue and synchronised time base value */
extern StbM_TimeStampType StbM_Offset_Overflow_Check (
       StbM_TimeStampType OffsetTimebaseValue,
       const StbM_TimeStampType * timeStampPtr);

/* Function to perform the initialisation of Virtual local time */
extern void StbM_VirtualLocalTime_Init(uint8 index);

/* Function to read the current time value without rate correction applied */
extern Std_ReturnType StbM_GetCurrentTimeWithoutRateCorrection(
        uint8 index,
        StbM_TimeStampType * globalTimeStamp,
        const StbM_VirtualLocalTimeType * timeStampVLocalNow_st,
        StbM_UserDataType * userData);

#if (STBM_GPT_ENABLE == STD_ON)
extern Gpt_ValueType StbM_DeltaTimeCalculation_GPT(Gpt_ValueType LastTickValue, Gpt_ValueType CurrentTickValue);
#endif /* #if (STBM_GPT_ENABLE == STD_ON) */

/* Internal API to update the Virtual Local Time Tuple*/
extern void StbM_Rb_GlobalTimeTupleUpdate
(StbM_SynchronizedTimeBaseType timeBaseId,
 StbM_VirtualLocalTimeType virtualLocalTimeNow,
 StbM_VirtualLocalTimeType virtualLocalTimeDiff);

/* API to convert the Virtual local time into StbM_TimeStampType format*/
extern void StbM_Rb_ConvertVirtualLocalTimeToTimestampType
(StbM_VirtualLocalTimeType localTimePtr,
 StbM_TimeStampType * VirtuallocalTimePtr);

/* Function to read the current Virtual Local time value*/
extern Std_ReturnType StbM_GetCurrentVirtualLocalTime_Internal(uint8 index,
                                      StbM_VirtualLocalTimeType * VLocalTimeStamp_Now,
                                      uint8 updateHwCounter);

/* API to calculate the difference between two Virtual Local Time*/
extern StbM_VirtualLocalTimeType StbM_Rb_VirtualLocalTimeDifference
        (StbM_VirtualLocalTimeType VLocalTimeStamp_Last,
         StbM_VirtualLocalTimeType VLocalTimeStamp_Current);

/* Function to check the Timeout */
extern void StbM_GetCurrentTime_Timeout_Check(uint8 index,
        const StbM_VirtualLocalTimeType * virtualLocalTime);

#if (STBM_TIME_CORRECTION ==  STD_ON )
/* Function to perform Time correction */
extern void StbM_TimeCorrectionMeasurement(uint8 index,
        StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampType timeStamp_Global,
        StbM_TimeStampType timeStamp_CurrentGlobal,
        StbM_VirtualLocalTimeType timeStamp_VLocal);

/* Function to reset the ongoing rate correction measurements */
extern void StbM_ResetOngoingRateCorrections(uint8 index);

/* Function to read the current time with rate correction */
extern StbM_TimeStampType StbM_GetRateCorrectedTime(uint8 index,
                                         StbM_SynchronizedTimeBaseType timeBaseId,
                                         const StbM_VirtualLocalTimeType * VLocalTimeNow);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */


#if (STBM_TIME_NOTIFICATION == STD_ON )
/* Function to cyclically monitor the Time Bases and start the GPT timer if the threshold has elapsed */
extern void StbM_SetExpiryTime(StbM_SynchronizedTimeBaseType timeBaseId,
                                                  StbM_CustomerIdType customerId);

/* Function to notify the StbM, that the GPT timer, which is used to trigger the StbM_TimeNotificationCallback has expired. */
extern void StbM_TimerCallback(void);

#if (STBM_ECUC_RB_RTE_IN_USE != STD_ON)
extern void StbM_TimerCallback_Wrapper(void);
#endif /* #if (STBM_ECUC_RB_RTE_IN_USE != STD_ON ) */

#endif /* #if (STBM_TIME_NOTIFICATION == STD_ON ) */

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#endif /* STBM_PRIV_H */

/*
**********************************************************************************************************************
* End of the file
**********************************************************************************************************************
*/
