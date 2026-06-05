/******************************************************************************
 * \file    Rte.c
 * \brief   RTE 集成胶水代码 — 连接 ASW(.a) 与 BSW/CDD(.a)。
 *****************************************************************************/
#include "Rte_Type.h"
#include "VehicleControl.h"       /* ASW_Libs */
#include "EnergyManagement.h"     /* ASW_Libs */
#include "Com.h"                  /* BSW_Libs (Vector COM) */
#include "CddPwm.h"               /* CDD_Libs */

void Rte_Task_10ms(void)
{
    VehState_T st = {0};
    VehicleControl_Step(&st);     /* ASW 模型 (.a) */
    (void)CddPwm_SetDutyCycle(0U, 500U);  /* 复杂驱动 (.a) */
}

void Rte_Task_100ms(void)
{
    EnergyManagement_Step();      /* ASW 模型 (.a) */
}
