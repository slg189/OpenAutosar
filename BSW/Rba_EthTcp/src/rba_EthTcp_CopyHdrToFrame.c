

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

#include "TcpIp_Lib.h"
#include "TcpIp_Prv.h"

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"


 /*
 ***************************************************************************************************
 * Local functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/*********************************************************************************************************************************/
/** rba_EthTcp_SetHdrToFrame    - Copy the TCP header into the Tx buffer provided by EthDrv                                     **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo         - Tcp dynamic socket table index                                                                **/
/** TxEthDrvBuf_pu8             - Pointer to the EthDrv Tx buffer, from where the TCP header starts                             **/
/**                                                                                                                             **/
/** Parameters (inout):           None                                                                                          **/
/**                                                                                                                             **/
/** Parameters (out):             None                                                                                          **/
/**                                                                                                                             **/
/** Return value:                 None                                                                                          **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_SetHdrToFrame( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                               uint8 *            TxEthDrvBuf_pu8 )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    uint16                          lHdrLen_u16;
    uint16                          lOptionLen_u16;

    /* Get Tcp dynamic table entry from Tcp socket index */
    lTcpDynSockTbl_pst  = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* -------------------------------------- */
    /* Compute Tcp header length              */
    /* -------------------------------------- */

    /* Get the length of the Tcp options */
    lOptionLen_u16 = rba_EthTcp_GetOptionLength( TcpDynSockTblIdx_uo );

    /* Compute the complete length of the Tcp header */
    lHdrLen_u16 = RBA_ETHTCP_DFL_HDRLENGTH + lOptionLen_u16;

    /* --------------------------------------------------------- */
    /* Copy the Tcp header into the Tx buffer provided by EthDrv */
    /* --------------------------------------------------------- */
    RBA_ETHTCP_WRITE_16BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_SRCPORT_E, lTcpDynSockTbl_pst->LocalPort_u16 )
    RBA_ETHTCP_WRITE_16BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_DESTPORT_E, lTcpDynSockTbl_pst->RemotePort_u16 )
    RBA_ETHTCP_WRITE_32BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_SEQNUM_E, lTcpDynSockTbl_pst->SndSeqNum_u32 )
    RBA_ETHTCP_WRITE_32BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_ACKNUM_E, lTcpDynSockTbl_pst->SndAckNum_u32 )
    RBA_ETHTCP_WRITE_8BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_OFFSET_E, (uint8)((lHdrLen_u16/4U) << 4U) )
    RBA_ETHTCP_WRITE_8BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_FLAGS_E, lTcpDynSockTbl_pst->SndHdrFlag_u8 )
    RBA_ETHTCP_WRITE_16BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_WNDSIZE_E, lTcpDynSockTbl_pst->SndWndSize_u16 )
    RBA_ETHTCP_WRITE_16BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_CHKSUM_E, (uint16)0U )       /* Checksum higher byte. Checksum is updated by HW or SW */
    RBA_ETHTCP_WRITE_16BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_URGPTR_E, (uint16)0U )       /* Urgent pointer higher byte. Urgent pointer not yet supported */

    return;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_GetOptionLength()  - Returns the length of the Tcp options that will be included in the Tcp header               **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo           - Tcp dynamic socket table index                                                              **/
/**                                                                                                                             **/
/** Parameters (inout):             None                                                                                        **/
/**                                                                                                                             **/
/** Parameters (out):               None                                                                                        **/
/**                                                                                                                             **/
/** Return value:                 - uint16                                                                                      **/
/**                                 Length of the Tcp options that will be included in the Tcp header                           **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
uint16 rba_EthTcp_GetOptionLength( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    uint16 lOptionLen_u16;

    /* Initialize the Tcp option length to 0 */
    lOptionLen_u16 = 0U;

    /* ------------------------------ */
    /* Options within SYN or SYN-ACK  */
    /* ------------------------------ */

    /* If the next frame to send is a SYN or SYN-ACK */
    if ( (rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].TcpFlag_en == RBA_ETHTCP_SEND_SYN_E)       ||
         (rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].TcpFlag_en == RBA_ETHTCP_SEND_SYN_ACK_E) )
    {
        /* ------------------------------ */
        /* Length of MSS option           */
        /* ------------------------------ */
#if ( RBA_ETHTCP_MSS_ENABLED == STD_ON )
        /* Add the MSS option length in the Tcp header length */
        lOptionLen_u16 += RBA_ETHTCP_OPT_LENGTH_MSS;
#endif

        /* ------------------------------ */
        /* Length of UTO option           */
        /* ------------------------------ */
#if ( RBA_ETHTCP_USER_TIMEOUT_ENABLED == STD_ON )
        /* Add the UTO option length in the Tcp header length */
        lOptionLen_u16 += RBA_ETHTCP_OPT_LENGTH_UTO;
#endif
    }

    return lOptionLen_u16;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_SetOptionsToFrame    - Copy the TCP options into the Tx buffer provided by EthDrv                                **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/** TxEthDrvBuf_pu8                 - Pointer to the EthDrv Tx buffer, from where the TCP header starts                         **/
