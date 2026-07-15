
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )
#include "rba_EthUdp.h"
#include "rba_EthUdp_Cfg_SchM.h"
#include "rba_EthUdp_Cbk.h"

#if ( TCPIP_ICMP_ENABLED == STD_ON )
#include "rba_EthIcmp.h"
#endif

#if ( TCPIP_ICMPV6_ENABLED == STD_ON )
#include "rba_EthIcmpV6.h"
#endif

#include "TcpIp_Lib.h"
#include "TcpIp_Prv.h"
#include "rba_EthUdp_Prv.h"


/*
***************************************************************************************************
* Static function declaration
***************************************************************************************************
*/

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#if ( TCPIP_IPV4_ENABLED == STD_ON )
static void rba_EthUdp_RxIndication_IPv4( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                          uint8 * RxData_pu8,
                                          uint16 RxDataSrcUdpPort_u16,
                                          uint16 RxDataDestUdpPort_u16,
                                          uint16 UdpBodyLen_u16,
                                          TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                          const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                          boolean IsBroadcast_b );
#endif

#if ( TCPIP_ICMP_ENABLED == STD_ON )
static void rba_EthUdp_SendIcmpPortNotReachable( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                 const uint8 * RxData_pcu8,
                                                 TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                                 const TcpIp_SockAddrInetType * RemoteAddrInet_pst );
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
static void rba_EthUdp_RxIndication_IPv6( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                          uint8 * RxData_pu8,
                                          uint16 RxDataSrcUdpPort_u16,
                                          uint16 RxDataDestUdpPort_u16,
                                          uint16 UdpBodyLen_u16,
                                          TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                          const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst );
#endif

#if ( ( TCPIP_ICMPV6_ENABLED == STD_ON ) && ( TCPIP_ICMPV6_MSG_DEST_UNREACH_ENABLE == STD_ON ) )
static void rba_EthUdp_SendIcmpV6PortNotReachable( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                   const uint8 * RxData_pcu8,
                                                   TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                                   const TcpIp_SockAddrInet6Type * RemoteAddrInet6_pst);
#endif

#if( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
LOCAL_INLINE void rba_EthUdp_IncMeasurementData( uint32 * MeasurementData_pu32 );
#endif

#if ( TCPIP_IPV4_ENABLED == STD_ON )
static Std_ReturnType rba_EthUdp_VerifyUdpCksum_IPv4 (uint8    EthIfCtrl_u8,
                                               const TcpIp_PseudoHdr_tst  *   PseudoHdr_pcst,
                                               const uint8 *  RxData_pcu8,
                                               uint16 BufLength_u16,
                                               uint16 * ChecksumField_pu16);
#endif

#if ( ( RBA_ETHUDP_SW_CHKSUM == STD_ON ) && ( TCPIP_IPV6_ENABLED == STD_ON ) )
static Std_ReturnType  rba_EthUdp_VerifyUdpCksum_IPv6 (uint8 EthIfCtrl_u8,
                                               const  TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                               const  uint8   * RxData_pcu8,
                                               uint16 UdpBodyLen_u16 );
#endif

#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

