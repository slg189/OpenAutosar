

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "StbM.h"

#if(STBM_DEV_ERROR_DETECT == STD_ON) /*Det included when Det is enabled in StbM*/
#include "Det.h"
/*#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#elif (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != STBM_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif*/

#endif  /*#if(STBM_DEV_ERROR_DETECT == STD_ON)*/

#if (STBM_NVM_ENABLE == STD_ON)
#include "NvM.h"
/*#if (!defined(NVM_AR_RELEASE_MAJOR_VERSION) || (NVM_AR_RELEASE_MAJOR_VERSION != STBM_AR_RELEASE_MAJOR_VERSION))
#error "NVM AUTOSAR major version undefined or mismatched"
#elif (!defined(NVM_AR_RELEASE_MINOR_VERSION) || (NVM_AR_RELEASE_MINOR_VERSION != STBM_AR_RELEASE_MINOR_VERSION))
#error "NVM AUTOSAR minor version undefined or mismatched"
#endif*/

#endif /*#if (STBM_NVM_ENABLE == STD_ON)*/

#include "StbM_Priv.h"
#include "StbM_Inl.h"

/*
 **********************************************************************************************************************
 * Variables
 **********************************************************************************************************************
 */

#define STBM_START_SEC_VAR_INIT_UNSPECIFIED
#include "StbM_MemMap.h"
/* [SWS_StbM_00100]A static status variable denoting if the StbM is initialized shall be initialized with value 0
   before any APIs of the StbM are called.make sure StbM_Init is called before StbM_MainFunction. */
boolean StbM_InitState_b = FALSE;
#define STBM_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_INIT_8
#include "StbM_MemMap.h"
/* Array to maintain the understanding of index for all global arrays for each timebase */
uint8 StbM_Index_au8[STBM_MAX_NUM_TIMEBASES] = {STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX
,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX

};
#define STBM_STOP_SEC_VAR_INIT_8
#include "StbM_MemMap.h"

#define STBM_START_SEC_VAR_INIT_8
#include "StbM_MemMap.h"
/* Array to maintain the understanding of index for all global arrays for each timebase */
uint8 StbM_Nvm_Index_au8[STBM_MAX_NUM_TIMEBASES] = {STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX
,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,
STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX,STBM_INVALID_INDEX
};
#define STBM_STOP_SEC_VAR_INIT_8
#include "StbM_MemMap.h"

#if (STBM_NVM_ENABLE == STD_ON)
#define STBM_START_SEC_VAR_SAVED_ZONE_UNSPECIFIED
#include "StbM_MemMap.h"
StbM_TimeStampType StbM_TimesourceArray_Nvm_ast [STBM_CFG_NUM_OF_TIMEBASES_NVM];
#define STBM_STOP_SEC_VAR_SAVED_ZONE_UNSPECIFIED
#include "StbM_MemMap.h"
#endif

#define STBM_START_SEC_VAR_CLEARED_8
#include "StbM_MemMap.h"
uint8 StbM_Index_HWCounterRef_ast [STBM_CFG_NUM_OF_TIMEBASES];
#define STBM_STOP_SEC_VAR_CLEARED_8
#include "StbM_MemMap.h"

#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/***************************************************************************************************
 *Service name      : StbM_Init
 *Syntax            : void StbM_Init( const StbM_ConfigType* ConfigPtr )
 *Service ID[hex]   : 0x00
 *Sync/Async        : Synchronous
 *Reentrancy        : Non Reentrant
 *Parameters (in)   : ConfigPtr --Pointer to the selected configuration set.
 *Parameters (inout): None
 *Parameters (out)  : None
 *Return value      : None
 *Description       : Initializes the Synchronized Time-base Manager
 ***************************************************************************************************/