/**                                                                                                                             **/
/** Parameters (inout):               None                                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                 None                                                                                      **/
/**                                                                                                                             **/
/** Return value:                   - Std_ReturnType                                                                            **/
/**                                   E_OK: The request has been accepted                                                       **/
/**                                   E_NOT_OK: The request has not been accepted                                               **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_SetOptionToFrame( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                  uint8 *            TxEthDrvBuf_pu8 )
{
    /* Declare local variables */
    uint8 * lOptionBuf_pu8;

    /* --------------------------------------------------------- */
    /* Compute pointer to the beginning of Tcp Options           */
    /* --------------------------------------------------------- */
    lOptionBuf_pu8 = ( TxEthDrvBuf_pu8 + RBA_ETHTCP_DFL_HDRLENGTH );

    /* ------------------------------ */
    /* Options within SYN or SYN-ACK  */
    /* ------------------------------ */

    /* If the next frame to send is a SYN or SYN-ACK */
    if ( (rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].TcpFlag_en == RBA_ETHTCP_SEND_SYN_E)       ||
         (rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].TcpFlag_en == RBA_ETHTCP_SEND_SYN_ACK_E) )
    {
        /* ------------------------------ */
        /* Copy MSS option                */
        /* ------------------------------ */
#if ( RBA_ETHTCP_MSS_ENABLED == STD_ON )
        /* Copy the kind of Tcp options */
        RBA_ETHTCP_WRITE_8BITS_AT_POS( lOptionBuf_pu8, (uint8)RBA_ETHTCP_POS_OPT_KIND_E, RBA_ETHTCP_OPT_KIND_MSS )

        /* Copy the length of Tcp options */
        RBA_ETHTCP_WRITE_8BITS_AT_POS( lOptionBuf_pu8, (uint8)RBA_ETHTCP_POS_OPT_LEN_E, RBA_ETHTCP_OPT_LENGTH_MSS )

        /* Copy the MSS value in the EthDrv Tx buffer */
        RBA_ETHTCP_WRITE_16BITS_AT_POS( lOptionBuf_pu8, (uint8)RBA_ETHTCP_POS_OPT_VAL_E, rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo].LocalMss_u16 )

        /* Move the pointer to the end of MSS option */
        lOptionBuf_pu8 += RBA_ETHTCP_OPT_LENGTH_MSS;
#endif

        /* ------------------------------ */
        /* Copy UTO option                */
        /* ------------------------------ */
#if ( RBA_ETHTCP_USER_TIMEOUT_ENABLED == STD_ON )
        /* Copy the kind of Tcp options */
        RBA_ETHTCP_WRITE_8BITS_AT_POS( lOptionBuf_pu8, (uint8)RBA_ETHTCP_POS_OPT_KIND_E, RBA_ETHTCP_OPT_KIND_UTO )

        /* Copy the length of Tcp options */
        RBA_ETHTCP_WRITE_8BITS_AT_POS( lOptionBuf_pu8, (uint8)RBA_ETHTCP_POS_OPT_LEN_E, RBA_ETHTCP_OPT_LENGTH_UTO )

        /* Copy the UTO value in the EthDrv Tx buffer */
        RBA_ETHTCP_WRITE_16BITS_AT_POS( lOptionBuf_pu8, (uint8)RBA_ETHTCP_POS_OPT_VAL_E, (uint16)(rba_EthTcp_CurrConfig_cpo->UserTimeOut_u32 / 1000U) )

        /* Copy the UTO granularity in the EthDrv Tx buffer */
        /* (UTO granularity is 1 bit long) */
        RBA_ETHTCP_WRITE_BIT_AT_POS( lOptionBuf_pu8, (uint8)RBA_ETHTCP_POS_OPT_VAL_E, 7U, 0U )

        /* Move the pointer to the end of UTO option */
        lOptionBuf_pu8 += RBA_ETHTCP_OPT_LENGTH_UTO;
#endif
    }

    return;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_SetChecksumToFrame() - Update checksum field of the Tcp header into the Tx buffer provided by EthDrv             **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo             - Tcp dynamic socket table index                                                            **/
