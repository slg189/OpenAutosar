
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) )

#include "rba_EthIPv4.h"
#include "rba_EthIPv4_Cbk.h"

#if ( TCPIP_ARP_ENABLED == STD_ON )
#include "rba_EthArp.h"
#endif

#if ( TCPIP_TCP_ENABLED == STD_ON )
#include "rba_EthTcp_Cbk.h"
#endif

#if ( TCPIP_ICMP_ENABLED == STD_ON )
#include "rba_EthIcmp.h"
#include "rba_EthIcmp_Cbk.h"
#endif

#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp_Cbk.h"
#endif

#include "TcpIp_Prv.h"
#include "TcpIp_Lib.h"
#include "rba_EthIPv4_Prv.h"

#include "rba_BswSrv.h"

/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */
#define RBA_ETHIPV4_START_SEC_CODE
#include "rba_EthIPv4_MemMap.h"

static Std_ReturnType rba_EthIPv4_ValidateRxFrame( uint8 EthIfCtrlIdx_u8,
                                                   boolean IsBroadcast_b,
                                                   const uint8 * RxData_pcu8,
                                                   uint16 * RxDataLen_pu16,
                                                   TcpIp_PseudoHdr_tst * PseudoHdr_pst,
                                                   TcpIp_LocalAddrIdType * LocalAddrId_pu8,
                                                   boolean * UpdateArpTable_pb);

static Std_ReturnType rba_EthIPv4_ValidateIPaddress( uint8 EthIfCtrlIdx_u8,
                                                     boolean IsBroadcast_b,
                                                     const uint8 * RxData_pcu8,
                                                     TcpIp_PseudoHdr_tst * PseudoHdr_pst,
                                                     TcpIp_LocalAddrIdType * LocalAddrId_pu8,
                                                     boolean * UpdateArpTable_pb,
                                                     boolean * InvalidIPAddr_pb );

static Std_ReturnType rba_EthIPv4_ReassemblyHandler( const uint8 EthIfCtrlIdx_cu8,
                                                     const uint8 * RxData_pcu8,
                                                     TcpIp_PseudoHdr_tst * PseudoHdr_pst,
                                                     rba_EthIPv4_ReassemblyBufDescriptor_tst **  ReassemblyBufDescriptor_ppst );


#if (RBA_ETHIPV4_VERIFY_TPLAYERCHKSUM_ENABLED)
static Std_ReturnType rba_EthIPv4_VerifyTPLayerChkSum( const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                                       const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                       const uint8 * Buffer_pcu8,
                                                       const uint16 TPLayerLength_cu16 );
#endif

#if ( RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON )
static void rba_EthIPv4_MergeFragIntoReassemblBuf( const uint8 * RxData_pcu8,
                                                   const boolean MoreFragFlag_cb,
                                                   const uint16 FragOffset_cu16,
                                                   const uint16 FragPayLoadLen_cu16,
                                                   rba_EthIPv4_ReassemblyBufDescriptor_tst **  ReassemblyBufDescriptor_ppst );

static void rba_EthIPv4_FindInitialHolePos( const rba_EthIPv4_ReassemblyBufDescriptor_tst * const ReassemblyBufDescriptor_cpcst,
                                            const uint16 FragOffset_cu16,
                                            const uint16 FragPayLoadLen_cu16,
                                            uint16 * FirstFreeHole_pu16,
                                            uint16 * FragEndHole_pu16 );
#endif

#if (TCPIP_ICMP_ENABLED == STD_ON)
static void rba_EthIPv4_CheckProtocolUnreachable( const uint8 * RxData_pu8,
                                                  const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                  TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                  boolean IsBroadcast_b);
#endif

#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)

LOCAL_INLINE void rba_EthIPv4_IncMeasurementData( uint32 * MeasurementData_pu32 );

#endif

static Std_ReturnType rba_EthIPv4_Prv_RxIndication_DetChecks( const uint8 * RmtPhysAddr_pcu8,
                                                              const uint8 * RxData_pcu8 );

/********************************************************************************************************************/
/* rba_EthIPv4_RxIndication() - This API is called when an IPv4 packet is received.                                 */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  EthIfCtrlIdx_u8     - Index of the EthIf controller on which the IPv4 packet is received                        */
/*  IsBroadcast_b       - parameter to indicate a broadcast frame                                                   */
/*  RmtPhysAddr_pu8     - Pointer to Physical source address (MAC address in network byte order) of the             */
/*                        transmitting node                                                                         */
/*  RxData_pu8          - Data pointer to the starting address of IPv4 header                                       */
/*  RxDataLen_u16       - Length of IPv4 packet                                                                     */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   void                                                                                                           */
/********************************************************************************************************************/

