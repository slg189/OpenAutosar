/******************************************************************************
 * \file    VehicleControl.h
 * \brief   ASW 应用: 整车控制 (模型生成 / 手写封装)
 *****************************************************************************/
#ifndef VEHICLECONTROL_H
#define VEHICLECONTROL_H
#include "Rte_Type.h"
extern void VehicleControl_Init(void);
extern void VehicleControl_Step(const VehState_T *in);
#endif /* VEHICLECONTROL_H */
