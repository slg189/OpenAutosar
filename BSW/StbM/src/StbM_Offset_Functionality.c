

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

#include "StbM_Priv.h"
#include "StbM_Inl.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      :StbM_SetOffset
Syntax            :Std_ReturnType StbM_SetOffset( StbM_SynchronizedTimeBaseType timeBaseId, const StbM_TimeStampType*
                                                              timeStamp, const StbM_UserDataType* userData )
Service ID[hex]   :0x0d
Sync/Async        :Synchronous
Reentrancy        :Non Reentrant
Parameters (in)   :timeBaseId   time base reference
                   timeStamp    New offset time stamp
                   userData     New User Data
Parameters (inout):None
Parameters (out)  :None
Return value      :Std_ReturnType E_OK: successful
                            E_NOT_OK: failed
Description       :Allows the Customers and the Timesync Modules to set the Offset Time and the User Data.
                   Note : userdata is availble only for specific 4.3 Rfc's implementation
 **********************************************************************************************************************/
/* [SWS_StbM_00223] */
Std_ReturnType StbM_SetOffset(StbM_SynchronizedTimeBaseType timeBaseId,
        const StbM_TimeStampType * timeStamp
        ,const StbM_UserDataType * userData
)
{
    /* Local Variable declaration */
    Std_ReturnType lRet_Val_u8;
    uint8 lIndex_u8;
#if (STBM_NVM_ENABLE ==  STD_ON )
    uint8 lNvm_Index_u8;
#endif/* #if (STBM_NVM_ENABLE ==  STD_ON )*/

    /* Local Variable Initialization */
    lRet_Val_u8 = E_NOT_OK;

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;
#if (STBM_NVM_ENABLE ==  STD_ON )
    lNvm_Index_u8 = STBM_INVALID_INDEX;
#endif/* #if (STBM_NVM_ENABLE ==  STD_ON )*/

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* [SWS_StbM_00190] Each invocation of StbM_SetOffset() shall update the Offset Time of the corresponding Time Base
           [SWS_StbM_00191] StbM_SetOffset() and StbM_GetOffset() shall only accept Offset Time Bases with a timeBaseId 16
           till 31 */
        /*  Check if the TimeBase is Offset TimeBase */
        if(StbM_Rb_IsOffsetTimeBase(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];
#if (STBM_NVM_ENABLE ==  STD_ON )
            lNvm_Index_u8 = StbM_Nvm_Index_au8[timeBaseId];
#endif/* #if (STBM_NVM_ENABLE ==  STD_ON )*/

            /* Check if the timeStamp is NULL_PTR or not*/
            if(timeStamp != NULL_PTR)
            {
                /* Check if the nanoseconds value passed is less than 999999999 */
                if(timeStamp->nanoseconds <= STBM_MAX_NS)
                {
                    /* Check if the userDataLength passed is less than or equal to 3 when UserData is not NULL */
                    if(StbM_Rb_IsUserDataValid(userData) != STBM_USER_DATA_INVALID)
                    {
                        /*  Enter Resource lock */
                        SchM_Enter_StbM_Time_Source();

                        /*  Time value updated for requested time base */
                        StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus = timeStamp->timeBaseStatus  ;/* Status value Check */
                        StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds    = timeStamp->nanoseconds     ;/* nanoseconds value */
                        StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds        = timeStamp->seconds         ;/* seconds value */
                        StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi      = timeStamp->secondsHi       ;/* seconds Hi value */

#if (STBM_NVM_ENABLE ==  STD_ON )
                        /* The lNvm_Index_u8 value is only valid when
                        StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StoreTimebaseNonVolatile_e==STBM_NVM_STORAGE_AT_SHUTDOWN*/
                        if (lNvm_Index_u8 != STBM_INVALID_INDEX)
                        {
                            StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].nanoseconds    = timeStamp->nanoseconds     ;/* nanoseconds value */
                            StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].seconds        = timeStamp->seconds         ;/* seconds value */
                            StbM_TimesourceArray_Nvm_ast[lNvm_Index_u8].secondsHi      = timeStamp->secondsHi       ;/* seconds Hi value */

                            lRet_Val_u8 = NvM_SetRamBlockStatus (STBM_NVM_BLOCK_REFERENCE, TRUE);
                        }
                        else
                        {
                            lRet_Val_u8 = E_OK;
                        }
#else
                        lRet_Val_u8 = E_OK;
#endif/* #if (STBM_NVM_ENABLE ==  STD_ON )*/

                        /* UserData can be either Null_Ptr or with Valid elements. Hence do update UserData, only if its not Null_Ptr */
                        if(StbM_Rb_IsUserDataValid(userData) != STBM_USER_DATA_NULL_PTR)
                        {
                            /* [SWS_StbM_00190]
                               Each invocation of StbM_SetOffset() shall update the Offset Time and the User Data
                               of the corresponding Time Base. */

                            /* Set the user data with the new data recieved based on the length provided by user as structure attribute */
                            StbM_UserDataUpdate(lIndex_u8,userData);
                        }
                        else
                        {
                            /* No need to update UserData, as its NULL */
                        }

                        /*  Exit Resource lock */
                        SchM_Exit_StbM_Time_Source();
                    }
                    else
                    {
                        /* [SWS_StbM_00454] If the switch StbMDevErrorDetect is set to TRUE, StbM_SetOffset()
                         * shall report to DET the development error STBM_E_PARAM_USERDATA, if called with an invalid
                         * value of parameter userData, i.e., userDataLength > 3. */
                        /* If userDataLength is invalid(greater than 3) then report error and exit the function */
                        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETOFFSET,STBM_E_PARAM_USERDATA)
                    }
                }
                else
                {
                    /* [SWS_StbM_00453] If the switch StbMDevErrorDetect is set to TRUE, StbM_SetOffset()
                     * shall report to DET the development error STBM_E_PARAM_TIMESTAMP, if called with a
                     * parameter timeStamp that contains invalid elements (e.g., nanoseconds part > 999999999 ns). */
                    /* If ns part in TimeStamp is greater than 999999999 then report error and exit the function */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETOFFSET,STBM_E_PARAM_TIMESTAMP)
                }
            }
            else
            {
                /* [SWS_StbM_00225]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_SetOffset()
               shall report to DET the development error STBM_E_PARAM_POINTER, if called with an invalid pointer of parameter
               timeStampPtr or userDataPtr. */
                /* if timeStampPtr is NULL pointer then report error and exit the function */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETOFFSET,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00224] If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_SetOffset()
               shall report to DET the development error STBM_E_PARAM, if called with a parameter timeBaseID which
               is not configured,refers to a Synchronized or Pure Local Time Base or is within the reserved value range*/

            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETOFFSET,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_SETOFFSET,STBM_E_NOT_INITIALIZED)
    }

    return (lRet_Val_u8);

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#endif /*#if (STBM_CFG_NO_OF_OFFSET_MASTER_TIMEBASES > STBM_ZERO)*/

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name         :StbM_GetOffset
Syntax               :Std_ReturnType StbM_GetOffset( StbM_SynchronizedTimeBaseType timeBaseId,
                      StbM_TimeStampType* timeStamp,StbM_UserDataType* userData )
