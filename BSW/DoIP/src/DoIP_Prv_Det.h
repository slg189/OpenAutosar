

#ifndef DOIP_PRV_DET_H
#define DOIP_PRV_DET_H

#include "DoIP.h"
#if (DOIP_CFG_DevelopmentErrorDetect == TRUE)
#include "Det.h"
#endif

//service IDs from Autosar
#define DOIP_DET_APIID_GetVersionInfo                   0x00u
#define DOIP_DET_APIID_Init                             0x01u
#define DOIP_DET_APIID_MainFunction                     0x02u
#define DOIP_DET_APIID_TP_Transmit                      0x03u
#define DOIP_DET_APIID_TP_CancelTransmit                0x04u
#define DOIP_DET_APIID_TP_CancelReceive                 0x05u
#define DOIP_DET_APIID_SoConModeChg                     0x0Bu
#define DOIP_DET_APIID_LocalIpAddrAssignmentChg         0x0Cu
#define DOIP_DET_APIID_ActivationLineSwitchInactive     0x0Eu
#define DOIP_DET_APIID_ActivationLineSwitchActive       0x0Fu
#define DOIP_DET_APIID_IF_TxConfirmation                0x40u
#define DOIP_DET_APIID_IF_RxIndication                  0x42u
#define DOIP_DET_APIID_TP_CopyTxData                    0x43u
#define DOIP_DET_APIID_TP_CopyRxData                    0x44u
#define DOIP_DET_APIID_TP_RxIndication                  0x45u
#define DOIP_DET_APIID_TP_StartOfReception              0x46u
#define DOIP_DET_APIID_TP_TxConfirmation                0x48u
#define DOIP_DET_APIID_IF_Transmit                      0x49u
#define DOIP_DET_APIID_IF_CancelTransmit                0x4Au
//0x80-0xFF user defined
//#define DOIP_DET_APIID                                0x80u
#define DOIP_DET_APIID_UTIL                             0x81u
#define DOIP_DET_APIID_CONNECTION                       0x82u
#define DOIP_DET_APIID_TCPCONNECTION                    0x83u
#define DOIP_DET_APIID_UDPCONNECTION                    0x84u
#define DOIP_DET_APIID_ROUTINGACTIVATION                0x85u
#define DOIP_DET_APIID_GENERIC_HEADER                   0x86u

#define DOIP_DET_INSTANCE_ID                            0x00u

#if (DOIP_CFG_DevelopmentErrorDetect == TRUE)
/* DoIP uses below mentioned DET API interface function to report the development error to DET Module */
#define DOIP_DET(APIID,ERRORID)       (void) Det_ReportError(DOIP_MODULE_ID,DOIP_DET_INSTANCE_ID,APIID,ERRORID)
#else
#define DOIP_DET(APIID,ERRORID)
#endif

#if (DOIP_CFG_DevelopmentErrorDetect == TRUE)
/* DoIP uses below mentioned DET API interface function to report the development error to DET Module */
#define DOIP_ASSERT(C,APIID,ERRORID)   do { if (!(C)) { DOIP_DET(APIID,ERRORID); } } while(0)
#else
#define DOIP_ASSERT(C,APIID,ERRORID)
#endif

#endif

