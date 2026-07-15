

#ifndef DOIP_PRV_H
#define DOIP_PRV_H

/* API */
#include "DoIP.h"
#include "DoIP_Cbk.h"

/* Internal */
#include "DoIP_ActivationLine.h"
#include "DoIP_AliveCheck.h"
#include "DoIP_Callback.h"
#include "DoIP_Channel.h"
#include "DoIP_Connection.h"
#include "DoIP_DiagnosticMessage.h"
#include "DoIP_GenericAcknowledge.h"
#include "DoIP_Init.h"
#include "DoIP_IpAddressAssignment.h"
#include "DoIP_Lib.h"
#include "DoIP_Header.h"
#include "DoIP_Prv_Types.h"
#include "DoIP_Prv_Cfg_Types.h"
#include "DoIP_Prv_Det.h"
#include "DoIP_RoutingActivation.h"
#include "DoIP_TCPConnection.h"
#include "DoIP_UDPConnection.h"
#include "DoIP_VehicleIdentification.h"

/* Cfg */
#include "DoIP_Cfg.h"
#include "DoIP_Cfg_HeaderInclusion.h"
#include "DoIP_Cfg_PbCfg.h"
#include "DoIP_Cfg_SchM.h"
#include "DoIP_Cfg_Version.h"

/* External header */
#include "PduR_DoIP.h"
#include "Rte_DoIP.h"
#include "SoAd.h"
#include "Dcm.h"
#include "rba_DiagLib_ByteOrderUtils.h"

/**
 **************************************************************************************************
 * macro to suppress QAC warnings for unused parameters
 **************************************************************************************************
 */
#define DOIP_UNUSED_PARAM(P)   ((void)(P))

/**
 **************************************************************************************************
 * macro for MIN and MAX
 **************************************************************************************************
 */
#define  DOIP_MIN(a,b)  (((a)<(b))?(a):(b))
#define  DOIP_MAX(a,b)  (((a)>(b))?(a):(b))

#endif

