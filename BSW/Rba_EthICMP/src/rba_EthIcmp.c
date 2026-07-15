

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"

#if ( defined(TCPIP_ICMP_ENABLED) && ( TCPIP_ICMP_ENABLED == STD_ON ) )

#include "rba_EthIcmp.h"

#if( RBA_ETHICMP_MSG_FRAGMENTATION_ALLOWED == STD_ON )
#include "rba_EthIcmp_Cfg_SchM.h"
#endif

#include "rba_EthIcmp_Cbk.h"

#if (TCPIP_TCP_ENABLED == STD_ON)
#include "rba_EthTcp.h"
#endif

#if (TCPIP_IPV4_ENABLED == STD_ON)
#include "rba_EthIPv4.h"
#endif

#include "TcpIp_Prv.h"
#include "TcpIp_Lib.h"
#include "rba_EthIcmp_Prv.h"

#if ( TCPIP_DEV_ERROR_DETECT != STD_OFF )
#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif
#endif /* ( TCPIP_DEV_ERROR_DETECT != STD_OFF ) */

#if(STD_ON == TCPIP_DEM_CONFIGURED)

/* DEM AR version check */
# if (!defined(DEM_AR_RELEASE_MAJOR_VERSION) || (DEM_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#  error "DEM AUTOSAR major version undefined or mismatched."
# endif
# if (!defined(DEM_AR_RELEASE_MINOR_VERSION) || (DEM_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#  error "DEM AUTOSAR major version undefined or mismatched."
# endif

#endif

/*
 ***************************************************************************************************
 * Global Variables
 ***************************************************************************************************
 */

#define RBA_ETHICMP_START_SEC_VAR_CLEARED_16
#include "rba_EthIcmp_MemMap.h"
static uint16 rba_EthIcmp_SeqNumCnt_u16 = 0;
#define RBA_ETHICMP_STOP_SEC_VAR_CLEARED_16
#include "rba_EthIcmp_MemMap.h"

#define RBA_ETHICMP_START_SEC_VAR_INIT_8
#include "rba_EthIcmp_MemMap.h"
/* Flag to indicate whether rba_EthIcmp_Init has been executed or not since power ON */
boolean rba_EthIcmp_InitFlag_b = FALSE;
#define RBA_ETHICMP_STOP_SEC_VAR_INIT_8
#include "rba_EthIcmp_MemMap.h"

#define RBA_ETHICMP_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthIcmp_MemMap.h"
/* ICMP configuration pointer */
const rba_EthIcmp_Config_to * rba_EthIcmp_CurrConfig_pco = NULL_PTR;
#define RBA_ETHICMP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthIcmp_MemMap.h"

#define RBA_ETHICMP_START_SEC_CODE
#include "rba_EthIcmp_MemMap.h"

/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

static Std_ReturnType rba_EthIcmp_Prv_Transmit_DetChecks( const TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                          const TcpIp_SockAddrInetType * RemoteAddr_cpst,
                                                          const uint8 * TxData_cpu8 );

/********************************************************************************************************************************/
/* rba_EthTcp_Init()    - By this API service the rba_EthIcmp module is initialised if the configuration pointer is valid       */
/*                                                                                                                              */
/* Parameters (in):                                                                                                             */
/* IcmpConfig_pco       - Pointer to the configuration data of the rba_EthIcmp module.                                          */
/*                                                                                                                              */
/* Parameters (inout):  None                                                                                                    */
/*                                                                                                                              */
/* Parameters (out):    None                                                                                                    */
/*                                                                                                                              */
/* Return value:        None                                                                                                    */
/*                                                                                                                              */
/********************************************************************************************************************************/
void rba_EthIcmp_Init( const rba_EthIcmp_Config_to * IcmpConfig_pco )
{

    /* Initialise the Init flag */
    rba_EthIcmp_InitFlag_b    =   FALSE;
    rba_EthIcmp_CurrConfig_pco =   NULL_PTR;

    /* Report DET if TcpIp_CurrConfig_pco is not intialized */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VOID( ( NULL_PTR == TcpIp_CurrConfig_pco ),                              \
                                           RBA_ETHICMP_E_INIT_API_ID, RBA_ETHICMP_E_INIT_FAILED )

    /* Report DET if TcpIp module is uninitialised */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VOID( ( IcmpConfig_pco != ( TcpIp_CurrConfig_pco->EthIcmpConfig_pco) ),   \
                                           RBA_ETHICMP_E_INIT_API_ID, RBA_ETHICMP_E_INV_ARG )

    /* Update the ICMP config pointer */
    rba_EthIcmp_CurrConfig_pco = IcmpConfig_pco;

    /* Set a flag, to indicate that the rba_EthIcmp_Init() has been called */
    rba_EthIcmp_InitFlag_b = TRUE;

    return;
}


