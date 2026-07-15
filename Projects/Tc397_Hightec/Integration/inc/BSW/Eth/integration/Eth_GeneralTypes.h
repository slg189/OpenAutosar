/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



#ifndef ETH_GENERAL_TYPES_H
#define ETH_GENERAL_TYPES_H

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "ComStack_Types.h"


/*
 ***************************************************************************************************
 * Type Definitions
 ***************************************************************************************************
 */

/* -------------------------------------------- */
/* Enumerations                                 */
/* -------------------------------------------- */

/* Eth_ReturnType : Ethernet Driver specific return type */
typedef enum
{
    ETH_OK                              = 0U,   /* Success */
    ETH_E_NOT_OK                        = 1U,   /* General failure */
    ETH_E_NO_ACCESS                     = 3U    /* Ethernet hardware access failure */
} Eth_ReturnType;


/* Eth_ModeType: This type defines the controller modes */
typedef enum
{
    ETH_MODE_DOWN                       = 0U,   /* Controller disabled */
    ETH_MODE_ACTIVE                     = 1U    /* Controller enabled */
} Eth_ModeType;

/* Eth_StateType: Status supervision used for Development Error Detection. The state shall be available for debugging */
typedef enum
{
    ETH_STATE_UNINIT                    = 0U,   /* Driver is not yet configured */
    ETH_STATE_INIT                      = 1U,   /* Driver is configured */
    ETH_STATE_ACTIVE                    = 2U    /* Driver is active */
} Eth_StateType;

/* Eth_FrameType: This type defines the Ethernet frame type used in the Ethernet frame header */
typedef uint16 Eth_FrameType;

/* Eth_DataType: This type defines the Ethernet data type used for data transmission. Its definition depends on the used CPU */
typedef uint8 Eth_DataType;

/* This type defines the Ethernet Buffer index */
typedef uint32 Eth_BufIdxType;

typedef Eth_DataType * Eth_DataRefType_t;

/* Eth_RxStatusType : Used as out parameter in Eth_Receive() indicates whether a frame has been received and if so,
 * whether more frames are available or frames got lost. */
typedef enum
{
    ETH_RECEIVED                        = 0U,   /* Ethernet frame has been received, no further frames available */
    ETH_NOT_RECEIVED                    = 1U,   /* Ethernet frame has not been received, no further frames available */
    ETH_RECEIVED_MORE_DATA_AVAILABLE    = 2U,   /* Ethernet frame has been received, more frames are available */
    ETH_RECEIVED_FRAMES_LOST            = 3U    /* Ethernet frame has been received, some frames got lost */
} Eth_RxStatusType;

/* Eth_FilterActionType: The Enumeration Type Eth_FilterActionType describes the action to be taklen for the MAC address given in *PhysAddrPtr.*/
typedef enum
{
    ADD_TO_FILTER                       = 0U,   /* Add the MAC address to the filter, meaning allow reception */
    ETH_ADD_TO_FILTER                   = 0U,   /* Add the MAC address to the filter, meaning allow reception */
    REMOVE_FROM_FILTER                  = 1U,   /* Remove the MAC address from the filter, meaning reception is blocked in the lower layer */
    ETH_REMOVE_FROM_FILTER              = 1U    /* Remove the MAC address from the filter, meaning reception is blocked in the lower layer */
} Eth_FilterActionType;

/* Eth_TimeStampQualType: Depending on the HW, quality information regarding the evaluated time stamp might be supported */
/* If not supported, the value shall always be valid. For Uncertain and Invalid values, the upper layer shall discard the time stamp */
typedef enum
{
    ETH_VALID                           = 0U,
    ETH_INVALID                         = 1U,
    ETH_UNCERTAIN                       = 2U
} Eth_TimeStampQualType;

/* EthTrcv_BaudRateType: This type defines the Ethernet baud rate. The baud rate gets either negotiated between the connected transceivers or has to be configured. */
typedef enum
{
    ETHTRCV_BAUD_RATE_10MBIT            = 0U,   /* 10MBIT Ethnernet connection */
    ETHTRCV_BAUD_RATE_100MBIT           = 1U,   /* 100MBit Ethernet connection */
    ETHTRCV_BAUD_RATE_1000MBIT          = 2U,   /* 1000MBit Ethernet connection */
    ETHTRCV_BAUD_RATE_UNDEF             = 3U    /* Undef */
}EthTrcv_BaudRateType;

