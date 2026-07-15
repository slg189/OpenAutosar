

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
/*The function declarations of RTE interfaced API's will be geberated in the SchM_StbM.h and Rte_StbM.h */
#include "SchM_StbM.h"
#include "Rte_StbM.h"
#endif  /*(STBM_ECUC_RB_RTE_IN_USE == STD_ON) */

#if(STBM_DEV_ERROR_DETECT == STD_ON) /*Det included when Det is enabled in StbM*/
#include "Det.h"
/*#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#elif (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != STBM_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif*//*#if(!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))*/

#endif /* #if(STBM_DEV_ERROR_DETECT == STD_ON) */

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

/* Precompile check for tiggered customer configuration */
#if STBM_CFG_TRIGGERED_CUSTOMER

#define STBM_START_SEC_VAR_CLEARED_32
#include "StbM_MemMap.h"
uint32 StbM_TrigCusPeriod_u32[STBM_CFG_TRIGGERED_CUSTOMER];
#define STBM_STOP_SEC_VAR_CLEARED_32
#include "StbM_MemMap.h"

/*
 **********************************************************************************************************************
 * Local functions decalrations
 **********************************************************************************************************************
 */

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

static void StbM_Triggered_Customer_Call( void );
static uint32 StbM_Get_Delta_ns_calculation(
        uint32 ST_duration_ns,
        uint32 Time_GCT,
        uint32 RemainderOAW,
        uint32 Rem_ns_u32
);
static uint32 StbM_Get_Delta_ns( const StbM_TimeStampType * timeStampPtr,
        uint32 ST_duration_ns,
        uint32 Rem_ns_per_sec_u32,
        uint32 Rem_ns_per_secHigh_u32);

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function for the remainder calculation */
static uint32 StbM_Get_Delta_ns_calculation(
        uint32 ST_duration_ns,
        uint32 Time_GCT,
        uint32 RemainderOAW,
        uint32 Rem_ns_u32
)
{
    uint32 lUnit_Size_u32;
    uint32 lRem_per_unit_u32;
    uint32 lCheck_u32;
    uint32 lNum_old_units_u32;
    uint32 lNum_unit_sets_u32;
    uint32 lNum_sec_rem_u32;
    uint32 lRem_per_unit_prev_u32;
    uint32 lmax_unit_num_u32;
    uint32 lbreak_var_u32;
    uint32 ltotal_time_consider_u32;
	/* local variable to hold the return value */
	uint32 lRem_ns_u32;

	/* Initialize the return value to default remainder value */
	lRem_ns_u32 = Rem_ns_u32;

    if((Time_GCT) != STBM_ZERO)
    {
        /*  Initially we'll start with full count of time value ( seconds or secondsHigh depending on the call ) */
        ltotal_time_consider_u32 = Time_GCT;

        /*  We need not do the calculation for seconds or secondsHigh part if there is no remainder per second or secondHigh */
        if(RemainderOAW != STBM_ZERO)
        {
            do /*  Loop 1 */
            {
                /*  We break out of this loop 1 when : */
                /*  1. we dont have any ltotal_time_consider_u32 remaining for which the remainder has to be calculated */

                /*  Future enhancement : */
                /*  Currently we are reiterating through the loop, in future this will be replaced by a 2D array as below, */
                /*  RemainderArray[UnitSize][Remainder], so that we don't have to do the calculation again and again. */

                lbreak_var_u32     = STBM_ONE;
                lUnit_Size_u32     = STBM_ONE;
                lRem_per_unit_u32  = RemainderOAW;
                lNum_old_units_u32 = lUnit_Size_u32;


                do /*  Loop 2 */
                {
                    /*  We break out of this loop 2 when : */
                    /*  1. The remainder of the unit is zero */
                    /*  2. We are unable to make any more units now */

                    /*  Check if the remainder from each unit is able to complete ST duration or not */
                    lCheck_u32 = ST_duration_ns % lRem_per_unit_u32;

                    if ( lCheck_u32 == STBM_ZERO )
                    {
                        /*  yes, my ST duration is completely occupied by the remainder from the unit */

                        /*  number of old units that we can combine to form new unit */
                        lNum_old_units_u32     = ( ST_duration_ns / lRem_per_unit_u32 );
                        lRem_per_unit_prev_u32 = lRem_per_unit_u32;

                        /*  New remainder per unit will be zero as the remaidner completely divides the ST duration */
                        lRem_per_unit_u32      = STBM_ZERO;
                        lbreak_var_u32         = STBM_ZERO;
                    }
                    else
                    {
                        /*  No, my ST duration was unable to completely occupy rhe ST duration */
                        /*  Thus, (New unit size) = (1 ST Duration) + ( lRem_per_unit_u32 ) */

                        lNum_old_units_u32     = ( ST_duration_ns / lRem_per_unit_u32 ) + STBM_ONE;
                        lRem_per_unit_prev_u32 = lRem_per_unit_u32;
                        lRem_per_unit_u32      = lRem_per_unit_u32 - lCheck_u32;
                    }

                    /*  Maximum how many previous units i can accomodate in my total secondsHigh duration */
                    lmax_unit_num_u32         = ltotal_time_consider_u32 / lUnit_Size_u32;

                    /*  Check if the number of old units that we need to form a new unit is less than or greater than,
                        the maximum units that can be accomodated in the total second duration */
                    if(lNum_old_units_u32 > lmax_unit_num_u32)
                    {
                        /*  Old Unit number that we need to make new unit is more than the maximum units
                            that we can accomodate in our entire seconds duration
                            Thus we need to break here and continue with details from previous iteration */

                        /*  Restore the remainder value from previous iteration and break from the code */
                        lRem_per_unit_u32      = lRem_per_unit_prev_u32;
                        lbreak_var_u32         = STBM_ZERO;
                    }
                    else
                    {

                        /*  we can succesfully create new unit, by combining <<lNum_old_units_u32>> of old units */
                        lUnit_Size_u32  = lUnit_Size_u32 * lNum_old_units_u32;
                    }

                    /*  total number of sets that we can form with the unit size calculated in the current iteration */
                    lNum_unit_sets_u32 = ltotal_time_consider_u32 / lUnit_Size_u32;

                    /*  SecondsHigh that are still remaining whose remainder is yet to be calculated */
                    lNum_sec_rem_u32   = ltotal_time_consider_u32 % lUnit_Size_u32; /*  lNum_sec_rem_u32 -> lNum_secHigh_rem_u32 */

                } while ( lbreak_var_u32 == STBM_ONE );

                /*  We'll update the total seconds count for which the remainder has to be calculated
                    with the remainder value from previous calculation */
                ltotal_time_consider_u32 = lNum_sec_rem_u32;

                /*  Update the remainder variable with each iteration */
                lRem_ns_u32              = (Rem_ns_u32 + (lNum_unit_sets_u32 * lRem_per_unit_u32)) % ST_duration_ns;

            } while ( lNum_sec_rem_u32 != STBM_ZERO ); /*  lNum_sec_rem_u32 -> lNum_secHigh_rem_u32 */
        }
    }
    else
    {
        /*  Do Nothing */
    }
    return lRem_ns_u32;
}
#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function for the remainder calculation */
static uint32 StbM_Get_Delta_ns( const StbM_TimeStampType * timeStampPtr,
        uint32 ST_duration_ns,
        uint32 Rem_ns_per_sec_u32,
        uint32 Rem_ns_per_secHigh_u32)
{
    uint32 lRem_ns_u32;

    /*  Calculate remainder for nanoseconds part of timeStampPtr */
    lRem_ns_u32 = (timeStampPtr->nanoseconds) % (ST_duration_ns);

    /*  ************************************* Seconds High Calculation ************************************************/
    lRem_ns_u32 = StbM_Get_Delta_ns_calculation(ST_duration_ns,
            timeStampPtr->secondsHi,
            Rem_ns_per_secHigh_u32,
            lRem_ns_u32);

    /*  ************************************* Seconds Calculation *****************************************************/
    lRem_ns_u32 = StbM_Get_Delta_ns_calculation(ST_duration_ns,
            timeStampPtr->seconds,
            Rem_ns_per_sec_u32,
            lRem_ns_u32);

    return lRem_ns_u32;
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_Triggered_Customer_Call
Syntax            : void StbM_Triggered_Customer_Call( void )
Service ID[hex]   : None
Sync/Async        :
Reentrancy        :
Parameters (in)   : None
Parameters (inout): None
Parameters (out)  : None
Return value      : None
Description       : Local function to call the Triggered customer
 **********************************************************************************************************************/
static void StbM_Triggered_Customer_Call(void)
{
    /* Local Variable declaration */
    StatusType lStbMRetVal_u8;
    ScheduleTableStatusType lScheduleStatus_en;
    uint8 lCount_u8;

    StbM_TimeStampType lTimeStamp_st;
    StbM_UserDataType lUserData_st;
    uint32 lSyncTime_u32;
    TickType lSyncTicks_uo;
    StbM_SynchronizedTimeBaseType lTimebaseID_u16;
    ScheduleTableType lSTInUse_u16;
    uint32 lSTInUseOneTickDurationNs_u32;
    uint32 lSTInUseDurationNs_u32;
    uint32 lRemNsForSec_u32;
    uint32 lRemNsForSecHigh_u32;

    /* Local Variable Initialization */
    lStbMRetVal_u8 = E_NOT_OK;

    for(lCount_u8=STBM_ZERO;lCount_u8<STBM_CFG_TRIGGERED_CUSTOMER;lCount_u8++)
    {
        /* Decrement the period, each time StbM_MainFunction is called */
        StbM_TrigCusPeriod_u32[lCount_u8] = StbM_TrigCusPeriod_u32[lCount_u8] - STBM_MAINFUNC_PERIOD;

        /* [SWS_StbM_00107]  If OS is configured as triggered customer, the function StbM_MainFunction shall synchronize
           the referenced OS ScheduleTable. */
        /* [SWS_StbM_00084]  Customers of type triggered customer shall be invoked periodically by the Synchronized
           Time-Base Manager */
        /* [SWS_StbM_00093]  "The triggering period STBM_TRIGGERED_CUSTOMER_PERIOD shall be configurable for each
           triggered customer Based on the configuration, the Synchronized Time-Base Manager synchronizes the OS counter
		   value of the associated OS ScheduleTable */

        if(StbM_TrigCusPeriod_u32[lCount_u8]== STBM_ZERO)
        {
            /* Reset the Customer Period to their original value */
            StbM_TrigCusPeriod_u32[lCount_u8] =
                    StbM_Cfg_TriggeredCustomerArray_ast[lCount_u8].TriggeredCustomerPeriod_u32;


            /*  Getting the configured timeBaseId for the triggered customer */
            lTimebaseID_u16 = StbM_Cfg_TriggeredCustomerArray_ast[lCount_u8].SynchronizedTimeBaseRef_u16;

            /* [SWS_StbM_00077]The Synchronized Time-Base Manager shall synchronize OS ScheduleTables only when the
                associated synchronized time base is synchronized. */

            if(StbM_Check_Bit
                  (StbM_GlobalTimeTupleArray_ast[StbM_Index_au8[lTimebaseID_u16]].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT))
            {
                /*  Get the ID of Schedule Table in use */
                lSTInUse_u16 = StbM_Cfg_TriggeredCustomerArray_ast[lCount_u8].ScheduleTableRef;

                /* Get the status of the schedule table */
                lStbMRetVal_u8 = GetScheduleTableStatus(lSTInUse_u16,&lScheduleStatus_en);
                /* [SWS_StbM_00092]   "The Synchronized Time-Base Manager shall synchronize only OS ScheduleTables that
                 are in one of the states WAITING, RUNNING or RUNNING_SYNCHRONOUS.
                 This implies that the Synchronized Time-Base Manager shall check the OS for the status of the OS
                 ScheduleTable before performing the synchronization." */
                if(lStbMRetVal_u8 == E_OK)
                {
                    if((lScheduleStatus_en == SCHEDULETABLE_WAITING) || (lScheduleStatus_en == SCHEDULETABLE_RUNNING) ||
                            (lScheduleStatus_en == SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS))
                    {
                        /*  Get the Schedule table duration in nanoseconds */
                        lSTInUseDurationNs_u32   =
                                StbM_Cfg_TriggeredCustomerArray_ast[lCount_u8].ScheduleTableRefDurationNS_u32;

                        /*  Get 1 tick duration for referenced Schedule Table */
                        lSTInUseOneTickDurationNs_u32 =
                                StbM_Cfg_TriggeredCustomerArray_ast[lCount_u8].ScheduleTableRefOneTickDurationNS_u32;

                        SchM_Enter_StbM_Sync_OsResource(); /*Lock the OS resource lock*/

                        /*  Getting the timestamp */
                        (void)StbM_GetCurrentTime(lTimebaseID_u16,&lTimeStamp_st,&lUserData_st);

                        /*  Get the remainder in nanoseconds for 1second and 1secondHigh, Already calculated by OAW */
                        lRemNsForSec_u32     = StbM_Cfg_TriggeredCustomerArray_ast[lCount_u8].RemPerSecInNs_u32;
                        lRemNsForSecHigh_u32 = StbM_Cfg_TriggeredCustomerArray_ast[lCount_u8].RemPerSecHighInNs_u32;

                        /*  Get remainder time in terms of nanoseconds */
                        lSyncTime_u32 = StbM_Get_Delta_ns(&lTimeStamp_st,
                                lSTInUseDurationNs_u32,
                                lRemNsForSec_u32,
                                lRemNsForSecHigh_u32);

                        /*  lSyncTime_u32 in ticks */
                        lSyncTicks_uo = (TickType)(lSyncTime_u32/lSTInUseOneTickDurationNs_u32);

                        (void) SyncScheduleTable(lSTInUse_u16, lSyncTicks_uo);

                        SchM_Exit_StbM_Sync_OsResource(); /* Un -lock the OS resource lock*/
                    }
                    else
                    {
                        /* do nothing */
                    }
                }
            }
            else
            {
                /* do nothing if the STBM state is SYNC */
            }
        }
        else
        {
             /* do nothing */
        }
    }
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#endif /* endif STBM_CFG_TRIGGERED_CUSTOMER */

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_UserDataUpdate
Syntax            : void StbM_UserDataUpdate( uint8 Index_u8,
                                         const StbM_UserDataType * userData )
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : index_u8 - index of timebase
                    userData - New user data
Parameters (inout): None
Parameters (out)  : None
Return value      : void
Description       : Local function is used to update the userdata bytes depending on the length of userdata length
 **********************************************************************************************************************/
void StbM_UserDataUpdate( uint8 Index_u8,
                                         const StbM_UserDataType * userData )
{
    /*Read the userdata length*/
    (StbM_UserDataArray_ast + Index_u8)->userDataLength = userData->userDataLength;

    /*Check the userdata length is between 0 to 3 range*/
    if(userData->userDataLength > STBM_ZERO)
    {
        /*The length is greater than one means at least one byte will be updated and rest will remain same as previous*/
        (StbM_UserDataArray_ast+Index_u8)->userByte0 = userData->userByte0;

        /*Check if the length is greater than one*/
        if(userData->userDataLength > STBM_ONE)
        {
            /*The length is greater than one means two bytes will be updated and rest will remain same as previous*/
            (StbM_UserDataArray_ast + Index_u8)->userByte1 = userData->userByte1;

            /*Check if the length is greater than two*/
            if(userData->userDataLength > STBM_TWO)
            {
                /*The length is greater than one means three bytes will be updated*/
                (StbM_UserDataArray_ast + Index_u8)->userByte2 = userData->userByte2;
            }
            else
            {
                 /*The userdata length is not greater than two so only two bytes of userdata will be set and rest will remain same as previous*/
            }
        }
        else
        {
            /*The userdata length is not greater than one so only one byte of userdata will be set and rest will remain same as previous*/
        }
    }
    else
    {
        /* UserDataLength is zero so no userdata bytes are updated and will remain same as previous*/
    }

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
*Service name      : StbM_Rb_GlobalTimeTupleUpdate
*Syntax            : Std_ReturnType StbM_Rb_GlobalTimeTupleUpdate
                     (StbM_SynchronizedTimeBaseType timeBaseId,
                     StbM_VirtualLocalTimeType virtualLocalTimeNow,
                     StbM_VirtualLocalTimeType virtualLocalTimeDiff)
*Service ID[hex]   : None
*Sync/Async        : None
*Reentrancy        : None
*Parameters (in)   : timeBaseId  - time base reference
*                    virtualLocalTimeDiff - VirtualLocalTime Difference
*Parameters (inout): None
*Parameters (out)  : None
*Return value      : None
*Description       : API to update the Main Time Tuple for Global Time
***********************************************************************************************************************/
 void StbM_Rb_GlobalTimeTupleUpdate
(StbM_SynchronizedTimeBaseType timeBaseId,
 StbM_VirtualLocalTimeType virtualLocalTimeNow,
 StbM_VirtualLocalTimeType virtualLocalTimeDiff)
{
    /* Local Variable declaration */
    StbM_TimeStampType lGlobalTimeStamp_st;
    uint8 lIndex_u8;

    /* Local Variable Initialization */
    lIndex_u8 = StbM_Index_au8[timeBaseId];

    /* Call the API to Get the current Global Time value */
    StbM_GetCurrentTime_Synchronised_Timebase(timeBaseId,virtualLocalTimeNow,virtualLocalTimeDiff,&lGlobalTimeStamp_st);

    /*  Enter Resource lock */
    SchM_Enter_StbM_Time_Source();

    /* Update the Global time array with the new Global time value */
    StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds    = lGlobalTimeStamp_st.nanoseconds;
    StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds        = lGlobalTimeStamp_st.seconds;
    StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi      = lGlobalTimeStamp_st.secondsHi;
    StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus = lGlobalTimeStamp_st.timeBaseStatus;

    /* Update the Virtual Local time in the Time Tuple array to make a tuple with Global Time */
    StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8].nanosecondsHi    = virtualLocalTimeNow.nanosecondsHi;
    StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8].nanosecondsLo    = virtualLocalTimeNow.nanosecondsLo;

    /*  Exit Resource lock */
    SchM_Exit_StbM_Time_Source();

}
#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/********************************************************************************************************************
 * Function name     : StbM_MainFunction
 * Syntax            : void StbM_MainFunction( void )
 * Service ID[hex]   : 0x04
 * Sync/Async        :
 * Reentrancy        :
 * Parameters (in)   : None
 * Parameters (inout): None
 * Parameters (out)  : None
 * Return value      : None
 * Description       : This function will be called cyclically by a task body provided by the BSW Schedule
                     It will invoke the triggered customers and synchronize the referenced OS ScheduleTables.
 ********************************************************************************************************************/
/* [SWS_StbM_00052] */
void StbM_MainFunction(void)
{
    /* Local Variable declaration */
    StbM_TimeStampType lCurrentLocalTime_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lVirtualLocalTimeDiff_st  = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocalLast_st = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocalNow_st  = {STBM_ZERO,STBM_ZERO};
    StbM_SynchronizedTimeBaseType lConfig_TimeBaseId_u16;
    uint8 lCount_u8 ;
    uint8 lIndex_u8;

#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
    uint16 lrecordTableBlockCountCurrent_u16;
    uint16 lRecordTableBlockCount_u16;
    uint16 lrecordTableBlockCountTemp_u16;
    const StbM_SyncRecordTableBlockType * lsyncTimeBaseBlockRecordArray;
    const StbM_OffsetRecordTableBlockType * loffsetTimeBaseBlockRecordArray;
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/
#if ( STBM_TIME_NOTIFICATION == STD_ON )
    uint8 lCustomerCount_u8 ;
    StbM_CustomerIdType lCustomerId_u16;
    StbMNotificationCustomerRecord_tst * lStbMTimeNotification_ptr;
#endif /* #if (STBM_TIME_NOTIFICATION ==  STD_ON )*/
#if (STBM_NVM_ENABLE ==  STD_ON )
    uint8 lNvm_Index_u8;
#endif/* #if (STBM_NVM_ENABLE ==  STD_ON )*/

	/* Local Variable Initialization */
#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
    lrecordTableBlockCountCurrent_u16 = STBM_ZERO;
    lRecordTableBlockCount_u16     = STBM_ZERO;
    lrecordTableBlockCountTemp_u16 = STBM_ZERO;
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/

	lConfig_TimeBaseId_u16 = STBM_ZERO;

	/* Check whether the module is initialised or not*/
    if(StbM_InitState_b != FALSE)
    {
        for(lCount_u8 = STBM_ZERO;lCount_u8<STBM_CFG_NUM_OF_TIMEBASES;lCount_u8++)
        {
            /* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
            lrecordTableBlockCountCurrent_u16 = StbM_RecordTableBlockCountCurrent_ast[lCount_u8];
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/

            /* Copy the configured timebase ID into a local array */
            lConfig_TimeBaseId_u16 =
                    StbM_Cfg_SynchronizedTimeBaseArray_ast[lCount_u8].SynchronizedTimeBaseIdentifier_u16;

            /* Check if the TimeBase is Synchronized or Pure Local TimeBase */
            if((StbM_Rb_IsSyncTimeBase(lConfig_TimeBaseId_u16)) || (StbM_Rb_IsPureLocalTimeBase(lConfig_TimeBaseId_u16)))
            {
                /* Fetch the index of the current timebase ID */
                lIndex_u8 = StbM_Index_au8[lConfig_TimeBaseId_u16];

                /*  Lock the resource */
                SchM_Enter_StbM_Sync_OsResource();

                /*[SWS_StbM_00433]
	          The Main Time Tuple shall only be updated
	          after setting a new Global Time or a new Rate Correction value by the application
	          after obtaining a Received Time Tuple (i.e., a new Time Tuple) from a Timesync Module
	          after the Offset Correction By Rate Adaption interval (see [SWS_StbM_00353])
	          However, the Main Time Tuple may be updated if there has been no update for more than 3s.

	          [SWS_StbM_00436]
              Although the retrieved value of the Virtual Local Time and the time which is returned by
              StbM_GetCurrentTime(), StbM_GetCurrentTimeExtended(), and StbM_BusGetCurrentTime() form a
              new Time Tuple [TL;TV], this tuple shall only replace the Main Time Tuple if the requirements
              as specified in [SWS_StbM_00433] are met.*/

                /* Load the previous VLT*/
                lTimeStampVLocalLast_st = StbM_VirtualLocalTimeArray_ast[lIndex_u8];

                /*Call the StbM_GetCurrentVirtualLocalTime_Internal to read the current Virtual Local Time */
                (void)StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocalNow_st,STBM_NO_COUNTER_UPDATE);

                /*Check if the Virtual time value difference */
                lVirtualLocalTimeDiff_st =
                        StbM_Rb_VirtualLocalTimeDifference(lTimeStampVLocalLast_st,lTimeStampVLocalNow_st);

                /* Convert the time value from StbM_VirtualLocalTimeType to StbM_TimeStampType format*/
                StbM_Rb_ConvertVirtualLocalTimeToTimestampType(lVirtualLocalTimeDiff_st,&lCurrentLocalTime_st);

                /*If seconds is greater than 3s then update the Main time Tuple*/
                if(lCurrentLocalTime_st.seconds >= STBM_MAX_SEC_LIMIT)
                {

                    /*Call the StbM_GetCurrentVirtualLocalTime_Internal to Update the counter value to note the VLT update*/
                    (void)StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocalNow_st,STBM_COUNTER_UPDATE);

                    /* Update the Virtual  Local time array with the new Virtual Local time value */
                    StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsHi    = lTimeStampVLocalNow_st.nanosecondsHi;
                    StbM_VirtualLocalTimeArray_ast[lIndex_u8].nanosecondsLo    = lTimeStampVLocalNow_st.nanosecondsLo;

                    /*Calculate Virtual time value difference with the VLT of the stored counter value */
                    lVirtualLocalTimeDiff_st =
                            StbM_Rb_VirtualLocalTimeDifference(lTimeStampVLocalLast_st,lTimeStampVLocalNow_st);

                    /*Call the API to update the Global Time Tuple*/
                    StbM_Rb_GlobalTimeTupleUpdate
                                      (lConfig_TimeBaseId_u16,lTimeStampVLocalNow_st,lVirtualLocalTimeDiff_st);

#if (STBM_NVM_ENABLE ==  STD_ON )
                    if (StbM_Cfg_SynchronizedTimeBaseArray_ast[lCount_u8].StoreTimebaseNonVolatile_e
                                                                                             == STBM_NVM_STORAGE_AT_SHUTDOWN)
                    {
                        /* Copy the index of the NvM configured timebase ID*/
                        lNvm_Index_u8 = StbM_Nvm_Index_au8[lConfig_TimeBaseId_u16];

                        /* Overflow check for the time value and updating the time value */
                        StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8]
                                                     = StbM_GlobalTimeTupleArray_ast[lCount_u8];
                    }
                    else
                    {
                        /* do nothing */
                    }
#endif/* #if (STBM_NVM_ENABLE ==  STD_ON )*/

                }
                else
                {
                    /* Since the seconds is not greater than 3 seconds Time value is not updated  */
                }

                /*  Un-Lock the resource */
                SchM_Exit_StbM_Sync_OsResource();

                /* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )

                /* [SWS_StbM_00317] If StbMTimeRecordingSupport is set to TRUE, the StbM notifies the Application by calling
                StbM_SyncTimeRecordBlockCallback() in the next StbM_MainFunction() call cycle block by block for all
                available blocks, starting with block index 0 (zero), if all elements of each block belonging to the
                Synchronized Time Base Record Table have been updated. */
                lRecordTableBlockCount_u16 = StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].RecordTableBlockCount_u16;

                /* Check if time recording feature support is on for this timebase ID */
                if(lRecordTableBlockCount_u16 != STBM_ZERO)
                {
                    lsyncTimeBaseBlockRecordArray =
                            StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].SynchronizedTimeBaseBlockRecordArray_ptr;

                    if( lrecordTableBlockCountCurrent_u16 == lRecordTableBlockCount_u16)
                    {
                        /* [SWS_StbM_00382] If StbMTimeRecordingSupport is set to TRUE, StbMSyncTimeRecordTableBlockCount shall be
                        used to increase the number of blocks of the Synchronized Time Base Record Table. */
                        for (lrecordTableBlockCountTemp_u16 = STBM_ZERO;
                             lrecordTableBlockCountTemp_u16 < lRecordTableBlockCount_u16;
                             lrecordTableBlockCountTemp_u16++)
                        {
                            (void)StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].StbMSyncTimeRecordBlockCallback
                            (lsyncTimeBaseBlockRecordArray + lrecordTableBlockCountTemp_u16) ;
                        }

                        // Do we have to apply lock over here?
                        /* [SWS_StbM_00312] If StbMTimeRecordingSupport is set to TRUE, on an invocation of StbM_BusSetGlobalTime()
                        the StbM shall update all elements of the block of the recording table. If all blocks have been
                        written and no notification via StbM_SyncTimeRecordBlockCallback()or StbM_OffsetTimeRecordBlockCallback()
                        did release all blocks with their elements, the StbM shall again overwrite the Block index 0 (zero)
                        with the incoming measurement data. */
                        StbM_RecordTableBlockCountCurrent_ast[lCount_u8] = STBM_ZERO;
                    }
                }
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/
            }
            else
            {
                /* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )
                /* [SWS_StbM_00318] If StbMTimeRecordingSupport is set to TRUE, the StbM notifies the Application by calling
                StbM_OffsetTimeRecordBlockCallback() in the next StbM_MainFunction() call cycle block by block for all
                available blocks, starting with block index 0 (zero), if all elements of the block belonging to the
                Offset Time Base Record Table have been updated. */

                lRecordTableBlockCount_u16 = StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].RecordTableBlockCount_u16;

                /* Check if time recording feature support is on for this timebase ID */
                if(lRecordTableBlockCount_u16 != STBM_ZERO)
                {
                    loffsetTimeBaseBlockRecordArray   =
                            StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].OffsetTimeBaseBlockRecordArray_ptr;

                    if( lrecordTableBlockCountCurrent_u16 == lRecordTableBlockCount_u16)
                    {
                        /* [SWS_StbM_00383] If StbMTimeRecordingSupport is set to TRUE, StbMOffsetTimeRecordTableBlockCount
                        shall be used to increase the number of blocks of the Offset Time Base Record Table. */
                        for (lrecordTableBlockCountTemp_u16 = STBM_ZERO;
                                lrecordTableBlockCountTemp_u16 < lRecordTableBlockCount_u16;
                                lrecordTableBlockCountTemp_u16++)
                        {
                            (void)StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].StbMOffsetTimeRecordBlockCallback
                            (loffsetTimeBaseBlockRecordArray + lrecordTableBlockCountTemp_u16) ;

                        }

                        // Do we have to apply lock over here?
                        /* [SWS_StbM_00312] If StbMTimeRecordingSupport is set to TRUE, on an invocation of StbM_BusSetGlobalTime()
                        the StbM shall update all elements of the block of the recording table. If all blocks have been
                        written and no notification via StbM_SyncTimeRecordBlockCallback()or StbM_OffsetTimeRecordBlockCallback()
                        did release all blocks with their elements, the StbM shall again overwrite the Block index 0 (zero)
                        with the incoming measurement data. */
                        StbM_RecordTableBlockCountCurrent_ast[lCount_u8] = STBM_ZERO;
                    }
                }
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/
            }
