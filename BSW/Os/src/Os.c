/* ETAS RTA-OS (AUTOSAR OS) - 演示源码实现 (供 BSW 源码构建链路验证)。
 * 说明: BSW 文件夹存放供应商源码; 编译产物 .a 释放到 BSW_Libs/。 */
#include "Os.h"

void StartOS(uint8 Mode)
{
    (void)Mode;
}

Std_ReturnType ActivateTask(TaskType TaskID)
{
    (void)TaskID;
    return E_OK;
}
