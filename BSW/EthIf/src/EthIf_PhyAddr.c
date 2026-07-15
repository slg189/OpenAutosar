

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Cfg_SchM.h"
#include "rba_BswSrv.h"
#include "EthIf_Prv.h"


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_VAR_INIT_UNSPECIFIED
#include "EthIf_MemMap.h"

#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
/* This array is the virtual address filter */
/* Design Decisions: */
/* 1. This is a global static array because it is not desirable to allow direct access to it outside the file */
/* 2. In order to save memory, there is only one Array which is shared among the configured HW Eth controllers */
/* 3. Its size is configurable and MUST NOT be greater than capacity of physical address filters */
static EthIf_PhysAddrFilterRefCounterType_tst EthIf_PhysAddrFilterRefCounters_ast[ETHIF_SIZE_OF_CONFIGURED_FILTERS];
#endif

#define ETHIF_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "EthIf_MemMap.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
static Std_ReturnType EthIf_Prv_FindAndUpdatePhysAddrFilterReference  ( uint8 PhysCtrlIdx_u8,
                                                                        const uint8 * PhysAddrPtr_pcu8,
                                                                        uint32 * NewReferenceCounter_u32,
                                                                        Eth_FilterActionType ActionType_en );

static Std_ReturnType EthIf_Prv_AllocateNewPhysAddrFilterReference    ( uint8 PhysCtrlIdx_u8,
                                                                        const uint8 * PhysAddrPtr_pcu8 );
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"


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
 * ETHIF061: Obtains the physical source address used by the indexed controller
 * \par Service ID 0x08, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of the EthIfController within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param PhysAddrPtr   Physical source address (MAC address) in network byte order.
 * Please refer to [16] for the physical source address specification.
 *
 * \return  None
 *
 ***************************************************************************************************
 */
void EthIf_GetPhysAddr( uint8 CtrlIdx,
                        uint8 * PhysAddrPtr )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*       lDynamicPhysCtrl_pst;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_GETPHYSADDR, ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_GETPHYSADDR,
                                  ETHIF_E_INV_CTRL_IDX )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VOID( (PhysAddrPtr == NULL_PTR), ETHIF_SID_ETHIF_GETPHYSADDR, ETHIF_E_PARAM_POINTER )

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

    /* Call Eth_<vi>_<ai>_GetPhysAddr() API */
    lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthGetPhysAddr_pfct(
                     lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                     PhysAddrPtr );

    return;
}

/**
 ***************************************************************************************************
 * \module description
 * ETHIF00132: Sets the physical source address used by the indexed controller
 * \par Service ID 0x0D, Synchronous, Non Reentrant for the same CtrlIdx, reentrant for different
 *
 * Parameter In:
 * \param CtrlIdx       Index of the EthIfPhysController within the context of the Ethernet Interface
 *
 * \param PhysAddrPtr   Pointer to memory containing the physical source address (MAC
 *                      address) in network byte order.
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: failure}
 *
 ***************************************************************************************************
 */
/* Note: CtrlIdx is not the EthIfCtrlIdx index but this is EthIfPhysControllerIdx index */
Std_ReturnType EthIf_SetPhysAddr ( uint8 CtrlIdx,
                                    const uint8* PhysAddrPtr )
{
    /* Declare local variables */
    Std_ReturnType              lStdRetVal_en;
    EthIf_DynamicPhysCtrl_tst*  lDynamicPhysCtrl_pst;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT),
                                   ETHIF_SID_ETHIF_SETPHYSADDR, ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfPhysCtrl_cu8),
                                  ETHIF_SID_ETHIF_SETPHYSADDR,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET if PhysAddrPtr is a Null Pointer */
    ETHIF_REPORT_ERROR_RET_VALUE( (PhysAddrPtr == NULL_PTR), ETHIF_SID_ETHIF_SETPHYSADDR,
                                  ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Initialise local variables */
    lStdRetVal_en = E_NOT_OK;

    /* It is possible to change the source MAC address only when the driver is in DOWN or UNINIT state.
     * If the MAC address is modified while the network is in FULL_COMM, the Ethernet communication can be perturbated. */
    if((EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[CtrlIdx].CtrlState_en == ETHIF_CTRL_STATE_UNINIT) ||
            (EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[CtrlIdx].CtrlState_en == ETHIF_CTRL_STATE_DOWN))
    {
        /* Fetch the PhysCtrl structure from EthIfCtrl index */
        lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[CtrlIdx];

        /* Call Eth_<vi>_<ai>_SetPhysAddr() API */
        lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthSetPhysAddr_pfct(
                         lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                         PhysAddrPtr );

        lStdRetVal_en = E_OK;
    }

    return lStdRetVal_en;
}

