/******************************************************************************
 * \file    Port.h
 * \brief   AUTOSAR MCAL PORT 驱动接口 (AURIX TC387)
 *****************************************************************************/
#ifndef PORT_H
#define PORT_H

#include "Std_Types.h"

typedef uint16 Port_PinType;

extern void Port_Init(const void *ConfigPtr);

#endif /* PORT_H */
