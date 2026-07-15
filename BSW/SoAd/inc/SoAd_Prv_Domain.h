

/* This file contains all the helper macros that deal with IP address domain */

#ifndef SOAD_PRV_DOMAIN_H
#define SOAD_PRV_DOMAIN_H

/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "SoAd_Prv.h"

/*******************************************************************************
**                      Macro definitions                                     **
*******************************************************************************/

LOCAL_INLINE void SoAd_SetPortToSockAddr( \
                                        TcpIp_SockAddrType * SockAddr_pst, \
                                        uint16               Port_u16 );

LOCAL_INLINE void SoAd_SetDyncSocConfigRemoteAddrtoDefault( \
                                        const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst);

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr( \
                                        const TcpIp_SockAddrType *         SockAddr_pcst, \
                                        uint16                             RemoteAddrPort_u16, \
                                        const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst);

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemoteIpAddr( \
                                        const TcpIp_SockAddrType *         SockAddr_pcst, \
                                        const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst);

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemotePort( \
                                        const TcpIp_SockAddrType *         SockAddr_pcst, \
                                        const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst);

LOCAL_INLINE void SoAd_CopyRemoteAddrFromDyncSocToSockAddr( \
                                        const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst, \
                                        TcpIp_SockAddrType *               SockAddr_pst);

LOCAL_INLINE void SoAd_CopyRemoteAddrFromStaticSocToDynSoc( \
                                        const SoAd_StaticSocConfigType_tst * StaticSocConfig_pcst, \
                                        const SoAd_DyncSocConfigType_tst *   DyncSocConfig_pcst);

LOCAL_INLINE void SoAd_GetDyncSocConfigTableRangeWithSameDomain( \
                                        const SoAd_StaticSocConfigType_tst * StaticSocConfig_pcst, \
                                        SoAd_SoConIdType *                   StartIdx_pst, \
                                        SoAd_SoConIdType *                   EndIdx_pst);

LOCAL_INLINE void SoAd_BuildSockAddr(const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst, \
                                        TcpIp_DomainType                Domain_en, \
                                        TcpIp_SockAddrInetType *        SockAddrInet_pst, \
                                        TcpIp_SockAddrInet6Type *       SockAddrInet6_pst, \
                                        TcpIp_SockAddrType **           SockAddr_ppst);



/* If IPv6 support is enabled, then define the helper macros to support both domain type structures. */
#if ( ( SOAD_IPv4_SUPPORT_ENABLE == STD_ON ) && ( SOAD_IPv6_SUPPORT_ENABLE == STD_ON ) )

/* Get the port from a TcpIp_SockAddrType structure. */
#define SOAD_GET_PORT_FROM_SOCK_ADDR( SockAddr_pst ) \
    ( ( (SockAddr_pst)->domain == TCPIP_AF_INET ) ? ( ( (const TcpIp_SockAddrInetType *) (SockAddr_pst) )->port ) : ( ( (const TcpIp_SockAddrInet6Type *)(SockAddr_pst) )->port ) )

/* Get the remote port from a static socket. */
#define SOAD_GET_REMOTE_PORT_FROM_STATIC_SOC_CONFIG( StaticSocConfig_cpst ) \
    ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[(StaticSocConfig_cpst)->soConGrpId_uo].domain_en == TCPIP_AF_INET ) ? \
        ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].port_u16 ) : \
        ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].port_u16 ) )

/* Get the remote port from a dynamic socket. */
#define SOAD_GET_REMOTE_PORT_FROM_DYNC_SOC_CONFIG( DyncSocConfig_pst ) \
    ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[(DyncSocConfig_pst)->idxStaticSoc_uo].soConGrpId_uo].domain_en == TCPIP_AF_INET ) ? \
        ( SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].port_u16 ) : \
        ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].port_u16 ) )


/* Check if a TcpIp_SockAddrType structure has a wildcard (ANY) IP address. */
#define SOAD_SOCK_ADDR_HAS_WILDCARD_IPADDR( SockAddr_pst ) \
    ( ( (SockAddr_pst)->domain == TCPIP_AF_INET ) ? \
        ( ( (const TcpIp_SockAddrInetType *) (SockAddr_pst) )->addr[0] == TCPIP_IPADDR_ANY ) : \
        ( ( ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[0] == TCPIP_IP6ADDR_ANY ) && \
        ( ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[1] == TCPIP_IP6ADDR_ANY ) && \
        ( ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[2] == TCPIP_IP6ADDR_ANY ) && \
        ( ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[3] == TCPIP_IP6ADDR_ANY ) ) )