/**
 ***************************************************************************************************
 * \module description
 * ETHIF139: Update the physical source address to/from the indexed controller filter.
 * If the Ethernet Controller is not capable to do the filtering, the software has to do this.
 * \par Service ID 0x0c, Synchronous, Non Reentrant for the same CtrlIdx, reentrant for different
 *
 * How it does:
 * I. If the requested action is ADD, then performs the following steps:
 *  1.  It tries to find and increment the retain count of an existing filter for the supplied physical controller index and MAC address by calling
 *              EthIf_Prv_FindAndUpdatePhysAddrFilterReference(..., ..., ETH_ADD_TO_FILTER)
 *  1.1 IF such filter could be found, then returns E_OK because the filter already exists.
 *  1.2 ELSE IF such filter could not be found, then continues with step 2
 *  2. Calls the Eth_UpdatePhysAddrFilter function to insert the filter into the HW buffer
 *  2.1 IF this operation fails (HW filter full, or other error), then returns E_NOT_OK
 *  2.2 ELSE allocates an alement into the virtual filter  by calling EthIf_Prv_AllocateNewPhysAddrFilterReference
 * If the above mentioned function succeeds, then returns E_OK, else E_NOT_OK
 *
 * II. If the requested action is REMOVE, then performs the following steps:
 *  1.  It tries to find and decrement the retain count of an existing filter for the supplied physical controller index and MAC address by calling
 *              EthIf_Prv_FindAndUpdatePhysAddrFilterReference(..., ..., ETH_REMOVE_FROM_FILTER)
 *  1.1 IF such element could not be found (does not exist in the virtual and physical filter), reports DET error(ETHIF_E_INV_PARAM) then returns E_NOT_OK
 *  1.2 ELSE continues with step 2
 *  2. IF the updated reference counter equal 0, then the element must be removed from the HW filter. In order to do this,
 *          the Eth_UpdatePhysAddrFilter function is called. The return value is given by this function as follows:
 *                          E_OK -  means that the filter could be removed from the HW registers
 *                          E_NOT_OK - means that the filter could not be found in the HW filter. In this case DET error(ETHIF_E_INV_PARAM) must be reported.
 * 3. ELSE IF reference counter is not 0, returns E_OK because the refererence count was previously decremented by the EthIf_Prv_FindAndUpdatePhysAddrFilterReference
 *          function.
 * \par Service ID 0x0C, Synchronous, Non Reentrant for the same CtrlIdx, reentrant for different
 *
 * Parameter In:
 * \param CtrlIdx      Index of the Ethernet controller within the context of the Ethernet Interface.
 * \param PhysAddrPtr  Pointer to memory containing the physical source address (MAC address) in network byte order.
 * \param Action       Add or remove the address from the Ethernet controllers filter.
 *
 * \return  E_OK:     filter was successfully changed
 *          E_NOT_OK: filter could not be changed
 *
 ***************************************************************************************************
 */