/* MR12 RULE 8.13 VIOLATION: According to 'Specification of Ethernet Interface AUTOSAR Release 4.1.1', in EthIf_RxIndication() parameter  RmtPhysAddr_pu8 is of pointer to variable. */
void rba_EthIPv4_RxIndication( uint8 EthIfCtrlIdx_u8,
                               boolean IsBroadcast_b,
                               uint8 * RmtPhysAddr_pu8,
                               uint8 * RxData_pu8,
                               uint16 RxDataLen_u16  )
{
    /* Local variable declaration */
    Std_ReturnType          lStdRetVal_en;
    TcpIp_PseudoHdr_tst     lPseudoHdr_st;
    TcpIp_LocalAddrIdType   lLocalAddrId_u8;
    boolean                 lUpdateArpTable_b;
    TcpIp_LocalAddrId_EthIfCtrl_tun             lLocalAddrId_CtrlIdx_un;
    rba_EthIPv4_ReassemblyBufDescriptor_tst *   lReassemblyBufDescriptor_pst;
    uint8 *                 lRxData_pu8;
    uint16                  lTPLayerDataLen_u16;

    /* Initialize local variable */
    lStdRetVal_en                   = E_NOT_OK;
    lUpdateArpTable_b               = FALSE;
    lReassemblyBufDescriptor_pst    = NULL_PTR;     /* Set to NULL pointer; will be initialized afterwards. */
    lLocalAddrId_u8                 = 0xFFU;
    lRxData_pu8                     = NULL_PTR;
    lTPLayerDataLen_u16             = 0U;

    /* Check the development errors for the API. */
    lStdRetVal_en = rba_EthIPv4_Prv_RxIndication_DetChecks( RmtPhysAddr_pu8,
                                                            RxData_pu8 );

    /* If development errors are detected, then exit from the API without further processing. */
    if( lStdRetVal_en != E_NOT_OK )
    {
        /* Validate IPv4 header fields in the received frame */
        lStdRetVal_en = rba_EthIPv4_ValidateRxFrame( EthIfCtrlIdx_u8,
                                                     IsBroadcast_b,
                                                     RxData_pu8,
                                                     &RxDataLen_u16,
                                                     &lPseudoHdr_st,
                                                     &lLocalAddrId_u8,
                                                     &lUpdateArpTable_b);

        #if( (TCPIP_ARP_ENABLED == STD_ON) && (TCPIP_IP_ADDR_CONFLICT_DETECTION_IP_FRAMES == STD_ON) )
        if( (E_OK == lStdRetVal_en ) &&
            (FALSE != lUpdateArpTable_b) )  /* lUpdateArpTable_b is true only if remote is on-link. If remote is detected off-link, checking for conflict detection is not required */
        {

            /* Check if there is conflict for the IP address */
            lStdRetVal_en = rba_EthArp_IpConflictDetection( EthIfCtrlIdx_u8,
                                                            lPseudoHdr_st.RemoteIpAddr_un.IPv4Addr_u32,
                                                            lPseudoHdr_st.LocalIpAddr_un.IPv4Addr_u32,
                                                            RmtPhysAddr_pu8,
                                                            TCPIP_TYPE_IPV4 );

            /* If lStdRetVal_en == E_NOT_OK, No conflict detected, update the ARP table */
            /* If lStdRetVal_en == E_OK, Conflict detected, Drop the received frame */

            lStdRetVal_en  = ( (E_NOT_OK == lStdRetVal_en) ? E_OK : E_NOT_OK );
        }
        #endif /* (TCPIP_IP_ADDR_CONFLICT_DETECTION_IP_FRAMES == STD_ON) */

        /* Check whether the frame is fragmented and reassemble it if so. */
        if( (E_OK == lStdRetVal_en) &&
            (TCPIP_IPPROTO_NONE != lPseudoHdr_st.ProtoType_en) )
        {
            lStdRetVal_en = rba_EthIPv4_ReassemblyHandler( EthIfCtrlIdx_u8,
                                                           RxData_pu8,
                                                           &lPseudoHdr_st,
                                                           &lReassemblyBufDescriptor_pst);
        }

        #if ( TCPIP_ARP_ENABLED == STD_ON )

        /* Update ARP table only if                                                                                                                       */
        /* 1. Unicast local IP address assigned to the EthIf controller on which frame was received (lUnicastLocalIp_u32) and remote IP are within subnet */
        /* 2. Remote IP is derived via AutoIp                                                                                                             */
        /* 3. Local IP is derived via AutoIp                                                                                                              */
        if( (E_OK == lStdRetVal_en ) &&
            ( FALSE != lUpdateArpTable_b ) )
        {
            (void)rba_EthArp_UpdateArpTable( EthIfCtrlIdx_u8,
                                             RmtPhysAddr_pu8,
                                             lPseudoHdr_st.RemoteIpAddr_un.IPv4Addr_u32 );
        }

    #endif /* #if ( TCPIP_ARP_ENABLED == STD_ON ) */

        /* If the protocol is unreachable, ICMP frame RBA_ETHICMP_DEST_UNREACH_PROTO_E needs to be sent. */
        /*  ICMP frame RBA_ETHICMP_DEST_UNREACH_PROTO_E shall be only set if the destination IP is unicast and in case of fragmented frame only on reception of zeroth fragment. */
        /* Pre conditions for sending ICMP frame are implemented within the following API */

        if( (E_OK == lStdRetVal_en) &&
            (TCPIP_IPPROTO_NONE == lPseudoHdr_st.ProtoType_en) )
        {
    #if (TCPIP_ICMP_ENABLED == STD_ON)
            rba_EthIPv4_CheckProtocolUnreachable( RxData_pu8,
                                                  &lPseudoHdr_st,
                                                  lLocalAddrId_u8,
                                                  IsBroadcast_b);
    #endif
            lStdRetVal_en = E_NOT_OK;
        }

        /* Check whether the frame needs be forwarded to upper layer */
        if(E_OK == lStdRetVal_en)
        {
            if(FALSE == lPseudoHdr_st.IsRxFragmented_b)
            {
                /* Frame shall be forwarded to upper layer if the frame is not fragmented */
                lRxData_pu8 = RxData_pu8 + lPseudoHdr_st.IPHdrLen_u16;
                lTPLayerDataLen_u16 = RxDataLen_u16 - lPseudoHdr_st.IPHdrLen_u16;
            }
            /* Frame shall be forwarded to upper layer if the frame is completely reassembled */
            /* If the frame is completely reassembled lReassemblyBufDescriptor_pst will point to valid reassembly descriptor */
            else if(NULL_PTR != lReassemblyBufDescriptor_pst)
            {

                lRxData_pu8 = (uint8*)lReassemblyBufDescriptor_pst->ReassemblyBuf_cpu16;

                lTPLayerDataLen_u16 = lReassemblyBufDescriptor_pst->TotalPalyloadLen_u16;
            }
            else
            {   /* Do not forward the frame to upper layer */
                lStdRetVal_en = E_NOT_OK;
            }
        }

        /* Verify transport layer checksum is valid, if checksum verification needs to be done by software */
        #if (RBA_ETHIPV4_VERIFY_TPLAYERCHKSUM_ENABLED)
        if(E_OK == lStdRetVal_en)
        {
            lStdRetVal_en = rba_EthIPv4_VerifyTPLayerChkSum( rba_EthIPv4_CtrlCfg_pcpcst[EthIfCtrlIdx_u8],
                                                             &lPseudoHdr_st,
                                                             lRxData_pu8,
                                                             lTPLayerDataLen_u16 );
        }
        #endif

        /* Call upper layer Rx indication */
        if(E_OK == lStdRetVal_en)
        {
            switch (lPseudoHdr_st.ProtoType_en)
            {
                #if ( TCPIP_ICMP_ENABLED == STD_ON )
                case TCPIP_IPPROTO_ICMP:
                {
                    rba_EthIcmp_RxIndication(   lPseudoHdr_st.RemoteIpAddr_un.IPv4Addr_u32,
                                                lLocalAddrId_u8,
                                                lRxData_pu8,
                                                lTPLayerDataLen_u16 ,
                                                IsBroadcast_b );
                }
                break;
                #endif

                #if ( TCPIP_TCP_ENABLED == STD_ON )
                case TCPIP_IPPROTO_TCP:
                {
                        rba_EthTcp_RxIndication( &lPseudoHdr_st,
                                                 lLocalAddrId_u8,
                                                 lRxData_pu8,
                                                 lTPLayerDataLen_u16 );
                }
                break;
                #endif

                #if ( TCPIP_UDP_ENABLED == STD_ON )
                case TCPIP_IPPROTO_UDP:
                {
                    if ( IsBroadcast_b != FALSE )

                    {   /* If it is a UDP broadcast frame, pass the EthIf ctrl Idx to UDP */
                        lLocalAddrId_CtrlIdx_un.EthIfCtrlIdx_u8 = EthIfCtrlIdx_u8;
                    }
                    else
                    {
                        /* If the frame is not broadcast, then pass the Local AddrId to UDP */
                        lLocalAddrId_CtrlIdx_un.LocalAddrId_u8 = lLocalAddrId_u8;
                    }

                    rba_EthUdp_RxIndication( &lPseudoHdr_st,
                                             lLocalAddrId_CtrlIdx_un,
                                             lRxData_pu8,
                                             lTPLayerDataLen_u16,
                                             IsBroadcast_b );
                }
                break;
                #endif

                default:
                {   /* Do nothing */
                }
                break;
            }   /* switch case end */

            /* Reassembly resources shall be released once the frame is reassembled and forwarded to upper layer */
            if(NULL_PTR != lReassemblyBufDescriptor_pst)
            {
                lReassemblyBufDescriptor_pst->TimerCurrentValue_u32 = 0U;
                lReassemblyBufDescriptor_pst->TotalPalyloadLen_u16 = 0U;
            }
        }
    }

    return;
}

/********************************************************************************************************************/
/* rba_EthIPv4_Prv_RxIndication_DetChecks() - This API checks the development errors (DETs) for                     */
/*                                            rba_EthIPv4_RxIndication() API                                        */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  RmtPhysAddr_pcu8     - Pointer to Physical source address (MAC address in network byte order) of the            */
/*                        transmitting node                                                                         */
/*  RxData_pcu8          - Data pointer to the starting address of IPv4 header                                      */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : No development error detected.                                                      */
/*                     - E_NOT_OK : Development error detected.                                                     */
/*                                                                                                                  */
/********************************************************************************************************************/

/* MR12 RULE 8.13 VIOLATION: According to 'Specification of Ethernet Interface AUTOSAR Release 4.1.1', in EthIf_RxIndication() parameter  RmtPhysAddr_pu8 is of pointer to variable. */
static Std_ReturnType rba_EthIPv4_Prv_RxIndication_DetChecks( const uint8 * RmtPhysAddr_pcu8,
                                                              const uint8 * RxData_pcu8 )
{
    /* Check for DET errors */
    /* Report DET if rba_EthIPv4 module is uninitialised. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthIPv4_CtrlCfg_pcpcst ), RBA_ETHIPV4_E_RX_INDICATION_API_ID,       \
                                           RBA_ETHIPV4_E_NOTINIT, E_NOT_OK )

    /* Report DET if RmtPhysAddr_pu8 is a Null pointer. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == RmtPhysAddr_pcu8 ), RBA_ETHIPV4_E_RX_INDICATION_API_ID,                  \
                                           RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET if RxData_pu8 is a Null pointer. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == RxData_pcu8 ), RBA_ETHIPV4_E_RX_INDICATION_API_ID,                       \
                                           RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* If there was no return from the DET checks, then return E_OK as all DET checks passed. */
    return E_OK;

}

/********************************************************************************************************************/
/* rba_EthIPv4_ValidateRxFrame() - This API is called by rba_EthIPv4_RxIndication. This API validates IPv4 header   */
/*                                 in the received frame.                                                           */
/* Input Parameters :                                                                                               */
/*  EthIfCtrlIdx_u8     - Index of the EthIf controller on which the IPv4 packet is received                        */
/*  IsBroadcast_b       - parameter to indicate a broadcast frame                                                   */
/*  RxData_pcu8         - Data pointer to the starting address of IPv4 header                                       */
/*                                                                                                                  */
/* InOut Parameters :                                                                                               */
/*  RxDataLen_u16       - Input: Frame length provided by EthIf                                                     */
/*                      - Output: Payload length of the received frame.                                             */
/*                      - (i.e. length mentioned in IPv4 'Total Length' field - IPv4 header length )                */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* PseudoHdr_pst        - Contains pseudo header information required for transport layer checksum verification     */
/* LocalAddrId_pu8      - LocalddrId corresponding to EthIfCtrl on which frame is received                          */
/* UpdateArpTable_pb    - Set to TRUE if the required conditions for updating Arp table are met, else set to FALSE  */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : Frame is valid and shall be further processed                                       */
/*                     - E_NOT_OK : Frame is invalid and shall be dropped                                           */
/********************************************************************************************************************/