/********************************************************************************************************************/
/* rba_EthUdp_RxIndication() This function is called from IP COMPs to UDP layer after reception of a UDP datagram.  */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input parameters:                                                                                                */
/* PseudoHdr_pcst           Pointer to pseudo structure provided by IPv4. Contains destination IP, source IP        */
/*                          and IPv4 header length                                                                  */
/* LocalAddrId_CtrlIdx_un   Provides the local addrId or EthIf CtrlIdx (if broadcast ) of the received frame        */
/* RxData_pu8               Pointer to the received UDP datagram                                                    */
/* RxDataLen_u16            Length of the UDP datagram                                                              */
/* IsBroadcast_b            Indicates whether received frame is a broadcast frame or not                            */
/*                                                                                                                  */
/* Return type :            void                                                                                    */
/*                                                                                                                  */
/* Notes:                                                                                                           */
/*    DD: Drop the frame if Udp checksum is not set by the remote node. Only UDP with checksum is supported.        */
/*  Processing steps:                                                                                               */
/*  - DET checks                                                                                                    */
/*  - Extract UDP protocol header info                                                                              */
/*  - Verify checksum                                                                                               */
/*  - Search for configured sockets that match the following conditions:                                            */
/*      1. Destination port mentioned in the frame matches with the local port of the socket                        */
/*      2. If the socket state is BOUND                                                                             */
/*      3a. If the received frame is unicast/multicast and localAddressId on which frame was received matches with  */
/*          localAddressId of the socket                                                                            */
/*      3b. If the received frame is broadcast and EthIfCtrlIdx on which frame was received matches with            */
/*          EthIfCtrlIdx of the socket                                                                              */
/*  - Call the SoAd Callback for the socket found.                                                                  */
/*                                                                                                                  */
/*  Note: Callback EXCEPTION: Call DHCP layer callback if the packet matches DHCP port                              */
/********************************************************************************************************************/
void rba_EthUdp_RxIndication( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                              TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                              uint8 * RxData_pu8,
                              uint16 RxDataLen_u16,
                              boolean IsBroadcast_b )
{
    /* Local variable declaration */
    uint16                             lRxDataSrcUdpPort_u16;
    uint16                             lRxDataDestUdpPort_u16;
    uint16                             lUdpHdrTotLen_u16;
    const TcpIp_LocalAddrConfig_to *   lLocalAddrConfig_pcst;
    boolean                            lRxFrameValid_b;
    Std_ReturnType                     lRetVal_en;

    /* Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_RX_INDICATION_API_ID, RBA_ETHUDP_E_NOTINIT )

    /* Report DET if local address ID is invalid */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( ( ( FALSE == IsBroadcast_b ) && ( LocalAddrId_CtrlIdx_un.LocalAddrId_u8 >= TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ), RBA_ETHUDP_E_RX_INDICATION_API_ID , RBA_ETHUDP_E_INV_ARG )

    /* Report DET: RxData_pu8 is a null pointer */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( ( NULL_PTR == RxData_pu8 ), RBA_ETHUDP_E_RX_INDICATION_API_ID, RBA_ETHUDP_E_NULL_PTR )

    /* Report DET: Broadcasting not supported in Ipv6 */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( ( ( TRUE == IsBroadcast_b ) && ( PseudoHdr_pcst->IpDomainType_t == TCPIP_AF_INET6 ) ), RBA_ETHUDP_E_RX_INDICATION_API_ID , RBA_ETHUDP_E_INV_ARG )

    /* Initialize the flag indicating if received frame is valid */
    lRxFrameValid_b = FALSE;

    /* Check that RxDataLen_u16 is not less then Udp header length BEFORE reading data from buffer. */
    if( RxDataLen_u16 >= RBA_ETHUDP_HDRLENGTH )
    {
        /* Read source and destination port from the buffer */
        TcpIp_ReadU16( &(RxData_pu8[TCPIP_TP_SRCPORT_OFFSET]),  &lRxDataSrcUdpPort_u16 );
        TcpIp_ReadU16( &(RxData_pu8[TCPIP_TP_DESTPORT_OFFSET]), &lRxDataDestUdpPort_u16 );

        /* Obtain total UDP datagram length in number of bytes */
        TcpIp_ReadU16( &(RxData_pu8[RBA_ETHUDP_UDPFRAMELENGTH_UB]), &lUdpHdrTotLen_u16 );

        /* Drop the frame if, UDP datagram length is greater than the received frame length or UDP datagram length is less than UDP header length */
        if( ( lUdpHdrTotLen_u16 >= RBA_ETHUDP_HDRLENGTH ) && ( lUdpHdrTotLen_u16 <= RxDataLen_u16 ) )
        {
            /* Received Udp frame is valid */
            lRxFrameValid_b=TRUE;
        }
    }

    /* Report DET if RxFrame is not valid */
    RBA_ETHUDP_DET_REPORT_ERROR_NO_RET( (lRxFrameValid_b == FALSE), RBA_ETHUDP_E_RX_INDICATION_API_ID, RBA_ETHUDP_E_INV_ARG )

    if( lRxFrameValid_b == TRUE )
    {
        /* If the received frame is unicast, fetch local address properties from TcpIp */
        if( FALSE == IsBroadcast_b )
        {
            lRetVal_en =  TcpIp_GetLocalAddrIdProperties( LocalAddrId_CtrlIdx_un.LocalAddrId_u8, (&lLocalAddrConfig_pcst) );
        }
        else
        {
            lLocalAddrConfig_pcst=NULL_PTR; /* not applicable for Broadcast packets */
            lRetVal_en=E_OK;
        }

        if( lRetVal_en == E_OK )
        {
            /* Domain specific processing: Update the IP address and port of the remote host, check cksum,...*/
            switch( PseudoHdr_pcst->IpDomainType_t )
            {
#if (TCPIP_IPV4_ENABLED == STD_ON)
                case TCPIP_AF_INET:
                {
                    /* Pass the RxData_pu8 buffer WITH the UDP header length to the IPv4 specific processing function.
                     * The UDP header is needed here to validate the UDP checksum (if SW checksum is enabled).*/
                    rba_EthUdp_RxIndication_IPv4( PseudoHdr_pcst,RxData_pu8, lRxDataSrcUdpPort_u16, lRxDataDestUdpPort_u16,
                         lUdpHdrTotLen_u16, LocalAddrId_CtrlIdx_un, lLocalAddrConfig_pcst, IsBroadcast_b );
                }
                break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
                case TCPIP_AF_INET6:
                {
                    /* Pass the RxData_pu8 buffer WITH the UDP header length to the IPv6 specific processing function.
                     * The UDP header is needed here to validate the UDP checksum (is SW checksum is enabled).
                     * Therefore, the passed Udp packet length must include the header length as well. */
                    rba_EthUdp_RxIndication_IPv6( PseudoHdr_pcst, RxData_pu8, lRxDataSrcUdpPort_u16, lRxDataDestUdpPort_u16,
                        lUdpHdrTotLen_u16, LocalAddrId_CtrlIdx_un, lLocalAddrConfig_pcst );
                }
                break;
#endif

                default:
                {
                    /* Unknown domain. Report to DET. */
                    RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_RX_INDICATION_API_ID, RBA_ETHUDP_E_AFNOSUPPORT )
                    /* If DET is disabled, default statement will be empty. Nothing to do here. */
                }
                break;
            }
        }
    }
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/********************************************************************************************************************/
/* rba_EthUdp_RxIndication_IPv4() This is a helper function called by rba_EthUdp_RxIndication and it performs only  */
/*                          IPv4 specific processing steps before RX indication to upper layer.                     */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input parameters:                                                                                                */
/* PseudoHdr_pcst           Pointer to pseudo structure provided by IPv4. Contains destination IP, source IP        */
/*                          and IPv4 header length                                                                  */
/* RxData_pu8               Pointer to the received UDP datagram                                                    */
/* RxDataSrcUdpPort_u16     Source port of the received UDP datagram                                                */
/* RxDataDestUdpPort_u16    Destination port of the received UDP datagram                                           */
/* UdpBodyLen_u16           The whole UDP datagram length -> including the UDP header length                        */
/* LocalAddrId_CtrlIdx_un   Provides the local addrId or EthIf CtrlIdx (if broadcast ) of the received frame        */
/* LocalAddrConfig_pcst     Pointer to local IPv4 address configuration if received UDP datagram is of unicast type.*/
/*                          Can be NULL if received UDP datagram is of broadcast type.                              */
/* IsBroadcast_b            Indicates whether received frame is a broadcast frame or not                            */
/*                                                                                                                  */
/* Return type :            void                                                                                    */
/*                                                                                                                  */
/* Notes:                                                                                                           */
/*  Processing steps:                                                                                               */
/*  - Search for configured sockets that match the following conditions:                                            */
/*      1. If the socket state is BOUND                                                                             */
/*      2. Destination port mentioned in the frame matches with the local port of the socket                        */
/*      3a. If the received frame is unicast/multicast and localAddressId on which frame was received matches with  */
/*  localAddressId of the socket                                                                                    */
/*      3b. If the received frame is broadcast and EthIfCtrlIdx on which frame was received matches with            */
/*  EthIfCtrlIdx of the socket                                                                                      */
/*  - Callback EXCEPTION: Call DHCP layer callback if the packet matches DHCP port                                  */
/*  - Call the SoAd Callback for the socket found                                                                   */
/*  - Send ICMP port unreachable errors if no matching socket is found                                              */
/********************************************************************************************************************/
#if ( TCPIP_IPV4_ENABLED == STD_ON )
static void rba_EthUdp_RxIndication_IPv4(  const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                           uint8 * RxData_pu8,
                                           uint16 RxDataSrcUdpPort_u16,
                                           uint16 RxDataDestUdpPort_u16,
                                           uint16 UdpBodyLen_u16,
                                           TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                           const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst,
                                           boolean IsBroadcast_b )
{
    /* Local variable declaration */
    uint16                                      lSocketId_u16;
    TcpIp_SockAddrInetType                      lRemoteAddrInet_st;
    Std_ReturnType                              lRetVal_en;
    boolean                                     lLocalMatch_b;
    boolean                                     lChecksumAllowed_b;
    uint8                                       lEthIfCtrl_u8;
    uint16                                      lChecksumField_u16;
    boolean                                     lMatchingSocketFound_b;
    Std_ReturnType                              lOperationRetVal_en;

    /* Initially socket is not found */
    lMatchingSocketFound_b = FALSE;

    /* Update the IP address and port of the remote host which sent the data */
    lRemoteAddrInet_st.domain = PseudoHdr_pcst->IpDomainType_t;
    lRemoteAddrInet_st.port = RxDataSrcUdpPort_u16;
    lRemoteAddrInet_st.addr[0U] = PseudoHdr_pcst->RemoteIpAddr_un.IPv4Addr_u32;

    /* Get the Ethernet controller Index */
    lEthIfCtrl_u8 = (FALSE==IsBroadcast_b) ? (rba_EthUdp_LocalAddressToCtrlMap_cau8[LocalAddrId_CtrlIdx_un.LocalAddrId_u8]) : (LocalAddrId_CtrlIdx_un.EthIfCtrlIdx_u8);

    /* Call the checksum verifying function */
    lRetVal_en = rba_EthUdp_VerifyUdpCksum_IPv4( lEthIfCtrl_u8,
                                                 PseudoHdr_pcst,
                                                 RxData_pu8,
                                                 UdpBodyLen_u16,
                                                 &lChecksumField_u16 );

    if( lRetVal_en == E_OK )
    {
        /* Move the payload pointer AFTER validating the checksum for RX UDP packet. Payload without UDP and IP headers is passed to SoAd */
        RxData_pu8 = RxData_pu8 + RBA_ETHUDP_HDRLENGTH;

        /* Remove the UDP header length from the received data length before passing it to the upper layer */
        UdpBodyLen_u16 = (uint16)(UdpBodyLen_u16 - RBA_ETHUDP_HDRLENGTH);

        /* Find the socket matching to destination IP(LocalAddrId) and destination port. See above the conditions */
        for ( lSocketId_u16 = 0U; lSocketId_u16 < TCPIP_UDPSOCKETMAX; lSocketId_u16++ )
        {
            /* Check if RxIndication operation is allowed for execution */
            /* Operation is allowed only if there is no closing operation ongoing in other execution context */
            lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( lSocketId_u16 );

            /* If the activation of the ongoing operation is not permitted */
            if( lOperationRetVal_en == E_NOT_OK )
            {
                /* Continue to the next socket */
                continue;
            }

            /* Check if local address match */
            lLocalMatch_b = (FALSE == IsBroadcast_b) ? (LocalAddrId_CtrlIdx_un.LocalAddrId_u8 == rba_EthUdp_DynSockTbl_ast[lSocketId_u16].LocalAddrId_u8) :
                            (LocalAddrId_CtrlIdx_un.EthIfCtrlIdx_u8 == rba_EthUdp_LocalAddressToCtrlMap_cau8[rba_EthUdp_DynSockTbl_ast[lSocketId_u16].LocalAddrId_u8]);

            /* If operation is allowed and the socket is in BOUND state and local address match */
            if( (rba_EthUdp_DynSockTbl_ast[lSocketId_u16].SockState_en == RBA_ETHUDP_SOCK_BOUND) &&    /* Check for condition 1 as mentioned above  */
                (RxDataDestUdpPort_u16 == rba_EthUdp_DynSockTbl_ast[lSocketId_u16].LocalPort_u16) &&   /* Check for condition 2 as mentioned above  */
                (lLocalMatch_b != FALSE) )                                          /* Check if condition 3a and 3b as mentioned above are satisfied */
            {
                /* Socket found -> this is a success criteria! */
                lMatchingSocketFound_b=TRUE;

                /* SockChecksumEnabled_u8 optionally controls whether UDP datagrams without checksums should be discarded or passed to the application. */
                /* UDP Datagrams containing a zero checksum shall be forwarded only if SockChecksumEnabled_u8 is STD_OFF */
                lChecksumAllowed_b = (lChecksumField_u16 != 0U) ? (TRUE) : (STD_OFF == rba_EthUdp_DynSockTbl_ast[lSocketId_u16].SockChecksumEnabled_u8) ;

                /* Call upper layer RxIndication API if it is configured for the socket owner. */
                if( (TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[lSocketId_u16].UL_SockOwnerIdx_u8].Up_RxIndication_pfct != NULL_PTR) &&
                     (lChecksumAllowed_b != FALSE) )
                {
                    /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrType* to be used in domain agnostic function call. It is safe because of the domain switch. */
                    TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[lSocketId_u16].UL_SockOwnerIdx_u8].Up_RxIndication_pfct( (lSocketId_u16 + TCPIP_UDP_SOCKET_OFFSET),
                                                                                                                                                   ( (TcpIp_SockAddrType*) (&lRemoteAddrInet_st) ),
                                                                                                                                                   RxData_pu8,
                                                                                                                                                   UdpBodyLen_u16 );
                }

                /* If the received frame is unicast there can be only one UL_RxIndication() possible and loop can be terminated. */
                /* In case of multicast and broadcast, frames shall be forwarded to ALL sockets matching destination port and localAddressId/EthIfCtrlIdx */
                /* DD: No multiple indications possible if DHCP -> so exit the search loop. */
                if( ((NULL_PTR != LocalAddrConfig_pcst) && (TCPIP_UNICAST == LocalAddrConfig_pcst->IpAddressType_en))
#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
                     || (rba_EthUdp_DynSockTbl_ast[lSocketId_u16].UL_SockOwnerIdx_u8 == SOCK_OWNER_DHCP)
#endif
                  )
                {
                    /* Operation not ongoing anymore */
                    rba_EthUdp_ReleaseOngoingOperation( lSocketId_u16 );
                    break; /* Unicast or DHCP -> processing is finished */
                }
            }

            /* Operation not ongoing anymore */
            rba_EthUdp_ReleaseOngoingOperation( lSocketId_u16 );
        }

        /* If matching socket was not found */
        if( lMatchingSocketFound_b == FALSE )
        {
#if ( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
            /* Increment counter if get/reset measurement data feature is enabled */
            /* If no matching socket is found then increment counter */
            rba_EthUdp_IncMeasurementData( &rba_EthUdp_InvalidIpPortDropCnt_u32 );
#endif
#if ( TCPIP_ICMP_ENABLED == STD_ON )
            /* Send ICMP port unreachable frame if there is no socket match and the received frame is unicast */
            if( (NULL_PTR != LocalAddrConfig_pcst) && (TCPIP_UNICAST == LocalAddrConfig_pcst->IpAddressType_en) ) /* Valid unicast */
            {
                rba_EthUdp_SendIcmpPortNotReachable( PseudoHdr_pcst, RxData_pu8, LocalAddrId_CtrlIdx_un, &lRemoteAddrInet_st );
            }
#endif
        }
    }
}
#endif

