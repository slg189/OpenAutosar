

#ifndef STBM_H
#define STBM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "StbM_Types.h"

#if (STBM_ECUC_RB_RTE_IN_USE == STD_ON)
/*The function declarations of RTE interfaced API's will be geberated in the SchM_StbM.h and Rte_StbM.h */
#include "Rte_StbM_Type.h"
#endif  /*(STBM_ECUC_RB_RTE_IN_USE == STD_ON) */

/*[SWS_StbM_00065] If a triggered customer is configured StbM shall include Os.h to have access to the schedule table
  interface of the OS*/
#include "Os.h"
/*#if (!defined(OS_AR_RELEASE_MAJOR_VERSION) || (OS_AR_RELEASE_MAJOR_VERSION != 4))
#error "OS AUTOSAR major version undefined or mismatched"
#elif (!defined(OS_AR_RELEASE_MINOR_VERSION) || (OS_AR_RELEASE_MINOR_VERSION != 2))
#error "OS AUTOSAR minor version undefined or mismatched"
#endif*/

#include "StbM_Cfg.h"

#if ((STBM_GPT_ENABLE == STD_ON) || (STBM_TIME_NOTIFICATION ==  STD_ON ))
/* If GPT is configured as StbMLocalTimeHardware StbM shall include Gpt.h */
#include "Gpt.h"
/*#if (!defined(GPT_AR_RELEASE_MAJOR_VERSION) || (GPT_AR_RELEASE_MAJOR_VERSION != 4))
#error "GPT AUTOSAR major version undefined or mismatched"
#elif (!defined(GPT_AR_RELEASE_MINOR_VERSION) ||(GPT_AR_RELEASE_MINOR_VERSION != 2))
#error "GPT AUTOSAR minor version undefined or mismatched"
#endif*/
#endif  /*(STBM_GPT_ENABLE == STD_ON) */

/*
 **********************************************************************************************************************
 * Define/Macros
 **********************************************************************************************************************
 */

#define STBM_INSTANCE_ID                      0u               /*INSTANCE ID of STBM module*/

/* #defines */
#define STBM_MODULE_ID                        (160u)           /*MODULE ID of STBM module*/
#define STBM_VENDOR_ID                        (6u)             /*VENDOR ID of STBM module*/

/*det error types*/
#if(STBM_DEV_ERROR_DETECT== STD_ON)

#define STBM_E_PARAM                          (0x0A)           /*API Request called with wrong parameter*/
#define STBM_E_NOT_INITIALIZED                (0x0B)           /*Synchronized Time-Base Manager is not initialized*/
#define STBM_E_PARAM_POINTER                  (0x10)           /*Invalid pointer in parameter list*/
#define STBM_E_SERVICE_DISABLED               (0x12)           /*API disabled by configuration*/
#define STBM_E_PARAM_TIMESTAMP                (0x25)           /*Invalid TimeStamp*/
#define STBM_E_PARAM_USERDATA                 (0x26)           /*Invalid UserData*/

#endif /*#if(STBM_DEV_ERROR_DETECT== STD_ON)*/

/* AUTOSAR SPECIFICATION VERSION */
#define STBM_AR_RELEASE_MAJOR_VERSION         (4u)
#define STBM_AR_RELEASE_MINOR_VERSION         (2u)
#define STBM_AR_RELEASE_REVISION_VERSION      (2u)

/* SOFTWARE VERSION INFORMATION */
#define STBM_SW_MAJOR_VERSION                 (STBM_SOFTWARE_MAJOR_VERSION)
#define STBM_SW_MINOR_VERSION                 (STBM_SOFTWARE_MINOR_VERSION)
#define STBM_SW_PATCH_VERSION                 (STBM_SOFTWARE_PATCH_VERSION)