Std_ReturnType EthIf_UpdatePhysAddrFilter( uint8 CtrlIdx,
                                           const uint8 * PhysAddrPtr,
                                           Eth_FilterActionType Action )
{
#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
    /* Declare local variables */
    Std_ReturnType                  lStdRetVal_en;
    uint32                          lNewReferenceCounter_u32;
    EthIf_DynamicPhysCtrl_tst*      lDynamicPhysCtrl_pst;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_UPDATE_PHYSADDRFILTER,
                                   ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_UPDATE_PHYSADDRFILTER,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET : Development Error for Invalid Configuration Pointer */
    ETHIF_REPORT_ERROR_RET_VALUE( (PhysAddrPtr == NULL_PTR), ETHIF_SID_ETHIF_UPDATE_PHYSADDRFILTER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error for Physical Address given in argument not a Multicast address */
    ETHIF_REPORT_ERROR_RET_VALUE( ((PhysAddrPtr != NULL_PTR) &&
                                   !(((PhysAddrPtr[0] == 0x01U) && (PhysAddrPtr[1] == 0x00U) && (PhysAddrPtr[2] == 0x5EU)) ||   /* IPv4 Multicast MAC */
                                     ((PhysAddrPtr[0] == 0x33U) && (PhysAddrPtr[1] == 0x33U))                              ||   /* IPv6 Multicast MAC */
                                     ((PhysAddrPtr[0] == 0x01U) && (PhysAddrPtr[1] == 0x80U) && (PhysAddrPtr[2] == 0xC2U)))),   /* IEEE 802.1 Multicast MAC */
                                   ETHIF_SID_ETHIF_UPDATE_PHYSADDRFILTER, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Initialise local variables */
    lStdRetVal_en = E_NOT_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

    /* If EthDriver API is configured */
    if( lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthUpdatePhysAddrFilter_pfct != NULL_PTR )
    {
        /* Initialize variable to 0 to avoid compiler warning. */
        lNewReferenceCounter_u32 = 0U;

        /* Enter critical section: A new request to update physical filter address is being processed. Any further request needs to be blocked till this request is completed. */
        /*                         This is to avoid the same PhyAddr entry of the virtual filter table being used when update requests are received for two different physical controllers. */
        SchM_Enter_EthIf_UpdatePhysAddrFilter();

        lStdRetVal_en = EthIf_Prv_FindAndUpdatePhysAddrFilterReference( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8,
                                                                        PhysAddrPtr,
                                                                        &lNewReferenceCounter_u32,
                                                                        Action);

        /* Switch based on provided action : ADD to filter or REMOVE from filter */
        switch( Action )
        {
            case ETH_ADD_TO_FILTER:
            {
                /* The provided physical address is added into the Eth Driver only if the entry could not be found in the virtual filter */
                if( lStdRetVal_en == E_NOT_OK )
                {
                    /* Call Eth_<vi>_<ai>_UpdatePhysAddrFilter() API */
                    lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthUpdatePhysAddrFilter_pfct(
                                            lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                            PhysAddrPtr,
                                            Action );

                    if( lStdRetVal_en == E_OK )
                    {
                        lStdRetVal_en = EthIf_Prv_AllocateNewPhysAddrFilterReference( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8,
                                                                                      PhysAddrPtr );
                    }
                    else
                    {
                        /* Nothing to do because lStdRetVal_en has already been set to E_NOT_OK */
                    }

                }
                else
                {
                    /* Nothing to do because the address already exists in the filter and the counter for the specified physical address is already incremented */
                }
            }
            break;

            case ETH_REMOVE_FROM_FILTER:
            {
                /* The actual removal from the HW filter takes place when the reference counter becomes 0 after several deallocation actions */
                if( lStdRetVal_en == E_OK )
                {
                    if( lNewReferenceCounter_u32 == 0U )
                    {
                        /* Call Eth_<vi>_<ai>_UpdatePhysAddrFilter() API */
                        lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthUpdatePhysAddrFilter_pfct(
                                                lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                                PhysAddrPtr,
                                                Action );

                        if( lStdRetVal_en == E_NOT_OK )
                        {
                            /* If in the rare possibility, the driver layer returns E_NOT_OK whenever a physical filter is attempted to be removed, the following error handling is done. */
                            /* The NewReferenceCounter_u32 shall be incremented to one for the PhyAddr. Since it is already decremented to zero for the corresponding PhyAddr, */
                            /* a new physAddrfilter reference shall be added with the existing PhyAddr. ( The old one is still there but the reference counter is 0 and it will be overwritten */
                            /* whenever a new add filter request is received. By doing this, when EthIf retries to remove the PhysAddrfilter, the reference counter is one and the driver layer will be triggered again. */
                            (void)EthIf_Prv_AllocateNewPhysAddrFilterReference( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8,
                                                                                PhysAddrPtr );
                        }
                    }
                    else
                    {
                        /* Nothing to do because the reference counter was already decremented */
                    }
                }
                else
                {
                    /* Remove has been called to an already removed/non-existing physical address. */
                    /* Return E_OK to indicate upper layer that the physical filter is not present anymore. */
                    lStdRetVal_en = E_OK;

                }
            }
            break;

            default:
            {
                /* Invalid action is given */
                lStdRetVal_en = E_NOT_OK;
            }
            break;
        }

        /* Exit critical section */
        SchM_Exit_EthIf_UpdatePhysAddrFilter();
    }
    else
    {
        /* Report DET : Development Error: Underlying EthDriver do not have required API configured */
        ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_UPDATE_PHYSADDRFILTER, ETHIF_E_INV_CTRL_IDX )
    }

    return lStdRetVal_en;
#else
    (void)CtrlIdx;
    (void)PhysAddrPtr;
    (void)Action;
    return E_NOT_OK;
#endif
}


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

/**
 ******************************************************************************************************************************************
 * This function must be called by the EthIf_Prv_SetControllerMode function, before setting the eth controller to ETHCTRL_MODE_ACTIVE.
 *
 * What it does:
 *  It clears the physical address filters for the specified physical eth controller index.
 *
 * Design Decisions:
 * 1. Critical sections are not required because the virtual addr filters are not called for various ETH controllers at the same time.
 *
 * Parameter In:
 * \param PhysCtrlIdx_u8    Index of the Ethernet controller within the context of the Ethernet Interface
 *
 * \return                  None
 *
 ******************************************************************************************************************************************
 **/
#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
void EthIf_Prv_ResetPhysAddrFilterReferencesForEthHwCtrlIdx( uint8 PhysCtrlIdx_u8 )
{
    /* Declare local variables */
    uint32      lLoopIdx_u32;
    uint8       lPhysicalAddressIdx_u8;

    /* Enter critical section: When a request to reset PhysAddr Filter reference is received, no update of physical address filter should occur. */
    /*                         Currently such use case is not seen. But critical section added to avoid future multicore problems.               */
    SchM_Enter_EthIf_UpdatePhysAddrFilter();

    /* Interates over the virtual filter entries and deallocates all the physical addresses for the specified physical controller index(PhysCtrlIdx_u8) */
    for( lLoopIdx_u32 = 0U; lLoopIdx_u32 < ETHIF_SIZE_OF_CONFIGURED_FILTERS; lLoopIdx_u32++ )
    {
        if( EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].HardwareControllerIndex_u8 == PhysCtrlIdx_u8 )
        {
            /* Deallocates the current entry of the PhysAddrFilter */
            EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].ReferenceCounter_u32 = 0U;    /* Counter of 0 means deallocated */
            EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].HardwareControllerIndex_u8 = 0xFFU;
            for( lPhysicalAddressIdx_u8 = 0U; lPhysicalAddressIdx_u8 < 6U; lPhysicalAddressIdx_u8++ )
            {
                EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].PhysicalAddress_au8[lPhysicalAddressIdx_u8] = 0U;
            }
        }
    }

    /* Exit critical section. */
    SchM_Exit_EthIf_UpdatePhysAddrFilter();

    return;
}
#endif