static Std_ReturnType rba_EthIPv4_ValidateRxFrame( uint8 EthIfCtrlIdx_u8,
                                                   boolean IsBroadcast_b,
                                                   const uint8 * RxData_pcu8,
                                                   uint16 * RxDataLen_pu16,
                                                   TcpIp_PseudoHdr_tst * PseudoHdr_pst,
                                                   TcpIp_LocalAddrIdType * LocalAddrId_pu8,
                                                   boolean * UpdateArpTable_pb )
{
    Std_ReturnType       lStdRetVal_en;
    uint8        lVerHdrLen_u8;
    uint16       lIPHdrTotLen_u16;
#if (( RBA_ETHIPV4_SW_CHKSUM_RX != STD_OFF ) && ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON ))
    uint16       lChksumCalc_u16;
#endif
    boolean      lInvalidIPAddr_b;

    /* Initialize local variables */
    lStdRetVal_en              = E_NOT_OK;
    lVerHdrLen_u8              = 0;
    lIPHdrTotLen_u16           = 0;
#if(( RBA_ETHIPV4_SW_CHKSUM_RX != STD_OFF ) && ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON ))
    lChksumCalc_u16            = 0xFFFFu;
#endif
    lInvalidIPAddr_b           = FALSE;



    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( *RxDataLen_pu16 < RBA_ETHIPV4_HDRLENGTH), RBA_ETHIPV4_E_VALIDATE_RXFRAME_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Read out version info and header length fields from the received buffer */
    lVerHdrLen_u8 = RxData_pcu8[RBA_ETHIPV4_IPV4VER_HDRLEN_TOS_UB];

    /* Set the IpDomainType_t field to TCPIP_AF_INET (used in checksum calculation). */
    PseudoHdr_pst->IpDomainType_t = TCPIP_AF_INET;

    /* Header length is stored in units of 4 bytes word */
    PseudoHdr_pst->IPHdrLen_u16 = (lVerHdrLen_u8 & 0x0Fu ) * 4u;

    /* Read out packet length from Total Length field of the received frame */
    TcpIp_ReadU16( ( RxData_pcu8 + RBA_ETHIPV4_TOTLEN_UB ),
                    &lIPHdrTotLen_u16 );

    /* Rx buffer size shall be greater than or equal to Total length field in IPv4 header */
    /* Otherwise return from the function and report DET if enabled. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( *RxDataLen_pu16 < lIPHdrTotLen_u16), RBA_ETHIPV4_E_VALIDATE_RXFRAME_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Consider the frame is valid if */
    if( (RBA_ETHIPV4_IPV4_VERSION_NUM == ( lVerHdrLen_u8 >> 4u)) &&    /* Version field in the IPv4 header is set to 4 */
        (lIPHdrTotLen_u16 > PseudoHdr_pst->IPHdrLen_u16) )           /* Total length field in IPv4 header is greater than IPv4 header length value */
    {
        /* Consider the value in the total length field of IPv4 header as the actual total length of the frame. */
        *RxDataLen_pu16 = lIPHdrTotLen_u16;

        /* Verify IPv4 header checksum */
        #if ( RBA_ETHIPV4_SW_CHKSUM_RX != STD_OFF )
        /* Check whether checksum offloading is enabled for the hardware referenced by the current TcpIpCtrl. */
        if( rba_EthIPv4_CtrlCfg_pcpcst[EthIfCtrlIdx_u8]->IPv4ChkSumOffloadingEnabled_cb == FALSE )
        {
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
            /* MR12 DIR 1.1 VIOLATION: The input parameter of TcpIp_OnesComplChkSum() is declared as (void*) for faster implmentation of checksum calculation. Low Risk */
            lStdRetVal_en = TcpIp_OnesComplChkSum( RxData_pcu8,
                                                   NULL_PTR,                  /* Pseudo header is not required for IPv4 */
                                                   PseudoHdr_pst->IPHdrLen_u16,
                                                   &lChksumCalc_u16);

            /* Discard the frame if the data received is corrupted */
            if ( 0U != lChksumCalc_u16)
            {
                lStdRetVal_en = E_NOT_OK;
            }
#else
            /* Report DET: If TcpIp_OnesComplChkSum() is not available */
            RBA_ETHIPV4_CALL_DET_REPORTERROR( RBA_ETHIPV4_E_VALIDATE_RXFRAME_API_ID, RBA_ETHIPV4_E_INV_ARG )

            lStdRetVal_en = E_NOT_OK;
#endif
        }
        else
        {
            /* If checksum offloading is enabled for the TcpIpCtrl, then return E_OK as checksum is verified by the hardware. */
            lStdRetVal_en = E_OK;
        }
        #else
        /* If IPv4 software checksum verification is disabled set local variable lStdRetVal_en to E_OK */
        lStdRetVal_en = E_OK;
        #endif
    }

    if( lStdRetVal_en != E_NOT_OK )
    {
        /* Validate the source and destination IP address of the received frame. */
        lStdRetVal_en = rba_EthIPv4_ValidateIPaddress(   EthIfCtrlIdx_u8,
                                                         IsBroadcast_b,
                                                         RxData_pcu8,
                                                         PseudoHdr_pst,
                                                         LocalAddrId_pu8,
                                                         UpdateArpTable_pb,
                                                         &lInvalidIPAddr_b );
    }

    /* Initialize the protocol member of the pseudo header structure to a invalid value */
    PseudoHdr_pst->ProtoType_en = TCPIP_IPPROTO_NONE;

    /* Update the protocol information in pseudo header structure */
    if (E_OK == lStdRetVal_en)
    {
#if(TCPIP_ICMP_ENABLED == STD_ON )
        if((uint8)TCPIP_IPPROTO_ICMP ==  RxData_pcu8[RBA_ETHIPV4_TP_PROTOCOL])
        {
            PseudoHdr_pst->ProtoType_en = TCPIP_IPPROTO_ICMP;
        }
        else
#endif
#if (TCPIP_TCP_ENABLED == STD_ON )
        if ((uint8)TCPIP_IPPROTO_TCP ==  RxData_pcu8[RBA_ETHIPV4_TP_PROTOCOL])
        {
            PseudoHdr_pst->ProtoType_en = TCPIP_IPPROTO_TCP;
        }
        else
#endif
#if(TCPIP_UDP_ENABLED == STD_ON )
        if ((uint8)TCPIP_IPPROTO_UDP ==  RxData_pcu8[RBA_ETHIPV4_TP_PROTOCOL])
        {
            PseudoHdr_pst->ProtoType_en = TCPIP_IPPROTO_UDP;
        }
        else
#endif
        {
            /* Do nothing */
        }
    }


    /* If flag was set for invalid source or destination IP address, increment the corresponding measurement data counter. */
    if( lInvalidIPAddr_b != FALSE )
    {
		/* Increment measurement data if function is enabled. */
#if( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
        rba_EthIPv4_IncMeasurementData( &rba_EthIPv4_InvalidIPaddrDropCnt_u32 );
#endif
    }


    return lStdRetVal_en;
}

/********************************************************************************************************************/
/* rba_EthIPv4_ValidateIPaddress() - This API is called by rba_EthIPv4_ValidateRxFrame(), which is inturn called by */
/*                                   rba_EthIPv4_RxIndication(). This API validates  specifically                   */
/*                                   the source and destination IP addresses of the received frame.                 */
/* Input Parameters :                                                                                               */
/*  EthIfCtrlIdx_u8     - Index of the EthIf controller on which the IPv4 packet is received                        */
/*  IsBroadcast_b       - Parameter to indicate a broadcast frame                                                   */
/*  RxData_pcu8         - Data pointer to the starting address of IPv4 header                                       */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* PseudoHdr_pst        - Contains pseudo header information required for transport layer checksum verification     */
/* LocalAddrId_pu8      - Pointer to LocalddrId corresponding to EthIfCtrl on which frame is received               */
/* UpdateArpTable_pb    - Set to TRUE if the required conditions for updating Arp table are met, else set to FALSE  */
/* InvalidIPAddr_pb    - Indicates whether the source or destination IP address of the received frame is invalid.   */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : Frame is valid and shall be further processed                                       */
/*                     - E_NOT_OK : Frame is invalid and shall be dropped                                           */
/********************************************************************************************************************/

