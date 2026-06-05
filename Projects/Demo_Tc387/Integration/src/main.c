/******************************************************************************
 * \file    main.c
 * \brief   集成入口 — 基础工程师维护; 调用各层(以 .a 释放)的对外接口。
 *****************************************************************************/
#include "Std_Types.h"
#include "Sbm.h"      /* Components 启动模块 */
#include "EcuM.h"     /* BSW_Libs */
#include "Os.h"       /* BSW_Libs (ETAS RTA-OS) */

int main(void)
{
    Sbm_StartupInit();          /* 启动初始化 (.a) */
    if (Sbm_VerifyApplication() != E_OK)
    {
        for (;;) { /* 校验失败: 安全停机 */ }
    }
    EcuM_Init();                /* BSW 初始化 (.a) */
    StartOS(0U);                /* 启动 AUTOSAR OS (.a) */
    return 0;
}
