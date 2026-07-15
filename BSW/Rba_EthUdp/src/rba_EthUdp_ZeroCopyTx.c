
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )
#include "rba_EthUdp.h"

#include "TcpIp_Lib.h"
#include "TcpIp_Prv.h"

#if ( TCPIP_IPV4_ENABLED == STD_ON )
#include "rba_EthIPv4.h"
#endif

#include "rba_EthUdp_Prv.h"

/* File is compiled only if ZeroCopy feature is enabled */
#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
#include "rba_EthUdp_Cfg_ZeroCopyTx.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define RBA_ETHUDP_START_SEC_CODE_FAST
#include "rba_EthUdp_MemMap.h"
#if (( TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON) && (TCPIP_IPV4_ENABLED == STD_ON))

LOCAL_INLINE Std_ReturnType rba_EthUdp_GetZeroCopyChecksum_IPv4( TcpIp_SocketIdType SocketId_u16,
                                                                 const TcpIp_SockAddrInetType * RemoteAddrIPv4_pcst,
                                                                 const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                                 uint16 * ComputedChecksum_pu16 );
#endif
#define RBA_ETHUDP_STOP_SEC_CODE_FAST
#include "rba_EthUdp_MemMap.h"

/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHUDP_START_SEC_CODE_FAST
#include "rba_EthUdp_MemMap.h"

/********************************************************************************************************************/
/* rba_EthUdp_CheckFragmentation() This API returns the number of IP fragments if fragmentation will occur          */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId                              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16             Socket id on which data is to be transmitted                                            */
/* DataLength_u16           Length of data that will be request for transmission                                    */
/*                                                                                                                  */
/* Output parameters:                                                                                               */
/* NumberOfIpFragments_pu8  The number of IP fragments which will be formed by IP layer during transmission         */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* Std_ReturnType   - E_OK or E_NOT_OK - Result of operation                                                        */
/********************************************************************************************************************/
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
Std_ReturnType rba_EthUdp_CheckFragmentation( TcpIp_SocketIdType SocketId_u16,
                                              uint16 DataLength_u16,
                                              uint8 * NumberOfIpFragments_pu8 )
{
    /* Declare local variables */
    Std_ReturnType                      lRetValStdType_en;
    Std_ReturnType                      lOperationRetVal_en;

    /* Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_CHECKFRAGMENTATION_API_ID, RBA_ETHUDP_E_NOTINIT, E_NOT_OK )

    /* Check whether socket handle is a plausible value */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( SocketId_u16 >= TCPIP_UDPSOCKETMAX ), RBA_ETHUDP_E_CHECKFRAGMENTATION_API_ID, RBA_ETHUDP_E_INV_ARG, E_NOT_OK )

    /* Report DET if DataLength_u16 is greater than the maximum allowed Udp payload length */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE((DataLength_u16 > (uint16)(TCPIP_MAXUINT16 - RBA_ETHUDP_HDRLENGTH)), RBA_ETHUDP_E_CHECKFRAGMENTATION_API_ID, RBA_ETHUDP_E_INV_ARG, E_NOT_OK)

    /* Local variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* Check if CheckFragmentation operation is allowed for execution */
    /* Operation is allowed only if there is no closing operation ongoing in other execution context */
    lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( SocketId_u16 );

    /* Check whether the ChangeParameter operation is allowed */
    if( lOperationRetVal_en == E_OK )
    {
        /* Check whether the requested socket state is RBA_ETHUDP_SOCK_BOUND */
        if( RBA_ETHUDP_SOCK_BOUND == rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en )
        {
#if ( TCPIP_IPV4_ENABLED == STD_ON )
            if( rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 == TCPIP_AF_INET )
            {
                /* Forward call to IPv4 layer */
                lRetValStdType_en = rba_EthIPv4_CheckFragmentation( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,
                                                                   (DataLength_u16 + RBA_ETHUDP_HDRLENGTH),
                                                                   NumberOfIpFragments_pu8 );
            }
            else
#endif
            {
                /* Unknown domain. Throw DET report and break execution. */
                /* ZeroCopy is only implemented for INET domain */
                RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_CHECKFRAGMENTATION_API_ID, RBA_ETHUDP_E_NOPROTOOPT)
                /* If DET is disabled, default statement will be empty. Nothing to do here. */
            }
        }

        /* Operation not ongoing anymore */
        rba_EthUdp_ReleaseOngoingOperation( SocketId_u16 );
    }

    return lRetValStdType_en;
}
#endif

