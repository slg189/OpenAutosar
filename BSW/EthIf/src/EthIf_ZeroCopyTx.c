

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "EthIf.h"

#ifdef ETHIF_CONFIGURED

#include "EthIf_Prv.h"

#if( ETHIF_ZEROCOPY_TX_SUPPORT == STD_ON )

/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define ETHIF_START_SEC_CODE_FAST
#include "EthIf_MemMap.h"

/*****************************************************************************************************
 * \module description
 * Prefill the ZeroCopy header buffer with the Ethernet header fields
 * \par Service ID 0x57, Synchronous, Reentrant
 *
 * Parameter In:
 * \param CtrlIdx_u8        Index of the Ethernet controller within the context of the Ethernet Interface
 * \param FrameType_u16     Ethernet Frame Type (EtherType)
 * \param Priority_u8       Priority value which shall be used for the 3-bit PCP field of the VLAN tag
 * \param PhysAddrPtr_pcu8  Physical target address (MAC address) in network byte order
 * \param ZeroCopyArg_pcst  ZeroCopy argument containing the allocated headers and the pointer to the UL data to be transmitted
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: prefilling not possible}
 *
 ***************************************************************************************************
 */
/* HIS METRIC PATH VIOLATION IN EthIf_ZeroCopyFillHeader: The number of path is increased due to the consecutive if blocks which are required for the execution of various DET checks. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
Std_ReturnType EthIf_ZeroCopyFillHeader( uint8 CtrlIdx_u8,
                                         Eth_FrameType FrameType_u16,
                                         uint8 Priority_u8,
                                         const uint8 * PhysAddrPtr_pcu8,
                                         const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*          lDynamicPhysCtrl_pst;
    Std_ReturnType                      lStdRetVal_en;
    uint8                               lLoopIndex_u8;
    uint8                               LocalPhysAddr_au8[ETHIF_HWADDR_LEN];
    uint16                              lVlanId_u16;
    uint8*                              lBufferPtr_pu8;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER,
                                  ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx_u8 >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
            ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (PhysAddrPtr_pcu8 == NULL_PTR), ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pcst == NULL_PTR), ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast == NULL_PTR), ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET: At least the Reuse header need to be present so that EthIf header can be written */
    ETHIF_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 < 1U ), ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8 == NULL_PTR), ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET: The length of the header block need to be big enough so that ethernet header can be present */
    ETHIF_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartLen_u8 < ETHIF_ZEROCOPY_ETHERNET_HDR_LEN ), ETHIF_SID_ETHIF_ZEROCOPYFILLHEADER, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Initialize local variable */
    lStdRetVal_en = E_NOT_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx_u8].RefPhysCtrlTableIdx_cu8];

    /* If the EthIfCtrl state is ACTIVE and mapped EthCtrl is ACTIVE */
    if( (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx_u8].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE) &&
        (lDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_ACTIVE) )
    {
        /* -------------------------------------- */
        /* Fill reuse header                      */
        /* -------------------------------------- */

        /* Get the Vlan Id */
        lVlanId_u16 = EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx_u8].VlanId_cu16;

        /* Get address of buffer to write into */
        if( lVlanId_u16 != 0xFFFFU )
        {
            /* If Vlan is present, start writing at index 0 */
            lBufferPtr_pu8 = &(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[0U]);
        }
        else
        {
            /* If Vlan is not present start writing at offset ETHIF_VLANTAG_LENGTH */
            lBufferPtr_pu8 = &(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[ETHIF_VLANTAG_LENGTH]);
        }

        /* Get the local MAC address */
        /* Call Eth_<vi>_<ai>_GetPhysAddr() API */
        lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthGetPhysAddr_pfct(
                         lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                         (&LocalPhysAddr_au8[0U]) );

        /* Write Dest MAC */
        for( lLoopIndex_u8 = 0U; lLoopIndex_u8 < ETHIF_HWADDR_LEN; lLoopIndex_u8++ )
        {
            lBufferPtr_pu8[lLoopIndex_u8] = PhysAddrPtr_pcu8[lLoopIndex_u8];
        }

        /* Write Src MAC */
        for( lLoopIndex_u8 = 0U; lLoopIndex_u8 < ETHIF_HWADDR_LEN; lLoopIndex_u8++ )
        {
            lBufferPtr_pu8[ETHIF_OFFSET_SOURCEMACADDR+lLoopIndex_u8] = LocalPhysAddr_au8[lLoopIndex_u8];
        }

        /* Check if Vlan is configured for the requested EthIfCtrl */
        if( lVlanId_u16 != 0xFFFFU )
        {
            /* Write TPID */
            lBufferPtr_pu8[ETHIF_ETHTYPE_BYTE1] = ( (uint8)( ETHIF_FRAMETYP_VLAN >> 8U ) );
            lBufferPtr_pu8[ETHIF_ETHTYPE_BYTE2] = ( (uint8)( ETHIF_FRAMETYP_VLAN & 0x00FFU ) );

            /* Advance pointer after EtherType */
            lBufferPtr_pu8 += ETHIF_MACHDR_LENGTH+ETHIF_ETHTYPEFIELD_LENGTH;

            /* Write PCP (Priority) to header --> upper 3 bits in ETHIF_VLANTAG_BYTE1 */
            lBufferPtr_pu8[ETHIF_VLANTAG_BYTE1] = ( Priority_u8 << 5U );

            /* Write VLANid into header */
            /* Don't overwrite upper 4 bits in ETHIF_VLANTAG_BYTE1 because they are PCP(priority of the VLAN frame) and CFI */
            lBufferPtr_pu8[ETHIF_VLANTAG_BYTE1] = ( (lBufferPtr_pu8[ETHIF_VLANTAG_BYTE1])) | ( (uint8)( lVlanId_u16 >> 8U ) );
            lBufferPtr_pu8[ETHIF_VLANTAG_BYTE2] = ( (uint8)( lVlanId_u16 & 0x00FFU ) );

            /* Write EthType */
            lBufferPtr_pu8[ETHIF_ETHTYPE_IN_VLAN_BYTE1] = ( (uint8)( FrameType_u16 >> 8U ) );
            lBufferPtr_pu8[ETHIF_ETHTYPE_IN_VLAN_BYTE2] = ( (uint8)( FrameType_u16 & 0x00FFU ) );
        }
        else
        {
            /* Write EthType */
            lBufferPtr_pu8[ETHIF_ETHTYPE_BYTE1] = ( (uint8)( FrameType_u16 >> 8U ) );
            lBufferPtr_pu8[ETHIF_ETHTYPE_BYTE2] = ( (uint8)( FrameType_u16 & 0x00FFU ) );
        }

        lStdRetVal_en = E_OK;
    }

    return lStdRetVal_en;
}


/*****************************************************************************************************
 * \module description
 * Execute the ZeroCopy transmission
 * \par Service ID 0x58, Synchronous, Reentrant
 *
 * Parameter In:
 * \param CtrlIdx_u8        Index of the Ethernet controller within the context of the Ethernet Interface
 * \param ZeroCopyArg_pcst  ZeroCopy argument containing the allocated headers and the pointer to the UL data to be transmitted
 *
 * Parameter Out:
 * \param TxId_pu32         Unique identifier of the ZeroCopy transmission (valid only if ZeroCopyArg_pst->TxConfFlag_b is TRUE)
 *
 * \return  Std_ReturnType {E_OK: success; E_NOT_OK: transmission not possible}
 *
 ***************************************************************************************************
 */
/* HIS METRIC PATH VIOLATION IN EthIf_ZeroCopyTransmit: The number of path is increased due to the consecutive if blocks which are required for the execution of various DET checks. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
/* MR12 RULE 8.13 VIOLATION: When EthIfCtrl has no VLAN configured, the adressed object by ZeroCopyArg_pst is modified inside the function (resizing of the buffer given in ZeroCopyArg_pst is executed) */
Std_ReturnType EthIf_ZeroCopyTransmit( uint8 CtrlIdx_u8,
                                       Eth_ZeroCopyArg_tst * ZeroCopyArg_pst,
                                       uint32 * TxId_pu32 )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*          lDynamicPhysCtrl_pst;
    Std_ReturnType                      lStdRetVal_en;
    uint8                               lLoopIndex_u8;
    uint16                              lEthernetFrameLen_u16;
    uint16                              lTxId_u16;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VALUE( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT,
                                  ETHIF_E_NOT_INITIALIZED, E_NOT_OK )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VALUE( (CtrlIdx_u8 >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT,
                                  ETHIF_E_INV_CTRL_IDX, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pst == NULL_PTR), ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pst->ZeroCopyAllocatedHeaders_ast == NULL_PTR), ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET: At least the Reuse header need to be present so that EthIf header can be written */
    ETHIF_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pst->AllocatedHeaderCount_u8 < 1U ), ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8 == NULL_PTR), ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Report DET: The length of the header block need to be big enough so that ethernet header can be present */
    ETHIF_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartLen_u8 < ETHIF_ZEROCOPY_ETHERNET_HDR_LEN ), ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT, ETHIF_E_INV_PARAM, E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    ETHIF_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pst->ZeroCopyData_pcst == NULL_PTR), ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT, ETHIF_E_PARAM_POINTER, E_NOT_OK )

    /* Initialize local variable */
    lStdRetVal_en = E_NOT_OK;

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx_u8].RefPhysCtrlTableIdx_cu8];

    /* If EthDriver API is configured */
    if( lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthZeroCopyTransmit_pfct != NULL_PTR )
    {
        /* Compute the length of Ethernet frame to be transmitted */
        lEthernetFrameLen_u16 = ZeroCopyArg_pst->ZeroCopyData_pcst->DataLen_u16;
        for( lLoopIndex_u8 = 0U; lLoopIndex_u8 < ZeroCopyArg_pst->AllocatedHeaderCount_u8; lLoopIndex_u8++ )
        {
            lEthernetFrameLen_u16 += ZeroCopyArg_pst->ZeroCopyAllocatedHeaders_ast[lLoopIndex_u8].HeaderPartLen_u8;
        }

        /* Check if the length to be transmitted is not greater than MTU */
        /* and check if the EthIfCtrl state is ACTIVE and mapped EthCtrl is ACTIVE */
        if( (lEthernetFrameLen_u16 <= (ETHIF_ZEROCOPY_ETHERNET_HDR_LEN+EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx_u8].MtuSize_cu16)) &&
            (EthIf_CfgPtr_pco->EthIf_DynamicEthIfCtrlTable_cpst[CtrlIdx_u8].State_en == ETHIF_ETHIFCTRL_STATE_ACTIVE) &&
            (lDynamicPhysCtrl_pst->CtrlState_en == ETHIF_CTRL_STATE_ACTIVE) )
        {
            /* -------------------------------------- */
            /* Adjust buffer if no VLAN               */
            /* -------------------------------------- */

            /* If Vlan is not configured for the requested EthIfCtrl */
            if( EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx_u8].VlanId_cu16 == 0xFFFFU )
            {
                /* Adjust the zero copy allocated reuse header (not the complete length is used as the Vlan is not filled) */
                ZeroCopyArg_pst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8 += ETHIF_VLANTAG_LENGTH;
                ZeroCopyArg_pst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartLen_u8 -= ETHIF_VLANTAG_LENGTH;
            }

            /* -------------------------------------- */
            /* Call lower layer                       */
            /* -------------------------------------- */

            /* Call the driver API to execute ZeroCopy transmission */
            /* Call Eth_<vi>_<ai>_ZeroCopyTransmit() API */
            lStdRetVal_en = lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthZeroCopyTransmit_pfct(
                                                  lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                                  ZeroCopyArg_pst,
                                                  &lTxId_u16 );

            /* -------------------------------------- */
            /* Handle returned TxId                   */
            /* -------------------------------------- */

            /* If TxConf flag is TRUE which means that TxId was provided by Driver */
            if( (lStdRetVal_en == E_OK) && (ZeroCopyArg_pst->TxConfFlag_b != FALSE) )
            {
                /* Construct TxId_u32 based on TxId provided by Driver */
                *TxId_pu32 = (uint32)( ((uint32)(EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx_u8].RefPhysCtrlTableIdx_cu8) << 16U) | (uint32)(lTxId_u16) );
            }
        }
    }
    else
    {
        /* Report DET : Development Error: Underlying EthDriver do not have required API configured */
        ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_ZEROCOPYTRANSMIT, ETHIF_E_INV_CTRL_IDX )
    }

    return lStdRetVal_en;
}