/* API id assignment */
#define STBM_APIID_INIT                       (0x00)           /*Service id for StbM_Init*/
#define STBM_APIID_MAINFUNCTION               (0x04)           /*Service id for StbM_MainFunction*/
#define STBM_APIID_GETVERSIONINFO             (0x05)           /*Servive id for StbM_GetVersionInfo*/
#define STBM_APIID_GETCURRENTTIME             (0x07)           /*Service id for StbM_GetCurrentTime*/
#define STBM_APIID_GETCURRENTTIMEEXTENDED     (0x08)           /*Service id for StbM_GetCurrentTimeExtended*/
#define STBM_APIID_UPDATEGLOBALTIME           (0x10)           /*Service id for StbM_UpdateGlobalTime*/
#define STBM_APIID_GETRATEDEVIATION           (0x11)           /*Service id for StbM_GetRateDeviation*/
#define STBM_APIID_SETRATECORRECTION          (0x12)           /*Service id for StbM_SetRateCorrection*/
#define STBM_APIID_GETTIMELEAP                (0x13)           /*Service id for StbM_StbM_GetTimeLeap*/
#define STBM_APIID_SETGLOBALTIME              (0x0b)           /*Service id for StbM_SetGlobalTime*/
#define STBM_APIID_SETUSERDATA                (0x0c)           /*Service id for StbM_SetUserData*/
#define STBM_APIID_SETOFFSET                  (0x0d)           /*Service id for StbM_SetOffset*/
#define STBM_APIID_GETOFFSET                  (0x0e)           /*Service id for StbM_GetOffset*/
#define STBM_APIID_BUSSETGLOBALTIME           (0x0f)           /*Service id for StbM_BusSetGlobalTime*/
#define STBM_APIID_GETMASTERCONFIG            (0x1d)           /*Service id for StbM_GetMasterConfig*/
#define STBM_APIID_STARTTIMER                 (0x15)           /*Service id for StbM_StartTimer*/
#define STBM_APIID_GETSYNCTIMERECORDHEAD      (0x16)           /*Service id for StbM_GetSyncTimeRecordHead*/
#define STBM_APIID_GETOFFSETTIMERECORDHEAD    (0x17)           /*Service id for StbM_GetOffsetTimeRecordHead*/
#define STBM_APIID_GETTIMEBASEUPDATECOUNTER   (0x1b)           /*Service id for StbM_GetTimeBaseUpdateCounter*/
#define STBM_APIID_TRIGGERTIMETRANSMISSION    (0x1c)           /*Service id for StbM_TriggerTimeTransmission*/
#define STBM_APIID_GETTIMEBASESTATUS          (0x14)           /*Service id for StbM_GetTimeBaseStatus*/
#define STBM_APIID_GETCURRENTVIRTUALLOCALTIME (0x1e)           /*Service id for StbM_GetCurrentVirtualLocalTime*/
#define STBM_APIID_BUSGETCURRENTTIME          (0x07)           /*Service id for StbM_BusGetCurrentTime*/

/* If DET is ON function definitions for DET error reporting */
#if (STBM_DEV_ERROR_DETECT == STD_ON)

/* For functions with return type as 'void' */
#define STBM_DET_REPORT_ERROR(INSTANCE_ID, API, ERROR)\
            (void)Det_ReportError((STBM_MODULE_ID), (INSTANCE_ID), (API), (ERROR));

/* For functions with return type as 'Std_ReturnType' */
#define STBM_DET_REPORT_ERROR_NOK(INSTANCE_ID, API, ERROR)\
            (void)Det_ReportError((STBM_MODULE_ID), (INSTANCE_ID), (API), (ERROR));

/* For functions with return type as 'uint8' */
#define STBM_DET_REPORT_ERROR_NULL(INSTANCE_ID, API, ERROR)\
            (void)Det_ReportError((STBM_MODULE_ID), (INSTANCE_ID), (API), (ERROR));

#else

/* No DET Code Expansion if Development Detection Error is OFF */

#define STBM_DET_REPORT_ERROR(INSTANCE_ID, API, ERROR)

#define STBM_DET_REPORT_ERROR_NOK(INSTANCE_ID, API, ERROR)

#define STBM_DET_REPORT_ERROR_NULL(INSTANCE_ID, API, ERROR)

#endif

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */



