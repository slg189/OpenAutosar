/* Vector MICROSAR COM 接口 - 实现见 src/ (编译后释放为 BSW_Libs/Com/libCom.a) */
#ifndef COM_H
#define COM_H
#include "Std_Types.h"
extern void           Com_Init(const void *config);
extern Std_ReturnType Com_SendSignal(uint16 SignalId, const void *SignalDataPtr);
#endif /* COM_H */