/*****************************************************************************************************
 * \module description
 * Mark the last accepted ZeroCopy transmission with TxConf=TRUE so that EthIf_ZeroCopyTxConfirmation will be called when this last Tx will be completed
 * \par Service ID 0x59, Synchronous, Reentrant
 *
 * Parameter In:
 * \param CtrlIdx_u8        Index of the Ethernet controller within the context of the Ethernet Interface
 *
 * Parameter Out:
 * \param TxId_pu32         Unique identifier of the last executed ZeroCopy transmission
 *
 * \return  void
 *
 ***************************************************************************************************
 */
void EthIf_ZeroCopyMarkLastTx( uint8 CtrlIdx_u8,
                               uint32 * TxId_pu32 )
{
    /* Declare local variables */
    EthIf_DynamicPhysCtrl_tst*          lDynamicPhysCtrl_pst;
    uint16                              lTxId_u16;

    /* Report DET : Development Error: Module not initialised */
    ETHIF_REPORT_ERROR_RET_VOID( (EthIf_InitStatus_en != ETHCTRL_STATE_INIT), ETHIF_SID_ETHIF_ZEROCOPYMARKLASTTX,
                                  ETHIF_E_NOT_INITIALIZED )

    /* Report DET : Development Error: CtrlIdx is not valid */
    ETHIF_REPORT_ERROR_RET_VOID( (CtrlIdx_u8 >= EthIf_CfgPtr_pco->EthIf_GeneralTable_cpcst->nrEthIfCtrl_cu8),
                                  ETHIF_SID_ETHIF_ZEROCOPYMARKLASTTX,
                                  ETHIF_E_INV_CTRL_IDX )

    /* Fetch the PhysCtrl structure from EthIfCtrl index */
    lDynamicPhysCtrl_pst = &EthIf_CfgPtr_pco->EthIf_DynamicPhysCtrlTable_cpst[EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx_u8].RefPhysCtrlTableIdx_cu8];

    /* If EthDriver API is configured */
    if( lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthZeroCopyMarkLastTx_pfct != NULL_PTR )
    {
        /* -------------------------------------- */
        /* Call lower layer                       */
        /* -------------------------------------- */

        /* Call the driver API to execute ZeroCopy transmission */
        /* Call Eth_<vi>_<ai>_ZeroCopyMarkLastTx() API */
        lDynamicPhysCtrl_pst->RefEthDrvAPITablePtr_cpcst->EthZeroCopyMarkLastTx_pfct(
                                                          lDynamicPhysCtrl_pst->EthCtrlIdx_cu8,
                                                          &lTxId_u16 );

        /* Construct TxId_u32 based on TxId provided by Driver */
        *TxId_pu32 = (uint32)( ((uint32)(EthIf_CfgPtr_pco->EthIf_StaticEthIfCtrlTable_cpcst[CtrlIdx_u8].RefPhysCtrlTableIdx_cu8) << 16U) | (uint32)(lTxId_u16) );
    }
    else
    {
        /* Report DET : Development Error: Underlying EthDriver do not have required API configured */
        ETHIF_REPORT_ERROR_TRUE_NO_RET( ETHIF_SID_ETHIF_ZEROCOPYMARKLASTTX, ETHIF_E_INV_CTRL_IDX )
    }

    return;
}

#define ETHIF_STOP_SEC_CODE_FAST
#include "EthIf_MemMap.h"

#endif /* #if ( ETHIF_ZEROCOPY_TX_SUPPORT == STD_ON ) */
#endif /* ETHIF_CONFIGURED */
