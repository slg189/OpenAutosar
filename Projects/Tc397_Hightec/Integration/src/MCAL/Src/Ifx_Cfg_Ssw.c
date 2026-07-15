/**
  * \file Ifx_Cfg_Ssw.c
 * \brief Configuration file for the Startup Software
 * \ingroup
 *
 * \copyright Copyright (c) 2017 Infineon Technologies AG. All rights reserved.
 *
 * $Date: 2017-07-17 08:52:44
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineons microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Ifx_Cfg_Ssw.h"
#include "Ifx_Ssw_Infra.h"

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/
#if defined(__TASKING__)
#pragma optimize RL
#elif defined(__HIGHTEC__)
#pragma GCC optimize ("O1")
#endif

#if (IFX_CFG_SSW_ENABLE_PMS_INIT == 1U)
void Ifx_Ssw_Pms_Init(void)
{
    if (IfxPmsEvr_runInitSequence(&IfxPmsEvr_cfgSequenceDefault) == 0)
    {
        __debug();
    }

    Ifx_Ssw_jumpBackToLink();
}

#if (IFX_CFG_SSW_ENABLE_PMS_INIT_CHECK == 1U)
void Ifx_Ssw_Pms_InitCheck(void)
{

    if (IfxPmsEvr_areInitValuesRight(&IfxPmsEvr_checkRegCfgDefault) == 0)
    {
        __debug();
    }
    Ifx_Ssw_jumpBackToLink();
}
#endif /* End of Ifx_Ssw_Pms_InitCheck() */
#endif /* End of Ifx_Ssw_Pms_Init() */

#if (IFX_CFG_SSW_ENABLE_LBIST == 1)
void Ifx_Ssw_Lbist(void)
{
    if (!IfxScuLbist_isDone())
    {
        IfxScuLbist_triggerInline(&IfxScuLbist_defaultConfig);
    }
    if (IfxScuLbist_evaluateResult(IfxScuLbist_defaultConfig.signature))
    {
         __debug();
    }

    Ifx_Ssw_jumpBackToLink();
}
#endif

#if (IFX_CFG_SSW_ENABLE_MONBIST == 1U)

static void Ifx_Ssw_MonbistCheck(void);

void Ifx_Ssw_Monbist(void)
{
	 uint32 timeout;

     /* Disable the write-protection for registers */
    IFX_CFG_SSW_CLEAR_SAFETY_ENDINIT();
    /* Enable SMU Standby */
    PMS_CMD_STDBY.U    = 0x40000001U;
    /* clears the TSTEN, TSTRUN, TSTDONE, TSTOK, SMUERR and PMSERR flags */
    PMS_MONBISTCTRL.U  = 0x40000002U;
    /* Clear the filter */
    PMS_MONFILT.U = 0x20000000U;
    /* Wait until register is updated */
    timeout = 0x1000U;
    while ((PMS_MONFILT.U != 0x20000000U) && (timeout--))
    {};
    /* Activate under voltage and over voltage alarms */
    PMS_MONCTRL.U = 0xa5a5a5U;
    /* Wait until register is updated */
    timeout = 0x1000U;
    while ((PMS_MONCTRL.U != 0xa5a5a5U) && (timeout--))
    {};
    /* corresponding Over-voltage and Under-voltage interrupts are disabled */
    PMS_PMSIEN.U &= ~0x00000FFFU;
    /* Fault Signal reaction on alarms are disabled */
    PMS_AGFSP_STDBY0.U = 0x40000000U;
    PMS_AGFSP_STDBY1.U = 0x40000000U;
    /* FSP0EN and FSP1EN configuration bits are cleared to avoid spurious Error pin activation */
    PMS_CMD_STDBY.U |= 0x40000000U;
    PMS_CMD_STDBY.U |= 0x40000008U;
    /* Reset the MONFILT register */
    PMS_MONFILT.U = 0x00000000U;
    /* Start MONBIST test */
    PMS_MONBISTCTRL.U = 0x40000001U;

    /* Wait until MONBIST execution is done */
    timeout = 0x1000U;
    while ((PMS_MONBISTSTAT.B.TSTRUN == 1) && (PMS_MONBISTSTAT.B.TSTDONE == 0) && (timeout--))
    {};

    /* Disable SMU Standby */
    PMS_CMD_STDBY.U    = 0x40000000U;
    IFX_CFG_SSW_SET_SAFETY_ENDINIT();

    Ifx_Ssw_jumpToFunction(&Ifx_Ssw_MonbistCheck);
}

void Ifx_Ssw_MonbistCheck(void)
{
    /* Check for MONBIST error state */
    if ((PMS_MONBISTSTAT.B.TSTOK == 0U) || (PMS_MONBISTSTAT.B.SMUERR == 1U) || (PMS_MONBISTSTAT.B.PMSERR == 1U))
    {
	    __debug();
    }

    Ifx_Ssw_jumpBackToLink();
}
#endif

#if (IFX_CFG_SSW_ENABLE_EMEM_INIT == 1U)

void Ifx_Ssw_UnlockEmem(void)
{
    if(SCU_CHIPID.B.EEA == 1U)
    {
      /* Enable EMEM clock */
      EMEM_CLC.U = 0x00000000U;
      /* sync access */
      EMEM_CLC.U;
      /* Disable Lock */
      EMEM_SBRCTR.U = 0x00000002U;
      EMEM_SBRCTR.U = 0x00000006U;
      EMEM_SBRCTR.U = 0x0000000EU;
    }
}
#endif

#if defined(__TASKING__)
#pragma endoptimize
#elif defined(__HIGHTEC__)
#pragma GCC reset_options
#endif

#ifdef __DCC__
int main(void){
	return 0;
}
#endif