/********************************************************************************************************************/
/* rba_EthUdp_ZeroCopyFillHeader()  This API prefill the reusable part of the Udp header                            */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId                              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16             Socket id on which data is to be transmitted                                            */
/* RemoteAddr_pcst           Pointer to a structure containing remote IP address and remote port                    */
/* ZeroCopyArg_pcst         ZeroCopy argument containing the allocated headers and the pointer to the UL data to be */
/*                          transmitted                                                                             */
/*                                                                                                                  */
/* Output parameters:                                                                                               */
/* UL_SockOwnerIdx_pu8      Index of the Udp socket owner                                                           */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* TcpIp_ReturnType      TCPIP_OK                  : UDP filling successful                                         */
/*                       TCPIP_E_NOT_OK            : UDP filling failed (general failure)                           */
/*                       TCPIP_E_ARP_CACHE_MISS    : UDP filling failed because of ARP cache miss)                  */
/********************************************************************************************************************/
/* HIS METRIC PATH VIOLATION IN rba_EthUdp_ZeroCopyFillHeader: The number of path is increased due to the consecutive if blocks which are required for the execution of various DET checks. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
TcpIp_ReturnType rba_EthUdp_ZeroCopyFillHeader ( TcpIp_SocketIdType SocketId_u16,
                                                 const TcpIp_SockAddrType * RemoteAddr_pcst,
                                                 const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                 uint8 * UL_SockOwnerIdx_pu8 )
{
    /* Declare local variables */
    TcpIp_ReturnType                    lTcpIpRetVal_en;
    Std_ReturnType                      lOperationRetVal_en;
#if ( TCPIP_IPV4_ENABLED == STD_ON )
    TcpIp_IpHeader_tst                  lIpHeader_st;
    const TcpIp_SockAddrInetType*       lRemoteAdrIPv4_pcst;
