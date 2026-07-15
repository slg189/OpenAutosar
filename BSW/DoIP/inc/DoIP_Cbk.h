

#ifndef DOIP_CBK_H
#define DOIP_CBK_H

#include "DoIP_Types.h"
#include "DoIP_Cfg.h"

/**
 * @ingroup DOIP_CBK_H
 * This function is called to acquire the transmit data of an I-PDU segment (N-PDU).
 * Each call to this function provides the next part of the I-PDU data unless retry->TpDataState is TP_DATARETRY.
 * In this case the function restarts to copy the data beginning at the offset from the current position indicated by retry->TxTpDataCnt.
 * The size of the remaining data is written to the position indicated by availableDataPtr.
 * @param[in] TxPduId Identification of the transmitted I-PDU.
 * @param[in] PduInfoPtr Provides destination buffer and the number of bytes to copy. If not enough transmit data is available, no data is copied. The transport protocol module will retry. A copy size of 0 can be used to indicate state changes in the retry parameter.
 * @param[in] retry This parameter is expected to be a NULL_PTR as retry is not supported by the DoIP module.
 * @param[out] availableDataPtr Indicates the remaining number of bytes that are available in the DoIP Tx buffer.
 * @see [AR422:SWS_DoIP_00031]
 * @return BufReq_ReturnType \n BUFREQ_OK: Data has been copied to the transmit buffer completely as requested. \n BUFREQ_E_BUSY: Request could not be fulfilled as the required amount of Tx data is not available. TP layer might retry later on. No data has been copied. \n BUFREQ_E_NOT_OK: Data has not been copied. Request failed.
 */
BufReq_ReturnType DoIP_SoAdTpCopyTxData(PduIdType TxPduId, const PduInfoType* PduInfoPtr, RetryInfoType* retry,
        PduLengthType* availableDataPtr);

/**
 * @ingroup DOIP_CBK_H
 * This function is called after the I-PDU has been transmitted on its network, the result indicates whether the transmission was successful or not.
 * @param[in] TxPduId Identification of the transmitted I-PDU.
 * @param[in] result Result of the transmission of the I-PDU.
 * @see [AR422:SWS_DoIP_00032]
 * @return void
 */
void DoIP_SoAdTpTxConfirmation(PduIdType TxPduId, Std_ReturnType result);

/**
 * @ingroup DOIP_CBK_H
 * This function is called to provide the received data of an I-PDU segment (N-PDU) to the upper layer.
 * Each call to this function provides the next part of the I-PDU data.
 * The size of the remaining data is written to the position indicated by bufferSizePtr.
 * @param[in] RxPduId Identification of the received I-PDU
 * @param[in] PduInfoPtr Requests Pointer to receive buffer and maximum receiveable length in the DoIP receive buffer.
 * @param[out] bufferSizePtr Available receive buffer after data has been copied.
 * @see [AR422:SWS_DoIP_00033]
 * @return BufReq_ReturnType \n BUFREQ_OK: Buffer request accomplished successful. \n BUFREQ_E_NOT_OK: Buffer request not successful. Buffer cannot be accessed.
 */
BufReq_ReturnType DoIP_SoAdTpCopyRxData(PduIdType RxPduId, const PduInfoType* PduInfoPtr, PduLengthType* bufferSizePtr);

/**
 * @ingroup DOIP_CBK_H
 * This function is called at the start of receiving an N-SDU.
 * The N-SDU might be fragmented into multiple N-PDUs (FF with one or more following CFs) or might consist of a single N-PDU (SF).
 * @param[in] RxPduId Identification of the I-PDU.
 * @param[in] TpSduLength Total length of the PDU to be received.
 * @param[out] bufferSizePtr Available receive buffer in the DoIP module.
 * @see [AR422:SWS_DoIP_00037]
 * @return BufReq_ReturnType \n BUFREQ_OK: Reception has been accepted. RxBufferSizePtr indicates the available receive buffer. \n BUFREQ_E_NOT_OK: Reception has been rejected. RxBufferSizePtr remains unchanged. \n BUFREQ_E_OVFL: No Buffer of the required length can be provided.
 */
BufReq_ReturnType DoIP_SoAdTpStartOfReception(PduIdType RxPduId, const PduInfoType* info, PduLengthType TpSduLength,
        PduLengthType* bufferSizePtr);

/**
 * @ingroup DOIP_CBK_H
 * This function will be called by the SoAd after an I-PDU has been received successfully or when an error occurred.
 * It is also used to confirm cancellation of an I-PDU.
 * @param[in] RxPduId Identification of the received I-PDU.
 * @param[in] result Result of the reception.
 * @see [AR422:SWS_DoIP_00038]
 * @return void
 */
void DoIP_SoAdTpRxIndication(PduIdType RxPduId, Std_ReturnType result);

/**
 * @ingroup DOIP_CBK_H
 * Indication of a received I-PDU from a lower layer communication interface module.
 * @param[in] RxPduId ID of the received I-PDU.
 * @param[in] PduInfoPtr Contains the length (SduLength) of the received I-PDU and a pointer to a buffer (SduDataPtr) containing the I-PDU.
 * @see [AR422:SWS_DoIP_00244]
 * @return void
 */
void DoIP_SoAdIfRxIndication(PduIdType RxPduId, const PduInfoType* PduInfoPtr);

/**
 * @ingroup DOIP_CBK_H
 * The lower layer communication interface module confirms the transmission of an I-PDU.
 * @param[in] TxPduId ID of the I-PDU that has been transmitted.
 * @see [AR422:SWS_DoIP_00245]
 * @return void
 */
void DoIP_SoAdIfTxConfirmation(PduIdType TxPduId);

/**
 * @ingroup DOIP_CBK_H
 * Notification about a SoAd socket connection state change, e.g. socket connection gets online
 * @param[in] SoConId Socket connection index specifying the socket connection with the mode change.
 * @param[in] Mode new mode
 * @see [AR422:SWS_DoIP_00039]
 * @return void
 */
void DoIP_SoConModeChg(SoAd_SoConIdType SoConId, SoAd_SoConModeType Mode);

/**
 * @ingroup DOIP_CBK_H
 * This function gets called by the SoAd if an IP address assignment related to a socket connection changes (i.e. new address assigned or assigned address becomes invalid).
 * @param[in] SoConId Socket connection index specifying the socket connection where the IP address assignment has changed.
 * @param[in] State state of IP address assignment
 * @see [AR422:SWS_DoIP_00040]
 * @return void
 */
void DoIP_LocalIpAddrAssignmentChg(SoAd_SoConIdType SoConId, TcpIp_IpAddrStateType State);

#if (DOIP_CFG_CHECK_API_CONSISTENCY == TRUE)
/**
 * @ingroup DOIP_CBK_H
 * This function is used to notify the DoIP on a switch of the DoIPActivationLine to active
 * @see [AR430:SWS_DoIP_00251]
 * @return void
 */
void DoIP_ActivationLineSwitchActive(void);

/**
 * @ingroup DOIP_CBK_H
 * This function is used to notify the DoIP on a switch of the DoIPActivationLine to inactive
 * @see [AR430:SWS_DoIP_91001]
 * @return void
 */
void DoIP_ActivationLineSwitchInactive(void);
#endif

#endif
