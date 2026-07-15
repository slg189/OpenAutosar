

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
 * Interface functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE
#include "EthIf_MemMap.h"

/*****************************************************************************************************/
/* \module description                                                                              */
/* ETHIF067: Provides access to a transmit buffer of the specified Ethernet controller
 * \par Service ID 0x09, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CtrlIdx       Index of the Ethernet controller within the context of the Ethernet Interface
 * \param FrameType     Ethernet Frame Type (EtherType)
 * \param Priority      Priority value which shall be used for the 3-bit PCP field of the VLAN tag
 *
 * Parameter InOut:
 * \param LenBytePtr    In: Desired length in bytes,
 *                      Out: Granted length in bytes
 *
 * Parameter Out:
 * \param BufIdxPtr     Index to the granted buffer resource. To be used for subsequent requests
 * \param BufPtr        Pointer to the granted buffer
 *
 * \return  BufReq_ReturnType {BUFREQ_OK: success; BUFREQ_E_NOT_OK: development error detected; BUFREQ_E_BUSY: all buffers in use}
 *
 ***************************************************************************************************
 */
/* HIS METRIC PATH VIOLATION IN EthIf_ProvideTxBuffer: The number of path is increased due to the consecutive if blocks which are required for the execution of various DET checks. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
BufReq_ReturnType EthIf_ProvideTxBuffer( uint8 CtrlIdx,
                                         Eth_FrameType FrameType,
                                         uint8 Priority,
                                         uint8 * BufIdxPtr,
                                         uint8 ** BufPtr,
                                         uint16 * LenBytePtr )
{
    /* Declare local variables */
    uint16                              lVlanId_u16;
    EthIf_DynamicPhysCtrl_tst*          lDynamicPhysCtrl_pst;
    BufReq_ReturnType                   lBufReqRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_PROVIDETXBUFFER,
                                  ETHIF_E_NOT_INITIALIZED, BUFREQ_E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_PROVIDETXBUFFER,
                                  ETHIF_E_INV_CTRL_IDX, BUFREQ_E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (BufIdxPtr ==  NULL_PTR), ETHIF_SID_ETHIF_PROVIDETXBUFFER, ETHIF_E_PARAM_POINTER, BUFREQ_E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (BufPtr == NULL_PTR), ETHIF_SID_ETHIF_PROVIDETXBUFFER, ETHIF_E_PARAM_POINTER, BUFREQ_E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (LenBytePtr == NULL_PTR), ETHIF_SID_ETHIF_PROVIDETXBUFFER, ETHIF_E_PARAM_POINTER, BUFREQ_E_NOT_OK )

    /* Report DET : Development Error: Frame priority parameter (Priority) is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (Priority > ETHIF_FRAMEPRIO_MAXVAL), ETHIF_SID_ETHIF_PROVIDETXBUFFER, ETHIF_E_INV_PARAM, BUFREQ_E_NOT_OK )

    /* Initialise local variables */
    lBufReqRetVal_en = BUFREQ_E_NOT_OK;

    /* If buffer length requested is greater than MTU, set buffer length to MTU configured and return with BUFREQ_E_OVFL */
    if( (*LenBytePtr) > EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].MtuSize_cu16 )
    {
        *LenBytePtr = EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].MtuSize_cu16;
        lBufReqRetVal_en = BUFREQ_E_OVFL;
    }
    else
    {
        /* Fetch the PhysCtrl structure from EthIfCtrl index */
        lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

        /* Enter critical section: EthCtrl is being used based on his state, in the meantime no change of EthCtrl state shall be done */
        SchM_Enter_EthIf_EthCtrlState();

        /* If the EthIfCtrl state is ACTIVE and mapped EthCtrl is ACTIVE */
        if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE) &&
            (lDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_ACTIVE) )
        {
            /* If requested EthIfCtrl is Vlan configured, increase the buffer length by 4 */
            if( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].VlanId_cu16 != 0xFFFFU )
            {
                *LenBytePtr = ( (*LenBytePtr) + ETHIF_VLANTAG_LENGTH );
            }

            /* Request buffer from Eth Driver */
            /* Call Eth_<vi>_<ai>_ProvideTxBuffer() API */
            lBufReqRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthProvideTxBuffer_pfct(
                                                    lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                                    BufIdxPtr,
                                                    BufPtr,
                                                    LenBytePtr );

        }

        /* Exit critical section */
        SchM_Exit_EthIf_EthCtrlState();

        /* EthDrv provided requested buffer length */
        if( lBufReqRetVal_en == BUFREQ_OK )
        {
            /* Check if Vlan is configured for the requested EthIfCtrl */
            if( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].VlanId_cu16 != 0xFFFFU )
            {
                /* Read out VLAN id from the configuration */
                lVlanId_u16 = EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].VlanId_cu16;

                /* Write PCP (Priority) to header --> upper 3 bits in ETHIF_VLANTAG_BYTE1 */
                ( *( ( *BufPtr ) + ETHIF_VLANTAG_BYTE1 ) ) = ( Priority << 5U );

                /* Write VLANid into header */
                /* Don't overwrite upper 4 bits in ETHIF_VLANTAG_BYTE1 because they are PCP(priority of the VLAN frame) and CFI */
                ( *( ( *BufPtr ) + ETHIF_VLANTAG_BYTE1 ) ) = ( * ( ( * BufPtr ) + ETHIF_VLANTAG_BYTE1 ) ) | ( (uint8)( lVlanId_u16 >> 8U ) );
                ( *( ( *BufPtr ) + ETHIF_VLANTAG_BYTE2 ) ) = ( (uint8)( lVlanId_u16 & 0x00FFU ) );

                /* Write EthType */
                ( *( ( *BufPtr ) + ETHIF_ETHTYPE_IN_VLAN_BYTE1 ) ) = ( (uint8)( FrameType >> 8U ) );
                ( *( ( *BufPtr ) + ETHIF_ETHTYPE_IN_VLAN_BYTE2 ) ) = ( (uint8)( FrameType & 0x00FFU ) );

                /* Shift pointer to the beginning of IP payload */
                *BufPtr = ( *BufPtr + ETHIF_VLANTAG_LENGTH );

                /* Update length byte pointer with the buffer length requested by upper layer */
                *LenBytePtr = ( *LenBytePtr - ETHIF_VLANTAG_LENGTH );

            }

            /* If buffer length to be provided to upper layer is greater than MTU, set it to MTU configured */
            if( (*LenBytePtr) > EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].MtuSize_cu16 )
            {
                /* Update the buffer length to MTU + EthIf header length */
                *LenBytePtr = EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].MtuSize_cu16;
            }
        }
        else if( (lBufReqRetVal_en == BUFREQ_E_OVFL) &&                                                     /* If Eth_ProvideTxBuffer() returned BUFREQ_E_OVFL and */
                 (EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].VlanId_cu16 != 0xFFFFU) )     /* requested EthIfCtrl is Vlan configured */
        {
            if( (*LenBytePtr) > ETHIF_VLANTAG_LENGTH )                                         /* Check whether the buffer length which EthDrv can provide is */
            {                                                                                  /* at least a byte more than the required for Vlan header */
                *LenBytePtr = ( (*LenBytePtr) - ETHIF_VLANTAG_LENGTH );
            }
            else
            {
                *LenBytePtr = 0U;
            }
        }
        else
        {
            /* Do Nothing */
        }
    }

    return lBufReqRetVal_en;
}


