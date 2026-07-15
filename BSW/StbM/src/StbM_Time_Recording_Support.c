

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

/* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
#if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )

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
#define STBM_START_SEC_VAR_CLEARED_16
#include "StbM_MemMap.h"
uint16 StbM_RecordTableBlockCountCurrent_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_16
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* [SWS_StbM_00319] */
/* SWS_StbM_00320] The function StbM_GetSyncTimeRecordHead() shall be pre compile time configurable ON/OFF by the
    configuration parameter: StbMTimeRecordingSupport. */
/*********************************************************************************************************************
Service name      : StbM_GetSyncTimeRecordHead
Syntax            : Std_ReturnType StbM_GetSyncTimeRecordHead( StbM_SynchronizedTimeBaseType timeBaseId,
                    StbM_SyncRecordTableHeadType* syncRecordTableHead )
Service ID[hex]   : 0x16
Sync/Async        : Synchronous
Reentrancy        : Non Reentrant
Parameters (in)   : timeBaseId
Parameters (inout): None
Parameters (out)  : syncRecordTableHead
Return value      : Std_ReturnType
Description       : Accesses to the recorded snapshot data Header of the table belonging to the Synchronized Time Base.
 **********************************************************************************************************************/
/* [SWS_StbM_00315] If StbMTimeRecordingSupport is set to TRUE, the application collects the contents of the header of
    the Synchronized Time Base Record Table by calling StbM_GetSyncTimeRecordHead(). */
Std_ReturnType StbM_GetSyncTimeRecordHead(StbM_SynchronizedTimeBaseType timeBaseId,
                              StbM_SyncRecordTableHeadType * syncRecordTableHead )
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
        /* Check if the TimeBase is Synchronized TimeBase */
        if(StbM_Rb_IsSyncTimeBase(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the userdata is NULL_PTR or not*/
            if((syncRecordTableHead != NULL_PTR))
            {
                /*StbM_GetSyncTimeRecordHead will be called only when timebase is SLAVE or GATEWAY */
                if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMState_e != STBM_MASTER )
                {
                    *syncRecordTableHead =
                            StbM_Cfg_TimeBaseHeaderRecordArray_ast[lIndex_u8].SynchronizedTimeBaseHeaderRecord_st;
                    lRet_Val_u8 = E_OK;
                }
                else
                {
                    /*return E_NOT_OK*/
                }
            }
            else
            {
                /* [SWS_StbM_00405] If the switch StbMDevErrorDetect is set to TRUE, GetSyncTimeRecordHead shall report to DET the
                    development error STBM_E_PARAM_POINTER, if called with an invalid pointer of parameter syncRecordTableHead. */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETSYNCTIMERECORDHEAD,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00394] If the switch StbMDevErrorDetect is set to TRUE, StbM_GetSyncTimeRecordHead() shall report to
                DET the development error STBM_E_PARAM, if called with a parameter timeBaseID, which
                - is not configured or
                - refers to a Pure Local or a Offset Time Base or
                - is within the reserved value range. */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETSYNCTIMERECORDHEAD,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETSYNCTIMERECORDHEAD,STBM_E_NOT_INITIALIZED)
    }

    return (lRet_Val_u8);
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#if (STBM_CFG_NO_OF_OFFSET_TB_WITH_TIMERECORDING > 0)
#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* [SWS_StbM_00325] */
/* [SWS_StbM_00326] The function StbM_GetOffsetTimeRecordHead() shall be pre compile time configurable ON/OFF by the
    configuration parameter: StbMTimeRecordingSupport. */
/*********************************************************************************************************************
Service name      : StbM_GetOffsetTimeRecordHead
Syntax            : Std_ReturnType StbM_GetOffsetTimeRecordHead( StbM_SynchronizedTimeBaseType timeBaseId,
                                                                 StbM_OffsetRecordTableHeadType* offsetRecordTableHead )
Service ID[hex]   : 0x17
Sync/Async        : Synchronous
Reentrancy        : Non Reentrant
Parameters (in)   : timeBaseId
Parameters (inout): None
Parameters (out)  : offsetRecordTableHead
Return value      : Std_ReturnType
Description       : Accesses to the recorded snapshot data Header of the table belonging to the Offset Time Base.
 **********************************************************************************************************************/
/* [SWS_StbM_00316] If StbMTimeRecordingSupport is set to TRUE, the application collects the contents of the header of
    the Offset Time Base Record Table by calling StbM_GetOffsetTimeRecordHead(). */
Std_ReturnType StbM_GetOffsetTimeRecordHead(StbM_SynchronizedTimeBaseType timeBaseId,
                              StbM_OffsetRecordTableHeadType * offsetRecordTableHead )
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
        /* Check if the TimeBase is Offest TimeBase */
        if(StbM_Rb_IsOffsetTimeBase(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the userdata is NULL_PTR or not*/
            if((offsetRecordTableHead != NULL_PTR))
            {
                /*StbM_GetOffsetTimeRecordHead will be called only when timebase is SLAVE or GATEWAY */
                if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMState_e != STBM_MASTER )
                {
                    *offsetRecordTableHead =
                            StbM_Cfg_TimeBaseHeaderRecordArray_ast[lIndex_u8].OffsetTimeBaseHeaderRecord_st;
                    lRet_Val_u8 = E_OK;
                }
                else
                {

                }
            }
            else
            {
                /* [SWS_StbM_00404] If the switch StbMDevErrorDetect is set to TRUE, GetOffsetTimeRecordHead shall report to DET the
                        development error STBM_E_PARAM_POINTER, if called with an invalid pointer of parameter offsetRecordTableHead. */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETOFFSETTIMERECORDHEAD,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00327] If the switch StbMDevErrorDetect is set to TRUE, StbM_GetOffsetTimeRecordHead() shall report to DET
                    the development error STBM_E_PARAM, if called with a parameter timeBaseID, which
                    - is not configured or
                    - refers to a Pure Local or a Synchronized Time Base or
                    - is within the reserved value range. */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETOFFSETTIMERECORDHEAD,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETOFFSETTIMERECORDHEAD,STBM_E_NOT_INITIALIZED)
    }

    return (lRet_Val_u8);
}


#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"
#endif /* #if (STBM_CFG_NO_OF_OFFSET_TB_WITH_TIMERECORDING > 0) */
#endif/* #if (STBM_TIME_RECORDING_SUPPORT ==  STD_ON )*/

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */


