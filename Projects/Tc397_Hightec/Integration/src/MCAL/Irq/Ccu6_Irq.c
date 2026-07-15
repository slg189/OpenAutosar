/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2017)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME     : Ccu6_Irq.c                                                 **
**                                                                            **
**  VERSION      : 1.0.0                                                      **
**                                                                            **
**  DATE         : 2017-10-12                                                 **
**                                                                            **
**  VARIANT      : Variant PB                                                 **
**                                                                            **
**  PLATFORM     : Infineon AURIX2G                                           **
**                                                                            **
**  AUTHOR       : DL-AUTOSAR-Engineering                                     **
**                                                                            **
**  VENDOR       : Infineon Technologies                                      **
**                                                                            **
**  DESCRIPTION  : This file contains the interrupt frames for the CCU6.      **
**                                                                            **
**  SPECIFICATION(S) : Specification of Icu Driver, AUTOSAR Release 4.2.2     **
**                                                                            **
**  MAY BE CHANGED BY USER : yes                                              **
**                                                                            **
*******************************************************************************/
/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
/* MCAL header file containing global macros, type definitions and functions
  needed by all MCAL drivers. */
#include "MCAL_Compiler.h"
#include "McalLib.h"
/*#include "IfxCpu_Intrinsics.h"*/
#include "IfxCcu6_reg.h"
/* Include MCU TIMERIP header file */
/* #include "Mcu_17_TimerIp.h"
   #include "IFX_Os.h"*/

/* MCAL driver utility functions for setting interrupt priority initialization
  and interrupt category */
#include "IFX_Os.h"
#include "Irq.h"
#include "Mcu_17_TimerIp.h"
/*If Test module is pwm*/
#if(((defined BASE_TEST_MODULE_ID) && (BASE_TEST_MODULE_ID == 121)) &&(APP_SW == TEST_APP))
#include "Test_Time.h"
void Timer_DelayMs(uint32 ReqTimeMs);
void Timer_DelayTicks(uint32 ReqTimeTicks);
extern volatile uint8 u8g_Resilience_Delay_En;
#endif

/*******************************************************************************
**                      Imported Compiler Switch Checks                       **
*******************************************************************************/

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/
#define ISR(x) void Os_Entry_##x(void)
#define CCU6_KERNEL_0  0U
#define CCU6_KERNEL_1  1U
#define CCU6_KERNEL_2  2U
#define CCU6_KERNEL_3  2U

#define CCU6_CHANNEL_0  0U
#define CCU6_CHANNEL_1  1U
#define CCU6_CHANNEL_2  2U
#define CCU6_CHANNEL_3  3U
/*******************************************************************************
**                      Private Function Declarations                         **
*******************************************************************************/

/*******************************************************************************
**                      Global Constant Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Global Variable Definitions                           **
*******************************************************************************/

/*******************************************************************************
**                      Private Constant Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Private Variable Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Private Function Definitions                          **
*******************************************************************************/

/*******************************************************************************
**                      Global Function Definitions                           **
*******************************************************************************/

