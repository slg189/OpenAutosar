

#ifndef RBA_ETHUDP_PRV_H
#define RBA_ETHUDP_PRV_H

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )

/*
***************************************************************************************************
* Includes
***************************************************************************************************
*/

#if (TCPIP_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif

/*
 ***************************************************************************************************
 * Global variable declaration
 ***************************************************************************************************
 */
#define RBA_ETHUDP_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthUdp_MemMap.h"

extern const rba_EthUdp_Config_to *       rba_EthUdp_Config_pco;   /* Pointer will be initialized in the Init Function */
extern uint16                             rba_EthUdp_UsedSocCnt_u16;
extern uint16                             rba_EthUdp_PortAny_u16;

#define RBA_ETHUDP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthUdp_MemMap.h"

#define RBA_ETHUDP_START_SEC_VAR_CLEARED_32
#include "rba_EthUdp_MemMap.h"

#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
/* Counter to keep count of datagrams dropped for port not mapped to valid port*/
extern uint32 rba_EthUdp_InvalidIpPortDropCnt_u32;
#endif

#define RBA_ETHUDP_STOP_SEC_VAR_CLEARED_32
#include "rba_EthUdp_MemMap.h"


/*
****************************************************************************************************
*                                   Defines
****************************************************************************************************
*/

/* In UDP header datagram length upper byte offset */
#define     RBA_ETHUDP_UDPFRAMELENGTH_UB            4U

/* In UDP header datagram length lower byte offset */
#define     RBA_ETHUDP_UDPFRAMELENGTH_LB            5U

/* In UDP header checksum upper byte offset */
#define     RBA_ETHUDP_UDPCHKSUM_UB                 6U

/* In UDP header checksum lower byte offset */
#define     RBA_ETHUDP_UDPCHKSUM_LB                 7U

/* Zero Copy defines */
/*************************DYNAMIC MODE******************************************/
/*******************************************************************************/
/* Reuse header offset and length */
#define     RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_HDR_OFFSET                 (32U)
#define     RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_HDR_LEN                    (4U)

/* Offset where to write the Src port in the Zero Copy Reuse header buffer */
#define     RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_SRCPORT_OFFSET             (32U)

/* Offset where to write the Dest port in the Zero Copy Reuse header buffer */
#define     RBA_ETHUDP_ZEROCOPY_DYNAMICMODE_REUSE_DESTPORT_OFFSET            (34U)

/* OneTime header length */
#define     RBA_ETHUDP_ZEROCOPY_ONETIME_HDR_LEN                              (4U)

/* Offset where to write the Length in the Zero Copy OneTimeUdp header buffer */
#define     RBA_ETHUDP_ZEROCOPY_ONETIME_LENGTH_OFFSET                        (0U)

/* Offset where to write the Checksum in the Zero Copy OneTimeUdp header buffer */
#define     RBA_ETHUDP_ZEROCOPY_ONETIME_CHKSUM_OFFSET                        (2U)

/*************************STATIC MODE*******************************************/
/*******************************************************************************/
/* Reuse header offset and length */
#define     RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_HDR_OFFSET                  (38U)
#define     RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_HDR_LEN                     (8U)

/* Offset where to write the Src port in the Zero Copy Reuse header buffer */
#define     RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_SRCPORT_OFFSET              (38U)

/* Offset where to write the Dest port in the Zero Copy Reuse header buffer */
#define     RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_DESTPORT_OFFSET             (40U)

/* Offset where to write the length in the Zero Copy Reuse header buffer */
#define     RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_LENGTH_OFFSET               (42U)

/* Offset where to write the checksum in the Zero Copy Reuse header buffer */
#define     RBA_ETHUDP_ZEROCOPY_STATICMODE_REUSE_CHKSM_OFFSET                (44U)

/*
 ***************************************************************************************************
 * DET Defines
 ***************************************************************************************************
 */

/* API IDs for DET Support */

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_Init()  */
#define RBA_ETHUDP_E_INIT_API_ID                                  (0x01U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_ProvideTxBuffer()  */
#define RBA_ETHUDP_E_PROVIDE_TXBUFFER_API_ID                      (0x02U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_Transmit()  */
#define RBA_ETHUDP_E_TRANSMIT_API_ID                              (0x03U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_RxIndication() */
#define RBA_ETHUDP_E_RX_INDICATION_API_ID                         (0x04U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_GetSocket() */
#define RBA_ETHUDP_E_GET_SOCKET_API_ID                            (0x05U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_Bind() */
#define RBA_ETHUDP_E_BIND_API_ID                                  (0x06U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_ChangeParameter() */
#define RBA_ETHUDP_E_CHANGE_PARAMETER_API_ID                      (0x07U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_TerminateSocUsingLocalAddr() */
#define RBA_ETHUDP_E_TERMINATE_SOC_USING_LOCALADDR_API_ID         (0x08U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_Close() */
#define RBA_ETHUDP_E_CLOSE_API_ID                                 (0x09U)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_GetAndResetMeasurementData() */
#define RBA_ETHUDP_E_GETANDRESET_MEAS_DATA_API_ID                 (0x0AU)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_MainFunction() */
#define RBA_ETHUDP_E_MAINFUNCTION_API_ID                          (0x0BU)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_CheckFragmentation() */
#define RBA_ETHUDP_E_CHECKFRAGMENTATION_API_ID                    (0x0CU)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_ZeroCopyFillHeader() */
#define RBA_ETHUDP_E_ZEROCOPYFILLHEADER_API_ID                    (0x0DU)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_ZeroCopyTransmit() */
#define RBA_ETHUDP_E_ZEROCOPYTRANSMIT_API_ID                      (0x0EU)

