/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */

#include "Eth_GeneralTypes.h"
#include "EthIf_Cfg.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Integration.h"
/**
 ***************************************************************************************************
 * \module description
 * ETHIF231: Called asynchronously when mode has been read out. Triggered by previous
 * Eth_SetControllerMode call. Can directly be called within the trigger functions.
 * \par Service ID 0x0E, Non Reentrant for the same CtrlIdx, reentrant for different
 *
 * Parameter In:
 * \param CtrlIdx       Index of the Ethernet controller within the context of the Ethernet Interface
 * \param CtrlMode        Notified Ethernet controller mode
 *
 * \return  None
 *
 ***************************************************************************************************
 */
#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

void EthIf_CtrlModeIndication(uint8 CtrlIdx, Eth_ModeType CtrlMode)
{
    /* This function is not supported*/
}

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"
#endif /* ETHIF_CONFIGURED */