#endif

    /* Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_NOTINIT, TCPIP_E_NOT_OK )

    /* Check whether socket handle is a possible value */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( SocketId_u16 >= TCPIP_UDPSOCKETMAX ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Report DET if RemoteAddr_pcst is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == RemoteAddr_pcst ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pcst == NULL_PTR), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET if ZeroCopyAllocatedHeaders_ast is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET: At least 1 header block should be passed by TcpIp (Reuse) */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 < 1U ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Report DET if HeaderPartBuffer_pu8 for Reuse header is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8 ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET: In STATIC Mode, the length of the Reuse header need to be big enough so that Udp Src, Dest port, Length and Checksum an be written */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 == 1U) && ( ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartLen_u8 < (RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_HDR_OFFSET + RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_HDR_LEN) ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Report DET: In DYNAMIC Mode, the length of the Reuse header need to be big enough so that Udp Src and Dest port can be written */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 1U) && ( ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartLen_u8 < (RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_HDR_OFFSET+RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_HDR_LEN) ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Report DET if ZeroCopyData_pcst is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyData_pcst ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET if DataLength_u16 is greater than the maximum allowed Udp payload length */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE((ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16 > (uint16)(TCPIP_MAXUINT16 - RBA_ETHUDP_HDRLENGTH)), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK)

    /* Local variable initialization */
    lTcpIpRetVal_en = TCPIP_E_NOT_OK;

    /* Check if ZeroCopyFillHeader operation is allowed for execution */
    /* Operation is allowed only if there is no closing operation ongoing in other execution context */
    lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( SocketId_u16 );

    /* Check whether the ChangeParameter operation is allowed */
    if( lOperationRetVal_en == E_OK )
    {
        /* Report DET: Local Domain not matching remote domain */
        RBA_ETHUDP_DET_REPORT_ERROR_NO_RET( ( RemoteAddr_pcst->domain != rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 ), RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_INV_ARG )

        /* Check whether the requested socket state is RBA_ETHUDP_SOCK_BOUND and local domain matching remote domain */
        if( (RBA_ETHUDP_SOCK_BOUND == rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en) &&
            (RemoteAddr_pcst->domain == rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32) )
        {

#if ( TCPIP_IPV4_ENABLED == STD_ON )

            /* Common code: provide protocol and TTL/Hop Limit information to rba_EthIP layer. */
            lIpHeader_st.ProtoType_en = TCPIP_IPPROTO_UDP;
            lIpHeader_st.Ttl_u8 = rba_EthUdp_Config_pco->Ttl_u8; /* The TTL field specific for IPv4 is actually Hop Limit in the IPv6 protocol. */
            lIpHeader_st.SockOwner_u8 = rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8;  /* Update the socket owner information for IPv4 layer */
            lIpHeader_st.FlowLabel_u32 = 0UL;           /* Set to 0 as flow label is an IPv6 field and it is not used in IPv4 header */
            lIpHeader_st.DiffServiceCodepoint_u8 = rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockDiffServiceCodePoint_u8;  /* Update DSCP value in the socket table. */

            if( rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 == TCPIP_AF_INET )
            {
                /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 data. It is safe because of the domain check before. */
                lRemoteAdrIPv4_pcst = (const TcpIp_SockAddrInetType*) RemoteAddr_pcst;

                /* -------------------------------------- */
                /* Fill reuse header                      */
                /* -------------------------------------- */
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
                if (ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 1U)
                {
                    /* Fill the Src port in the reuse header provided by TcpIp */
                    TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_SRCPORT_OFFSET])),     /* Allocated reuse header is at index 0 */
                                    (rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalPort_u16) );

                    /* Fill the Dest port in the reuse header */
                    TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_DESTPORT_OFFSET])),     /* Allocated reuse header is at index 0 */
                                    (lRemoteAdrIPv4_pcst->port) );
                }
                else
#endif
                {
#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
                    /* Fill the Src port in the reuse header provided by TcpIp */
                    TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_SRCPORT_OFFSET])),     /* Allocated reuse header is at index 0 */
                                    (rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalPort_u16) );

                    /* Fill the Dest port in the reuse header */
                    TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_DESTPORT_OFFSET])),     /* Allocated reuse header is at index 0 */
                                    (lRemoteAdrIPv4_pcst->port) );

                    /* Fill the Length in the reuse header */
                    TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_LENGTH_OFFSET])),       /* Allocated reuse header is at index 0 */
                                    (ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16 + (uint16)RBA_ETHUDP_HDRLENGTH) );

                    /* Fill Udp checksum to 0 in the reuse header as it will be computed by hardware*/
                    TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_CHKSM_OFFSET])),        /* Allocated reuse header is at index 0 */
                                    0U );
#endif
                }
                /* -------------------------------------- */
                /* Call lower layer                       */
                /* -------------------------------------- */

                /* If destination IPv4 address is not ANY */
                if( TCPIP_IPADDR_ANY != lRemoteAdrIPv4_pcst->addr[0U] )
                {
                    lTcpIpRetVal_en = rba_EthIPv4_ZeroCopyFillHeader( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,          /* Local address id */
                                                                      rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockFramePrio_u8,        /* Frame prio */
                                                                      lRemoteAdrIPv4_pcst->addr[0U],                                   /* Remote IPv4 address */
                                                                      &lIpHeader_st,                                                   /* IP header info */
                                                                      (ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16 + (uint16)RBA_ETHUDP_HDRLENGTH),  /* Total Udp length */
                                                                      ZeroCopyArg_pcst );                                              /* ZeroCopy argument */
                }
                else
                {
                    /* Report DET if DestIPv4Addr_u32 is 0.0.0.0 and return BUFREQ_E_NOT_OK at the end of the function */
                    RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_INV_ARG )
                    /* If DET is disabled, default statement will be empty. Nothing to do here. */
                }
            }
            else