#if (STBM_TIME_NOTIFICATION ==  STD_ON )
/* Local structure to maintain Time Notification information */
typedef struct
{
    /*This represents the function pointer to customer specific notification callback function   */
    Std_ReturnType (*StbMTimeNotificationCallback) (StbM_TimeDiffType DeviationTime);
    /* Identification of a event notification customer.*/
    const StbM_CustomerIdType StbMNotificationCustomerId_u16;
    /* This attribute represents the interval when a GPT Timer shall be started for Time Notification Customer */
    const StbM_TimeStampType StbMTimerStartThreshold_st;
    /* This value represents the CustomerExpireTime */
    StbM_TimeStampType StbMCustomerExpireTime_st;

}StbMNotificationCustomer_tst;

/* Local structure to maintain Pointer to Time Notification Array  */
typedef struct
{
    /* This represents the size of the notification customers configured for the timebase */
    const uint8 StbMNotificationCustomerSize_u8;
    /* Pointer to StbMNotificationCustomer_tst structure */
    StbMNotificationCustomer_tst * StbMNotificationCustomerInfoStruct_ptr;

}StbMNotificationCustomerRecord_tst;

typedef struct
{
    /* This value represents whether GPT Timer is in the RUNNING state */
    boolean IsGptTimerRunning_b;
    /*This value represents the synchronized time-base for which the timer is started*/
    StbM_SynchronizedTimeBaseType SynchronizedTimeBaseIdentifier_u16;
    /* This value represents the event notification customer for which the timer is started.*/
    StbM_CustomerIdType StbMNotificationCustomerId_u16;
    /* This value represents the value of CustomerExpireTime at which the timer will expire  */
    StbM_TimeStampType StbMCustomerLastExpireTime_st;

}StbMNotificationExpiryRecord_tst;

#endif /* #if (STBM_TIME_NOTIFICATION ==  STD_ON ) */


/*This type defines the type that collects the information about a specific time-base provider within the system*/
typedef struct
{
    /*This attribute describes the timeout for the situation that the time synchronization gets lost in the scope of the
            time domain. nanoseconds part*/
    const uint32 SyncLossTimeout_sec_u32;
    /*This represents the minimum delta between the time value in two sync messages for which the sync loss flag is set.
            nanoseconds part*/
    const uint32 SyncLossTimeout_ns_u32;
    /*This represents the maximum allowed positive difference between the current Local Time Base value and a newly
            received Global Time Base value.seconds part*/
    const uint32 StbMTimeLeapFutureThreshold_sec_u32;
    /*This represents the maximum allowed positive difference between the current Local Time Base value and a newly
            received Global Time Base value. seconds part*/
    const uint32 StbMTimeLeapFutureThreshold_ns_u32;
    /*This represents the the maximum allowed negative difference between the current Local Time Base value and a newly
            received Global Time Base value.seconds part*/
    const uint32 StbMTimeLeapPastThreshold_sec_u32;
    /*This represents the the maximum allowed negative difference between the current Local Time Base value and a newly
            received Global Time Base value seconds part*/
    const uint32 StbMTimeLeapPastThreshold_ns_u32;
    /* The parameter defines the initial value for NotificationMask mask, which defines the events for   */
    /*  which the event notification callback function shall be called                                   */
    const uint32 StbMStatusNotificationMask_u32;
    /*This attribute represents the name of the customer specific status notification callback function  */
    /* which shall be called, if a non-masked status event occurs*/
    Std_ReturnType (*StbMStatusNotificationCallback) (StbM_TimeBaseNotificationType EventNotification);
    /* The required number of updates to the Time Base */
    const uint16 StbMClearTimeleapCount_u16;
    /*This attribute describes the maximum allowed absolute value of the rate deviation value to be set*/
    /*by StbM_SetRateCorrection.seconds part*/
    const uint16 StbMMasterRateDeviationMax_u16;
    /*Identification of a synchronized time-base via a unique identifier*/
    const StbM_SynchronizedTimeBaseType SynchronizedTimeBaseIdentifier_u16;
    /*This is the reference to the Synchronized Time-Base this Offset Time-Base is based on*/
    const uint8 OffsetTimeBaseRef_u8;
    /*This attribute describes whether the rate correction value of a Time Base can be set by StbM_SetRateCorrection*/
    const boolean StbMAllowMasterRateCorrection_b;
    /*For postbuild variant of the StbM this parameter has to be set to true for a Global Time Master that may act as*/
    /*a system-wide source of time*/
    const boolean StbMAllowSystemWideGlobalTimeMaster_b;
    /*This parameter shall be set to true for a global time master that acts as a system-wide source of time information*/
    /* with respect to global time.*/
    const boolean IsSystemWideGlobalTimeMaster_b;
    /*This allows for specifying that the timebase shall be stored in the NvRam*/
    const StbM_StoreTimebaseNonVolatileType_ten StoreTimebaseNonVolatile_e;
#if (STBM_TIME_NOTIFICATION ==  STD_ON )
    /* Pointer to Notification customer structure */
    StbMNotificationCustomerRecord_tst * StbMNotificationCustomerRecordStruct_ptr;
#endif
    /*This allows for specifying that StbM is acting as MASTER or SLAVE or GATEWAY*/
    const StbM_StateType_ten StbMState_e;
}StbM_SynchronizedTimeBase_tst;