/********************************************************************************************************************/
/* TcpIp_IcmpTransmit() - By this API service TCP/IP stack sends an ICMP message according to specified parameters. */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8 - IP address identifier representing the local IP address and EthIf controller which shall be    */
/*                   used for transmission of the ICMP message.                                                     */
/*  RemoteAddr_cpst- Pointer to structure representing the remote address.                                           */
/*  Ttl_u8        - Time to live value to be used for the ICMP message. If 0 is specified the default value shall   */
/*                  be used.                                                                                        */
/*  Type_u8       - Type field value to be used in the ICMP message                                                 */
/*                  Note: the value of the type field determines the format of the remaining ICMP message data      */
/*  Code_u8       - Code field value to be used in the ICMP message                                                 */
/*  DataLength_u16- Length of ICMP message                                                                          */
/*  TxData_cpu8    - Pointer to data which shall be sent as ICMP message data                                        */
/*                                                                                                                  */
/* Return   - Result of operation :                                                                                 */
/*               E_OK The request has been accepted                                                                 */
/*               E_NOT_OK The request has not been accepted                                                         */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType rba_EthIcmp_Transmit (
                                        TcpIp_LocalAddrIdType LocalAddrId_u8,
                                        const TcpIp_SockAddrInetType * RemoteAddr_cpst,
                                        uint8 Ttl_u8,
                                        uint8 Type_u8,
                                        uint8 Code_u8,
                                        uint16 DataLength_u16,
                                        const uint8 * TxData_cpu8 )
{
    /* Local variable declaration */
    BufReq_ReturnType lBufReqRetVal_en;
    TcpIp_ReturnType lIPv4TxRetVal_en;
    Std_ReturnType lRetVal_en;
    TcpIp_IpHeader_tst lIpHeader_st;
    uint8 * lTxDatBuf_pu8;
    uint16 lIdx_u16;
    uint16 lLength_u16;
    uint16 lDataOffset_u16;
    uint8 lBufIdx_u8;
    uint8 lFramePrio_u8;
    boolean lMessageMtuExceeded_b;

    /* Local variable initialisation */
    lRetVal_en                  = E_NOT_OK;
    lBufReqRetVal_en            = BUFREQ_E_NOT_OK;
    lIpHeader_st.ProtoType_en   = TCPIP_IPPROTO_NONE;
    lIpHeader_st.Ttl_u8         = 0;
    lIpHeader_st.SockOwner_u8   = 0xFFU;
    lIpHeader_st.FlowLabel_u32  = 0UL;
    lMessageMtuExceeded_b       = FALSE;

    /* Check the development errors for the API. */
    lRetVal_en = rba_EthIcmp_Prv_Transmit_DetChecks( LocalAddrId_u8,
                                                     RemoteAddr_cpst,
                                                     TxData_cpu8 );

    /* If development errors are detected, then exit from the API without further processing. */
    if( lRetVal_en != E_NOT_OK )
    {
        /* Re-intialize return value to E_NOT_OK. */
        lRetVal_en = E_NOT_OK;

        /* Increment the message length by Icmp header length */
        lLength_u16 = ( RBA_ETHICMP_PRV_HDR_LENGTH + DataLength_u16 );

        /* Check if the length ( Icmp header + data length + IPv4 header length) to be transmitted is greater than the size of the EthIfCtrlMTU. */
        /* And the message type is echo reply.  */
        if( ( ( lLength_u16 + RBA_ETHIPV4_HDRLENGTH ) > rba_EthIcmp_CurrConfig_pco->IcmpSubConfig_pco[LocalAddrId_u8].EthIfCtrlMtu_cu16 ) &&
            ( (uint8) RBA_ETHICMP_ECHO_REPLY_E == Type_u8 ) )
        {
            /* Set flag to indicate that the echo reply message length exceeds MTU size. */
            lMessageMtuExceeded_b = TRUE;

            /* ICMP echo reply may have data length greater than the EthIfCtrl MTU size. This will result in fragmentation in IPV4 layer. */
            /* The compiler switch if enabled, will truncate the ICMP echo reply to equal to or less than the EthIf Ctrl MTU size. */
#if( RBA_ETHICMP_MSG_FRAGMENTATION_ALLOWED == STD_OFF )
            /* If yes, truncate the size of data to be equal to the MTU so that fragmentation does not occur in IPv4. */
            lLength_u16 = rba_EthIcmp_CurrConfig_pco->IcmpSubConfig_pco[LocalAddrId_u8].EthIfCtrlMtu_cu16 - RBA_ETHIPV4_HDRLENGTH;

#else
            /* Enter critical section: rba_EthIPv4_ProvideTxBuffer() and rba_EthIPv4_Transmit() APIs shall be called in an atomic section, if ICMP messages (echo reply) */
            /*  having length greater than the size of MTU, are transmitted in interrupt context. */
            SchM_Enter_rba_EthIcmp_ExclusiveAreaIcmpFragmentedMsg();
#endif
        }


        /* Get the statically configured frame priority (TcpIpIpFramePrioDefault) */
        lFramePrio_u8 = rba_EthIcmp_CurrConfig_pco->IcmpSubConfig_pco[LocalAddrId_u8].TcpIpCtrlFramePrio_cu8;

        /* Call the rba_EthIPv4_ProvideTxBuffer() to allocate memory for the data to be transmitted */
        lBufReqRetVal_en = rba_EthIPv4_ProvideTxBuffer(
                                            LocalAddrId_u8,
                                            ( RemoteAddr_cpst->addr[0] ),
                                            lFramePrio_u8,
                                            &lBufIdx_u8,
                                            &lTxDatBuf_pu8,
                                            &lLength_u16 );

        /* If buffer of requested length is allocated */
        if( BUFREQ_OK == lBufReqRetVal_en )
        {
            /* Prepare the ICMP header */
            ( lTxDatBuf_pu8[ RBA_ETHICMP_PRV_TYPE_OFFSET ] )        = Type_u8;
            ( lTxDatBuf_pu8[ RBA_ETHICMP_PRV_CODE_OFFSET ] )        = Code_u8;
            /* Initialise the checksum fields to zero for checksum calculation */
            ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_CHKSUM_OFFSET] )        = 0x00U;
            ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_CHKSUM_OFFSET + 1U] )   = 0x00U;

            switch ( Type_u8 )
            {
                 case (uint8) RBA_ETHICMP_ECHO_REQ_E:
                {
                    /* Increment sequence number for echo transmit request */
                    ( rba_EthIcmp_SeqNumCnt_u16++ );
                    TcpIp_WriteU16( &lTxDatBuf_pu8[RBA_ETHICMP_PRV_IDENTIFIER_OFFSET],  RBA_ETHICMP_PRV_IDENTIFIER_FIELD );
                    TcpIp_WriteU16( &lTxDatBuf_pu8[RBA_ETHICMP_PRV_SEQNUM_OFFSET],      rba_EthIcmp_SeqNumCnt_u16 );
                    lDataOffset_u16 = 0;
                    break;
                }

                case (uint8) RBA_ETHICMP_ECHO_REPLY_E:
                {   /* Move the pointer to identifier offset */
                    TxData_cpu8 = ( TxData_cpu8 - RBA_ETHICMP_PRV_HDR_LENGTH_PARTIAL );
                    ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_IDENTIFIER_OFFSET] )        = TxData_cpu8[0];
                    ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_IDENTIFIER_OFFSET + 1U] )   = TxData_cpu8[1];
                    ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_SEQNUM_OFFSET] )            = TxData_cpu8[2];
                    ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_SEQNUM_OFFSET + 1U] )       = TxData_cpu8[3];
                    /* The received ICMP header should be copied into the transmit buffer. Hence add offset of 4 */
                    lDataOffset_u16                                             = RBA_ETHICMP_PRV_HDR_LENGTH_PARTIAL;
                    break;
                }

                case (uint8) RBA_ETHICMP_DEST_UNREACH_E:
                default:
                {   /* Identifier field and Sequence are unused for Destination Unreachable ICMP frame */
                    ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_IDENTIFIER_OFFSET] )        = 0;
                    ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_IDENTIFIER_OFFSET + 1U] )   = 0;
                    ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_SEQNUM_OFFSET] )            = 0;
                    ( lTxDatBuf_pu8[RBA_ETHICMP_PRV_SEQNUM_OFFSET + 1U] )       = 0;
                    lDataOffset_u16                                             = 0;
                    break;
                }
            } /* end - if( ( RBA_ETHICMP_PRV_HDR_LENGTH + DataLength_u16 ) <= lLength_u16 ) */

            /* Copy data into the transmit buffer */
            for( lIdx_u16 = 0; lIdx_u16 < ( lLength_u16 - RBA_ETHICMP_PRV_HDR_LENGTH ); lIdx_u16++ )
            {
                ( lTxDatBuf_pu8[ lIdx_u16 + RBA_ETHICMP_PRV_HDR_LENGTH ] ) = ( TxData_cpu8[ lIdx_u16 + lDataOffset_u16 ] );
            }

            /* Update the protocol type to ICMP in the IP header content structure */
            lIpHeader_st.ProtoType_en = TCPIP_IPPROTO_ICMP;

            /* Initialize DSCP value to 0. (Setting of DSCP value is not yet supported from ICMP.) */
            lIpHeader_st.DiffServiceCodepoint_u8 = 0u;

            if( 0 == Ttl_u8 )
            {
                /* If the passed Time-to-live value is 0, configured default value is used */
                lIpHeader_st.Ttl_u8 = rba_EthIcmp_CurrConfig_pco->Ttl_u8;
            }
            else
            {
                /* Pass the Time-to-live value as it is, if it is not 0 */
                lIpHeader_st.Ttl_u8 = Ttl_u8;
            }

            /* Send the ICMP packet */
            lIPv4TxRetVal_en = rba_EthIPv4_Transmit (   LocalAddrId_u8,
                                                        ( RemoteAddr_cpst->addr[0] ),
                                                        lTxDatBuf_pu8,
                                                        lBufIdx_u8,
                                                        &lIpHeader_st,
                                                        lLength_u16 );

            /* Check if the transmitted echo reply message had exceeded the MTU size. */
            if( lMessageMtuExceeded_b != FALSE )
            {
                /* If the transmitted message had exceeded the MTU size, then critical section needs to be stopped if fragmentation was allowed. */
#if( RBA_ETHICMP_MSG_FRAGMENTATION_ALLOWED == STD_ON )
                /* Exit critical section. */
                SchM_Exit_rba_EthIcmp_ExclusiveAreaIcmpFragmentedMsg();
#endif
            }

            if( TCPIP_OK == lIPv4TxRetVal_en )
            {
                lRetVal_en = E_OK;
            }
        } /* end - if( BUFREQ_OK == lBufReqRetVal_en ) */
    } /* end - if(lRetVal_en != E_NOT_OK) */

    return ( lRetVal_en );
}