/* Check if a static socket has a wildcard (ANY) remote IP address. */
#define SOAD_STATIC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( StaticSocConfig_cpst ) \
    ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[(StaticSocConfig_cpst)->soConGrpId_uo].domain_en == TCPIP_AF_INET ) ? \
        ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_u32 == TCPIP_IPADDR_ANY ) : \
        ( ( ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_au32[0] ) == TCPIP_IP6ADDR_ANY ) && \
        ( ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_au32[1] ) == TCPIP_IP6ADDR_ANY ) && \
        ( ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_au32[2] ) == TCPIP_IP6ADDR_ANY ) && \
        ( ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_au32[3] ) == TCPIP_IP6ADDR_ANY ) ) )

/* Check if a dynamic socket has a wildcard (ANY) remote IP address. */
#define SOAD_DYNC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( DyncSocConfig_pst ) \
    ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[(DyncSocConfig_pst)->idxStaticSoc_uo].soConGrpId_uo].domain_en == TCPIP_AF_INET ) ? \
        ( SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_u32 == TCPIP_IPADDR_ANY ) : \
        ( ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[0] == TCPIP_IP6ADDR_ANY ) && \
        ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[1] == TCPIP_IP6ADDR_ANY ) && \
        ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[2] == TCPIP_IP6ADDR_ANY ) && \
        ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[3] == TCPIP_IP6ADDR_ANY ) ) )

/* Check if the IP address from a TcpIp_SockAddrType structure is equal to the remote IP address for a dynamic socket. */
#define SOAD_SOCK_IPADDR_IS_EQUAL_TO_DYNC_SOC_CONFIG_REMOTE_IPADDR( SockAddr_pst, DyncSocConfig_pst ) \
    ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[(DyncSocConfig_pst)->idxStaticSoc_uo].soConGrpId_uo].domain_en != (SockAddr_pst)->domain ) ? ( FALSE ) : \
        ( ( (SockAddr_pst)->domain == TCPIP_AF_INET ) ? \
            ( SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_u32 == ( (const TcpIp_SockAddrInetType *) (SockAddr_pst) )->addr[0] ) : \
            ( ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[0] == ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[0] ) && \
            ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[1] == ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[1] ) && \
            ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[2] == ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[2] ) && \
            ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[3] == ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[3] ) ) ) )

/* Check if a given TcpIp_SockAddrType structure is large enough to store an address for a given connection. */
#define SOAD_SOCK_ADDR_CAN_STORE_CON_ADDR( SockAddr_pst, SoConId_uo ) \
    ( ( (SockAddr_pst)->domain == TCPIP_AF_INET6 ) || \
    ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[(SoConId_uo)].soConGrpId_uo].domain_en ) == TCPIP_AF_INET ) )