/********************************************************************************************************************/
/* rba_EthUdp_SendIcmpPortNotReachable() This is a helper function called from rba_EthUdp_RxIndication_IPv4, which  */
/*                          sends an ICMP port unreachable frame if there is no socket match and the received frame */
/*                          is unicast.                                                                             */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* PseudoHdr_pcst           Pointer to pseudo structure provided by IPv4. Contains destination IP, source IP        */
/*                          and IPv4 header length                                                                  */
/* RxData_pu8               Pointer to the received UDP datagram                                                    */
/* LocalAddrId_CtrlIdx_un   Provides the local addrId or EthIf CtrlIdx (if broadcast ) of the received frame        */
/* RemoteAddrInet_pst       Pointer to structure containing the remote address info of the UDP frame transmitter    */
/*                                                                                                                  */
/* Return type :            void                                                                                    */
/********************************************************************************************************************/
#if ( TCPIP_ICMP_ENABLED == STD_ON )
static void rba_EthUdp_SendIcmpPortNotReachable( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                 const uint8 * RxData_pcu8,
                                                 TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                                 const TcpIp_SockAddrInetType * RemoteAddrInet_pst )
{
    /* Local variable declaration and initialization */
    uint16 lUdpHdrTotLen_u16 = ( RBA_ETHICMP_DEST_UNREACH_DATASIZE + PseudoHdr_pcst->IPHdrLen_u16 );

    /* ICMP payload/message contains IP header followed by 8 bytes of UDP/TCP header */
    /* Move the pointer back to IP header */
    RxData_pcu8 = ( RxData_pcu8 - ( PseudoHdr_pcst->IPHdrLen_u16 + RBA_ETHUDP_HDRLENGTH ) );

    (void)rba_EthIcmp_Transmit( LocalAddrId_CtrlIdx_un.LocalAddrId_u8,
                        RemoteAddrInet_pst,
                        0U,     /* TTL = 0 indicates default value will be used */
                        (uint8)RBA_ETHICMP_DEST_UNREACH_E,
                        (uint8)RBA_ETHICMP_DEST_UNREACH_PORT_E,
                        lUdpHdrTotLen_u16,
                        RxData_pcu8 );
}
#endif