/********************************************************************************************************************/
/* rba_EthIcmp_Prv_Transmit_DetChecks() - This API checks the development errors (DETs) for                         */
/*                                            rba_EthIcmp_Transmit() API                                            */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8  - constant IP address identifier representing the local IP address and EthIf controller which   */
/*                    shall be used for transmission of the ICMP message.                                           */
/*                                                                                                                  */
/*  RemoteAddr_cpst - Pointer to constant  destination address  of the receiving node                               */
/*                                                                                                                  */
/*  TxData_cpu8     - pointer to the constant starting address of ICMP header                                       */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : No development error detected. The function shall do further processing.            */
/*                     - E_NOT_OK : Development error detected. The function shall exit without processing further. */
/*                                                                                                                  */
/********************************************************************************************************************/
static Std_ReturnType rba_EthIcmp_Prv_Transmit_DetChecks( const TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                          const TcpIp_SockAddrInetType * RemoteAddr_cpst,
                                                          const uint8 * TxData_cpu8 )
{
    /* Report DET if rba_EthIcmp module is uninitialised */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == rba_EthIcmp_InitFlag_b ), RBA_ETHICMP_E_TRANSMIT_API_ID,      \
                                            RBA_ETHICMP_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),   \
                                            RBA_ETHICMP_E_TRANSMIT_API_ID, RBA_ETHICMP_E_INV_ARG, E_NOT_OK )

    /* Report DET if RemoteAddr_cpst is a Null Pointer */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == RemoteAddr_cpst ), RBA_ETHICMP_E_TRANSMIT_API_ID,          \
                                            RBA_ETHICMP_E_NULL_PTR, E_NOT_OK )

    /* Report DET if TxData_cpu8 is a Null Pointer */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == TxData_cpu8 ), RBA_ETHICMP_E_TRANSMIT_API_ID,              \
                                            RBA_ETHICMP_E_NULL_PTR, E_NOT_OK )

    /* If there was no return from the DET checks, then return E_OK as all DET checks passed. */
    return E_OK;
}

