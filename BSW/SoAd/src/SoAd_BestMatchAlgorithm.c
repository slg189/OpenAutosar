

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

#ifdef SOAD_DEBUG_AND_TEST
#include "TestSoAdApplication.h"
#endif

/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
 */

#define SOAD_BEST_MATCH_ALGO_MAX_PRIORITY_NUM       3

/*
 ***************************************************************************************************
 * Prototype for Static functions: Start
 ***************************************************************************************************
 */
#define SOAD_START_SEC_CODE
#include "SoAd_MemMap.h"

LOCAL_INLINE void SoAd_RunBestMatchAlgoWithRmtAddr ( SoAd_SoConIdType           DynSocketId_uo, \
                                                     const TcpIp_SockAddrType * RemoteAddr_cpst, \
                                                     uint8 *                    MatchId_pu8 );

/**
 ***********************************************************************************************************************
 * \Function Name : SoAd_BestMatchAlgorithm()
 *
 * \Function description
 * The function implements the best match algorithm according to below steps.
 *  SWS_SoAd_00680SoAd shall use the following best match algorithm to select a socket connection of a socket
 *  connection group based on a provided (specific) remote address:
 *  1   socket connections that have no (specific or wildcard) remote address set shall be ignored
 *  2   the remote address of the remaining socket connections shall be compared with the provided remote address and
 *  the socket connection with the best match according to the following ordered list (item listed earlier has
 *  higher priority towards items listed later) shall be selected:
 *  a   IP address and port match
 *  b   IP address match (and wildcard set for port)
 *  c   Port match (and wildcard set for IP address)
 *  d   Wildcards are used for both IP address and port
 *  e   No match (i.e. no socket connections can be selected)
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConId_uo : Socket connection Id
 * \param   SoAd_SoConIdType
 * \arg     SoConGrpId_uo: Socket connection group Id.
 * \param   const TcpIp_SockAddrType*
 * \arg     RemoteAddrPtr_cpst: pointer to IP address and port of the remote host.
 *
 * Parameters (inout):   * \param   uint8
 * \arg     SocketIdBestMatch_puo : socket Id for which best match of the remote address is occurred.
 *
 * Parameters (out):    None
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK :      Best match found for the given entry
 *          E_NOT_OK:   not found
 *
 ***********************************************************************************************************************
 */
