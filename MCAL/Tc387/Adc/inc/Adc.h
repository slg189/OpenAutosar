/******************************************************************************
 * \file    Adc.h
 * \brief   AUTOSAR MCAL ADC 驱动接口 (Infineon AURIX TC387)
 * \details 符合 AUTOSAR Classic Platform - Specification of ADC Driver.
 *****************************************************************************/
#ifndef ADC_H
#define ADC_H

#include "Std_Types.h"

/* ADC 通道与组标识类型 */
typedef uint16 Adc_ChannelType;
typedef uint16 Adc_GroupType;
typedef uint16 Adc_ValueGroupType;

/* 模块初始化, 由 EcuM 在启动阶段调用 */
extern void Adc_Init(const void *ConfigPtr);

/* 启动一次软件触发的转换组 */
extern Std_ReturnType Adc_StartGroupConversion(Adc_GroupType Group);

/* 读取转换结果 */
extern Std_ReturnType Adc_ReadGroup(Adc_GroupType Group, Adc_ValueGroupType *DataBufferPtr);

#endif /* ADC_H */