void StbM_Init(const StbM_ConfigType * ConfigPtr)
{
    /* [SWS_StbM_00250] */
    /* The Configuration pointer ConfigPtr shall always have a NULL_PTR value */
    if(ConfigPtr == NULL_PTR)
    {
        /* Local Variable declaration */

        uint8 lCount_u8;
		StbMLocalTimeHardware_ten lStbMLocalTimeHardware_ten;
#if (STBM_OS_ENABLE == STD_ON)
        uint8 lIndexCurrentOS;
#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
#if (STBM_ETH_ENABLE == STD_ON)
        uint8 lIndexCurrentEth;
#endif /* #if (STBM_ETH_ENABLE ==  STD_ON )*/
#if (STBM_GPT_ENABLE == STD_ON)
        uint8 lIndexCurrentGPT;
        Gpt_ChannelType lGPT_ChannelId;
#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
        uint8 lIndexCurrentExternal_u8;
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/
#if (STBM_TIME_RECORDING_SUPPORT == STD_ON)
        uint16 lBlockCount_u8;
        uint16 lBlockCountItr_u8;
        StbM_SyncRecordTableBlockType * lSyncBlockRecordArrayPtr;
        StbM_OffsetRecordTableBlockType * lOffsetBlockRecordArray;
#endif /* #if (STBM_TIME_RECORDING_SUPPORT == STD_ON) */

#if (STBM_NVM_ENABLE == STD_ON)
        Std_ReturnType lRet_NRB_u8; /* Return Value from NvM_ReadBlock() */
        Std_ReturnType lRet_NGES_u8; /* Return Value from NvM_GetErrorStatus() */
        uint8 lCount_Nvm_u8;

        /* Local Variable declaration */
        NvM_RequestResultType StbM_Nvm_ErrorStatus = NVM_REQ_OK;

        /* Local Variable Initialization */
        lRet_NGES_u8 =  E_NOT_OK;
        lRet_NRB_u8 = E_NOT_OK;
        lCount_Nvm_u8 = STBM_ZERO ;

        /* Read the value from NvM */
        /*MR12 DIR 1.1 VIOLATION: A 'pointer-to-object' conversion is necessary in this case*/
        lRet_NRB_u8 = NvM_ReadBlock(STBM_NVM_BLOCK_REFERENCE,StbM_TimesourceArray_Nvm_ast);

        /* Check the error status of the Block */
        lRet_NGES_u8 = NvM_GetErrorStatus(STBM_NVM_BLOCK_REFERENCE, &StbM_Nvm_ErrorStatus);
#endif /* (STBM_NVM_ENABLE == STD_ON) */

#if (STBM_OS_ENABLE == STD_ON)
        lIndexCurrentOS  = STBM_ZERO;
#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
#if (STBM_ETH_ENABLE == STD_ON)
        lIndexCurrentEth = STBM_ZERO;
#endif /* #if (STBM_ETH_ENABLE ==  STD_ON )*/
#if (STBM_GPT_ENABLE == STD_ON)
        lIndexCurrentGPT = STBM_ZERO;
        lGPT_ChannelId   = STBM_GPT_INVALID_CHANNEL_ID;
#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
        lIndexCurrentExternal_u8 = STBM_ZERO;
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/
        lStbMLocalTimeHardware_ten = STBMLOCALTIMEHARDWARE_NOT_CONFIGURED;

        for(lCount_u8=STBM_ZERO;lCount_u8<STBM_CFG_NUM_OF_TIMEBASES;lCount_u8++)
        {
            StbM_Index_au8[StbM_Cfg_SynchronizedTimeBaseArray_ast[lCount_u8].SynchronizedTimeBaseIdentifier_u16]
                           = lCount_u8;

            /* [SWS_StbM_00344]For each Time Base the StbM shall initialize the corresponding update counter
              StbM_TimeBaseUpdateCounter_au8 with 0.*/
            StbM_TimeBaseUpdateCounter_au8[lCount_u8] = STBM_ZERO ;

            /* [SWS_StbM_00345]
            For each Time Base the StbM shall initialize the corresponding event status NotificationEvents with 0. */
            StbM_NotificationEvents_au32[lCount_u8]    = STBM_ZERO ;

            /* [SWS_StbM_00171]  For each Time Base configured to be stored non-volatile
               (StbMStoreTimebaseNonVolatile == STBM_NVM_STORAGE_AT_SHUTDOWN), the value shall be loaded from NvM. */
#if (STBM_NVM_ENABLE == STD_ON)

            if (StbM_Cfg_SynchronizedTimeBaseArray_ast[lCount_u8].StoreTimebaseNonVolatile_e == STBM_NVM_STORAGE_AT_SHUTDOWN)
            {
                StbM_Nvm_Index_au8[StbM_Cfg_SynchronizedTimeBaseArray_ast[lCount_u8].SynchronizedTimeBaseIdentifier_u16]
                                                   = lCount_Nvm_u8;

                if ((StbM_Nvm_ErrorStatus == NVM_REQ_OK )&&(lRet_NRB_u8 == E_OK)&&(lRet_NGES_u8 == E_OK))
                {
                    /* IF the restore is successfull time values will be loaded from NvM during read_block */
                    StbM_GlobalTimeTupleArray_ast[lCount_u8].nanoseconds  =
                                                             StbM_TimesourceArray_Nvm_ast[lCount_Nvm_u8].nanoseconds;
                    StbM_GlobalTimeTupleArray_ast[lCount_u8].seconds      =
                                                             StbM_TimesourceArray_Nvm_ast[lCount_Nvm_u8].seconds;
                    StbM_GlobalTimeTupleArray_ast[lCount_u8].secondsHi    =
                                                             StbM_TimesourceArray_Nvm_ast[lCount_Nvm_u8].secondsHi;
                }
                else
                {
                    /* IF the restore is not successfull initialise with zero */
                    StbM_GlobalTimeTupleArray_ast[lCount_u8].nanoseconds  = STBM_ZERO;
                    StbM_GlobalTimeTupleArray_ast[lCount_u8].seconds      = STBM_ZERO;
                    StbM_GlobalTimeTupleArray_ast[lCount_u8].secondsHi    = STBM_ZERO;
                }
                lCount_Nvm_u8++;
            }
            else
#endif /* #if (STBM_NVM_ENABLE == STD_ON)*/
            {
                /* [SWS_StbM_00170] On invocation of StbM_Init() each configured Time Base shall be initialized
                   with zero and its synchronization status timeBaseStatus shall be set set for all Time Domains
                   and all bits to 0x00 */
                StbM_GlobalTimeTupleArray_ast[lCount_u8].nanoseconds    = STBM_ZERO;
                StbM_GlobalTimeTupleArray_ast[lCount_u8].seconds        = STBM_ZERO;
                StbM_GlobalTimeTupleArray_ast[lCount_u8].secondsHi      = STBM_ZERO;
            }
            /* [SWS_StbM_00170] On invocation of StbM_Init() each configured Time Base shall be initialized
               with zero and its synchronization status timeBaseStatus shall be set set for all Time Domains
               and all bits to 0x00
               Timebase status needs to be initialised with zero , since only time valuse needs to be read */
            StbM_GlobalTimeTupleArray_ast[lCount_u8].timeBaseStatus         = STBM_ZERO;

            /* [SWS_StbM_00399]For Pure Local Time Bases all bits of the Time Base status timeBaseStatus shall be set to 0,
               except for bit GLOBAL_TIME_BASE.GLOBAL_TIME_BASE shall be set to 1, by a valid invocation of
               StbM_SetGlobalTime() or StbM_UpdateGlobalTime() and only set to 0 by StbM_Init()  */

            /* Initialise the userdata for the configured timebases */
            StbM_UserDataArray_ast[lCount_u8].userDataLength = STBM_ZERO;
            StbM_UserDataArray_ast[lCount_u8].userByte0      = STBM_ZERO;
            StbM_UserDataArray_ast[lCount_u8].userByte1      = STBM_ZERO;
            StbM_UserDataArray_ast[lCount_u8].userByte2      = STBM_ZERO;

/* [SWS_StbM_00307] The StbM shall support the Global Time precision measurement, if StbMTimeRecordingSupport is set to TRUE. */
#if (STBM_TIME_RECORDING_SUPPORT == STD_ON)
            // Initialize the array that holds the current block count for a particular time base with STBM_ZERO
            StbM_RecordTableBlockCountCurrent_ast[lCount_u8] = STBM_ZERO;

            lBlockCount_u8 = StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].RecordTableBlockCount_u16;


            /* Check if time recording feature support is on for this timebase ID */
            if(lBlockCount_u8 != STBM_ZERO)
            {
                /* Check if the TimeBase is Synchronized TimeBase */
                if(StbM_Rb_IsSyncTimeBase(StbM_Cfg_SynchronizedTimeBaseArray_ast[lCount_u8].SynchronizedTimeBaseIdentifier_u16))
                {
                    lSyncBlockRecordArrayPtr = StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].
                                                                               SynchronizedTimeBaseBlockRecordArray_ptr;

                    /* [SWS_StbM_00306] If StbMTimeRecordingSupport (ECUC_StbM_00038 : ) is set to TRUE,
                        the StbM shall initialize all Block Elements of the measurement recording table with zero. */
                    /* [SWS_StbM_00382] If StbMTimeRecordingSupport is set to TRUE, StbMSyncTimeRecordTableBlockCount shall be
                        used to increase the number of blocks of the Synchronized Time Base Record Table. */
                    for(lBlockCountItr_u8=STBM_ZERO; lBlockCountItr_u8 < lBlockCount_u8; lBlockCountItr_u8++)
                    {
                        lSyncBlockRecordArrayPtr[lBlockCountItr_u8].GlbSeconds       = STBM_ZERO;
                        lSyncBlockRecordArrayPtr[lBlockCountItr_u8].GlbNanoSeconds   = STBM_ZERO;
                        lSyncBlockRecordArrayPtr[lBlockCountItr_u8].TimeBaseStatus   = STBM_ZERO;
                        lSyncBlockRecordArrayPtr[lBlockCountItr_u8].VirtualLocalTimeLow = STBM_ZERO;
                        lSyncBlockRecordArrayPtr[lBlockCountItr_u8].RateDeviation    = STBM_SIGNED_ZERO;
                        lSyncBlockRecordArrayPtr[lBlockCountItr_u8].LocSeconds       = STBM_ZERO;
                        lSyncBlockRecordArrayPtr[lBlockCountItr_u8].LocNanoSeconds   = STBM_ZERO;
                        lSyncBlockRecordArrayPtr[lBlockCountItr_u8].PathDelay        = STBM_ZERO;
                    }
                }
                else
                {
                    lOffsetBlockRecordArray = StbM_Cfg_TimeBaseBlockRecordArray_ast[lCount_u8].
                                                                                     OffsetTimeBaseBlockRecordArray_ptr;

                    /* [SWS_StbM_00306] If StbMTimeRecordingSupport (ECUC_StbM_00038 : ) is set to TRUE,
                        the StbM shall initialize all Block Elements of the measurement recording table with zero. */
                    /* [SWS_StbM_00383] If StbMTimeRecordingSupport is set to TRUE, StbMOffsetTimeRecordTableBlockCount
                        shall be used to increase the number of blocks of the Offset Time Base Record Table. */
                    for(lBlockCountItr_u8=STBM_ZERO; lBlockCountItr_u8 < lBlockCount_u8; lBlockCountItr_u8++)
                    {
                        lOffsetBlockRecordArray[lBlockCountItr_u8].GlbSeconds       = STBM_ZERO;
                        lOffsetBlockRecordArray[lBlockCountItr_u8].GlbNanoSeconds   = STBM_ZERO;
                        lOffsetBlockRecordArray[lBlockCountItr_u8].TimeBaseStatus   = STBM_ZERO;
                    }
                }
            }

#endif /* #if (STBM_TIME_RECORDING_SUPPORT == STD_ON) */

            /* Updating locally maintained array for storing indexes in respective arrays of HW Counter
             * references for local time maintainence */

            lStbMLocalTimeHardware_ten = StbM_Cfg_LocalTimeClockArray_ast[lCount_u8].StbMLocalTimeHardware_e;

            if(lStbMLocalTimeHardware_ten == OS)
            {
#if (STBM_OS_ENABLE == STD_ON)
                StbM_Index_HWCounterRef_ast[lCount_u8] = lIndexCurrentOS;
                lIndexCurrentOS++;
#endif /* #if (STBM_OS_ENABLE ==  STD_ON )*/
            }
            else if(lStbMLocalTimeHardware_ten == ETHTSYN)
            {
#if (STBM_ETH_ENABLE == STD_ON)
                StbM_Index_HWCounterRef_ast[lCount_u8] = lIndexCurrentEth;
                lIndexCurrentEth++;
#endif /* #if (STBM_ETH_ENABLE ==  STD_ON )*/
            }
            else if(lStbMLocalTimeHardware_ten == GPT)
            {
#if (STBM_GPT_ENABLE == STD_ON)
                StbM_Index_HWCounterRef_ast[lCount_u8] = lIndexCurrentGPT;
                lIndexCurrentGPT++;

                /* Start GPT counter for current timebase ID */
                /* Get the GPT channel reference */
                lGPT_ChannelId = StbM_Cfg_LocalTimeClockArray_ast[lCount_u8].StbMLocalTimeHardware_GptChannel_Id;

                (void)Gpt_StartTimer(lGPT_ChannelId, STBM_MAX_UINT32);
#endif /* #if (STBM_GPT_ENABLE ==  STD_ON )*/
            }
            else if(lStbMLocalTimeHardware_ten == EXTERNAL_COUNTER)
            {
#if (STBM_EXT_COUNTER_ENABLE == STD_ON)
                StbM_Index_HWCounterRef_ast[lCount_u8] = lIndexCurrentExternal_u8;
                lIndexCurrentExternal_u8++;
#endif /* #if (STBM_EXT_COUNTER_ENABLE ==  STD_ON )*/
            }
            else
            {
                StbM_Index_HWCounterRef_ast[lCount_u8] = STBM_INVALID_INDEX;
            }

			/* Call the API StbM_VirtualLocalTime_Init to initilaise the Virtual local time */
            StbM_VirtualLocalTime_Init(lCount_u8);
        }