Std_ReturnType SoAd_BestMatchAlgorithm( SoAd_SoConIdType           SoConId_uo, \
                                        SoAd_SoConIdType           SoConGrpId_uo , \
                                        const TcpIp_SockAddrType * RemoteAddrPtr_cpst, \
                                        SoAd_SoConIdType *         SocketIdBestMatch_puo )
{
    /* End of socket id in the group */
    SoAd_SoConIdType lEndSocketId_uo ;

    /* variable to store index of the static socket connection */
    SoAd_SoConIdType lStaticSocketId_uo ;

    /* variable to signalize that the index found */
    SoAd_SoConIdType lIdxFind_auo[4];

    /* Variable to store the dynamic config table id*/
    SoAd_SoConIdType lIdxDynSocket_uo ;

    /* Variable to hold the return value */
    Std_ReturnType   lRetVal_u8;

    /* Index for looping across the finding socket */
    uint8            lIdxMatch_u8 ;

    /* Index for looping across the finding socket */
    uint8            lIdxHighPrioMatch_u8 ;

    /* variable to identify that the index found */
    boolean          lIsIdxFindMask_ab[4];


    /*Initialization of local variables */
    lIdxFind_auo[0]        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
    lIdxFind_auo[1]        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
    lIdxFind_auo[2]        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
    lIdxFind_auo[3]        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
    lRetVal_u8             = E_NOT_OK;
    lIdxMatch_u8           = 0xFF;
    lIdxHighPrioMatch_u8   = 0xFF;
    lIsIdxFindMask_ab[0]   = FALSE;
    lIsIdxFindMask_ab[1]   = FALSE;
    lIsIdxFindMask_ab[2]   = FALSE;
    lIsIdxFindMask_ab[3]   = FALSE;

    /* Calculate the end socket id from Group table and max socket count */
    lEndSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].startSoConIdx_uo + \
                      SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].maxSoConChannel_uo;

    /* loop through all the sockets connection through the socket connection group for the processing */
    for( lStaticSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].startSoConIdx_uo ; \
                    ((lStaticSocketId_uo < lEndSocketId_uo) && \
                     ((FALSE == lIsIdxFindMask_ab[0]) || (lStaticSocketId_uo == SoConId_uo) )); \
            lStaticSocketId_uo++ )
    {

        /* Get the dynamic socket from static socket table */
        lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lStaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

        /* Check dynamic socket is valid */
        if(lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo)
        {
            /* Call the below function to apply the best match algorithm and to get the match id to which romote address matched */
            SoAd_RunBestMatchAlgoWithRmtAddr(lIdxDynSocket_uo, RemoteAddrPtr_cpst, &lIdxMatch_u8 );

            /* If SoAd_RunBestMatchAlgoWithRmtAddr failed (means that no matching has been found) or if the priority of the matching rule is not high enough */

            /* Specific configuration: First Tx pdu socket and then Rx pdu socket in group */
            /* If sockets are configured in the above order, suppose if we are receiving the packet from remote and */
            /* Best Match Algo ran and returns the socket which matches first(Tx pdu socket) in the group, */
            /* SoAd will discard the packet since no Rx Pdu mapped because it is Tx only socket. */

            /* In the scenario, Best Match Algo has to fetch the Rx pdu socket if any instead of Tx Pdu socket. */
            /* 1. succeed for 1st socket in the group */
            /* 2,3 & 4. If earlier matched socket is mapped to Tx pdu socket, but new matched socket has mapped to Rx Pdu, */
            /*          then Best match Algo has to fetch the socket which is mapped to Rx pdu(second match) */
            if( (0xFFU == lIdxMatch_u8 )  || \
                (lIdxMatch_u8 > lIdxHighPrioMatch_u8) || \
                 ( (lIdxMatch_u8 == lIdxHighPrioMatch_u8) && \
                   (!(( (SoConId_uo == SOAD_CFG_SOCON_ID_DEFAULT_VALUE) && \
                      ((FALSE == lIsIdxFindMask_ab[lIdxMatch_u8]) ||
                      ((TRUE  == lIsIdxFindMask_ab[lIdxMatch_u8]) && \
                         (0U == SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxFind_auo[lIdxMatch_u8]].numberOfRxPdusConfigured_u8) && \
                         (0 < SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lStaticSocketId_uo].numberOfRxPdusConfigured_u8)))) || \
                   (lStaticSocketId_uo == SoConId_uo)) )) )
            {
                continue;
            }

            lIdxHighPrioMatch_u8 = lIdxMatch_u8;
            lIsIdxFindMask_ab[lIdxMatch_u8] = TRUE;
            lIdxFind_auo[lIdxMatch_u8] = lStaticSocketId_uo;
        }
    }

    /* Finally load the static socket index which we found after applying the
    * Best Match Algo by looking all the sockets which are presents in group */
    if(SOAD_BEST_MATCH_ALGO_MAX_PRIORITY_NUM >= lIdxHighPrioMatch_u8)
    {
        /* load the found socon id to out param  SocketIdBestMatch_puo */
        ( *SocketIdBestMatch_puo ) = lIdxFind_auo[lIdxHighPrioMatch_u8];
        lRetVal_u8  = E_OK;
    }

#ifdef SOAD_DEBUG_AND_TEST
    SoAd_RetVal_BestMatch_tst = lRetVal_u8;
#endif

    return(lRetVal_u8);
}

#if (SOAD_BEST_MATCH_ALG_RX_HEADERID_CHK == STD_ON)
/**
 ***********************************************************************************************************************
 * \Function Name : SoAd_BestMatchAlgRxHeaderIdChk()
 *
 * \Function description
 * The function implements the best match algorithm according to below steps.
 *  SWS_SoAd_00680SoAd shall use the following best match algorithm to select a socket connection of a socket
 *  connection group based on a provided (specific) remote address:
 *  1   socket connections that have no (specific or wildcard) remote address set shall be ignored
 *  2   the remote address of the remaining socket connections shall be compared with the provided remote address and
 *  the socket connection with the best match according to the following ordered list (item listed earlier has
 *  higher priority towards items listed later) shall be selected:
 *  a   IP address and port match
 *  b   IP address match (and wildcard set for port)
 *  c   Port match (and wildcard set for IP address)
 *  d   Wildcards are used for both IP address and port
 *  e   No match (i.e. no socket connections can be selected)
 *  f.  Rx header id matches with configured one
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     SoConGrpId_uo: Socket connection group Id.
 * \param   const TcpIp_SockAddrType*
 * \arg     RemoteAddrPtr_cpst: pointer to IP address and port of the remote host.
 * \param   uint8**
 * \arg     Buffer_ppcu8: Buffer pointer.
 * \param   uint16*
 * \arg     Length_pu16: Length of the received message.
 * Parameters (inout):   * \param   uint8
 * \arg     SocketIdBestMatch_puo : socket Id for which best match of the remote address is occurred.
 *
 * Parameters (out):    None
 *
 * Return value:
 * \return  Std_ReturnType
 * \retval  E_OK :      Best match found for the given entry
 *          E_NOT_OK:   not found
 *
 ***********************************************************************************************************************
 */
