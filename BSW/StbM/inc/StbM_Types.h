

#ifndef STBM_TYPES_H
#define STBM_TYPES_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */

#include "Std_Types.h"
#include "StbM_Cfg_Rte.h"

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

/*This type defines the type that are used for specifying that the timebase shall be stored in the NvRam*/
typedef enum
{
    STBM_NVM_NO_STORAGE                                                  = 0x00,        /*Values are not stored in NvM*/
    STBM_NVM_STORAGE_AT_SHUTDOWN                                         = 0x01,        /*Values are stored and resd from
                                                                                          the NvM*/
    STBM_NVM_NOT_CONFIGURED                                              = 0x02         /*NvM is not configured */
}StbM_StoreTimebaseNonVolatileType_ten;


/*This allows for specifying that StbM is acting as MASTER or SLAVE or GATEWAY*/
typedef enum
{
    STBM_MASTER                                                       = 0x00,        /*StbM is in MASTER state*/
    STBM_SLAVE                                                        = 0x01,        /*StbM is in SLAVE state*/
    STBM_GATEWAY                                                      = 0x02         /*StbM is in GATEWAY state*/
}StbM_StateType_ten;

/*This allows for specifying the HW reference */
typedef enum
{
    OS                                                       = 0x00,        /*StbMLocalTimeHardwareRef is OS*/
    ETHTSYN                                                  = 0x01,        /*StbMLocalTimeHardwareRef is ETHTSYN*/
    GPT                                                      = 0x02,        /*StbMLocalTimeHardwareRef is GPT*/
    EXTERNAL_COUNTER                                         = 0x03,        /*External Counter Configuration*/
    STBMLOCALTIMEHARDWARE_NOT_CONFIGURED                     = 0X04         /*No HW counter is configured */
}StbMLocalTimeHardware_ten;

/* Measurement data type */
typedef struct
{
    /* Propagation delay in nanoseconds */
    uint32 pathDelay;
}StbM_MeasurementType;

/*This type defines the type for Time Correction */
/* Default value for offset timebases is 0x00 and for synchronized timebases if
   offset correction is configured is 0x01 else is 0x00 */
typedef enum
{
    STBM_RATE_CORRECTION                 = 0x00,   /* Time Values are corrected using Rate Correction */
    STBM_JUMP_CORRECTION                 = 0x01,   /* Time Values are corrected using Jump Correction */
    STBM_RATE_ADAPTION                   = 0x02,   /* Time Values are corrected using Rate Adaption */
    STBM_TIME_CORRECTION_NOT_CONFIGURED  = 0x03    /* Time Correction not configured */
}StbM_TimeCorrectionType_ten;

/*This type defines whether time value is Global or Local*/
typedef enum
{
    STBM_GLOBAL_TIME_VALUE        = 0x00,                 /* Time Value is GLOBAL time value without rate correction */
    STBM_GLOBAL_TIME_VALUE_RATE_CORRECTION = 0x02         /* Time Value is GLOBAL time value for rate correction */
}StbM_TimeValueType_ten;


/* Variables of this type store time stamps of the Virtual Local Time. The unit is nanoseconds */
typedef struct
{
    uint32 nanosecondsLo;                                 /*Least significant 32 bits of the 64 bit Virtual Local Time*/
    uint32 nanosecondsHi;                                 /*Most significant 32 bits of the 64 bit Virtual Local Time*/
}StbM_VirtualLocalTimeType;


#if (STBM_ECUC_RB_RTE_IN_USE == STD_OFF)

typedef uint8 StbM_MasterConfigType;

typedef uint32 StbM_TimeBaseNotificationType;

typedef sint32 StbM_TimeDiffType;

typedef uint16 StbM_SynchronizedTimeBaseType;

typedef uint8 StbM_TimeBaseStatusType;

typedef uint16 StbM_CustomerIdType;

typedef struct {
   StbM_TimeBaseStatusType timeBaseStatus;
   uint32                  nanoseconds;
   uint64                  seconds;
} StbM_TimeStampExtendedType;

typedef struct {
   StbM_TimeBaseStatusType timeBaseStatus;
   uint32                  nanoseconds;
   uint32                  seconds;
   uint16                  secondsHi;
} StbM_TimeStampType;

typedef struct {
   uint8 userDataLength;
   uint8 userByte0;
   uint8 userByte1;
   uint8 userByte2;
} StbM_UserDataType;

typedef struct {
   uint32                  GlbSeconds;
   uint32                  GlbNanoSeconds;
   StbM_TimeBaseStatusType TimeBaseStatus;
} StbM_OffsetRecordTableBlockType;

typedef struct {
   uint8 OffsetTimeDomain;
} StbM_OffsetRecordTableHeadType;

typedef sint16 StbM_RateDeviationType;

typedef struct {
   uint32                  GlbSeconds;
   uint32                  GlbNanoSeconds;
   StbM_TimeBaseStatusType TimeBaseStatus;
   uint32                  VirtualLocalTimeLow;
   StbM_RateDeviationType  RateDeviation;
   uint32                  LocSeconds;
   uint32                  LocNanoSeconds;
   uint32                  PathDelay;
} StbM_SyncRecordTableBlockType;

typedef struct {
   uint8  SynchronizedTimeDomain;
   uint32 HWfrequency;
   uint32 HWprescaler;
} StbM_SyncRecordTableHeadType;

#endif  /*(STBM_ECUC_RB_RTE_IN_USE == STD_OFF) */

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */


#endif /*STBM_TYPES_H */

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