/********************************************************************************************************************/
/* rba_EthUdp_RxIndication_IPv6() This is a helper function called by rba_EthUdp_RxIndication and it performs only  */
/*                          IPv6 specific processing steps before RX indication to upper layer.                     */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input parameters:                                                                                                */
/* PseudoHdr_pcst           Pointer to pseudo structure provided by IPv6. Contains destination IP, source IP        */
/*                          and IPv6 header length                                                                  */
/* RxData_pu8               Pointer to the received UDP datagram                                                    */
/* RxDataSrcUdpPort_u16     Source port of the received UDP datagram                                                */
/* RxDataDestUdpPort_u16    Destination port of the received UDP datagram                                           */
/* UdpBodyLen_u16           The whole UDP datagram length -> including the UDP header length                        */
/* LocalAddrId_CtrlIdx_un   Provides the local addrId or EthIf CtrlIdx (if broadcast ) of the received frame        */
/* LocalAddrConfig_pcst     Pointer to local IPv4 address configuration if received UDP datagram is of unicast type.*/
/*                          Can be NULL if received UDP datagram is of broadcast type.                              */
/*                                                                                                                  */
/* Return type :            void                                                                                    */
/*                                                                                                                  */
/* Notes:                                                                                                           */
/*  Processing steps:                                                                                               */
/*  - Verifies the checksum of the received UDP frame                                                               */
/*    Note: UDP handles the checksum validation only for IPv6 Rx frames (due to UDP protocol specific processing for*/
/*          IPv6). The checksum validation for IPv6 Tx is handled in the IPv6 layer for better performance.         */
/*  - Search for configured sockets that match the following conditions:                                            */
/*      1. If the socket state is BOUND                                                                             */
/*      2. Destination port mentioned in the frame matches with the local port of the socket                        */
/*      3a. If the received frame is unicast/multicast and localAddressId on which frame was received matches with  */
/*  localAddressId of the socket                                                                                    */
/*  - Call the SoAd Callback for the socket found                                                                   */
/*                                                                                                                  */
/*  Differences from IPv4 specific processing:                                                                      */
/*  1. It does not support DHCP Rx indication call                                                                  */
/*  2. It does not reply with ICMP frame in case of port unreachable                                                */
/********************************************************************************************************************/
#if ( TCPIP_IPV6_ENABLED == STD_ON )
static void rba_EthUdp_RxIndication_IPv6( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                          uint8 * RxData_pu8,
                                          uint16 RxDataSrcUdpPort_u16,
                                          uint16 RxDataDestUdpPort_u16,
                                          uint16 UdpBodyLen_u16,
                                          TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                          const TcpIp_LocalAddrConfig_to * LocalAddrConfig_pcst )
{
    /* Local variable declaration */
    uint8                                       lIdx_u8;
    TcpIp_SockAddrInet6Type                     lRemoteAddrInet6_st;
    boolean                                     lMatchingSocketFound_b;
    uint16                                      lSocketId_u16;
    Std_ReturnType                              lOperationRetVal_en;
#if ( RBA_ETHUDP_SW_CHKSUM == STD_ON )
    Std_ReturnType                              lRetVal_en;
    uint8                                       lEthIfCtrl_u8;
#endif

    /* Update the IP address and port of the remote host which sent the data */
    lRemoteAddrInet6_st.domain = PseudoHdr_pcst->IpDomainType_t;
    lRemoteAddrInet6_st.port = RxDataSrcUdpPort_u16;
    for (lIdx_u8 = 0U; lIdx_u8 < 4U; lIdx_u8++) /* Address copy loop */
    {
        lRemoteAddrInet6_st.addr[lIdx_u8] = PseudoHdr_pcst->RemoteIpAddr_un.IPv6Addr_pcu32[lIdx_u8];
    }

    /* Actual processing: checksum, match socket, call upper layers, send error frames (if applicable). */
#if ( RBA_ETHUDP_SW_CHKSUM == STD_ON ) /* Verify checksum (only if it is not already performed by the HW) */

    /* Get the Ethernet controller Index */
    lEthIfCtrl_u8 = rba_EthUdp_LocalAddressToCtrlMap_cau8[LocalAddrId_CtrlIdx_un.LocalAddrId_u8];

    /* DD: Checksum for IPv6 Rx frames is performed by UDP component */
    lRetVal_en = rba_EthUdp_VerifyUdpCksum_IPv6(lEthIfCtrl_u8, PseudoHdr_pcst, RxData_pu8, UdpBodyLen_u16 );

    if( lRetVal_en == E_OK )
#endif
    {
        /* Move the payload pointer AFTER validating the checksum for RX UDP packet. Payload without UDP and IP headers is passed to SoAd */
        RxData_pu8 = RxData_pu8 + RBA_ETHUDP_HDRLENGTH;

        /* Remove the UDP header length from the received data length before passing it to the upper layer */
        UdpBodyLen_u16 = (uint16)( UdpBodyLen_u16 - RBA_ETHUDP_HDRLENGTH);

        /* Initially socket is not found */
        lMatchingSocketFound_b = FALSE;

        /* Find the socket matching to destination IP(LocalAddrId) and destination port. See above the conditions */
        for ( lSocketId_u16 = 0U; lSocketId_u16 < TCPIP_UDPSOCKETMAX; lSocketId_u16++ )
        {
            /* Check if RxIndication operation is allowed for execution */
            /* Operation is allowed only if there is no closing operation ongoing in other execution context */
            lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( lSocketId_u16 );

            /* If the activation of the ongoing operation is not permitted */
            if(lOperationRetVal_en == E_NOT_OK)
            {
                /* Continue to the next socket */
                continue;
            }

            /* If operation is allowed and the socket is in BOUND state and local address match */
            if( (rba_EthUdp_DynSockTbl_ast[lSocketId_u16].SockState_en == RBA_ETHUDP_SOCK_BOUND) &&                   /* Check for condition 1 as mentioned above  */
                (RxDataDestUdpPort_u16 == rba_EthUdp_DynSockTbl_ast[lSocketId_u16].LocalPort_u16) &&                  /* Check for condition 2 as mentioned above  */
                (LocalAddrId_CtrlIdx_un.LocalAddrId_u8 == rba_EthUdp_DynSockTbl_ast[lSocketId_u16].LocalAddrId_u8) )  /* Check for condition 3a as mentioned above */
            {
                /* Socket found -> this is a success criteria! */
                lMatchingSocketFound_b = TRUE;

                /* Call upper layer RxIndication API if it is configured for the socket owner. */
                if( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[lSocketId_u16].UL_SockOwnerIdx_u8].Up_RxIndication_pfct != NULL_PTR )
                {
                    /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrType* to be used in domain agnostic function call. It is safe because of the domain switch. */
                    TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[lSocketId_u16].UL_SockOwnerIdx_u8].Up_RxIndication_pfct( ( lSocketId_u16 + TCPIP_UDP_SOCKET_OFFSET ),
                                                                                                                                                   ( (TcpIp_SockAddrType*) (&lRemoteAddrInet6_st) ),
                                                                                                                                                   RxData_pu8,
                                                                                                                                                   UdpBodyLen_u16 );
                }

                /* If the received frame is unicast there can be only one UL_RxIndication() possible and loop can be terminated. */
                /* In case of multicast and broadcast, frames shall be forwarded to ALL sockets matching destination port  and localAddressId/EthIfCtrlIdx */
                if( ( NULL_PTR != LocalAddrConfig_pcst ) && ( TCPIP_UNICAST == LocalAddrConfig_pcst->IpAddressType_en ) )
                {
                    /* Operation not ongoing anymore */
                    rba_EthUdp_ReleaseOngoingOperation( lSocketId_u16 );
                    break; /* Unicast -> processing is finished */
                }
            }

            /* Operation not ongoing anymore */
            rba_EthUdp_ReleaseOngoingOperation( lSocketId_u16 );
        }

        /* If matching socket was not found */
        if( lMatchingSocketFound_b == FALSE )
        {
#if ( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
            /* Increment counter if get/reset measurement data feature is enabled */
            /* If no matching socket is found then increment counter */
            rba_EthUdp_IncMeasurementData( &rba_EthUdp_InvalidIpPortDropCnt_u32 );
#endif
#if ( ( TCPIP_ICMPV6_ENABLED == STD_ON ) && ( TCPIP_ICMPV6_MSG_DEST_UNREACH_ENABLE == STD_ON ) )
            /* Send ICMPV6 Destination Unreachable frame if there is no socket match and the received frame is unicast */
            if( (NULL_PTR != LocalAddrConfig_pcst) && (TCPIP_UNICAST == LocalAddrConfig_pcst->IpAddressType_en) ) /* valid unicast */
            {
                rba_EthUdp_SendIcmpV6PortNotReachable( PseudoHdr_pcst, RxData_pu8, LocalAddrId_CtrlIdx_un, &lRemoteAddrInet6_st);
            }
#endif
        }
    }
}
#endif