/**
 *******************************************************************************************************************
 * \Function Name : SoAd_SetPortToSockAddr()
 *
 * \Function description: Set the port in a TcpIp_SockAddrType structure
 *
 * Parameters (in):
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   uint16
 * \arg     Port_u16 - Holds port number
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: SockAddr_pst is modified inside the function and hence cannot be P2CONST */
LOCAL_INLINE void SoAd_SetPortToSockAddr( TcpIp_SockAddrType * SockAddr_pst, \
                                          uint16               Port_u16 )
{
    if( (SockAddr_pst)->domain == TCPIP_AF_INET )
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInetType *) (SockAddr_pst) )->port = (Port_u16);
    }
    else
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->port = (Port_u16);
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_SetDyncSocConfigRemoteAddrtoDefault()
 *
 * \Function description: Set the remote IP address for a dynamic socket to default values
 *
 * Parameters (in):
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_SetDyncSocConfigRemoteAddrtoDefault( const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    if( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->\
         SoAd_StaticSocConfig_cpst[(DyncSocConfig_pcst)->idxStaticSoc_uo].soConGrpId_uo].domain_en == TCPIP_AF_INET )
    {
        SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32 = SOAD_DEFAULT_IP;
        SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = SOAD_DEFAULT_PORT;
    }
    else
    {
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0] = SOAD_DEFAULT_IP;
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1] = SOAD_DEFAULT_IP;
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2] = SOAD_DEFAULT_IP;
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3] = SOAD_DEFAULT_IP;
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16    = SOAD_DEFAULT_PORT;
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr()
 *
 * \Function description: Copy the IP address and port from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   uint16
 * \arg     RemoteAddrPort_u16 - Remote address port
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                  uint16                             RemoteAddrPort_u16, \
                                                                  const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    if( (SockAddr_pcst)->domain == TCPIP_AF_INET )
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32 = ( (const TcpIp_SockAddrInetType *) (SockAddr_pcst) )->addr[0];

        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = RemoteAddrPort_u16;
    }
    else
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[0];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[1];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[2];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[3];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16    = RemoteAddrPort_u16;
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemoteIpAddr()
 *
 * \Function description: Copy only the IP address from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemoteIpAddr( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                    const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    if( (SockAddr_pcst)->domain == TCPIP_AF_INET )
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32 = ( (const TcpIp_SockAddrInetType *) (SockAddr_pcst) )->addr[0];
    }
    else
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[0];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[1];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[2];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[3];
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemotePort()
 *
 * \Function description: Copy only the port number from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemotePort( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                  const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    if( (SockAddr_pcst)->domain == TCPIP_AF_INET )
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = ( (const TcpIp_SockAddrInetType *) (SockAddr_pcst) )->port;
    }
    else
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->port;
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyRemoteAddrFromDyncSocToSockAddr()
 *
 * \Function description: Copy the remote IP address and port of a dynamic socket to a TcpIp_SockAddrType structure
 *
 * Parameters (in):
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pcst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: SockAddr_pst is modified inside the function and hence cannot be P2CONST */
LOCAL_INLINE void SoAd_CopyRemoteAddrFromDyncSocToSockAddr(const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst, \
                                                           TcpIp_SockAddrType *               SockAddr_pst)
{
    if( (SockAddr_pst)->domain == TCPIP_AF_INET )
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInetType*) (SockAddr_pst) )->addr[0] = SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32;
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInetType*) (SockAddr_pst) )->port    = SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16;
    }
    else
    {
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->addr[0] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->addr[1] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->addr[2] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->addr[3] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3];
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->port    = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16;
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyRemoteAddrFromStaticSocToDynSoc()
 *
 * \Function description: Copy the remote IP address and port from a static socket to a dynamic socket
 *
 * Parameters (in):
 * \param   const SoAd_StaticSocConfigType_tst*
 * \arg     StaticSocConfig_pcst - Pointer to a structure of type SoAd_StaticSocConfigType_tst
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyRemoteAddrFromStaticSocToDynSoc( const SoAd_StaticSocConfigType_tst * StaticSocConfig_pcst, \
                                                            const SoAd_DyncSocConfigType_tst *   DyncSocConfig_pcst)
{
    if( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[(StaticSocConfig_pcst)->soConGrpId_uo].domain_en == TCPIP_AF_INET )
    {
        SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32 = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_u32;
        SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].port_u16;
    }
    else
    {
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0] = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_au32[0];
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1] = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_au32[1];
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2] = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_au32[2];
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3] = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_au32[3];
        SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16    = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].port_u16;
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_GetDyncSocConfigTableRangeWithSameDomain()
 *
 * \Function description: This function gets the range of indexes in the dynamic socket table which have the same domain as a given static socket
 * (all dynamic sockets represented by the index range [*(StartIdx_puo), *(EndIdx_puo)) have the same domain as the static socket)
 *
 * Parameters (in):
 * \param   const SoAd_StaticSocConfigType_tst*
 * \arg     StaticSocConfig_pcst - Pointer to a structure of type StaticSocConfig_cpst
 * \param   SoAd_SoConIdType*
 * \arg     StartIdx_pst - Pointer to a structure of type SoAd_SoConIdType
 * \param   SoAd_SoConIdType*
 * \arg     EndIdx_pst - Pointer to a structure of type SoAd_SoConIdType
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_GetDyncSocConfigTableRangeWithSameDomain(const SoAd_StaticSocConfigType_tst * StaticSocConfig_pcst, \
                                                                SoAd_SoConIdType *                   StartIdx_pst, \
                                                                SoAd_SoConIdType *                   EndIdx_pst)
{
    if( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[(StaticSocConfig_pcst)->soConGrpId_uo].domain_en == TCPIP_AF_INET )
    {
        *(StartIdx_pst) = 0;
        *(EndIdx_pst)   = SoAd_CurrConfig_cpst->SoAd_noInetActiveSockets_uo;
    }
    else
    {
        *(StartIdx_pst) = SoAd_CurrConfig_cpst->SoAd_noInetActiveSockets_uo;
        *(EndIdx_pst)   = SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo;
    }
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_BuildSockAddr()
 *
 * \Function description: This function returns a pointer to a TcpIp_SockAddrType structure which has the IP address
 *  and port equal to the remote IP address and port of a given dynamic socket.
 *
 * Parameters (in):
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 * \param   TcpIp_DomainType
 * \arg     Domain_en - variable of type TcpIp_DomainType
 * \param   TcpIp_SockAddrInetType*
 * \arg     SockAddrInet_pst - Pointer to a structure of type TcpIp_SockAddrInetType
 * \param   TcpIp_SockAddrInet6Type*
 * \arg     SockAddrInet6_pst - Pointer to a structure of type TcpIp_SockAddrInet6Type
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_BuildSockAddr(const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst, \
                                     TcpIp_DomainType                   Domain_en, \
                                     TcpIp_SockAddrInetType *           SockAddrInet_pst, \
                                     TcpIp_SockAddrInet6Type *          SockAddrInet6_pst, \
                                     TcpIp_SockAddrType **              SockAddr_ppst)
{
    if ( (Domain_en) == TCPIP_AF_INET )
    {
        (SockAddrInet_pst)->domain  = (Domain_en);
        (SockAddrInet_pst)->addr[0] = SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32;
        (SockAddrInet_pst)->port    = SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16;
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        (*SockAddr_ppst)              = ( (TcpIp_SockAddrType *) (SockAddrInet_pst) );

    }
    else
    {
        (SockAddrInet6_pst)->domain  = (Domain_en);
        (SockAddrInet6_pst)->addr[0] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0];
        (SockAddrInet6_pst)->addr[1] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1];
        (SockAddrInet6_pst)->addr[2] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2];
        (SockAddrInet6_pst)->addr[3] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3];
        (SockAddrInet6_pst)->port    = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16;
        /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
        (*SockAddr_ppst)               = ( (TcpIp_SockAddrType *) (SockAddrInet6_pst) ) ;
    }
}

