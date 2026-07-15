

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */
#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED

#if ( TCPIP_ICMP_ENABLED == STD_ON )
#include "rba_EthIcmp.h"

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************/
/*                                                                                                                  */
/* TcpIp_IcmpTransmit() By this API service the TCP/IP stack sends an ICMP message according to the specified       */
/*                      parameters.                                                                                 */
/*                                                                                                                  */
/* Service ID                 - 0x0C                                                                                */
/* Sync/Async                 - Synchronous                                                                         */
/* Reentrancy                 - Non Reentrant                                                                       */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalIpAddrId       - IP address identifier representing the local IP address and EthIf controller which shall  */
/*                      be used for transmission of the ICMP message.                                               */
/*  RemoteAddrPtr       - Pointer to struct representing the remote address                                         */
/*  Ttl                 - Time to live value to be used for the ICMP message. If 0 is specified the default value   */
/*                      shall be used.                                                                              */
/*  Type                - Type field value to be used in the ICMP message                                           */
/*                      (Note: the value of the type field determines the format of the remaining ICMP message data)*/
/*  Code                - Code field value to be used in the ICMP message                                           */
/*  DataLength          - Length of ICMP message                                                                    */
/*  DataPtr         - Pointer to data which shall be sent as ICMP message data                                      */
/*                                                                                                                  */
/* Std_ReturnType       Result of operation                                                                         */
/*      E_OK            The ICMP message has been sent successfully                                                 */
/*      E_NOT_OK        The ICMP message was not sent.                                                              */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType TcpIp_IcmpTransmit( TcpIp_LocalAddrIdType LocalIpAddrId,
                                   const TcpIp_SockAddrType * RemoteAddrPtr,
                                   uint8 Ttl,
                                   uint8 Type,
                                   uint8 Code,
                                   uint16 DataLength,
                                   const uint8 * DataPtr )
{
    Std_ReturnType    lRetType_en;

    lRetType_en = E_NOT_OK;

    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_ICMP_TRANSMIT_API_ID, TCPIP_E_NOTINIT, E_NOT_OK )

    /* Report DET if RemoteAddrPtr is a NULLPTR */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == RemoteAddrPtr ), TCPIP_E_ICMP_TRANSMIT_API_ID, TCPIP_E_NULL_PTR, E_NOT_OK )

    if( ( TCPIP_AF_INET == ( RemoteAddrPtr->domain ) ) &&
        ( TCPIP_AF_INET == ( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[LocalIpAddrId].IpDomainType_u32 ) ) )
    {
        /* MR12 RULE 11.3 VIOLATION: Typecasted to TcpIp_SockAddrInetType to match the parameter Type of function definition. Cast is safe because the domain is checked to be equal to TCPIP_AF_INET above. */
        lRetType_en = rba_EthIcmp_Transmit( LocalIpAddrId,
                                            ( (const TcpIp_SockAddrInetType * ) RemoteAddrPtr ),
                                            Ttl,
                                            Type,
                                            Code,
                                            DataLength,
                                            DataPtr );
    }

    return lRetType_en;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* ( TCPIP_ICMP_ENABLED == STD_ON ) */
#endif /* #ifdef TCPIP_CONFIGURED */