/********************************************************************************************************************/
/* rba_EthIcmp_RxIndication() - Function called when an ICMP is received. A response is sent according to the ICMP  */
/*                              frame type.                                                                         */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* RmtIPAddr_u32       - (Source IP) needed in SoAd optional RX Filter and in ICMP                                  */
/* localAddrId_u32     - Contains EthIfCtrl and local IP address derived in IPv4 module from IP                     */
/*                       header                                                                                     */
/* RxData_pu8          - Pointer to received data behind IPv4 header                                                */
/* RxDataLen_u16       - Length of the pointer received in parameter                                                */
/* IsBroadcast_b       - Indicates whether received frame is a broadcast frame or not                               */
/*                                                                                                                  */
/* Return   - Result of operation :  void                                                                           */
/*                                                                                                                  */
/********************************************************************************************************************/
void rba_EthIcmp_RxIndication(    uint32 RmtIPAddr_u32,
                                  uint8 LocalAddrId_u8,
                                  uint8 * RxData_pu8,
                                  uint16 RxDataLen_u16,
                                  boolean IsBroadcast_b )
{
    /* Local variable declaration */

    TcpIp_SockAddrInetType lIPAddrPort_st;
    uint16 lTempVal_u16;
    uint8 lProtType_u8;
    boolean lUpperLayerCbkReqd_b;
    uint8 * lRxData_pu8;

#if ( TCPIP_TCP_ENABLED == STD_ON )
    Std_ReturnType lRetVal_en;
    TcpIp_SockAddrInetType lLocalIPAddrPort_st;
    TcpIp_SockAddrInetType lRemoteIPAddrPort_st;
    TcpIp_SocketIdType lTcpIpSockId_u16;
#endif

#if(STD_ON == TCPIP_DEM_CONFIGURED)
    const rba_EthIcmp_SubConfig_to * lIcmpSubConfig_pcst;
#endif

    /* Local variable initialisation */
    lUpperLayerCbkReqd_b    = FALSE;
    lRxData_pu8             = NULL_PTR;

    /* Report DET if rba_EthIcmp module is uninitialised */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VOID( ( FALSE == rba_EthIcmp_InitFlag_b ), RBA_ETHICMP_E_RX_INDICATION_API_ID,      \
                                           RBA_ETHICMP_E_NOTINIT )


    /* Report DET if local address ID is invalid */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VOID( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),        \
                                           RBA_ETHICMP_E_RX_INDICATION_API_ID, RBA_ETHICMP_E_INV_ARG )

    /* Report DET if RxData_pu8 is a Null Pointer */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VOID( ( NULL_PTR == RxData_pu8 ), RBA_ETHICMP_E_RX_INDICATION_API_ID,               \
                                           RBA_ETHICMP_E_NULL_PTR )

    /* Report DET if RxDataLen_u16 is invalid */
    RBA_ETHICMP_DET_REPORT_ERROR_RET_VOID( ( RxDataLen_u16 < RBA_ETHICMP_PRV_HDR_LENGTH ), RBA_ETHICMP_E_RX_INDICATION_API_ID,  \
                                           RBA_ETHICMP_E_INV_ARG )