/* EthTrcv_CableDiagResultType: Describes the results of the cable diagnostics */
typedef enum
{
    ETHTRCV_CABLEDIAG_OK                = 0U,   /* Cable diagnostic ok */
    ETHTRCV_CABLEDIAG_ERROR             = 1U,   /* Cable diagnostic failed */
    ETHTRCV_CABLEDIAG_SHORT             = 2U,   /* Short circuit detected */
    ETHTRCV_CABLEDIAG_OPEN              = 3U,   /* Open circuit detected */
    ETHTRCV_CABLEDIAG_PENDING           = 4U,   /* Cable Test is still going on */
    ETHTRCV_CABLEDIAG_WRONG_POLARITY    = 5U    /* Wrong polarity detected */
}EthTrcv_CableDiagResultType;

/* EthTrcv_DuplexModeType: This type defines the Ethernet duplex mode. The duplex mode gets either negotiated between the connected transceivers or has to be configured. */
typedef enum
{
    ETHTRCV_DUPLEX_MODE_HALF            = 0U,   /* Half duplex Ethernet connection */
    ETHTRCV_DUPLEX_MODE_FULL            = 1U,   /* Full duplex Ethernet connection */
    ETHTRCV_DUPLEX_MODE_UNDEF           = 2U    /* Undef */
}EthTrcv_DuplexModeType;

/* EthTrcv_LinkStateType: This type defines the Ethernet link state. The link state changes after an Ethernet cable gets plugged in and the transceivers on both ends negotiated the transmission parameters (i.e. baud rate and duplex mode) */
typedef enum
{
    ETHTRCV_LINK_STATE_DOWN             = 0U,   /* No physical Ethernet connection established */
    ETHTRCV_LINK_STATE_ACTIVE           = 1U,   /* Physical Ethernet connection established */
    ETHTRCV_LINK_STATE_UNDEF            = 2U    /* Undefined */
}EthTrcv_LinkStateType;

/* EthTrcv_ModeType: This type defines the transceiver modes */
typedef enum
{
    ETHTRCV_MODE_DOWN                   = 0U,   /* Transceiver disabled */
    ETHTRCV_MODE_ACTIVE                 = 1U,   /* Transceiver enabled */
    ETHTRCV_MODE_SLEEP                  = 2U    /* Transceiver in sleep mode */
}EthTrcv_ModeType;

/* EthTrcv_StateType: Status supervision used for Development Error Detection. The state shall be available for debugging. */
typedef enum
{
    ETHTRCV_STATE_UNINIT                = 0U,   /* Driver is not yet configured */
    ETHTRCV_STATE_INIT                  = 1U,   /* Driver is configured */
    ETHTRCV_STATE_ACTIVE                = 2U    /* Driver is active */
} EthTrcv_StateType;

/* EthTrcv_WakeupModeType: This type controls the transceiver wake up modes and/or clears the wake-up reason. */
typedef enum
{
    ETHTRCV_WUM_DISABLE                 = 0U,   /* Transceiver wake up disabled */
    ETHTRCV_WUM_ENABLE                  = 1U,   /* Transceiver wake up enabled */
    ETHTRCV_WUM_CLEAR                   = 2U    /* Transceiver wake up reason cleared */
}EthTrcv_WakeupModeType;

/* EthTrcv_WakeupReasonType: This type defines the transceiver wake up reasons. */
typedef enum
{
    ETHTRCV_WUR_NONE                    = 0U,   /* No wake up reason detected. */
    ETHTRCV_WUR_GENERAL                 = 1U,   /* General wake up detected, no distinct reason supported by hardware */
    ETHTRCV_WUR_BUS                     = 2U,   /* Bus wake up detected. Available if supported by hardware */
    ETHTRCV_WUR_INTERNAL                = 3U,   /* Internal wake up detected. Available if supported by hardware*/
    ETHTRCV_WUR_RESET                   = 4U,   /* Reset wake up detected. Available if supported by hardware*/
    ETHTRCV_WUR_POWER_ON                = 5U,   /* Power on wake up detected. Available if supported by hardware*/
    ETHTRCV_WUR_PIN                     = 6U,   /* Pin wake up detected. Available if supported by hardware*/
    ETHTRCV_WUR_SYSERR                  = 7U    /* System error wake up detected. Available if supported by hardware*/
}EthTrcv_WakeupReasonType;

