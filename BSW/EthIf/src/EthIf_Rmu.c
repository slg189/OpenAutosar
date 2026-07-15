

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Prv.h"

#if( ETHIF_RMU_SUPPORT == STD_ON )

/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"


Std_ReturnType EthIf_PhysCtrlInit( uint8 PhysCtrlIdx )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_PHYSCTRLINIT,
                                  ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: PhysCtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PhysCtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfPhysCtrl_cu8),
                                  ETHIF_SID_ETHIF_PHYSCTRLINIT,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK)

    /* Request physical controller for initialization */
    lStdRetVal_en = EthIf_Prv_PhysCtrlInit( PhysCtrlIdx );

    return lStdRetVal_en;
}



Std_ReturnType EthIf_SetPhysCtrlMode( uint8 PhysCtrlIdx,
                                      Eth_ModeType CtrlMode )
{
    /* Declare local variables */
    Std_ReturnType                      lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SETPHYSCTRLMODE,
                                  ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: PhysCtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PhysCtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfPhysCtrl_cu8),
                                  ETHIF_SID_ETHIF_SETPHYSCTRLMODE,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK)

    /* Request the physical controller mode to ACTIVE/DOWN and increment/decrement ActiveCntr */
    lStdRetVal_en = EthIf_Prv_SetPhysCtrlMode( PhysCtrlIdx,
                                               CtrlMode );

    return lStdRetVal_en;
}

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* #if ( ETHIF_RMU_SUPPORT == STD_ON ) */
#endif /* ETHIF_CONFIGURED */
