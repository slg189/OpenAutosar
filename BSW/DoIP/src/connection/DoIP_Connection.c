

#include "DoIP_Prv.h"

#define DOIP_START_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

boolean DoIP_OpenConnections_b;
static DoIP_ConnectionType_tst DoIP_Connection_ast[DOIP_CFG_CONN_ARRAY_SIZE];

#define DOIP_STOP_SEC_VAR_CLEARED
#include "DoIP_MemMap.h"

#define DOIP_START_SEC_CODE
#include "DoIP_MemMap.h"

void DoIP_Connection_Init(void)
{
    uint16 connIdx_u16;
    Std_ReturnType retVal_u8 = E_NOT_OK;
    SoAd_SoConIdType soConId_uo;
    DoIP_ConnectionType_tst* soCon_pst;

    /* Init submodules */
    DoIP_UDPConnection_Init();
    DoIP_TCPConnection_Init();

    /* Init socket connections */
    for (connIdx_u16 = 0; connIdx_u16 < DoIP_activeConfig_pcst->connSize_u16; connIdx_u16++)
    {
        soCon_pst = &DoIP_Connection_ast[connIdx_u16];
        soCon_pst->cfg_pcst = &DoIP_activeConfig_pcst->conn[connIdx_u16];
        soCon_pst->connIdx_u16 = connIdx_u16;
        soCon_pst->state_en = DOIP_CON_OFFLINE_E;
        soCon_pst->soAdConMode_en = SOAD_SOCON_OFFLINE;
        soCon_pst->closeNeeded_b = FALSE;

        retVal_u8 = SoAd_GetSoConId(soCon_pst->cfg_pcst->txPduId_uo, &soConId_uo);
        if (retVal_u8 != E_OK)
        {
            DOIP_DET(DOIP_DET_APIID_CONNECTION, DOIP_E_INVALID_PDU_SDU_ID);
            soConId_uo = DOIP_INVALID_SOCONDID;
        }
        soCon_pst->soConId_uo = soConId_uo;

        switch (soCon_pst->cfg_pcst->connectionKind_en)
        {
            case DoIP_Udp:
                DoIP_UDPConnection_Create(soCon_pst);
                break;
            case DoIP_Tcp:
                DoIP_TCPConnection_Create(soCon_pst);
                break;
            default:
                /* Nothing to do, can never be reached (enum)*/
                break;
        }
    }
}

void DoIP_Connection_SetCloseNeeded(DoIP_ConnectionType_tst *soCon_pst)
{
    soCon_pst->closeNeeded_b = TRUE;
}

void DoIP_Connection_SetResetNeeded(DoIP_ConnectionType_tst *soCon_pst)
{
    soCon_pst->resetNeeded_b = TRUE;
}

static void DoIP_Connection_CloseTcpUdp(const DoIP_ConnectionType_tst *soCon_pcst)
{
    switch (soCon_pcst->cfg_pcst->connectionKind_en)
    {
        case DoIP_Udp:
            DoIP_UDPConnection_Close(DoIP_SoCon_GetUDPCon(soCon_pcst));
            break;
        case DoIP_Tcp:
            DoIP_TCPConnection_Close(DoIP_SoCon_GetTCPCon(soCon_pcst));
            break;
        default:
            /* Nothing to do */
            break;
    }
}