/* EthTrcv_PhyLoopbackModeType: Describes the possible PHY loopback modes */
typedef enum
{
    ETHTRCV_PHYLOOPBACK_NONE            = 0U,   /* Normal operation */
    ETHTRCV_PHYLOOPBACK_INTERNAL        = 1U,   /* Internal loopback */
    ETHTRCV_PHYLOOPBACK_EXTERNAL        = 2U,   /* External loopback */
    ETHTRCV_PHYLOOPBACK_REMOTE          = 3U    /* Remote loopback */
}EthTrcv_PhyLoopbackModeType;

/* EthTrcv_PhyTxModeType: Describes the possible PHY transmit modes */
typedef enum
{
    ETHTRCV_PHYTXMODE_NORMAL            = 0U,   /* Normal operation */
    ETHTRCV_PHYTXMODE_TX_OFF            = 1U,   /* Transmitter disabled */
    ETHTRCV_PHYTXMODE_SCRAMBLER_OFF     = 2U    /* Scrambler disabled */
}EthTrcv_PhyTxModeType;

/* EthTrcv_PhyTestModeType: Describes the possible PHY test modes */
typedef enum
{
    ETHTRCV_PHYTESTMODE_NONE            = 0U,   /* Normal operation */
    ETHTRCV_PHYTESTMODE_1               = 1U,   /* Test transmitter droop */
    ETHTRCV_PHYTESTMODE_2               = 2U,   /* Test master timing jitter */
    ETHTRCV_PHYTESTMODE_3               = 3U,   /* Test slave timing jitter */
    ETHTRCV_PHYTESTMODE_4               = 4U,   /* Test transmitter distortion */
    ETHTRCV_PHYTESTMODE_5               = 5U,   /* Test power spectral density (PSD) mask */
    /*HW specific applicable for RTL9010*/
    ETHTRCV_PHYTESTMODE_6               = 6U,   /* Transmission of continuous pattern */
    ETHTRCV_PHYTESTMODE_7               = 7U    /* Measurement of Bi Error Ration */


}EthTrcv_PhyTestModeType;


/* -------------------------------------------- */
/* Structures                                   */
/* -------------------------------------------- */

/* Eth_MacVlanType: Variables of this type are used to express ARL table entry of the switch (this structure consist of the MAC-address, VLAN-ID and port). It is used in out argument of the EthSwt_GetArlTable() API. */
typedef struct
{
    uint8 MacAddr[6];                      /* Specifies the MAC address */
    uint16 VlanId;                         /* Specifies the VLAN address */
    uint8 SwitchPort;                      /* Port of the switch */
    uint16 HashValue;                      /* Hash value that is generated by the Ethernet switch according the given MacAddr */
}Eth_MacVlanType;

/* Eth_TimeStampType: Variables of this type are used for expressing time stamps including relative time and absolute calendar time. The absolute time starts at 1970-01-01. */
typedef struct
{
    uint32 nanoseconds;                    /* Nanoseconds part of the time */
    uint32 seconds;                        /* 32 bit LSB of the 48 bits Seconds part of the time */
    uint16 secondsHi;                      /* 16 bit MSB of the 48 bits Seconds part of the time */
} Eth_TimeStampType;

/* Eth_TimeIntDiffType: Variables of this type are used to express time differences. */
typedef struct
{
    Eth_TimeStampType diff;                 /* Time difference */
    boolean sign;                           /* Positive (True) / negative (False) time */
}Eth_TimeIntDiffType;

/* Eth_RateRatioType: Variables of this type are used to express frequency ratios. */
typedef struct
{
    Eth_TimeIntDiffType IngressTimeStampDelta;          /* IngressTimeStampSync2 - IngressTimeStampSync1 */
    Eth_TimeIntDiffType OriginTimeStampDelta;           /* OriginTimeStampSync2[FUP2] - OriginTimeStampSync1[FUP1] */
}Eth_RateRatioType;