/**
 ***************************************************************************************************
 * \module description
 * ETHIF075: Triggers transmission of a previously filled transmit buffer
 * \par Service ID 0x0a, Synchronous, Non Reentrant
 *
 * Parameter In:
 * \param CtrlIdx           Index of the Ethernet controller within the context of the Ethernet Interface
 * \param FrameType         Ethernet frame type
 * \param TxConfirmation    Activates transmission confirmation
 * \param PhysAddrPtr       Physical target address (MAC address) in network byte order
 *
 * Parameter InOut:
 * \param LenByte           Data length in byte
 *
 * Parameter Out:
 * \param BufIdx            Index of the buffer resource
 *
 * \return  Std_ReturnType {E_OK: success;E_NOT_OK: transmission failed}
 *
 ***************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: Pointer to variable is defined in AUTOSAR for PhysAddrPtr. Deviation is not critical as it has no functional impact. */
Std_ReturnType EthIf_Transmit( uint8 CtrlIdx,
                               uint8 BufIdx,
                               Eth_FrameType FrameType,
                               boolean TxConfirmation,
                               uint16 LenByte,
                               uint8 * PhysAddrPtr )
{
    /* Declare local variables */
    Std_ReturnType                  lStdRetVal_en;
    EthIf_DynamicPhysCtrl_tst*      lDynamicPhysCtrl_pst;
    Eth_FrameType                   lFrameType_u16;
    boolean                         lBufReleaseRequested_b;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_TRANSMIT,
                                  ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_TRANSMIT,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PhysAddrPtr == NULL_PTR), ETHIF_SID_ETHIF_TRANSMIT, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Initialise local variables */
    lFrameType_u16 = FrameType;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

    /* Data length 0 indicates upper layer is trying to release the buffer */
    lBufReleaseRequested_b = (LenByte == 0U) ? (TRUE) : (FALSE);

    /* Enter critical section: EthCtrl is being used based on his state, in the meantime no change of EthCtrl state shall be done */
    SchM_Enter_EthIf_EthCtrlState();

    if( lBufReleaseRequested_b == FALSE )
    {
        /* If the EthIfCtrl state is ACTIVE and mapped EthCtrl is ACTIVE */
        /* and the Data length is within configured MTU */
        if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE) &&
            (lDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_ACTIVE) &&
            (LenByte <= EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].MtuSize_cu16) )
        {
            /* If VALN is configured for the requested EthIf controller increase the data length by 4 bytes */
            if( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].VlanId_cu16 != 0xFFFFU )
            {
                LenByte = LenByte + ETHIF_VLANTAG_LENGTH;
                lFrameType_u16 = ETHIF_FRAMETYP_VLAN;
            }
        }
        else
        {
            /* If data length greater than MTU or if controller state is not ETH_MODE_ACTIVE, release the buffer */
            LenByte = 0U;
        }
    }

    /* Call Eth_<vi>_<ai>_Transmit() API */
    lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthTransmit_pfct(
                                  lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                  BufIdx,
                                  lFrameType_u16,
                                  TxConfirmation,
                                  LenByte,
                                  PhysAddrPtr );

    /* If upper layer didn't request for releasing buffer and if EthIf is releasing buffer */
    /* (Because of not meeting required condition for Tx), E_NOT_OK shall be returned to the upper layer */
    lStdRetVal_en = ((FALSE == lBufReleaseRequested_b) && (0U == LenByte)) ? (E_NOT_OK) : (lStdRetVal_en);

    /* Exit critical section */
    SchM_Exit_EthIf_EthCtrlState();

    return lStdRetVal_en;
}