#elif ( SOAD_IPv4_SUPPORT_ENABLE == STD_ON )    /* Only IPv4 specific code is enabled; define the same macros but without the domain check and IPv6 specific branch. */

/* Get the port from a TcpIp_SockAddrType structure. */
#define SOAD_GET_PORT_FROM_SOCK_ADDR( SockAddr_pst ) \
    ( ( (const TcpIp_SockAddrInetType *) (SockAddr_pst) )->port )

/* Get the remote port from a static socket. */
#define SOAD_GET_REMOTE_PORT_FROM_STATIC_SOC_CONFIG( StaticSocConfig_cpst ) \
    ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].port_u16 )

/* Get the remote port from a dynamic socket. */
#define SOAD_GET_REMOTE_PORT_FROM_DYNC_SOC_CONFIG( DyncSocConfig_pst ) \
    ( SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].port_u16 )

/* Check if a TcpIp_SockAddrType structure has a wildcard (ANY) IP address. */
#define SOAD_SOCK_ADDR_HAS_WILDCARD_IPADDR( SockAddr_pst ) \
    ( ( (const TcpIp_SockAddrInetType *) (SockAddr_pst) )->addr[0] == TCPIP_IPADDR_ANY )

/* Check if a static socket has a wildcard (ANY) remote IP address. */
#define SOAD_STATIC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( StaticSocConfig_cpst ) \
    ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_u32 == TCPIP_IPADDR_ANY )

/* Check if a dynamic socket has a wildcard (ANY) remote IP address. */
#define SOAD_DYNC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( DyncSocConfig_pst ) \
    ( SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_u32 == TCPIP_IPADDR_ANY )

/* Check if the IP address from a TcpIp_SockAddrType structure is equal to the remote IP address for a dynamic socket. */
#define SOAD_SOCK_IPADDR_IS_EQUAL_TO_DYNC_SOC_CONFIG_REMOTE_IPADDR( SockAddr_pst, DyncSocConfig_pst ) \
    ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[(DyncSocConfig_pst)->idxStaticSoc_uo].soConGrpId_uo].domain_en != (SockAddr_pst)->domain ) ? \
        ( FALSE ) : \
        ( SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_u32 == ( (const TcpIp_SockAddrInetType *) (SockAddr_pst) )->addr[0] ) )

/* Check if a given TcpIp_SockAddrType structure is large enough to store an address for a given connection. */
#define SOAD_SOCK_ADDR_CAN_STORE_CON_ADDR( SockAddr_pst, SoConId_uo ) \
    ( ( SoAd_CurrConfig_cpst->SoAd_StaticSocGroupConfig_cpst[SoAd_CurrConfig_cpst->SoAd_StaticSocConfig_cpst[(SoConId_uo)].soConGrpId_uo].domain_en ) == TCPIP_AF_INET )