/*This type defines the type that collects the information about The triggered customer is directly triggered by the
Synchronized Timebase Manager by getting synchronized with the current (global) definition of time and passage of time*/
typedef struct
{
    /*Mandatory reference to synchronized OS ScheduleTable, which will be explicitly synchronized by the StbM*/
    const ScheduleTableType ScheduleTableRef;
    /*Remainder per second in nanoseconds*/
    const uint32 RemPerSecInNs_u32;
    /*Remainder per secondHigh in nanoseconds*/
    const uint32 RemPerSecHighInNs_u32;
    /*The triggering period of the triggered customer, called by the StbM_MainFunction*/
    const uint32 TriggeredCustomerPeriod_u32;
    /*Mandatory reference to the required synchronized time-base*/
    const StbM_SynchronizedTimeBaseType SynchronizedTimeBaseRef_u16;
    /*Schedule Table 1 tick duration in Nanoseconds*/
    const uint32 ScheduleTableRefOneTickDurationNS_u32;
    /*Schedule Table Duration in Nanoseconds*/
    const uint32 ScheduleTableRefDurationNS_u32;
}StbM_TriggeredCustomer_tst;

/*This type defines the type that defines configuration data structure of the StbM module.*/
typedef struct
{
    /*Pointer to StbMSynchronizedTimeBase config structure*/
    const StbM_SynchronizedTimeBase_tst * SynchronizedTimeBase_pcst;
    /*Pointer to StbMTriggeredCustomer config structure*/
    const StbM_TriggeredCustomer_tst * TriggeredCustomer_pcst;
}StbM_ConfigType;

#if (((STBM_GLOBAL_TIME_MASTER != STD_ON) && (STBM_ECUC_RB_RTE_IN_USE == STD_ON)) || (STBM_ECUC_RB_RTE_IN_USE == STD_OFF))

#define STBM_SYSTEM_WIDE_MASTER_DISABLED (0U)
#define STBM_SYSTEM_WIDE_MASTER_ENABLED (1U)

#endif /* #if (((STBM_GLOBAL_TIME_MASTER != STD_ON) && (STBM_ECUC_RB_RTE_IN_USE == STD_ON)) || (STBM_ECUC_RB_RTE_IN_USE == STD_OFF)) */
/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*Module initialization function*/
extern void StbM_Init(const StbM_ConfigType * ConfigPtr);

/*Module version info function*/
#if (STBM_VERSION_INFO_API == STD_ON)
extern void StbM_GetVersionInfo(Std_VersionInfoType * versioninfo);
#endif /*#if (STBM_VERSION_INFO_API == STD_ON) */

/*If RTE is configured in the project and if Global Time Master is configured, then the declaration of the below functions
 * will be done by RTE. Hence need not be declared here. */
#if (((STBM_GLOBAL_TIME_MASTER != STD_ON) && (STBM_ECUC_RB_RTE_IN_USE == STD_ON)) || (STBM_ECUC_RB_RTE_IN_USE == STD_OFF))

/* Indicates if the functionality for a system wide master (e.g. StbM_SetGlobalTime) for a given Time Base is available or not*/
extern Std_ReturnType StbM_GetMasterConfig(StbM_SynchronizedTimeBaseType timeBaseId,
       StbM_MasterConfigType * masterConfig);