#if(STBM_CFG_TRIGGERED_CUSTOMER > STBM_ZERO)
        for(lCount_u8=STBM_ZERO;lCount_u8 < STBM_CFG_TRIGGERED_CUSTOMER;lCount_u8++)
        {
            /* Copy the triggering period of the customer into a temporary array */
            StbM_TrigCusPeriod_u32[lCount_u8] = StbM_Cfg_TriggeredCustomerArray_ast[lCount_u8]
                                                                                    .TriggeredCustomerPeriod_u32;
        }
#endif /*#if(STBM_CFG_TRIGGERED_CUSTOMER > STBM_ZERO)*/

        /* [SWS_StbM_00121]StbM_Init shall set the static status variable to a value not equal to 0. */
        StbM_InitState_b = TRUE;
    }
}

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"


#define STBM_START_SEC_CODE
#include "StbM_MemMap.h"

/********************************************************************************************************************
Service name      : StbM_GetVersionInfo
Syntax            : void StbM_GetVersionInfo( Std_VersionInfoType* versioninfo )
Service ID[hex]   : 0x05
Sync/Async        : Synchronous
Reentrancy        : Reentrant
Parameters (in)   : None
Parameters (inout): None
Parameters (out)  : versioninfo : Pointer to the memory location holding the version information of the module.
Return value      : None
Description       : Returns the version information of this module.
 ********************************************************************************************************************/