/**
 *******************************************************************************************************************
 * \Function Name : SoAd_SetPortToSockAddr()
 *
 * \Function description: Set the port in a TcpIp_SockAddrType structure
 *
 * Parameters (in):
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   uint16
 * \arg     Port_u16 - Holds port number
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: SockAddr_pst is modified inside the function and hence cannot be P2CONST */
LOCAL_INLINE void SoAd_SetPortToSockAddr( TcpIp_SockAddrType * SockAddr_pst, \
                                          uint16               Port_u16 )
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInetType *) (SockAddr_pst) )->port = (Port_u16);
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_SetDyncSocConfigRemoteAddrtoDefault()
 *
 * \Function description: Set the remote IP address for a dynamic socket to default values
 *
 * Parameters (in):
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_SetDyncSocConfigRemoteAddrtoDefault( const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32 = SOAD_DEFAULT_IP;
    SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = SOAD_DEFAULT_PORT;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr()
 *
 * \Function description: Copy the IP address and port from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   uint16
 * \arg     RemoteAddrPort_u16 - Remote address port
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                  uint16                             RemoteAddrPort_u16, \
                                                                  const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32 = ( (const TcpIp_SockAddrInetType *) (SockAddr_pcst) )->addr[0];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = RemoteAddrPort_u16;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemoteIpAddr()
 *
 * \Function description: Copy only the IP address from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemoteIpAddr( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                    const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32 = ( (const TcpIp_SockAddrInetType *) (SockAddr_pcst) )->addr[0];
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemotePort()
 *
 * \Function description: Copy only the port number from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemotePort( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                  const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = ( (const TcpIp_SockAddrInetType *) (SockAddr_pcst) )->port;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyRemoteAddrFromDyncSocToSockAddr()
 *
 * \Function description: Copy the remote IP address and port of a dynamic socket to a TcpIp_SockAddrType structure
 *
 * Parameters (in):
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pcst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: SockAddr_pst is modified inside the function and hence cannot be P2CONST */
LOCAL_INLINE void SoAd_CopyRemoteAddrFromDyncSocToSockAddr(const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst, \
                                                           TcpIp_SockAddrType *               SockAddr_pst)
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInetType*) (SockAddr_pst) )->addr[0] = SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32;
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInetType*) (SockAddr_pst) )->port    = SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyRemoteAddrFromStaticSocToDynSoc()
 *
 * \Function description: Copy the remote IP address and port from a static socket to a dynamic socket
 *
 * Parameters (in):
 * \param   const SoAd_StaticSocConfigType_tst*
 * \arg     StaticSocConfig_pcst - Pointer to a structure of type SoAd_StaticSocConfigType_tst
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyRemoteAddrFromStaticSocToDynSoc(const SoAd_StaticSocConfigType_tst * StaticSocConfig_pcst, \
                                                           const SoAd_DyncSocConfigType_tst *   DyncSocConfig_pcst)
{
    SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32 = \
        SoAd_CurrConfig_cpst->statSoConRemoteAddrInet_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_u32;
    SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = \
        SoAd_CurrConfig_cpst->statSoConRemoteAddrInet_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].port_u16;
}