#if (STBM_TIME_NOTIFICATION ==  STD_ON )

            lStbMTimeNotification_ptr =
                    StbM_Cfg_SynchronizedTimeBaseArray_ast[lCount_u8].StbMNotificationCustomerRecordStruct_ptr ;

            /* [SWS_StbM_00335] The StbM shall cyclically monitor the Time Bases and, if needed, re-adjust in its
       StbM_MainFunction the expiration time CustomerTimerExpireTime for the currently active Time Notification Customers.*/

            if(lStbMTimeNotification_ptr != NULL_PTR)
            {
                for(lCustomerCount_u8 = STBM_ZERO ;
                        lCustomerCount_u8 < lStbMTimeNotification_ptr -> StbMNotificationCustomerSize_u8 ;
                        lCustomerCount_u8++)
                {
                    lCustomerId_u16 =
                    (lStbMTimeNotification_ptr -> StbMNotificationCustomerInfoStruct_ptr + lCustomerCount_u8 ) ->
                    StbMNotificationCustomerId_u16 ;

                    StbM_SetExpiryTime((StbM_SynchronizedTimeBaseType)lConfig_TimeBaseId_u16,lCustomerId_u16);
                }
            }
            else
            {
                /* QAC - do nothing */
            }
#endif /* #if (STBM_TIME_NOTIFICATION ==  STD_ON )*/
        }
