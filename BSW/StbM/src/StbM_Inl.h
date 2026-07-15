

#ifndef STBM_INL_H
#define STBM_INL_H

/**
 ***************************************************************************************************
 * \moduledescription
 *                      StbM Inline function definition file
 *
 * \scope               This is included only by StbM module
 ***************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
#if (STBM_ECUC_RB_RTE_IN_USE == STD_ON)
/*The function declarations of RTE interfaced API's will be generated in the Rte_StbM_Type.h */
#include "Rte_StbM_Type.h"
#else
#include "StbM_Types.h"
#endif  /*(STBM_ECUC_RB_RTE_IN_USE == STD_ON) */
#include "StbM_Priv.h"

/*
 **********************************************************************************************************************
 * Define/Macros
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

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

/*
 **********************************************************************************************************************
 * INLINE function declarations
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * INLINE function definitions
 **********************************************************************************************************************
 */

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to Set Event the Bit */
LOCAL_INLINE void StbM_SetEvent_Bit(StbM_TimeBaseNotificationType * const Number,
                                                  uint8 Position)
{

    *Number |= 1u<<Position;

}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to Check the Event Bit */
LOCAL_INLINE boolean StbM_CheckEvent_Bit(StbM_TimeBaseNotificationType Number,uint8 Position)
{
    boolean lRetval_b;
    lRetval_b = FALSE;

    lRetval_b = (
                 STBM_ZERO !=((Number >> Position) & STBM_ONE)
                );

    return lRetval_b;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to Set the Bit */
LOCAL_INLINE uint8 StbM_Set_Bit(uint8 Number, uint8 Position
                  ,StbM_TimeBaseNotificationType * const eventPtr,
                   StbM_TimeBaseNotificationType Mask,   uint8 eventPos)
{

    Number |= 1u<<Position;
    if(StbM_CheckEvent_Bit(Mask,eventPos))
    {
        StbM_SetEvent_Bit(eventPtr,eventPos);
    }
     return Number;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to Reset the Bit */
LOCAL_INLINE uint8 StbM_Reset_Bit(uint8 Number, uint8 Position
                                    ,StbM_TimeBaseNotificationType * const eventPtr,
                                    StbM_TimeBaseNotificationType Mask,   uint8 eventPos)
{
    Number &= ~(1u << Position);

    if(StbM_CheckEvent_Bit(Mask,eventPos))
    {
        StbM_SetEvent_Bit(eventPtr,eventPos);
    }

    return Number;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to Check the Bit */
LOCAL_INLINE boolean StbM_Check_Bit(uint8 Number,uint8 Position)
{
    boolean lRetval_b;
    lRetval_b = FALSE;

    lRetval_b = (
                 STBM_ZERO !=((Number >> Position) & STBM_ONE)
                );

    return lRetval_b;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to Compare the Bit */
LOCAL_INLINE boolean StbM_Compare_Bit(uint8 Number1,uint8 Number2,uint8 Position)
{
    boolean lRetval_b;
    lRetval_b = FALSE;
	/* Check if Number 1 and Numer 2 are having different bit values in the Position passed */
    if(((Number1 >> Position) & STBM_ONE) != ((Number2 >> Position) & STBM_ONE))
    {
        /* return TRUE, if the bit value in the mentioned Position is different in both numbers */
        lRetval_b = TRUE;
    }
    else
    {
        /* return FALSE, if the bit value in the mentioned Position is same in both numbers */
        lRetval_b = FALSE;
    }

    return lRetval_b;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to Copy the Bit */
LOCAL_INLINE uint8 StbM_Copy_Bit(uint8 Number,
                                                   uint8 Position,boolean bit_b)
{
    /* If the bit is 1 then set the bit , if the bit is 0 reset the bit */
    if(bit_b)
    {
        Number |= 1u<<Position;
    }
    else
    {
        Number &= ~(1u << Position);
    }

    return Number;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to Copy the Bit */
LOCAL_INLINE uint8 StbM_Rb_IsUserDataValid(const StbM_UserDataType * userData)
{
    uint8 lRetval_u8;
    lRetval_u8 = STBM_USER_DATA_INVALID;

    /* APIs StbM_SetGlobalTime(), StbM_UpdateGlobalTime(), StbM_BusSetGlobalTime(), StbM_SetOffset() can be called
     * with UserData as NULL_PTR, which means there is no New User Data. Hence do nothing but return TRUE
     */
    if(userData == NULL_PTR)
    {
        lRetval_u8 = STBM_USER_DATA_NULL_PTR;
    }
    else
    {
        /* Check if the UserDataLength passed is less than or equal to 3 */
        if(userData->userDataLength <= STBM_USERDATA_LENGTH_MAX)
        {
            lRetval_u8 = STBM_USER_DATA_VALID;
        }
        else /* UserDataLength is invalid, as length is Greater than 3 */
        {
            lRetval_u8 = STBM_USER_DATA_INVALID;
        }
    }
    return lRetval_u8;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to check if the timebaseId is valid */
LOCAL_INLINE boolean StbM_Rb_IsTimeBaseIdValid(StbM_SynchronizedTimeBaseType timeBaseId)
{
    boolean lRetval_b;
    lRetval_b = FALSE;

    /* Check if TimeBaseID is within valid Range */
    if(timeBaseId < STBM_MAX_NUM_TIMEBASES)
    {
        /* Check if Index of the TimeBaseID is not INVALID. If TimeBaseID is not configured one,
         * then the Index will be STBM_INVALID_INDEX
         */
        if(StbM_Index_au8[timeBaseId] != STBM_INVALID_INDEX)
        {
            lRetval_b = TRUE;
        }
        else
        {
            lRetval_b = FALSE;
        }
    }
    else
    {
        lRetval_b = FALSE;
    }
    return lRetval_b;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to check if the timebase is Sync TimeBase */
LOCAL_INLINE boolean StbM_Rb_IsSyncTimeBase(StbM_SynchronizedTimeBaseType timeBaseId)
{
    boolean lRetval_b;
    lRetval_b = FALSE;

    /* Check if TimeBaseID is within Sync TimeBaseID Range */
    if(timeBaseId <= STBM_SYNC_TIMEBASES_MAX)
    {
        /* Check if Index of the TimeBaseID is not INVALID. If TimeBaseID is not configured one,
         * then the Index will be STBM_INVALID_INDEX
         */
        if(StbM_Index_au8[timeBaseId] != STBM_INVALID_INDEX)
        {
            lRetval_b = TRUE;
        }
        else
        {
            lRetval_b = FALSE;
        }
    }
    else
    {
        lRetval_b = FALSE;
    }
    return lRetval_b;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to check if the timebase is Offset TimeBase */
LOCAL_INLINE boolean StbM_Rb_IsOffsetTimeBase(StbM_SynchronizedTimeBaseType timeBaseId)
{
    boolean lRetval_b;
    lRetval_b = FALSE;

    /* Check if TimeBaseID is within Offset TimeBaseID Range */
    if((timeBaseId >= STBM_OFFSET_TIMEBASES_MIN) && (timeBaseId <= STBM_OFFSET_TIMEBASES_MAX))
    {
        /* Check if Index of the TimeBaseID is not INVALID. If TimeBaseID is not configured one,
         * then the Index will be STBM_INVALID_INDEX
         */
        if(StbM_Index_au8[timeBaseId] != STBM_INVALID_INDEX)
        {
            lRetval_b = TRUE;
        }
        else
        {
            lRetval_b = FALSE;
        }
    }
    else
    {
        lRetval_b = FALSE;
    }
    return lRetval_b;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/* Local inline function to check if the timebase is PureLocal TimeBase */
LOCAL_INLINE boolean StbM_Rb_IsPureLocalTimeBase(StbM_SynchronizedTimeBaseType timeBaseId)
{
    boolean lRetval_b;
    lRetval_b = FALSE;

    /* Check if TimeBaseID is within Pure Local TimeBaseID Range */
    if((timeBaseId >= STBM_PURELOCAL_TIMEBASES_MIN) && (timeBaseId <= STBM_PURELOCAL_TIMEBASES_MAX))
    {
        /* Check if Index of the TimeBaseID is not INVALID. If TimeBaseID is not configured one,
         * then the Index will be STBM_INVALID_INDEX
         */
        if(StbM_Index_au8[timeBaseId] != STBM_INVALID_INDEX)
        {
            lRetval_b = TRUE;
        }
        else
        {
            lRetval_b = FALSE;
        }
    }
    else
    {
        lRetval_b = FALSE;
    }
    return lRetval_b;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

LOCAL_INLINE Std_ReturnType StbM_Rb_StatusNotificationCallback(uint8 Index)
{
    Std_ReturnType lRet_SNCB_u8;
    lRet_SNCB_u8 = E_NOT_OK;
    if((StbM_Cfg_SynchronizedTimeBaseArray_ast[Index].StbMStatusNotificationCallback != NULL_PTR) &&
                                                            (StbM_NotificationEvents_au32[Index] != STBM_ZERO))
    {
        lRet_SNCB_u8 = StbM_Cfg_SynchronizedTimeBaseArray_ast[Index].StbMStatusNotificationCallback
                (*(StbM_NotificationEvents_au32+Index)) ;
        StbM_NotificationEvents_au32[Index] = STBM_ZERO ;
    }
    else
    {
        lRet_SNCB_u8 = E_OK ;
    }
    return lRet_SNCB_u8;
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

LOCAL_INLINE void StbM_Rb_UpdateTimeBaseStatusBits(uint8 Index, StbM_TimeBaseNotificationType NotifyMask_u32)
{
    if(!(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT)))
    {
        StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus =
                StbM_Set_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_GLOBAL_TIME_BASE_BIT,
                (StbM_NotificationEvents_au32+Index),NotifyMask_u32,STBM_EV_GLOBAL_TIME_BASE);
    }
    if(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_SYNC_TO_GATEWAY_BIT))
    {
        StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus =
                StbM_Reset_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_SYNC_TO_GATEWAY_BIT,
                (StbM_NotificationEvents_au32+Index),NotifyMask_u32,STBM_EV_SYNC_TO_GLOBAL_MASTER);
    }
    if(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_TIMELEAP_FUTURE_BIT))
    {
        StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus =
            StbM_Reset_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_TIMELEAP_FUTURE_BIT,
            (StbM_NotificationEvents_au32+Index),NotifyMask_u32,STBM_EV_TIMELEAP_FUTURE_REMOVED);
    }
    if(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_TIMELEAP_PAST_BIT))
    {
        StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus =
                StbM_Reset_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_TIMELEAP_PAST_BIT,
                (StbM_NotificationEvents_au32+Index),NotifyMask_u32,STBM_EV_TIMELEAP_PAST_REMOVED);
    }
    if(StbM_Check_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_TIMEOUT_BIT))
    {
        StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus =
                StbM_Reset_Bit(StbM_GlobalTimeTupleArray_ast[Index].timeBaseStatus,STBM_TIMEOUT_BIT,
                        (StbM_NotificationEvents_au32+Index),NotifyMask_u32,STBM_EV_TIMEOUT_REMOVED);
    }

}
#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

#endif /* STBM_INL_H */

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
