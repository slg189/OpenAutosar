

#ifndef DOIP_H
#define DOIP_H

#include "DoIP_Types.h"
#include "DoIP_Cfg.h"

#if(DOIP_VARIANT != DOIP_PRE_COMPILE)
#include "DoIP_Cfg_PbCfg.h"
#endif

/**
 * \defgroup DOIP_H
 *  The header file contains all the interface functions required in Autosar.
 *  see AUTOSAR_SWS_DiagnosticOverIP.pdf Version 4.2 Rev 2
 *  To use this interface include the header <b>DoIP.h</b>
 */

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

//8.3 function definitions

/**
 * @ingroup DOIP_H
 * This service is called to request the transfer data from the PduRouter to the SoAd.
 * It is used to indicate the transmission which will be performed in the DoIP_Mainfunction.
 * Within the provided DoIPPduRTxInfoPtr only SduLength is valid (no data)!
 * If this function returns E_OK then the DoIP module will raise a subsequent call to PduR_DoIPCopyTxData in order to get the data to send.
 * @param[in] DoIPPduRTxId DoIP unique identifier of the PDU to be transmitted by the PduR.
 * @param[in] DoIPPduRTxInfoPtr Tx Pdu information structure which contains the length of the DoIPTxMessage.
 * @see [AR422:SWS_DoIP_00022]
 * @return Std_ReturnType \n E_OK: The request has been accepted \n E_NOT_OK: The request has not been accepted, e.g. parameter check has failed or no resources are available for transmission
 */
Std_ReturnType DoIP_TpTransmit(PduIdType DoIPPduRTxId, const PduInfoType* DoIPPduRTxInfoPtr);

/**
 * @ingroup DOIP_H
 * This service is not supported.\n
 * This service primitive is used to cancel the transfer of pending DoIPPduRTxIds.
 * The connection is identified by DoIPPduRTxId. When the function returns, no transmission is in progress anymore with the given DoIPPduRTxId identifier.
 * @see [AR422:SWS_DoIP_00023]
 * @return Std_ReturnType
 */
Std_ReturnType DoIP_TpCancelTransmit(PduIdType DoIPPduRTxId);

/**
 * @ingroup DOIP_H
 * This service is not supported.\n
 * Only a dummy function is implemented.\n
 * By calling this API with the corresponding DoIPPduRRxId the currently ongoing data reception is terminated immediately.
 * When the function returns, no reception is in progress anymore with the given DoIPPduRRxId identifier.
 * @see [AR422:SWS_DoIP_00024]
 * @return Std_ReturnType
 */
Std_ReturnType DoIP_TpCancelReceive(PduIdType DoIPPduRRxId);

/**
 * @ingroup DOIP_H
 * This service is not supported.\n
 * Only a dummy function is implemented.\n
 * Requests transmission of an I-PDU.
 * @param[in] DoIPPduRRxId DoIP unique identifier of the PDU to be canceled by the PduR.
 * @param[in] DoIPPduRTxInfoPtr Length and pointer to the buffer of the I-PDU.
 * @see [AR422:SWS_DoIP_00277]
 * @return Std_ReturnType
 */
Std_ReturnType DoIP_IfTransmit(PduIdType DoIPPduRTxId ,const PduInfoType* DoIPPduRTxInfoPtr);

/**
 * @ingroup DOIP_H
 * This service is not supported.\n
 * Only a dummy function is implemented.\n
 * Requests cancellation of an ongoing transmission of an I-PDU in a lower layer communication interface module.
 * @see [AR422:SWS_DoIP_00278]
 * @return Std_ReturnType
 */
Std_ReturnType DoIP_IfCancelTransmit(PduIdType DoIPPduRTxId);

/**
 * @ingroup DOIP_H
 * This service initializes all global variables of the DoIP module.
 * After return of this service the DoIP module is operational.
 * @param[in] DoIPConfigPtr Pointer to a data structure for the post build parameters of the DoIP module.
 * @see [AR422:SWS_DoIP_00026]
 * @return void
 */
void DoIP_Init(const DoIP_ConfigType* DoIPConfigPtr);

/**
 * @ingroup DOIP_H
 * This service returns the version information of this module.
 * The function DoIP_GetVersionInfo shall return the version information of the DoIP module.
 * The version information includes: Module Id, Vendor Id, Vendor specific version numbers.
 * @param[out] versioninfo A pointer to the struct with the version information.
 * @see [AR422:SWS_DoIP_00027]
 * @return void
 */
void DoIP_GetVersionInfo(Std_VersionInfoType* versioninfo);

//8.5 scheduled functions

/**
 * @ingroup DOIP_H
 * Schedules the Diagnostic over IP module. (Entry point for scheduling)
 * @see [AR422:SWS_DoIP_00041]
 * @return void
 */
void DoIP_MainFunction(void);

/* TODO
 * Temporary work around, for backward compatibility to PduR module,
 * Shall be removed once PduR interfaces are migrated to AR4.2 */
#define DoIP_Transmit DoIP_TpTransmit
#define DoIP_CancelReceive DoIP_TpCancelReceive
#define DoIP_CancelTransmit DoIP_TpCancelTransmit

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

#endif

