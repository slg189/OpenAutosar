

#ifndef RBA_ETHIPV4_PRV_H
#define RBA_ETHIPV4_PRV_H

#if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) )

/*
***************************************************************************************************
* Includes
***************************************************************************************************
*/

#if (TCPIP_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif

#define RBA_ETHIPV4_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthIPv4_MemMap.h"

/* Pointer to IPv4 configuration structure */
extern const rba_EthIPv4_CtrlCfg_tst * const * rba_EthIPv4_CtrlCfg_pcpcst;

#define RBA_ETHIPV4_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthIPv4_MemMap.h"

#if( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )

#define RBA_ETHIPV4_START_SEC_VAR_CLEARED_32
#include "rba_EthIPv4_MemMap.h"

/* Global variable to store the count of dropped frames due to invalid Source or Destination IP address. */
extern uint32 rba_EthIPv4_InvalidIPaddrDropCnt_u32;

#define RBA_ETHIPV4_STOP_SEC_VAR_CLEARED_32
#include "rba_EthIPv4_MemMap.h"

#endif

/*
****************************************************************************************************
*                                   Defines
****************************************************************************************************
*/

#define     RBA_ETHIPV4_ADDR_ANY                        0x00000000UL     /* IP address ANY                          */

#define     RBA_ETHIPV4_LOOPBACK                        0x7F000000UL     /* Loopback address                        */

#define     RBA_ETHIPV4_LOOPBACK_MASK                   0xFF000000UL     /* Mask to check loop back IP              */

#define     RBA_ETHIPV4_RESERVED_IP                     0xF0000000UL     /* Reserved IP range 240.0.0.0/4           */

#define     RBA_ETHIPV4_IPV4_VERSION_NUM                0x4U             /* IPv4 version number used in IPv4 header */

#define     RBA_ETHIPV4_MORE_FRAG_FLAG                  0x2000U          /* More fragments flag                     */

#define     RBA_ETHIPV4_FRAG_OFFSET_MASK                 0x1fffU         /* Mask for fragmentation offset field     */

#define     RBA_ETHIPV4_TTL_VALUE                       0x01U            /* Default TimeToLive value                */

#define     RBA_ETHIPV4_TOS_VALUE                       0x00U            /* Default TypeOfService value             */

#define     RBA_ETHIPV4_MACADDR_LEN                     6U               /* Number of bytes in MAC address          */

#define     RBA_ETHIPV4_HIDDEN_HDR_LEN                  12U               /* Two bytes to store buffer length, one byte is required to store frame priority, six bytes to store destination MAC and */
                                                                          /*  an additional three bytes padding to make buffer word bounded */

#define     RBA_ETHIPV4_HIDDEN_HDR_FRAMEPRIO_OFFSET     2U                /* Position of frame priority within the hidden header section of the fragmentation buffer */

#define     RBA_ETHIPV4_HIDDEN_HDR_DSTMAC_OFFSET        3U                /* Position of destination MAC within the hidden header section of the fragmentation buffer */

#define     RBA_ETHIPV4_INVALID_REASSEMBLY_HOLEPOS      0xffffU

/* IPv4 header fields */
#define     RBA_ETHIPV4_IPV4VER_HDRLEN_TOS_UB           0U               /* Position of upper byte of version number, HeaderLength and TypeOfService in IPv4 header */
#define     RBA_ETHIPV4_TOTLEN_UB                       2U               /* Position of upper byte of IPv4 datagram length in IPv4 header                           */
#define     RBA_ETHIPV4_FRAGID_UB                       4U               /* Position of upper byte of fragmentation identifier in IPv4 header                       */
#define     RBA_ETHIPV4_FRAGOFFSET_UB                   6U               /* Position of upper byte of fragmentation offset in IPv4 header                           */
#define     RBA_ETHIPV4_TTL_OFFSET                      8U               /* Position of TimeToLive value in IPv4 header                                             */
#define     RBA_ETHIPV4_IPHDRCHECKSUM_UB                10U              /* Position of upper byte of IPv4 header checksum in IPv4 header                           */
#define     RBA_ETHIPV4_MORE_FRAGMENT_FLAG_POS          13U              /* Position of more fragment bit within word comprising of bytes 7 and 8 of IPV4 header    */
#define     RBA_ETHIPV4_DONT_FRAGMENT_FLAG_POS          14U              /* Position of don't fragment bit (DF) within word comprising of bytes 7 and 8 of IPV4 header. */


#define RBA_ETHIPV4_START_SEC_VAR_INIT_16
#include "rba_EthIPv4_MemMap.h"
extern uint16 rba_EthIPv4_FragID_u16;
#define RBA_ETHIPV4_STOP_SEC_VAR_INIT_16
#include "rba_EthIPv4_MemMap.h"

