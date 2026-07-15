

#ifndef DOIP_TYPES_H
#define DOIP_TYPES_H

#include "Std_Types.h"
#include "ComStack_Types.h"
#include "SoAd_Types.h"
#include "Rte_DoIP_Type.h"

/* Autosar */
#define DOIP_E_UNINIT                       0x01
#define DOIP_E_PARAM_POINTER                0x02
#define DOIP_E_INVALID_PDU_SDU_ID           0x03
#define DOIP_E_INVALID_PARAMETER            0x04
#define DOIP_E_INIT_FAILED                  0X05
#define DOIP_E_PENDING                      0x10

/* RB */
#define DOIP_E_NOT_SUPPORTED                0xFD
#define DOIP_E_ASSERT                       0xFE
#define DOIP_E_SHOULD_NOT_REACH             0xFF

typedef struct DoIP_ConfigType DoIP_ConfigType;

#ifndef DOIP_NOT_READY
typedef uint8 DoIP_PowerStateType;
#define DOIP_NOT_READY        0
#define DOIP_READY            1
#define DOIP_NOT_SUPPORTED    2
#endif

#ifndef DOIP_ACTIVATION_LINE_INACTIVE
typedef uint8 DoIP_ActivationLineRequestType;
#define DOIP_ACTIVATION_LINE_INACTIVE    0
#define DOIP_ACTIVATION_LINE_ACTIVE      1
#endif

#endif

