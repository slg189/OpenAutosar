/* Vector MICROSAR COM 接口 - 实现由 libCom.a 提供 */
#ifndef COM_H
#define COM_H
#include "Std_Types.h"
extern void           Com_Init(const void *config);
extern Std_ReturnType Com_SendSignal(uint16 SignalId, const void *SignalDataPtr);
#endif /* COM_H */