#if(STBM_CFG_TRIGGERED_CUSTOMER > STBM_ZERO)
        /* Call triggered customer */
        StbM_Triggered_Customer_Call();
#endif /*#if(StbM_CFG_TRIGGERED_CUSTOMER > STBM_ZERO)*/
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised */
        STBM_DET_REPORT_ERROR(STBM_INSTANCE_ID,STBM_APIID_MAINFUNCTION,STBM_E_NOT_INITIALIZED)
    }
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
 * Service name        : StbM_SetUserData
 * Syntax              : Std_ReturnType StbM_SetUserData( StbM_SynchronizedTimeBaseType timeBaseId,
 *                       const StbM_UserDataType* userData )
 * Service ID[hex]     : 0x0c
 * Sync/Async          : Synchronous
 * Reentrancy          : Non Reentrant
 * Parameters (in)     : timeBaseId  - time base reference
 *                       userData    - New user data
 * Parameters (inout)  : None
 * Parameters (out)    : None
 * Return value        : Std_ReturnType E_OK: successful E_NOT_OK: failed
 * Description         : Allows the Customers to set the new user data that has to be valid for the system,
 *                       which will be sent to the busses.
 * ******************************************************************************************************************/
/* [SWS_StbM_00218] */
Std_ReturnType StbM_SetUserData(StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_UserDataType * userData)
{
    /* Local Variable declaration */
    Std_ReturnType lRet_Val_u8; /* Return value of this API StbM_SetUserData() */
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
            if(StbM_Rb_IsUserDataValid(userData) != STBM_USER_DATA_NULL_PTR)
            {
                /* Check if the userDataLength passed is greater than 3 */
                if(StbM_Rb_IsUserDataValid(userData) != STBM_USER_DATA_INVALID)
                {
                    /*  Lock the resource */
                    SchM_Enter_StbM_Time_Source();

                    /* [SWS_StbM_00398]For Pure Local Time Bases StbM_GetCurrentTime() and StbM_GetCurrentTimeExtended() shall
            return the User Data as set by StbM_SetGlobalTime(), StbM_UpdateGlobalTime() or StbM_SetUserData() by the
            local Pure Local Time Master  */

                    if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].IsSystemWideGlobalTimeMaster_b == TRUE)
                    {
                        /* Set the user data with the new data received based on the length provided by user as structure attribute */
                        StbM_UserDataUpdate(lIndex_u8, userData);
                    }
                    else
                    {
                        /* Do nothing - QAC */
                    }

                    /*  Un-Lock the resource */
                    SchM_Exit_StbM_Time_Source();

                    lRet_Val_u8 = E_OK;
                }
                else
                {
                    /* [SWS_StbM_00457] If the switch StbMDevErrorDetect is set to TRUE, StbM_SetUserData()
                     * shall report to DET the development error STBM_E_PARAM_USERDATA, if called with an invalid
                     * value of parameter userData, i.e., userDataLength > 3. */
                    /* If userDataLength is invalid(greater than 3) then report error and exit the function */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETUSERDATA,STBM_E_PARAM_USERDATA)
                }
            }
            else
            {
                /* if Config is NULL pointer then report error and exit the function */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETUSERDATA,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00219]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_SetUserData()
            shall report to DET the development error STBM_E_PARAM, if called with an invalid parameter timeBaseID */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETUSERDATA,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETUSERDATA,STBM_E_NOT_INITIALIZED)
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