/**
 *******************************************************************************************************************
 * \Function Name : SoAd_GetDyncSocConfigTableRangeWithSameDomain()
 *
 * \Function description: This function gets the range of indexes in the dynamic socket table which have the same domain as a given static socket
 * (all dynamic sockets represented by the index range [*(StartIdx_puo), *(EndIdx_puo)) have the same domain as the static socket)
 *
 * Parameters (in):
 * \param   const SoAd_StaticSocConfigType_tst*
 * \arg     StaticSocConfig_pcst - Pointer to a structure of type SoAd_StaticSocConfigType_tst
 * \param   SoAd_SoConIdType*
 * \arg     StartIdx_pst - Pointer to a structure of type SoAd_SoConIdType
 * \param   SoAd_SoConIdType*
 * \arg     EndIdx_pst - Pointer to a structure of type SoAd_SoConIdType
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_GetDyncSocConfigTableRangeWithSameDomain(const SoAd_StaticSocConfigType_tst * StaticSocConfig_pcst, \
                                                                SoAd_SoConIdType *                   StartIdx_pst, \
                                                                SoAd_SoConIdType *                   EndIdx_pst)
{
    *(StartIdx_pst) = 0;
    *(EndIdx_pst)   = SoAd_CurrConfig_cpst->SoAd_noInetActiveSockets_uo;
    (void)StaticSocConfig_pcst;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_BuildSockAddr()
 *
 * \Function description: This function returns a pointer to a TcpIp_SockAddrType structure which has the IP address
 *  and port equal to the remote IP address and port of a given dynamic socket.
 *
 * Parameters (in):
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 * \param   TcpIp_DomainType
 * \arg     Domain_en - variable to enum of type TcpIp_DomainType
 * \param   TcpIp_SockAddrInetType*
 * \arg     SockAddrInet_pst - Pointer to a structure of type TcpIp_SockAddrInetType
 * \param   TcpIp_SockAddrInet6Type*
 * \arg     SockAddrInet6_pst - Pointer to a structure of type TcpIp_SockAddrInet6Type
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_BuildSockAddr(const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst, \
                                     TcpIp_DomainType                   Domain_en, \
                                     TcpIp_SockAddrInetType *           SockAddrInet_pst, \
/* MR12 RULE 8.13 VIOLATION: The Pointer SockAddrInet6_pst is not used when only IPV4 is ON, but will be used when both IPV4 and IPV6 are ON */
                                     TcpIp_SockAddrInet6Type * SockAddrInet6_pst, \
                                     TcpIp_SockAddrType **              SockAddr_ppst)
{
    (void)(SockAddrInet6_pst);
    (SockAddrInet_pst)->domain  = (Domain_en);
    (SockAddrInet_pst)->addr[0] = SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_u32;
    (SockAddrInet_pst)->port    = SoAd_DynSoConRemoteAddrInet_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16;
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    (*SockAddr_ppst)              = ( (TcpIp_SockAddrType *) (SockAddrInet_pst) );
}

#elif ( SOAD_IPv6_SUPPORT_ENABLE == STD_ON )    /* Only IPv6 specific code is enabled; define the same macros but without the domain check and IPv4 specific branch. */

/* Get the port from a TcpIp_SockAddrType structure. */
#define SOAD_GET_PORT_FROM_SOCK_ADDR( SockAddr_pst ) \
    ( ( (const TcpIp_SockAddrInet6Type *)(SockAddr_pst) )->port )

/* Get the remote port from a static socket. */
#define SOAD_GET_REMOTE_PORT_FROM_STATIC_SOC_CONFIG( StaticSocConfig_cpst ) \
    ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].port_u16 )

/* Get the remote port from a dynamic socket. */
#define SOAD_GET_REMOTE_PORT_FROM_DYNC_SOC_CONFIG( DyncSocConfig_pst ) \
    ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].port_u16 )

/* Check if a TcpIp_SockAddrType structure has a wildcard (ANY) IP address. */
#define SOAD_SOCK_ADDR_HAS_WILDCARD_IPADDR( SockAddr_pst ) \
    ( ( ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[0] == TCPIP_IP6ADDR_ANY ) && \
    ( ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[1] == TCPIP_IP6ADDR_ANY ) && \
    ( ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[2] == TCPIP_IP6ADDR_ANY ) && \
    ( ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[3] == TCPIP_IP6ADDR_ANY ) )

/* Check if a static socket has a wildcard (ANY) remote IP address. */
#define SOAD_STATIC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( StaticSocConfig_cpst ) \
    ( ( ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_au32[0] ) == \
    TCPIP_IP6ADDR_ANY ) && ( ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_au32[1] ) == \
    TCPIP_IP6ADDR_ANY ) && ( ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_au32[2] ) == \
    TCPIP_IP6ADDR_ANY ) && ( ( SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_cpst)->idxStatSoConRemoteAddr_uo].addr_au32[3] ) == TCPIP_IP6ADDR_ANY ) )

/* Check if a dynamic socket has a wildcard (ANY) remote IP address. */
#define SOAD_DYNC_SOC_CONFIG_HAS_WILDCARD_REMOTE_IPADDR( DyncSocConfig_pst ) \
    ( ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[0] == TCPIP_IP6ADDR_ANY ) && \
    ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[1] == TCPIP_IP6ADDR_ANY ) && \
    ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[2] == TCPIP_IP6ADDR_ANY ) && \
    ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[3] == TCPIP_IP6ADDR_ANY ) )

