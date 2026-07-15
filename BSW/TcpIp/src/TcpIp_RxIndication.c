

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cbk.h"

#if ( TCPIP_ARP_ENABLED == STD_ON )
#include "rba_EthArp_Cbk.h"
#endif

#if ( TCPIP_IPV4_ENABLED == STD_ON )
#include "rba_EthIPv4_Cbk.h"
#endif

#if ( TCPIP_IPV6_ENABLED == STD_ON )
#include "rba_EthIPv6_Cbk.h"
#endif

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE
#include "TcpIp_MemMap.h"

/********************************************************************************************************************/
/* TcpIp_RxIndication() - This API service the TCP/IP stack gets an indication and the data of a received frame.    */
/*                                                                                                                  */
/* Service ID           - 0x14                                                                                      */
/* Sync/Async           - Synchronous                                                                               */
/* Reentrancy           - Reentrant                                                                                 */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*      CtrlIdx         - Index of the EthIf controller.                                                            */
/*      FrameType       - Frame type of received Ethernet frame.                                                    */
/*      IsBroadcast     - parameter to indicate a broadcast frame                                                   */
/*      PhysAddrPtr     - Pointer to Physical source address (MAC address in network byte order) of received        */
/*                        Ethernet frame                                                                            */
/*      DataPtr         - Pointer to payload of the received Ethernet frame (i.e. Ethernet header is not provided). */
/*      LenByte         - Length of received data.                                                                  */
/*                                                                                                                  */
/* Return  :    void                                                                                                */
/*                                                                                                                  */
/********************************************************************************************************************/
void TcpIp_RxIndication( uint8 CtrlIdx,
                         Eth_FrameType FrameType,
                         boolean IsBroadcast,
                         uint8 * PhysAddrPtr,
                         uint8 * DataPtr,
                         uint16 LenByte )
{
    /* Check for DET errors */
    /* Report DET if TcpIp module is uninitialised */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_RXINDICATION_API_ID, TCPIP_E_NOTINIT )

    /* Report DET if PhysAddrPtr OR DataPtr is a null pointer */
    TCPIP_DET_REPORT_ERROR_RET_VOID ( ( ( NULL_PTR ==  PhysAddrPtr ) || ( NULL_PTR ==  DataPtr ) ),
                                     TCPIP_E_RXINDICATION_API_ID, TCPIP_E_NULL_PTR )

    /* Check the frame type and correspondingly call the upper layer functions */
    switch ( FrameType )
    {
#if (TCPIP_ARP_ENABLED == STD_ON)
        case TCPIP_TYPE_ARP:
        {
            /* If received frame is an ARP frame, call rba_EthArp_RxArpFrame() */
            (void)rba_EthArp_RxIndication(    CtrlIdx,        /* EthIf controller index */
                                              DataPtr,        /* pointer to ARP payload */
                                              LenByte );      /* Number of data bytes received */
        }
        break;
#endif

#if (TCPIP_IPV4_ENABLED == STD_ON)
        case TCPIP_TYPE_IPV4:
        {   /* If received frame is an IP frame, call rba_EthIPv4_RxIndication() */
            rba_EthIPv4_RxIndication(   CtrlIdx,        /* EthIf controller index */
                                        IsBroadcast,    /* indicates a broadcast frame */
                                        PhysAddrPtr,    /* MAC address of the sending node */
                                        DataPtr,        /* pointer to IPv4 packet payload */
                                        LenByte );      /* Number of data bytes received */
        }
        break;
#endif

#if (TCPIP_IPV6_ENABLED == STD_ON)
        case TCPIP_TYPE_IPV6:
        {
            /* The IsBroadcast parameter is not used because there is no broadcast feature in IPv6. */
            (void)IsBroadcast;

            /* If received frame is an IP frame, call rba_EthIPv6_RxIndication() */
            rba_EthIPv6_RxIndication(   CtrlIdx,        /* EthIf controller index */
                                        PhysAddrPtr,    /* MAC address of the sending node */
                                        DataPtr,        /* pointer to IPv6 packet payload */
                                        LenByte );      /* Number of data bytes received */
        }
        break;
#endif

        default:
        {
            /* Do nothing */
        }
        break;
    }

    return;
}

#define TCPIP_STOP_SEC_CODE
#include "TcpIp_MemMap.h"

#endif /* #ifdef TCPIP_CONFIGURED */
