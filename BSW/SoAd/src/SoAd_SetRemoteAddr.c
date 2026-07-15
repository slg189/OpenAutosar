

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "SoAd.h"
#ifdef SOAD_CONFIGURED

#include "TcpIp.h"

#include "SoAd_Prv_Domain.h"

#if (!defined(TCPIP_AR_RELEASE_MAJOR_VERSION) || (TCPIP_AR_RELEASE_MAJOR_VERSION != SOAD_AR_RELEASE_MAJOR_VERSION))
#error "AUTOSAR major version undefined or mismatched  - SoAd and TcpIp"
#endif

#if (!defined(TCPIP_AR_RELEASE_MINOR_VERSION) || (TCPIP_AR_RELEASE_MINOR_VERSION != SOAD_AR_RELEASE_MINOR_VERSION))
#error "AUTOSAR minor version undefined or mismatched - SoAd and TcpIp"
#endif

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

static Std_ReturnType SoAd_SetRemoteAddrDetChk( SoAd_SoConIdType           SoConId_uo, \
                                                const TcpIp_SockAddrType * RemoteAddrPtr_cpst);

/**
 ***************************************************************************************************
 * \Function Name : SoAd_SetRemoteAddr()
 *
 * \Function description
 *  By this API service the remote address (IP address and port) of the specified socket connection shall be set.
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId - socket connection index specifying the socket connection for which the remote address shall be set.
 * \param   TcpIp_SockAddrType*
 * \arg     RemoteAddrPtr - Struct contain the IP address and port to be set.
 *
 * Parameters (inout):  None
 *
 * Parameters (out):    None
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK: The request has been accepted
 *          E_NOT_OK: The request has not been accepted
 *
 *
 ***************************************************************************************************
 */
