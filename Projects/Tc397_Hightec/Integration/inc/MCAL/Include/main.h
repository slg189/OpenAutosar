/*
 * main.h
 *
 *  Created on: Aug 15, 2017
 */

#ifndef MAIN_H
#define MAIN_H

/* Global variables */
#define CPU0_START_SEC_VAR_INIT_32
#include "MemMap.h"
extern uint32 OS_Counter_1ms;
#define CPU0_STOP_SEC_VAR_INIT_32
#include "MemMap.h"

#define CPU0_START_SEC_CODE
#include "MemMap.h"
extern void TargetEn_PeriodicInterrupt(void);
#define CPU0_STOP_SEC_CODE
#include "MemMap.h"

#endif /* MAIN_H */
