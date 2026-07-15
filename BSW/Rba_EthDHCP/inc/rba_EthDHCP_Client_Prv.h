

#ifndef RBA_ETHDHCPCLIENT_PRV_H
#define RBA_ETHDHCPCLIENT_PRV_H

#if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )

/*********************************************************************/
/* ISO 13400-2 Constants - Table 10 - recommended performance values */
/*********************************************************************/

#define RBA_ETHDHCP_PROBE_WAIT          1U   /* second   (initial random delay)                 */
#define RBA_ETHDHCP_PROBE_MIN           1U   /* second   (minimum delay till repeated probe)    */
#define RBA_ETHDHCP_PROBE_MAX           1U   /* seconds  (maximum delay till repeated probe)    */
#define RBA_ETHDHCP_PROBE_NUM           1U  /*          (number of probe packets)              */
#define RBA_ETHDHCP_ANNOUNCE_NUM        1U  /*          (number of announcement packets)       */
#define RBA_ETHDHCP_ANNOUNCE_INTERVAL   1U  /* seconds  (time between announcement packets)    */
#define RBA_ETHDHCP_ANNOUNCE_WAIT       1U  /* seconds  (delay before announcing)              */

/* Time in Tick */
#define RBA_ETHDHCP_PROBE_WAIT_TICK         (RBA_ETHDHCP_PROBE_WAIT         * RBA_ETHDHCP_TICKTIME)
#define RBA_ETHDHCP_PROBE_MIN_TICK          (RBA_ETHDHCP_PROBE_MIN          * RBA_ETHDHCP_TICKTIME)
#define RBA_ETHDHCP_PROBE_MAX_TICK          (RBA_ETHDHCP_PROBE_MAX          * RBA_ETHDHCP_TICKTIME)
#define RBA_ETHDHCP_ANNOUNCE_INTERVAL_TICK  (RBA_ETHDHCP_ANNOUNCE_INTERVAL  * RBA_ETHDHCP_TICKTIME)
#define RBA_ETHDHCP_ANNOUNCE_WAIT_TICK      (RBA_ETHDHCP_ANNOUNCE_WAIT      * RBA_ETHDHCP_TICKTIME)

#define RBA_ETHDHCP_MAX_CONFLICTS               10U  /*          (max conflicts before rate limiting)   */
#define RBA_ETHDHCP_RATE_LIMIT_INTERVAL         60U  /* seconds  (delay between successive attempts)    */
#define RBA_ETHDHCP_DEFEND_INTERVAL             10U  /* seconds  (min. wait between defensive ARPs)     */

/* Time in Tick */
#define RBA_ETHDHCP_RATE_LIMIT_INTERVAL_TICK    (RBA_ETHDHCP_RATE_LIMIT_INTERVAL    * RBA_ETHDHCP_TICKTIME)
#define RBA_ETHDHCP_DEFEND_INTERVAL_TICK        (RBA_ETHDHCP_DEFEND_INTERVAL        * RBA_ETHDHCP_TICKTIME)

/************************************/
/* Retransmission TimeOut constants */
/************************************/

#define RBA_ETHDHCP_MAX_RETRANSMIT_REQUEST      4U  /* Frames                                   */
#define RBA_ETHDHCP_MAXI_RETRANSMIT_TIME        64U  /* second  - Maximum retransmit time for DISCOVER Frames */
#define RBA_ETHDHCP_MINUTE_RETRANSMIT_TIME      60U  /* seconds - Maximum retransmit time for REQUEST Frames  */

#define RBA_ETHDHCP_MAXI_RETRANSMIT_TIME_TICK   (RBA_ETHDHCP_MAXI_RETRANSMIT_TIME   * RBA_ETHDHCP_TICKTIME)
#define RBA_ETHDHCP_MINUTE_RETRANSMIT_TIME_TICK (RBA_ETHDHCP_MINUTE_RETRANSMIT_TIME * RBA_ETHDHCP_TICKTIME)

/* Release Message MAX retransmission constant*/
#define RBA_ETHDHCP_MAX_TXATTEMPTS_RELEASE_FRM      10      /* Release Frame MAX tries */


/************************************/
/* Tick Time constants              */
/************************************/

#define RBA_ETHDHCPCLIENT_MINUTETICK            (RBA_ETHDHCP_TICKTIME * 60U)

/************************************/
/* SubnetMask Conversion MACROS     */
/************************************/
#define RBA_ETHDHCP_SUBNETMASK_U32_VALUE        0xFFFFFFFFUL
#define RBA_ETHDHCP_SUBNETMASK_32U_MACRO        0x00000020UL
#define RBA_ETHDHCP_SUBNETMASK_01U_MASK         0x00000001UL

/************************************/
/* Extern declarations              */
/************************************/

#define RBA_ETHDHCP_START_SEC_VAR_CLEARED_8
#include "rba_EthDHCP_MemMap.h"
/* State of DHCP_ClientInit() API call */
extern boolean rba_EthDHCP_ClientInitFlag_b;

#define RBA_ETHDHCP_STOP_SEC_VAR_CLEARED_8
#include "rba_EthDHCP_MemMap.h"

#define RBA_ETHDHCP_START_SEC_VAR_CLEARED_UNSPECIFIED
#include "rba_EthDHCP_MemMap.h"

/* Structure used by APIs to specify an IP address */
extern TcpIp_SockAddrInetType rba_EthDHCP_SrvrSockAddr_st;

/* Pointer - will be initialized with the DHCP Client structure in the Init Function */
extern const rba_EthDHCP_ClntConfig_tst * rba_EthDHCP_ClientCurrConfig_pco;

#define RBA_ETHDHCP_STOP_SEC_VAR_CLEARED_UNSPECIFIED
#include "rba_EthDHCP_MemMap.h"

#endif  /* #if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHDHCPCLIENT_PRV_H */