/********************************************************************************************************************/
/* rba_EthUdp_SendIcmpV6PortNotReachable() This is a helper function called from rba_EthUdp_RxIndication_IPv6, which*/
/*                          sends an ICMPV6 Destination Unreachable frame if there is no socket match and the       */
/*                          received frame is unicast.                                                              */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* PseudoHdr_pcst           Pointer to pseudo structure provided by IPv6. Contains destination IP, source IP        */
/*                          and IPv6 header length                                                                  */
/* RxData_pu8               Pointer to the received UDP datagram                                                    */
/* LocalAddrId_CtrlIdx_un   Provides the local addrId or EthIf CtrlIdx (if broadcast ) of the received frame        */
/* RemoteAddrInet6_pst      Pointer to structure containing the remote address info of the UDP frame transmitter    */
/*                                                                                                                  */
/* Return type :            void                                                                                    */
/********************************************************************************************************************/
#if ( ( TCPIP_ICMPV6_ENABLED == STD_ON) && ( TCPIP_ICMPV6_MSG_DEST_UNREACH_ENABLE == STD_ON  )  )
static void rba_EthUdp_SendIcmpV6PortNotReachable( const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                   const uint8 * RxData_pcu8,
                                                   TcpIp_LocalAddrId_EthIfCtrl_tun LocalAddrId_CtrlIdx_un,
                                                   const TcpIp_SockAddrInet6Type * RemoteAddrInet6_pst)
{
    /* Local variable declaration */
    Icmpv6_Header_tst        IcmpV6_Header_st;

    /* ICMPV6 payload/message contains IPV6 header followed by 8 bytes of UDP header */
    /* Move the pointer back to IPV6 header */
    RxData_pcu8 = RxData_pcu8 - ( ( PseudoHdr_pcst->IPHdrLen_u16 ) + RBA_ETHUDP_HDRLENGTH);
    IcmpV6_Header_st.HopLimit = 0U; /* Hop limit will be taken from IcmpV6 configuration */
    IcmpV6_Header_st.Type = RBA_ETHICMPV6_DEST_UNREACH_E;
    IcmpV6_Header_st.Code = (uint8)RBA_ETHICMPV6_DEST_UNREACH_PORT_UNREACHABLE_E;
    IcmpV6_Header_st.RemoteAddr_pst[0] = RemoteAddrInet6_pst->addr[0];
    IcmpV6_Header_st.RemoteAddr_pst[1] = RemoteAddrInet6_pst->addr[1];
    IcmpV6_Header_st.RemoteAddr_pst[2] = RemoteAddrInet6_pst->addr[2];
    IcmpV6_Header_st.RemoteAddr_pst[3] = RemoteAddrInet6_pst->addr[3];

    (void)rba_EthIcmpV6_Transmit( LocalAddrId_CtrlIdx_un.LocalAddrId_u8,
                        &IcmpV6_Header_st,
                        (PseudoHdr_pcst->IPHdrLen_u16 + RBA_ETHUDP_HDRLENGTH ),
                        RxData_pcu8 );
}
#endif