Std_ReturnType SoAd_SetRemoteAddr( SoAd_SoConIdType           SoConId , \
                                   const TcpIp_SockAddrType * RemoteAddrPtr )
{
    /* Local pointer to store the Static array for given index*/
    const SoAd_StaticSocConfigType_tst *      lSoAdStaticSocConfig_cpst;

    /* Variable to hold the return value */
    SoAd_SoConModeType         lSoConMode_en;

    /* Variable to hold the socon submode */
	SoAd_SoConSubMode_ten      lSoConSubMode_en;

    Std_ReturnType             lFunctionRetVal_u8;

    Std_ReturnType             RetVal_u8;

    /* Index to store the dynamic socket connection */
    SoAd_SoConIdType           lIdxDynSocket_uo;

    /* Variable to store the port for remote address */
    uint16                     lRemoteAddrPort_u16;

    /* Variable to store the port for dynamic socket */
    uint16                     lDyncSocPort_u16;

    /* Variable to hold if the address from RemoteAddrPtr is wildcard (ANY) */
    boolean                    lRemoteIpAddrIsWildcard_b;

    /* Variable to store if the remote IP address of dynamic socket is wildcard (ANY) */
    boolean                    lDyncSocConfigHasWildcardAnyAddr_b;

    boolean                    lModeChangeCbkReq_b;

    /* Get static socket configuration pointers */
    lSoAdStaticSocConfig_cpst = &(SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId]);

    /* Initialize the funtion return variable to E_NOT_OK */
    lFunctionRetVal_u8  = E_NOT_OK;

    RetVal_u8 = E_NOT_OK;

    lModeChangeCbkReq_b = FALSE;

    /* Function call to do the DET check */
    RetVal_u8 = SoAd_SetRemoteAddrDetChk(SoConId, RemoteAddrPtr);

    if(RetVal_u8 == E_OK)
    {
        /* Initialize the local variables which are declared above */
        /* Get the "index of dynamic socket table" from static socket table */
        lIdxDynSocket_uo = (* SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].SoAd_Prv_idxDynSocTable_puo) ;

        /* Dynamic socket is not yet assigned for the static socket, That means no SoAd_OpenSoCon is called for this socket.
         * So, first get the available dynamic socket and link it with the static socket and update the remote address in dync table */
        if(lIdxDynSocket_uo >= SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo)
        {
            /* Get the available dynamic socket from the list by calling a function */
            SoAd_ReserveFreeDynSocket(SoConId, &lIdxDynSocket_uo);
        }

        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        lRemoteAddrPort_u16 = SOAD_GET_PORT_FROM_SOCK_ADDR(RemoteAddrPtr);

        /* As per SoAd old design 0xFFFFU was treated as wildcard remote address
         * As per Diamler Importer scripts, 0 has to be treated as wild card.
         * So, SoAd will treat both 0xFFFFU and 0 as wildcard by considering the backward compatibility and TCPIP_PORT_ANY will be used
         * as wildcard inside SoAd component. So It will be overwritten with TCPIP_PORT_ANY if port is 0xFFFFU */
        if( 0xFFFFU == lRemoteAddrPort_u16)
        {
            lRemoteAddrPort_u16 = TCPIP_PORT_ANY;
        }

        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        lRemoteIpAddrIsWildcard_b = SOAD_SOCK_ADDR_HAS_WILDCARD_IPADDR(RemoteAddrPtr);

        /****************************************************************************************************************/
        /****************************************** [SWS_SoAd_00532] ****************************************************/
        /****************************************************************************************************************/
        /****************************************************************************************************************/
        /** The function SoAd_SetRemoteAddr() shall only proceed if SoConId refers to                                 ***/
        /**  (1) a socket connection that is in the mode SOAD_SOCON_OFFLINE or                                        ***/
        /**  (2) a UDP socket connection that has no active TP session (i.e. no reception or transmission via the     ***/
        /**      TP-API ongoing). Otherwise the request shall be rejected and return with E_NOT_OK                    ***/
        /****************************************************************************************************************/
        /****************************************************************************************************************/

        /* Condition checks:
         * 1. The dynamic socket is valid or not
         * 2. The remote address is updated for TCP only if socket connection is closed. or
         * 3. Remote address shall be updated for UDP sockets if there are no active TP processing on this socket connection,
         *    even though socket is ONLINE
         */
        /* Remote address shall be updated for UDP sockets if there is no active IF Tx ongoing and there is no close request from TCPIP.
         * This check is added to ensure data consistency in multicore environment*/

        if( lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo )
        {
            /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
            SchM_Enter_SoAd_SocConModeProperties();

            lSoConMode_en    = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en;
            lSoConSubMode_en = SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en;

            SchM_Exit_SoAd_SocConModeProperties();

            /* The exclusive area SoAd_TxRxReq is used to avoid race condition when there is Req for Tx/Rx or for discard Tx/Rx req */
            SchM_Enter_SoAd_TxRxReq();

            if( ( SOAD_SOCON_OFFLINE == lSoConMode_en )
#if (SOAD_UDP_PRESENT != STD_OFF )
                || ( ( TCPIP_IPPROTO_UDP == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId].soConGrpId_uo].soConGrpProtocol_en )
#if (SOAD_TP_PRESENT != STD_OFF )
                 && ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].tpTransmissionOnGoing_b )
                 && ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].tpReceptionOngoing_b )
#endif /* SOAD_TP_PRESENT != STD_OFF */
#if (SOAD_IF_PRESENT != STD_OFF )
                 && ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifTransmissionOnGoing_b )
#endif
#if ( SOAD_IF_TRIGGER_TX_ENABLED != STD_OFF )
                 && ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxOngoing_b )
                 && ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ifRgTxPending_b )