/*The function allows the Customers to set the new global time that has to be valid for the system,
 which will be sent to the busses and modify HW registers behind the providers, if supported*/
extern Std_ReturnType StbM_SetGlobalTime(StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_TimeStampType * timeStamp,
        const StbM_UserDataType * userData);

/*The function allows the Customers to set the new user data that has to be valid for the system,
  which will be sent to the busses*/
extern Std_ReturnType StbM_SetUserData(StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_UserDataType * userData );

#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > 0u)
/*The function allows the Customers and the Timebase Provider Modules
  to set the offset time that has to be valid for the system.*/
extern Std_ReturnType StbM_SetOffset(StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_TimeStampType * timeStamp
        ,const StbM_UserDataType * userData
);
#endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > 0u)*/

/*The function allows the Customers to set the new global time that has to be valid for the system,
 which will be sent to the busses and modify HW registers behind the providers, if supported*/
extern Std_ReturnType StbM_UpdateGlobalTime(StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_TimeStampType * timeStamp,
        const StbM_UserDataType * userData);

#endif  /* #if (((STBM_GLOBAL_TIME_MASTER != STD_ON) && (STBM_ECUC_RB_RTE_IN_USE == STD_ON)) || (STBM_ECUC_RB_RTE_IN_USE == STD_OFF)) */

#if (STBM_ECUC_RB_RTE_IN_USE == STD_ON)
    /* If Pure Local Timebase is the only Master timebase, RTE will not provide the declaration of StbM_TriggerTimeTransmission() */
    #if ((STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES == 0u) && (STBM_CFG_NO_OF_SYNC_MASTER_TIMEBASES == 0u))
        /*The function is called by the <Upper Layer> to force the Timesync Modules to transmit the current Time Base again due
        to an incremented StbM_TimeBaseUpdateCounter_au8[timeBaseId]*/
        extern Std_ReturnType StbM_TriggerTimeTransmission(StbM_SynchronizedTimeBaseType timeBaseId);
    #endif /* ((STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES == 0) && (STBM_CFG_NO_OF_SYNC_MASTER_TIMEBASES == 0)) */

    /* If there is no Master Timebase with TimeCorrection, RTE will not provide the declaration of StbM_SetRateCorrection() */
    #if ((STBM_TIME_CORRECTION ==  STD_ON) && (STBM_CFG_NO_OF_MASTER_TC_TIMEBASES == 0u))
        extern Std_ReturnType StbM_SetRateCorrection(StbM_SynchronizedTimeBaseType timeBaseId,
                                                 StbM_RateDeviationType rateDeviation);
    #endif /* #if ((STBM_ECUC_RB_RTE_IN_USE == STD_ON) && (STBM_CFG_NO_OF_MASTER_TC_TIMEBASES == 0)) */
#else
    /*The function is called by the <Upper Layer> to force the Timesync Modules to transmit the current Time Base again due
    to an incremented StbM_TimeBaseUpdateCounter_au8[timeBaseId]*/
    extern Std_ReturnType StbM_TriggerTimeTransmission(StbM_SynchronizedTimeBaseType timeBaseId);

    #if (STBM_TIME_CORRECTION ==  STD_ON )
    extern Std_ReturnType StbM_SetRateCorrection(StbM_SynchronizedTimeBaseType timeBaseId,
                                                 StbM_RateDeviationType rateDeviation);
    #endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
#endif /* (STBM_ECUC_RB_RTE_IN_USE == STD_ON) */

#if (STBM_ECUC_RB_RTE_IN_USE == STD_OFF)

/*StbM Main Function*/
extern void StbM_MainFunction(void);

/* Returns value of Time Leap, if StbMTimeLeapFuture/PastThreshold is exceeded */
extern Std_ReturnType StbM_GetTimeLeap(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeDiffType * timeJump);

/*The function returns a time value (Local Time Base derived from Global Time Base) in extended format*/
#if (STBM_GET_CURRENT_TIME_EXTENDED == STD_ON)
extern Std_ReturnType StbM_GetCurrentTimeExtended
        (StbM_SynchronizedTimeBaseType timeBaseId,
                StbM_TimeStampExtendedType * timeStamp,
                StbM_UserDataType * userData);