/********************************************************************************************************************/
/* rba_EthUdp_IncMeasurementData()  Increments the measurement data passed as inout parameter if it has not         */
/*                                  exceeded TCPIP_MAXUINT32.                                                       */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* InOut Parameters :                                                                                               */
/* MeasurementData_pu32     Pointer to the measurement data which shall be incrmented.                              */
/*                                                                                                                  */
/* Return type :            None                                                                                    */
/********************************************************************************************************************/
#if( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
LOCAL_INLINE void rba_EthUdp_IncMeasurementData( uint32 * MeasurementData_pu32 )
{
    /* Enter critical section */
    /* (Incrementation operation is not atomic) */
    SchM_Enter_rba_EthUdp_ExclusiveAreaMeasurementData();

    /* All measurement data which counts data shall not overrun. */
    if(( *MeasurementData_pu32 ) < TCPIP_MAXUINT32 )
    {
        /* Increment the counter */
        (*MeasurementData_pu32)++;
    }

    /* Exit critical section */
    SchM_Exit_rba_EthUdp_ExclusiveAreaMeasurementData();
}
#endif

/********************************************************************************************************************/
/* rba_EthUdp_VerifyUdpCksum_IPv4() This is a helper function called from rba_EthUdp_RxIndication, which            */
/* verifies the checksum of the received UDP datagram.                                                              */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/* Input Parameters :                                                                                               */
/* EthIfCtrl_u8             Ethernet controller Index.                                                              */
/* PseudoHdr_pcst           Pointer to pseudo structure provided by IPv4. Contains destination IP, source IP        */
/*                          and IPv4 header length                                                                  */
/* RxData_pcu8              Pointer to the received UDP datagram                                                    */
/* BufLength_u16            UDP datagram length                                                                     */
/*                                                                                                                  */
/* Out Parameters :                                                                                                 */
/* ChecksumField_pu16       The checksum field value inside the frame.                                              */
/*                                                                                                                  */
/* Return type :            Std_ReturnType                                                                          */
/*                          - E_OK, if checksum compution is done.                                                  */
/*                          - E_NOT_OK,  if checksum compution is failed.                                           */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (TCPIP_IPV4_ENABLED == STD_ON)
static Std_ReturnType rba_EthUdp_VerifyUdpCksum_IPv4( uint8 EthIfCtrl_u8,
                                                      const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                      const uint8 * RxData_pcu8,
                                                      uint16 BufLength_u16,
                                                      uint16 * ChecksumField_pu16)
{
    /* Local variables declaration */
    Std_ReturnType                 lRetVal_en;
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
    uint16                         lUdpChksum_u16;
#endif

#if(RBA_ETHUDP_SW_CHKSUM == STD_OFF)
    /* EthIfCtrl_u8 is unused if RBA_ETHUDP_SW_CHKSUM is OFF. */
    (void)EthIfCtrl_u8;
#endif

    /* Local variables Initialization */
    lRetVal_en = E_NOT_OK;

    /* Read the Udp checksum value in the received buffer */
    TcpIp_ReadU16( &(RxData_pcu8[RBA_ETHUDP_UDPCHKSUM_UB]), &(*ChecksumField_pu16));

    /* When UDP packets are originated by an IPv4 node, the UDP checksum is optional. */
    /* if the checksum value in the datagram is non-zero, checksumming is enabled and verification has to be done */
    if( 0U != (*ChecksumField_pu16) )
    {
        /* Checksum shall be verified by software if */
        /* 1. The software checksum verification for dedicated lEthIfCtrl_u8 is not offloaded to the driver OR  */
        /* 2. The received frame is fragmented */
        if(
#if(RBA_ETHUDP_SW_CHKSUM == STD_ON)
            (rba_EthUdp_ChecksumOffloading_cab[EthIfCtrl_u8] == FALSE) ||
#endif
            (PseudoHdr_pcst->IsRxFragmented_b != FALSE) )
        {
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
            /* Initialize the checksum value output */
            lUdpChksum_u16 = 0U;

            /* MR12 DIR 1.1 VIOLATION: The input parameter of TcpIp_OnesComplChkSum() is declared as (void*) for faster implmentation of checksum calculation. Low Risk */
            lRetVal_en = TcpIp_OnesComplChkSum( RxData_pcu8,
                                                PseudoHdr_pcst,
                                                BufLength_u16,
                                                &lUdpChksum_u16 );

            /* If the checksum does not evaluate to zero, return E_NOT_OK indicating checksum verification failed */
            if((0U != lUdpChksum_u16) && (E_OK == lRetVal_en))
            {
                lRetVal_en = E_NOT_OK;
            }
#else
            /* RxData_pcu8 and BufLength_u16 is unused if TCPIP_ONES_COMPL_CHKSUM_ENABLED is OFF. */
            (void)RxData_pcu8;
            (void)BufLength_u16;

            /* Report DET: If TcpIp_OnesComplChkSum() is not available */
            RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_RX_INDICATION_API_ID, RBA_ETHUDP_E_INV_ARG )
            lRetVal_en = E_NOT_OK;
#endif
        }
        else
        {
            /* Set return value to E_OK as checksum is calculated by H/W and frame is not reassembled (no need to recalculate checksum again in this case) */
            lRetVal_en = E_OK;
        }
    }
    else
    {
        /* As per RFC 1122, the UDP checksum may be optional, UDP datagrams without checksums should be not be discarded and passed to the application */
        lRetVal_en = E_OK;
    }

    return lRetVal_en;
}
#endif

