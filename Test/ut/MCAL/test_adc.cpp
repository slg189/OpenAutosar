// ADC 驱动单元测试示例 (GoogleTest)
#include <gtest/gtest.h>
extern "C" {
#include "Adc.h"
}
TEST(AdcTest, ReadGroupRejectsNull) {
    EXPECT_EQ(E_NOT_OK, Adc_ReadGroup(0, nullptr));
}