/**
 ******************************************************************************************************************************************
 * This function must be called by the ETHCTRL_STATE_INIT.
 *
 * What it does:
 * It clears the physical address filters for all configured physical controllers.
 *
 * Design Decisions:
 * 1. Critical sections are not required because the virtual addr filters are not called for various ETH controllers at the same time.
 *
 * \return  None
 *
 ******************************************************************************************************************************************
 **/
#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
void EthIf_Prv_ResetPhysAddrFilterReferences( void )
{
    /* Declare local variables */
    uint32      lLoopIdx_u32;
    uint8       lPhysicalAddressIdx_u8;

    /* Enter critical section: When a request to reset PhysAddr Filter reference is received, no update of physical address filter should occur. */
    /*                         Currently such use case is not seen. But critical section added to avoid future multicore problems.               */
    SchM_Enter_EthIf_UpdatePhysAddrFilter();

    /* Interates over the virtual filter entries and deallocates all physical addresses  */
    for (lLoopIdx_u32 = 0U; lLoopIdx_u32 < ETHIF_SIZE_OF_CONFIGURED_FILTERS; lLoopIdx_u32++)
    {
        /* Deallocates the current entry of the PhysAddrFilter */
        EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].ReferenceCounter_u32 = 0U;    /* Counter of 0 means deallocated */
        EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].HardwareControllerIndex_u8 = 0xFFU;
        for( lPhysicalAddressIdx_u8 = 0U; lPhysicalAddressIdx_u8 < 6U; lPhysicalAddressIdx_u8++ )
        {
            EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].PhysicalAddress_au8[lPhysicalAddressIdx_u8] = 0U;
        }
    }

    /* Exit critical section. */
    SchM_Exit_EthIf_UpdatePhysAddrFilter();

    return;
}
#endif


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/*
 *****************************************************************************************************************************************************************************
 * EthIf_Prv_FindAndUpdatePhysAddrFilterReference: This static function manages the reference counter of the existing filters as follows:
 *          Increments the reference counter each time upper layers call the EthIf_UpdatePhysAddrFilter and the MAC filter is already set for the provided physical ctrl index,
 *          Decrements the reference counters each time UL call the EthIf_UpdatePhysAddrFilter and the MAC filter is already set for the provided physical ctrl index,
 *
 *  What it does:
 *  1. Iterates over the EthIf_PhysAddrFilterRefCounters_ast` elements and tries to find a valid filter element for the supplied MAC address and Eth physical controller index.
 *     A valid element can be found only if all of the following conditions are met:
 *          - element.HardwareControllerIndex_u8 ==  PhysCtrlIdx_u8   AND
 *          - element.ReferenceCounter_u32 > 0 (that is: the filter is valid and added in the driver`s filter) AND
 *          - element.PhysicalAddress_au8 matches the provided as argument physical address (PhysAddrPtr_pcu8)
 *
 *  1.1 IF such element has NOT been found, then RETURNS E_NOT_OK because the filter is not present in the eth driver`s addresses filter
 *  1.2 IF such element has been found, then continues with step 2
 *
 *   2. Checks the desied ACTION (which can be ADD or REMOVE)
 *  2.1 IF the required action is ADD to filter, then increments the reference counter
 *   2.2 ELSE IF the required action is REMOVE from filter, then decrements the reference counter
 *  3. updates the output parameter `NewReferenceCounter_u32` which represents the reference counter for the current MAC filter reference
 *
 * Design Decisions:
 * 1. This function is static because it is not necessary to expose it outside this unit/file.
 * 2. Critical sections are not required because the virtual addr filters are not called for various ETH controllers at the same time.
 *
 * Parameter In:
 * \param PhysCtrlIdx_u8            Index of the Ethernet controller within the context of the Ethernet Interface
 * \param PhysAddrPtr_pcu8          Physical source address (MAC address) in network byte order. Please refer to [16] for the physical source address specification.
 * \param ActionType_en             Add or remove the address from the filter.
 *
 * Parameter Out:
 * \param NewReferenceCounter_u32   Current reference counter of the modified entry within the virtual filter
 *
 * \return  E_OK: The pair {PhysCtrlIdx_u8, PhysAddrPtr_pcu8} could be found in the virtual address filter and the pair is valid (has the reference counter greater than 0)
 *                E_OK is also returned when ActionType_en is invalid.
 *          E_NOT_OK: The pair {PhysCtrlIdx_u8, PhysAddrPtr_pcu8} could NOT be found in the virtual address filter. In this case it must be allocated. This is not a DET error.
 *
 *****************************************************************************************************************************************************************************
 */
