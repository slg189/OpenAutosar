/******************************************************************************
 * \file    Sbm.h
 * \brief   Sbm (Startup / Secure Boot Manager) 启动代码模块接口
 *****************************************************************************/
#ifndef SBM_H
#define SBM_H
#include "Std_Types.h"

/* 复位后最早执行的启动入口, 在 main 之前完成时钟/看门狗/内存初始化 */
extern void Sbm_StartupInit(void);
/* 校验应用程序镜像完整性 */
extern Std_ReturnType Sbm_VerifyApplication(void);
#endif /* SBM_H */