/********************************************************************************************************************/
/* rba_EthUdp_VerifyUdpCksum_IPv6() This is a helper function called from rba_EthUdp_RxIndication_IPv6, which       */
/*                          verifies the checksum of the received UDP datagram.                                     */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8           Local address index to verify checksum only if it is not already performed by the HW    */
/* PseudoHdr_pcst           Pointer to pseudo structure provided by IPv6. Contains destination IP, source IP        */
/*                          and IPv6 header length                                                                  */
/* RxData_pcu8              Pointer to the received UDP datagram                                                    */
/* UdpBodyLen_u16           UDP datagram length                                                                     */
/*                                                                                                                  */
/* Return type :            Std_ReturnType                                                                          */
/*                          - E_OK, if checksum is correct                                                          */
/*                          - E_NOT_OK, if checksum is wrong or it is not set (UDP checksum is mandatory for IPv6)  */
/*                                                                                                                  */
/* Note :   UDP handles the checksum validation only for IPv6 Rx frames (due to UDP protocol specific processing for*/
/*          IPv6). The checksum validation for IPv6 Tx is handled in the IPv6 layer for better performance.         */
/********************************************************************************************************************/
#if ( ( RBA_ETHUDP_SW_CHKSUM == STD_ON ) && ( TCPIP_IPV6_ENABLED == STD_ON ) )
static Std_ReturnType  rba_EthUdp_VerifyUdpCksum_IPv6 (uint8 EthIfCtrl_u8,
                                               const  TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                               const  uint8   * RxData_pcu8,
                                               uint16 UdpBodyLen_u16 )
{
    /* Local variables declaration    */
    uint16            lTPLayerChksum_u16;
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
    uint16            lUdpChksum_u16;
#endif
    Std_ReturnType    lRetVal_en;

    /* Local variables Initialization */
    lRetVal_en = E_NOT_OK;

    /* Verify checksum only if it is not already performed by the HW */
    if( rba_EthUdp_ChecksumOffloading_cab[EthIfCtrl_u8] == FALSE )
    {
        /* Read the Udp checksum value in the received buffer */
        TcpIp_ReadU16( &(RxData_pcu8[RBA_ETHUDP_UDPCHKSUM_UB]), &lTPLayerChksum_u16 );

        /* The Udp checksum validation is implemented according to RFC 2460 specifications:
         * "Unlike IPv4, when UDP packets are originated by an IPv6 node, the UDP checksum is not optional."
         * "IPv6 receivers must discard UDP packets containing a zero checksum, and should log the error." */
        if( lTPLayerChksum_u16 != 0U )
        {
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
            /* MR12 DIR 1.1 VIOLATION: The input parameter of TcpIp_OnesComplChkSum() is declared as (void*) for faster implmentation of checksum calculation. Low Risk */
            lRetVal_en = TcpIp_OnesComplChkSum( RxData_pcu8, PseudoHdr_pcst, UdpBodyLen_u16, &lUdpChksum_u16 );
            if( ( lRetVal_en == E_OK ) && ( 0U != lUdpChksum_u16 ) )
            {
                lRetVal_en=E_NOT_OK;
            }
#else
            /* PseudoHdr_pcst and UdpBodyLen_u16 is unused if TCPIP_ONES_COMPL_CHKSUM_ENABLED is OFF. */
            (void)PseudoHdr_pcst;
            (void)UdpBodyLen_u16;

            /* Report DET: If TcpIp_OnesComplChkSum() is not available */
            RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_RX_INDICATION_API_ID, RBA_ETHUDP_E_INV_ARG )
            lRetVal_en = E_NOT_OK;
#endif
        }
        else
        {
            /* The rba_EthUdp_RxIndication() API has no return value. Therefore report at least to DET that the checksum is invalid (if DET is enabled). */
            RBA_ETHUDP_DET_REPORT_ERROR_TRUE_RET_VALUE( RBA_ETHUDP_E_RX_INDICATION_API_ID, RBA_ETHUDP_E_ZERO_CHECKSUM_IPV6, E_NOT_OK )
        }
    }
    else
    {
        /* Checksum verification is performed by the HW */
        lRetVal_en = E_OK;
    }

    return lRetVal_en;
}
#endif

#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#endif /* #if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */
