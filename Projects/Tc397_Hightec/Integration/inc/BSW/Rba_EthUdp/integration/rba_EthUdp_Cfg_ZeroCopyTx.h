/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



#ifndef RBA_ETHUDP_CFG_ZEROCOPYTX_H
#define RBA_ETHUDP_CFG_ZEROCOPYTX_H

#include "TcpIp.h"

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )
#include "rba_EthUdp.h"

/* A sample integration code is given below. */
#if ( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )

LOCAL_INLINE uint16 rba_EthUdp_ZeroCopyChkSumCalculation( const TcpIp_PseudoHdr_tst * PseudoHeader_pcst,
                                                          const uint8 * DataPtr_pcu8,
                                                          uint16 DataLength_u16 );

/* Inline function definition : Integration function to compute Udp checksum for ZeroCopy transmission when Udp checksum cannot be offloaded to the HW Driver */
LOCAL_INLINE uint16 rba_EthUdp_ZeroCopyChkSumCalculation( const TcpIp_PseudoHdr_tst * PseudoHeader_pcst,
                                                          const uint8 * DataPtr_pcu8,
                                                          uint16 DataLength_u16 )
{
    /* Integrator shall place his code for computing Udp checksum */
    return 0U;
}

#endif /* ( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON ) */

#endif /* ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */
#endif /* RBA_ETHUDP_CFG_ZEROCOPYTX_H */


