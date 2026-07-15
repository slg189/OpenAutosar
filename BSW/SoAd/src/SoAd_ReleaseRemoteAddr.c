

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "SoAd.h"
#ifdef SOAD_CONFIGURED
#include "SoAd_Prv_Domain.h"

 /*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

 /*
 ***************************************************************************************************
 * Prototype for Static functions: Start
 ***************************************************************************************************
 */
#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

 /**
 ***************************************************************************************************
 * \Function Name : SoAd_ReleaseRemoteAddr()
 *
 * \function description
 * By this API service the remote address (IP address and port) of the specified socket connection
 * shall be released , i.e. set back to the configured remote address setting.
 * Note: The intention is to roll back to a wildcard configuration after it was set via SoAd_UniqueRemoteAddr().
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId - socket connection index for which the remote address shall be released
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:        None
 *
 ***************************************************************************************************
 */
 void SoAd_ReleaseRemoteAddr(SoAd_SoConIdType SoConId)
 {

    /* Index to store the dynamic socket connection */
    SoAd_SoConIdType                  lIdxDynSocket_uo;

    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst * lSoAdStaticSocConfig_cpst;

    /* Local pointer to store the Dynamic array */
    SoAd_DyncSocConfigType_tst         * lSoAdDyncSocConfig_pst ;

    /* DET error reporting */
    /* Check whether SoAd Init function was already called or not */
    SOAD_DET_CHECK_ERROR_VOID( ( FALSE == SoAd_InitFlag_b ), SOAD_RELEASE_REMOTE_ADDR_API_ID, SOAD_E_NOTINIT )

    /* Check whether SoConId is valid or not */
    SOAD_DET_CHECK_ERROR_VOID( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId ),
                                   SOAD_RELEASE_REMOTE_ADDR_API_ID, SOAD_E_INV_ARG)

    /* Check whether Automatic socket is enabled or not */
    SOAD_DET_CHECK_ERROR_VOID( ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo].soConGrpAutoConSetup_b ) == TRUE ), SOAD_RELEASE_REMOTE_ADDR_API_ID, SOAD_E_INV_ARG );

    /* Initialize the local variables which are declared above */
    /* Get the "index of dynamic socket table" from static socket table */
    lIdxDynSocket_uo = (* SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].SoAd_Prv_idxDynSocTable_puo) ;

    /* Dynamic socket is not yet assigned for the static socket, that means no SoAd_OpenSoCon is called for this socket.
    * So, first get the available dynamic socket and link it with the static socket and update the remote address in dync table */
    if(lIdxDynSocket_uo <= SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo)
    {
        /* Initialization of local variables */
        /* copy the global array index address to local pointer for further processing */
        lSoAdStaticSocConfig_cpst   = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId]);

        /* Extract pointer to Dynamic structure for given index */
        lSoAdDyncSocConfig_pst  =  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo]);

        /* Condition checks:
         *  If the SoConID refers to a socket connection which is not currently locked by the SoAd
         */
        if( E_OK == SoAd_IsRemoteAddrUnLock(SoConId, lIdxDynSocket_uo))
        {
            /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
            SchM_Enter_SoAd_RemoteAddrUpdate();

            /* release the remote address */
            SoAd_CopyRemoteAddrFromStaticSocToDynSoc(lSoAdStaticSocConfig_cpst, lSoAdDyncSocConfig_pst);

            SchM_Exit_SoAd_RemoteAddrUpdate();
        }
        else
        {
            (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].PostponeReleaseRemoteAddr_b) = TRUE;
        }
    } /* End for: if(lIdxDynSocket_uo >= SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo) */
    return;
 }

/**
 ***************************************************************************************************
 * \Function Name : SoAd_IsRemoteAddrUnLock()
 *
 * \function description
 * This function is called from SoAd_ReleaseRemoteAddr to check remote address is locked or not
 * It covers SWS_SoAd_00743 requirement.
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     IdxDynSocket_uo - Dynamic socket connection index for which the remote address shall be released
 *
 *  Parameters (inout): None
 *
 *  Parameters (out):   None
 *
 *  Return value:       None
 *
 ***************************************************************************************************
 */
Std_ReturnType SoAd_IsRemoteAddrUnLock( SoAd_SoConIdType SoConId_uo,
                                               SoAd_SoConIdType IdxDynSocket_uo)