Service ID[hex]      :0x0e
Sync/Async           :Synchronous
Reentrancy           :Non Reentrant
Parameters (in)      :timeBaseId time base reference
Parameters (inout)   :None
Parameters (out)     :timeStamp Current offset time stamp
                      userData  Current User Data
Return value         :Std_ReturnType E_OK: successful
                            E_NOT_OK: failed
Description          :Allows the Timesync Modules to get the current Offset Time and User Data.
                      Note : userdata is availble only for specific 4.3 Rfc's implementation
 **********************************************************************************************************************/
/* [SWS_StbM_00228] */
Std_ReturnType StbM_GetOffset(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampType * timeStamp
        ,StbM_UserDataType * userData

)
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
        /* [SWS_StbM_00190] Each invocation of StbM_SetOffset() shall update the Offset Time of the corresponding Time Base
           [SWS_StbM_00191] StbM_SetOffset() and StbM_GetOffset() shall only accept Offset Time Bases with a timeBaseId 16
           till 31 */
        /*  Check if the TimeBase is Offset TimeBase */
        if(StbM_Rb_IsOffsetTimeBase(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the userdata is NULL_PTR or not*/
            if(userData != NULL_PTR)
            {
                /* Check if the userdata is NULL_PTR or not*/
                if(timeStamp != NULL_PTR)
                {

                    /*  Time value updated for requested time base */
                    timeStamp->timeBaseStatus = StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus  ;/* Status value Check */
                    timeStamp->nanoseconds    = StbM_GlobalTimeTupleArray_ast[lIndex_u8].nanoseconds     ;/* nanoseconds value */
                    timeStamp->seconds        = StbM_GlobalTimeTupleArray_ast[lIndex_u8].seconds         ;/* seconds value */
                    timeStamp->secondsHi      = StbM_GlobalTimeTupleArray_ast[lIndex_u8].secondsHi       ;/* seconds Hi value */

                    /* [SWS_StbM_00192]
           Each invocation of StbM_GetOffset() shall return the Offset Time and the User Data of the
           corresponding Offset Time Base. */

                    *userData = *(StbM_UserDataArray_ast + lIndex_u8);

                    lRet_Val_u8 = E_OK;
                }
                else
                {
                    /* [SWS_StbM_00230]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_GetOffset()
                       shall report to DET the development error STBM_E_PARAM_POINTER, if called with an invalid pointer of parameter
                       timeStampPtr */
                    /* if timeStampPtr is NULL pointer then report error and exit the function */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETOFFSET,STBM_E_PARAM_POINTER)
                }
            }
            else
            {
                /* [SWS_StbM_00230]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_GetOffset()
                   shall report to DET the development error STBM_E_PARAM_POINTER, if called with an invalid pointer of parameter
                   timeStampPtr */
                /* If userDataPtr is NULL_PTR report error and exit function */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETOFFSET,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00229]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_GetOffset()
               shall report to DET the development error STBM_E_PARAM, if called with a parameter timeBaseID which
               is not configured,refers to a Synchronized or Pure Local Time Base or is within the reserved value range*/

            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETOFFSET,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not initialised */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETOFFSET,STBM_E_NOT_INITIALIZED)
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