static Std_ReturnType rba_EthIPv4_ValidateIPaddress( uint8 EthIfCtrlIdx_u8,
                                                     boolean IsBroadcast_b,
                                                     const uint8 * RxData_pcu8,
                                                     TcpIp_PseudoHdr_tst * PseudoHdr_pst,
                                                     TcpIp_LocalAddrIdType * LocalAddrId_pu8,
                                                     boolean * UpdateArpTable_pb,
                                                     boolean * InvalidIPAddr_pb )
{
    /* Declaration of local variables. */
    Std_ReturnType       lStdRetVal_en;
    TcpIp_StateType      lTcpIpEthIfState_en;
    uint32               lRxDestIPv4Addr_u32;
    uint32               lRxSrcIPv4Addr_u32;
    uint32               lUnicastLocalIp_u32;
    uint32               lNetMask_u32;

    /* Initialize local variables */
    lTcpIpEthIfState_en        = TCPIP_STATE_OFFLINE;
    lRxDestIPv4Addr_u32        = TCPIP_IPADDR_ZERO;
    lRxSrcIPv4Addr_u32         = TCPIP_IPADDR_BROADCAST;
    lUnicastLocalIp_u32        = TCPIP_IPADDR_BROADCAST;
    lNetMask_u32               = 0xFFFFFFFFuL;
    lStdRetVal_en              = E_OK;

    /* Read out source IP address from the received frame */
     TcpIp_ReadU32(  ( RxData_pcu8 + RBA_ETHIPV4_SRCIPV4ADDR_OFFSET ),
                     &lRxSrcIPv4Addr_u32);

     /* Read out destination IP address from the received frame */
     TcpIp_ReadU32(  ( RxData_pcu8+ RBA_ETHIPV4_DESTIPV4ADDR_OFFSET ),
                     &lRxDestIPv4Addr_u32 );

     /* Store the source and destination IP address in the pseudo header. */
     PseudoHdr_pst->RemoteIpAddr_un.IPv4Addr_u32  = lRxSrcIPv4Addr_u32;
     PseudoHdr_pst->LocalIpAddr_un.IPv4Addr_u32   = lRxDestIPv4Addr_u32;

    /* Discard received IP frame if the Source IP address in received frame is */
    /* 1. is loop back IP                                                      */
    /* 2. Reserved IP range 240.0.0.0/4 ( 0xffffffff  )                        */
    /* 3. multicast address                                                    */
    /* 4. IP is AutoIp broadcast address.                                      */
    if( (RBA_ETHIPV4_LOOPBACK == (lRxSrcIPv4Addr_u32 & RBA_ETHIPV4_LOOPBACK_MASK))  ||                                        /* Check for loop back IP            */
       (RBA_ETHIPV4_RESERVED_IP == (lRxSrcIPv4Addr_u32 & RBA_ETHIPV4_RESERVED_IP)) ||                                        /* Check for reserved IP             */
       ( (lRxSrcIPv4Addr_u32 >= RBA_ETHIPV4_MULTICASTADDR_MIN) && (lRxSrcIPv4Addr_u32 <= RBA_ETHIPV4_MULTICASTADDR_MAX) ) || /* Check for multicast IP            */
       ( lRxSrcIPv4Addr_u32 == TCPIP_AUTOIP_BROADCAST))                                                  /* Check for AutoIp broadcast address. */

    {
       lStdRetVal_en = E_NOT_OK;

       *InvalidIPAddr_pb = TRUE;
    }

    lTcpIpEthIfState_en = TcpIp_GetEthIfCtrlState( EthIfCtrlIdx_u8 );

    if( (E_OK == lStdRetVal_en) &&
        (TCPIP_STATE_ONLINE == lTcpIpEthIfState_en) )
    {
       /* Get Unicast IP and Netmask assigned to EthIfCtrl on which frame was received */
       lStdRetVal_en = TcpIp_GetIPv4AddrFrmCtrlIdx(    EthIfCtrlIdx_u8,
                                                       &lUnicastLocalIp_u32,
                                                       &lNetMask_u32 );

       /* Discard the received frame if source IP is subnet broadcast address   */
       /* This check shall be done only if source IP is within the subnet       */
       if( ((lRxSrcIPv4Addr_u32 & lNetMask_u32) == (lUnicastLocalIp_u32 & lNetMask_u32)) &&
           (lRxSrcIPv4Addr_u32 == (lRxSrcIPv4Addr_u32 | (~ lNetMask_u32))) )
       {
           lStdRetVal_en = E_NOT_OK;

           *InvalidIPAddr_pb = TRUE;
       }

       /* Discard the received frame if destination IP is zero OR input parameter IsBroadcast_b is not consistent with the destination IP */
       if( ( TCPIP_IPADDR_ZERO == lRxDestIPv4Addr_u32 ) ||                            /* Check whether the destination IP is zero */
           ( ( (lRxDestIPv4Addr_u32 == (lUnicastLocalIp_u32 | (~lNetMask_u32))) ||    /* Check whether the destination IP directed broadcast(subnet) address                  */
               (TCPIP_IPADDR_BROADCAST == lRxDestIPv4Addr_u32) ) !=                   /* Check whether the destination IP is limited broadcast address                        */
             ( FALSE != IsBroadcast_b ) ) )                                           /* Check whether the input parameter IsBroadcast_b is consistent with the above result  */
       {
           lStdRetVal_en = E_NOT_OK;

           *InvalidIPAddr_pb = TRUE;
       }

       /* TCP is point to point protocol. And if TCP frame is received with destination IP as multicast or broadcast, frame needs to be dropped. */
       if( (E_OK == lStdRetVal_en) &&
           ((uint8)TCPIP_IPPROTO_TCP ==  RxData_pcu8[RBA_ETHIPV4_TP_PROTOCOL]) &&
           ( (IsBroadcast_b != FALSE) ||
         ( ( PseudoHdr_pst->LocalIpAddr_un.IPv4Addr_u32 >= RBA_ETHIPV4_MULTICASTADDR_MIN ) &&
           ( PseudoHdr_pst->LocalIpAddr_un.IPv4Addr_u32 <= RBA_ETHIPV4_MULTICASTADDR_MAX ) ) ) )
       {
           lStdRetVal_en = E_NOT_OK;

           *InvalidIPAddr_pb = TRUE;
       }

       /* Get the localAddressId */
       if(E_OK == lStdRetVal_en)
       {
           /* If the destination IP is broadcast get the localAddressId having unicast IP assigned for the same EthIfCtrl */
           if(IsBroadcast_b != FALSE)
           {
               lRxDestIPv4Addr_u32 = lUnicastLocalIp_u32;
           }

           /* Get the local address Id mapped to the EthIf CtrlIdx and IP address */
           lStdRetVal_en = TcpIp_GetLocalAddrIdForRxFrame( EthIfCtrlIdx_u8,
                                                           lRxDestIPv4Addr_u32,
                                                           LocalAddrId_pu8 );
       }

       if(E_OK == lStdRetVal_en)
       {
           /* Update ARP table only if                                                                                                                       */
           /* 1. Unicast local IP address assigned to the EthIf controller on which frame was received (lUnicastLocalIp_u32) and remote IP are within subnet */
           /* 2. Remote IP is derived via AutoIp                                                                                                             */
           /* 3. Local IP is derived via AutoIp                                                                                                              */
           *UpdateArpTable_pb = ( ( (lRxSrcIPv4Addr_u32  & lNetMask_u32)      == (lUnicastLocalIp_u32 & lNetMask_u32)) ||
                                  ( (lRxSrcIPv4Addr_u32  & TCPIP_AUTOIP_MASK) == TCPIP_AUTOIP_NET ) ||
                                  ( (lUnicastLocalIp_u32 & TCPIP_AUTOIP_MASK) == TCPIP_AUTOIP_NET ) );

       }
    }
    else if( (E_OK == lStdRetVal_en) &&
            (TCPIP_STATE_STARTUP == lTcpIpEthIfState_en) &&        /* Reception of only broadcast Udp frame shall be allowed at controller state STARTUP to facilitate Dhcp IP assignment */
            ((uint8)TCPIP_IPPROTO_UDP == RxData_pcu8[RBA_ETHIPV4_TP_PROTOCOL] ) &&
            (TCPIP_IPADDR_BROADCAST == lRxDestIPv4Addr_u32) &&
            (FALSE != IsBroadcast_b))
    {
        lStdRetVal_en = E_OK;
    }
    else
    {
       lStdRetVal_en = E_NOT_OK;
    }

    return lStdRetVal_en;
}