#endif
            {
                /* Unknown domain. Throw DET report and break execution. */
                /* ZeroCopy is only implemented for INET domain */
                RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHUDP_E_NOPROTOOPT)
                /* If DET is disabled, default statement will be empty. Nothing to do here. */
            }

            /* Return out argument */
            *UL_SockOwnerIdx_pu8 = rba_EthUdp_DynSockTbl_ast[SocketId_u16].UL_SockOwnerIdx_u8;
        }

        /* Operation not ongoing anymore */
        rba_EthUdp_ReleaseOngoingOperation( SocketId_u16 );
    }

    return lTcpIpRetVal_en;
}


/********************************************************************************************************************/
/* rba_EthUdp_ZeroCopyTransmit()  This API execute the ZeroCopy transmission                                        */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId                              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16             Socket id on which data is to be transmitted                                            */
/* RemoteAddr_pcst           Pointer to a structure containing remote IP address and remote port                    */
/* ZeroCopyArg_pcst         ZeroCopy argument containing the allocated headers and the pointer to the UL data to be */
/*                          transmitted                                                                             */
/*                                                                                                                  */
/* Output parameters:                                                                                               */
/* TxId_pu32            Unique identifier of the ZeroCopy transmission                                              */
/*                      (valid only if ZeroCopyArg_pst->TxConfFlag_b is TRUE)                                       */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* TcpIp_ReturnType      TCPIP_OK                  : UDP datagram transmission successful                           */
/*                       TCPIP_E_NOT_OK            : UDP datagram transmission failed (general failure)             */
/*                       TCPIP_E_ARP_CACHE_MISS    : UDP datagram transmission failed because of ARP cache miss     */
/********************************************************************************************************************/
/* HIS METRIC PATH VIOLATION IN rba_EthUdp_ZeroCopyTransmit: The number of path is increased due to the consecutive if blocks which are required for the execution of various DET checks. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
TcpIp_ReturnType rba_EthUdp_ZeroCopyTransmit ( TcpIp_SocketIdType SocketId_u16,
                                               const TcpIp_SockAddrType * RemoteAddr_pcst,
                                               const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                               uint32 * TxId_pu32 )
{
    /* Declare local variables */
    TcpIp_ReturnType                    lTcpIpRetVal_en;
    Std_ReturnType                      lOperationRetVal_en;
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    Std_ReturnType                      lRetVal_en;
    uint16                              lComputedChecksum_u16;
#endif
#if ( TCPIP_IPV4_ENABLED == STD_ON )
    const TcpIp_SockAddrInetType*       lRemoteAdrIPv4_pcst;