/* Indicates the Api Id passed in the call of Det_ReportError, corresponding to rba_EthUdp_MainFunction() */
#define RBA_ETHUDP_E_GET_SOCKET_INFO_API_ID                       (0x0FU)

/* Development Error Codes for DET Support */

/*RBA_ETHUDP_E_NOTINIT: API service used without rba_EthUdp_Init module initialisation */
#define RBA_ETHUDP_E_NOTINIT                                      TCPIP_E_NOTINIT

/* RBA_ETHUDP_E_INV_ARG: Invalid argument received */
#define RBA_ETHUDP_E_INV_ARG                                      TCPIP_E_INV_ARG

/* RBA_ETHUDP_E_NULL_PTR: API service called with NULL pointer as an argument */
#define RBA_ETHUDP_E_NULL_PTR                                     TCPIP_E_NULL_PTR

/* RBA_ETHUDP_E_INIT_FAILED: API service called without TcpIp module initialization */
#define RBA_ETHUDP_E_INIT_FAILED                                 TCPIP_E_INIT_FAILED

/* RBA_ETHUDP_E_AFNOSUPPORT: Address family not supported by protocol family */
#define RBA_ETHUDP_E_AFNOSUPPORT                                 TCPIP_E_AFNOSUPPORT

/* RBA_ETHUDP_E_ADDRNOTAVAIL: Can't assign requested address */
#define RBA_ETHUDP_E_ADDRNOTAVAIL                                TCPIP_E_ADDRNOTAVAIL

/* RBA_ETHUDP_E_ADDRINUSE: Address already in use */
#define RBA_ETHUDP_E_ADDRINUSE                                   TCPIP_E_ADDRINUSE

/* RBA_ETHUDP_E_NOPROTOOPT: Protocol not available */
#define RBA_ETHUDP_E_NOPROTOOPT                                  TCPIP_E_NOPROTOOPT

/* RBA_ETHUDP_E_ZERO_CHECKSUM_IPV6: Checksum fied in the UDP header for an IPv6 packet is zero */
#define RBA_ETHUDP_E_ZERO_CHECKSUM_IPV6                         (0x10)    /* Continue numbering from last value for Development Error Codes in TcpIp: 0x0FU */


/*
 ***************************************************************************************************
 * DET Check
 ***************************************************************************************************
 */

/* No reporting to DET module if Development Detection Error is OFF */
/* According to CAP Coding Guideline, Development Error detection shall not be controlled by configuration switch DevErrorDetect for safety reasons. */
/* This behavior is compliant to runtime-error behavior and a deviation from AUTOSAR */
#if (TCPIP_DEV_ERROR_DETECT == STD_ON)
#define RBA_ETHUDP_CALL_DET_REPORTERROR(API, ERROR) (void)Det_ReportError(RBA_ETHUDP_MODULE_ID, RBA_ETHUDP_INSTANCE_ID, API, ERROR);
#else
#define RBA_ETHUDP_CALL_DET_REPORTERROR(API, ERROR)
#endif

/* DET For functions with return type as 'void' */
#define RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( CONDITION, API, ERROR )            \
    if(CONDITION)                                                                \
    {                                                                            \
        RBA_ETHUDP_CALL_DET_REPORTERROR(API, ERROR)                              \
        return;                                                                  \
    }                                                                            \

/* For functions with a return value */
#define RBA_ETHUDP_DET_REPORT_ERROR_RET_VALUE( CONDITION, API, ERROR, RETVAL )   \
    if(CONDITION)                                                                \
    {                                                                            \
        RBA_ETHUDP_CALL_DET_REPORTERROR(API, ERROR)                              \
        return (RETVAL);                                                         \
    }                                                                            \

/*  Macro For functions to report DET but does not return to calling function */
#define RBA_ETHUDP_DET_REPORT_ERROR_NO_RET( CONDITION, API, ERROR )              \
    if( CONDITION )                                                              \
    {                                                                            \
        RBA_ETHUDP_CALL_DET_REPORTERROR(API, ERROR)                              \
    }                                                                            \

/*  Macro for functions to report DET always, but does not return to calling function */
#define RBA_ETHUDP_DET_REPORT_ERROR_TRUE_NO_RET( API, ERROR )                    \
    RBA_ETHUDP_CALL_DET_REPORTERROR(API, ERROR)                                  \

/*  Macro for functions to report DET always, and returns to calling function */
#define RBA_ETHUDP_DET_REPORT_ERROR_TRUE_RET_VALUE( API, ERROR, RETVAL )         \
    RBA_ETHUDP_CALL_DET_REPORTERROR(API, ERROR)                                  \
    return (RETVAL);                                                             \


/*
 ***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
 */

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

extern void rba_EthUdp_ResetSockProperties                  ( TcpIp_SocketIdType SocketId_u16 );

extern Std_ReturnType rba_EthUdp_ActivateOngoingOperation   ( TcpIp_SocketIdType SocketId_u16 );

extern void rba_EthUdp_ReleaseOngoingOperation              ( TcpIp_SocketIdType SocketId_u16 );

extern Std_ReturnType rba_EthUdp_IsCloseOperationAllowed    ( TcpIp_SocketIdType SocketId_u16 );

#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"


#endif  /* ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHUDP_PRV_H */