/********************************************************************************************************************/
/* rba_EthIPv4_ReassemblyHandler() - This API is called by rba_EthIPv4_RxIndication. The API validates fragmentation*/
/*                                   fields and reassemble the fragmented frames.                                   */
/* Input Parameters :                                                                                               */
/*  EthIfCtrlIdx_cu8    - Index of the EthIf controller on which the IPv4 frame is received                         */
/*  RxData_pcu8         - Data pointer to the starting address of IPv4 header                                       */
/*                                                                                                                  */
/* InOut Parameters :                                                                                               */
/* PseudoHdr_pst        - The API sets the flag to indicate TCP whether the received frame is reassembled frame.    */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* ReassemblyBufDescriptor_ppst - Will point to valid buffer descriptor if on reception of a fragmented frame       */
/*                              - reassembly is complete, else contain NULL_PTR.                                    */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : Frame is valid and the received fragmented frame is reassembled.                   */
/*                     - E_NOT_OK : Frame does not match the reassembly configuration of the controller and shall be dropped */
/********************************************************************************************************************/
static Std_ReturnType rba_EthIPv4_ReassemblyHandler( const uint8 EthIfCtrlIdx_cu8,
                                                     const uint8 * RxData_pcu8,
                                                     TcpIp_PseudoHdr_tst * PseudoHdr_pst,
                                                     rba_EthIPv4_ReassemblyBufDescriptor_tst **  ReassemblyBufDescriptor_ppst )
{
    /* Local variable declaration */
    uint16              lFragFlagOffset_u16;
    Std_ReturnType      lStdRetVal_en;

    #if ( RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON )
    uint8     lLoopIdx_u8;
    uint16    lFragOffset_u16;
    uint16    lFragPayLoadLen_u16;
    uint16    lFrameId_u16;
    uint8     lNewBufIdx_u8;
    boolean   lMoreFragmentFlag_b;
    rba_EthIPv4_ReassemblyBufDescriptor_tst *   lReassemblyBufDescriptor_pst;
    const rba_EthIPv4_CtrlCfg_tst *             lCtrlCfg_pcst;
    #endif

    /* Initialize local variables */
    lStdRetVal_en  = E_NOT_OK;
    PseudoHdr_pst->IsRxFragmented_b = FALSE;
    *ReassemblyBufDescriptor_ppst  = NULL_PTR;

    #if ( RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON )

    /* Initialize controller configuration */
    lCtrlCfg_pcst = rba_EthIPv4_CtrlCfg_pcpcst[EthIfCtrlIdx_cu8];    /* rba_EthIPv4_ValidateRxFrame() has already verified that the referenced EthIfCtrl is of IPv4 type and hence no need for check against NULL_PTR  */

    lReassemblyBufDescriptor_pst = NULL_PTR;

    #endif

    /* Extract fragmentation offset and flags from the received IPv4 header */
    TcpIp_ReadU16(  ( RxData_pcu8 + RBA_ETHIPV4_FRAGOFFSET_UB ),
                    &lFragFlagOffset_u16 );


    /* Check if the frame is fragmented */
    if( 0U != (lFragFlagOffset_u16 & (RBA_ETHIPV4_FRAG_OFFSET_MASK | RBA_ETHIPV4_MORE_FRAG_FLAG)) )
    {
        /* PseudoHdr_pst->IsRxFragmented_b to TRUE indicates that received frame is fragmented */
        PseudoHdr_pst->IsRxFragmented_b = TRUE;

        #if ( RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON )    /* If reassembly is enabled */
        lMoreFragmentFlag_b = (RBA_ETHIPV4_MORE_FRAG_FLAG == (lFragFlagOffset_u16 &  RBA_ETHIPV4_MORE_FRAG_FLAG));

        /* Extract frame identification value from the received IPv4 header */
        TcpIp_ReadU16(  ( RxData_pcu8 + RBA_ETHIPV4_FRAGID_UB ),
                        &lFrameId_u16 );

        /* Extract total frame length from the IPv4 header */
        TcpIp_ReadU16(  ( RxData_pcu8 + RBA_ETHIPV4_TOTLEN_UB ),
                        &lFragPayLoadLen_u16 );

        /* Calculate IPv4 payload length */
        lFragPayLoadLen_u16 -= PseudoHdr_pst->IPHdrLen_u16;

        /* Calculate the fragmentation offset in terms of byte (Fragmentation offset is stored in terms of 8 byte word in IPv4 header) */
        lFragOffset_u16 = (lFragFlagOffset_u16 & RBA_ETHIPV4_FRAG_OFFSET_MASK) * 8u;

        /* Initialize lNewBufIdx_u8 to invalid value, so that it can be used to store and identify the available free buffer  */
        lNewBufIdx_u8 = lCtrlCfg_pcst->ReassemblyBufCnt_cu8;

        /* Loop through all configured reassembly buffers */
        for(lLoopIdx_u8 = 0; lLoopIdx_u8 < lCtrlCfg_pcst->ReassemblyBufCnt_cu8 ; lLoopIdx_u8++)
        {
            lReassemblyBufDescriptor_pst = lCtrlCfg_pcst->ReassemblyBufDescriptor_cpst + lLoopIdx_u8;

            /* Check whether the buffer is already allocated for sourceIP-destIP-FrameId-Protocol combination */
            if( (0U !=  lReassemblyBufDescriptor_pst->TimerCurrentValue_u32) &&                       /* Check whether the buffer is already in use */
                 (PseudoHdr_pst->LocalIpAddr_un.IPv4Addr_u32 == lReassemblyBufDescriptor_pst->DstIp_u32) &&
                 (PseudoHdr_pst->RemoteIpAddr_un.IPv4Addr_u32 == lReassemblyBufDescriptor_pst->SrcIp_u32) &&
                 (PseudoHdr_pst->ProtoType_en == lReassemblyBufDescriptor_pst->ProtoType_en) &&
                 (lFrameId_u16 == lReassemblyBufDescriptor_pst->FrameId_u16))
            {
                break;
            }
            else if( (0U ==  lReassemblyBufDescriptor_pst->TimerCurrentValue_u32) &&        /* Check whether the buffer is free       */
                     (lNewBufIdx_u8 == lCtrlCfg_pcst->ReassemblyBufCnt_cu8) )               /* To allocate first available free buffer */
            {
                lNewBufIdx_u8 = lLoopIdx_u8;                                                /* Store the free buffer index */
            }
            else
            {   /* Do nothing */
            }
        }

        /* Discard the fragment if */
        if( ( (lLoopIdx_u8 == lCtrlCfg_pcst->ReassemblyBufCnt_cu8) &&                                 /* Buffer is not already allocated                  */
              (lNewBufIdx_u8 == lCtrlCfg_pcst->ReassemblyBufCnt_cu8) ) ||                             /* No new buffer is available                       */
            ( ( (lFragOffset_u16 + lFragPayLoadLen_u16) > lCtrlCfg_pcst->ReassemblyBufSize_cu16) ||    /* Fragment does not fit into the reassembly buffer */
              ( ( (lFragOffset_u16 + lFragPayLoadLen_u16) == lCtrlCfg_pcst->ReassemblyBufSize_cu16) && (FALSE != lMoreFragmentFlag_b) ) ) )
        {
            /* If the received fragment does not fit in the reassembly buffer and the buffer is already allocated, release the buffer */
            if(lLoopIdx_u8 != lCtrlCfg_pcst->ReassemblyBufCnt_cu8)
            {
                lReassemblyBufDescriptor_pst->TimerCurrentValue_u32 = 0U;
                lReassemblyBufDescriptor_pst->TotalPalyloadLen_u16 = 0U;
            }

        }
        else        /* If the buffer is successfully allocated */
        {
            if(lLoopIdx_u8 == lCtrlCfg_pcst->ReassemblyBufCnt_cu8)  /* If new buffer needs to be allocated, initialize buffer descriptor */
            {
                /* Initialize local variable to new buffer descriptor pointer */
                lReassemblyBufDescriptor_pst = lCtrlCfg_pcst->ReassemblyBufDescriptor_cpst + lNewBufIdx_u8;

                /* Initialize buffer descriptor properties */
                lReassemblyBufDescriptor_pst->ProtoType_en          = PseudoHdr_pst->ProtoType_en;
                lReassemblyBufDescriptor_pst->SrcIp_u32             = PseudoHdr_pst->RemoteIpAddr_un.IPv4Addr_u32;
                lReassemblyBufDescriptor_pst->DstIp_u32             = PseudoHdr_pst->LocalIpAddr_un.IPv4Addr_u32;
                lReassemblyBufDescriptor_pst->TimerCurrentValue_u32 = lCtrlCfg_pcst->ReassemblyTimeout_cu32;
                lReassemblyBufDescriptor_pst->FrameId_u16           = lFrameId_u16;
                lReassemblyBufDescriptor_pst->FirstHolePos_u16      = 0U;
                lReassemblyBufDescriptor_pst->TotalPalyloadLen_u16  = 0U;

                /* Initialize next hole position and length element in buffer */
                *( lReassemblyBufDescriptor_pst->ReassemblyBuf_cpu16 ) = 0U;

                *( lReassemblyBufDescriptor_pst->ReassemblyBuf_cpu16 + 1 ) = lCtrlCfg_pcst->ReassemblyBufSize_cu16;
            }

            /* If the fragment received is the zeroth fragment, copy IPv4 header and first 8 octets of IPv4 payload, as it is required to send ICMP Time Exceeded message */
            #if ( TCPIP_ICMP_ENABLED == STD_ON )
            if(0 == lFragOffset_u16)
            {
                for(lLoopIdx_u8 = 0; lLoopIdx_u8 < (PseudoHdr_pst->IPHdrLen_u16 + 8u) ; lLoopIdx_u8++)
                {
                    lReassemblyBufDescriptor_pst->IcmpTimeExceededPayload_au8[lLoopIdx_u8] = RxData_pcu8[lLoopIdx_u8];
                }
            }
            #endif

            /* If all the fragments are received and reassembled rba_EthIPv4_MergeFragIntoReassemblBuf() updates output parameter ReassemblyBufDescriptor_ppst to valid buffer descriptor. */
            /* This indicates rba_EthIPv4_RxIndication() that all fragments are reassembled and rba_EthIPv4_RxIndication() can release buffer once upper layer Rx indication is called. */
            *ReassemblyBufDescriptor_ppst = lReassemblyBufDescriptor_pst;

            /* Merge the received frame with reassembly buffer */
            rba_EthIPv4_MergeFragIntoReassemblBuf( (RxData_pcu8 + PseudoHdr_pst->IPHdrLen_u16),
                                                   lMoreFragmentFlag_b,
                                                   lFragOffset_u16,
                                                   lFragPayLoadLen_u16,
                                                   ReassemblyBufDescriptor_ppst);

            lStdRetVal_en = E_OK;
        }


        #else
        /* Return E_NOT_OK as reassembly is not configured and frame received is fragmented */
        lStdRetVal_en = E_NOT_OK;
        (void)EthIfCtrlIdx_cu8;    /* Parameter EthIfCtrlIdx_cu8 is not used */
        #endif
    }
    else
    {   /* If the frame is not fragmented return E_OK */
        /* PseudoHdr_pst->IsRxFragmented_b to FALSE indicates that received frame is not fragmented */
        PseudoHdr_pst->IsRxFragmented_b = FALSE;
        lStdRetVal_en  = E_OK;
    }

    return lStdRetVal_en;

 }