/* Eth_CounterType: Management structure for Eth Counter Type used in static counter for diagnostic */
typedef struct
{
     uint32 DropPktBufOverrun;              /* Dropped packets due to buffer overrun */
     uint32 DropPktCrc;                     /* Dropped packets due to CRC errors */
     uint32 UndersizePkt;                   /* Number of undersize packets which were less than 64 octets long and were otherwise well formed. (see IETF RFC 1757) */
     uint32 OversizePkt;                    /* Number of oversize packets which are longer than 1518 octets and were otherwise well formed. (see IETF RFC 1757) */
     uint32 AlgnmtErr;                      /* Number of alignment errors, i.e. packets which are received and are not an integral number of octets in length and */
                                            /* do not pass the CRC. */
     uint32 SqeTestErr;                     /* SQE test error according to IETF RFC1643 dot3StatsSQETestErrors */
     uint32 DiscInbdPkt;                    /* The number of inbound packets which were chosen to be discarded even though no errors had been detected */
                                            /* to prevent their being deliverable to a higher-layer protocol. */
     uint32 ErrInbdPkt;                     /* Total number of erroneous inbound packets */
     uint32 DiscOtbdPkt;                    /* The number of outbound packets which were chosen to be discarded even though no errors had been detected to prevent */
                                            /* their being transmitted. */
     uint32 ErrOtbdPkt;                     /* Total number of erroneous outbound packets */
     uint32 SnglCollPkt;                    /* Single collision frames: A count of successfully transmitted frames on a particular interface for which transmission is */
                                            /* inhibited by exactly one collision. (see IETF RFC1643 dot3StatsSingleCollisionFrames) */
     uint32 MultCollPkt;                    /* Multiple collision frames: A count of successfully transmitted frames on a particular interface for which transmission is */
                                            /* inhibited by more than one collision. (see IETF RFC1643 dot3StatsMultipleCollisionFrames) */
     uint32 DfrdPkt;                        /* Number of deferred transmission: A count of frames for which the first transmission attempt on a particular interface is */
                                            /* delayed because the medium is busy. (see IETF RFC1643 dot3StatsDeferredTransmissions) */
     uint32 LatCollPkt;                     /* Number of late collisions: The number of times that a collision is detected on a particular interface later than 512 bit-times */
                                            /* into the transmission of a packet. (see IETF RFC1643 dot3StatsLateCollisions) */
     uint32 HwDepCtr0;                      /* Hardware dependent counter value */
     uint32 HwDepCtr1;                      /* Hardware dependent counter value */
     uint32 HwDepCtr2;                      /* Hardware dependent counter value */
     uint32 HwDepCtr3;                      /* Hardware dependent counter value */
}Eth_CounterType;