/* Check if the IP address from a TcpIp_SockAddrType structure is equal to the remote IP address for a dynamic socket. */
#define SOAD_SOCK_IPADDR_IS_EQUAL_TO_DYNC_SOC_CONFIG_REMOTE_IPADDR( SockAddr_pst, DyncSocConfig_pst ) \
    ( ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[0] == \
    ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[0] ) && \
    ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[1] == \
    ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[1] ) && \
    ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[2] == \
    ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[2] ) && \
    ( SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pst)->idxDynSoConRemoteAddr_uo].addr_au32[3] == \
    ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->addr[3] ) )

/* Check if a given TcpIp_SockAddrType structure is large enough to store an address for a given connection. */
#define SOAD_SOCK_ADDR_CAN_STORE_CON_ADDR( SockAddr_pst, SoConId_uo ) \
    ( (SockAddr_pst)->domain == TCPIP_AF_INET6 )

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_SetPortToSockAddr()
 *
 * \Function description: Set the port in a TcpIp_SockAddrType structure
 *
 * Parameters (in):
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   uint16
 * \arg     Port_u16 - Holds port number
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: SockAddr_pst is modified inside the function and hence cannot be P2CONST */
LOCAL_INLINE void SoAd_SetPortToSockAddr( TcpIp_SockAddrType * SockAddr_pst, \
                                          uint16               Port_u16 )
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInet6Type *) (SockAddr_pst) )->port = (Port_u16);
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_SetDyncSocConfigRemoteAddrtoDefault()
 *
 * \Function description: Set the remote IP address for a dynamic socket to default values
 *
 * Parameters (in):
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_SetDyncSocConfigRemoteAddrtoDefault( const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0] = SOAD_DEFAULT_IP;
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1] = SOAD_DEFAULT_IP;
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2] = SOAD_DEFAULT_IP;
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3] = SOAD_DEFAULT_IP;
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16    = SOAD_DEFAULT_PORT;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr()
 *
 * \Function description: Copy the IP address and port from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   uint16
 * \arg     RemoteAddrPort_u16 - Remote address port
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemoteAddr( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                  uint16                             RemoteAddrPort_u16, \
                                                                  const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[0];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[1];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[2];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[3];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16    = RemoteAddrPort_u16;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemoteIpAddr()
 *
 * \Function description: Copy only the IP address from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemoteIpAddr( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                    const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[0];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[1];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[2];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3] = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->addr[3];
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyFromSockAddrToDyncSocConfigRemotePort()
 *
 * \Function description: Copy only the port number from a TcpIp_SockAddrType structure to the remote address for a dynamic socket.
 *
 * Parameters (in):
 * \param   const TcpIp_SockAddrType*
 * \arg     SockAddr_pcst - Pointer to a structure of type TcpIp_SockAddrType
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyFromSockAddrToDyncSocConfigRemotePort( const TcpIp_SockAddrType *         SockAddr_pcst, \
                                                                  const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst)
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16 = ( (const TcpIp_SockAddrInet6Type *) (SockAddr_pcst) )->port;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyRemoteAddrFromDyncSocToSockAddr()
 *
 * \Function description: Copy the remote IP address and port of a dynamic socket to a TcpIp_SockAddrType structure
 *
 * Parameters (in):
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pcst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */
/* MR12 RULE 8.13 VIOLATION: SockAddr_pst is modified inside the function and hence cannot be P2CONST */
LOCAL_INLINE void SoAd_CopyRemoteAddrFromDyncSocToSockAddr(const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst, \
                                                           TcpIp_SockAddrType *               SockAddr_pst)
{
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->addr[0] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->addr[1] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->addr[2] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->addr[3] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3];
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    ( (TcpIp_SockAddrInet6Type*)(SockAddr_pst) )->port    = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_CopyRemoteAddrFromStaticSocToDynSoc()
 *
 * \Function description: Copy the remote IP address and port from a static socket to a dynamic socket
 *
 * Parameters (in):
 * \param   const SoAd_StaticSocConfigType_tst*
 * \arg     StaticSocConfig_pcst - Pointer to a structure of type SoAd_StaticSocConfigType_tst
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_CopyRemoteAddrFromStaticSocToDynSoc(const SoAd_StaticSocConfigType_tst * StaticSocConfig_pcst, \
                                                           const SoAd_DyncSocConfigType_tst *   DyncSocConfig_pcst)
{
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0] = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_au32[0];
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1] = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_au32[1];
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2] = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_au32[2];
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3] = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].addr_au32[3];
    SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16    = SoAd_CurrConfig_cpst->statSoConRemoteAddrInet6_cpst[(StaticSocConfig_pcst)->idxStatSoConRemoteAddr_uo].port_u16;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_GetDyncSocConfigTableRangeWithSameDomain()
 *
 * \Function description: This function gets the range of indexes in the dynamic socket table which have the same domain as a given static socket
 * (all dynamic sockets represented by the index range [*(StartIdx_puo), *(EndIdx_puo)) have the same domain as the static socket)
 *
 * Parameters (in):
 * \param   const SoAd_StaticSocConfigType_tst*
 * \arg     StaticSocConfig_pcst - Pointer to a structure of type SoAd_StaticSocConfigType_tst
 * \param   SoAd_SoConIdType*
 * \arg     StartIdx_pst - Pointer to a structure of type SoAd_SoConIdType
 * \param   SoAd_SoConIdType*
 * \arg     EndIdx_pst - Pointer to a structure of type SoAd_SoConIdType
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_GetDyncSocConfigTableRangeWithSameDomain(const SoAd_StaticSocConfigType_tst * StaticSocConfig_pcst, \
                                                                SoAd_SoConIdType *                   StartIdx_pst, \
                                                                SoAd_SoConIdType *                   EndIdx_pst)
{
    *(StartIdx_pst) = SoAd_CurrConfig_cpst->SoAd_noInetActiveSockets_uo;
    *(EndIdx_pst)   = SoAd_CurrConfig_cpst->SoAd_noActiveSockets_uo;
    (void)StaticSocConfig_pcst;
}

