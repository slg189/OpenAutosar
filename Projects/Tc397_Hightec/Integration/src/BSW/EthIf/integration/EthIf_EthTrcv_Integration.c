/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
 
#include "EthIf_Integration.h"

/***********************************************************************************************************
 * Function name    : EthTrcv_TransceiverInit                                                              *
 * Syntax           : Std_ReturnType EthTrcv_TransceiverInit(uint8 TrcvIdx, uint8 CfgIdx)                  *
 * Description      : From AUTOSAR 4.2.2, the EthTrcv_TransceiverInit functionality merged into            *
 *              Eth_Init API. This function is dummy function for EthIf's call in EthIf_Prv_TransceiverInit*
 *              In MCAL following AUTOSAR version older than AR4.2.2, because MCAL already implement the   *
 *      EthTrcv_TransceiverInit() function, integrators should remove the implementation of                *
 *      EthTrcv_TransceiverInit() in this file and the declaration of it in EthIf_Integration.h            * 
 * Parameter        :                                                                                      *
 *      TrcvIdx  Index of the transceiver within the context of the Ethernet Transceiver Driver            *
 *      CfgIdx   Index of the used configuration                                                           *
 * Return value     : E_OK                                                                                 *
/**************************************************************************************************/
// #define ETHIF_START_SEC_CODE
// #include "EthIf_MemMap.h"

// #if(ETHIF_WRAPPER_ETH_VERSION == 0)
// #elif(ETHIF_WRAPPER_ETH_VERSION == 2)
// FUNC(Std_ReturnType, ETH_CODE) EthTrcv_TransceiverInit( VAR(uint8, AUTOMATIC) TrcvIdx,
//                                                         VAR(uint8, AUTOMATIC) CfgIdx )
// {
// 	// dummy function whenn using EthIf with EthTrcv AR v4.2.2
// 	return E_OK;
// }
// #endif

/**
*******************************************************************************************************
* Function name:  EthTrcv_TransceiverLinkStateRequest
* Description:    Request the given link state for the given Ethernet transceiver
*		This API is required by RTA_BSW EthIf, but it is not defined by AR. It is required integrator
* 		enable this stub function by uncomment below function when integrating with other 3rd party EthTrcv
*		
*
* Arguments:    - TrcvIdx   Index of the transceiver on the MII
*               - LinkState Link State ACTIVE or DOWN
* Returns:      - E_OK - The request has been accepted
*               - E_NOT_OK - The request has not been accepted.
* Remarks:      - TJA1100 -  Request of this API will change the state of Link_Control bit
*               - Other Tranceivers - Request of this API will set the mode requested by EthTrcv_SetTransceiverMode
*******************************************************************************************************/

//Std_ReturnType EthTrcv_TransceiverLinkStateRequest( uint8 TrcvIdx, EthTrcv_LinkStateType LinkState )
//{
//
//    Std_ReturnType RetVal_o = E_OK;
//    return RetVal_o;
//} /* END EthTrcv_TransceiverLinkStateRequest */

// #define ETHIF_STOP_SEC_CODE
// #include "EthIf_MemMap.h"
