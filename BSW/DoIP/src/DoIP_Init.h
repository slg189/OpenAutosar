

#ifndef DOIP_INIT_H
#define DOIP_INIT_H

#include "DoIP_Cfg.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

extern boolean DoIP_Initialized_b;
extern const DoIP_ConfigType* DoIP_activeConfig_pcst;

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#endif