/* [SWS_StbM_00066] */
#if(STBM_VERSION_INFO_API==STD_ON)
void StbM_GetVersionInfo(Std_VersionInfoType * versioninfo)
{
    /* Check whether the versioninfo is NULL_PTR */
    if(versioninfo!=NULL_PTR)
    {
        /* Update the version info */
        versioninfo->vendorID         = STBM_VENDOR_ID;
        versioninfo->moduleID         = STBM_MODULE_ID;
        versioninfo->sw_major_version = STBM_SW_MAJOR_VERSION;
        versioninfo->sw_minor_version = STBM_SW_MINOR_VERSION;
        versioninfo->sw_patch_version = STBM_SW_PATCH_VERSION;
    }
    else
    {
        /* [SWS_StbM_00094]   If development error detection for the StbM module is enabled the function StbM_GetVersionInfo
           shall raise the development error
           STBM_E_PARAM_POINTER and return if versioninfo is a NULL pointer (NULL_PTR). */
        /*if Config is NULL pointer then report error and exit the function*/
        STBM_DET_REPORT_ERROR(STBM_INSTANCE_ID,STBM_APIID_GETVERSIONINFO,STBM_E_PARAM_POINTER)
    }

}
#endif /*#if(STBM_VERSION_INFO_API==STD_ON)*/

#define STBM_STOP_SEC_CODE
#include "StbM_MemMap.h"

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
