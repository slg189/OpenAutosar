/**
 * @file    Crc.c
 * @brief   CRC 模块实现（演示版）。
 */
#include "Crc.h"

void Crc_Init(void)
{
    /* 模块级初始化（演示版无需操作） */
}

uint8_t Crc_CalculateCRC8(const uint8_t *data,
                          uint32_t length,
                          uint8_t startValue,
                          uint8_t isFirstCall)
{
    uint8_t crc = (isFirstCall != 0u) ? 0xFFu : startValue;
    uint32_t i;
    uint8_t  bit;

    if (data == ((void *)0)) {
        return 0u;
    }

    for (i = 0u; i < length; ++i) {
        crc ^= data[i];
        for (bit = 0u; bit < 8u; ++bit) {
            if ((crc & 0x80u) != 0u) {
                crc = (uint8_t)((crc << 1) ^ 0x1Du);
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}