#endif
                 && ( FALSE == SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].discardNewTxRxReq_b )
                 && (SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].rxOngoingCnt_u8 < 1 )
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

                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00687] ****************************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** If the function SoAd_SetRemoteAddr() is used to set the remote address of a socket connection that is in  ***/
                /** the mode SOAD_SOCON_ONLINE to a value that contains wildcards, SoAd shall change the mode of the socket   ***/
                /** connection to SOAD_SOCON_RECONNECT                                                                        ***/
                /****************************************************************************************************************/
                /****************************************************************************************************************/

                /* Condition checks:
                 * 1. socket connection state is ONLINE.
                 * 2. Remote address is wild card,
                 */

                /*  If socket connection state is ONLINE and remote address is wild card then change the state of the socket connection to RECONNECT state*/
                if( ( SOAD_SOCON_ONLINE  == lSoConMode_en ) && ( ( TCPIP_PORT_ANY   == lRemoteAddrPort_u16 ) || lRemoteIpAddrIsWildcard_b ) )
                {
                    /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                    SchM_Enter_SoAd_SocConModeProperties();

                    /* Set the connection request state to reconnect such that it shall be tried in next main function */
                    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en  = SOAD_SOCON_BOUND;

                    SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en = SOAD_SOCON_RECONNECT;

                    SchM_Exit_SoAd_SocConModeProperties();

                    lSoConMode_en = SOAD_SOCON_RECONNECT;

                    lModeChangeCbkReq_b = TRUE;

                }
                else
                {
                    /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is read */
                    SchM_Enter_SoAd_RemoteAddrUpdate();

                    lDyncSocConfigHasWildcardAnyAddr_b = SOAD_DYNC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( ((SoAd_DyncSocConfigType_tst *) &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo])) );
                    lDyncSocPort_u16                   = SOAD_GET_REMOTE_PORT_FROM_DYNC_SOC_CONFIG( ((SoAd_DyncSocConfigType_tst *)  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo])) );

                    SchM_Exit_SoAd_RemoteAddrUpdate();

                    if( ( SOAD_SOCON_RECONNECT  == lSoConMode_en ) && ( SOAD_SOCON_BOUND == lSoConSubMode_en ) && \
                         ( ( TCPIP_PORT_ANY   ==  lDyncSocPort_u16) || lDyncSocConfigHasWildcardAnyAddr_b) && \
                         ( ( TCPIP_PORT_ANY   != lRemoteAddrPort_u16 ) && !lRemoteIpAddrIsWildcard_b ) )
                    {
                        /* The exclusive area SocConModeProperties is used to avoid race condition when the soccket mode and submode are updated */
                        SchM_Enter_SoAd_SocConModeProperties();

                        /* Set the connection request state to reconnect such that it shall be tried in next main function */
                        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConSubMode_en  = SOAD_SOCON_ACTIVE;

                        SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].socConMode_en = SOAD_SOCON_ONLINE;

                        SchM_Exit_SoAd_SocConModeProperties();

                        lSoConMode_en = SOAD_SOCON_ONLINE;

                        lModeChangeCbkReq_b = TRUE;

                    }
                }

                /****************************************************************************************************************/
                /****************************************** [SWS_SoAd_00533] ****************************************************/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /** The function SoAd_SetRemoteAddr() shall set the remote address of the socket connection referred by       ***/
                /** parameter SoConId according to the IP address and port specified by parameter RemoteAddrPtr               ***/
                /****************************************************************************************************************/
                /****************************************************************************************************************/
                /* Load the remote address value in dynamic socket table */
                /* If remote port is 0xFFFFU then remote port address is stored as TCPIP_PORT_ANY in dynamic socket table else remote port is stored with the  port value in RemoteAddrPtr */
                /* Since RemoteAddrPtr is a constant pointer , the remote port passed in RemoteAddrPtr is not overwritten and the remote port to be stored is passed as an argument. */

                /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is modified */
                SchM_Enter_SoAd_RemoteAddrUpdate();

                /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
                SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr( RemoteAddrPtr, \
                                                                lRemoteAddrPort_u16, \
                                                                ((SoAd_DyncSocConfigType_tst *)  &(SoAd_DyncSocConfig_ast[lIdxDynSocket_uo])) );

                SchM_Exit_SoAd_RemoteAddrUpdate();

                SchM_Exit_SoAd_TxRxReq();