/* Eth_RxStatsType: Management structure for Eth_RxStats Type used in static counter for diagnstic for Rx Frame */
typedef struct
{
     uint64 RxStatsOctets;                  /* The total number of octets of data (including those in bad packets) received on the network */
                                            /* (excluding framing bits but including FCS octets). Also described in IETF RFC 2819 MIB etherStatsOctets. */
     uint32 RxStatsDropEvents;              /* The total number of events in which packets were dropped by the probe due to lack of resources. */
                                            /* Also described in IETF RFC 2819 MIB etherStatsDropEvents. */
     uint32 RxStatsPkts;                    /* The total number of packets (including bad packets, broadcast packets, and multicast packets) received. */
                                            /* Also described in IETF RFC 2819 MIB etherStatsPkts */
     uint32 RxStatsBroadcastPkts;           /* The total number of good packets received that were directed to the broadcast address. */
                                            /* Also described in IETF RFC 2819 MIB etherStatsBroadcastPkts */
     uint32 RxStatsMulticastPkts;           /* The total number of good packets received that were directed to a multicast address. */
                                            /* Also described in IETF RFC 2819 MIB etherStatsMulticastPkts. */
     uint32 RxStatsCrcAlignErrors;          /* The total number of packets received that had a length of bertween 64 and 1518 octets that had either a bad Frame Check Sequence (FCS) */
                                            /* with an integral number of octets (FCS Error) or a bad FCS with a non-integral number of octets (Alignment Error). Also described in IETF RFC 2819 MIB etherStatsCRCAlignErrors */
     uint32 RxStatsUndersizePkts;           /* The total number of packets received that were less than 64 octets long (excluding framing bits, but including FCS octets) */
                                            /* and were otherwise well formed. Also described in IETF RFC 2819 MIB etherStatsUndersizePkts. */
     uint32 RxStatsOversizePkts;            /* The total number of packets received that were longer than 1518 octets (excluding framing bits, but including FCS octets) */
                                            /* and were otherwise well formed. Also described in IETF RFC 2819 MIB etherStatsOversizePkts */
     uint32 RxStatsFragments;               /* The total number of packets received that were less than 64 octets in length (excluding framing bits but including FCS octets) and had either a bad Frame Check Sequence (FCS) */
                                            /* with an integral number of octets (FCS Error) or a bad FCS with a non-integral number of octets (Alignment Error). Also described in IETF RFC 2819 MIB etherStatsFragments. */
     uint32 RxStatsJabbers;                 /* The total number of packets received that were longer than 1518 octets, and had either a bad Frame Check Sequence (FCS) with an integral number of octets (FCS Error) or a bad FCS with a non-integral number of octets (Alignment Error). Also described in IETF RFC 2819 MIB etherStatsJabbers. */
     uint32 RxStatsCollisions;              /* The best estimate of the total number of collisions on this Ethernet segment. Also described in IETF RFC 2819 MIB etherStatsCollisions */
     uint32 RxStatsPkts64Octets;            /* The total number of packets (including bad packets) received that were 64 octets in length. Also described in IETF RFC 2819 MIB etherStatsPkts64Octets */
     uint32 RxStatsPkts65to127Octets;       /* The total number of packets (including bad packets) received that were between 65 and 127 octets in length. Also described in IETF RFC 2819 MIB etherStatsPkts65to127Octets */
     uint32 RxStatsPkts128to255Octets;      /* The total number of packets (including bad packets) received that were between 128 and 255 octets in length. Also described in IETF RFC 2819 MIB etherStatsPkts128to255Octets */
     uint32 RxStatsPkts256to511Octets;      /* The total number of packets (including bad packets) received that were between 256 and 511 octets in length. Also described in IETF RFC 2819 MIB etherStatsPkts256to511Octets */
     uint32 RxStatsPkts512to1023Octets;     /* The total number of packets (including bad packets) received that were between 512 and 1023 octets in length. Also described in IETF RFC 2819 MIB etherStatsPkts512to1023Octets */
     uint32 RxStatsPkts1024to1518Octets;    /* The total number of packets (including bad packets) received that were between 1024 and 1518 octets in length. Also described in IETF RFC 2819 MIB etherStatsPkts1024to1518Octets */
     uint32 RxUnicastFrames;                /* The number of subnetwork-unicast packets delivered to a higher-layer protocol. Also described in IETF RFC1213 MIB ifInUcastPkts */
}Eth_RxStatsType;

/* Eth_TxStatsType: Management structure for Eth_TxStats Type used in static counter for diagnstic for Tx Frame */
typedef struct
{
     uint64 TxNumberOfOctets;               /* The total number of octets transmitted out of the interface, including framing characters. Also described in IETF RFC1213 MIB ifOutOctets. */
     uint32 TxNUcastPkts;                   /* The total number of packets that higher-level protocols requested be transmitted to a non-unicast (i.e., a subnetwork-broadcast or subnetwork-multicast) address, */
                                            /* including those that were discarded or not sent. Also described in IETF RFC1213 MIB ifOutNUcastPkts */
     uint32 TxUniCastPkts;                  /* The total number of packets that higher-level protocols requested be transmitted to a subnetwork-unicast address, */
                                            /* including those that were discarded or not sent. Also described in IETF RFC1213 MIB ifOutUcastPkts. */
}Eth_TxStatsType;