/********************************************************************************************************************/
/* rba_EthIPv4_MergeFragIntoReassemblBuf() - This API merges the received fragment with the reassembly buffer and   */
/*                                           and informs the caller whether all the fragments are received.         */
/* Input Parameters :                                                                                               */
/*  RxData_pcu8         - Received data pointer, pointing to IPv4 payload                                           */
/*  MoreFragFlag_cb     - Extracted from received IPv4 header and Indicates the fragment is the last fragment       */
/*  FragOffset_cu16     - Fragmentation offset of the received fragment in terms of byte                            */
/*  FragPayLoadLen_cu16 - Length of IPv4 payload in the received fragment                                           */
/*                                                                                                                  */
/* InOut Parameters :                                                                                               */
/* ReassemblyBufDescriptor_ppst - Will point to valid buffer descriptor if on reception of a fragmented frame       */
/*                              - reassembly is complete, else contain NULL_PTR.                                    */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   void                                                                                                           */
/********************************************************************************************************************/
#if ( RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON )
static void rba_EthIPv4_MergeFragIntoReassemblBuf( const uint8 * RxData_pcu8,
                                                   const boolean MoreFragFlag_cb,
                                                   const uint16 FragOffset_cu16,
                                                   const uint16 FragPayLoadLen_cu16,
                                                   rba_EthIPv4_ReassemblyBufDescriptor_tst **  ReassemblyBufDescriptor_ppst )
{
    /* Local variable declaration */
    uint16    lFirstFreeHole_u16;
    uint16    lFragEndHole_u16;
    uint16    lFirstFreeHole_LastBytePos_u16;
    uint16    lNextHoleOfFirstFreeHole_u16;
    uint16    lFragEndHole_LastBytePos_u16;
    uint16    lNextHoleOfFragEndHole_u16;
    uint8 *   lReassemblyBuf_pu8;
    uint16 *  lReassemblyBuf_pu16;
    rba_EthIPv4_ReassemblyBufDescriptor_tst *    lReassemblyBufDescriptor_pst;

    /* Store buffer descriptor pointer and reset inout parameter ReassemblyBufDescriptor_ppst to NULL_PTR, so that*/
    /* if all fragments are received it can be set to valid descriptor pointer */
    lReassemblyBufDescriptor_pst = *ReassemblyBufDescriptor_ppst;
    *ReassemblyBufDescriptor_ppst = NULL_PTR;

    /* lReassemblyBuf_pu8 is used when buffer is accessed byte wise and lReassemblyBuf_pu16 is used when buffer is accessed word wise */
    /* Note: While accessing lReassemblyBuf_pu16 in below code, index of the array is often divided by 2 as, lReassemblyBuf_pu16 increment by 1 corresponds to lReassemblyBuf_pu8 increment by 2 */

    lReassemblyBuf_pu8 = (uint8 *)lReassemblyBufDescriptor_pst->ReassemblyBuf_cpu16;

    lReassemblyBuf_pu16 = lReassemblyBufDescriptor_pst->ReassemblyBuf_cpu16;

    /* rba_EthIPv4_FindInitialHolePos() does first two steps of merging fragment into reassembly buffer. */
    /* Step 1: Identify the start hole position which is just prior to the fragment to be placed in the buffer. And this hole position is called as lFirstFreeHole_u16 */
    /* Step 2: Identify the hole where fragment ends. And this hole position is called as FragEndHole  */
    rba_EthIPv4_FindInitialHolePos( lReassemblyBufDescriptor_pst,
                                     FragOffset_cu16,
                                     FragPayLoadLen_cu16,
                                     &lFirstFreeHole_u16,
                                     &lFragEndHole_u16 );

    /* If the fragment is not completely overlapping with received portion of the buffer, then the links between holes or length of the current hole needs to be updated */
    if(RBA_ETHIPV4_INVALID_REASSEMBLY_HOLEPOS != lFirstFreeHole_u16)
    {
        lNextHoleOfFirstFreeHole_u16   = lReassemblyBuf_pu16[(lFirstFreeHole_u16/2)];                             /* Extract the next hole position of lFirstFreeHole_u16   */
        lFirstFreeHole_LastBytePos_u16 =lFirstFreeHole_u16 +  lReassemblyBuf_pu16[(lFirstFreeHole_u16/2) + 1];    /* Calculate the last byte position of lFirstFreeHole_u16 */

        lNextHoleOfFragEndHole_u16     = lReassemblyBuf_pu16[(lFragEndHole_u16/2)];                               /* Extract the next hole position of FragEndHole          */
        lFragEndHole_LastBytePos_u16   = lFragEndHole_u16 + lReassemblyBuf_pu16[(lFragEndHole_u16/2) + 1];        /* Calculate the last byte position of FragEndHole        */

        /* Step 3: If the fragment does not overlap with the very beginning of lFirstFreeHole_u16, it leads to a hole before the fragment and for this hole length needs to be updated  */
        if((FragOffset_cu16 > lFirstFreeHole_u16) && (FragOffset_cu16 < lFirstFreeHole_LastBytePos_u16 ))
        {
            lReassemblyBuf_pu16[(lFirstFreeHole_u16/2) + 1] = FragOffset_cu16 - lFirstFreeHole_u16;
        }

        /* Step 4: */
        if(FALSE == MoreFragFlag_cb)    /* If the received fragment is the last fragment, i.e. MF flag is 0 */
        {
            lReassemblyBuf_pu16[lFirstFreeHole_u16/2u] = 0U;                                                /* Set next hole position value in lFirstFreeHole_u16 to zero to indicate that it is the last free hole in the buffer */
            lReassemblyBufDescriptor_pst ->TotalPalyloadLen_u16 = FragOffset_cu16 + FragPayLoadLen_cu16;   /* Update the frame length member in the buffer descriptor */

        }else if( lFragEndHole_LastBytePos_u16 > (FragOffset_cu16 + FragPayLoadLen_cu16) )                 /* If the fragment leaves free space in FragEndHole (i.e. after the fragment), it leads to another hole */
        {
            lReassemblyBuf_pu16[(FragOffset_cu16 + FragPayLoadLen_cu16)/2u] = lNextHoleOfFragEndHole_u16;                                                    /* Update the next hole position in the newly created hole to the next hole position pointed by FragEndHole */
            lReassemblyBuf_pu16[((FragOffset_cu16 + FragPayLoadLen_cu16)/2u) + 1u] = lFragEndHole_LastBytePos_u16 - (FragOffset_cu16 + FragPayLoadLen_cu16) ;   /* Update the length of newly created hole */
            lReassemblyBuf_pu16[lFirstFreeHole_u16/2u] = FragOffset_cu16 + FragPayLoadLen_cu16;                                                              /* Make lFirstFreeHole_u16 point to newly created hole  */

        }else if( lNextHoleOfFirstFreeHole_u16 < (FragOffset_cu16 + FragPayLoadLen_cu16) )    /* If the fragment overlaps the next hole pointed by lFirstFreeHole_u16, then next hole position of lFirstFreeHole_u16 shall be set to correct value */
        {
            lReassemblyBuf_pu16[lFirstFreeHole_u16/2u] = lNextHoleOfFragEndHole_u16;
        }else
        {   /* Do nothing */
        }

        /* Step 5: Indicate all the fragments are received OR update first hole position in the buffer descriptor if required. */

        /* Whether all the fragments are received can be decided based on the position of first and last byte of the arrived a packet  */
        /* Fragment first byte position: Fragment must start on or before first hole in the buffer */
        /* Fragment last byte position:                                                            */
        /* case 1: The arrived fragment is the last fragment (MF = 0) OR                            */
        /* case 2: If the arrived fragment is not the last fragment, {Part A} -> the fragment must end in the last hole of the buffer AND {Part B} -> fragment shall completely cover that last hole */

        /* Conditions to decide whether all the fragments are received */
        if( (FragOffset_cu16 <= lReassemblyBufDescriptor_pst->FirstHolePos_u16) &&                     /* Fragment first byte position                  */
            ( (FALSE == MoreFragFlag_cb) ||                                                            /* Fragment last byte position: Case 1           */
              ( ( (0 == lNextHoleOfFragEndHole_u16) &&                                                 /* Fragment last byte position: Case 2, {Part A} */
                  ( (FragOffset_cu16 + FragPayLoadLen_cu16) >= lFragEndHole_LastBytePos_u16 ) ) ) ) )  /* Fragment last byte position: Case 2, {Part B} */
        {
            /* indicates all fragments are received and ready to be forwarded to upper layer */
            *ReassemblyBufDescriptor_ppst = lReassemblyBufDescriptor_pst;

        }else if(FragOffset_cu16 <= lReassemblyBufDescriptor_pst->FirstHolePos_u16)    /* If the fragment overlaps with the first hole in the buffer, first hole position needs to be updated. It has 2 possible cases. */
        {
            /* Case 1: If after the fragment hole is created, then first hole position shall be pointing to start of new hole */
            if(lFragEndHole_LastBytePos_u16 > (FragOffset_cu16 + FragPayLoadLen_cu16))
            {
                lReassemblyBufDescriptor_pst->FirstHolePos_u16 = FragOffset_cu16 + FragPayLoadLen_cu16;
            }
            else /* Case 2: If after the fragment hole is not created, then first hole position shall be pointing to next hole position pointed by FragEndHole */
            {
                lReassemblyBufDescriptor_pst->FirstHolePos_u16 = lNextHoleOfFragEndHole_u16;
            }
        }
        else
        {   /* Do nothing */
        }
    }

    /* Step 6: Copy the payload into reassembly buffer */
    /* MR12 DIR 1.1 VIOLATION: The input parameters of rba_BswSrv_MemCopy() are declared as (void*) for generic implementation of MemCopy. Low Risk */
    (void)rba_BswSrv_MemCopy( &lReassemblyBuf_pu8[FragOffset_cu16],
                              RxData_pcu8,
                              FragPayLoadLen_cu16 );

}


