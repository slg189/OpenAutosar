/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */

#ifndef ETHIF_INTEGRATION_H
#define ETHIF_INTEGRATION_H

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "Eth_GeneralTypes.h"

/*
 ***************************************************************************************************
 * Define MACRO to process TX Buffer Address.									
 * According to the [SWS_TCPIP_00131] TcpIp shall always call EthIf_Transmit() with parameter
 * TxConfirmation set to FALSE. Therefore, EthIf_TxConfirmation will not be called until EthTSyn module
 * is integrated.
 * Therefore, by default, ETHIF_TXBUFADDR_PROC_USED is set to STD_OFF. In case EthIf_TxConfirmation is
 * used, please change this macro to STD_ON.										   
 ***************************************************************************************************
 */
#define ETHIF_TXBUFADDR_PROC_USED STD_ON

/*
 ***************************************************************************************************
 *  This Macro is used to define AR version of Eth Driver
 *  Example:
 *  For AR 4.0.3, the minor version is 0U
 *  For AR 4.2.2, the minor version is 2U
 *  Default is 2U for compliance with AR 4.2.2
 *
 ***************************************************************************************************
 */
#define ETHIF_WRAPPER_ETH_VERSION (2U)

/*
 ***************************************************************************************************
 * RTA_BSW strickly required a compatible version of Eth Driver to be integrated with
 * Below is a fix version macro to indicate the Eth Driver version required.
 * Even users integrate with AR 4.0.3, these information should follow RTA_BSW version
 *
 *  If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * 	Ex: For infineon Eth Driver on TC26x Board, it is required to update
 * 		+ ETH_AR_RELEASE_MAJOR_VERSION --> ETH_17_ETHMAC_AR_RELEASE_MAJOR_VERSION  
 * 		+ ETH_AR_RELEASE_MINOR_VERSION --> ETH_17_ETHMAC_AR_RELEASE_MINOR_VERSION  
 ***************************************************************************************************
 */

#define ETH_AR_RELEASE_MAJOR_VERSION 4
#define ETH_AR_RELEASE_MINOR_VERSION 2

/*
 ***************************************************************************************************
 * Extern declarations																			   
 ***************************************************************************************************
 */
#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/* ETHIF231: Called asynchronously when mode has been read out. Triggered by previous Eth_SetControllerMode call.
   Can directly be called within the trigger functions. */
extern void EthIf_CtrlModeIndication(uint8 CtrlIdx, Eth_ModeType CtrlMode);

/*
 ***************************************************************************************************
 * Description: These API are created to support the gap between AR v4.0.3, v4.2.2
 * If EthIfEthDriverPrefix is used, users need update all Eth name to right one
 * Ex: For infineon Eth Driver on TC26x Board, it is required to update
 *  + Name of below APIs defined as Eth_<EthIfEthDriverPrefix>_<functionName>
 *    + Eth_ControllerInit_Internal --> Eth_17_EthMac_ControllerInit_Internal
 *    ...
 * For detail description of each function, please check the definition of each ones.																	   
 ***************************************************************************************************
 */
    
extern Std_ReturnType Eth_17_GEthMac_ControllerInit_Internal(uint8 CtrlIdx, uint8 CfgIdx );
extern Std_ReturnType Eth_UpdatePhysAddrFilter( uint8 CtrlIdx, const uint8* PhysAddrPtr, Eth_FilterActionType Action );
extern void Eth_SetPhysAddr( uint8 CtrlIdx_u8,const uint8 * PhysAddrPtr_pu8 );
extern BufReq_ReturnType Eth_17_GEthMac_ProvideTxBuffer_Internal(uint8 CtrlIdx, uint8* BufIdxPtr, Eth_DataType** BufPtr,uint16* LenBytePtr);
extern Std_ReturnType Eth_17_GEthMac_Transmit_Internal( uint8 CtrlIdx, uint8 BufIdx,Eth_FrameType FrameType, boolean TxConfirmation, uint16 LenByte, const uint8* PhysAddrPtr);
extern void Eth_17_GEthMac_Receive_Internal(uint8 CtrlIdx, uint8 FifoIdx, Eth_RxStatusType* RxStatusPtr);


/*
 ***************************************************************************************************
 * Description: These API are created to support the gap between AR v4.0.3 and v4.2.2
 * From AR v4.0.3 Eth required EthIf have a callback function as - 	EthIf_Cbk_RxIndication
 * For this reason, this API is only enable when using with Eth Mcal v4.0.3																   
 ***************************************************************************************************
 */
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
extern void EthIf_Cbk_RxIndication(uint8 CtrlIdx, Eth_DataType* DataPtr, uint16 LenByte);  
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
// Current Ethernet version is 4.2.2 - This API is not required
#endif

/*
 ***************************************************************************************************
 * Description: These API are created to support the gap between AR v4.0.3 and v4.2.2
 * EthIf_Cbk_TxConfirmation is only required for v4.0.3
 * For newer version, EthIf_TxConfirmation shall be used for callback reference from EthIf_Cbk.h																	   
 ***************************************************************************************************
 */
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
extern void EthIf_Cbk_TxConfirmation( uint8 CtrlIdx, uint8 BufIdx); 
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
extern void EthIf_TxConfirmation(uint8 CtrlIdx, Eth_BufIdxType BufIdx);
#endif

/*
 ***************************************************************************************************
 * Description: EthTrcv_TransceiverInit is removed from AR v4.2.2, this integration file define a stub function
 * It is used to integrate EthIf with other 3rd party EthTrcv AR v4.2.2																	   
 ***************************************************************************************************
 */
#if(ETHIF_WRAPPER_ETH_VERSION == 0)
#elif(ETHIF_WRAPPER_ETH_VERSION == 2)
extern FUNC(Std_ReturnType, ETH_CODE) EthTrcv_TransceiverInit( VAR(uint8, AUTOMATIC) TrcvIdx, VAR(uint8, AUTOMATIC) CfgIdx );
#endif

/**
*******************************************************************************************************
* Function name:  EthTrcv_TransceiverLinkStateRequest
* Description:    Request the given link state for the given Ethernet transceiver
*		This API is required by RTA_BSW EthIf, but it is not defined by AR. It is required integrator
* 		enable this stub function by uncomment below extern function when integrating with other 3rd party EthTrcv
*******************************************************************************************************/

//extern Std_ReturnType EthTrcv_TransceiverLinkStateRequest( uint8 TrcvIdx, EthTrcv_LinkStateType LinkState );

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_INTEGRATION_H */
