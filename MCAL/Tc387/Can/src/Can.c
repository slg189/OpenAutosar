#include "Can.h"

void Can_Init(const void *Config)
{
    (void)Config; /* TODO: 初始化 MultiCAN+ 节点与消息对象 */
}

Std_ReturnType Can_Write(Can_HwHandleType Hth, const uint8 *Pdu)
{
    (void)Hth;
    return (Pdu == NULL_PTR) ? E_NOT_OK : E_OK;
}