Std_ReturnType SoAd_BestMatchAlgRxHeaderIdChk(SoAd_SoConIdType           SoConGrpId_uo , \
                                              const TcpIp_SockAddrType * RemoteAddrPtr_cpst, \
                                              SoAd_SoConIdType *         SocketIdBestMatch_puo,
                                              uint8 **                   Buffer_ppcu8, \
                                              uint16 *                   Length_pu16)
{
    /* End of socket id in the group */
    SoAd_SoConIdType lEndSocketId_uo ;

    /* variable to store index of the static socket connection */
    SoAd_SoConIdType lStaticSocketId_uo ;

    /* variable to signalize that the index found */
    SoAd_SoConIdType lIdxFind_auo[4];

    /* Variable to store the dynamic config table id*/
    SoAd_SoConIdType lIdxDynSocket_uo ;

    /* Variable to hold the return value */
    Std_ReturnType lRetVal_u8;

    /* holds the extracted header id*/
    uint32         lRxHeaderExtracted_u32;

    /* holds the pdu length */
    uint32         lPduLength_u32;

    /* holds the processed msg length */
    uint32         lProcessdMsgLength_u32;

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
    /* holds the dropped udp pdu count*/
    uint8          lDroppedUdpCnt_au8[4];
#endif /* SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON */

    /* holds the message length */
    uint16         lLocalLength_au16[4];

    /* local pointer to the buffer */
    uint8 *        lLocalBuffer_pau8[4];

    /* Index for looping across the finding socket */
    uint8          lIdxMatch_u8 ;

    /* Index for looping across the finding socket */
    uint8          lIdxHighPrioMatch_u8 ;

    /* variable to identify that the index found */
    boolean        lIsIdxFindMask_ab[4];

    /* Variable to store the status */
    boolean        lSoConIdFound_b;

    /*Initialization of local variables */
    lIdxFind_auo[0]        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
    lIdxFind_auo[1]        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
    lIdxFind_auo[2]        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;
    lIdxFind_auo[3]        = SOAD_CFG_SOCON_ID_DEFAULT_VALUE;

    lRetVal_u8             = E_NOT_OK;
    lIdxMatch_u8           = 0xFF;
    lIdxHighPrioMatch_u8   = 0xFF;
    lIsIdxFindMask_ab[0]   = FALSE;
    lIsIdxFindMask_ab[1]   = FALSE;
    lIsIdxFindMask_ab[2]   = FALSE;
    lIsIdxFindMask_ab[3]   = FALSE;
    lSoConIdFound_b        = FALSE;
    lPduLength_u32         = 0;
    lProcessdMsgLength_u32 = 0;
    lRxHeaderExtracted_u32  = 0;

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
    lDroppedUdpCnt_au8[0]      = 0;
    lDroppedUdpCnt_au8[1]      = 0;
    lDroppedUdpCnt_au8[2]      = 0;
    lDroppedUdpCnt_au8[3]      = 0;
#endif /* ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON ) */

    /* Calculate the end socket id from Group table and max socket count */
    lEndSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].startSoConIdx_uo + \
                      SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].maxSoConChannel_uo;

    /* loop through all the sockets connection through the socket connection group for the processing */
    for( lStaticSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].startSoConIdx_uo ; \
                    ((lStaticSocketId_uo < lEndSocketId_uo) && \
                     (FALSE == lIsIdxFindMask_ab[0])); \
            lStaticSocketId_uo++ )
    {
        /* Reset the lSoConIdFound_b flag to FALSE for each loop */
        lSoConIdFound_b = FALSE;

        /* Get the dynamic socket from static socket table */
        lIdxDynSocket_uo = (*SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lStaticSocketId_uo].SoAd_Prv_idxDynSocTable_puo);

        /* Check dynamic socket is valid */
        if(lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo)
        {
            /* Call the below function to apply the best match algorithm and to get the match id to which romote address matched */
            SoAd_RunBestMatchAlgoWithRmtAddr(lIdxDynSocket_uo, RemoteAddrPtr_cpst, &lIdxMatch_u8 );

            /* If SoAd_RunBestMatchAlgoWithRmtAddr failed (means that no matching has been found) or if the priority of the matching rule is not high enough */
            if( (0xFFU == lIdxMatch_u8 )  || \
                (lIdxMatch_u8 > lIdxHighPrioMatch_u8) || \
                 ((lIdxMatch_u8 == lIdxHighPrioMatch_u8) && \
                         (!((FALSE == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].headerEnable_b ) && \
                         ((FALSE == lIsIdxFindMask_ab[lIdxMatch_u8]) ||
                       ((TRUE  == lIsIdxFindMask_ab[lIdxMatch_u8]) && \
                          (0U == SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lIdxFind_auo[lIdxMatch_u8]].numberOfRxPdusConfigured_u8) && \
                          (0 < SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[lStaticSocketId_uo].numberOfRxPdusConfigured_u8))))
                          ) ) )
            {
                continue;
            }

            /* ------------------------------------- */
            /* Best Match Algorithm performs from LL */
            /* ------------------------------------- */

            /* 1. SoConId_uo == SOAD_CFG_SOCON_ID_DEFAULT_VALUE */
            /*    Condition will be applicable for Rx path when its called from SoAd_TcpAccepted() for Tcp server socket or from SoAd_RxIndication() for UDP multi sockets */

            lLocalBuffer_pau8[lIdxMatch_u8]   = *Buffer_ppcu8;
            lLocalLength_au16[lIdxMatch_u8]    = *Length_pu16;

            /* --------------- */
            /* Header disabled */
            /* --------------- */

            /* Specific configuration: First Tx pdu socket and then Rx pdu socket in group */
            /* If sockets are configured in the above order, suppose if we are receiving the packet from remote and */
            /* Best Match Algo ran and returns the socket which matches first(Tx pdu socket) in the group, */
            /* SoAd will discard the packet since no Rx Pdu mapped because it is Tx only socket. */

            /* In the scenario, Best Match Algo has to fetch the Rx pdu socket if any instead of Tx Pdu socket. */
            /* 1. succeed for 1st socket in the group */
            /* 2,3 & 4. If earlier matched socket is mapped to Tx pdu socket, but new matched socket has mapped to Rx Pdu, */
            /*          then Best match Algo has to fetch the socket which is mapped to Rx pdu(second match) */
            lSoConIdFound_b = TRUE;

            /* --------------- */
            /* Header enabled  */
            /* --------------- */
            /* If header is enabled, then match the RxPdu header id as well along the Remote info */
            /* Bug fix scenario: Header Id will be diff for each Rx Pdu's, If Header Id is not compared, */
            /* then there is chance to fetch the socket id which not belongs to corresponding UL(header id wont match), so packet will be discarded. */

            /* Get the status of Header whether it is enabled or disabled*/
            /* Check Rx Frame Length as passed in parameter is atleast greater than the Pdu header length*/
            if(TRUE == SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].headerEnable_b )
            {
                lSoConIdFound_b = FALSE;
                lProcessdMsgLength_u32 = 0;

                /* run the while loop, untill processed length reaches the msg length and SoConIdFound_b is False*/
                while(((FALSE == lSoConIdFound_b ) && ((lProcessdMsgLength_u32 + SOAD_PDUHDR_SIZE) <= *Length_pu16)))
                {
                    /* Reading Pdu Id from Ethernet header which shall be used for de-multiplexing and finding out Pdu owner*/
                    /* converting the data from Big Endian to controller specific Endianness*/
                    lRxHeaderExtracted_u32 = (lLocalBuffer_pau8[lIdxMatch_u8][3]) |(((uint32)lLocalBuffer_pau8[lIdxMatch_u8][2 ])<<8u) |\
                                                                (((uint32)lLocalBuffer_pau8[lIdxMatch_u8][1])<<16u) |(((uint32)lLocalBuffer_pau8[lIdxMatch_u8][0])<<24u);
                    /* extract the pdu length */
                    lPduLength_u32 = (lLocalBuffer_pau8[lIdxMatch_u8][7]) |(((uint32)lLocalBuffer_pau8[lIdxMatch_u8][6])<<8u) |\
                            (((uint32)lLocalBuffer_pau8[lIdxMatch_u8][5])<<16u) |(((uint32)lLocalBuffer_pau8[lIdxMatch_u8][4])<<24u);

                    lSoConIdFound_b = SoAd_IsMatchRxPduFound(lStaticSocketId_uo, lRxHeaderExtracted_u32);

                    if(FALSE == lSoConIdFound_b)
                    {
                        /* Increment the processed length by Pdu length and SOAD_PDUHDR_SIZE*/
                        lProcessdMsgLength_u32 += (lPduLength_u32 + SOAD_PDUHDR_SIZE);

                        /* Increment the buffer pointer by the size of discarded pdu + SOAD_PDUHDR_SIZE */
                        lLocalBuffer_pau8[lIdxMatch_u8] = (lLocalBuffer_pau8[lIdxMatch_u8] + lPduLength_u32 + SOAD_PDUHDR_SIZE);

                        /* Decerement the length by the size of discarded pdu + SOAD_PDUHDR_SIZE */
                        lLocalLength_au16[lIdxMatch_u8] = ((lLocalLength_au16[lIdxMatch_u8] > ((uint16)(lPduLength_u32 + SOAD_PDUHDR_SIZE))) ? \
                                (lLocalLength_au16[lIdxMatch_u8] - ((uint16)(lPduLength_u32 + SOAD_PDUHDR_SIZE))) : 0U);

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
                        /* incrementing the SoAd_RxUdpDropCnt_u32 */
                        /* [SWS_SoAd_00754] All measurement data which counts data shall not overrun. */
                        lDroppedUdpCnt_au8[lIdxMatch_u8] = (lDroppedUdpCnt_au8[lIdxMatch_u8] + 1U);
#endif /* ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON ) */

                    }/* end of if(FALSE == lSoConIdFound_b) */
                }/* end of while(((FALSE == lSoConIdFound_b ) && (lProcessdMsgLength_u16 < *Length_pu16))) */
            }/* end of if( lRxHeaderExtracted_u32 != 0U ) */

            /* If any of the socket Best Match Algo success, then store the following information
             * 1. Prio Match according to function SoAd_RunBestMatchAlgoWithRmtAddr()
             * 2. set the flag to TRUE to notify match found for the praticular match index
             * 3. store the static socon id to which Best match found
             * */
            if( TRUE == lSoConIdFound_b )
            {
                lIdxHighPrioMatch_u8 = lIdxMatch_u8;
                lIsIdxFindMask_ab[lIdxMatch_u8] = TRUE;
                lIdxFind_auo[lIdxMatch_u8] = lStaticSocketId_uo;
            }
        }/* end of if(lIdxDynSocket_uo < SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo) */
    }/* end of for( lStaticSocketId_uo = SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoConGrpId_uo].startSoConIdx_uo ; */

    /* Finally load the static socket index which we found after applying the
    * Best Match Algo by looking all the sockets which are presents in group */
    if(SOAD_BEST_MATCH_ALGO_MAX_PRIORITY_NUM >= lIdxHighPrioMatch_u8)
    {
        /* load the found socon id to out param  SocketIdBestMatch_puo */
        (*SocketIdBestMatch_puo ) = lIdxFind_auo[lIdxHighPrioMatch_u8];
        lRetVal_u8  = E_OK;

        /* if header id is not 0 */
        if( lRxHeaderExtracted_u32 != 0U )
        {
            /* update the buffer pointer with the local pointer */
            *Buffer_ppcu8 = lLocalBuffer_pau8[lIdxHighPrioMatch_u8];

            /* update the length with local length variable */
            *Length_pu16 = lLocalLength_au16[lIdxHighPrioMatch_u8];

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
            /* The exclusive area SoAd_MeasurementData is used to avoid race condition when measurement data is stored */
            SchM_Enter_SoAd_MeasurementData();

            /* incrementing the SoAd_RxUdpDropCnt_u32 */
            /* [SWS_SoAd_00754] All measurement data which counts data shall not overrun. */
            SoAd_RxUdpDropCnt_u32 = (((0xFFFFFFFFu - SoAd_RxUdpDropCnt_u32) >= \
                                            (uint32)(lDroppedUdpCnt_au8[lIdxHighPrioMatch_u8])) ? \
                                    (SoAd_RxUdpDropCnt_u32 + (uint32)(lDroppedUdpCnt_au8[lIdxHighPrioMatch_u8])) : 0xFFFFFFFFu);

            SchM_Exit_SoAd_MeasurementData();
#endif /* ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON ) */
        }/* end of if( lRxHeaderExtracted_u32 != 0U )  */
   }


