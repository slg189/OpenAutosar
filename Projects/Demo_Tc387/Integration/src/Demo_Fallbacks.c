/******************************************************************************
 * \file    Demo_Fallbacks.c
 * \brief   Weak fallback symbols for the offline Demo_Tc387 host build.
 *
 * These functions keep the integration smoke build linkable when delivered
 * vendor/application libraries only provide headers. A real library definition
 * overrides the weak fallback on GNU-compatible toolchains.
 *****************************************************************************/
#include "EcuM.h"
#include "EnergyManagement.h"

#if defined(HOST_BUILD)
#define OA_DEMO_WEAK
#elif defined(__GNUC__)
#define OA_DEMO_WEAK __attribute__((weak))
#else
#define OA_DEMO_WEAK
#endif

OA_DEMO_WEAK void EcuM_Init(void)
{
}

OA_DEMO_WEAK void EnergyManagement_Step(void)
{
}