/********************************************************************************************************************/
/* rba_EthIPv4_FindInitialHolePos() - Provides position of start of hole where received fragment starts and ends.   */
/* Input Parameters :                                                                                               */
/* ReassemblyBufDescriptor_pst - Points to allocated buffer descriptor for the received fragment                    */
/*  FragOffset_cu16            - Fragmentation offset of the received fragment in terms of byte                     */
/*  FragPayLoadLen_cu16        - Length of IPv4 payload in the received fragment                                    */
/*                                                                                                                  */
/* Out Parameters :                                                                                                 */
/* FirstFreeHole_pu16          - Offset of hole start position which is just before FragOffset_cu16.                */
/* FragEndHole_pu16            - Offset of hole start position where fragment's last byte resides.                  */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   void                                                                                                           */
/********************************************************************************************************************/
static void rba_EthIPv4_FindInitialHolePos( const rba_EthIPv4_ReassemblyBufDescriptor_tst * const ReassemblyBufDescriptor_cpcst,
                                            const uint16 FragOffset_cu16,
                                            const uint16 FragPayLoadLen_cu16,
                                            uint16 * FirstFreeHole_pu16,
                                            uint16 * FragEndHole_pu16 )
{
    uint16    lTempHolePos_u16;
    uint16 *  lReassemblyBuf_pu16;

    /* Looping through the holes in the reassembly buffer shall start from first hole which is stored in reassembly buffer descriptor  */
    lTempHolePos_u16 = ReassemblyBufDescriptor_cpcst->FirstHolePos_u16;

    lReassemblyBuf_pu16 = ReassemblyBufDescriptor_cpcst->ReassemblyBuf_cpu16;



    /* Step 1: Identify the start hole position which is just prior to the fragment to be placed in the buffer */
    /* And this hole position is called as lFirstFreeHole_u16 */
    /* MR12 RULE 15.4 VIOLATION: Avoiding multiple break statements will make code unreadable */
    while(1)
    {
        /* If the fragment completely overlaps with the already received portion of the buffer and the fragment ends before the current hole position (hole referred by lTempHolePos_u16) */
        /* then no need to update any links within the buffer. But content of the received fragment needs to be copied to the reassembly buffer. */
        if(lTempHolePos_u16 >= (FragOffset_cu16 + FragPayLoadLen_cu16) )
        {
            *FirstFreeHole_pu16 = RBA_ETHIPV4_INVALID_REASSEMBLY_HOLEPOS;
            break;
        }

        if( ( (lTempHolePos_u16 + lReassemblyBuf_pu16[(lTempHolePos_u16/2u) + 1u]) > FragOffset_cu16 ) ||                                                                     /* Check whether the fragment starts within the hole referred by lTempHolePos_u16 */
            ( (lReassemblyBuf_pu16[(lTempHolePos_u16/2)] >= FragOffset_cu16) && (lReassemblyBuf_pu16[(lTempHolePos_u16/2)] < (FragOffset_cu16 + FragPayLoadLen_cu16)) ) )   /* Check whether the fragment starts after the hole referred by lTempHolePos_u16 and */
        {                                                                                                                                                                   /* the fragnment size exceeds the first byte of the next hole referred by lTempHolePos_u16 */
            *FirstFreeHole_pu16 = lTempHolePos_u16;                                                                                                                          /* shall be considered as lFirstFreeHole_u16. */
            break;
        }

        /* Get the next hole position with reference to lTempHolePos_u16 */
        lTempHolePos_u16 = lReassemblyBuf_pu16[(lTempHolePos_u16/2)];

        /* If the search reaches the last hole in the buffer(i.e. fragment completely overlaps with the received fragment present after the last hole), no need to update any links within the buffer.  */
        /*  But content of the received fragment needs to be copied to the reassembly buffer.*/
        if(0 == lTempHolePos_u16)
        {
            *FirstFreeHole_pu16 = RBA_ETHIPV4_INVALID_REASSEMBLY_HOLEPOS;

            break;
        }
    }

    /* If the fragment is not completely overlapping with received portion of the buffer, then the links between holes or length of the current hole needs to be updated */
    if(RBA_ETHIPV4_INVALID_REASSEMBLY_HOLEPOS != (*FirstFreeHole_pu16))
    {
        /* Step 2: Identify the hole where fragment ends. And this hole position is called as FragEndHole  */
        while(1)
        {
            if( (0 == lReassemblyBuf_pu16[lTempHolePos_u16/2u]) ||                                                 /* If the current hole is the last hole in the buffer OR */
                (lReassemblyBuf_pu16[lTempHolePos_u16/2u] >= (FragOffset_cu16 + FragPayLoadLen_cu16) ) )           /* If the fragment ends before the start of next hole */
            {
                *FragEndHole_pu16 = lTempHolePos_u16;
                break;
            }

            /* Get the next hole position with reference to lTempHolePos_u16 */
            lTempHolePos_u16 = lReassemblyBuf_pu16[lTempHolePos_u16/2u];
        }
    }
}

#endif



/********************************************************************************************************************/
/* rba_EthIPv4_VerifyTPLayerChkSum() - This API verifies checksum for transport layer protocol.                     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  CtrlCfg_pcst       - Pointer to the IPv4 configuration structure corresponding to the requested EthIfCtrl.      */
/*  PseudoHdr_pcst     - Pointer to structure containing information about protocol, source IP  and destination IP. */
/*  Buffer_pcu8        - Pointer to transport layer header.                                                         */
/*  TPLayerLength_cu16 - Length of transport layer frame.                                                           */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  Std_ReturnType     - E_OK       if checksum verification is successful                                          */
/*                       E_NOT_OK   if checksum verification failed                                                 */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (RBA_ETHIPV4_VERIFY_TPLAYERCHKSUM_ENABLED)                    /* If rba_EthIPv4_VerifyTPLayerChkSum() API is enabled */
static Std_ReturnType rba_EthIPv4_VerifyTPLayerChkSum( const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                                       const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                       const uint8 * Buffer_pcu8,
                                                       const uint16 TPLayerLength_cu16 )
{

    /* Local variable declaration */
    Std_ReturnType      lRetValStdType_en;
    boolean             lVerifyChkSum_b;

#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
    uint16              lTPLayerChksum_u16 = 0xFFFFu;
#endif
    /* Local variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* Transport layer checksum verification shall be done by software only if                                                       */
    /* 1. The received frame is reassembled and is not TCP frame (For TCP frames, rba_EtnTcp internally verifies the checksum ) OR 	 */
    /* 2. Received frame is ICMP and software checksum verification for ICMP is enabled                                              */
    lVerifyChkSum_b  =
    #if ( RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON )
    ( ( FALSE != PseudoHdr_pcst->IsRxFragmented_b ) && ( TCPIP_IPPROTO_ICMP == PseudoHdr_pcst->ProtoType_en )) ||
    #endif
    #if ( RBA_ETHIPV4_SW_CHKSUM_RX_ICMP == STD_ON )
    (( TCPIP_IPPROTO_ICMP == PseudoHdr_pcst->ProtoType_en ) && ( CtrlCfg_pcst->IcmpChkSumOffloadngEnabled_cb == FALSE )) ||
    #endif
    FALSE;     /* OR with FALSE is required to avoid compilation error when above preprocessor switches are STD_OFF  */

    /* If checksum verification needs to be done by software ( only ICMP checksum verification is done by IPv4) */
    if(FALSE != lVerifyChkSum_b)
    {
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
        /* For ICMP checksum verification pseudo header shall not be considered  */
        /* MR12 DIR 1.1 VIOLATION: The input parameter of TcpIp_OnesComplChkSum() is declared as (void*) for faster implmentation of checksum calculation. Low Risk */
        lRetValStdType_en = TcpIp_OnesComplChkSum( Buffer_pcu8,
                                                    NULL_PTR,
                                                    TPLayerLength_cu16,
                                                    &lTPLayerChksum_u16 );

        /* If the checksum does not evaluate to zero, return E_NOT_OK indicating checksum verification failed */
        if( ( 0 != lTPLayerChksum_u16 ) && ( E_OK == lRetValStdType_en ) )
        {
            lRetValStdType_en = E_NOT_OK;
        }
#else

        (void)Buffer_pcu8;
        (void)TPLayerLength_cu16;

        /* Report DET: If TcpIp_OnesComplChkSum() is not available */
        RBA_ETHIPV4_CALL_DET_REPORTERROR( RBA_ETHIPV4_E_RX_INDICATION_API_ID, RBA_ETHIPV4_E_INV_ARG )

        lRetValStdType_en = E_NOT_OK;
#endif

    }
    else
    {
		/* If no software checksum verification required or in case of the frame is not an ICMP, return with E_OK  */
        lRetValStdType_en = E_OK;
    }

    /* CtrlCfg_pcst is used only when UDP and ICMP software checksum validation is enabled.  */
    (void)CtrlCfg_pcst;

    return lRetValStdType_en;
}
#endif  /* #if (RBA_ETHIPV4_VERIFY_TPLAYERCHKSUM_ENABLED)  */


/********************************************************************************************************************/
/* rba_EthIPv4_CheckProtocolUnreachable() - This API sends ICMP frame Protocol unreachable if the destination IP is */
/*                                          unicast and if zeroth fragment is received in case of fragmented frame. */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  RxData_pu8         - Data pointer to the starting address of IPv4 header                                        */
/*  PseudoHdr_pcst     - Pointer to structure containing information about protocol, source IP  and destination IP. */
/*  LocalAddrId_u8     - LocalAddressId on which ICMP frame Protocol unreachable shall be sent.                     */
/*  IsBroadcast_b      - parameter to indicate whether the received frame is broadcast.                             */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   void                                                                                                           */
/********************************************************************************************************************/

#if (TCPIP_ICMP_ENABLED == STD_ON)
static void rba_EthIPv4_CheckProtocolUnreachable( const uint8 * RxData_pu8,
                                                  const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                  TcpIp_LocalAddrIdType LocalAddrId_u8,
                                                  boolean IsBroadcast_b)
{
    uint16                      lFragFlagOffset_u16;
    TcpIp_SockAddrInetType      lRemoteAddr_st;

    /* Extract fragmentation offset and flags from the received IPv4 header */
    TcpIp_ReadU16(  ( RxData_pu8 + RBA_ETHIPV4_FRAGOFFSET_UB ),
                    &lFragFlagOffset_u16 );

    /* Icmp frame Protocol unreachable shall be sent only if the destination IP is unicast and if zeroth fragment is received in case of fragmented frame.*/
    if( (FALSE == IsBroadcast_b) &&
        ( (PseudoHdr_pcst->LocalIpAddr_un.IPv4Addr_u32 < RBA_ETHIPV4_MULTICASTADDR_MIN) || (PseudoHdr_pcst->LocalIpAddr_un.IPv4Addr_u32 > RBA_ETHIPV4_MULTICASTADDR_MAX) ) &&
        (0U == (lFragFlagOffset_u16 & RBA_ETHIPV4_FRAG_OFFSET_MASK) ) )
    {
        lRemoteAddr_st.addr[0] = PseudoHdr_pcst->RemoteIpAddr_un.IPv4Addr_u32;

        (void)rba_EthIcmp_Transmit( LocalAddrId_u8,                                                        /* LocalAddressId                               */
                                    &lRemoteAddr_st,                                                       /* Remote node IP address                       */
                                    0U,                                                                    /* TTL = 0 indicates default value will be used */
                                    (uint8)RBA_ETHICMP_DEST_UNREACH_E,                                     /* ICMP Control Message Type                    */
                                    (uint8)RBA_ETHICMP_DEST_UNREACH_PROTO_E,                               /* ICMP Control Message Code                    */
                                    ( RBA_ETHICMP_DEST_UNREACH_DATASIZE + PseudoHdr_pcst->IPHdrLen_u16 ), /* Data length                                  */
                                    RxData_pu8 );
    }
}
#endif  /* #if (TCPIP_ICMP_ENABLED == STD_ON) */

#if( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
/**
 **********************************************************************************************************************
 * \module description
 *  Increments the measurement data passed as inout parameter if it has not exceeded TCPIP_MAXUINT32.
 *
 * Parameter In:
 * None
 *
 * Parameter Out:
 * None
 *
 *  Parameter Inout:
 * \param MeasurementData_pu32            Pointer to the measurement data which shall be incremented.
 *
* \return  None
 *
 **********************************************************************************************************************
 */
LOCAL_INLINE void rba_EthIPv4_IncMeasurementData( uint32 * MeasurementData_pu32 )
{
    /* [SWS_TCPIP_00294]: All measurement data which counts data shall not overrun. */
    if(( *MeasurementData_pu32 ) < TCPIP_MAXUINT32 )
    {
        /* Increment the counter. */
        (*MeasurementData_pu32)++;
    }
}

#endif  /* #if ( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON ) */

#endif /* #if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) ) */


#define RBA_ETHIPV4_STOP_SEC_CODE
#include "rba_EthIPv4_MemMap.h"