#if(STD_ON == TCPIP_DEM_CONFIGURED)
    lIcmpSubConfig_pcst     = ( &(rba_EthIcmp_CurrConfig_pco->IcmpSubConfig_pco[LocalAddrId_u8] ) );
#endif

        /* Check the type of the ICMP Frame */
        switch ( RxData_pu8[ RBA_ETHICMP_PRV_TYPE_OFFSET ] )
        {
            /* If echo reply is received */
            case (uint8) RBA_ETHICMP_ECHO_REPLY_E:
            {
                /* Check if the identifier field is correct */
                TcpIp_ReadU16 ( &RxData_pu8[RBA_ETHICMP_PRV_IDENTIFIER_OFFSET], &lTempVal_u16 );

                if ( RBA_ETHICMP_PRV_IDENTIFIER_FIELD == lTempVal_u16 )
                {   /* Call configured callback function - TcpIpIcmpMsgHandlerName */
                    lUpperLayerCbkReqd_b = TRUE;
                }
            }
            break;

            /* If echo request is received  */
            case (uint8) RBA_ETHICMP_ECHO_REQ_E:
            {
                /* Send echo reply if the received echo request is broadcast and */
                /* reply to Broadcast request is enabled for the particular LocalAddrId. */
                /* Send echo reply if its a unicast request */
                if(( TCPIP_FRAME_IS_NOTBROADCAST == IsBroadcast_b ) ||
                   ( rba_EthIcmp_CurrConfig_pco->IcmpSubConfig_pco[LocalAddrId_u8].EchoBroadcastEn_cu8 == (STD_ON) ) )
                {
                     ( lIPAddrPort_st.addr[0] )  = RmtIPAddr_u32;
                     /* Send ICMP Echo reply frame */
                     /* Transmit the received identifier field, sequence number and data back to the sending node */
                     /* So Frame data length is - ( identifier field length + sequence number length + receive data length )*/
                     (void)rba_EthIcmp_Transmit( LocalAddrId_u8,
                                               ( &lIPAddrPort_st ) ,             /* Remote node IP address and port */
                                               0U,                               /* TTL = 0 indicates default value will be used */
                                               (uint8)RBA_ETHICMP_ECHO_REPLY_E,  /* ICMP Control Message Type */
                                               0U,                               /* ICMP Control Message Code - 0 */
                                               ( RxDataLen_u16 - RBA_ETHICMP_PRV_HDR_LENGTH ), /* DataLength */
                                               ( &( RxData_pu8[RBA_ETHICMP_PRV_HDR_LENGTH] ) ) );  /* Pointer to ICMP data field */
                }
            }
            break;

            /* If received Destination unreachable */
            case  (uint8) RBA_ETHICMP_DEST_UNREACH_E:
            {
                /* If ICMP port unreachable frame is received */
                if ( RxData_pu8[RBA_ETHICMP_PRV_CODE_OFFSET] ==  (uint8) RBA_ETHICMP_DEST_UNREACH_PORT_E )
                {
#if(STD_ON == TCPIP_DEM_CONFIGURED)
                    /* Report DEM error if TCPIP_E_CONNREFUSED is configured */
                    if( 0 != lIcmpSubConfig_pcst->DemEventId_ConRefused_u16 )
                    {
                        Dem_ReportErrorStatus( lIcmpSubConfig_pcst->DemEventId_ConRefused_u16,
                                               DEM_EVENT_STATUS_FAILED );
                    }
#endif

                    /* Upper layer call back not required */
                    lUpperLayerCbkReqd_b = FALSE;

                    /* Move the RX buffer pointer to the ICMP data field */
                    lRxData_pu8 = &( RxData_pu8[RBA_ETHICMP_PRV_HDR_LENGTH] );

                    /* Extract the transport layer protocol type */
                    lProtType_u8 = lRxData_pu8[RBA_ETHIPV4_TP_PROTOCOL];

                    /* If protocol type is TCP, close the socket with abort value true */
                    if ( (uint8) TCPIP_IPPROTO_TCP == lProtType_u8 )
                    {
                    #if ( TCPIP_TCP_ENABLED == STD_ON )

                        /* Initialize local and remote address domain to TCPIP_AF_INET */
                        lLocalIPAddrPort_st.domain  = TCPIP_AF_INET;
                        lRemoteIPAddrPort_st.domain = TCPIP_AF_INET;
                        lTcpIpSockId_u16        = 0;

                        /* Extract the local and remote IP addresses from the IP header in the ICMP data field */
                        TcpIp_ReadU32 ( &( lRxData_pu8[RBA_ETHIPV4_SRCIPV4ADDR_OFFSET] ),
                                                  &( lLocalIPAddrPort_st.addr[0] ) );

                        TcpIp_ReadU32 ( &( lRxData_pu8[RBA_ETHIPV4_DESTIPV4ADDR_OFFSET] ),
                                                  &( lRemoteIPAddrPort_st.addr[0] ) );

                        /* Move the RX buffer pointer to the TP header in the ICMP data field */
                        lRxData_pu8 = ( lRxData_pu8 + RBA_ETHIPV4_HDRLENGTH );

                        /* Extract the local and remote port numbers from the TP header */
                        TcpIp_ReadU16 ( &( lRxData_pu8[TCPIP_TP_SRCPORT_OFFSET] ),
                                                   &( lLocalIPAddrPort_st.port ) );

                        TcpIp_ReadU16 ( &( lRxData_pu8[TCPIP_TP_DESTPORT_OFFSET] ),
                                                   &( lRemoteIPAddrPort_st.port ) );


                        /* Call rba_EthTcp_FindTcpSockId API to retrieve the Id of the socket that has to be closed. */
                        /* The input parameters to the API are obtained from the IPv4 header present in the ICMP data field */
                        /* MR12 RULE 11.3 VIOLATION: typecasted to TcpIp_SockAddrType to match the function parameter Type definition */
                        lRetVal_en=  rba_EthTcp_FindTcpSockId( (const TcpIp_SockAddrType*)&lLocalIPAddrPort_st,
                                                               (const TcpIp_SockAddrType*)&lRemoteIPAddrPort_st,
                                                                &lTcpIpSockId_u16);


                        /* A socket has been found that shall be closed */
                        if (( E_OK == lRetVal_en ) && ( lTcpIpSockId_u16 < TCPIP_TCPSOCKETMAX ))
                        {
                                (void)rba_EthTcp_Close( lTcpIpSockId_u16,
                                                   TRUE );
                        }
                        /* Socket to be closed was not found in the TcpIp dynamic socket table */
                        else
                        {
                        /* Do nothing */
                        }
                    #endif
                    }
                    else
                    {
                        /* If protocol is UDP, do not close the socket */
                    }
                }/* end - if ( RxData_pu8[RBA_ETHICMP_PRV_CODE_OFFSET] == RBA_ETHICMP_DEST_UNREACH_PORT_E ) */
            }
            break;

            default:
                /*Nothing to do*/
            break;
        }

        /* Call configured callback function defined in upper layer. */
        /* If Echo request is received, the reply is sent by rba_EthIcmp. No need to inform upper layer about reception of Echo Request */
        if( ( NULL_PTR != ( rba_EthIcmp_CurrConfig_pco->IcmpRxIndication_pfct ) ) &&
            ( FALSE != lUpperLayerCbkReqd_b ) )
        {
            ( lIPAddrPort_st.addr[0] )  = RmtIPAddr_u32;

            (void)( rba_EthIcmp_CurrConfig_pco->IcmpRxIndication_pfct )(
                                                LocalAddrId_u8,
                                                ( (TcpIp_SockAddrInetType*)&( lIPAddrPort_st ) ),   /* Remote node IP address */
                                                0,                          /* TTL = 0 indicates default value will be used */
                                                RxData_pu8[ RBA_ETHICMP_PRV_TYPE_OFFSET ],  /* ICMP Control Message Type */
                                                RxData_pu8[ RBA_ETHICMP_PRV_CODE_OFFSET ],  /* ICMP Control Message Code */
                                                ( RxDataLen_u16 - RBA_ETHICMP_PRV_HDR_LENGTH ),     /* DataLength */
                                                ( &( RxData_pu8[RBA_ETHICMP_PRV_HDR_LENGTH] ) ) );  /* Pointer to ICMP data field */
        }

    return;
}
#define RBA_ETHICMP_STOP_SEC_CODE
#include "rba_EthIcmp_MemMap.h"

#endif  /* #if ( defined(TCPIP_ICMP_ENABLED) && ( TCPIP_ICMP_ENABLED == STD_ON ) ) */



