

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#if( ETHIF_ETHIFCTRL_SWITCH_REFERENCED == STD_ON || ETHIF_ETHIFCTRL_PORTGROUP_CTRL_REFERENCED == STD_ON || ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
#include "EthSwt.h"
#endif

#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#if(ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )

#define ETHIF_START_SEC_VAR_CLEARED_32
#include "EthIf_MemMap.h"

/* Global variable for EthIf Switch Ports active timer */
uint32 EthIf_PortActiveTime_u32 = 0UL;

#define ETHIF_STOP_SEC_VAR_CLEARED_32
#include "EthIf_MemMap.h"

#endif

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
 * ETHIF91103: Request to start all configured ports
 * \par Service ID 0x07, Asynchronous, Non Reentrant
 *
 * \return  Std_ReturnType {E_OK: Request processed successfully, E_NOT_OK: Request NOT processed successfully}
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_StartAllPorts( void )
{
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    /* Declare local variables */
    Std_ReturnType      lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_STARTALLPORTS,
                                  ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Initialise local variable */
    lStdRetVal_en = E_NOT_OK;

    /* Check whether the EthIfPortStartupActiveTime is zero. If not, EthIf_StartAllPorts() was called previously. */
    /* Continue only if the timer is zero */
    if( EthIf_PortActiveTime_u32 == 0U )
    {
        /* Start the timer with the configured value for all the ports. */
        EthIf_PortActiveTime_u32 = ETHIF_PORT_STARTUP_ACTIVE_TIME;

        lStdRetVal_en = E_OK;
    }
    else
    {
        /* If the timer is non-zero, it means EthIf_StartAllPorts was called previously */
        /* Timer is not reset, reset is done in EthIf_MainFunctionState() after timer expires */
    }

    return lStdRetVal_en;

#else
    return E_NOT_OK;
#endif
}


/**
 ***************************************************************************************************
 * \module description
 * ETHIF91102: Request a mode for the EthIfSwtPortGroup
 * The call shall be forwarded to EthSwt by calling EthSwt_SetSwitchPortMode, for all EthSwtPorts referenced by the port group.
 * \par Service ID 0x06, Asynchronous, Non Reentrant
 *
 * Parameter In:
 * \param PortGroupIdx      Index of the port group within the context of the Ethernet Interface
 * \param PortMode          {ETHTRCV_MODE_DOWN: disable the port group, ETHTRCV_MODE_ACTIVE: enable the port group}
 *
 * \return  Std_ReturnType {E_OK: Request processed successfully, E_NOT_OK: Request NOT processed successfully}
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_SwitchPortGroupRequestMode( EthIf_SwitchPortGroupIdxType PortGroupIdx,
                                                 EthTrcv_ModeType PortMode )
{
#if( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON )
    /* Declare local variables */
    EthIf_DynamicStandaloneSwtPortGroup_tst*    lDynamicStandaloneSwtPortGroup_pst;


    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_SWITCHPORTGROUPREQUESTMODE,
                                   ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: PortGroupIdx is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PortGroupIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfSwtPortGroup_cu8), ETHIF_SID_ETHIF_SWITCHPORTGROUPREQUESTMODE,
                                  ETHIF_E_INV_PORT_GROUP_IDX, E_NOT_OK )

    /* Report DET : Development Error: PortGroupIdx addresses a port group referenced by an EthIfController */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[PortGroupIdx].Semantic_cen != ETHIF_SWTPORTGROUP_UNREFERENCED),
                                  ETHIF_SID_ETHIF_SWITCHPORTGROUPREQUESTMODE,
                                  ETHIF_E_INV_PORT_GROUP_IDX, E_NOT_OK )

    /* Fetch the Dynamic Standalone Swt PG structure */
    lDynamicStandaloneSwtPortGroup_pst = EthIf_CfgPtr_pco->EthIf_StaticSwtPortGroupTable_cpcst[PortGroupIdx].RefStandaloneSwtPortGroupTableEntry_cpst;

    /* Store the requested PG requested mode */
    lDynamicStandaloneSwtPortGroup_pst->ReqMode_en = PortMode;

    return E_OK;

#else /* ( ETHIF_PORTGROUP_PNC_CONFIGURED == STD_ON ) */
    (void)PortGroupIdx;
    (void)PortMode;
    return E_NOT_OK;
#endif
}

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
