/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2016)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME     : Cpu0_Main.c                                                **
**                                                                            **
**  VERSION      : 0.0.1                                                      **
**                                                                            **
**  DATE         : 2016-08-17                                                 **
**                                                                            **
**  VARIANT      : NA                                                         **
**                                                                            **
**  PLATFORM     : Infineon AURIX2G                                           **
**                                                                            **
**  AUTHOR       : DL-AUTOSAR-Engineering                                     **
**                                                                            **
**  VENDOR       : Infineon Technologies                                      **
**                                                                            **
**  DESCRIPTION  : Example Cpu0 Main startup file                             **
**                                                                            **
**  SPECIFICATION(S) : NA                                                     **
**                                                                            **
**  MAY BE CHANGED BY USER : yes                                              **
**                                                                            **
*******************************************************************************/
/*******************************************************************************
**                      Includes                                              **
*******************************************************************************/
#include "Ifx_Ssw_Infra.h"
#include "IFX_Os.h"
#include "EcuM.h"
#include "Os.h"
#include "IfxSrc_reg.h"
#include "Irq.h"
/*******************************************************************************
**                      Imported Compiler Switch Check                        **
*******************************************************************************/

/*******************************************************************************
**                      Private Macro Definitions                             **
*******************************************************************************/

/*******************************************************************************
**                      Private Type Definitions                              **
*******************************************************************************/

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
**                      Global Functon Definitions                            **
*******************************************************************************/
#define CPU0_START_SEC_CODE
#include "MemMap.h"
void core0_main (void)
{
  volatile unsigned short LoopFlag = 1U;
  unsigned short cpuWdtPassword;
  unsigned short safetyWdtPassword;


  ENABLE();
  /*
   * !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
   * Enable the watchdog in the demo if it is required and also service the watchdog periodically
   * */
  cpuWdtPassword = Ifx_Ssw_getCpuWatchdogPassword(&MODULE_SCU.WDTCPU[0]);
  safetyWdtPassword = Ifx_Ssw_getSafetyWatchdogPassword();
  Ifx_Ssw_disableCpuWatchdog(&MODULE_SCU.WDTCPU[0], cpuWdtPassword);
  Ifx_Ssw_disableSafetyWatchdog(safetyWdtPassword);

	OS_MTCR(0xfe04, 0x200 | (OS_MFCR(0xfe04))); /* Ensure PSW.IS */
	Os_StartCoreGate(); 
	inner_main(); 

  while (LoopFlag == 1U)
  {

  }

}
#define CPU0_STOP_SEC_CODE
#include "MemMap.h"