/* Development Error Codes for DET Support   */
#define RBA_ETHIPV4_E_NOTINIT                                 TCPIP_E_NOTINIT       /* API service used without rba_EthIPv4_Init module initialisation */
#define RBA_ETHIPV4_E_INV_ARG                                 TCPIP_E_INV_ARG       /* Invalid argument received                                       */
#define RBA_ETHIPV4_E_NULL_PTR                                TCPIP_E_NULL_PTR      /* API service called with NULL pointer as an argument             */
#define RBA_ETHIPV4_E_INIT_FAILED                             TCPIP_E_INIT_FAILED   /* API service called without TcpIp module initialization          */
#define RBA_ETHIPV4_E_MSGSIZE                                 TCPIP_E_MSGSIZE       /* Message too long */
#define RBA_ETHIPV4_E_BUFRELEASE_FAILED                       0xF0u                 /* API rba_EthIPv4_FragBufHandler() failed to release the buffer.  */

/* API IDs used to report DET error */
#define RBA_ETHIPV4_E_INIT_API_ID                            (0x01U)               /* API ID for rba_EthIPv4_Init()                        */
#define RBA_ETHIPV4_E_GET_IPMULTICAST_DESTETHADDR_API_ID     (0x02U)               /* API ID for rba_EthIPv4_GetIPMulticastDestEthAddr()   */
#define RBA_ETHIPV4_E_PROVIDE_TXBUFFER_API_ID                (0x03U)               /* API ID for rba_EthIPv4_ProvideTxBuffer()             */
#define RBA_ETHIPV4_E_TRANSMIT_API_ID                        (0x04U)               /* API ID for rba_EthIPv4_Transmit()                    */
#define RBA_ETHIPV4_E_RX_INDICATION_API_ID                   (0x05U)               /* API ID for rba_EthIPv4_RxIndication()                */
#define RBA_ETHIPV4_E_GETANDRESET_MEASDATA_API_ID            (0x06U)               /* API ID for rba_EthIPv4_GetAndResetMeasurementData()  */
#define RBA_ETHIPV4_E_CHK_IPV4ADDR_TYPE_API_ID               (0x07U)               /* API ID for rba_EthIPv4_ChkIPv4AddrType()             */
#define RBA_ETHIPV4_E_FRAGBUFHANDLER_API_ID                  (0x08U)               /* API ID for rba_EthIPv4_FragBufHandler()              */
#define RBA_ETHIPV4_E_VALIDATE_RXFRAME_API_ID                (0x09U)               /* API ID for rba_EthIPv4_ValidateRxFrame()             */
#define RBA_ETHIPV4_E_MAIN_FUNCTION_API_ID                   (0x0AU)               /* API ID for rba_EthIPv4_MainFunction()                */
#define RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID              (0x0BU)               /* API ID for rba_EthIPv4_ZeroCopyFillHeader()          */
#define RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID                (0x0CU)               /* API ID for rba_EthIPv4_ZeroCopyTransmit()            */
#define RBA_ETHIPV4_E_CHECKFRAGMENTATION_API_ID              (0x0DU)               /* API ID for rba_EthIPv4_CheckFragmentation()          */
#define RBA_ETHIPV4_E_GETPSEUDOHEADER_API_ID                 (0x0EU)               /* API ID for rba_EthIPv4_GetPseudoHeader()             */

/* Zero Copy defines */
#define RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET      0U
#define RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET  1U
#define RBA_ETHIPV4_ZEROCOPY_UDPONETIMEHDR_BUFFER_OFFSET 2U
#define RBA_ETHIPV4_REUSE_VERHDRLENTOS_OFFSET            18U
#define RBA_ETHIPV4_REUSE_TTL_OFFSET                     20U
#define RBA_ETHIPV4_REUSE_TP_PROTOCOL_OFFSET             21U
#define RBA_ETHIPV4_REUSE_HDRCHECKSUMFIRSTBYTE_OFFSET    22U
#define RBA_ETHIPV4_REUSE_HDRCHECKSUMSECONDBYTE_OFFSET   23U
#define RBA_ETHIPV4_REUSE_SRCIPV4_ADDR_OFFSET            24U
#define RBA_ETHIPV4_REUSE_DESTIPV4_ADDR_OFFSET           28U
#define RBA_ETHIPV4_ONETIME_TOTLENGTH_OFFSET              0U
#define RBA_ETHIPV4_ONETIME_IPV4FRAGID_OFFSET             2U
#define RBA_ETHIPV4_ONETIME_FRAGFIELD_OFFSET              4U
#define RBA_ETHIPV4_ONETIME_HEADER_LENGTH                 6U

/* Resuse header offset for the non-fragmentation use case of zerocopy transmission. */
#define RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET             18U

#define RBA_ETHIPV4_REUSE_NON_FRAG_FIXED_ID           0x100U