#endif

    /* Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthUdp_Config_pco ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_NOTINIT, TCPIP_E_NOT_OK )

    /* Check whether socket handle is a plausible value */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( SocketId_u16 >= TCPIP_UDPSOCKETMAX ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Report DET if RemoteAddr_pcst is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == RemoteAddr_pcst ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET : Development Error: Pointer is invalid */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pcst == NULL_PTR), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET if ZeroCopyAllocatedHeaders_ast is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET: At least 1 header block should be passed by TcpIp (Reuse) */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 < 1U ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Report DET if HeaderPartBuffer_pu8 for Udp OneTime is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[ZeroCopyArg_pcst->AllocatedHeaderCount_u8-1U].HeaderPartBuffer_pu8 ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET: If we are in Dynamic Mode the length of the Udp OneTime header need to big enough so that Udp one time part can be written  */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 1U ) && ( ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[ZeroCopyArg_pcst->AllocatedHeaderCount_u8-1U].HeaderPartLen_u8 != RBA_ETHUDP_ZEROCOPY_ONETIME_HDR_LEN ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Report DET if ZeroCopyData_pcst is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyData_pcst ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Report DET if DataLength_u16 is greater than the maximum allowed Udp payload length */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE((ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16 > (uint16)(TCPIP_MAXUINT16 - RBA_ETHUDP_HDRLENGTH)), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK)

    /* Report DET if DataBufferList_ppu8 is NULL */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyData_pcst->DataBufferList_ppu8 ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Check whether DataBufferCount_u16 is equal to 1 if we are in Dynamic Mode */
    /* (currently only fragmentation ON use case is supported which does not permit to have more than one data buffer to transmit at the same time) */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( (ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 1U ) && ( 1U != ZeroCopyArg_pcst->ZeroCopyData_pcst->DataBufferCount_u16 ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Local variable initialization */
    lTcpIpRetVal_en = TCPIP_E_NOT_OK;

    /* Check if ZeroCopyTransmit operation is allowed for execution */
    /* Operation is allowed only if there is no closing operation ongoing in other execution context */
    lOperationRetVal_en = rba_EthUdp_ActivateOngoingOperation( SocketId_u16 );

    /* Check whether the ChangeParameter operation is allowed */
    if( lOperationRetVal_en == E_OK )
    {
        /* Report DET: Local Domain not matching remote domain */
        RBA_ETHUDP_DET_REPORT_ERROR_NO_RET( ( RemoteAddr_pcst->domain != rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 ), RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG )

        /* Check whether the requested socket state is RBA_ETHUDP_SOCK_BOUND and local domain matching remote domain */
        if( (RBA_ETHUDP_SOCK_BOUND == rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en) &&
            (RemoteAddr_pcst->domain == rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32) )
        {
#if ( TCPIP_IPV4_ENABLED == STD_ON )
            if( rba_EthUdp_DynSockTbl_ast[SocketId_u16].DomainType_u32 == TCPIP_AF_INET )
            {
                /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically interpret the data as IPv4 data. It is safe because of the domain check before. */
                lRemoteAdrIPv4_pcst = (const TcpIp_SockAddrInetType*) RemoteAddr_pcst;

                /* If destination IPv4 address not is ANY */
                if( TCPIP_IPADDR_ANY != lRemoteAdrIPv4_pcst->addr[0U] )
                {
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
                    /* In DYNAMIC Mode, AllocatedHeaderCount_u8 is greater than 1 as all the headers(One ReuseHeader, multiple IpOneTime Header and One UdpOneTime Header) are allocated */
                    /* In STATIC Mode, AllocatedHeaderCount_u8 is always 1 as only ReuseHeader is allocated and contains all the fields(Static and Dynamic) */
                    if (ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 1U)
                    {
                        lComputedChecksum_u16 = 0U;

                        /* -------------------------------------- */
                        /* Fill one time header                   */
                        /* -------------------------------------- */

                        /* Fill the Length in the header */
                        TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[(ZeroCopyArg_pcst->AllocatedHeaderCount_u8-1U)].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_ONETIME_LENGTH_OFFSET])), /* Udp one time header is the last of the list */
                                        (ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16 + (uint16)RBA_ETHUDP_HDRLENGTH) );

                        /* Compute the checksum to be placed in the frame */
                        lRetVal_en = rba_EthUdp_GetZeroCopyChecksum_IPv4( SocketId_u16,
                                                                          lRemoteAdrIPv4_pcst,
                                                                          ZeroCopyArg_pcst,
                                                                          &lComputedChecksum_u16 );

                        if( E_OK == lRetVal_en )
                        {
                            /* Fill computed Udp checksum */
                            TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[(ZeroCopyArg_pcst->AllocatedHeaderCount_u8-1U)].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_ONETIME_CHKSUM_OFFSET])), /* Udp one time header is the last of the list */
                                            (lComputedChecksum_u16) );

                            /* -------------------------------------- */
                            /* Call lower layer                       */
                            /* -------------------------------------- */

                            lTcpIpRetVal_en = rba_EthIPv4_ZeroCopyTransmit( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,             /* Local address id */
                                                                            (ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16 + (uint16)RBA_ETHUDP_HDRLENGTH),  /* Total Udp length */
                                                                            ZeroCopyArg_pcst,                                                   /* ZeroCopy argument */
                                                                            TxId_pu32 );                                                        /* TxId out argument */
                        }
                    }
                    else