/**
 ***********************************************************************************************************************
 * \module description
 * EthIf_OverrideTxBufSrcAddr calls the driver function to override the source MAC address
 *
 * Parameter In:
 * \param CtrlIdx           Index of the Ethernet controller within the context of the Ethernet Interface
 * \param BufIdx            Index of the buffer resource
 * \param PhysAddrPtr       Physical target address (MAC address) in network byte order
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: failure}
 *
 ***********************************************************************************************************************
 */
#if( ETHIF_ENABLE_OVERRIDE_SRCMAC_SUPPORT == STD_ON)
Std_ReturnType EthIf_OverrideTxBufSrcAddr( uint8 CtrlIdx,
                                           uint8 BufIdx,
                                           const uint8 * PhysAddrPtr )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*          lDynamicPhysCtrl_pst;
    Std_ReturnType                      lStdRetVal_en;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( ( EthIf_InitStatus_en != ETHCTRL_STATE_INIT ), ETHIF_SID_ETHIF_OVERRIDEBUFTXSRCADDR,
                                    ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                     ETHIF_SID_ETHIF_OVERRIDEBUFTXSRCADDR,
                                     ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( ( NULL_PTR == PhysAddrPtr), ETHIF_SID_ETHIF_OVERRIDEBUFTXSRCADDR, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Initialize the local variable*/
    lStdRetVal_en = E_NOT_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx].RefPhysCtrlTableIdx_cu8];

    /* If EthDriver API is configured */
    if( lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthOverrideTxBufSrcAddr_pfct != NULL_PTR )
    {
        /* Enter critical section: EthCtrl is being used based on his state, in the meantime no change of EthCtrl state shall be done */
        SchM_Enter_EthIf_EthCtrlState();

        /* If the EthIfCtrl state is ACTIVE and mapped EthCtrl is ACTIVE */
        if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE) &&
            (lDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_ACTIVE) )
        {
            /* Call the driver API to override the source MAC address */
            /* Call Eth_<vi>_<ai>_OverrideTxBufSrcAddr() API */
            lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthOverrideTxBufSrcAddr_pfct(
                                                      lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                                      BufIdx,
                                                      PhysAddrPtr);
        }

        /* Exit critical section */
        SchM_Exit_EthIf_EthCtrlState();
    }
    else
    {
        /* Report DET : Development Error: Underlying EthDriver do not have required API configured */
        ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_OVERRIDEBUFTXSRCADDR, ETHIF_E_INV_CTRL_IDX )
    }

    return lStdRetVal_en;
}

#endif

#define ETHIF_STOP_SEC_CODE
#include "EthIf_MemMap.h"

#endif /* ETHIF_CONFIGURED */
