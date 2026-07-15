

#ifndef RBA_ETHAUTOIP_PRV_H
#define RBA_ETHAUTOIP_PRV_H

#if ( defined(TCPIP_AUTOIP_ENABLED) && ( TCPIP_AUTOIP_ENABLED == STD_ON ) )

/*
****************************************************************************************************
*                                   Defines
****************************************************************************************************
*/

#define RBA_ETHAUTOIP_DEFENDING                 1U
#define RBA_ETHAUTOIP_RETREATING                2U

/*********************************************/
/* RFC 3927 Constants                        */
/*********************************************/

#define RBA_ETHAUTOIP_PROBE_WAIT               1U   /* second   (initial random delay)                 */
#define RBA_ETHAUTOIP_PROBE_MIN                1U   /* second   (minimum delay till repeated probe)    */
#define RBA_ETHAUTOIP_PROBE_MAX                2U   /* seconds  (maximum delay till repeated probe)    */
#define RBA_ETHAUTOIP_PROBE_NUM                3U   /*          (number of probe packets)              */
#define RBA_ETHAUTOIP_ANNOUNCE_NUM             2U   /*          (number of announcement packets)       */
#define RBA_ETHAUTOIP_ANNOUNCE_INTERVAL        2U   /* seconds  (time between announcement packets)    */
#define RBA_ETHAUTOIP_ANNOUNCE_WAIT            2U   /* seconds  (delay before announcing)              */

/* Time in Tick */
#define RBA_ETHAUTOIP_PROBE_WAIT_TICK           (RBA_ETHAUTOIP_PROBE_WAIT           * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_PROBE_MIN_TICK            (RBA_ETHAUTOIP_PROBE_MIN            * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_PROBE_MAX_TICK            (RBA_ETHAUTOIP_PROBE_MAX            * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_ANNOUNCE_INTERVAL_TICK    (RBA_ETHAUTOIP_ANNOUNCE_INTERVAL    * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_ANNOUNCE_WAIT_TICK        (RBA_ETHAUTOIP_ANNOUNCE_WAIT        * RBA_ETHAUTOIP_TICKTIME)


/*********************************************************************/
/* ISO 13400-2 Constants - Table 10 - recommended performance values */
/*********************************************************************/

#define RBA_ETHAUTOIP_DOIP_PROBE_WAIT           1u   /* second   (initial random delay)                 */
#define RBA_ETHAUTOIP_DOIP_PROBE_MIN            1u   /* second   (minimum delay till repeated probe)    */
#define RBA_ETHAUTOIP_DOIP_PROBE_MAX            1u   /* seconds  (maximum delay till repeated probe)    */
#define RBA_ETHAUTOIP_DOIP_PROBE_NUM            1u  /*          (number of probe packets)              */
#define RBA_ETHAUTOIP_DOIP_ANNOUNCE_NUM         1u  /*          (number of announcement packets)       */
#define RBA_ETHAUTOIP_DOIP_ANNOUNCE_INTERVAL    1u  /* seconds  (time between announcement packets)    */
#define RBA_ETHAUTOIP_DOIP_ANNOUNCE_WAIT        1u  /* seconds  (delay before announcing)              */

/* Time in Tick */
#define RBA_ETHAUTOIP_DOIP_PROBE_WAIT_TICK          (RBA_ETHAUTOIP_DOIP_PROBE_WAIT          * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_DOIP_PROBE_MIN_TICK           (RBA_ETHAUTOIP_DOIP_PROBE_MIN           * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_DOIP_PROBE_MAX_TICK           (RBA_ETHAUTOIP_DOIP_PROBE_MAX           * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_DOIP_ANNOUNCE_INTERVAL_TICK   (RBA_ETHAUTOIP_DOIP_ANNOUNCE_INTERVAL   * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_DOIP_ANNOUNCE_WAIT_TICK       (RBA_ETHAUTOIP_DOIP_ANNOUNCE_WAIT       * RBA_ETHAUTOIP_TICKTIME)

