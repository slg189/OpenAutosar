/**
 * @file    Crc.h
 * @brief   AUTOSAR CRC 模块（最小演示版，用于验证 SCons 构建链路）。
 *
 * @copyright Copyright (c) 2026 OpenAutosar
 */
#ifndef CRC_H
#define CRC_H

#include <stdint.h>

/* AUTOSAR 标准返回类型 */
typedef uint8_t Std_ReturnType;
#define E_OK    ((Std_ReturnType)0)
#define E_NOT_OK ((Std_ReturnType)1)

/**
 * @brief 初始化 CRC 模块。
 */
void Crc_Init(void);

/**
 * @brief 计算 CRC-8（多项式 0x1D，初值由 startValue 决定）。
 *
 * @param[in] data        待计算数据指针
 * @param[in] length      数据长度（字节数）
 * @param[in] startValue  起始 CRC 值
 * @param[in] isFirstCall 是否为首次调用（TRUE 用 0xFF 作初值）
 * @return                计算得到的 CRC-8 值
 */
uint8_t Crc_CalculateCRC8(const uint8_t *data,
                          uint32_t length,
                          uint8_t startValue,
                          uint8_t isFirstCall);

#endif /* CRC_H */
