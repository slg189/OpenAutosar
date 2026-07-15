

#ifndef RBA_ETHAUTOIP_TYPES_H
#define RBA_ETHAUTOIP_TYPES_H

#include "EthIf_Types.h"
#include "TcpIp_GeneralTypes.h" /* Required for TcpIp_LocalAddrIdType */

#if ( defined(TCPIP_AUTOIP_ENABLED) && ( TCPIP_AUTOIP_ENABLED == STD_ON ) )

/*
****************************************************************************************************
*                                   Define
****************************************************************************************************
*/


/*
****************************************************************************************************
*                                   Structures
****************************************************************************************************
*/

/* AutoIP entries state */
typedef enum
{
    RBA_ETHAUTOIP_STATE_OFF  = 0,
    RBA_ETHAUTOIP_STATE_INIT,
    RBA_ETHAUTOIP_STATE_PROBING,
    RBA_ETHAUTOIP_STATE_ANNOUNCING,
    RBA_ETHAUTOIP_STATE_BOUND,
    RBA_ETHAUTOIP_STATE_STOP
} rba_EthAutoIp_AutoIpState_ten;

/* AutoIP TABLE */
typedef struct
{
    uint8                           srcHwAddr_au8[ETHIF_HWADDR_LEN];    /* MAC addr : used to calculate random value (AutoIP addr / Random Time) */
    rba_EthAutoIp_AutoIpState_ten   entryState_en;                  /* current AutoIP state machine state  */
    TcpIp_IpAddrStateType           autoIpState_en;                 /* Assigned / unassigned */
    uint32                          localLinkIpAddr_u32;            /* the currently selected, probed, announced or used LL IP-Address */
    uint32                          lastConflict_u32;               /* ticks until a conflict can be solved by defending */
    uint32                          timeToWait_u32;                 /* ticks to wait, tick is AUTOIP_TMR_INTERVAL long */
    uint32                          initTimeOut_u32;                /* */
    uint32                          announceWait_u32;       /* Delay before announcing locally configured IP address */
    uint32                          announceInterval_u32;   /* Time between announcement messages */
    uint32                          probeWait_u32;          /* Time before first probe message afterlink became active (initial delay).*/
    uint32                          probeMin_u32;           /* Minimum time between ARP probe messages */
    uint32                          probeMax_u32;           /* Maximum time between ARP probe messages */
    uint8                           probeNum_u8;                /* Number of probe messages. */
    uint8                           announceNum_u8;         /* Number of announcement messages */
    uint8                           triedLocalLinkIpAddr_u8;        /* total number of probed/used Link Local IP-Addresses */
    uint8                           sentArpFrameCnt_u8;             /* sent number of ARP probes or announces, dependent on state */
    boolean                         baseIpStatus_b;             /* Base IP address status to start the IP address assignment provided by upper layer*/
} rba_EthAutoIP_Table_tst;

typedef struct
{
    rba_EthAutoIP_Table_tst         * autoIpTable_pst;          /* AutoIp Table */
    TcpIp_IpAddrAssignmentType      assignmentMethod_en;    /* Assignment Method (AUTO_IP or AUTO_IP_DOIP) */
    TcpIp_LocalAddrIdType           localAddrId_tu8;        /* LocalAddrId assigned to the AutoIp process */
    uint8                           index_u8;               /* Index of the AutoIp process */
    uint8                           ctrlIdx_u8;         /* CtrlIdx matched to the LocalAddrId */
}rba_EthAutoIp_SubConfig_tst;

typedef struct
{
    const rba_EthAutoIp_SubConfig_tst   * subConfig_cst;
    uint32                              initTimeout_u32;            /* The time in TickTime Auto-IP waits at startup, before beginning with ARP probing.*/
    uint8                               procConfigured_u8;      /* Number of AutoIp process configured */
} rba_EthAutoIp_Config_to;

/****************************************************************************************************************
**                                   Global Function Prototypes                                                **
****************************************************************************************************************/

#endif  /* #if ( defined(TCPIP_AUTOIP_ENABLED) && ( TCPIP_AUTOIP_ENABLED == STD_ON ) ) */
#endif /* RBA_ETHAUTOIP_TYPES_H */

