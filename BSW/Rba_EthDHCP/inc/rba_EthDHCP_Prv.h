

#ifndef RBA_ETHDHCP_PRV_H
#define RBA_ETHDHCP_PRV_H


#if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )
/*
****************************************************************************************************
*                                   Defines
****************************************************************************************************
*/

#define RBA_ETHDHCP_MAXUINT32       0xFFFFFFFFUL

/* According to the BOOTP and DHCP RFCs, all BOOTP/DHCP packets must be at least 300 bytes in size, and use padding to  */
/* reach this size if the actual data is shorter. */
#define RBA_ETHDHCP_MIN_FRAME_SIZE              300U

/* According to feedback from BMW the length check for >=300 Bytes in DHCP RxIndication shall be removed or reduced. */
/* The recommendation in RFC2131 shall not be followed */
#define RBA_ETHDHCP_MIN_RXFRAME_LEN             240U

/* Magic cookie as per RFC 2132. The value of standard DHCP magic cookie is the 4 octet dotted decimal 99.130.83.99 */
/*   (or hexadecimal number 63.82.53.63) in network byte order */
#define RBA_ETHDHCP_MAGICCOOKIE_VAL             0x63825363UL

/* Macro to define the maximum length of a DHCP client message. */
#define RBA_ETHDHCP_CLIENT_MSG_MAXLENGTH        548U

/* DHCP Message Type */
#define RBA_ETHDHCP_DISCOVER                    1U
#define RBA_ETHDHCP_OFFER                       2U
#define RBA_ETHDHCP_REQUEST                     3U
#define RBA_ETHDHCP_DECLINE                     4U
#define RBA_ETHDHCP_ACK                         5U
#define RBA_ETHDHCP_NACK                        6U
#define RBA_ETHDHCP_RELEASE                     7U
#define RBA_ETHDHCP_INFORM                      8U

/** DHCP hardware type, currently only ethernet is supported */
#define RBA_ETHDHCP_HWTYPE_ETH                  1U

/* DHCP Operation States */
#define RBA_ETHDHCP_BOOTREQUEST                 1U
#define RBA_ETHDHCP_BOOTREPLY                   2U

/* DHCP Broadcast Flag */
#define RBA_ETHDHCP_UNICAST                     0U
#define RBA_ETHDHCP_BROADCAST                   0x8000U // 32768

/************************/
/* DHCP message options */
/************************/

#define RBA_ETHDHCP_OPTION_MSGTYPE_LENGTH       1U
#define RBA_ETHDHCP_OPTION_ENDOPT_LENGTH        1U
#define RBA_ETHDHCP_OPTION_SUBNETMASK_LENGTH    4U
#define RBA_ETHDHCP_OPTION_REQUESTEDIP_LENGTH   4U
#define RBA_ETHDHCP_OPTION_LEASETIME_LENGTH     4U
#define RBA_ETHDHCP_OPTION_SERVERID_LENGTH      4U
#define RBA_ETHDHCP_OPTION_ROUTER_LENGTH        4U
#define RBA_ETHDHCP_OPTION_RENEWALTIME_LENGTH   4U
#define RBA_ETHDHCP_OPTION_REBINDINGTIME_LENGTH 4U

/* Vendor Extensions */
#define RBA_ETHDHCP_OPTION_OFFSET_CODE          1U       /* Offset code*/
#define RBA_ETHDHCP_OPTION_PADOPT               0U      /* PAD Option           */
#define RBA_ETHDHCP_OPTION_PADOPT_SIZE          1U      /* PAD Option Size          */
#define RBA_ETHDHCP_OPTION_ENDOPT               255U    /* END Option           */
#define RBA_ETHDHCP_OPTION_SUBNETMASK_ID        1U      /* Subnet Mask Option   */
#define RBA_ETHDHCP_OPTION_ROUTER_ID            3U      /* Router Option        */

/* DHCP Extensions Options */
#define RBA_ETHDHCP_OPTION_FQDN_ID              81U /*Needed - FQDN Option  */
#define RBA_ETHDHCP_OPTION_MAX_LENGTH           255U /*Needed - Max Option Length  */
#define RBA_ETHDHCP_OPTION_FQDN_OFFSET_VALUE    5U /*Needed - FQDN Option Offset */
#define RBA_ETHDHCP_OPTION_NONFQDN_OFFSET_VALUE 2U /*Needed - Non FQDN Option Offset */
#define RBA_ETHDHCP_OPTION_FQDN_FLAG_VALUE      0x08  /*Needed-FQDN */
#define RBA_ETHDHCP_OPTION_FQDN_RCODE_VALUE     0U  /*Needed-FQDN */
#define RBA_ETHDHCP_OPTION_FQDN_FLAG_LEN        1U /*Needed - FQDN*/
#define RBA_ETHDHCP_OPTION_FQDN_RCODE1_LEN      1U /*Needed - FQDN*/
#define RBA_ETHDHCP_OPTION_FQDN_RCODE2_LEN      1U /*Needed - FQDN*/
#define RBA_ETHDHCP_OPTION_REQUESTEDIP_ID       50U /* Needed - Requested IP Address    */
#define RBA_ETHDHCP_OPTION_LEASETIME_ID         51U /* Needed - IP Address Lease Time   */
#define RBA_ETHDHCP_OPTION_OVERLOAD_ID          52U /* Not Needed - Option Overload     */
#define RBA_ETHDHCP_OPTION_MSGTYPE_ID           53U /* Needed - DHCP Message Type       */
#define RBA_ETHDHCP_OPTION_SERVERID_ID          54U /* Needed - Server Identifier       */
#define RBA_ETHDHCP_OPTION_PARAMRQSTLIST_ID     55U /* Needed - Parameter Request List  */
#define RBA_ETHDHCP_OPTION_MESSAGE_ID           56U
#define RBA_ETHDHCP_OPTION_MAXMESSSIZE_ID       57U
#define RBA_ETHDHCP_OPTION_RENEWALTIME_ID       58U
#define RBA_ETHDHCP_OPTION_REBINDINGTIME_ID     59U
#define RBA_ETHDHCP_OPTION_CLASSID_ID           60U
#define RBA_ETHDHCP_OPTION_CLIENTID_ID          61U



