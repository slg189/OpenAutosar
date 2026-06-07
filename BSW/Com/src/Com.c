/* Vector MICROSAR COM - 演示源码实现 (供 BSW 源码构建链路验证)。
 * 说明: BSW 文件夹存放供应商源码; 编译产物 .a 释放到 BSW_Libs/。 */
#include "Com.h"

void Com_Init(const void *config)
{
    (void)config;
}

Std_ReturnType Com_SendSignal(uint16 SignalId, const void *SignalDataPtr)
{
    (void)SignalId;
    (void)SignalDataPtr;
    return E_OK;
}