/********************************/
/* Conflict Detection Constants */
/********************************/

#define RBA_ETHAUTOIP_MAX_CONFLICTS            10u  /*          (max conflicts before rate limiting)   */
#define RBA_ETHAUTOIP_RATE_LIMIT_INTERVAL      60u  /* seconds  (delay between successive attempts)    */
#define RBA_ETHAUTOIP_DEFEND_INTERVAL          10u  /* seconds  (min. wait between defensive ARPs)     */

/* Time in Tick */
#define RBA_ETHAUTOIP_RATE_LIMIT_INTERVAL_TICK  (RBA_ETHAUTOIP_RATE_LIMIT_INTERVAL  * RBA_ETHAUTOIP_TICKTIME)
#define RBA_ETHAUTOIP_DEFEND_INTERVAL_TICK      (RBA_ETHAUTOIP_DEFEND_INTERVAL      * RBA_ETHAUTOIP_TICKTIME)


/*********************************************/
/* DET macros                                */
/*********************************************/


/* DET ERROR CODE*/
#define RBA_ETHAUTOIP_E_NOTINIT             TCPIP_E_NOTINIT

#define RBA_ETHAUTOIP_E_PARAM_POINTER       TCPIP_E_NULL_PTR

#define RBA_ETHAUTOIP_E_INV_ARG             TCPIP_E_INV_ARG


/* DET API CODE*/

#define RBA_ETHAUTOIP_E_APIINIT_ID                                  ((uint8)(0x01))
#define RBA_ETHAUTOIP_E_APISTART_ID                                 ((uint8)(0x02))
#define RBA_ETHAUTOIP_E_APISTOP_ID                                  ((uint8)(0x03))
#define RBA_ETHAUTOIP_E_APICONFLICTDETECTED_ID                      ((uint8)(0x04))
#define RBA_ETHAUTOIP_E_APIGETIPADDRESS_ID                          ((uint8)(0x05))
#define RBA_ETHAUTOIP_E_APIALLOCBASEIPADDR_ID                       ((uint8)(0x06))
#define RBA_ETHAUTOIP_E_APIMAINFUNCTION_ID                          ((uint8)(0x07))



#if( TCPIP_DEV_ERROR_DETECT == STD_ON )

    #define RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( CONDITION, API, ERROR )    \
                if ( CONDITION )                                           \
                {                                                          \
                    (void)Det_ReportError( ( RBA_ETHAUTOIP_MODULE_ID ), ( RBA_ETHAUTOIP_INSTANCE_ID ), ( API ), ( ERROR ) );  \
                    return ( E_NOT_OK );                                   \
                }


    #define RBA_ETHAUTOIP_DET_REPORT_ERROR( CONDITION, API, ERROR )    \
                if ( CONDITION )                                       \
                {                                                      \
                    (void)Det_ReportError( ( RBA_ETHAUTOIP_MODULE_ID ), ( RBA_ETHAUTOIP_INSTANCE_ID ), ( API ), ( ERROR ) );  \
                    return;                                             \
                }

#else

    #define RBA_ETHAUTOIP_DET_REPORT_ERROR_NOK( CONDITION, API, ERROR )

    #define RBA_ETHAUTOIP_DET_REPORT_ERROR( CONDITION, API, ERROR )

#endif /* TCPIP_DEV_ERROR_DETECT */

/************************************/
/* Extern declarations              */
/************************************/

#define RBA_ETHAUTOIP_START_SEC_VAR_CLEARED_8
#include "rba_EthAutoIp_MemMap.h"

/* Flag to indicate whether rba_EthAutoIp_Init has been executed or not */
extern boolean rba_EthAutoIp_InitFlag_b;

#define RBA_ETHAUTOIP_STOP_SEC_VAR_CLEARED_8
#include "rba_EthAutoIp_MemMap.h"

#endif  /* #if ( defined(TCPIP_AUTOIP_ENABLED) && ( TCPIP_AUTOIP_ENABLED == STD_ON ) ) */
#endif /* RBA_ETHAUTOIP_PRV_H */

