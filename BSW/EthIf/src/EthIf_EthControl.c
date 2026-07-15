

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Cfg_SchM.h"
#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/**
 ***************************************************************************************************
 * \module description
 * Initialises the physical controller mapped to the EthIfCtrl given in argument
 * \par Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param EthIfCtrlIdx_u8   Index of the Ethernet controller within the context of the Ethernet Interface
 *
 * \return  Std_ReturnType { E_OK: success; E_NOT_OK: physical controller could not be initialised }
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_Prv_ControllerInit( uint8 EthIfCtrlIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;

    /* Initialize the related PhysCtrl */
    lStdRetVal_en = EthIf_Prv_PhysCtrlInit( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefPhysCtrlTableIdx_cu8 );

    return lStdRetVal_en;
}


/**
 ***************************************************************************************************
 * \module description
 * Initialises the indexed physical controller
 * \par Synchronous, Reentrant
 * This function need to be reentrant because it can be called from API EthIf_SetPhysCtrlMode() and also from State Machines execution context
 *
 * Parameter In:
 * \param EthIfPhysCtrlIdx_u8           Index of the EthIfPhysController within the context of the Ethernet Interface
 *
 * \return  Std_ReturnType { E_OK: success; E_NOT_OK: physical controller could not be initialised }
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_Prv_PhysCtrlInit( uint8 EthIfPhysCtrlIdx_u8 )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;
    EthIf_DynamicPhysCtrl_tst*          lDynamicPhysCtrl_pst;

    /* If requested EthCtrl is no more in state ETHIF_CTRL_STATE_UNINIT no need to call driver API. Return with E_OK */
    lStdRetVal_en = E_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8];

    /* Enter critical section: EthCtrl state is being changed, in the meantime no call to Eth APIs shall be done */
    /* Also, EthIf_Prv_PhysCtrlInit() can be called from the execution context of EthIf_MainFunctionState() and also from EthIf_SetPhysCtrlMode()  */
    /* As the content of this function and the EthDriver API Eth_ControllerInit are not reentrant, a critical section is required  */
    SchM_Enter_EthIf_EthCtrlState();

    /* Check if EthCtrl initialization is not already called */
    if( lDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_UNINIT )
    {
        /* Call Eth_<vi>_<ai>_ControllerInit() API */
        lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthControllerInit_pfct(
                                lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                0U );

        /* If driver API returns E_OK set EthCtrl state in EthIf to ETHIF_CTRL_STATE_DOWN */
        if( lStdRetVal_en == E_OK )
        {
            lDynamicPhysCtrl_pst->CtrlState_en = ETHIF_CTRL_STATE_DOWN;
        }
    }

    /* Exit critical section */
    SchM_Exit_EthIf_EthCtrlState();

    return lStdRetVal_en;
}


/**
 ***************************************************************************************************
 * \module description
 * Enables / disables the physical controller mapped to the EthIfCtrl given in argument
 * \par Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param EthIfPhysCtrlIdx_u8           Index of the EthIf PhysController whose mode has to be changed
 * \param CtrlReqMode_en                {ETHCTRL_MODE_DOWN: disable the controller, ETHCTRL_MODE_ACTIVE: enable the controller}
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: controller mode could not be changed}
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_Prv_SetControllerMode( uint8 EthIfCtrlIdx_u8,
                                            Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;

    /* Change the mode of the related PhysCtrl */
    lStdRetVal_en = EthIf_Prv_SetPhysCtrlMode( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[EthIfCtrlIdx_u8].RefPhysCtrlTableIdx_cu8,
                                               CtrlReqMode_en );

    return lStdRetVal_en;
}


