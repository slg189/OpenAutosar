#ifndef IOHWAB_DIO_TEST_H
#define IOHWAB_DIO_TEST_H
#include "Std_Types.h"

#define IOHWAB_DIO_TEST_START_SEC_CODE
#include "IoHwAb_DIO_Test_MemMap.h"
 extern FUNC (Std_ReturnType, IoHwAb_DIO_Test_CODE) IoHwAb_DIO_Test_Set_func(VAR(boolean, AUTOMATIC) DioLevel);

 extern /*FUNC (void, IoHwAb_DIO_Test_CODE)*/void IoHwAb_DIO_Test_Init_func(void);
#define IOHWAB_DIO_TEST_STOP_SEC_CODE
#include "IoHwAb_DIO_Test_MemMap.h"
#endif