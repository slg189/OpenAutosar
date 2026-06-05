/******************************************************************************
 * \file    CddPwm.h
 * \brief   复杂驱动: PWM 输出 (直接访问 GTM TOM/ATOM)
 *****************************************************************************/
#ifndef CDDPWM_H
#define CDDPWM_H
#include "Std_Types.h"
typedef uint8 CddPwm_ChannelType;
extern void           CddPwm_Init(void);
extern Std_ReturnType CddPwm_SetDutyCycle(CddPwm_ChannelType Ch, uint16 Duty);
#endif /* CDDPWM_H */