#define CPU0_START_SEC_CODE
#include "MemMap.h"
/*******************************************************************************
** Syntax:           void CCU60_ISR_SRN00(void)                               **
**                                                                            **
** Service ID:       NA                                                       **
**                                                                            **
** Sync/Async:       Synchronous                                              **
**                                                                            **
** Reentrancy:       Reentrant                                                **
**                                                                            **
** Parameters (in):  none                                                     **
**                                                                            **
** Parameters (out): none                                                     **
**                                                                            **
** Return value:     none                                                     **
**                                                                            **
** Description :     This is Interrput Service request nodes 0 for the  CCU60.**
*******************************************************************************/
#if IRQ_CCU60_SR0_TOS != IRQ_TOS_DMA
#if((IRQ_CCU60_SR0_PRIO > 0) || (IRQ_CCU60_SR0_CAT == IRQ_CAT2))
#if((IRQ_CCU60_SR0_PRIO > 0) && (IRQ_CCU60_SR0_CAT == IRQ_CAT1))
IFX_INTERRUPT(CCU60SR0_ISR, 0, IRQ_CCU60_SR0_PRIO)
#elif IRQ_CCU60_SR0_CAT == IRQ_CAT2
ISR(CCU60SR0_ISR)
#endif
{
  /* Enable Global Interrupts */
  /* Mcal_EnableAllInterrupts(); */
  ENABLE();
  /*If Test module is pwm*/
  /*This piece of code introduces delay in isr for checking resilience
  against interrupt latency*/
  #if(((defined BASE_TEST_MODULE_ID) && (BASE_TEST_MODULE_ID == 121)) && (APP_SW == TEST_APP))
  if(u8g_Resilience_Delay_En == 1)
  {
    Timer_DelayMs(1);
	u8g_Resilience_Delay_En = 0;
  }
  #endif
  Mcu_17_Ccu6_ChannelIsr(CCU6_KERNEL_0, CCU6_CHANNEL_0);

}

#endif
#endif

#if IRQ_CCU60_SR1_TOS != IRQ_TOS_DMA
#if((IRQ_CCU60_SR1_PRIO > 0) || (IRQ_CCU60_SR1_CAT == IRQ_CAT2))
#if((IRQ_CCU60_SR1_PRIO > 0) && (IRQ_CCU60_SR1_CAT == IRQ_CAT1))
IFX_INTERRUPT(CCU60SR1_ISR, 0, IRQ_CCU60_SR1_PRIO)
#elif IRQ_CCU60_SR1_CAT == IRQ_CAT2
ISR(CCU60SR1_ISR)
#endif
{
  /* Enable Global Interrupts */
  /* Mcal_EnableAllInterrupts(); */
  ENABLE ();
  Mcu_17_Ccu6_ChannelIsr(CCU6_KERNEL_0, CCU6_CHANNEL_1);

}

#endif
#endif

#if IRQ_CCU60_SR2_TOS != IRQ_TOS_DMA
#if((IRQ_CCU60_SR2_PRIO > 0) || (IRQ_CCU60_SR2_CAT == IRQ_CAT2))
#if((IRQ_CCU60_SR2_PRIO > 0) && (IRQ_CCU60_SR2_CAT == IRQ_CAT1))
IFX_INTERRUPT(CCU60SR2_ISR, 0, IRQ_CCU60_SR2_PRIO)
#elif IRQ_CCU60_SR2_CAT == IRQ_CAT2
ISR(CCU60SR2_ISR)
#endif
{
  /* Enable Global Interrupts */
  /* Mcal_EnableAllInterrupts(); */
  ENABLE ();
  Mcu_17_Ccu6_ChannelIsr(CCU6_KERNEL_0, CCU6_CHANNEL_2);

}

#endif
#endif

#if IRQ_CCU60_SR3_TOS != IRQ_TOS_DMA
#if((IRQ_CCU60_SR3_PRIO > 0) || (IRQ_CCU60_SR3_CAT == IRQ_CAT2))
#if((IRQ_CCU60_SR3_PRIO > 0) && (IRQ_CCU60_SR3_CAT == IRQ_CAT1))
IFX_INTERRUPT(CCU60SR3_ISR, 0, IRQ_CCU60_SR3_PRIO)
#elif IRQ_CCU60_SR3_CAT == IRQ_CAT2
ISR(CCU60SR3_ISR)
#endif
{
  /* Enable Global Interrupts */
  /* Mcal_EnableAllInterrupts(); */
  ENABLE ();
  Mcu_17_Ccu6_ChannelIsr(CCU6_KERNEL_0, CCU6_CHANNEL_3);

}

#endif
#endif