#endif /*#if (STBM_GET_CURRENT_TIME_EXTENDED == STD_ON) */

/* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
/*If Time Recording functionality is on, this function allows the Customers to accesses to the recorded snapshot data
  Header of the table belonging to the Synchronized Time Base.*/
extern Std_ReturnType StbM_GetSyncTimeRecordHead(StbM_SynchronizedTimeBaseType timeBaseId,
       StbM_SyncRecordTableHeadType * syncRecordTableHead );
#if (STBM_CFG_NO_OF_OFFSET_TB_WITH_TIMERECORDING > 0)
/*If Time Recording functionality is on, this function allows the Customers to accesses to the recorded snapshot data
  Header of the table belonging to the Offset Time Base.*/
extern Std_ReturnType StbM_GetOffsetTimeRecordHead(StbM_SynchronizedTimeBaseType timeBaseId,
       StbM_OffsetRecordTableHeadType * offsetRecordTableHead );
#endif /* #if STBM_CFG_NO_OF_OFFSET_TB_WITH_TIMERECORDING */
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/

#if (STBM_TIME_CORRECTION ==  STD_ON )
extern Std_ReturnType StbM_GetRateDeviation(StbM_SynchronizedTimeBaseType timeBaseId,
                                                 StbM_RateDeviationType * rateDeviation);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */

#if(STBM_TIME_NOTIFICATION == STD_ON )
extern Std_ReturnType StbM_StartTimer(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_CustomerIdType customerId,
        const StbM_TimeStampType * expireTime);

#endif /* #if (STBM_TIME_NOTIFICATION == STD_ON ) */

#endif  /*(STBM_ECUC_RB_RTE_IN_USE == STD_OFF) */

#ifndef STBM_GETCURRENTTIME_DEFINED

/*The function returns a time value (Local Time Base derived from Global Time Base) in standard format*/
extern Std_ReturnType StbM_GetCurrentTime(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampType * timeStamp,
        StbM_UserDataType * userData);

#endif /* STBM_GETCURRENTTIME_DEFINED */

#ifndef STBM_GETTIMEBASESTATUS_DEFINED

/* The function Returns the detailed status of the Time Base. For Offset Time Bases the status of the Offset Time Base
   itself and the status of the underlying Synchronized Time Base is returned*/
extern Std_ReturnType StbM_GetTimeBaseStatus(
        StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeBaseStatusType * syncTimeBaseStatus,
        StbM_TimeBaseStatusType * offsetTimeBaseStatus );

#endif /* STBM_GETTIMEBASESTATUS_DEFINED */

/*The function allows the Timebase Provider Modules to get the currentoffset time*/
extern Std_ReturnType StbM_GetOffset(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampType * timeStamp
        ,StbM_UserDataType * userData);

/*The function allows the Timebase Provider Modules to forward a new
Global Time to the StbM, which has been received from different busses*/
extern Std_ReturnType StbM_BusSetGlobalTime(StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_TimeStampType * globalTimePtr,
        const StbM_UserDataType * userDataPtr,
        const StbM_MeasurementType * measureDataPtr,
        const StbM_VirtualLocalTimeType * localTimePtr);

/*Allows the Timesync Modules to detect, whether a Time Base should be
transmitted immediately in the subsequent <Bus>TSyn_MainFunction() cycle*/
extern uint8 StbM_GetTimeBaseUpdateCounter(StbM_SynchronizedTimeBaseType timeBaseId);

/*Returns the Virtual Local Time of the referenced Time Base*/
extern Std_ReturnType StbM_GetCurrentVirtualLocalTime(
        StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_VirtualLocalTimeType * localTimePtr);

/* Returns the current Time Tuple, status and User Data of the Time Base*/
extern Std_ReturnType StbM_BusGetCurrentTime(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampType * globalTimePtr,
        StbM_VirtualLocalTimeType * localTimePtr,
        StbM_UserDataType * userData);

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#endif /*STBM_H */

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
