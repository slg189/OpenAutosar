

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
 * Variables
 **********************************************************************************************************************
*/



#define STBM_START_SEC_VAR_CLEARED_8
#include "StbM_MemMap.h"
uint8 StbM_TimeBaseUpdateCounter_au8[STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_8
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_TriggerTimeTransmission
Syntax            : Std_ReturnType StbM_TriggerTimeTransmission( StbM_SynchronizedTimeBaseType timeBaseId )
Service ID[hex]   : 0x1c
Sync/Async        : Synchronous
Reentrancy        : Non Reentrant
Parameters (in)   : timeBaseId
Parameters (inout): None
Parameters (out)  : None
Return value      : Std_ReturnType
Description       :Called by the <Upper Layer> to force the Timesync Modules to transmit the current Time Base again due
                   to an incremented timeBaseUpdateCounter[timeBaseId]
 **********************************************************************************************************************/
/* [SWS_StbM_00346] */
Std_ReturnType StbM_TriggerTimeTransmission(StbM_SynchronizedTimeBaseType timeBaseId)
{
    /* Local Variable declaration */
    Std_ReturnType lRet_Val_u8;
    uint8 lIndex_u8;

    /* Local Variable Initialization */
    lRet_Val_u8 = E_NOT_OK;

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBase is Synchronized or Offset TimeBase */
        if((StbM_Rb_IsSyncTimeBase(timeBaseId)) || (StbM_Rb_IsOffsetTimeBase(timeBaseId)))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /*[SWS_StbM_00350]On a valid invocation of StbM_SetGlobalTime(), StbM_BusSetGlobalTime() or StbM_TriggerTimeTransmission()
      the StbM shall increment the timeBaseUpdateCounter of the corresponding Time Base by 1 (one).
      At 255 it shall wrap around to 0.*/

            (*(StbM_TimeBaseUpdateCounter_au8 + lIndex_u8))++ ;

            lRet_Val_u8 = E_OK ;
        }
        else
        {
            /*[SWS_StbM_00349]If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_TriggerTimeTransmission()
                   shall report to DET the development error STBM_E_PARAM, if called with a parameter timeBaseID, which  is not
                   configured or refers to a Pure Local Time Base or is within the reserved value range. */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_TRIGGERTIMETRANSMISSION,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_TRIGGERTIMETRANSMISSION,STBM_E_NOT_INITIALIZED)
    }

    return (lRet_Val_u8);
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_GetTimeBaseUpdateCounter
Syntax            : uint8* StbM_GetTimeBaseUpdateCounter( StbM_SynchronizedTimeBaseType timeBaseId )
Service ID[hex]   : 0x1b
Sync/Async        : Synchronous
Reentrancy        : Non Reentrant
Parameters (in)   : timeBaseId
Parameters (inout): None
Parameters (out)  : None
Return value      : uint8
Description       :Allows the Timesync Modules to detect, whether a Time Base should be
                   transmitted immediately in the subsequent <Bus>TSyn_MainFunction() cycle.
 **********************************************************************************************************************/
/* [SWS_StbM_00347] */
uint8 StbM_GetTimeBaseUpdateCounter(StbM_SynchronizedTimeBaseType timeBaseId)
{
    /* Local Variable declaration */
    uint8 lIndex_u8;
    uint8 lTimeBaseUpdateCounter_u8;

    /* Local Variable Initialization */
    lTimeBaseUpdateCounter_u8 = STBM_ZERO;

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBase is Synchronized or Offset TimeBase */
        if((StbM_Rb_IsSyncTimeBase(timeBaseId)) || (StbM_Rb_IsOffsetTimeBase(timeBaseId)))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            lTimeBaseUpdateCounter_u8 = *(StbM_TimeBaseUpdateCounter_au8+lIndex_u8);
        }
        else
        {
            /* [SWS_StbM_00348]If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_GetTimeBaseUpdateCounter()
                    shall report to DET the development error STBM_E_PARAM, if called with a parameter timeBaseID, which
                    is not configured or refers to a Pure Local Time Base or is within the reserved value range. */
            STBM_DET_REPORT_ERROR_NULL(STBM_INSTANCE_ID,STBM_APIID_GETTIMEBASEUPDATECOUNTER,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised */
        STBM_DET_REPORT_ERROR_NULL(STBM_INSTANCE_ID,STBM_APIID_GETTIMEBASEUPDATECOUNTER,STBM_E_NOT_INITIALIZED)
    }

    return(lTimeBaseUpdateCounter_u8);
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */


