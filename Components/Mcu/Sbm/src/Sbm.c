#include "Sbm.h"

void Sbm_StartupInit(void)
{
    /* TODO: 关闭看门狗 -> 初始化时钟树 -> 清 .bss -> 拷贝 .data */
}

Std_ReturnType Sbm_VerifyApplication(void)
{
    /* TODO: 校验应用区 CRC / 签名 */
    return E_OK;
}