#if ( SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF)
                SoAd_DyncSocConfig_ast[lIdxDynSocket_uo].ZeroCopyConnChanged_b = TRUE;
#endif /*SOAD_ZERO_COPY_IF_UDP_TX != STD_OFF*/

                /*  the return value set to E_OK*/
                lFunctionRetVal_u8 = E_OK;
            }
            else
            {
                SchM_Exit_SoAd_TxRxReq();
            }
        }

        /* check if mode cahbge call back is required*/
        if( FALSE != lModeChangeCbkReq_b )
        {
            /* call a function which shall invoke the upper layer call back for mode change */
            SoAd_ModeChgCallbackToUl( SoConId, lSoConMode_en );
        }
    }

    return(lFunctionRetVal_u8);
}

/**
 ***************************************************************************************************
 * \Function Name : SoAd_SetRemoteAddrDetChk()
 *
 * \function description
 * This function is called from SoAd_SetyRemoteAddr to handle the DET error for function arguments.
 * It covers SWS_SoAd_00628, SWS_SoAd_00531 & SWS_SoAd_00699 requirements
 *
 * Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId_uo - socket connection index specifying the socket connection for which the remote address shall be set.
 * \param   const TcpIp_SockAddrType*
 * \arg     RemoteAddrPtr_cpst - Struct contain the IP address and port to be set.
 *
 *  Parameters (inout): None
 *
 *  Parameters (out):   None
 *
 * Return value:        Std_ReturnType
 *                      E_OK: No Det reported
 *                      E_NOT_OK: DET reported
 *
 ***************************************************************************************************
 */
static Std_ReturnType SoAd_SetRemoteAddrDetChk( SoAd_SoConIdType           SoConId_uo, \
                                                const TcpIp_SockAddrType * RemoteAddrPtr_cpst)
{

    /* Variable to store the port for remote address */
    uint16                     lRemoteAddrPort_u16;

    /********************************* Start: DET Checking *************************************/

    /* Check whether SoAd Init function was already called or not */
   SOAD_DET_CHECK_ERROR_RETVAL( ( FALSE == SoAd_InitFlag_b ), SOAD_SET_REMOTE_ADDRESS_API_ID, SOAD_E_NOTINIT, E_NOT_OK )

    /* Check whether SoConId is valid or not */
   SOAD_DET_CHECK_ERROR_RETVAL( ( SoAd_CurrConfig_cpst->SoAd_noStaticSockets_uo <= SoConId_uo ), SOAD_SET_REMOTE_ADDRESS_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

   /* Check whether RemoteAddrPtr pointer is valid or not */
   SOAD_DET_CHECK_ERROR_RETVAL( ( NULL_PTR == RemoteAddrPtr_cpst ), SOAD_SET_REMOTE_ADDRESS_API_ID, SOAD_E_PARAM_POINTER, E_NOT_OK )

   /* get the port for the socket AFTER checking the socket for null pointer*/
   /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
   lRemoteAddrPort_u16 = SOAD_GET_PORT_FROM_SOCK_ADDR(RemoteAddrPtr_cpst);

   /* Check whether Domain and port are valid */
   SOAD_DET_CHECK_ERROR_RETVAL( ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId_uo].soConGrpId_uo].domain_en != RemoteAddrPtr_cpst->domain)  || \
                            ( lRemoteAddrPort_u16 == SOAD_DHCP_RESERVED_CLIENT_PORT ) || \
                            ( lRemoteAddrPort_u16 == SOAD_DHCP_RESERVED_SERVER_PORT ) ), \
                                SOAD_SET_REMOTE_ADDRESS_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

   /* Check whether Automatic socket is enabled or not */
   SOAD_DET_CHECK_ERROR_RETVAL( ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[SoConId_uo].soConGrpId_uo].soConGrpAutoConSetup_b ) == TRUE ), \
                                   SOAD_SET_REMOTE_ADDRESS_API_ID, SOAD_E_INV_ARG, E_NOT_OK )

       /********************************* End: DET Checking *************************************/
    return E_OK;
}


#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */
