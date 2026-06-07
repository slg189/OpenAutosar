#include "CddPwm.h"
#include "CddPwm_Cfg.h"   /* 来自 gen/ 的配置代码 */

void CddPwm_Init(void)
{
    /* TODO: 依据 CddPwm_Cfg 初始化 GTM 通道 */
}

Std_ReturnType CddPwm_SetDutyCycle(CddPwm_ChannelType Ch, uint16 Duty)
{
    if (Ch >= CDDPWM_CHANNEL_COUNT) { return E_NOT_OK; }
    (void)Duty;
    return E_OK;
}
