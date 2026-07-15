

#ifndef RBA_ETHDHCP_TYPES_H
#define RBA_ETHDHCP_TYPES_H

/*
****************************************************************************************************
*                                   Enumerations
****************************************************************************************************
*/

#if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )

/* Length of CHADDR in the DHCP frame */
#define RBA_ETHDHCP_CHADDR_LENGTH                   16U
/* DHCP Client HW address length */
#define RBA_ETHDHCP_HWADDR_LENGTH                   6U
/* Length of the server name in the DHCP frame */
#define RBA_ETHDHCP_SRVRNAME_LENGTH                 64U
/* Length of Boot file name in DHCP frame */
#define RBA_ETHDHCP_BOOTFILENAME_LENGTH             128U
/* Length of the options field in DHCP frame */
#define RBA_ETHDHCP_OPTION_LENGTH                   312U

#define RBA_ETHDHCP_OPTION_PARAMRQSTLIST_LENGTH         5U
#define RBA_ETHDHCP_OPTION_PARAMRQSTLIST_INFRM_LENGTH   1U

/* Note: RFC 2131 section 2: A DHCP client must be prepared to receive DHCP messages with an 'options' field of at least */
/*   length 312 octets.  This requirement implies that a DHCP client must be prepared to receive a message */
/*   of up to 576 octets, the minimum IP datagram size an IP host must be prepared to accept. */
/* DHCP frame buffer offsets  positions */

#define RBA_ETHDHCP_OP_POS                           0U
#define RBA_ETHDHCP_HWTYPE_POS                       1U
#define RBA_ETHDHCP_HWADDR_LENGTH_POS                2U
#define RBA_ETHDHCP_HOPS_POS                         3U
#define RBA_ETHDHCP_XID_POS                          4U
#define RBA_ETHDHCP_SEC_ELAPSED_POS                  8U
#define RBA_ETHDHCP_BOOTPFLAGS_POS                   10U
#define RBA_ETHDHCP_CIADDR_POS                       12U
#define RBA_ETHDHCP_YIADDR_POS                       16U
#define RBA_ETHDHCP_SIADDR_POS                       20U
#define RBA_ETHDHCP_GIADDR_POS                       24U
#define RBA_ETHDHCP_CHADDR_POS                       28U
#define RBA_ETHDHCP_SERVER_HOSTNAME_POS              44U
#define RBA_ETHDHCP_BOOT_FILENAME_POS                108U
#define RBA_ETHDHCP_MAGIC_COOKIE_POS                 236U
#define RBA_ETHDHCP_OPTION_POS                       240U
#define RBA_ETHDHCP_OPT_MSGTYPE_POS                  240U
#define RBA_ETHDHCP_OPT_MSGLEN_POS                   241U
#define RBA_ETHDHCP_OPT_MSGVAL_POS                   242U
#define RBA_ETHDHCP_IPBRND_OPT1_CLIENT_REQ_END_POS   243U
#define RBA_ETHDHCP_OPT_SUBNETMASK_POS               243U
#define RBA_ETHDHCP_OPT_SUBNETMASKLEN_POS            244U
#define RBA_ETHDHCP_OPT_SUBNETMASKVAL_POS            245U
#define RBA_ETHDHCP_OPT_END_POS                      249U
#define RBA_ETHDHCP_FRAME_SIZE                       249U
#define RBA_ETHDHCP_UINT8_MAX_VALUE                  255U



/*  */
typedef enum
{
    RBA_ETHDHCP_SOCK_BOUND      = 1,
    RBA_ETHDHCP_SOCK_NOTBOUND   = 2
} rba_EthDHCP_SockState_ten;


#include "rba_EthDHCP_Client_Types.h"

/*
****************************************************************************************************
*                                   Structures
****************************************************************************************************
*/

/* DHCP message format */
typedef struct
{
    uint32          xId_u32;
    uint32          ciAddr_u32;                                         /* Client IP addr. only filled in if client is in BOUND, RENEW or REBINDING state and can respond to ARP requests. */
    uint32          yiAddr_u32;                                         /* your (client) IP address. */
    uint32          siAddr_u32;                                         /* Server IP address */
    uint32          giAddr_u32;                                         /* Gateway IP address */
    uint32          magicCookie_u32;                                    /* DHCP Magic Cookie */

    uint16          flags_u16;

    uint8           packetOpcode_u8;                                    /* Message Type : BOOTREQUEST/BOOTREPLY */
    uint8           hwType_u8;
    uint8           hwAddrLen_u8;
    uint8           hops_u8;
    uint8           clientHwAddr_au8[RBA_ETHDHCP_CHADDR_LENGTH];        /* Client HW address. */
    uint8           serverName_au8  [RBA_ETHDHCP_SRVRNAME_LENGTH];      /* Server host name */
    uint8           bootFile_au8    [RBA_ETHDHCP_BOOTFILENAME_LENGTH];  /* Bootfile name */
    uint8           options_au8     [RBA_ETHDHCP_OPTION_LENGTH];        /* Vendor specific options */
}rba_EthDHCP_Hdr_tst;


/* DHCP Client Config - Sub-Container of DHCP Config */
#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
typedef struct
{
    rba_EthDHCPClient_Table_tst     * ClntTable_s;                  /* DHCP Client Table */
    uint8                           ClntIndex_u8;                   /* Index of the current DHCP Client */
    uint8                           ClntCtrlIdx_u8;                 /* CtrlIdx matched to the LocalAddrId */
    TcpIp_LocalAddrIdType           ClntLocalAddrId_u8;             /* LocalAddrId matched to the DHCP Client */

}rba_EthDHCP_ClntSubConfig_tst;
#endif

/* DHCP Client General Config - Sub-Container of DHCP Config */
#if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
typedef struct
{
    const rba_EthDHCP_ClntSubConfig_tst     * rba_EthDHCP_ClientSubConfig_cst;
    uint32                                  ClntWaitingServerRespTime_u32;  /* Time to wait before a retransmission if no response received */
    uint8                                   ClntConfigured_u8;              /* Number of Clients configured */

}rba_EthDHCP_ClntConfig_tst;
#endif

/* This container is a sub-container of TcpIpConfig and specifies the configuration parameters of the DHCP sub-module */
typedef struct
{
    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
    const rba_EthDHCP_ClntConfig_tst    * rba_EthDHCP_ClientConfig_cst;
    #endif
}rba_EthDHCP_Config_to;

#endif  /* #if ( defined(TCPIP_DHCP_CLIENT_ENABLED) && ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON ) )  */

#endif /* RBA_ETHDHCP_TYPES_H */