{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst *      lSoAdStaticSocConfig_cpst;

    /* Initialize the funtion return variable to E_OK */
    Std_ReturnType lFunctionRetVal_u8;

    /* Variable to hold the return value */
    SoAd_SoConModeType         lSoConMode_en;

    /* Variable to hold the socon submode */
    SoAd_SoConSubMode_ten      lSoConSubMode_en;

    /* Variable to store the port for remote address */
    uint16                     lStaticSocPort_u16;

    /* Variable to store the port for dynamic socket */
    uint16                     lDyncSocPort_u16;

    /* Variable to hold if the address from RemoteAddrPtr is wildcard (ANY) */
    boolean                    lStaticIpAddrIsWildcard_b;

    /* Variable to store if the remote IP address of dynamic socket is wildcard (ANY) */
    boolean                    lDyncSocConfigHasWildcardAnyAddr_b;

    boolean                    lModeChangeCbkReq_b;

    /* Get static socket configuration pointers */
    lSoAdStaticSocConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId_uo]);

    lFunctionRetVal_u8  = E_NOT_OK ;

    lModeChangeCbkReq_b = FALSE;

    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    lStaticSocPort_u16 = SOAD_GET_REMOTE_PORT_FROM_STATIC_SOC_CONFIG( ((const SoAd_StaticSocConfigType_tst *) &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId_uo])));

    /* As per SoAd old design 0xFFFFU was treated as wildcard remote address
     * As per Diamler Importer scripts, 0 has to be treated as wild card.
     * So, SoAd will treat both 0xFFFFU and 0 as wildcard by considering the backward compatibility and TCPIP_PORT_ANY will be used
     * as wildcard inside SoAd component. So It will be overwritten with TCPIP_PORT_ANY if port is 0xFFFFU */
    if( 0xFFFFU == lStaticSocPort_u16)
    {
        lStaticSocPort_u16 = TCPIP_PORT_ANY;
    }

    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    lStaticIpAddrIsWildcard_b = SOAD_STATIC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( ((const SoAd_StaticSocConfigType_tst *) &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId_uo])));

    /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
    SchM_Enter_SoAd_SocConModeProperties();

    lSoConMode_en    = SoAd_DyncSocConfig_ast[IdxDynSocket_uo].socConMode_en;
    lSoConSubMode_en = SoAd_DyncSocConfig_ast[IdxDynSocket_uo].socConSubMode_en;

    SchM_Exit_SoAd_SocConModeProperties();

    /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
    SchM_Enter_SoAd_TxRxReq();

    /* conditions checks [SWS_SoAd_00743]
     * (a) TCP socket connections not in state SOAD_SOCON_OFFLINE,
     * (b) active receptions
     * (c) pending receptions of TP-PDUs,
     * (d) active transmissions,
     * (e) pending transmissions of the nPdu feature
    */

    if( (SOAD_SOCON_OFFLINE == SoAd_DyncSocConfig_ast[IdxDynSocket_uo].socConMode_en  )
#if (SOAD_UDP_PRESENT != STD_OFF )
               || ( ( TCPIP_IPPROTO_UDP == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId_uo].soConGrpId_uo].soConGrpProtocol_en )
#if (SOAD_TP_PRESENT != STD_OFF )
      && ( FALSE == SoAd_DyncSocConfig_ast[IdxDynSocket_uo].tpReceptionOngoing_b)
      && ( FALSE == SoAd_DyncSocConfig_ast[IdxDynSocket_uo].tpTransmissionOnGoing_b)
#endif /* SOAD_TP_PRESENT != STD_OFF */
#if (SOAD_IF_PRESENT != STD_OFF )
      &&  ( FALSE  == SoAd_DyncSocConfig_ast[IdxDynSocket_uo].ifTransmissionOnGoing_b)
#endif /* SOAD_IF_PRESENT != STD_OFF */
#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
       && ( FALSE == SoAd_DyncSocConfig_ast[IdxDynSocket_uo].ifRgTxOngoing_b )
       && ( FALSE == SoAd_DyncSocConfig_ast[IdxDynSocket_uo].ifRgTxPending_b )
#endif
       && ( FALSE == SoAd_DyncSocConfig_ast[IdxDynSocket_uo].discardNewTxRxReq_b )
       && (SoAd_DyncSocConfig_ast[IdxDynSocket_uo].rxOngoingCnt_u8 < 1 )
