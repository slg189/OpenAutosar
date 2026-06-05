/* 公共工具函数 */
#include "Std_Types.h"
uint16 Common_Crc16(const uint8 *data, uint16 len)
{
    uint16 crc = 0xFFFFU;
    for (uint16 i = 0U; i < len; ++i) { crc ^= (uint16)data[i]; }
    return crc;
}