#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
static Std_ReturnType EthIf_Prv_FindAndUpdatePhysAddrFilterReference( uint8 PhysCtrlIdx_u8,
                                                                      const uint8 * PhysAddrPtr_pcu8,
                                                                      uint32 * NewReferenceCounter_u32,
                                                                      Eth_FilterActionType ActionType_en )
{
    /* Declare local variables */
    Std_ReturnType                              lStdRetVal_en;
    uint8*                                      lLocalPhysAddrPointer_pu8;
    uint8                                       lLocalCtrlIdx_pu8;
    uint32                                      lLocalReferenceCounter_u32;
    uint32                                      lLoopIdx_u32;
    EthIf_PhysAddrFilterRefCounterType_tst*     lLocalPhysAddrFilterRefCounter_pst;

    /* Initialize local variables */
    lStdRetVal_en = E_OK;
    lLocalPhysAddrFilterRefCounter_pst = NULL_PTR;

    /* Iterates over the EthIf_PhysAddrFilterRefCounters_ast` elements and tries to find a valid filter element for the supplied MAC address and Eth physical controller index. */
    for( lLoopIdx_u32 = 0U; lLoopIdx_u32 < ETHIF_SIZE_OF_CONFIGURED_FILTERS; lLoopIdx_u32++ )
    {
        lLocalPhysAddrPointer_pu8 = (uint8*) (EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].PhysicalAddress_au8);
        lLocalCtrlIdx_pu8 = EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].HardwareControllerIndex_u8;
        lLocalReferenceCounter_u32 = EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32].ReferenceCounter_u32;

        /*  A valid entry can be found only if the following pair {PhysCtrlIdx_u8, PhysAddrPtr_pcu8} exists in the virtual filter and its reference counter is greater than 0 */
        if( (lLocalCtrlIdx_pu8 == PhysCtrlIdx_u8) && (lLocalReferenceCounter_u32 > 0U) )
        {
            if( rba_BswSrv_MemCompare8( lLocalPhysAddrPointer_pu8, PhysAddrPtr_pcu8, 6U ) == 0U )
            {
                lLocalPhysAddrFilterRefCounter_pst = &(EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32]);
                break;
            }
            else { /* do nothing */ }
        }
        else { /* do nothing */ }
    }

    /* A valid entry could be found in the virtual address filter */
    if( lLocalPhysAddrFilterRefCounter_pst != NULL_PTR )
    {
        /* Increments the reference counter because this entry already exists in the virtual address filter */
        if( ActionType_en == ETH_ADD_TO_FILTER)
        {
            lLocalPhysAddrFilterRefCounter_pst->ReferenceCounter_u32++;
        }
        /* Decrements the reference counter  */
        else if( ActionType_en == ETH_REMOVE_FROM_FILTER )
        {
            lLocalPhysAddrFilterRefCounter_pst->ReferenceCounter_u32--;
        }
        else { /* do nothing */ }

        /* Store the current reference counter of the modified entry into out parameter: lNewReferenceCounter_u32 */
        *NewReferenceCounter_u32 = lLocalPhysAddrFilterRefCounter_pst->ReferenceCounter_u32;
    }
    /* A valid entry could not be found in the virtual address filter */
    else
    {
        lStdRetVal_en = E_NOT_OK;
    }

    return lStdRetVal_en;
}
#endif