/* Reuse header indexes for the sub headers */
#define RBA_ETHIPV4_ZEROCOPY_REUSEHDR_ETHERNET_OFFSET          (0U)
#define RBA_ETHIPV4_ZEROCOPY_REUSEHDR_ETHERNET_LEN             (18U)
#define RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_FIRST_OFFSET        (18U)
#define RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_FIRST_LEN           (2U)
#define RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_LAST_OFFSET         (20U)
#define RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_LAST_LEN            (12U)
#define RBA_ETHIPV4_ZEROCOPY_REUSEHDR_UDP_LEN                  (4U)

/* Compiler switch for rba_EthIPv4_UpdateTPLayerChkSum() */
/* rba_EthIPv4_UpdateTPLayerChkSum() shall be available if software checksum offload needs to be done for Udp/Icmp or fragmentation is enabled */
#define RBA_ETHIPV4_UPDATE_TPLAYERCHKSUM_ENABLED             ( ((TCPIP_UDP_ENABLED ==  STD_ON) && (RBA_ETHIPV4_SW_CHKSUM_TX_UDP == STD_ON)) ||    \
                                                               ((TCPIP_ICMP_ENABLED == STD_ON) && (RBA_ETHIPV4_SW_CHKSUM_TX_ICMP == STD_ON)) ||   \
                                                               (RBA_ETHIPV4_FRAG_ENABLED == STD_ON ) )

/* Compiler switch for rba_EthIPv4_VerifyTPLayerChkSum() */
/* rba_EthIPv4_VerifyTPLayerChkSum() shall be available if software checksum verification needs to be done for Udp/Icmp or reassembly is enabled */
#define RBA_ETHIPV4_VERIFY_TPLAYERCHKSUM_ENABLED             ( ((TCPIP_UDP_ENABLED ==  STD_ON) && (RBA_ETHIPV4_SW_CHKSUM_RX_UDP == STD_ON)) ||    \
                                                               ((TCPIP_ICMP_ENABLED == STD_ON) && (RBA_ETHIPV4_SW_CHKSUM_RX_ICMP == STD_ON)) ||   \
                                                               (RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON ) )


#if (TCPIP_DEV_ERROR_DETECT == STD_ON)
#define RBA_ETHIPV4_CALL_DET_REPORTERROR(API, ERROR) (void)Det_ReportError(RBA_ETHIPV4_MODULE_ID, RBA_ETHIPV4_INSTANCE_ID, API, ERROR);
#else
#define RBA_ETHIPV4_CALL_DET_REPORTERROR(API, ERROR)
#endif

    /* DET For functions with return type as 'void' */
    #define RBA_ETHIPV4_DET_REPORT_ERROR_RET_VOID(CONDITION, API, ERROR)                                      \
            if( CONDITION )                                                                                   \
            {                                                                                                 \
                RBA_ETHIPV4_CALL_DET_REPORTERROR(API, ERROR)                                                  \
                return;                                                                                       \
            }

    /* For functions with a return value */
    #define RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( CONDITION, API, ERROR, RETVAL )                           \
            if( CONDITION )                                                                                   \
            {                                                                                                 \
                RBA_ETHIPV4_CALL_DET_REPORTERROR(API, ERROR)                                                  \
                return ( RETVAL );                                                                            \
            }


    /*  Macro for functions to report DET always, and returns to calling function */
    #define RBA_ETHIPV4_DET_REPORT_ERROR_TRUE_RET_VOID( API, ERROR)                                       \
                RBA_ETHIPV4_CALL_DET_REPORTERROR(API, ERROR)                                              \
                return;                                                                                   \

/* This structure contains information needs to be filled in IPv4 header during rba_EthIPv4_Transmit() call */
typedef struct
{
    TcpIp_ProtocolType ProtoType_en;          /* Transport layer protocol                                            */
    uint32 SrcIp_u32;                         /* Source IP to be present in the transmitted frame                    */
    uint32 DstIp_u32;                         /* Destination IP to be present in the transmitted frame               */
    uint16 TotalLen_u16;                      /* Value of the total length field in the frame to be transmitted      */
    uint16 FragId_u16;                        /* Value of fragmentation Id in the frame to be transmitted            */
    uint16 FragFlagOffset_u16;                /* Byte 6 and 7 of the IPv4 header - Flags and fragmentation offset    */
    uint8 TTL_u8;                             /* TTL value to be present in the transmitted frame                    */
    uint8 Sockowner_u8;                       /* Upper layer who is requested to transmit the frame                  */
    uint8 DiffServiceCodepoint_u8;            /* DSCP field value for the IPv4 header.                               */
}rba_EthIPv4_HdrFields_tst;

extern TcpIp_ReturnType rba_EthIPv4_GetDestEthAddr(  const TcpIp_LocalAddrConfig_to * const LocalAddrConfig_cpcst,
                                                     uint32 DestIpAddr_u32,
                                                     uint8 * DestEthAddr_pu8 );

#endif /* #if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHIPV4_PRV_H */
