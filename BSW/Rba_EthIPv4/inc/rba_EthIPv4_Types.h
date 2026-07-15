

#ifndef RBA_ETHIPV4_TYPES_H
#define RBA_ETHIPV4_TYPES_H

#if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) )

/* This structure holds information about amount of buffer available per controller */
typedef struct
{
        uint32 FreeBufPos_u32;          /* Index pointing to the available buffer   */
        uint32 FreeBufLength_u32;       /* Number of bytes available from the index */
} rba_EthIPv4_FragBufTbl_tst;

/* This structure contains information about reassembly resources */
typedef struct{
    uint16 * const ReassemblyBuf_cpu16;                  /* Pointer to reassembly buffer                                                                              */
    TcpIp_ProtocolType ProtoType_en;                     /* TP layer protocol of the received fragment                                                                */
    uint32 SrcIp_u32;                                    /* Source IP of the received fragment                                                                        */
    uint32 DstIp_u32;                                    /* Destination IP of the received fragment                                                                   */
    uint32 TimerCurrentValue_u32;                        /* Pending time out value for the current reassembly buffer in ticks (1 tick = TcpIp main function period)   */
    #if ( TCPIP_ICMP_ENABLED == STD_ON )
    uint8 IcmpTimeExceededPayload_au8[68];               /* IPv4 header of the first fragment + first eight octets of the IPv4 payload                                */
    #endif /* #if ( TCPIP_ICMP_ENABLED == STD_ON ) */
    uint16 FrameId_u16;                                  /* Frame identifier of the received frame                                                                    */
    uint16 FirstHolePos_u16;                             /* Index of the first free hole in the reassembly buffer                                                     */
    uint16 TotalPalyloadLen_u16;                         /* Total length of IPv4 payload before frame undergoing fragmentation                                        */
}rba_EthIPv4_ReassemblyBufDescriptor_tst;


/* This structure contains IPv4 configuration per Ctrl */
typedef struct
{
#if ( RBA_ETHIPV4_REASSEMBLY_ENABLED == STD_ON )
    rba_EthIPv4_ReassemblyBufDescriptor_tst * const ReassemblyBufDescriptor_cpst; /* Pointer to reassembly buffer descriptor                                               */
    const uint32 ReassemblyTimeout_cu32;       /* Reassembly time out value configured, in ticks (1 tick = TcpIp main function period) */
    const uint16 ReassemblyBufSize_cu16;       /* Reassembly buffer size                                                                */
    const uint8 ReassemblyBufCnt_cu8;         /* Reassembly buffer count                                                               */
#endif

#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
    uint8 * const TxFragBuf_cpu8;         /* Pointer to fragmentation buffer. NULL_PTR indicates fragmentation is disabled for the Ctrl  */
    const uint32 TxFragBufSize_cu32;     /* Size of the fragmentation buffer for the controller. In addition to configured Size it includes additional bytes required to store hidden header  */
    rba_EthIPv4_FragBufTbl_tst * const TxFragBufTbl_cpst;      /* Pointer to buffer handle table. NULL_PTR indicates fragmentation is disabled for the Ctrl   */
    const uint16 TxFragBufTblSize_cu16;  /* Size of table rba_EthIPv4_FragBufTbl_tst. Table size is defined sufficient enough to handle all fragmentation buffer request.*/
#endif

    const uint16 EthIfCtrlMtu_cu16;      /* Specifies the maximum transmission unit (MTU) of the EthIfCtrl in bytes */
    const boolean TtlEnabled_cb;          /* Enable/Disable TTL                                                      */
    const boolean IPv4ChkSumOffloadingEnabled_cb; /* Indicates whether IPv4 checksum offloading is enabled or not for the TcpIpCtrl. */
    const boolean IcmpChkSumOffloadngEnabled_cb;  /* Indicates whether ICMP checksum offloading is enabled or not for the TcpIpCtrl. */
    const boolean UDPChkSumOffloadingEnabled_cb;  /* Indicates whether UDP checksum offloading is enabled or not for the TcpIpCtrl.  */
}rba_EthIPv4_CtrlCfg_tst;

/* IPv4 configuration structure */
typedef struct
{
    const rba_EthIPv4_CtrlCfg_tst * const *IPv4CtrlCfg_pco;                    /* Pointer to Ctrl configuration structure         */
}rba_EthIPv4_Config_to;

#endif /* #if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) ) */

#endif /* RBA_ETHIPV4_TYPES_H */