/*******************************************************************************
** Syntax:           void CCU61_ISR_SRN00(void)                               **
**                                                                            **
** Service ID:       NA                                                       **
**                                                                            **
** Sync/Async:       Synchronous                                              **
**                                                                            **
** Reentrancy:       Reentrant                                                **
**                                                                            **
** Parameters (in):  none                                                     **
**                                                                            **
** Parameters (out): none                                                     **
**                                                                            **
** Return value:     none                                                     **
**                                                                            **
** Description :     This is Interrput Service request nodes 0 for the  CCU61.**
*******************************************************************************/
#if IRQ_CCU61_SR0_TOS != IRQ_TOS_DMA
#if((IRQ_CCU61_SR0_PRIO > 0) || (IRQ_CCU61_SR0_CAT == IRQ_CAT2))
#if((IRQ_CCU61_SR0_PRIO > 0) && (IRQ_CCU61_SR0_CAT == IRQ_CAT1))
IFX_INTERRUPT(CCU61SR0_ISR, 0, IRQ_CCU61_SR0_PRIO)
#elif IRQ_CCU61_SR0_CAT == IRQ_CAT2
ISR(CCU61SR0_ISR)
#endif
{
  /* Enable Global Interrupts */
  ENABLE ();
  Mcu_17_Ccu6_ChannelIsr(CCU6_KERNEL_1, CCU6_CHANNEL_0);
}
#endif
#endif

#if IRQ_CCU61_SR1_TOS != IRQ_TOS_DMA
#if((IRQ_CCU61_SR1_PRIO > 0) || (IRQ_CCU61_SR1_CAT == IRQ_CAT2))
#if((IRQ_CCU61_SR1_PRIO > 0) && (IRQ_CCU61_SR1_CAT == IRQ_CAT1))
IFX_INTERRUPT(CCU61SR1_ISR, 0, IRQ_CCU61_SR1_PRIO)
#elif IRQ_CCU61_SR1_CAT == IRQ_CAT2
ISR(CCU61SR1_ISR)
#endif
{
  /* Enable Global Interrupts */
  ENABLE ();
  Mcu_17_Ccu6_ChannelIsr(CCU6_KERNEL_1, CCU6_CHANNEL_1);
}
#endif
#endif

#if IRQ_CCU61_SR2_TOS != IRQ_TOS_DMA
#if((IRQ_CCU61_SR2_PRIO > 0) || (IRQ_CCU61_SR2_CAT == IRQ_CAT2))
#if((IRQ_CCU61_SR2_PRIO > 0) && (IRQ_CCU61_SR2_CAT == IRQ_CAT1))
IFX_INTERRUPT(CCU61SR2_ISR, 0, IRQ_CCU61_SR2_PRIO)
#elif IRQ_CCU61_SR2_CAT == IRQ_CAT2
ISR(CCU61SR2_ISR)
#endif
{
  /* Enable Global Interrupts */
  ENABLE ();
  Mcu_17_Ccu6_ChannelIsr(CCU6_KERNEL_1, CCU6_CHANNEL_2);
}
#endif
#endif

#if IRQ_CCU61_SR3_TOS != IRQ_TOS_DMA
#if((IRQ_CCU61_SR3_PRIO > 0) || (IRQ_CCU61_SR3_CAT == IRQ_CAT2))
#if((IRQ_CCU61_SR3_PRIO > 0) && (IRQ_CCU61_SR3_CAT == IRQ_CAT1))
IFX_INTERRUPT(CCU61SR3_ISR, 0, IRQ_CCU61_SR3_PRIO)
#elif IRQ_CCU61_SR3_CAT == IRQ_CAT2
ISR(CCU61SR3_ISR)
#endif
{
  /* Enable Global Interrupts */
  ENABLE ();
  Mcu_17_Ccu6_ChannelIsr(CCU6_KERNEL_1, CCU6_CHANNEL_3);
}
#endif
#endif

#define CPU0_STOP_SEC_CODE
#include "MemMap.h"