/**
 *****************************************************************************************************************************************
  * EthIf_AllocateNewPhysAddrFilterReference: allocates filters in the EthIf_PhysAddrFilterRefCounters_ast array.
  *    This function is called by the EthIf_UpdatePhysAddrFilter, only if the requested action is ADD to filter an the provided MAC
  *         addres could not be found in the Physical address filter.
  *
  * What it does:
  *  1. Iterates over the EthIf_PhysAddrFilterRefCounters_ast` elements and tries to find an empty/not used element.
  *  Note: An empty/not used element is any element having the ReferenceCounter_u32 set to 0
  *
  *  1.1. IF such element could not be found, returns E_NOT_OK because the filter is full.
  *  1.2 ELSE continues with step 2
  *  2. Sets the the following fields:
  *     - element.ReferenceCounter_u32 to 1
  *     - element.HardwareControllerIndex_u8 to  PhysCtrlIdx_u8
  *     - copies the provided physical address (PhysAddrPtr_pcu8)  into element.PhysicalAddress_au8
  *
  * Design Decisions:
  * 1. This function is static because it is not necessary to expose it outside this unit/file.
  * 2. Critical sections are not required because the virtual addr filters are not called for various ETH controllers at the same time.
  *
  * Parameter In:
  * \param PhysCtrlIdx_u8       Index of the Ethernet controller within the context of the Ethernet Interface
  * \param PhysAddrPtr_pcu8     Physical source address (MAC address) in network byte order. Please refer to [16] for the physical source address specification.
  *
  * \return  E_OK: An empty filter could be found, and the provided PhysAddrPtr_pcu8 is stored
  *          E_NOT_OK: The virtual filter is full, therefore the provided PhysAddrPtr_pcu8 could not be stored
  *
  ******************************************************************************************************************************************
  **/