/**
 ***************************************************************************************************
 * \module description
 * Enables / disables the indexed physical controller
 * \par Synchronous, Reentrant
 * This function need to be reentrant because it can be called from API EthIf_SetPhysCtrlMode() and also from State Machines execution context
 *
 * If ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT is ON (that is: physical address filtering is activated in the driver), then
 * it clears the physical address filters for the specified physical eth controller index before setting the controller to
 * ETH_MODE_ACTIVE mode.
 *
 * Parameter In:
 * \param EthIfPhysCtrlIdx_u8           Index of the EthIfPhysController within the context of the Ethernet Interface
 * \param CtrlReqMode_en                {ETHCTRL_MODE_DOWN: disable the controller, ETHCTRL_MODE_ACTIVE: enable the controller}
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: controller mode could not be changed}
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_Prv_SetPhysCtrlMode( uint8 EthIfPhysCtrlIdx_u8,
                                          Eth_ModeType CtrlReqMode_en )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;
    EthIf_DynamicPhysCtrl_tst*          lDynamicPhysCtrl_pst;

    /* Initialise local variables */
    lStdRetVal_en = E_NOT_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIfPhysCtrlIdx_u8];

    /* Enter critical section: EthCtrl state is being changed, in the meantime no call to Eth APIs shall be done */
    /* Also, EthIf_Prv_PhysCtrlInit() can be called from the execution context of EthIf_MainFunctionState() and also from EthIf_SetPhysCtrlMode()  */
    /* As the content of this function and the EthDriver API Eth_ControllerInit are not reentrant, a critical section is required  */
    SchM_Enter_EthIf_EthCtrlState();

    /* -------------------------------------- */
    /* Request DOWN                           */
    /* -------------------------------------- */

    /* If EthIf_SetControllerMode() is called with ETH_MODE_DOWN */
    if( CtrlReqMode_en == ETH_MODE_DOWN )
    {
        /* Check the ActiveCntr of the EthCtrl to know if it is used by any other EthIfCtrl */
        if( lDynamicPhysCtrl_pst->ActiveCntr_u8 == 1U )
        {
            /* We are in presence of the last EthIfCtrl using the EthCtrl */

            /* If no other EthIfCtrl (mapped to same EthCtrl) except the requested one is in state ETH_MODE_ACTIVE */
            /* call driver API with ETH_MODE_DOWN */
            /* Call Eth_<vi>_<ai>_SetControllerMode() API */
            lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthSetControllerMode_pfct(
                                    lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                    ETH_MODE_DOWN );

            if( lStdRetVal_en == E_OK )
            {
                /* Update the EThIfCtrl state to UNINIT*/
                lDynamicPhysCtrl_pst->CtrlState_en = ETHIF_CTRL_STATE_UNINIT;

                /* Decrement ActiveCntr */
                lDynamicPhysCtrl_pst->ActiveCntr_u8--;

                /* E_OK will be returned */
            }
        }
        else
        {
            /* Other EthIfCtrl are using the EthCtrl */

            /* Decrement ActiveCntr */
            lDynamicPhysCtrl_pst->ActiveCntr_u8 = (lDynamicPhysCtrl_pst->ActiveCntr_u8 > 0U) ?
                    (lDynamicPhysCtrl_pst->ActiveCntr_u8 - 1U) : (lDynamicPhysCtrl_pst->ActiveCntr_u8);

            /* Return with E_OK */
            lStdRetVal_en = E_OK;
        }
    }

    /* -------------------------------------- */
    /* Request ACTIVE                         */
    /* -------------------------------------- */

    /* If EthIf_SetControllerMode() is called with ETH_MODE_ACTIVE */
    else
    {
        /* If the ActiveCntr is not at the uint8 upper boundary */
        /* Hint: This check is required because EthIf_Prv_SetPhysCtrlMode() can be called from EthIf_MainFunctionState() and EthIf_SetPhysCtrlMode() */
        /* It need to be always ensured that ActiveCntr_u8 has not reached uint8 max value before accepting the request from UL or from EthIf state machine */
        if( lDynamicPhysCtrl_pst->ActiveCntr_u8 < ETHIF_UINT8_MAX )
        {
            /* Check the state EthCtrl in context of EthIf */
            switch( lDynamicPhysCtrl_pst->CtrlState_en )
            {
                /* If the state of EthCtrl is ETHIF_CTRL_STATE_DOWN, it indicates EthIf_SetControllerMode( CtrlMode == ETH_MODE_ACTIVE ) is */
                /* not yet called to any of EthIfCtrl mapped to the current EthCtrl. And hence driver API needs to be called.  */
                case ETHIF_CTRL_STATE_DOWN:
                {
#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
                    EthIf_Prv_ResetPhysAddrFilterReferencesForEthHwCtrlIdx( lDynamicPhysCtrl_pst->EthCtrlIdx_cu8 );
#endif

                    /* Call Eth_<vi>_<ai>_SetControllerMode() API */
                    lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthSetControllerMode_pfct(
                                            lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                            ETH_MODE_ACTIVE );

                    if( E_OK == lStdRetVal_en )
                    {
                        /* Update the EThIfCtrl state to ACTIVE */
                        lDynamicPhysCtrl_pst->CtrlState_en = ETHIF_CTRL_STATE_ACTIVE;

                        /* Increment ActiveCntr */
                        lDynamicPhysCtrl_pst->ActiveCntr_u8++;

                        /* E_OK will be returned */
                    }
                }
                break;

                /* If the state of EthCtrl is ETHIF_CTRL_STATE_ACTIVE, it indicates EthIf_SetControllerMode( CtrlMode == ETH_MODE_ACTIVE ) is */
                /* already successfully called at to least one of EthIfCtrl mapped to the current EthCtrl. And hence there is no need to call driver API  */
                case ETHIF_CTRL_STATE_ACTIVE:
                {
                    /* Increment ActiveCntr */
                    lDynamicPhysCtrl_pst->ActiveCntr_u8++;

                    /* Return with E_OK */
                    lStdRetVal_en = E_OK;
                }
                break;

                /* Default state corresponds ETHIF_CTRL_STATE_UNINIT */
                /* API shall return E_NOT_OK as EthIf_Prv_ControllerInit() needs to be called before calling EthIf_SetControllerMode( CtrlMode == ETH_MODE_ACTIVE ) */
                default:
                {
                    /* E_NOT_OK will be returned */
                }
                break;
            }
        }
    }

    /* Exit critical section */
    SchM_Exit_EthIf_EthCtrlState();

    return lStdRetVal_en;
}

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