#if ( SOAD_NUM_nPDU_UDP_TX_SOCKET > 0 )
       && ( (lSoAdStaticSocConfig_cpst->nPduUdpTxTableIndex_uo == SOAD_CFG_nPDU_UDP_TX_DEFAULT_VALUE) || \
            ((lSoAdStaticSocConfig_cpst->nPduUdpTxTableIndex_uo < SOAD_CFG_nPDU_UDP_TX_DEFAULT_VALUE ) && \
             (0U == ((SoAd_CurrConfig_cpst->SoAdIf_nPduUdpTransmitInfo_pst)[lSoAdStaticSocConfig_cpst->nPduUdpTxTableIndex_uo].nPduUdpTxBuffCopiedLen_u16))
            )
          )
#endif
       )
#endif /* SOAD_UDP_PRESENT != STD_OFF */
      )

    {
        /* Condition checks:
         * 1. socket connection state is ONLINE.
         * 2. Remote address is wild card,
         */

        /*  If socket connection state is ONLINE and remote address is wild card then change the state of the socket connection to RECONNECT state*/
        if( ( SOAD_SOCON_ONLINE  == lSoConMode_en ) && ( ( TCPIP_PORT_ANY   == lStaticSocPort_u16 ) || lStaticIpAddrIsWildcard_b ) )
        {
            /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
            SchM_Enter_SoAd_SocConModeProperties();

            /* Set the connection request state to reconnect such that it shall be tried in next main function */
            SoAd_DyncSocConfig_ast[IdxDynSocket_uo].socConSubMode_en  = SOAD_SOCON_BOUND;

            SoAd_DyncSocConfig_ast[IdxDynSocket_uo].socConMode_en = SOAD_SOCON_RECONNECT;

            SchM_Exit_SoAd_SocConModeProperties();

            lSoConMode_en = SOAD_SOCON_RECONNECT;

            lModeChangeCbkReq_b = TRUE;

        }
        else
        {
            lDyncSocConfigHasWildcardAnyAddr_b = SOAD_DYNC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( ((SoAd_DyncSocConfigType_tst *) &(SoAd_DyncSocConfig_ast[IdxDynSocket_uo])) );
            lDyncSocPort_u16                   = SOAD_GET_REMOTE_PORT_FROM_DYNC_SOC_CONFIG( ((SoAd_DyncSocConfigType_tst *)  &(SoAd_DyncSocConfig_ast[IdxDynSocket_uo])) );

            if( ( SOAD_SOCON_RECONNECT  == lSoConMode_en ) && ( SOAD_SOCON_BOUND == lSoConSubMode_en ) && \
                  ( ( TCPIP_PORT_ANY   ==  lDyncSocPort_u16) || lDyncSocConfigHasWildcardAnyAddr_b) && \
                 ( ( TCPIP_PORT_ANY   !=  lStaticSocPort_u16) || !lStaticIpAddrIsWildcard_b) )
            {
                /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                SchM_Enter_SoAd_SocConModeProperties();

                /* Set the connection request state to Online such that it shall be tried in next main function */
                SoAd_DyncSocConfig_ast[IdxDynSocket_uo].socConSubMode_en  = SOAD_SOCON_ACTIVE;

                SoAd_DyncSocConfig_ast[IdxDynSocket_uo].socConMode_en = SOAD_SOCON_ONLINE;

                SchM_Exit_SoAd_SocConModeProperties();

                lSoConMode_en = SOAD_SOCON_ONLINE;

                lModeChangeCbkReq_b = TRUE;
            }

        }

        SchM_Exit_SoAd_TxRxReq();

#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
                SoAd_DyncSocConfig_ast[IdxDynSocket_uo].ZeroCopyConnChanged_b = TRUE;
#endif /*SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF*/

        /* postpone releaseRemoteAddr to the SoAd_MainFunction because it is currently locked*/
        lFunctionRetVal_u8 = E_OK;
    }
    else
    {
        SchM_Exit_SoAd_TxRxReq();
    }

    /* check if mode cahbge call back is required*/
    if( FALSE != lModeChangeCbkReq_b )
    {
        /* call a function which shall invoke the upper layer call back for mode change */
        SoAd_ModeChgCallbackToUl( SoConId_uo, lSoConMode_en );
    }

    return(lFunctionRetVal_u8);
}

#endif /* SOAD_CONFIGURED */
