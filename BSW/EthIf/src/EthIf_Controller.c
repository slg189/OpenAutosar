

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 ***************************************************************************************************
 * \module description
 * ETHIF034: Enables / disables the indexed controller
 * \par Service ID 0x03, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of the Ethernet controller within the context of the Ethernet Interface
 * \param CtrlMode      ETHCTRL_MODE_DOWN: disable the controller ETHCTRL_MODE_ACTIVE: enable the controller
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: controller mode could not be changed}
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_SetControllerMode( uint8 CtrlIdx,
                                        Eth_ModeType CtrlMode )
{
    /* Declare local variables */
    Std_ReturnType      lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETCONTROLLERMODE,
                                  ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_SETCONTROLLERMODE,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK)

    /* Initialise local variable */
    lStdRetVal_en = E_NOT_OK;

    /* Error flag shall be reset only when the requested CtrlMode is ACTIVE, */
    /* to ensure that the reset is done only when there is an active (restart) request from EthSM. */
    if( CtrlMode == ETH_MODE_ACTIVE )
    {
        /* If mode is requested to ACTIVE and EthIfCtrl is not yet DOWN (except for Link-Info), request is not accepted */
        /* Before EthSM can start FULL_COMM sequence, EthIf need to be in proper DOWN state */
        /* For Link-Info EthIfCtrl, it is allowed to start EthIfCtrl again even if DOWN is not yet reached */
        if(( EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].State_en == ETHIF_ETHIFCTRL_STATE_DOWN ) ||
           ( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].ReferenceType_cen == ETHIF_ETHIFCTRL_REF_PORTGROUP_LINKINFO ))
        {
            /* Store the request, which will be processed in the next main function */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].ReqMode_en = CtrlMode;

            /* Clear the error flag for the EthIfCtrl */
            /* When error flag is set for an EthIfCtrl, the flag is reset at the time EthSM request the restart of the EthIfCtrl via EthIf_SetControllerMode */
            EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].ErrorFlg_en = ETHIF_ETHIFCTRL_ERROR_NONE;

            /* ACTIVE request is always accepted. */
            lStdRetVal_en = E_OK;
        }
    }
    else
    {
        /* Store the request, which will be processed in the next main function */
        EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].ReqMode_en = CtrlMode;

        /* DOWN request is always accepted */
        lStdRetVal_en = E_OK;
    }

    return lStdRetVal_en;
}


/**
 ***************************************************************************************************
 * \module description
 * ETHIF039: Obtains the state of the indexed controller
 * \par Service ID 0x04, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of the Ethernet controller within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param CtrlModePtr   {ETHCTRL_MODE_DOWN: the controller is disabled; ETHCTRL_MODE_ACTIVE: the controller is enabled}
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: controller could not be initialised}
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_GetControllerMode( uint8 CtrlIdx,
                                        Eth_ModeType * CtrlModePtr )
{
    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETCONTROLLERMODE,
                                  ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_GETCONTROLLERMODE,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlModePtr == NULL_PTR), ETHIF_SID_ETHIF_GETCONTROLLERMODE, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Return the EthIfCtrl mode */
    if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE) ||
        (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].State_en == ETHIF_ETHIFCTRL_STATE_WAIT_TRCVMODE_DOWN) )
    {
        *CtrlModePtr = ETH_MODE_ACTIVE;
    }
    else
    {
        *CtrlModePtr = ETH_MODE_DOWN;
    }

    return E_OK;
}


/**
 ***************************************************************************************************
 * \module description
 * ETHIF91052: Returns the VLAN identifier of the requested Ethernet controller
 * \par Service ID 0x43, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of the Ethernet controller within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param VlanIdPtr     Pointer to store the VLAN identifier (VID) of the Ethernet controller
 *                      0 if  the Ethernet controller represents no virtual network (VLAN)
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: failure}
 *
 ***************************************************************************************************
 */
#if( ETHIF_ENABLE_ETHIF_GET_VLANID == STD_ON )
Std_ReturnType EthIf_GetVlanId( uint8 CtrlIdx,
                                uint16 * VlanIdPtr )
{
    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT),
                                   ETHIF_SID_ETHIF_GETVLANID, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_GETVLANID,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET if VlanIdPtr is a Null Pointer */
    ETHIF_REPORT_ERROR_RET_VALUE( (VlanIdPtr == NULL_PTR), ETHIF_SID_ETHIF_GETVLANID,
                                  ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Fetch the corresponding Vlan Id for the EthIf CtrlIdx */
    *VlanIdPtr = EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].VlanId_cu16;

    /* If the CtrlIdx does not have a Vlan Id, then the value 0 is returned */
    if( *VlanIdPtr == 0xFFFFUL )
    {
        /* Zero is a valid VlanId incase of priority tagged frames. Hence 0 means either untagged or priority tagged frame */
        *VlanIdPtr = 0U;
    }

    return E_OK;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
