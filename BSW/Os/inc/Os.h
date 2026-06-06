/* ETAS RTA-OS 接口 (AUTOSAR OS) - 实现见 src/ (编译后释放为 BSW_Libs/Os/libRtaOs.a) */
#ifndef OS_H
#define OS_H
#include "Std_Types.h"
typedef uint8 TaskType;
extern void StartOS(uint8 Mode);
extern Std_ReturnType ActivateTask(TaskType TaskID);
#endif /* OS_H */