/*********************************************/
/* DET macros                                */
/*********************************************/


/* DET ERROR CODE*/
#define RBA_ETHDHCP_E_NOTINIT           TCPIP_E_NOTINIT

#define RBA_ETHDHCP_E_PARAM_POINTER     TCPIP_E_NULL_PTR

#define RBA_ETHDHCP_E_INV_ARG           TCPIP_E_INV_ARG

/* DET API CODE*/

#define RBA_ETHDHCP_E_APIINIT_ID                                    (0x01)

#define RBA_ETHDHCP_E_APICLIENTINIT_ID                              (0x02)
#define RBA_ETHDHCP_E_APICLIENTSTART_ID                             (0x03)
#define RBA_ETHDHCP_E_APICLIENTSTOP_ID                              (0x04)
#define RBA_ETHDHCP_E_APICLIENTGETIPADDRESS_ID                      (0x05)
#define RBA_ETHDHCP_E_APICLIENTIPCONFLICTDETECTED_ID                (0x06)
#define RBA_ETHDHCP_E_APICLIENTMAINFUNCTION_ID                      (0x07)
#define RBA_ETHDHCP_E_APICLIENTREADOPTION_ID                        (0x08)
#define RBA_ETHDHCP_E_APICLIENTWRITEOPTION_ID                       (0x09)
#define RBA_ETHDHCP_E_APICLIENTRXINDICATION_ID                      (0x0A)
#define RBA_ETHDHCP_E_APICLIENTCHKPORTNUM_ID                        (0x0B)


#if( TCPIP_DEV_ERROR_DETECT == STD_ON )

    #define RBA_ETHDHCP_DET_REPORT_ERROR_NOK( CONDITION, API, ERROR )    \
                if ( CONDITION )                                           \
                {                                                          \
                    (void)Det_ReportError( ( RBA_ETHDHCP_MODULE_ID ), ( RBA_ETHDHCP_INSTANCE_ID ), ( API ), ( ERROR ) );  \
                    return ( E_NOT_OK );                                   \
                }


    #define RBA_ETHDHCP_DET_REPORT_ERROR( CONDITION, API, ERROR )    \
                if ( CONDITION )                                       \
                {                                                      \
                    (void)Det_ReportError( ( RBA_ETHDHCP_MODULE_ID ), ( RBA_ETHDHCP_INSTANCE_ID ), ( API ), ( ERROR ) );  \
                    return;                                             \
                }

#else

    #define RBA_ETHDHCP_DET_REPORT_ERROR_NOK( CONDITION, API, ERROR )

    #define RBA_ETHDHCP_DET_REPORT_ERROR( CONDITION, API, ERROR )

#endif /* TCPIP_DEV_ERROR_DETECT */




/* DHCP Option - Frame Format */

#define RBA_ETHDHCP_OPTFRM_IDPOS                0U
#define RBA_ETHDHCP_OPTFRM_LENPOS               1U
#define RBA_ETHDHCP_OPTFRM_DATAPOS0             2U
#define RBA_ETHDHCP_OPTFRM_FQDN_FLAGPOS         2U  /*FQDN Flag position*/
#define RBA_ETHDHCP_OPTFRM_FQDN_RCODE1POS       3U  /*FQDN RCODE1 position*/
#define RBA_ETHDHCP_OPTFRM_FQDN_RCODE2POS       4U  /*FQDN RCODE2 position*/
#define RBA_ETHDHCP_OPTFRM_FQDN_DATAPOS0        5U

/* MACROS needed for future implementation*/
/* #define  RBA_ETHDHCP_OPTFRM_DATAPOS1     3U */
/* #define  RBA_ETHDHCP_OPTFRM_DATAPOS2     4U */
/* #define  RBA_ETHDHCP_OPTFRM_DATAPOS3     5U */


#endif  /* #if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )  */

#endif /* RBA_ETHDHCP_PRV_H */

