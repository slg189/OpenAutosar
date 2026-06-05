/******************************************************************************
 * \file    Can.h
 * \brief   AUTOSAR MCAL CAN 驱动接口 (AURIX TC387 - MultiCAN+)
 *****************************************************************************/
#ifndef CAN_H
#define CAN_H

#include "Std_Types.h"

typedef uint8  Can_HwHandleType;
typedef uint32 Can_IdType;

extern void           Can_Init(const void *Config);
extern Std_ReturnType Can_Write(Can_HwHandleType Hth, const uint8 *Pdu);

#endif /* CAN_H */