/* Eth_TxErrorCounterValuesType: Management structure for Eth_TxErrorCounterValuesType used in static counter for diagnstic for Tx Frame */
typedef struct
{
     uint32 TxDroppedNoErrorPkts;           /* The number of outbound packets which were chosen to be discarded even though no errors had been detected to prevent their being transmitted. */
                                            /* One possible reason for discarding such a packet could be to free up buffer space. Also described in IETF RFC1213 MIB ifOutDiscards */
     uint32 TxDroppedErrorPkts;             /* The number of outbound packets which were chosen to be discarded because of errors. Also described in IETF RFC1213 MIB ifOutErrors */
     uint32 TxDeferredTrans;                /* A count of frames for which the first transmission attempt on a particular interface is delayed because the medium is busy. */
                                            /* The count represented by an instance of this object does not include frames involved in collisions. Also described in IETF RFC1643 MIB dot3StatsDeferredTransmissions */
     uint32 TxSingleCollision;              /* A count of successfully transmitted frames on a particular interface for which transmission is inhibited by exactly one collision. */
                                            /* A frame that is counted by an instance of this object is also counted by the corresponding instance of either the TxUniCastPkts and TxNUcastPkts and is not counted by the corresponding instance of the TxMultipleCollision object. Also described in IETF RFC1643 MIB dot3StatsSingleCollisionFrames */
     uint32 TxMultipleCollision;            /* A count of successfully transmitted frames on a particular interface for which transmission is inhibited by more than one collision. */
                                            /* A frame that is counted by an instance of this object is also counted by the corresponding instance of either the TxUniCastPkts and TxNUcastPkts and is not counted by the corresponding instance of the TxSingleCollision object. Also described in IETF RFC1643 MIB dot3StatsMultipleCollisionFrames. */
     uint32 TxLateCollision;                /* The number of times that a collision is detected on a particular interface later than 512 bit-times into the transmission of a packet. */
                                            /* Five hundred and twelve bit-times corresponds to 51.2 microseconds on a 10 Mbit/s system. A (late) collision included in a count represented by an instance of this object is also considered as a (generic) collision for purposes of other collision-related statistics. Also described in IETF RFC1643 MIB dot3StatsLateCollisions */
     uint32 TxExcessiveCollision;           /* A count of frames for which transmission on a particular interface fails due to excessive collisions. Also described in IETF RFC1643 MIB dot3StatsExcessiveCollisions */
}Eth_TxErrorCounterValuesType;


/* -------------------------------------- */
/* Zero Copy types                        */
/* -------------------------------------- */

/* Eth_ZeroCopyTxHeaderInfo: Structure used to store the Zero Copy Tx header information which is required by EthDriver to transmit the frame */
typedef struct
{
    uint16 HeaderId_u16;                    /* Id of the header in TcpIp context */
    uint8 * HeaderPartBuffer_pu8;           /* Pointer to the header buffer */
    uint8 HeaderPartLen_u8;                 /* Length of the header buffer */
}Eth_ZeroCopyTxHeaderInfo_tst;

/* Eth_ZeroCopyTxDataInfo: Structure used to store the Zero Copy Tx data provided by the Upper Layer application */
typedef struct
{
    uint8 ** DataBufferList_ppu8;           /* Pointer to the list of data buffers */
    uint16 DataBufferCount_u16;             /* Number of data buffers */
    uint16 DataLen_u16;                     /* Length of the data buffers */
}Eth_ZeroCopyTxDataInfo_tst;

/* Zero Copy - ZeroCopy structure containing all required information to execute a ZeroCopy transmission. This structure is passed from one layer to another and contains the allocated header pointers and the application data pointer. */
typedef struct
{
    Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_ast;      /* List of allocated TcpIp header blocks */
    uint8 AllocatedHeaderCount_u8;                                    /* Number of allocated TcpIp header blocks */
    const Eth_ZeroCopyTxDataInfo_tst * ZeroCopyData_pcst;             /* ZeroCopy data to be transmitted given by UL */
    boolean TxConfFlag_b;                                             /* Boolean indicating if TxConfirmation need to be monitored */
    boolean TriggerTx_b;                                              /* Boolean indicating if previously saved and current transmission requests need to be processed by the driver (until this flag is set to TRUE, the driver will hold the requests) */
} Eth_ZeroCopyArg_tst;

#endif /* ETH_GENERAL_TYPES_H */