void DoIP_Connection_Close(DoIP_ConnectionType_tst* soCon_pst)
{
    Std_ReturnType result_u8 = E_NOT_OK;

    /* If DoIP is already online AND [(SoAd Mode != ONLINE) OR (CloseNeeded == TRUE) OR (Activation Line Inactive)]  */
    if ( DOIP_CON_ONLINE_E == soCon_pst->state_en )
    {
        DoIP_Connection_CloseTcpUdp(soCon_pst);
        soCon_pst->state_en = DOIP_CON_CLOSE_NEEDED_E;
    }

    /* Once socket connection was not opened after successful IP Address Assignment and then activation line was inactive --> DoIP needs to go offline :
     * This might be the case for a TCP connection in listening mode or for a UDP connection with Wildcard Remote Address */
    if ( ( DOIP_CON_OFFLINE_OPEN_REQUESTED_E == soCon_pst->state_en ) && ( soCon_pst->soAdConMode_en != SOAD_SOCON_ONLINE ) )
    {
        if( FALSE == DoIP_OpenConnections_b )
        {
            soCon_pst->state_en = DOIP_CON_CLOSE_NEEDED_E;
        }
    }

    /* If DoIp needs to be closed and if Socket closure succeeded in SoAd, then DoIp connection State needs to go to DOIP_CON_CLOSE_REQUESTED_E
     *  else if SoAd is offline, then DoIP shall go offline : The second statement might be typically the case when DoIP is online but suddenly SoAd goes offline */
    if ( DOIP_CON_CLOSE_NEEDED_E == soCon_pst->state_en )
    {
        if( TRUE == soCon_pst->resetNeeded_b )
        {
            soCon_pst->resetNeeded_b = FALSE;
            result_u8 = SoAd_CloseSoCon ( soCon_pst->soConId_uo, TRUE );
        }
        else if ( TRUE == soCon_pst->closeNeeded_b )
        {
            soCon_pst->closeNeeded_b = FALSE;
            result_u8 = SoAd_CloseSoCon ( soCon_pst->soConId_uo, FALSE );
        }
        else
        {
            result_u8 = SoAd_CloseSoCon ( soCon_pst->soConId_uo, FALSE );
        }

        if ( E_OK == result_u8 )
        {
            soCon_pst->state_en = DOIP_CON_CLOSE_REQUESTED_E;
        }
        else if(SOAD_SOCON_OFFLINE == soCon_pst->soAdConMode_en)
        {
            soCon_pst->state_en = DOIP_CON_OFFLINE_E;
        }
        else
        {
            /* Ignore Request */
        }
    }

    /* Close for socket was requested and now is closed in SoAd (callback is called) */
    if ( ( DOIP_CON_CLOSE_REQUESTED_E == soCon_pst->state_en ) && ( soCon_pst->soAdConMode_en != SOAD_SOCON_ONLINE ) )
    {
        soCon_pst->state_en = DOIP_CON_OFFLINE_E;
    }
}

static void DoIP_Connection_OpenTcpUdp(const DoIP_ConnectionType_tst *soCon_pcst)
{
    switch (soCon_pcst->cfg_pcst->connectionKind_en)
    {
        case DoIP_Udp:
            DoIP_UDPConnection_Open(DoIP_SoCon_GetUDPCon(soCon_pcst));
            break;
        case DoIP_Tcp:
            DoIP_TCPConnection_Open(DoIP_SoCon_GetTCPCon(soCon_pcst));
            break;
        default:
            /* Nothing to do */
            break;
    }
}

void DoIP_Connection_Open ( DoIP_ConnectionType_tst* soCon_pst )
{
    /* Socket connection is online but SoAd goes suddenly offline or if socket shall be closed for one of the reasons described in the Standard */
    if ((TRUE == soCon_pst->closeNeeded_b)
     || (TRUE == soCon_pst->resetNeeded_b)
     || (( DOIP_CON_ONLINE_E == soCon_pst->state_en ) && (soCon_pst->soAdConMode_en != SOAD_SOCON_ONLINE)))
    {
        DoIP_Connection_Close ( soCon_pst );
    }

    /* Upon successful IP Address Assignment, Socket connection is offline --> Request SoAd Opening from SoAd Component! */
    if ( DOIP_CON_OFFLINE_E == soCon_pst->state_en )
    {
        if ( E_OK == SoAd_OpenSoCon ( soCon_pst->soConId_uo ) )
        {
            soCon_pst->state_en = DOIP_CON_OFFLINE_OPEN_REQUESTED_E;
        }
    }

    /* If SoAd open request was placed successfully and the SoAd was Opened --> DoIP can now go online! */
    if ( ( DOIP_CON_OFFLINE_OPEN_REQUESTED_E == soCon_pst->state_en ) && ( SOAD_SOCON_ONLINE == soCon_pst->soAdConMode_en ) )
    {
        soCon_pst->state_en = DOIP_CON_ONLINE_E;

        /* Connection (in SoAd) needs to be mapped to our internal connections (TCP/UDP) */
        DoIP_Connection_OpenTcpUdp(soCon_pst);
    }
}

