

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

#endif /*#if(STBM_DEV_ERROR_DETECT == STD_ON)*/

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


/*
**********************************************************************************************************************
* Local functions decalrations
**********************************************************************************************************************
*/

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Function to calculate the current time for Offset Timebase */
static Std_ReturnType StbM_GetCurrentTime_Offset_Timebase(
       StbM_SynchronizedTimeBaseType offset_timeBaseId,
       uint8 offset_index,
       StbM_SynchronizedTimeBaseType timeBaseId,
       StbM_TimeStampType * timeStampPtr,
       StbM_VirtualLocalTimeType * vLocalTimeStamp);

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#if (STBM_GPT_ENABLE == STD_ON)
#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function for elapsed time calculation when HW reference is set as GPT */
Gpt_ValueType StbM_DeltaTimeCalculation_GPT(Gpt_ValueType LastTickValue,
                                                              Gpt_ValueType CurrentTickValue)
{
    Gpt_ValueType DeltaTickValue;

    if(CurrentTickValue > LastTickValue)
    {
        DeltaTickValue = CurrentTickValue - LastTickValue;
    }
    else if(CurrentTickValue < LastTickValue)
    {
        DeltaTickValue = ( STBM_MAX_UINT32 - LastTickValue) + STBM_ONE + CurrentTickValue;
    }
    else
    {
        DeltaTickValue = STBM_ZERO;
    }

    return DeltaTickValue;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"
#endif /* #if (STBM_GPT_ENABLE == STD_ON) */

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_Overflow_Check
Syntax            : StbM_TimeStampType StbM_Overflow_Check
											   (uint8 index,
												StbM_TimeStampType timestampIn,
                                                StbM_TimeValueType_ten timevaluetype)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : index                - Index of the timebase ID
					timestampIn          - timestamp value that is passed
                    timevaluetype        - time value type( Global time or Virtual Local Time)

Parameters (inout): None
Parameters (out)  : None
Return value      : StbM_TimeStampType   - Time value returned after overflow calculation

Description       : Overflow check is done for the sum of Delta time value and Offset time basevalue and synchronised
					time base value
 **********************************************************************************************************************/
/* Local function for the time overflow check  */
StbM_TimeStampType StbM_Overflow_Check (uint8 index,
                                                         StbM_TimeStampType timestampIn,
                                                         StbM_TimeValueType_ten timevaluetype)
{
    /* Local Variable declaration  */
    StbM_TimeStampType lStbM_TimesourceArray_Overflow_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lStbM_TimesourceArray_Global_st   = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    uint32 l_Sec_u32,l_Sec1_u32, l_Sec2_u32;
    uint16 l_Sec_Hi_u16;

    /* Local Variable initialisation  */
    l_Sec_u32    = STBM_ZERO;
    l_Sec1_u32   = STBM_ZERO;
    l_Sec2_u32   = STBM_ZERO;
    l_Sec_Hi_u16 = STBM_ZERO;

    /*Check the time value type*/
    if(timevaluetype == STBM_GLOBAL_TIME_VALUE)
    {
        /* Copy the Global time value into a local variable */
        lStbM_TimesourceArray_Global_st.nanoseconds = StbM_GlobalTimeTupleArray_ast[index].nanoseconds;
        lStbM_TimesourceArray_Global_st.seconds     = StbM_GlobalTimeTupleArray_ast[index].seconds;
        lStbM_TimesourceArray_Global_st.secondsHi   = StbM_GlobalTimeTupleArray_ast[index].secondsHi;

    }
    else
    {
        /* Copy the Global time value updated by the previous StbM_BusSetGlobalTime/StbM_SetGlobalTime/StbM_UpdateGlobalTime API call into a local variable */
        lStbM_TimesourceArray_Global_st.nanoseconds = StbM_GlobalTimeReception_ast[index].nanoseconds;
        lStbM_TimesourceArray_Global_st.seconds     = StbM_GlobalTimeReception_ast[index].seconds;
        lStbM_TimesourceArray_Global_st.secondsHi   = StbM_GlobalTimeReception_ast[index].secondsHi;

    }

    /* Overflow check for nanoseconds part  */
    /* Add the delta time value and Synchronised time value (nanoseconds)  */
    if((lStbM_TimesourceArray_Global_st.nanoseconds + (timestampIn.nanoseconds %(STBM_MAX_NS+1U)))> STBM_MAX_NS)
    {
        /* If the nanoseconds portion exceeds the MAX nanoseconds value divide the time value by the MAX nanoseconds
           value to get the seconds and nanoseconds portion of time value  */
        lStbM_TimesourceArray_Overflow_st.nanoseconds = (lStbM_TimesourceArray_Global_st.nanoseconds +
                                                 (timestampIn.nanoseconds % (STBM_MAX_NS + 1U))) % (STBM_MAX_NS+1U);

        l_Sec_u32 = (lStbM_TimesourceArray_Global_st.nanoseconds +
        (timestampIn.nanoseconds %(STBM_MAX_NS+1U)))/(STBM_MAX_NS+1U);
    }
    else
    {
        /* IF the nanoseconds portion does not exceeds the MAX nanoseconds value assign the time value to the local variable  */
        lStbM_TimesourceArray_Overflow_st.nanoseconds =  lStbM_TimesourceArray_Global_st.nanoseconds +
                   (timestampIn.nanoseconds %(STBM_MAX_NS+1U));
    }

    /* overflow check for seconds part  */
    /* step 1 */
    l_Sec1_u32 = lStbM_TimesourceArray_Global_st.seconds + l_Sec_u32;
    /* Handling overflow, i.e. if result of addition is less than any of its operands
    (there was an overflow) l_Sec_Hi_u16 is incremented*/
    if(l_Sec1_u32 < l_Sec_u32 )
    {
        /* Increment the SecondsHi part  */
        l_Sec_Hi_u16 = STBM_ONE;

    }
    l_Sec1_u32 = l_Sec1_u32 + (timestampIn.nanoseconds /(STBM_MAX_NS+1U));
    /* Handling overflow, i.e. if result of addition is less than any of its operands
        (there was an overflow) l_Sec_Hi_u16 is incremented*/
    if(l_Sec1_u32 < (timestampIn.nanoseconds / (STBM_MAX_NS+1U)))
    {
            /* Increment the SecondsHi part  */
            l_Sec_Hi_u16 =  l_Sec_Hi_u16 + STBM_ONE ;

    }

    /* step 2 */
    l_Sec2_u32 = l_Sec1_u32 + timestampIn.seconds;
    /* Handling overflow, i.e. if result of addition is less than any of its operands
       (there was an overflow) l_Sec_Hi_u16 is incremented*/
    if(l_Sec2_u32 < l_Sec1_u32 )
    {
        /* Increment the SecondsHi part  */
        l_Sec_Hi_u16 = l_Sec_Hi_u16 + STBM_ONE;
    }

    /*  Update the seconds value  */
    lStbM_TimesourceArray_Overflow_st.seconds = l_Sec2_u32;

    /* Add the delta time value and Synchronised time value and l_Sec_Hi_u16 (secondsHi)  */
    lStbM_TimesourceArray_Overflow_st.secondsHi = l_Sec_Hi_u16 +
                                    lStbM_TimesourceArray_Global_st.secondsHi + timestampIn.secondsHi;

    return (lStbM_TimesourceArray_Overflow_st);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_Offset_Overflow_Check
Syntax            : StbM_TimeStampType StbM_Offset_Overflow_Check
                                               (StbM_TimeStampType OffsetTimebaseValue,
                                                StbM_TimeStampType * const timeStampPtr)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : OffsetTimebaseValue  - Time current time value of the offset time base
                    timeStampPtr         - Time current time value of Sync timebase

Parameters (inout): None
Parameters (out)  : None
Return value      : StbM_TimeStampType   - Time value returned after overflow calculation

Description       : Overflow check is done for the sum of Offset time basevalue and synchronised
                    time base value
 **********************************************************************************************************************/
/* Local function for the overflow check for Offset TimeBase */
StbM_TimeStampType StbM_Offset_Overflow_Check (StbM_TimeStampType OffsetTimebaseValue,
        const StbM_TimeStampType * timeStampPtr)
{
    /* Local Variable declaration  */

    StbM_TimeStampType lStbM_TimesourceArray_Overflow_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    uint32 l_Sec_u32,l_Sec1_u32, l_Sec2_u32;
    uint16 l_Sec_Hi_u16;

    /* Local Variable initialisation  */
    l_Sec_u32    = STBM_ZERO;
    l_Sec1_u32   = STBM_ZERO;
    l_Sec2_u32   = STBM_ZERO;
    l_Sec_Hi_u16 = STBM_ZERO;

    /* Overflow check for nanoseconds part  */
    /* Add Synchronised time value and the Offset time value (nanoseconds)  */
    if((timeStampPtr->nanoseconds +OffsetTimebaseValue.nanoseconds)> STBM_MAX_NS)
    {
        /* If the nanoseconds portion exceeds the MAX nanoseconds value divide the time value by the MAX nanoseconds
            value to get the seconds and nanoseconds portion of time value  */
        lStbM_TimesourceArray_Overflow_st.nanoseconds =
                (timeStampPtr->nanoseconds + OffsetTimebaseValue.nanoseconds)%(STBM_MAX_NS+1U);

        l_Sec_u32 = (timeStampPtr->nanoseconds + OffsetTimebaseValue.nanoseconds)/(STBM_MAX_NS+1U);
    }
    else
    {
        /* IF the nanoseconds portion does not exceeds the MAX nanoseconds value assign the time value to the local variable  */
        lStbM_TimesourceArray_Overflow_st.nanoseconds =  timeStampPtr->nanoseconds + OffsetTimebaseValue.nanoseconds;
    }

    /* overflow check for seconds part  */
    /* Add the Synchronised time value and the Offset time value (seconds)  */
    l_Sec1_u32 = (timeStampPtr->seconds +  OffsetTimebaseValue.seconds);
    /* Handling overflow, i.e. if result of addition is less than any of its operands
       (there was an overflow) l_Sec_Hi_u16 is incremented*/
    if(l_Sec1_u32 < timeStampPtr->seconds)
    {
        /* Increment the SecondsHi part  */
        l_Sec_Hi_u16 = STBM_ONE;

    }
    /* Add the parameters to get the sum  */
    l_Sec2_u32 = l_Sec_u32 + l_Sec1_u32 ;

    /* Handling overflow, i.e. if result of addition is less than any of its operands
       (there was an overflow) l_Sec_Hi_u16 is incremented*/
    if (l_Sec2_u32 < l_Sec1_u32)
    {
        /* Increment the SecondsHi part  */
        l_Sec_Hi_u16 = l_Sec_Hi_u16 + STBM_ONE;
    }
    /*  Update the seconds value  */
    lStbM_TimesourceArray_Overflow_st.seconds = l_Sec2_u32 ;

    /* Add the delta time value and Synchronised time value and the Offset time value and l_Sec_Hi_u16 (secondsHi)  */
    lStbM_TimesourceArray_Overflow_st.secondsHi     = l_Sec_Hi_u16 + timeStampPtr->secondsHi
            + OffsetTimebaseValue.secondsHi;

    return (lStbM_TimesourceArray_Overflow_st);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_Time_DifferenceCalculation
Syntax            : void StbM_Time_DifferenceCalculation
                                           (StbM_TimeStampType * timeStampPtr_current,
                                            StbM_TimeStampType * timeStampPtr_last,
                                            StbM_TimeStampType * timeStampPtr,
                                            uint8 greaterTimeStamp)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : timeStampPtr_current - current time value
					timeStampPtr_last    - last time value

Parameters (inout): None
Parameters (out)  : timeStampPtr         - Time difference value
                    greaterTimeStamp     - 1 - Current time is the greater value
                                           0 - Last time value is the greater value
Return value      : void

Description       : Local function for time difference calculation
**********************************************************************************************************************/
/* Local function for the time for Time difference calculation  */
void StbM_Time_DifferenceCalculation
        (const StbM_TimeStampType * timeStampPtr_current,
                const StbM_TimeStampType * timeStampPtr_last,
                StbM_TimeStampType * timeStampPtr,
                uint8 * greaterTimeStamp

)
{
    /* Local Variable declaration  */
    uint32 lNanoseconds_last_u32;
    uint32 lSeconds_last_u32;
    uint16 lSecondsHi_last_u16;
    uint32 lNanoseconds_current_u32;
    uint32 lSeconds_current_u32;
    uint16 lSecondsHi_current_u16;
    uint32 lDiff_nanoseconds_u32;
    uint32 lDiff_seconds_u32;
    uint16 lDiff_secondsHi_u16;

    /* Local Variable Initialisation  */
    lNanoseconds_last_u32   = STBM_ZERO;
    lSeconds_last_u32       = STBM_ZERO;
    lSecondsHi_last_u16     = STBM_ZERO;
    lNanoseconds_current_u32= STBM_ZERO;
    lSeconds_current_u32    = STBM_ZERO;
    lSecondsHi_current_u16  = STBM_ZERO;
    lDiff_nanoseconds_u32   = STBM_ZERO;
    lDiff_seconds_u32       = STBM_ZERO;
    lDiff_secondsHi_u16     = STBM_ZERO;

    /* Copy the Current time and last time into the local variables  */
    lNanoseconds_last_u32    =  timeStampPtr_last->nanoseconds    ;
    lSeconds_last_u32        =  timeStampPtr_last->seconds        ;
    lSecondsHi_last_u16      =  timeStampPtr_last->secondsHi      ;

    lNanoseconds_current_u32 =  timeStampPtr_current->nanoseconds ;
    lSeconds_current_u32     =  timeStampPtr_current->seconds     ;
    lSecondsHi_current_u16   =  timeStampPtr_current->secondsHi   ;

    /* For difference calculation we need to first check which is the greater number among last and current time
         to check the greater number between the last and current time the first deciding factor is secondshi part */
    if(lSecondsHi_current_u16 > lSecondsHi_last_u16)
    {
        /*Even if current time greater than last time the nanoseconds and seconds part can be less than
            the last time hence underflow is taken care by borrowing */
        if(lNanoseconds_current_u32 < lNanoseconds_last_u32)
        {
            if(lSeconds_current_u32!=0U)
            {
                /* If the seconds value is not 0 then decrement the seconds value */
                lSeconds_current_u32--;
            }
            else
            {
                /* If the seconds value is 0 then decrement the secondsHi value */
                lSecondsHi_current_u16--;
                /* If the seconds value is 0 then decrement the secondsHi value and add one secondHi value(4294967296) to seconds
                   and add One second value(10^9) to nanoseconds */
                /* Seconds is updated with 4294967295 since, 1s value(STBM_MAX_NS + 1U) is added to nanoseconds */
                lSeconds_current_u32 = STBM_MAX_SEC;
            }
            /* If the seconds value is not 0 then decrement the seconds value and add one second value(10^9) to nanoseconds*/
            /* If the seconds value is 0 then decrement the secondsHi value and add one secondHi value(4294967296) to seconds
               and add One seconds value(10^9) to nanoseconds */
            lNanoseconds_current_u32 = lNanoseconds_current_u32 + STBM_MAX_NS + 1U ;
        }
        /* Check whether the current seconds is less than the last seconds
           ?overflow condition taken care for the lDiff_seconds_u32: Difference of the seconds part  */
        if (lSeconds_current_u32<lSeconds_last_u32)
        {
            /* If the lSeconds_current_u32 is less than lSeconds_last_u32, lSecondsHi_current_u16 is decremented to take a borrow to lSeconds_current_u32 */
            lSecondsHi_current_u16--;
            lDiff_seconds_u32 = lSeconds_current_u32 - lSeconds_last_u32 ;
        }
        else
        {
            lDiff_seconds_u32 = lSeconds_current_u32 - lSeconds_last_u32 ;
        }

        /* Difference of the SecondsHi part  */
        lDiff_secondsHi_u16   = lSecondsHi_current_u16   - lSecondsHi_last_u16 ;

        /* Difference of the nanoseconds part  */
        lDiff_nanoseconds_u32 = lNanoseconds_current_u32 - lNanoseconds_last_u32 ;

        /*the greater timestamp is timeStampPtr_current */
        *greaterTimeStamp = STBM_ONE ;

    }
    /* Check if the seconds HI are equal  */
    else if(lSecondsHi_current_u16 == lSecondsHi_last_u16)
    {
        lDiff_secondsHi_u16 = 0U ;
        /* Check whether the current seconds is greater than the last seconds  */
        if(lSeconds_current_u32 > lSeconds_last_u32)
        {
            if(lNanoseconds_current_u32 < lNanoseconds_last_u32)
            {
                lSeconds_current_u32-- ;
                lNanoseconds_current_u32 = lNanoseconds_current_u32 + STBM_MAX_NS + 1U ;
            }
            /* Difference of the time values  */
            lDiff_seconds_u32     = lSeconds_current_u32     - lSeconds_last_u32 ;
            lDiff_nanoseconds_u32 = lNanoseconds_current_u32 - lNanoseconds_last_u32 ;

            /*the greater timestamp is timeStampPtr_current */
            *greaterTimeStamp = STBM_ONE;

        }
        /* Check whether the current seconds is equal to last seconds  */
        else if(lSeconds_current_u32 == lSeconds_last_u32)
        {
            lDiff_seconds_u32 = 0U ;
            /* Check whether the current nanoseconds is less than the last nanoseconds */
            if(lNanoseconds_current_u32<lNanoseconds_last_u32)
            {
                lDiff_nanoseconds_u32 = lNanoseconds_last_u32-lNanoseconds_current_u32;
                *greaterTimeStamp = STBM_ZERO;
            }
            else
            {
                lDiff_nanoseconds_u32 = lNanoseconds_current_u32-lNanoseconds_last_u32;
                *greaterTimeStamp = STBM_ONE;
            }

        }
        /* Check whether the current seconds is less than the last seconds  */
        else
        {
            if(lNanoseconds_last_u32 < lNanoseconds_current_u32)
            {
                lSeconds_last_u32-- ;
                lNanoseconds_last_u32 = lNanoseconds_last_u32 + STBM_MAX_NS + 1U ;
            }
            /* Difference of the time values  */
            lDiff_seconds_u32     = lSeconds_last_u32     - lSeconds_current_u32 ;
            lDiff_nanoseconds_u32 = lNanoseconds_last_u32 - lNanoseconds_current_u32 ;

            /*the greater timestamp is timeStampPtr_last */
            *greaterTimeStamp = STBM_ZERO ;

        }
    }
    /* The current Hi is less than the last hi  */
    else
    {
        if(lNanoseconds_last_u32 < lNanoseconds_current_u32)
        {
            if(lSeconds_last_u32!=0U)
            {
                /* If the seconds value is not 0 then decrement the seconds value */
                lSeconds_last_u32--;
            }
            else
            {
                /* If the seconds value is 0 then decrement the secondsHi value and add one secondHi value(4294967296) to seconds
                   and add One second value(10^9) to nanoseconds */
                /* Seconds is updated with 4294967295 since, 1s value(STBM_MAX_NS + 1U) is added to nanoseconds */
                lSecondsHi_last_u16--;
                lSeconds_last_u32=STBM_MAX_SEC;
            }
            /* If the seconds value is not 0 then decrement the seconds value and add one second value(10^9) to nanoseconds*/
            /* If the seconds value is 0 then decrement the secondsHi value and add one secondHi value(4294967296) to seconds
               and add One seconds value(10^9) to nanoseconds */
            lNanoseconds_last_u32 = lNanoseconds_last_u32 + STBM_MAX_NS + 1U;
        }
        /*  ? overflow condition taken care for the lDiff_seconds_u32 */
        if(lSeconds_last_u32<lSeconds_current_u32)
        {
            /* If the lSeconds_last_u32 is less than lSeconds_current_u32, lSecondsHi_last_u16 is decremented to take a borrow to lSeconds_last_u32 */
            lSecondsHi_last_u16-- ;
            lDiff_seconds_u32= lSeconds_last_u32-lSeconds_current_u32;
        }
        else
        {
            (lDiff_seconds_u32=lSeconds_last_u32-lSeconds_current_u32);
        }

        lDiff_secondsHi_u16   = lSecondsHi_last_u16   - lSecondsHi_current_u16;
        lDiff_nanoseconds_u32 = lNanoseconds_last_u32 - lNanoseconds_current_u32;

        /*the greater timestamp is timeStampPtr_last */
        *greaterTimeStamp = STBM_ZERO ;

    }
    /* Copy the time value into the pointer  */
    timeStampPtr->nanoseconds = lDiff_nanoseconds_u32;
    timeStampPtr->seconds     = lDiff_seconds_u32;
    timeStampPtr->secondsHi   = lDiff_secondsHi_u16  ;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_GetCurrentTime_Timeout_Check()
Syntax            : void StbM_GetCurrentTime_Timeout_Check(uint8 index,
                                                StbM_TimeStampType * const timeStampPtr)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : index                - Index of the timebase ID
                    timeStampPtr         - Current time value

Parameters (inout): None
Parameters (out)  : None
Return value      : None

Description       : Timeout Check for Timebase
**********************************************************************************************************************/
/* Local function for the time for Timeout check  */
void StbM_GetCurrentTime_Timeout_Check( uint8 index,
        const StbM_VirtualLocalTimeType * virtualLocalTime)
{
    /* Local Variable declaration */
    uint64 lVirtualLocalTimeDiff_u64;
    uint64 lSyncTimeoutConfigured_u64;
    StbM_VirtualLocalTimeType lVirtualLocalTimeDiff_st  = {STBM_ZERO,STBM_ZERO};
    StbM_TimeBaseStatusType lTimbasestatus_st = STBM_ZERO;
    StbM_TimeBaseNotificationType lNotifyMask_u32 = STBM_ZERO ;

    /* Local Variable Initialization */
    lVirtualLocalTimeDiff_u64 = STBM_ZERO;
    lSyncTimeoutConfigured_u64   = STBM_ZERO ;

    /*[SWS_StbM_00279]: For each Time Base the StbM has a configurable mask StbMStatusNotificationMask , which allows to
            mask individually status event notifications.*/
    /* [SWS_StbM_00278]:For Synchronized and Offset Time Bases the StbM shall use a variable NotificationEvents of type
         StbM_TimeBaseNotificationType to keep track, if any status event for the referenced Time Base occurs.
         If any status event occurs and the corresponding bit in the NotificationMask mask is set, the corresponding bit
         in the NotificationEvents variable is set , i.e., NotificationEvents contains the bits for the events, which are
         enabled within the NotificationMask mask.*/
    lNotifyMask_u32 = StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMStatusNotificationMask_u32 ;

    /* Copy the timebasestatus of the requested timebase from global array to local variable  */
    lTimbasestatus_st = StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus;


    /* [SWS_StbM_00183]For each Time Domain where a Time Slave belongs to, the StbM shall observe a timeout.
        The timeout StbMSyncLossTimeout (ECUC_StbM_00028 :) shall be measured from last invocation of
        StbM_BusSetGlobalTime() If the timeout occurs, the StbM shall set the TIMEOUT bit within timeBaseStatus of
        the Time Base. An invocation of StbM_BusSetGlobalTime() shall clear the bit.  */

    /* TIMEOUT calculation from the last invocation of the BusSetGlobalTime wrt Virtual Local Time */
    /* Check the difference between the last VLT from the last invoaction of BSGT and Current VLT */
    lVirtualLocalTimeDiff_st =
    StbM_Rb_VirtualLocalTimeDifference(StbM_VirtualLocalTimeReception_ast[index],*virtualLocalTime);

    /* Convert the StbM_VirtualLocalTimeType into uint64 value*/
    lVirtualLocalTimeDiff_u64 = (((uint64)(lVirtualLocalTimeDiff_st.nanosecondsHi)) << 32) |
            (uint64)(lVirtualLocalTimeDiff_st.nanosecondsLo);

    /* copy the configured StbMSyncLossTimeout value  */
    lSyncTimeoutConfigured_u64 =
            (((uint64)(StbM_Cfg_SynchronizedTimeBaseArray_ast[index].SyncLossTimeout_sec_u32)) * STBM_NS_PER_SEC) +
                                  StbM_Cfg_SynchronizedTimeBaseArray_ast[index].SyncLossTimeout_ns_u32;

    if(lSyncTimeoutConfigured_u64 != STBM_ZERO)
    {
        /*  check whether the difference exceeds the threshold value  */
        if(lVirtualLocalTimeDiff_u64 > lSyncTimeoutConfigured_u64)
        {
            /* If STBM_GLOBAL_TIME_BASE_BIT is set then set the STBM_TIMEOUT_BIT and SYNC_TO_GATEWAY bit
			   [SWS_StbM_00239] Any of the bits TIMEOUT, TIMELEAP_FUTURE, TIMELEAP_PAST and SYNC_TO_GATEWAY can only
               be set if the GLOBAL_TIME_BASE bit is set.*/
            if(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT))
            {
                /* If the timeout occurs, the StbM shall set the TIMEOUT bit within timeBaseStatus of the Time Base.  */
                lTimbasestatus_st = StbM_Set_Bit(lTimbasestatus_st,STBM_TIMEOUT_BIT,
                                     (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_TIMEOUT_OCCURED);

                /* SYNC_TO_GATEWAY bit will be set only when StbM is GATEWAY */
                if(StbM_Cfg_SynchronizedTimeBaseArray_ast[index].StbMState_e == STBM_GATEWAY )
                {
                    /* [SWS_StbM_00187] If the timeout occurs, the StbM shall set the SYNC_TO_GATEWAY bit within timeBaseStatus
                        of the Time Base  */
                    lTimbasestatus_st = StbM_Set_Bit(lTimbasestatus_st,STBM_SYNC_TO_GATEWAY_BIT,
                                    (StbM_NotificationEvents_au32+index),lNotifyMask_u32,STBM_EV_SYNC_TO_SUBDOMAIN);

                }
                else
                {
                    /* Do nothing - QAC */
                }
#if (STBM_TIME_CORRECTION ==  STD_ON )
                /* Reset the Rate correction cycle count back to zero to discard all the ongoing measurements */
                StbM_ResetOngoingRateCorrections(index);
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
            }
            else
            {
                /* If STBM_GLOBAL_TIME_BASE_BIT bit is not set then STBM_TIMEOUT_BIT and SYNC_TO_GATEWAY should not be set */
            }

        }
        else
        {
            /* Do nothing - QAC */
        }
    }
    else
    {
        /* Do nothing - QAC */
    }
    /*  Enter the resource lock  */
    SchM_Enter_StbM_Time_Source();

    /* Copy the updated timebasestatus into the global structure paramater  */
    StbM_GlobalTimeTupleArray_ast[index].timeBaseStatus        = lTimbasestatus_st;


    /*  Exit the resource lock  */
    SchM_Exit_StbM_Time_Source();

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_GetCurrentTime_Offset_Timebase(timeBaseId)
Syntax            : Std_ReturnType StbM_GetCurrentTime_Offset_Timebase
                        (StbM_SynchronizedTimeBaseType offset_timeBaseId,
                         uint8 offset_index,
                         StbM_SynchronizedTimeBaseType timeBaseId,
                         StbM_TimeStampType * const timeStampPtr)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : timeBaseId	     - time base ID of referenced sync time base
                    offset_timeBaseId- time base ID of offset time base
                    offset_index     - Index of offset time base
Parameters (inout): None
Parameters (out)  : timeStampPtr     -  Current time value for offset timebase
Return value      : Std_ReturnType   -  E_OK    : successful
                                        E_NOT_OK: failed
Description       : Current time calculation for Offset time base
**********************************************************************************************************************/
/* Local function for the time for Time difference calculation  */
static Std_ReturnType StbM_GetCurrentTime_Offset_Timebase
               (StbM_SynchronizedTimeBaseType offset_timeBaseId,
                uint8 offset_index,
                StbM_SynchronizedTimeBaseType timeBaseId,
                StbM_TimeStampType * timeStampPtr,
                StbM_VirtualLocalTimeType * vLocalTimeStamp)
{
    /* Local Variable declaration */
    StbM_TimeStampType lStbM_TimesourceArray_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lTimeStamp_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_UserDataType lUserData_st ={STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lVirtualLocalTimeDiff_st  = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocalLast_st = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocalNow_st  = {STBM_ZERO,STBM_ZERO};
    Std_ReturnType lRet_Val_u8;
    uint8 lIndex_u8;

    /* Local Variable Initialization */
    lRet_Val_u8  = E_NOT_OK;

    /*  [SWS_StbM_00177]For Time Domains 16 till 31 the StbM_GetCurrentTime() shall return for the requested
        Time Domain a Time Base calculated by adding the given offset (via StbM_SetOffset()) to the current
        Time Base of the referenced Time Domain via StbMOffsetTimeBase (ECUC_StbM_00030 : ).
        The timeBaseStatus of the referenced Time Domain shall be returned, except no Offset value is available.
        In this case (offset value is not available), the returned timestamp shall be of the referenced
        Time Domain and the timeBaseStatus shall be 0x00.  */

    /* Get the index for the referenced sync time base ID of the offset timebase  */
    lIndex_u8 = StbM_Index_au8[timeBaseId];

    /* for 16 to 31 timebase get the offset value and userdata  */
    lRet_Val_u8 = StbM_GetOffset(offset_timeBaseId,&lTimeStamp_st,&lUserData_st);

    if(lRet_Val_u8==E_OK)
    {
        /* Load the previous VLT*/
        lTimeStampVLocalLast_st = StbM_VirtualLocalTimeTupleArray_ast[lIndex_u8];

        /*Call the StbM_GetCurrentVirtualLocalTime_Internal to read the current Virtual Local Time */
        lRet_Val_u8 = StbM_GetCurrentVirtualLocalTime_Internal
                                                            (lIndex_u8,&lTimeStampVLocalNow_st,STBM_NO_COUNTER_UPDATE);

        if(lRet_Val_u8==E_OK)
        {
            /*Check if the Virtual time value difference */
            lVirtualLocalTimeDiff_st =
                                    StbM_Rb_VirtualLocalTimeDifference(lTimeStampVLocalLast_st,lTimeStampVLocalNow_st);

            /* Get the current time value of the referenced sync time base  */
            StbM_GetCurrentTime_Synchronised_Timebase
                                             (timeBaseId,lTimeStampVLocalNow_st,lVirtualLocalTimeDiff_st,timeStampPtr);

            /* Overflow check for the time value and updating the time value  */
            lStbM_TimesourceArray_st = StbM_Offset_Overflow_Check(lTimeStamp_st,timeStampPtr);

            /* Timeout check has to be done if the state of the StbM is SLAVE / GATEWAY  */
            if(StbM_Cfg_SynchronizedTimeBaseArray_ast[offset_index].StbMState_e != STBM_MASTER)
            {
                /*  Call the local function to Check whether the timeout has occurred  */
                StbM_GetCurrentTime_Timeout_Check(offset_index,&lTimeStampVLocalNow_st);
            }
            else
            {
                /* Do nothing - QAC */
            }

            /*  Value received is put into the global timebase structure  */
            timeStampPtr->nanoseconds    = lStbM_TimesourceArray_st.nanoseconds;
            timeStampPtr->seconds        = lStbM_TimesourceArray_st.seconds;
            timeStampPtr->secondsHi      = lStbM_TimesourceArray_st.secondsHi;
            timeStampPtr->timeBaseStatus = StbM_GlobalTimeTupleArray_ast[offset_index].timeBaseStatus;

            /* Update the Virtual Local Time of the corresponding Sync timebase */
            vLocalTimeStamp->nanosecondsHi = lTimeStampVLocalNow_st.nanosecondsHi;
            vLocalTimeStamp->nanosecondsLo = lTimeStampVLocalNow_st.nanosecondsLo;

        }
        else
        {
            /* Time value is not updated since the retuen value is E_NOT_OK*/
        }

    }
    else
    {
        /* Time value is not updated since the retuen value is E_NOT_OK*/
    }

    return (lRet_Val_u8);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_GetCurrentTime_Synchronised_Timebase
Syntax            : void StbM_GetCurrentTime_Synchronised_Timebase(
                                                uint8 index,
                                                StbM_VirtualLocalTimeType virtualLocalTimeNow,
                                                StbM_VirtualLocalTimeType virtualLocalTimeDiff,
                                                StbM_TimeStampType * timeStampPtr)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : index                - the sequence number of the Synchronised time base
                    virtualLocalTimeNow  - Virtual Local Time value difference
                    virtualLocalTimeDiff - Virtual Local Time difference
Parameters (inout): None
Parameters (out)  : timeStampPtr     -  Current time value for sync timebase
Description       : Current time calculation for Synchronised time base
**********************************************************************************************************************/

 void StbM_GetCurrentTime_Synchronised_Timebase(
         StbM_SynchronizedTimeBaseType timeBaseId,
         StbM_VirtualLocalTimeType virtualLocalTimeNow,
         StbM_VirtualLocalTimeType virtualLocalTimeDiff,
        StbM_TimeStampType * timeStampPtr)
{
    /* Local Variable declaration */
    StbM_TimeStampType lStbM_TimesourceArray_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lCurrentLocalTime_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    uint8 lIndex_u8;

    /* Local Variable Initialization */
    lIndex_u8 = StbM_Index_au8[timeBaseId];

    /* Convert the time value from StbM_VirtualLocalTimeType to StbM_TimeStampType format*/
    StbM_Rb_ConvertVirtualLocalTimeToTimestampType(virtualLocalTimeDiff,&lCurrentLocalTime_st);

    /*Check the overflow for the timebase*/
    lStbM_TimesourceArray_st = StbM_Overflow_Check(lIndex_u8,lCurrentLocalTime_st,STBM_GLOBAL_TIME_VALUE);

    /* Timeout check has to be done if the state of the StbM is SLAVE / GATEWAY  */
    if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMState_e != STBM_MASTER)
    {
        /*  Call the local function to Check whether the timeout has occurred  */
        StbM_GetCurrentTime_Timeout_Check(lIndex_u8,&virtualLocalTimeNow);
    }
    else
    {
        /* Do nothing - QAC */
    }

    /*  Value received is put into the global timebase structure  */
    timeStampPtr->nanoseconds    = lStbM_TimesourceArray_st.nanoseconds;
    timeStampPtr->seconds        = lStbM_TimesourceArray_st.seconds;
    timeStampPtr->secondsHi      = lStbM_TimesourceArray_st.secondsHi;
    timeStampPtr->timeBaseStatus = StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/*********************************************************************************************************************
Service name      : StbM_GetCurrentTimeWithoutRateCorrection
Syntax            : Std_ReturnType StbM_GetCurrentTimeWithoutRateCorrection(
                                            uint8 index,
                                            StbM_TimeStampType * globalTimeStamp,
                                            StbM_VirtualLocalTimeType * vlocalTimeStamp,
                                            StbM_UserDataType * userData)
Service ID[hex]   : None
Sync/Async        : None
Reentrancy        : None
Parameters (in)   : index            - the sequence number of the Synchronised time base
Parameters (inout): None
Parameters (out)  : globalTimeStamp  -  Current time value for sync timebase
                    vlocalTimeStamp  -  Current Virtual Local Time
                    userData         -  User data of the Time Base
Return value      : Std_ReturnType   -  E_OK    : successful
                                        E_NOT_OK: failed
Description       : Current time calculation for Synchronised time base
**********************************************************************************************************************/

Std_ReturnType StbM_GetCurrentTimeWithoutRateCorrection(
        uint8 index,
        StbM_TimeStampType * globalTimeStamp,
        const StbM_VirtualLocalTimeType * timeStampVLocalNow_st,
        StbM_UserDataType * userData)
{
    Std_ReturnType lRet_Val_u8;
    uint16 lTimeBaseId_u16,lTimeBaseIdSync_u16;
    StbM_VirtualLocalTimeType lTimeStampVLocalLast_st  = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocalNow_st  = {STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lVirtualLocalTimeDiff_st  = {STBM_ZERO,STBM_ZERO};

    /* Local variable initialization */
    lRet_Val_u8  = E_NOT_OK;
    lTimeBaseId_u16 = STBM_ZERO;
    lTimeBaseIdSync_u16 = STBM_ZERO;

    /* Copy the configured timebase ID into a local array */
    lTimeBaseId_u16 =
            StbM_Cfg_SynchronizedTimeBaseArray_ast[index].SynchronizedTimeBaseIdentifier_u16;

    /* Check if the TimeBase is synchronized or Pure Local TimeBase */
    if((StbM_Rb_IsSyncTimeBase(lTimeBaseId_u16)) || (StbM_Rb_IsPureLocalTimeBase(lTimeBaseId_u16)))
    {
        /* Load the previous VLT*/
        lTimeStampVLocalLast_st = StbM_VirtualLocalTimeTupleArray_ast[index];

        /*Check if the Virtual time value difference */
        lVirtualLocalTimeDiff_st =
                StbM_Rb_VirtualLocalTimeDifference(lTimeStampVLocalLast_st,(*timeStampVLocalNow_st));

        /* For synchronized and pure local timebase */
        StbM_GetCurrentTime_Synchronised_Timebase
                 (lTimeBaseId_u16,(*timeStampVLocalNow_st),lVirtualLocalTimeDiff_st,globalTimeStamp);

        lRet_Val_u8 = E_OK;

    }
    else
    {
        /* For offset timebase check the corresponding synctimebase*/
        lTimeBaseIdSync_u16 = StbM_Cfg_SynchronizedTimeBaseArray_ast[index].OffsetTimeBaseRef_u8;
        /*  Call the local function to calculate the time value  */
        lRet_Val_u8 = StbM_GetCurrentTime_Offset_Timebase
                                    (lTimeBaseId_u16,index,lTimeBaseIdSync_u16,globalTimeStamp,&lTimeStampVLocalNow_st);
    }

    /*  [SWS_StbM_00398]For Pure Local Time Bases StbM_GetCurrentTime() and StbM_GetCurrentTimeExtended() shall
        return the User Data as set by StbM_SetGlobalTime(), StbM_UpdateGlobalTime() or StbM_SetUserData() by the
        local Pure Local Time Master  */

    /* Copy the Userdata value from structure to the userdata pointer  */
    *userData = *(StbM_UserDataArray_ast+index);

    return lRet_Val_u8;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
 *Service name       : StbM_GetCurrentTime_Internal
 *Syntax             : Std_ReturnType StbM_GetCurrentTime_Internal(uint8, AUTOMATIC)index,
 *                                                       StbM_TimeStampType* timeStamp,
 *                                                       StbM_VirtualLocalTimeType* vlocalTime,
 *                                                       StbM_UserDataType* userData )
 *Service ID[hex]    : 0x07
 *Sync/Async         : Synchronous
 *Reentrancy         : Non Reentrant
 *Parameters (in)    : index            -  Index of the timebase
 *Parameters (inout) : None
 *Parameters (out)   : timeStamp        -  Current time stamp that is valid at this time
                       vlocalTime       -  Current Virtual Local Time
                       userData         -  User data of the Time Base
 *Return value       : Std_ReturnType   -  E_OK: successful
                                           E_NOT_OK: failed
 *Description        : Internal API - Returns a time value (Local Time Base derived from Global Time Base) in standard format.
 **********************************************************************************************************************/
Std_ReturnType StbM_GetCurrentTime_Internal(uint8 index,
        StbM_TimeStampType * timeStamp,
        const StbM_VirtualLocalTimeType * vlocalTime,
        StbM_UserDataType * userData)
{
    /* Local Variable declaration */
    StbM_TimeStampType lTimeStampGlobal_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
#if (STBM_TIME_CORRECTION == STD_ON )
    uint16 lTimeBaseId_u16;
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
    Std_ReturnType lRet_SNCB_u8; /* Return Value from StatusNotificationCallback */
    Std_ReturnType lRet_GCTWRC_u8; /* Return Value from StbM_GetCurrentTimeWithoutRateCorrection() */

    /* Local Variable Initialization */
#if (STBM_TIME_CORRECTION == STD_ON )
    lTimeBaseId_u16 = StbM_Cfg_SynchronizedTimeBaseArray_ast[index].SynchronizedTimeBaseIdentifier_u16;
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
    lRet_SNCB_u8 = E_NOT_OK;
    lRet_GCTWRC_u8  = E_NOT_OK;

    /* Read the current time value without rate correction */
    lRet_GCTWRC_u8 = StbM_GetCurrentTimeWithoutRateCorrection(index,&lTimeStampGlobal_st,vlocalTime,userData);

#if (STBM_TIME_CORRECTION == STD_ON )
    /* Check if the time correction container is configured for current timebase */
    if(StbM_Cfg_TimeCorrectionArray_ast[index].TimeCorrectionType_ten != STBM_TIME_CORRECTION_NOT_CONFIGURED)
    {

        if(lRet_GCTWRC_u8 == E_OK)
        {
            /* Time correction container is configured for current timebase, so the returned time value needs to be Rate corrected */
            *timeStamp = StbM_GetRateCorrectedTime(index, lTimeBaseId_u16, vlocalTime);
        }
        else
        {
            /*Return E_NOT_OK since the virtual local time is not returned correctly*/
            lRet_GCTWRC_u8 = E_NOT_OK;
        }
    }
    else
#endif /* #if (STBM_TIME_CORRECTION ==  STD_ON ) */
    {
        if(lRet_GCTWRC_u8 == E_OK)
        {
            /* Time correction container is not configured for current timebase, so the returned time value will not be Rate corrected */
            *timeStamp = lTimeStampGlobal_st;
        }
        else
        {
            /* Return E_NOT_OK since the current time is not returned correctly*/
            lRet_GCTWRC_u8 = E_NOT_OK;
        }
    }

    /* If any status event (refer to [SWS_StbM_00284]) occurs and the corresponding bit in the NotificationMask mask is set,
     * the StbM shall the callback function StbM_StatusNotificationCallback.If multiple status events occur simultaneously
     * for the same Time Base, the StbM shall call the callback function StbM_StatusNotificationCallback only once.
     * The StbM shall set the eventNotifications parameter of StbM_StatusNotificationCallback to the value of the NotificationEvents variable.*/

    lRet_SNCB_u8 = StbM_Rb_StatusNotificationCallback(index);

    if((lRet_GCTWRC_u8 != E_NOT_OK)&&(lRet_SNCB_u8 != E_NOT_OK))
    {
        lRet_GCTWRC_u8 = E_OK;
    }
    else
    {
        lRet_GCTWRC_u8 = E_NOT_OK;
    }
    return (lRet_GCTWRC_u8);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
 *Service name       : StbM_GetCurrentTime
 *Syntax             : Std_ReturnType StbM_GetCurrentTime( StbM_SynchronizedTimeBaseType timeBaseId,
 *                                                       StbM_TimeStampType* timeStamp,
 *                                                       StbM_UserDataType* userData )
 *Service ID[hex]    : 0x07
 *Sync/Async         : Synchronous
 *Reentrancy         : Non Reentrant
 *Parameters (in)    : timeBaseId       -  time base reference
 *Parameters (inout) : None
 *Parameters (out)   : timeStamp        -  Current time stamp that is valid at this time
                       userData         -  User data of the Time Base
 *Return value       : Std_ReturnType   -  E_OK: successful
                                           E_NOT_OK: failed
 *Description        : Returns a time value (Local Time Base derived from Global Time Base) in standard format.
 **********************************************************************************************************************/
/* [SWS_StbM_00195]  */
Std_ReturnType StbM_GetCurrentTime(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampType * timeStamp,
        StbM_UserDataType * userData)
{
    /* Local Variable declaration */
    StbM_TimeStampType lTimeStampGlobal_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocal_st  = {STBM_ZERO,STBM_ZERO};
    uint8 lIndex_u8;
    Std_ReturnType lRet_Val_u8;
    Std_ReturnType lRet_Val_GCVLTI_u8; /* Return Value from StbM_GetCurrentVirtualLocalTime_Internal() */

    /* Local Variable Initialization */
    lRet_Val_u8  = E_NOT_OK;
	lRet_Val_GCVLTI_u8 = E_NOT_OK;

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBaseID is valid */
        if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the userdata is NULL_PTR or not*/
            if(userData != NULL_PTR)
            {
                /* Check if the timeStamp is NULL_PTR or not*/
                if(timeStamp != NULL_PTR)
                {
                    lRet_Val_GCVLTI_u8 = StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocal_st,STBM_NO_COUNTER_UPDATE);

                    if(lRet_Val_GCVLTI_u8 == E_OK)
                    {
                        /* Call the API to get the current Global time value*/
                        lRet_Val_u8 = StbM_GetCurrentTime_Internal (lIndex_u8,
                                                                   &lTimeStampGlobal_st,
                                                                   &lTimeStampVLocal_st,
                                                                   userData);
                        if(lRet_Val_u8 == E_OK)
                        {
                           /* Copy the current global time value into the output parameter */
                           *timeStamp = lTimeStampGlobal_st;
                        }
                        else
                        {
                           /* since the return value is E_NOT_OK no Global time value is returned for the timebase */
                        }
                    }
                    else
                    {
                        /* since the return value is E_NOT_OK, StbM_GetCurrentTime_Internal() wont be called */
                    }
                }
                else
                {
                    /* [SWS_StbM_00197]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_GetCurrentTime()
                       shall report to DET the development error STBM_E_PARAM_POINTER, if called with an invalid pointer of
                       parameter timeStampPtr and userDataPtr  */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTTIME,STBM_E_PARAM_POINTER)
                }
            }
            else
            {
                /* [SWS_StbM_00197]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_GetCurrentTime()
                   shall report to DET the development error STBM_E_PARAM_POINTER, if called with an invalid pointer of
                   parameter timeStampPtr and userDataPtr  */
                /* If userDataPtr is NULL_PTR report error and exit function */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTTIME,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00196]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True, StbM_GetCurrentTime()
                shall report to DET the development error STBM_E_PARAM, if called with an invalid parameter timeBaseID
                To inform DET in case of invalid timebase  */

            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTTIME,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTTIME,STBM_E_NOT_INITIALIZED)
    }

    /*Return the return value of the API*/
    return (lRet_Val_u8);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/**********************************************************************************************************************
   Function name    : StbM_GetCurrentTimeExtended
   Syntax           : Std_ReturnType StbM_GetCurrentTimeExtended
                      ( StbM_SynchronizedTimeBaseType timeBaseId,
                      StbM_TimeStampExtendedType* timeStamp, StbM_UserDataType* userData )

   Description      : The function returns a time value (Local Time Base derived from Global Time Base)
                      in extended format.
   Parameter        : timeBaseID,timeStamp,userData
   Return value     : Std_ReturnType
 **********************************************************************************************************************/
/* [SWS_StbM_00200] */
#if(STBM_GET_CURRENT_TIME_EXTENDED==STD_ON)
Std_ReturnType StbM_GetCurrentTimeExtended( StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampExtendedType * timeStamp,
        StbM_UserDataType * userData)
{
    /* Local Variable declaration */
    StbM_TimeStampType lTimeStampPtr_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocal_st  = {STBM_ZERO,STBM_ZERO};
    Std_ReturnType lRet_Val_u8;
    Std_ReturnType lRet_Val_GCVLTI_u8; /* Return Value from StbM_GetCurrentVirtualLocalTime_Internal() */
    uint8 lIndex_u8;

    /* Local Variable Initialization */
    lRet_Val_u8 = E_NOT_OK;
    lRet_Val_GCVLTI_u8 = E_NOT_OK;

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBaseID is valid */
        if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the userdata is NULL_PTR or not*/
            if(userData != NULL_PTR)
            {
                /* Check if the timeStamp is NULL_PTR or not*/
                if(timeStamp != NULL_PTR)
                {
                    lRet_Val_GCVLTI_u8 = StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocal_st,STBM_NO_COUNTER_UPDATE);

                    if(lRet_Val_GCVLTI_u8 == E_OK)
                    {
                        lRet_Val_u8 = StbM_GetCurrentTime_Internal
                                                               (lIndex_u8,&lTimeStampPtr_st,&lTimeStampVLocal_st,userData);

                        /* once the value is read put it into the timeStampPtr  */
                        timeStamp->timeBaseStatus = lTimeStampPtr_st.timeBaseStatus ;
                        timeStamp->nanoseconds    = lTimeStampPtr_st.nanoseconds    ;
                        /* left shift the secondsHi by 32  */
                        timeStamp->seconds        = (uint64)(lTimeStampPtr_st.seconds)
                                                                            + ((uint64)(lTimeStampPtr_st.secondsHi) << 32);
                    }
                    else
                    {
                        /* since the return value is E_NOT_OK, StbM_GetCurrentTime_Internal() wont be called */
                    }
                }
                else
                {
                    /* [SWS_StbM_00202]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True,
                         StbM_GetCurrentTimeExtended() shall report to DET the development error STBM_E_PARAM_POINTER,
                         if called with an invalid pointer of parameter timeStampPtr and userDataPtr  */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTTIMEEXTENDED,STBM_E_PARAM_POINTER)
                }
            }
            else
            {
                /* [SWS_StbM_00202]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True,
                     StbM_GetCurrentTimeExtended() shall report to DET the development error STBM_E_PARAM_POINTER,
                     if called with an invalid pointer of parameter timeStampPtr and userDataPtr  */
                /* If userDataPtr is NULL_PTR report error and exit function */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTTIMEEXTENDED,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00201]  If the switch STBM_DEV_ERROR_DETECT (ECUC_StbM_00012 : ) is set to True,
               StbM_GetCurrentTimeExtended()shall report to DET the development error STBM_E_PARAM,
               if called with an invalid parameter timeBaseID  */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTTIMEEXTENDED,STBM_E_PARAM)
        }
    }
    else
    {
        /* DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETCURRENTTIMEEXTENDED,STBM_E_NOT_INITIALIZED)
    }
    return (lRet_Val_u8);

}
#endif /*#if(STBM_GET_CURRENT_TIME_EXTENDED==STD_ON)*/

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
 *Service name       : StbM_BusGetCurrentTime
 *Syntax             : Std_ReturnType StbM_BusGetCurrentTime( StbM_SynchronizedTimeBaseType timeBaseId,
                                                              StbM_TimeStampType* globalTimePtr,
                                                              StbM_VirtualLocalTimeType* localTimePtr,
                                                              StbM_UserDataType* userData )
 *Service ID[hex]    : 0x07
 *Sync/Async         : Synchronous
 *Reentrancy         : Non Reentrant
 *Parameters (in)    : timeBaseId       -  time base reference
 *Parameters (inout) : None
 *Parameters (out)   : globalTimePtr    -  Value of the local instance of the Global Time,
                                           which is sampled when the function is called
                       localTimePtr     -  Value of the Virtual Local Time,
                                           which is sampled when the function is called
                       userData         -  User data of the Time Base
 *Return value       : Std_ReturnType   -  E_OK: successful
                                           E_NOT_OK: failed
 *Description        : Returns the current Time Tuple, status and User Data of the Time Base..
 **********************************************************************************************************************/
/* [SWS_StbM_91005]  */
/* HIS METRIC LEVEL VIOLATION IN StbM_BusGetCurrentTime: This is highly optimized and proven code. */
Std_ReturnType StbM_BusGetCurrentTime(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeStampType * globalTimePtr,
        StbM_VirtualLocalTimeType * localTimePtr,
        StbM_UserDataType * userData)
{
    /* Local Variable declaration */
    StbM_TimeStampType lTimeStampGlobal_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocal_st  = {STBM_ZERO,STBM_ZERO};
    Std_ReturnType lRet_Val_u8;
    Std_ReturnType lRet_Val_GCVLTI_u8; /* Return Value from StbM_GetCurrentVirtualLocalTime_Internal() */
    uint8 lIndex_u8;

    /* Local Variable Initialization */
    lRet_Val_u8  = E_NOT_OK;
    lRet_Val_GCVLTI_u8 = E_NOT_OK;

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBaseID is a valid SyncTimeBaseID */
        if(StbM_Rb_IsSyncTimeBase(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the userdata is NULL_PTR or not*/
            if(userData != NULL_PTR)
            {
                /* Check if the globalTimePtr is NULL_PTR or not*/
                if(globalTimePtr != NULL_PTR)
                {
                    /* Check if the localTimePtr is NULL_PTR or not*/
                    if(localTimePtr != NULL_PTR)
                    {
                        /*[SWS_StbM_00435] For Time Base 0 to 15 StbM_BusGetCurrentTime() shall return for the requested Time Domain
      the Time Tuple [TL;TV] of the Time Base, the related Status and the User Data. The current time of the Time Base
      shall be derived from the associated Virtual Local Time, which is derived from either the referenced OS counter,
      a GPT or a referenced Ethernet controller (refer to StbMLocalTimeHardware).*/

                        lRet_Val_GCVLTI_u8 = StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocal_st,STBM_NO_COUNTER_UPDATE);

                        if(lRet_Val_GCVLTI_u8 == E_OK)
                        {
                            /* Call the API to get the current Global time value*/
                            lRet_Val_u8 = StbM_GetCurrentTime_Internal
                                                           (lIndex_u8,&lTimeStampGlobal_st,&lTimeStampVLocal_st,userData);
                            if(lRet_Val_u8 == E_OK)
                            {
                                /* Copy the current global time value into the output parameter */
                                *globalTimePtr = lTimeStampGlobal_st;

                                /* Assign the converted time value to the output parameter */
                                *localTimePtr = lTimeStampVLocal_st;
                            }
                            else
                            {
                                /* since the return value is E_NOT_OK no Global time value/Virtual Local Time is returned for the timebase */
                            }
                        }
                        else
                        {
                            /* since the return value is E_NOT_OK, StbM_GetCurrentTime_Internal() wont be called */
                        }
                    }
                    else
                    {
                        /* [SWS_StbM_00447]
                        If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                        StbM_BusGetCurrentTime() shall report to DET the development error STBM_E_PARAM_POINTER,
                        if called with a NULL pointer for parameter globalTimePtr, localTimePtr or userData  */
                        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSGETCURRENTTIME,STBM_E_PARAM_POINTER)
                    }
                }
                else
                {
                    /* [SWS_StbM_00447]
                      If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                      StbM_BusGetCurrentTime() shall report to DET the development error STBM_E_PARAM_POINTER,
                      if called with a NULL pointer for parameter globalTimePtr, localTimePtr or userData  */
                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSGETCURRENTTIME,STBM_E_PARAM_POINTER)
                }
            }
            else
            {
                /* [SWS_StbM_00447]
                    If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                    StbM_BusGetCurrentTime() shall report to DET the development error STBM_E_PARAM_POINTER,
                    if called with a NULL pointer for parameter globalTimePtr, localTimePtr or userData  */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSGETCURRENTTIME,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /* [SWS_StbM_00446]
                   If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                   StbM_BusGetCurrentTime() shall report to DET the development error STBM_E_PARAM,
                   if called with a parameter timeBaseId, which is
                   1.referring to an Offset Time Base
                   2.not configured or
                   3.within the reserved value range.*/
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSGETCURRENTTIME,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_BUSGETCURRENTTIME,STBM_E_NOT_INITIALIZED)
    }

    return (lRet_Val_u8);
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
* Service name      : StbM_GetTimeBaseStatus
* Syntax            : Std_ReturnType StbM_GetTimeBaseStatus(
*                     StbM_SynchronizedTimeBaseType timeBaseId,
*                     StbM_TimeBaseStatusType* syncTimeBaseStatus,
*                     StbM_TimeBaseStatusType* offsetTimeBaseStatus )
* Service ID[hex]   : 0x14
* Sync/Async        : Synchronous
* Reentrancy        : Reentrant
* Parameters (in)   : timeBaseId           -  time base reference
* Parameters (inout): None
* Parameters (out)  : syncTimeBaseStatus   -  Status of the Synchronized Time Base
offsetTimeBaseStatus -  Status of the Offset Time Base
* Return value      : Std_ReturnType E_OK: successful
*                                    E_NOT_OK: failed
* Description       : Returns the detailed status of the Time Base. For Offset Time Bases the status of the
*                     Offset Time Base itself and the status of the underlying Synchronized Time Base is returned.
**********************************************************************************************************************/
/* [SWS_StbM_00263] */
/* HIS METRIC LEVEL VIOLATION IN StbM_GetTimeBaseStatus: This is highly optimized and proven code. */
Std_ReturnType StbM_GetTimeBaseStatus(
        StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_TimeBaseStatusType * syncTimeBaseStatus,
        StbM_TimeBaseStatusType * offsetTimeBaseStatus )
{
    /* Local Variable declaration */
    Std_ReturnType lRet_Val_u8; /* Return value of StbM_GetTimeBaseStatus() */
    Std_ReturnType lRet_GCWRSync_u8; /* Return Value from StbM_GetCurrentTimeWithoutRateCorrection() for Sync TB */
    Std_ReturnType lRet_GCWROffset_u8; /* Return Value from StbM_GetCurrentTimeWithoutRateCorrection() for Offset TB */
    Std_ReturnType lRet_Val_GCVLTI_u8; /* Return Value from StbM_GetCurrentVirtualLocalTime_Internal() */
    uint8 lIndex_u8;
    uint8 lIndexOfRefSync_u8;
    uint16 lTimeBaseId_u16;
	StbM_TimeStampType lTimeStamp_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocal_st  = {STBM_ZERO,STBM_ZERO};
    StbM_UserDataType lUserData_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lTimeStamp_Sync_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_UserDataType lUserData_Sync_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};

    /* Local Variable Initialization */
    lRet_Val_u8  = E_NOT_OK;
    lRet_GCWRSync_u8  = E_NOT_OK;
    lRet_GCWROffset_u8  = E_NOT_OK;
	lRet_Val_GCVLTI_u8 = E_NOT_OK;
    lIndexOfRefSync_u8 = STBM_ZERO;

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBaseID is valid */
        if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the syncTimeBaseStatus is NULL_PTR or not*/
            if(syncTimeBaseStatus != NULL_PTR)
            {
                /* Check if the offsetTimeBaseStatus is NULL_PTR or not*/
                if(offsetTimeBaseStatus != NULL_PTR)
                {
                    /*[SWS_StbM_00262]For Synchronized Time Bases StbM_GetTimeBaseStatus() shall return
        -> the status of the corresponding Synchronized Time Base via syncTimeBaseStatus and
        ->0 via offsetTimeBaseStatus
        For Offset Time Bases StbM_GetTimeBaseStatus() shall return
        -> the status of the corresponding Offset Time Base via offsetTimeBaseStatus and
        -> the status of the related Synchronized Time Base via syncTimeBaseStatus */

                    /* Check if the timebase is a Synchronized or Pure local timebase*/
                    if((StbM_Rb_IsSyncTimeBase(timeBaseId)) || (StbM_Rb_IsPureLocalTimeBase(timeBaseId)))
                    {
                        lRet_Val_GCVLTI_u8 = StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocal_st,STBM_NO_COUNTER_UPDATE);

                        if(lRet_Val_GCVLTI_u8 == E_OK)
                        {
                            /* Call the StbM_GetCurrentTimeWithoutRateCorrection API to check whether the timeout has occured or not for the timebase*/
                            lRet_GCWRSync_u8 = StbM_GetCurrentTimeWithoutRateCorrection
                                                           (lIndex_u8,&lTimeStamp_st,&lTimeStampVLocal_st,&lUserData_st);

                            if (lRet_GCWRSync_u8 == E_OK)
                            {
                                /* Update the timebasestatus for the Sync and the Offset timebase*/
                                *syncTimeBaseStatus   = lTimeStamp_st.timeBaseStatus;
                                *offsetTimeBaseStatus = STBM_ZERO;
                                lRet_Val_u8 = E_OK;
                            }
                            else
                            {
                                /* Timebasestatus is not updated since the return value of the API StbM_GetCurrentTime_Synchronised_Timebase is E_NOT_OK */
                                *syncTimeBaseStatus   = STBM_ZERO;
                                *offsetTimeBaseStatus = STBM_ZERO;
                            }
                        }
                        else
                        {
                            /* since the return value is E_NOT_OK, StbM_GetCurrentTimeWithoutRateCorrection() wont be called */
                        }
                    }
                    else
                    {
                        /* Get the referenced sync time base ID for the offset time base  */
                        lTimeBaseId_u16 = StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].OffsetTimeBaseRef_u8;

                        /* Get the index of the referenced sync time base ID for the offset time base  */
                        lIndexOfRefSync_u8 = StbM_Index_au8[lTimeBaseId_u16];

                        lRet_Val_GCVLTI_u8 = StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocal_st,STBM_NO_COUNTER_UPDATE);

                        if(lRet_Val_GCVLTI_u8 == E_OK)
                        {
                            /* Call the StbM_GetCurrentTimeWithoutRateCorrection API to check whether the timeout has occured or not for referenced sync timebase*/
                            lRet_GCWRSync_u8 = StbM_GetCurrentTimeWithoutRateCorrection
                                    (lIndexOfRefSync_u8,&lTimeStamp_Sync_st,&lTimeStampVLocal_st,&lUserData_Sync_st);

                            /* Call the StbM_GetCurrentTimeWithoutRateCorrection API to check whether the timeout has occured or not for the timebase*/
                            lRet_GCWROffset_u8 = StbM_GetCurrentTimeWithoutRateCorrection
                                    (lIndex_u8,&lTimeStamp_st,&lTimeStampVLocal_st,&lUserData_st);

                            if ((lRet_GCWRSync_u8 == E_OK) && (lRet_GCWROffset_u8 == E_OK))
                            {
                                /* Update the timebasestatus for the Sync and the Offset timebase*/
                                *syncTimeBaseStatus   = lTimeStamp_Sync_st.timeBaseStatus;
                                *offsetTimeBaseStatus = lTimeStamp_st.timeBaseStatus;
                                lRet_Val_u8 = E_OK;
                            }
                            else
                            {
                                /* Timebasestatus is not updated since the return value of the API StbM_GetCurrentTime_Offset_Timebase is E_NOT_OK */
                                *syncTimeBaseStatus   = STBM_ZERO;
                                *offsetTimeBaseStatus = STBM_ZERO;
                            }
                        }
                        else
                        {
                            /* since the return value is E_NOT_OK, StbM_GetCurrentTimeWithoutRateCorrection() wont be called */
                        }
                    }
                }
                else
                {
                    /*[SWS_StbM_00386] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_GetTimeBaseStatus()
                         shall report to DET the development error STBM_E_PARAM_POINTER, if called with a NULL pointer for parameter
                         syncTimeBaseStatus or offsetTimeBaseStatus.*/

                    STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETTIMEBASESTATUS,STBM_E_PARAM_POINTER)
                }
            }
            else
            {
                /*[SWS_StbM_00386] If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_GetTimeBaseStatus()
                      shall report to DET the development error STBM_E_PARAM_POINTER, if called with a NULL pointer for parameter
                      syncTimeBaseStatus or offsetTimeBaseStatus.*/
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETTIMEBASESTATUS,STBM_E_PARAM_POINTER)
            }
        }
        else
        {
            /*[SWS_StbM_00264]If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE,
                   StbM_GetTimeBaseStatus() shall report to DET
                   the development error STBM_E_PARAM, if called with a parameter timeBaseID, which
                   -> is not configured or
                   -> is within the reserved value range.  */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETTIMEBASESTATUS,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETTIMEBASESTATUS,STBM_E_NOT_INITIALIZED)
    }

    return (lRet_Val_u8);
}
#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
* Service name      : StbM_GetMasterConfig
* Syntax            : Std_ReturnType StbM_GetMasterConfig(
*                     StbM_SynchronizedTimeBaseType timeBaseId,
*                     StbM_MasterConfigType* masterConfig )
* Service ID[hex]   : 0x1d
* Sync/Async        : Synchronous
* Reentrancy        : Non Reentrant
* Parameters (in)   : timeBaseId           -  time base reference
* Parameters (inout): None
* Parameters (out)  : masterConfig   -  Indicates, if system wide master functionality is supported
* Return value      : Std_ReturnType E_OK: successful
*                                    E_NOT_OK: failed
* Description       :Indicates if the functionality for a system wide master (e.g. StbM_SetGlobalTime) for a given
*                    Time Base is available or not.
**********************************************************************************************************************/
Std_ReturnType StbM_GetMasterConfig(StbM_SynchronizedTimeBaseType timeBaseId,
        StbM_MasterConfigType * masterConfig)
{
    /* Local Variable declaration */
    Std_ReturnType lRet_Val_u8;
    uint8 lIndex_u8;

    /* Local Variable Initialization */
    lRet_Val_u8  = E_NOT_OK;
    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBaseID is valid */
        if(StbM_Rb_IsTimeBaseIdValid(timeBaseId))
        {
            /* Check if the masterConfig is NULL_PTR or not*/
            if(masterConfig != NULL_PTR)
            {
                /* For a timebase within the range, Index is loaded */
                lIndex_u8 = StbM_Index_au8[timeBaseId];

                /* [SWS_StbM_00408]StbM_GetMasterConfig() shall return the value of the configuration parameter
StbMIsSystemWideGlobalTimeMaster  for the Time Base timeBaseId. This is to check
if the StbM is configured as system wide Global Time Master for a specific Time Base.          */
                if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].IsSystemWideGlobalTimeMaster_b == TRUE)
                {
                    *masterConfig = STBM_SYSTEM_WIDE_MASTER_ENABLED ;
                    lRet_Val_u8 = E_OK;
                }
                else
                {
                    *masterConfig = STBM_SYSTEM_WIDE_MASTER_DISABLED ;
                    lRet_Val_u8 = E_OK;
                }
            }
            else
            {
                /*  [SWS_StbM_00xxx]
                If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_GetMasterConfig() shall report to DET the
                development error STBM_E_PARAM_POINTER, if called with a NULL pointer for  parameter masterConfig.      */
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETMASTERCONFIG,STBM_E_PARAM_POINTER);
            }
        }
        else
        {
            /*  [SWS_StbM_00xxx]
            If the switch StbMDevErrorDetect (ECUC_StbM_00012 : ) is set to TRUE, StbM_GetMasterConfig() shall report to DET the development error STBM_E_PARAM, if called with a parameter timeBaseID, which
                -> is not configured or
                -> is within the reserved value range   */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETMASTERCONFIG,STBM_E_PARAM)
        }
    }
    else
    {
        /* DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETMASTERCONFIG,STBM_E_NOT_INITIALIZED)
    }
    return (lRet_Val_u8);

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"
/*********************************************************************************************************************
* Service name      : StbM_GetTimeLeap
* Syntax            : Std_ReturnType StbM_GetTimeLeap(
*                     StbM_SynchronizedTimeBaseType timeBaseId,
*                     StbM_TimeDiffType* timeJump )
* Service ID[hex]   : 0x13
* Sync/Async        : Synchronous
* Reentrancy        : Reentrant
* Parameters (in)   : timeBaseId           -  time base reference
* Parameters (inout): None
* Parameters (out)  : timeJump   -  Time leap value
* Return value      : Std_ReturnType E_OK: successful
*                                    E_NOT_OK: failed
* Description       :Returns value of Time Leap, if StbMTimeLeapFuture/PastThreshold is exceeded.
**********************************************************************************************************************/
/* HIS METRIC LEVEL VIOLATION IN StbM_GetTimeLeap: This is highly optimized and proven code. */
Std_ReturnType StbM_GetTimeLeap(StbM_SynchronizedTimeBaseType timeBaseId,
StbM_TimeDiffType * timeJump)
{
    /* Local Variable declaration */
    Std_ReturnType lRet_Val_u8;
    Std_ReturnType lRet_GCTWRC_u8; /* Return Value from StbM_GetCurrentTimeWithoutRateCorrection() */
    Std_ReturnType lRet_Val_GCVLTI_u8; /* Return Value from StbM_GetCurrentVirtualLocalTime_Internal() */
    uint8 lIndex_u8;
    StbM_TimeStampType lTimeStampPtr_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_TimeStampType lDiff_Time_st = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_UserDataType lUserDataPtr_st  = {STBM_ZERO,STBM_ZERO,STBM_ZERO,STBM_ZERO};
    StbM_VirtualLocalTimeType lTimeStampVLocal_st  = {STBM_ZERO,STBM_ZERO};
    uint8 lgreaterTimeStamp_u8;

    /* Local Variable Initialization */
    lRet_Val_u8  = E_NOT_OK;
    lRet_GCTWRC_u8  = E_NOT_OK;
    lRet_Val_GCVLTI_u8  = E_NOT_OK;
    lgreaterTimeStamp_u8  = STBM_ZERO;

    /* Initialize the local variable with Invalid Index */
    lIndex_u8 = STBM_INVALID_INDEX;

    /* Check whether module is initialized */
    if(StbM_InitState_b!=FALSE)
    {
        /* Check if the TimeBase is Synchronized or Offest TimeBase */
        if(StbM_Rb_IsSyncTimeBase(timeBaseId) || StbM_Rb_IsOffsetTimeBase(timeBaseId))
        {
            /* For a timebase within the range, Index is loaded */
            lIndex_u8 = StbM_Index_au8[timeBaseId];

            /* Check if the timeJump is NULL_PTR or not*/
            if(timeJump != NULL_PTR)
            {
                /* Deviation is taken that in the case of integer overflow for the parameter timeJump and non requirement of
      TimeLeap functionality and until the second valid invocation of StbM_BusSetGlobalTime() for the corresponding Time Base
      E_NOT_OK is returned.
      (AUTOSAR Bugzilla Ref ID:RfC  78023 - [StbM]: Clarification required on return value of the API 'StbM_GetTimeLeap()')    */

                /* [SWS_StbM_00425]
      For Time Slaves and Time Gateways StbM_GetTimeLeap() shall return the Global Time difference between the
      Received Time and the Synclocal Time, i.e., TGRx - TLSync, which is calculated upon each, except the very first,
      valid invocation of StbM_BusSetGlobalTime() for the corresponding Time Base.
      With
      TLSync = Global Time part of the Synclocal Time Tuple
      TGRx = Global Time part of the Received Time Tuple
      If the calculated time difference exceeds the value range of the timeJump parameter of StbM_GetTimeLeap()
      the returned time difference shall be limited to either the maximum negative or the maximum positive value of
      the type of timeJump (refer to StbM_TimeDiffType).
      StbM_GetTimeLeap() shall return E_NOT_OK until the second valid invocation of StbM_BusSetGlobalTime()
      for the corresponding Time Base.*/

                /* Check whether TimeLeapMonitor is ON */
                if(StbM_Cfg_SynchronizedTimeBaseArray_ast[lIndex_u8].StbMClearTimeleapCount_u16 != STBM_ZERO)
                {
                    /* Check if at least one timestamp for the Time Base has been successfully received before.*/
                    if(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[lIndex_u8].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT))
                    {
                        lRet_Val_GCVLTI_u8 = StbM_GetCurrentVirtualLocalTime_Internal(lIndex_u8,&lTimeStampVLocal_st,STBM_NO_COUNTER_UPDATE);

                        if(lRet_Val_GCVLTI_u8 == E_OK)
                        {
                            /* Get the current time for particular time base TLSync = Global Time part of the Synclocal Time Tuple */
                            lRet_GCTWRC_u8 =  StbM_GetCurrentTimeWithoutRateCorrection
                                    (lIndex_u8,&lTimeStampPtr_st,&lTimeStampVLocal_st,&lUserDataPtr_st);

                            if(lRet_GCTWRC_u8 == E_OK)
                            {
                                /* Calculate the difference between the new time-TGRx and the current time-TLSync*/
                                StbM_Time_DifferenceCalculation(&StbM_GlobalTimeReception_ast[lIndex_u8],
                                        &lTimeStampPtr_st,&lDiff_Time_st,&lgreaterTimeStamp_u8);

                                /* Check if secondsHi value is equal to zero and check for seconds and nanoseconds overflow  */
                                /* MR12 RULE 10.4 VIOLATION: The conversion of unsigned int to singned int is necessary in this case*/
                                if((lDiff_Time_st.secondsHi == STBM_ZERO)&&
                                 ((lDiff_Time_st.seconds) < ((uint32)((STBM_INT_MAX32 / STBM_NS_PER_SEC) + (STBM_ONE))))&&
                                 ((lDiff_Time_st.nanoseconds) <
                                            ((uint32)(STBM_INT_MAX32 - ((lDiff_Time_st.seconds) * (STBM_NS_PER_SEC))))))
                                {
                                    /* Check if TimeleapFuture has occured */
                                    if(lgreaterTimeStamp_u8 == STBM_ONE)
                                    {
                                        /* Add nanoseconds to the seconds part */
                                        *timeJump = ((((sint32)lDiff_Time_st.seconds) * ((sint32)STBM_NS_PER_SEC)) +
                                                ((sint32)lDiff_Time_st.nanoseconds));
                                        lRet_Val_u8 = E_OK;
                                    }
                                    /* If TimeleapPast has occured */
                                    else
                                    {
                                        /* Add nanoseconds to the seconds part */
                                        *timeJump = (-((((sint32)lDiff_Time_st.seconds) * ((sint32)STBM_NS_PER_SEC)) +
                                                ((sint32)lDiff_Time_st.nanoseconds)));
                                        lRet_Val_u8 = E_OK;
                                    }

                                }
                                else
                                {
                                    /* Do nothing - QAC */
                                }

                            }
                            else
                            {
                                /* Do nothing - QAC */
                            }
                        }
                        else
                        {
                            /* since the return value is E_NOT_OK, StbM_GetCurrentTimeWithoutRateCorrection() wont be called */
                        }
                    }
                    else
                    {
                        /* Do nothing - QAC */
                    }
                }
                else
                {
                    /* Do nothing - QAC */
                }
            }
            else
            {
                /*   [SWS_StbM_00269] If the switch StbMDevErrorDetect is set to TRUE, StbM_GetTimeLeap() shall report
                        to DET the development error STBM_E_PARAM_POINTER, if called with a NULL pointer for parameter timeJump.*/
                STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETTIMELEAP,STBM_E_PARAM_POINTER);
            }
        }
        else
        {
            /* If the switch StbMDevErrorDetect is set to TRUE, StbM_GetTimeLeap() shall report to DET the development
                    error STBM_E_PARAM, if called with a parameter timeBaseID, which
                    -> is not configured or
                    -> refers to a Pure Local Time Base or
                    -> is within the reserved value range.                            */
            STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETTIMELEAP,STBM_E_PARAM)
        }
    }
    else
    {
        /* [SWS_StbM_00198]DET error, if StbM is not intialised  */
        STBM_DET_REPORT_ERROR_NOK(STBM_INSTANCE_ID,STBM_APIID_GETTIMELEAP,STBM_E_NOT_INITIALIZED)
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