#endif
                    {
#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
                        lTcpIpRetVal_en = rba_EthIPv4_ZeroCopyTransmit( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,             /* Local address id */
                                                                        (ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16 + (uint16)RBA_ETHUDP_HDRLENGTH),  /* Total Udp length */
                                                                        ZeroCopyArg_pcst,                                                   /* ZeroCopy argument */
                                                                        TxId_pu32 );                                                        /* TxId out argument */
#endif
                    }
                }
                else
                {
                    /* Report DET if DestIPv4Addr_u32 is 0.0.0.0 and return BUFREQ_E_NOT_OK at the end of the function */
                    RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_INV_ARG )
                    /* If DET is disabled, default statement will be empty. Nothing to do here. */
                }
            }
            else
#endif
            {
                /* Unknown domain. Throw DET report and break execution. */
                /* ZeroCopy is only implemented for INET domain */
                RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHUDP_E_NOPROTOOPT)
                /* If DET is disabled, default statement will be empty. Nothing to do here. */
            }
        }

        /* Operation not ongoing anymore */
        rba_EthUdp_ReleaseOngoingOperation( SocketId_u16 );
    }

    return lTcpIpRetVal_en;
}


/********************************************************************************************************************/
/* rba_EthUdp_GetZeroCopyChecksum_IPv4()  Compute the checksum to be filled in ZeroCopy frames                      */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId                              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16             Socket id on which data is to be transmitted                                            */
/* RemoteAddrIPv4_pcst      Pointer to a structure containing remote IP address and remote port                     */
/* ZeroCopyArg_pcst         ZeroCopy argument containing the allocated headers and the pointer to the UL data to be */
/*                          transmitted                                                                             */
/*                                                                                                                  */
/* Output parameters:                                                                                               */
/* ComputedChecksum_pu16    Computed checksum value                                                                 */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* Std_ReturnType   - E_OK or E_NOT_OK - Result of operation                                                        */
/********************************************************************************************************************/
#if (( TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON) && (TCPIP_IPV4_ENABLED == STD_ON))
LOCAL_INLINE Std_ReturnType rba_EthUdp_GetZeroCopyChecksum_IPv4( TcpIp_SocketIdType SocketId_u16,
                                                                 const TcpIp_SockAddrInetType * RemoteAddrIPv4_pcst,
                                                                 const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                                 uint16 * ComputedChecksum_pu16 )
{
    /* Declare local variables */
    Std_ReturnType                      lRetVal_en;
    TcpIp_PseudoHdr_tst                 lPseudoHdr_st;
    TcpIp_SockAddrInetType              lLocalAddrIPv4_st;
    uint8                               lNetmask_u8;
    TcpIp_SockAddrInetType              lDefaultRtrIPv4_st;
    uint8                               lUdpHeaderBuffer_u8[RBA_ETHUDP_HDRLENGTH];
    uint8                               lIdx_u8;

    /* Initialize return value */
    lRetVal_en = E_NOT_OK;

    /* Check if the the facility to generate UDP checksums is enabled */
    if(rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockChecksumEnabled_u8 != STD_OFF)
    {
        /* Checksum for transport layer shall be calculated by software if */
        /* 1. The software checksum calculation for UDP is enabled OR */
        /* 2. the frame undergoes fragmentation */
        if(
#if (RBA_ETHUDP_SW_CHKSUM == STD_ON)
            (rba_EthUdp_ChecksumOffloading_cab[rba_EthUdp_LocalAddressToCtrlMap_cau8[rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8]]==FALSE)  ||
#endif
            /* If more than 3 header buffers (ReuseHeader, IpOneTimeHeader, UdpOneTimeHeader) have been allocated, then there is IP fragmentation */
            (ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 3U))
        {
            /* Set domain in structure */
            lLocalAddrIPv4_st.domain = TCPIP_AF_INET;
            lDefaultRtrIPv4_st.domain = TCPIP_AF_INET;

            /* Get the current assigned IP address required for checksum computation */
            /* MR12 RULE 11.3 VIOLATION: Cast is the only way to dynamically provide to TcpIp_GetIpAddr the IP address structure which is domain independant. It is safe because the casted structure type is smalled than the declared structure type. */
            lRetVal_en = TcpIp_GetIpAddr( rba_EthUdp_DynSockTbl_ast[SocketId_u16].LocalAddrId_u8,
                                          (TcpIp_SockAddrType*)(&lLocalAddrIPv4_st),
                                          &lNetmask_u8,
                                          (TcpIp_SockAddrType*)(&lDefaultRtrIPv4_st) );

            if( E_OK == lRetVal_en )
            {
                /* Create Pseudo header required for checksum computation */
                lPseudoHdr_st.IpDomainType_t = TCPIP_AF_INET;
                lPseudoHdr_st.ProtoType_en = TCPIP_IPPROTO_UDP;
                lPseudoHdr_st.RemoteIpAddr_un.IPv4Addr_u32 = RemoteAddrIPv4_pcst->addr[0U];
                lPseudoHdr_st.LocalIpAddr_un.IPv4Addr_u32 = lLocalAddrIPv4_st.addr[0U];

                /* Create UDP header required to reduce complications for CDD layer to compute the checksum.*/
                /* Fill the Source Port and Dest port in the buffer */
                for(lIdx_u8=0U; lIdx_u8<RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_HDR_LEN; lIdx_u8++)
                {
                    lUdpHeaderBuffer_u8[lIdx_u8] = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_SRCPORT_OFFSET + lIdx_u8];     /* Allocated reuse header is at index 0 */
                }

                /* Fill the length in the buffer */
                lUdpHeaderBuffer_u8[lIdx_u8] = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[(ZeroCopyArg_pcst->AllocatedHeaderCount_u8-1U)].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_ONETIME_LENGTH_OFFSET];   /* Udp one time header length */
                lUdpHeaderBuffer_u8[lIdx_u8+1U] = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[(ZeroCopyArg_pcst->AllocatedHeaderCount_u8-1U)].HeaderPartBuffer_pu8[RBA_ETHUDP_ZEROCOPY_ONETIME_LENGTH_OFFSET+1U]; /* Udp one time header length */

                /* Fill the Checksum with 0 in the buffer */
                lUdpHeaderBuffer_u8[lIdx_u8+2U] = 0U;
                lUdpHeaderBuffer_u8[lIdx_u8+3U] = 0U;

                /* Compute checksum */
                *ComputedChecksum_pu16 = rba_EthUdp_ZeroCopyChkSumCalculation( &lPseudoHdr_st,
                                                                               &lUdpHeaderBuffer_u8[0],
                                                                               ZeroCopyArg_pcst->ZeroCopyData_pcst->DataBufferList_ppu8[0U],
                                                                               ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16 );
            }
        }

        /* If the Udp checksum will be calculated by HW */
        else
        {
            /* Fill it with 0 */
            *ComputedChecksum_pu16 = 0U;
            lRetVal_en = E_OK;
        }
    }
    else /* rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockChecksumEnabled_u8) = FALSE  */
    {
        /* UDP Sw checksum should not be executed since it was disabled optionnally */
        /* In this case, the frame shall be sent with the value 0 inside the field belong to the UDP checksum */
        *ComputedChecksum_pu16 = 0U;
        lRetVal_en = E_OK;
    }

    return lRetVal_en;
}
#endif

#define RBA_ETHUDP_STOP_SEC_CODE_FAST
#include "rba_EthUdp_MemMap.h"

#endif /* #if ( RBA_ETHUDP_ZEROCOPY_TX_SUPPORT == STD_ON ) */
#endif /* #if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */
