

#ifndef DOIP_LIB_H
#define DOIP_LIB_H

#include "DoIP_Cfg.h"
#include "rba_DiagLib_MemUtils.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

extern boolean DoIP_IsVinReady_b;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

uint8* DoIP_IntGetVin(void);

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#define DOIP_MEMCPY     RBA_DIAGLIB_MEMCPY
#define DOIP_MEMSET     RBA_DIAGLIB_MEMSET
#define DOIP_MEMCMP     RBA_DIAGLIB_MEMCMP

#endif

