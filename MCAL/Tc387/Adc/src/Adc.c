/******************************************************************************
 * \file    Adc.c
 * \brief   AUTOSAR MCAL ADC 驱动实现 (Infineon AURIX TC387) - 静态代码
 *****************************************************************************/
#include "Adc.h"

void Adc_Init(const void *ConfigPtr)
{
    (void)ConfigPtr;
    /* TODO: 配置 VADC 内核, 队列与结果寄存器 */
}

Std_ReturnType Adc_StartGroupConversion(Adc_GroupType Group)
{
    (void)Group;
    /* TODO: 触发软件转换 */
    return E_OK;
}

Std_ReturnType Adc_ReadGroup(Adc_GroupType Group, Adc_ValueGroupType *DataBufferPtr)
{
    if (DataBufferPtr == NULL_PTR)
    {
        return E_NOT_OK;
    }
    (void)Group;
    *DataBufferPtr = 0U;
    return E_OK;
}