/** TxEthDrvBuf_pu8                 - Pointer to the Tx buffer, from where the TCP header starts                                **/
/** TotalLength_u16                 - Total length of Tcp frame (in bytes)                                                      **/
/**                                                                                                                             **/
/** Parameters (inout):               None                                                                                      **/
/**                                                                                                                             **/
/** Parameters (out):                 None                                                                                      **/
/**                                                                                                                             **/
/** Return value:                                                                                                               **/
/** Std_ReturnType                  - E_OK if update of checksum field is successful                                            **/
/**                                 - E_NOT_OK if update of checksum field failed                                               **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
#if ( RBA_ETHTCP_SW_CHKSUM_TX == STD_ON )
Std_ReturnType rba_EthTcp_SetChecksumToFrame( TcpIp_SocketIdType TcpDynSockTblIdx_uo,
                                              uint8 * TxEthDrvBuf_pu8,
                                              uint16 TotalLength_u16 )
{
    /* Declare local variables */
    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;
    rba_EthTcp_DynSockTblType_tst *  lTcpDynSockTbl_pst;
    TcpIp_IPAddrParamType_tun *      lIPAddrParamType_pun;
    TcpIp_PseudoHdr_tst              lPseudoHdr_st;
    uint16                           lTcpChksum_u16;
    Std_ReturnType                   lFunctionRetVal_en;
    Std_ReturnType                   lRetVal_en;

    /* Local variable initialisation */
    lFunctionRetVal_en              = E_NOT_OK; /* Initialize the return value of the function to E_NOT_OK */
    lTcpDynSockTbl_pst              = &(rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo]);
    lRetVal_en                      = TcpIp_GetLocalAddrIdProperties( lTcpDynSockTbl_pst->LocalAddrId_u8, &lLocalAddrConfig_pcst );

    if( lRetVal_en == E_OK )
    {
        /* ------------------------------ */
        /* Create Pseudo-Header           */
        /* ------------------------------ */

        /* Create the Tcp pseudo header ( protocol type, local IP and Remote IP ) to calculate Tcp checksum */
        lPseudoHdr_st.ProtoType_en      = TCPIP_IPPROTO_TCP;
        lPseudoHdr_st.IpDomainType_t    = lTcpDynSockTbl_pst->DomainType_u32;
        lIPAddrParamType_pun            = &(lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un);

        switch( lTcpDynSockTbl_pst->DomainType_u32 )
        {
            #if ( TCPIP_IPV4_ENABLED == STD_ON )
            case TCPIP_AF_INET:
            {
                lPseudoHdr_st.LocalIpAddr_un.IPv4Addr_u32       = lIPAddrParamType_pun->IPv4Par_st.IpAddr_u32;      /* Copy source IPv4 address from TcpIp Local Addr cfg */
                lPseudoHdr_st.RemoteIpAddr_un.IPv4Addr_u32      = lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv4Addr_u32; /* Copy dest IPv4 address from EthTcp dynamic socket cfg */
            }
            break;
            #endif

            #if ( TCPIP_IPV6_ENABLED == STD_ON )
            case TCPIP_AF_INET6:
            {
                lPseudoHdr_st.LocalIpAddr_un.IPv6Addr_pcu32     = &(lIPAddrParamType_pun->IPv6Par_st.IpAddr_st.Addr_au32[0]); /* Copy source IPv6 address from TcpIp Local Addr cfg */
                lPseudoHdr_st.RemoteIpAddr_un.IPv6Addr_pcu32    = &(lTcpDynSockTbl_pst->RemoteIPAddr_un.IPv6Addr_au32[0]);    /* Copy dest IPv6 address from EthTcp dynamic socket cfg */
            }
            break;
            #endif

            default:
            {
                /* If execution reaches here, then the IP support for the specified domain is not enabled; set return value to E_NOT_OK. */
                lRetVal_en = E_NOT_OK;
            }
            break;
        }

        if( lRetVal_en == E_OK )
        {
            /* ------------------------------ */
            /* Checksum computation           */
            /* ------------------------------ */

            /* Call the checksum calculation function */
            /* MR12 DIR 1.1 VIOLATION: The input parameters of TcpIp_OnesComplChkSum() are declared as (void*) for generic implementation. */
            lRetVal_en = TcpIp_OnesComplChkSum( TxEthDrvBuf_pu8,
                                                &lPseudoHdr_st,
                                                TotalLength_u16,
                                                &lTcpChksum_u16);

            if( lRetVal_en == E_OK )
            {
                /* Copy the calculated checksum in the corresponding field of the Tcp header */
                RBA_ETHTCP_WRITE_16BITS_AT_POS( TxEthDrvBuf_pu8, (uint8)RBA_ETHTCP_POS_CHKSUM_E, lTcpChksum_u16 )

                lFunctionRetVal_en = E_OK;
            }
        }
    }

    return lFunctionRetVal_en;
}
#endif


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
