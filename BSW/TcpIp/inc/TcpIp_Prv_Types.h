

#ifndef TCPIP_PRV_TYPES_H
#define TCPIP_PRV_TYPES_H

#include "TcpIp_Cfg.h"        /* Required for TCPIP_CONFIGURED */

#ifdef TCPIP_CONFIGURED

/*
***************************************************************************************************
* Private Typedefs
***************************************************************************************************
*/

/* TcpIp specific return type for indicating the validity of requested static IP address and its related parameters. */
typedef enum
{
    TCPIP_ADDR_E_OK,                /* IP address and related parameters are valid.        */
    TCPIP_ADDR_E_NOT_OK,            /* IP address and related parameters are invalid.      */
    TCPIP_ADDR_ASSIGNED             /* IP address is already assigned to the localAddrId.  */
}TcpIp_AddrReturnType;


/*
***************************************************************************************************
* Private ZeroCopy Typedefs
***************************************************************************************************
*/

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
/* ZeroCopy - State of a ZeroCopy descriptor */
typedef enum
{
    TCPIP_ZEROCOPY_DESC_CLOSED      = 0x00U,    /* The ZeroCopy descriptor is closed */
    TCPIP_ZEROCOPY_DESC_ALLOCATED   = 0x01U,    /* The ZeroCopy descriptor is allocated by a socket and can be used to execute ZeroCopy transmissions */
    TCPIP_ZEROCOPY_DESC_REQ_CLOSE   = 0x02U     /* The ZeroCopy descriptor is requested to be closed but still in use for ongoing transmission. The closing is delayed to next MainFunction. */
}TcpIp_ZeroCopyDescriptorState_ten;

/* Zero Copy - Reuse header structure */
typedef struct
{
#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
    uint8                           HeaderPart_au8[46U];            /* Reusable header block (46 bytes) */
    uint16                          LinkedLength_u16;               /* Indicates the length that is linked with the current reuse header */
#else
    uint8                           HeaderPart_au8[36U];            /* Reusable header block (36 bytes) */
#endif
    uint16                          ReuseCntr_u16;                  /* Indicates how many transmit requests uses this reuse buffer block (it is decremented on header release) */
    boolean                         Lock_b;                         /* Boolean indicating if the header part is currently locked (in use by EthDriver) */
    boolean                         InvalidFlag_b;                  /* Boolean indicating if the Reuse header is invalid (because of connection changed) */
} TcpIp_ReuseHeader_tst;

#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
/* Zero Copy - IP One Time header structure */
typedef struct
{
    uint8                           HeaderPart_au8[6U];             /* One time header block (6 bytes) */
    boolean                         Lock_b;                         /* Boolean indicating if the header part is currently locked (in use by EthDriver) */
} TcpIp_IpOneTimeHeader_tst;

/* Zero Copy - UDP One Time header structure */
typedef struct
{
    uint8                           HeaderPart_au8[4U];             /* One time header block (4 bytes) */
    boolean                         Lock_b;                         /* Boolean indicating if the header part is currently locked (in use by EthDriver) */
} TcpIp_UdpOneTimeHeader_tst;

#endif

/* Zero Copy - TxIdElement structure */
typedef struct
{
    uint32                          TxId_u32;                       /* TxId that will be monitored for TxConfirmation */
    uint16                          ReuseHeaderCntrMap_au16[TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF];        /* ReuseCntr associated with each Reuse header for this particular TxId */
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    uint16                          EndIpOneTimeHeaderIdx_u16;      /* Last Ip one time header index which is part of the Tx block to be polled for TxConfirmation */
    uint16                          EndUdpOneTimeHeaderIdx_u16;     /* Last Udp one time header index which is part of the Tx block to be polled for TxConfirmation */
#endif
} TcpIp_TxIdElement_tst;

/* Zero Copy - ZeroCopy descriptor used to store the header buffers */
typedef struct
{
    TcpIp_ZeroCopyDescriptorState_ten DescState_en;                                                 /* ZeroCopy descriptor state */
    TcpIp_SocketIdType              LinkedSocket_u16;                                               /* Index of the socket which own the descriptor */

    TcpIp_LocalAddrIdType           LocalAddrId_u8;                                                 /* LocalAddrId on which this ZeroCopy descriptor is used */
    uint8                           UL_SockOwnerIdx_u8;                                             /* Upper Layer socket owner which will be used to give ZeroCopyTxConfirmation */

    TcpIp_ReuseHeader_tst           ReuseHeaderList_ast[TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF];         /* List of Reuse header */
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    TcpIp_IpOneTimeHeader_tst       IpOneTimeHeaderList_ast[TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF*TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX];     /* List of Ip One time header */
    TcpIp_UdpOneTimeHeader_tst      UdpOneTimeHeaderList_ast[TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF];    /* List of Udp One time header */
#endif
    uint16                          CurrentReuseHeaderIdx_u16;                                      /* Last locked reuse header */
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    uint16                          NbFreeIpOneTimeHeader_u16;                                      /* Total number of free Ip one time headers */
    uint16                          UnconfirmedIpOneTimeHeaderIdx_u16;                              /* First locked Ip one time header */
    uint16                          NxtFreeIpOneTimeHeaderIdx_u16;                                  /* Next free Ip one time header */

    uint16                          NbFreeUdpOneTimeHeader_u16;                                     /* Total number of free Udp one time headers */
    uint16                          UnconfirmedUdpOneTimeHeaderIdx_u16;                             /* First locked Udp one time header */
    uint16                          NxtFreeUdpOneTimeHeaderIdx_u16;                                 /* Next free Udp one time header */
#endif
    TcpIp_TxIdElement_tst           TxIdList_ast[TCPIP_ZEROCOPY_MAX_MONITORED_TXCONF];              /* List of monitored TxId for tranmission confirmation */
    uint16                          OldestTxIdIdx_u16;                                              /* Oldest TxId index currently saved in the list of monitored TxId */
    uint16                          CurrentTxIdIdx_u16;                                             /* Current TxId index */

} TcpIp_ZeroCopyDescriptor_tst;
#endif

#endif /* #ifdef TCPIP_CONFIGURED */
#endif /* TCPIP_PRV_TYPES_H */