#ifdef SOAD_DEBUG_AND_TEST
    SoAd_RetVal_BestMatch_tst = lRetVal_u8;
#endif

    return(lRetVal_u8);
}
#endif /* SOAD_BEST_MATCH_ALG_RX_HEADERID_CHK == STD_ON */

/**
 ***************************************************************************************************
 * \Function Name : SoAd_RunBestMatchAlgoWithRmtAddr()
 *
 * \Function description
 * The function implements the best match algorithm according to below steps.
 *  a   IP address and port match
 *  b   IP address match (and wildcard set for port)
 *  c   Port match (and wildcard set for IP address)
 *  d   Wildcards are used for both IP address and port
 *  e   No match (i.e. no socket connections can be selected)
 *
 *  Parameters (in):
 * \param   SoAd_SoConIdType
 * \arg     DynSocketId_uo : Dynamic socket id mapped to static socket id
 * \param   const TcpIp_SockAddrType*
 * \arg     RemoteAddr_cpst: pointer to IP address and port of the remote host.
 *
 *
 * Parameters (inout):
 * \param   uint8*
 * \arg     MatchId_pu8 : socket Id for which best match of the remote address is occurred.
 *
 * Parameters (out):    None
 *
 * Return value: None
 ***************************************************************************************************
 */
LOCAL_INLINE void SoAd_RunBestMatchAlgoWithRmtAddr( SoAd_SoConIdType           DynSocketId_uo, \
                                                    const TcpIp_SockAddrType * RemoteAddr_cpst, \
                                                    uint8 *                    MatchId_pu8 )
{
    /* Local pointer to store the Dynamic array for given Index */
    SoAd_DyncSocConfigType_tst * lSoAdDyncSocConfig_pst ;

    /* Variable to hold if the remote address and the address from dynamic socket are equal */
    boolean                      lAddressesAreEqual_b;

    /* Variable to store the port for dynamic socket */
    uint16                       lDyncSocPort_u16;

    /* Variable to store the port for remote address */
    uint16                       lRemoteAddrPort_u16;

    /* Variable to store if the remote address of dynamic socket is wildcard (ANY)  */
    boolean                      lDyncSocConfigHasWildcardAnyAddr_b;

    /* Get the index of related socket connection group */
    lSoAdDyncSocConfig_pst   =  &(SoAd_DyncSocConfig_ast[DynSocketId_uo]);

    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    lRemoteAddrPort_u16                = SOAD_GET_PORT_FROM_SOCK_ADDR(RemoteAddr_cpst);

    /* The exclusive area RemoteAddrUpdate is used to avoid race condition when remote address is read */
    SchM_Enter_SoAd_RemoteAddrUpdate();

    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    lAddressesAreEqual_b               = SOAD_SOCK_IPADDR_IS_EQUAL_TO_DYNC_SOC_CONFIG_REMOTE_IPADDR(RemoteAddr_cpst, lSoAdDyncSocConfig_pst);
    lDyncSocPort_u16                   = SOAD_GET_REMOTE_PORT_FROM_DYNC_SOC_CONFIG(lSoAdDyncSocConfig_pst);
    lDyncSocConfigHasWildcardAnyAddr_b = SOAD_DYNC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR(lSoAdDyncSocConfig_pst);

    SchM_Exit_SoAd_RemoteAddrUpdate();

    /* if the remote IP address and port match */
    if( lAddressesAreEqual_b && ( lRemoteAddrPort_u16 == lDyncSocPort_u16 ) )
    {
        ( *MatchId_pu8 ) = 0;
    }
    /* if the remote IP address is match and port is wildcard */
    else if( lAddressesAreEqual_b && ( TCPIP_PORT_ANY == lDyncSocPort_u16 ) )
    {
        ( *MatchId_pu8 ) = 1;
    }
    /* if the remote port is match and IP is wildcard */
    else if( lDyncSocConfigHasWildcardAnyAddr_b && ( lRemoteAddrPort_u16 == lDyncSocPort_u16 ) )
    {
        ( *MatchId_pu8 ) = 2;
    }
    /* Remote IP and port are wildcard */
    else if( lDyncSocConfigHasWildcardAnyAddr_b && ( TCPIP_PORT_ANY == lDyncSocPort_u16 ) )
    {

        ( *MatchId_pu8 ) = 3;
    }
    /* No match, so update FF to the variable */
    else
    {
        ( *MatchId_pu8 ) = 0xFF;
    }

}

#define SOAD_STOP_SEC_CODE
#include "SoAd_MemMap.h"

#endif /* SOAD_CONFIGURED */