void DoIP_Connection_SetUdpRemoteAddressToWildcard(const DoIP_ConnectionType_tst* soCon_pcst)
{
    /* Wildcard remote address for UDP */
    TcpIp_SockAddrInet6Type localAddr_st;
    uint8 netmask_u8;
    TcpIp_SockAddrInet6Type defaultRoute_st;
    TcpIp_SockAddrInetType wildcardAddr_st;
    TcpIp_SockAddrInet6Type wildcardAddr6_st;
    TcpIp_SockAddrType* udpAddr_pst;

    /* only, if remote address is configured as wildcard */
    if (soCon_pcst->cfg_pcst->hasUdpWildcardRemoteAddress_b)
    {
        wildcardAddr_st.domain = TCPIP_AF_INET;
        wildcardAddr_st.addr[0] = TCPIP_IPADDR_ANY;
        wildcardAddr_st.port = TCPIP_PORT_ANY;

        wildcardAddr6_st.domain = TCPIP_AF_INET6;
        wildcardAddr6_st.addr[0] = TCPIP_IP6ADDR_ANY;
        wildcardAddr6_st.addr[1] = TCPIP_IP6ADDR_ANY;
        wildcardAddr6_st.addr[2] = TCPIP_IP6ADDR_ANY;
        wildcardAddr6_st.addr[3] = TCPIP_IP6ADDR_ANY;
        wildcardAddr6_st.port = TCPIP_PORT_ANY;

        localAddr_st.domain = TCPIP_AF_INET6;
        defaultRoute_st.domain = TCPIP_AF_INET6;
        /* MR12 RULE 11.3 VIOLATION: Casting to different object pointer type is needed due to the by AUTOSAR
         * defined API for SoAd_GetLocalAddr, SoAd_SetRemoteAddr and the TcpIp_SockAddr-Types. */
        (void)SoAd_GetLocalAddr(soCon_pcst->soConId_uo, (TcpIp_SockAddrType*)&localAddr_st, &netmask_u8, (TcpIp_SockAddrType*)&defaultRoute_st);

        /* MR12 RULE 11.3 VIOLATION: Casting to different object pointer type is needed due to the by AUTOSAR
         * defined API for SoAd_GetLocalAddr, SoAd_SetRemoteAddr and the TcpIp_SockAddr-Types. */
        udpAddr_pst = (TcpIp_SockAddrType*)&wildcardAddr6_st;
        if (localAddr_st.domain == TCPIP_AF_INET)
        {
            /* MR12 RULE 11.3 VIOLATION: Casting to different object pointer type is needed due to the by AUTOSAR
             * defined API for SoAd_GetLocalAddr, SoAd_SetRemoteAddr and the defined TcpIp_SockAddr-Types. */
            udpAddr_pst = (TcpIp_SockAddrType*)&wildcardAddr_st;
        }

        (void)SoAd_SetRemoteAddr(soCon_pcst->soConId_uo, udpAddr_pst);
    }
}

/* Only called after init */
boolean DoIP_Connection_GetConnIdxForSoConId(SoAd_SoConIdType soConId_uo, uint16* connIdx_u16)
{
    uint16 counter_u16;
    boolean retVal_b = FALSE;

    for (counter_u16 = 0; counter_u16 < DoIP_activeConfig_pcst->connSize_u16; counter_u16++)
    {
        if (DoIP_Connection_ast[counter_u16].soConId_uo == soConId_uo)
        {
            *connIdx_u16 = counter_u16;
            retVal_b = TRUE;
            break;
        }
    }
    return retVal_b;
}

boolean DoIP_SoCon_IsSoAdSoConIdValid(SoAd_SoConIdType soConId_uo)
{
    uint16 counter_u16;
    boolean retVal_b = FALSE;

    for (counter_u16 = 0; counter_u16 < DoIP_activeConfig_pcst->connSize_u16; counter_u16++)
    {
        if (DoIP_Connection_ast[counter_u16].soConId_uo == soConId_uo)
        {
            retVal_b = TRUE;
            break;
        }
    }
    return retVal_b;
}

DoIP_ConnectionType_tst* DoIP_SoCon_GetForSoConId(SoAd_SoConIdType soConId_uo)
{
    uint16 counter_u16;
    DoIP_ConnectionType_tst* soCon_pcst = NULL_PTR;

    for (counter_u16 = 0; counter_u16 < DoIP_activeConfig_pcst->connSize_u16; counter_u16++)
    {
        if (DoIP_Connection_ast[counter_u16].soConId_uo == soConId_uo)
        {
            soCon_pcst = &DoIP_Connection_ast[counter_u16];
            break;
        }
    }
    return soCon_pcst;
}

PduIdType DoIP_SoCon_GetSoAdPdu(const DoIP_ConnectionType_tst* soCon_pcst)
{
    return soCon_pcst->cfg_pcst->txPduId_uo;
}

DoIP_ConnectionType_tst* DoIP_SoCon_GetSocketConnection(uint16 connIdx_u16)
{
    return &DoIP_Connection_ast[connIdx_u16];
}

boolean DoIP_SoCon_IsActive(const DoIP_ConnectionType_tst* soCon_pcst)
{
    return (soCon_pcst->state_en == DOIP_CON_ONLINE_E);
}

boolean DoIP_SoCon_IsUdpIndex(uint16 connIdx_u16)
{
    return (connIdx_u16 < DoIP_activeConfig_pcst->udpConnSize_u8);
}

boolean DoIP_SoCon_IsTcpIndex(uint16 connIdx_u16)
{
    return ((connIdx_u16 >= DoIP_activeConfig_pcst->udpConnSize_u8) && (connIdx_u16 < DoIP_activeConfig_pcst->connSize_u16));
}

/* This functions is used to determine if IpAddress can be released.
 * From AUTOSAR requirements, only the UDP connections need to be considered for this */
boolean DoIP_SoCon_AllRelevantConnectionsOffline(void)
{
    uint16 connIdx_u16;
    boolean retVal_b = TRUE;

    for (connIdx_u16 = 0; connIdx_u16 < DoIP_activeConfig_pcst->udpConnSize_u8; connIdx_u16++)
    {
        retVal_b = retVal_b && (DoIP_Connection_ast[connIdx_u16].state_en == DOIP_CON_OFFLINE_E);
    }

    return retVal_b;
}

/* Attention! All connections are summerized in the construct DoIP_Connection_ast. The first part of the array is the part with
 * all UDP-Connections and in the second part the TCP-Connections. For access of the TCP-Connections the number of DUP-Connections
 * from DoIP_activeConfig_pcst is needed (udpConnSize_u8).
 */


DoIP_Type_UDPConnection_tst* DoIP_SoCon_GetUDPCon(const DoIP_ConnectionType_tst* soCon_pcst)
{
    return DoIP_UDPConnection_Get(soCon_pcst->connIdx_u16);
}

DoIP_Type_TCPConnection* DoIP_SoCon_GetTCPCon(const DoIP_ConnectionType_tst* soCon_pcst)
{
    return DoIP_TCPConnection_Get(soCon_pcst->connIdx_u16 - DoIP_activeConfig_pcst->udpConnSize_u8);
}

void DoIP_Connection_MainFunction(void)
{
    uint16 idx_u16;
    DoIP_ConnectionType_tst* soCon_pcst;

    for (idx_u16 = 0; idx_u16 < DoIP_activeConfig_pcst->connSize_u16; idx_u16++)
    {
        soCon_pcst = DoIP_SoCon_GetSocketConnection(idx_u16);

        ( DoIP_OpenConnections_b == TRUE ) ? DoIP_Connection_Open(soCon_pcst): DoIP_Connection_Close(soCon_pcst);

        if (DoIP_SoCon_IsActive(soCon_pcst))
        {
            switch (soCon_pcst->cfg_pcst->connectionKind_en)
            {
                case DoIP_Udp:
                    DoIP_UDPConnection_MainFunction(DoIP_SoCon_GetUDPCon(soCon_pcst));
                    break;
                case DoIP_Tcp:
                    DoIP_TCPConnection_MainFunction(DoIP_SoCon_GetTCPCon(soCon_pcst));
                    break;
                default:
                    /* Nothing to do */
                    break;
            }
        }
    }
}

void DoIP_SetRemoteAddressToWildCard(DoIP_Type_UDPConnection_tst* udpCon_pst)
{
    uint8 queueIdx_u8;
    boolean emptyQueue_b = TRUE;
    /* Set remote address for UDP socket back to wildcard, if it was configured as wildcard. */
    /* todo:think about usecase that they can change the tester if we reset remote address */
    for (queueIdx_u8 = 0; queueIdx_u8 < DOIP_CFG_MaxUDPQueueSize; queueIdx_u8++)
    {
        if(udpCon_pst->DoIP_UDPQueue_au8[queueIdx_u8].udpQueueState_u8 != DOIP_UDP_DATA_EMPTY)
        {
            emptyQueue_b = FALSE;
            break;
        }
    }

    if(emptyQueue_b == TRUE)
    {
        DoIP_Connection_SetUdpRemoteAddressToWildcard(udpCon_pst->soCon_pcst);
        udpCon_pst->setUdpRemoteAddressToWildcardWhenQueueEmpty_b = FALSE; //reset the confirmation reminder bit
    }
    else
    {
        udpCon_pst->setUdpRemoteAddressToWildcardWhenQueueEmpty_b = TRUE;
    }
}

#define DOIP_STOP_SEC_CODE
#include "DoIP_MemMap.h"