#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
static Std_ReturnType EthIf_Prv_AllocateNewPhysAddrFilterReference( uint8 PhysCtrlIdx_u8,
                                                                    const uint8 * PhysAddrPtr_pcu8 )
{
    /* Declare local variables */
    Std_ReturnType                              lStdRetVal_en;
    uint32                                      lLoopIdx_u32;
    EthIf_PhysAddrFilterRefCounterType_tst*     lLocalPhysAddrFilterRefCounter_pst;

    /* Initialize local variables */
    lStdRetVal_en = E_OK;

    /* Iterates over the EthIf_PhysAddrFilterRefCounters_ast` elements and tries to find an empty/not used entry. An empty/not used entry is any element having the ReferenceCounter_u32 set to 0 */
    for( lLoopIdx_u32 = 0U; lLoopIdx_u32 < ETHIF_SIZE_OF_CONFIGURED_FILTERS; lLoopIdx_u32++ )
    {
        lLocalPhysAddrFilterRefCounter_pst = &(EthIf_PhysAddrFilterRefCounters_ast[lLoopIdx_u32]);

        /* If an empty/not used entry could be found, then: */
        if( lLocalPhysAddrFilterRefCounter_pst->ReferenceCounter_u32 == 0U )
        {
            /* marks the current entry as valid by setting the reference counter to 1*/
            lLocalPhysAddrFilterRefCounter_pst->ReferenceCounter_u32 = 1U;

            /* stores the physical Eth Controller index into the current entry */
            lLocalPhysAddrFilterRefCounter_pst->HardwareControllerIndex_u8 = PhysCtrlIdx_u8;

            /* copies the provided physical address into the current entry */
            rba_BswSrv_MemCopy8(lLocalPhysAddrFilterRefCounter_pst->PhysicalAddress_au8, PhysAddrPtr_pcu8, 6U);

            break;
        }
    }

    /* An invalid/empty slot could not be found */
    if( lLoopIdx_u32 == ETHIF_SIZE_OF_CONFIGURED_FILTERS )
    {
        lStdRetVal_en = E_NOT_OK;
    }

    return lStdRetVal_en;
}
#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