/**
 *******************************************************************************************************************
 * \Function Name : SoAd_BuildSockAddr()
 *
 * \Function description: This function returns a pointer to a TcpIp_SockAddrType structure which has the IP address
 *  and port equal to the remote IP address and port of a given dynamic socket.
 *
 * Parameters (in):
 * \param   SoAd_DyncSocConfigType_tst*
 * \arg     DyncSocConfig_pst - Pointer to a structure of type SoAd_DyncSocConfigType_tst
 * \param   TcpIp_DomainType
 * \arg     Domain_en - variable to enum of type TcpIp_DomainType
 * \param   TcpIp_SockAddrInetType*
 * \arg     SockAddrInet_pst - Pointer to a structure of type TcpIp_SockAddrInetType
 * \param   TcpIp_SockAddrInet6Type*
 * \arg     SockAddrInet6_pst - Pointer to a structure of type TcpIp_SockAddrInet6Type
 * \param   TcpIp_SockAddrType*
 * \arg     SockAddr_pst - Pointer to a structure of type TcpIp_SockAddrType
 *
 * Parameters (inout): None
 *
 * Parameters (out): None
 *
 * Return value: void
 *
 ********************************************************************************************************************
 */

LOCAL_INLINE void SoAd_BuildSockAddr(const SoAd_DyncSocConfigType_tst * DyncSocConfig_pcst, \
                                     TcpIp_DomainType                   Domain_en, \
/* MR12 RULE 8.13 VIOLATION: The Pointer SockAddrInet_pst is not used when only IPV6 is ON, but will be used when both IPV4 and IPV6 are ON */
                                     TcpIp_SockAddrInetType *           SockAddrInet_pst, \
                                     TcpIp_SockAddrInet6Type *          SockAddrInet6_pst, \
                                     TcpIp_SockAddrType **              SockAddr_ppst)
{
    (void)(SockAddrInet_pst);
    (SockAddrInet6_pst)->domain  = (Domain_en);
    (SockAddrInet6_pst)->addr[0] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[0];
    (SockAddrInet6_pst)->addr[1] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[1];
    (SockAddrInet6_pst)->addr[2] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[2];
    (SockAddrInet6_pst)->addr[3] = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].addr_au32[3];
    (SockAddrInet6_pst)->port    = SoAd_DynSoConRemoteAddrInet6_pst[(DyncSocConfig_pcst)->idxDynSoConRemoteAddr_uo].port_u16;
    /* MR12 RULE 11.3 VIOLATION: The cast is safe because the generic type is used by the caller function and extracts the info depending on the domain. */
    (*SockAddr_ppst)               = ( (TcpIp_SockAddrType *) (SockAddrInet6_pst) );
}

#endif  /* ( ( SOAD_IPv4_SUPPORT_ENABLE == STD_ON ) && ( SOAD_IPv6_SUPPORT_ENABLE == STD_ON ) ) */

#endif /* SOAD_PRV_DOMAIN_H */
