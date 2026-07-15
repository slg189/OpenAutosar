
#ifndef FLS_H
#define FLS_H

#include "Fls_17_Dmu.h"

#if 1

#define Fls_ConfigType		Fls_17_Dmu_ConfigType
#define Fls_Read(a,b,c) 	Fls_17_Dmu_Read(a,b,c)
#define Fls_GetJobResult	Fls_17_Dmu_GetJobResult
#define Fls_Compare(a,b,c) 	Fls_17_Dmu_Compare(a,b,c)
#define Fls_Write(a,b,c)	Fls_17_Dmu_Write(a,b,c)
#define Fls_Erase(a,b)		Fls_17_Dmu_Erase(a,b)
#define Fls_SetMode(Mode)	Fls_17_Dmu_SetMode(Mode)
#define Fls_MainFunction(a)	Fls_17_Dmu_MainFunction(a)
#define Fls_Init(a)			Fls_17_Dmu_Init(a)
#define Fls_ControlTimeoutDet(a)	Fls_17_Dmu_ControlTimeoutDet(a)
#define Fls_GetStatus() 	Fls_17_Dmu_GetStatus()
#define Fls_BlankCheck(a, b) 	Fls_17_Dmu_BlankCheck(a, b)

#else

#define Fls_Read(a,b,c) 	Fls_17_Dmu_Read(a-0xAF000000,b,c)
#define Fls_GetJobResult	Fls_17_Dmu_GetJobResult
#define Fls_Compare(a,b,c) 	Fls_17_Dmu_Compare(a-0xAF000000,b,c)
#define Fls_Write(a,b,c)	Fls_17_Dmu_Write(a-0xAF000000,b,c)
#define Fls_Erase(a,b)		Fls_17_Dmu_Erase(a-0xAF000000,b)
#define Fls_SetMode(Mode)	Fls_17_Dmu_SetMode(Mode)
#define Fls_MainFunction()	Fls_17_Dmu_MainFunction()
#define Fls_Init(a)			Fls_17_Dmu_Init(a)

#endif

/* CIX2SGH */
/* Fls_Mode for FeeFs1 */
#define Fls_Mode Fls_ConfigPtr->FlsStateVarPtr->FlsMode

/* Autosar specification version */
#define FLS_AR_RELEASE_MAJOR_VERSION 4
#define FLS_AR_RELEASE_MINOR_VERSION 2
#define FLS_AR_RELEASE_REVISION_VERSION 2

typedef uint32 Fls_AddressType;

typedef uint32 Fls_LengthType;

extern Std_ReturnType Fls_BlankCheck(Fls_AddressType TargetAddress,
                                         Fls_LengthType Length);
/* CIX2SGH */

#endif /* FLS_H */
