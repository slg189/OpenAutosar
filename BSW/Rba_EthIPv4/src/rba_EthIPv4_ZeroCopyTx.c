

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) )

#include "rba_EthIPv4.h"

#include "EthIf.h"
#if (!defined(ETHIF_AR_RELEASE_MAJOR_VERSION) || (ETHIF_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "EthIf AUTOSAR major version undefined or mismatched - EthIf and TcpIp"
#endif
#if (!defined(ETHIF_AR_RELEASE_MINOR_VERSION) || (ETHIF_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "EthIf AUTOSAR minor version undefined or mismatched - EthIf and TcpIp"
#endif

#if ( TCPIP_ARP_ENABLED == STD_ON )
#include "rba_EthArp.h"
#endif

#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp.h"
#endif

#include "TcpIp_Prv.h"
#include "TcpIp_Lib.h"
#include "rba_EthIPv4_Prv.h"

/* File is compiled only if ZeroCopy feature is enabled */
#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )

/*
 ***************************************************************************************************
 * Global Variables
 ***************************************************************************************************
 */

/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHIPV4_START_SEC_CODE_FAST
#include "rba_EthIPv4_MemMap.h"

/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

static Std_ReturnType rba_EthIPv4_Prv_ZeroCopyTransmit_DetChecks
                                                          ( uint8 LocalAddrId_u8,
                                                            const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                            const TcpIp_LocalAddrConfig_to ** LocalAddrConfig_ppcst );

static Std_ReturnType rba_EthIPv4_Prv_ZeroCopyFillHeader_DetChecks
                                                        ( uint8 LocalAddrId_u8,
                                                          const TcpIp_IpHeader_tst * IpHeader_pcst,
                                                          const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                          const TcpIp_LocalAddrConfig_to ** LocalAddrConfig_ppcst );

/********************************************************************************************************************/
/* rba_EthIPv4_ZeroCopyTransmit()  This API executes the ZeroCopy transmission                                      */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId                              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8           LocalAddrId on which data is to be transmitted.                                         */
/* UdpDataLength_u16        UDP data length to be transmitted.                                                      */
/* ZeroCopyArg_pcst         ZeroCopy argument containing the allocated headers and the pointer to the UL data to be */
/*                          transmitted.                                                                            */
/*                                                                                                                  */
/* Output parameters:                                                                                               */
/* TxId_pu32            Unique identifier of the ZeroCopy transmission                                              */
/*                      (valid only if ZeroCopyArg_pst->TxConfFlag_b is TRUE)                                       */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* TcpIp_ReturnType      TCPIP_OK                  : IPv4 packet transmission successful                            */
/*                       TCPIP_E_NOT_OK            : IPv4 packet transmission failed (general failure)              */
/*                       TCPIP_E_ARP_CACHE_MISS    : IPv4 packet transmission failed because of ARP cache miss      */
/********************************************************************************************************************/
TcpIp_ReturnType rba_EthIPv4_ZeroCopyTransmit
                                            ( uint8 LocalAddrId_u8,
                                              uint16 UdpDataLength_u16,
                                              const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                              uint32 * TxId_pu32 )
{
    /* Declare local variables */
    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;
    Std_ReturnType                   lRetValStdType_en;
    TcpIp_ReturnType                 lTcpIpRetVal_en;
    Eth_ZeroCopyArg_tst              lZeroCopyArg_st;

#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    const                           rba_EthIPv4_CtrlCfg_tst * lCtrlCfg_pst;
    uint8                           lNumOfIPFrags_u8;
    uint8                           lloopIdx_u8;
    uint16                          lFragPayloadSize8_u16;
    uint16                          lFragPayLoadSize_u16;
    uint16                          lFragOffsetbytes_u16;
    uint16                          lFragOffset_u16;
    uint16                          lTotalLen_u16;
    uint16                          lFragFlagOffset_u16;
    uint16                          lFragID_u16;
    uint8                           lAllocatedHeaderCnt_u8;
    Eth_ZeroCopyTxHeaderInfo_tst    lZeroCopyAllocatedHeaders_st[3U];
    Eth_ZeroCopyTxDataInfo_tst      lZeroCopyTxDataInfo_st;
    Eth_ZeroCopyTxHeaderInfo_tst    lZeroCopyContinuousHeaders_ast[4U];  /* Worst case : Reuse(Ethernet,Ip) + OneTime(Ip) + Reuse(Ip,Udp) + OneTime(Udp) */
    uint8                           lContinuousHeaderCount_u8;
    uint8*                          lDataBufferList_pu8[1];

    /* Initialize local variables. */
    lFragOffset_u16 = 0;
    lFragFlagOffset_u16 = 0;
    lNumOfIPFrags_u8 = 0;
#endif

    lTcpIpRetVal_en  = TCPIP_E_NOT_OK;

    /* Initialize lZeroCopyArg_st structure members. */
    /* If the TxConfFlag is TRUE, it shall be set to TRUE only for the last fragment. */
    /* Trigger transmit flag shall be set to the last fragment. */
    /* Initialize to FALSE. */
    lZeroCopyArg_st.TxConfFlag_b =   FALSE;
    lZeroCopyArg_st.TriggerTx_b  =   FALSE;

    /* Check whether any development errors are present. */
    /* Fetch the pointer to localAddrId configuration structure, if no DETs are present. */
    lRetValStdType_en = rba_EthIPv4_Prv_ZeroCopyTransmit_DetChecks ( LocalAddrId_u8,
                                                                     ZeroCopyArg_pcst,
                                                                     &lLocalAddrConfig_pcst );

    if( lRetValStdType_en != E_NOT_OK )
    {
        /* If frames are allowed to be fragmented, then the frame is fragmented and transmitted if required. */
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
        if( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 1U )
        {
            /* Initialize data buffer count to 1, as for zero copy transmission with Fragmentation only one buffer is supported. */
            lZeroCopyTxDataInfo_st.DataBufferCount_u16 = 1;

            /* Initialize the pointer to requested controller configuration */
            lCtrlCfg_pst = rba_EthIPv4_CtrlCfg_pcpcst[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] ;

            /* Store the Reuse header buffer pointer and buffer length in local zero copy structure with offset RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET (0). */
            lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8 = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8;

            lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET].HeaderPartLen_u8    = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET].HeaderPartLen_u8;

            /* Get the number of fragments that will be required for the transmission of payload data. */
            /* If fragmentation is not required, then number of fragments will be returned as 1. */
            lRetValStdType_en = rba_EthIPv4_CheckFragmentation( LocalAddrId_u8,
                                                                UdpDataLength_u16,
                                                                &lNumOfIPFrags_u8 );

            if(lRetValStdType_en != E_NOT_OK )
            {
                /* Report DET if lNumOfIPFrags_u8 is not equal to the allocated one time IP headers. ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8-2, two headers are allocated for reuse header and one time UDP header) */
                RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( lNumOfIPFrags_u8 != ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 - 2U )), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG, TCPIP_E_NOT_OK )

                /* Initialize allocated header count to 3 ( one for reuse header, one for IP one time header, one for UDP one time header( only valid for first IPv4 fragment). */
                lAllocatedHeaderCnt_u8 = 3;

                /* Store the UDP one time header buffer pointer and buffer length in local zero copy structure with offset RBA_ETHIPV4_ZEROCOPY_UDPONETIMEHDR_BUFFER_OFFSET (2). */
                /* The UDP one time header buffer will be present after the IP one time headers ( Reuse header at 0th position, IP headers from 1 to lNumOfIPFrags_u8 and UDP header at lNumOfIPFrags_u8+1th position. */
                lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_UDPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8 = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[lNumOfIPFrags_u8+1].HeaderPartBuffer_pu8;

                lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_UDPONETIMEHDR_BUFFER_OFFSET].HeaderPartLen_u8   = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[lNumOfIPFrags_u8+1].HeaderPartLen_u8;

                /* Store the incremented fragmentation ID into local variable. */
                ++rba_EthIPv4_FragID_u16;

                lFragID_u16 = rba_EthIPv4_FragID_u16;

                /* lFragPayloadSize8_u16 contains number of 8 byte blocks and ensures that all fragments except last one contains upper layer payload in terms of 8 byte block. */
                lFragPayloadSize8_u16 = (uint16)( (lCtrlCfg_pst->EthIfCtrlMtu_cu16 - RBA_ETHIPV4_HDRLENGTH)/8u );

                /* Actual size of a fragment in terms of number of bytes. */
                lFragPayLoadSize_u16 = (lFragPayloadSize8_u16 * 8u);

                /* Initialize return value to E_OK. If a transmission of a IPv4 fragment fails, the return value will be set to E_NOT_OK. */
                lRetValStdType_en = E_OK;

                /* Loop for all the fragments and call EthIf_ZeroCopyTransmit() (Is called only once if no fragmentation is required). */
                for( lloopIdx_u8 = 0; (( lloopIdx_u8 <lNumOfIPFrags_u8) && (lRetValStdType_en == E_OK)); lloopIdx_u8++ )
                {
                    /* Report DET if HeaderPartBuffer_pu8 for IPv4 OneTime is NULL */
                    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[lloopIdx_u8+1].HeaderPartBuffer_pu8 ), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHIPV4_E_NULL_PTR, TCPIP_E_NOT_OK )

                    /* Report DET: The length of the IPv4 OneTime header should be equal to RBA_ETHIPV4_ONETIME_HEADER_LENGTH. */
                    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[lloopIdx_u8+1].HeaderPartLen_u8 != RBA_ETHIPV4_ONETIME_HEADER_LENGTH ), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG, TCPIP_E_NOT_OK )

                    /* Store the IP one time header buffer pointer and buffer length in local zero copy structure with offset RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET (1). */
                    /* Depending on which fragment is being transmitted, the buffer pointer being stored will change. */
                    lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8 = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[lloopIdx_u8+1].HeaderPartBuffer_pu8;

                    lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartLen_u8 = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[lloopIdx_u8+1].HeaderPartLen_u8;

                    /* -------------------------------------- */
                    /* Fill one time header                   */
                    /* -------------------------------------- */
                    /* The IP one time header consists of the following fields. */
                    /* 1. Total length - Present at offset RBA_ETHIPV4_ONETIME_TOTLENGTH_OFFSET (0) */
                    /* 2. Fragmentation ID - Present at offset RBA_ETHIPV4_ONETIME_IPV4FRAGID_OFFSET(2). */
                    /* 3. Fragmentation flags and fragment offset - Present at offset RBA_ETHIPV4_ONETIME_FRAGFIELD_OFFSET(4) (Shall be set to 0 if no fragmentation occurs). */

                    /* Check if fragmentation is required. */
                    if( lNumOfIPFrags_u8 > 1 )
                    {
                        /* Clear all flags and fragmentation offset, which shall be set for the particular IPv4 fragment. */
                        lFragFlagOffset_u16 = 0U;

                        lFragOffset_u16 = lloopIdx_u8 * lFragPayloadSize8_u16;     /* Calculate fragmentation offset.  */

                        lFragOffsetbytes_u16 = (lFragOffset_u16 * 8u );            /* Calulate fragmentation offest in bytes. */

                        lDataBufferList_pu8[0] =  ZeroCopyArg_pcst->ZeroCopyData_pcst->DataBufferList_ppu8[0] + lFragOffsetbytes_u16;     /* Update the application data pointer. */

                        lZeroCopyTxDataInfo_st.DataBufferList_ppu8 = &lDataBufferList_pu8[0];

                        /* From the second fragment onwards, the DataPtr should be reduced by RBA_ETHUDP_HDRLENGTH .*/
                        if( lloopIdx_u8 != 0U )
                        {
                            lZeroCopyTxDataInfo_st.DataBufferList_ppu8[0] = lZeroCopyTxDataInfo_st.DataBufferList_ppu8[0] - RBA_ETHUDP_HDRLENGTH;
                        }

                        if(lloopIdx_u8 == (lNumOfIPFrags_u8 - 1) )           /* Check whether the current fragment is the last fragment to be transmitted.       */
                        {                                                           /* If so buffer size shall be set to length of the remaining message + IPv4 header */
                            lTotalLen_u16 = (UdpDataLength_u16 - lFragOffsetbytes_u16) + RBA_ETHIPV4_HDRLENGTH;

                            /* Fill the Length in the header. */
                            /* 1. Total length - Present at offset RBA_ETHIPV4_ONETIME_TOTLENGTH_OFFSET (0) */
                            TcpIp_WriteU16( (&(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ONETIME_TOTLENGTH_OFFSET])),
                                              lTotalLen_u16 );

                           lZeroCopyTxDataInfo_st.DataLen_u16 = (UdpDataLength_u16 - lFragOffsetbytes_u16);

                            /* Store the actual Tx confirmation flag for the last fragment, all other fragments shall have the flag as FALSE. */
                            lZeroCopyArg_st.TxConfFlag_b = ZeroCopyArg_pcst->TxConfFlag_b;

                            /* Set the trigger transmit bit to TRUE for the last fragment. */
                            lZeroCopyArg_st.TriggerTx_b = TRUE;
                        }
                        else
                        {
                            /* If the fragment is not the last fragment buffer length shall be set to payload per fragment as calculated + IPv4 header length  */
                            /* Fill the Length in the header. */
                            /* 1. Total length - Present at offset RBA_ETHIPV4_ONETIME_TOTLENGTH_OFFSET (0) */
                            TcpIp_WriteU16( (&(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ONETIME_TOTLENGTH_OFFSET])),
                                            (lFragPayLoadSize_u16 + RBA_ETHIPV4_HDRLENGTH) );

                            lFragFlagOffset_u16 = ((uint16)1u) << RBA_ETHIPV4_MORE_FRAGMENT_FLAG_POS;       /* More fragment flag shall be set.  */

                            lZeroCopyTxDataInfo_st.DataLen_u16 = lFragPayLoadSize_u16;
                        }

                        /* Combine fragmentation flag and fragmentation offset into a single word */
                        lFragFlagOffset_u16 = lFragFlagOffset_u16 | lFragOffset_u16;

                        /* Update the fragmentation ID field in the IP header. */
                        /* 2. Fragmentation ID - Present at offset RBA_ETHIPV4_ONETIME_IPV4FRAGID_OFFSET(2). */
                        TcpIp_WriteU16( (&(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ONETIME_IPV4FRAGID_OFFSET])),
                                        lFragID_u16 );

                        /* Update the fragmentation offset field in the IP header */
                        /* 3. Fragmentation flags and fragment offset - Present at offset RBA_ETHIPV4_ONETIME_FRAGFIELD_OFFSET(4). */
                        TcpIp_WriteU16( (&(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ONETIME_FRAGFIELD_OFFSET])),
                                        lFragFlagOffset_u16 );

                        /* If it is not the first fragment, then UDP header should not be present- Allocated Header count shall be 2. */
                        /* UDP one time header is present as the last allocated header in lZeroCopyAllocatedHeaders_st structure. */
                        if( lloopIdx_u8 != 0 )
                        {
                            /* Udp header is not present in the frame, therefore the Udp OneTime header need not to be passed to EthIf layer */
                            lAllocatedHeaderCnt_u8 = 2;

                            /* Also the Reuse header (which contains UDP src and dest port) need to be decremented by 4 bytes as the UDP header is not present in the frame */
                            lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET].HeaderPartLen_u8 = (36U-4U);
                        }
                        else
                        {
                            /* If it is the first fragment, then UDP header exists as payload of IPv4, hence application data length shall be decremented by RBA_ETHUDP_HDRLENGTH. */
                            lZeroCopyTxDataInfo_st.DataLen_u16 = lZeroCopyTxDataInfo_st.DataLen_u16 - RBA_ETHUDP_HDRLENGTH;
                        }
                    }
                    else  /* Fragmentation is not required, hence EthIf_ZeroCopyTransmit is called only once. */
                    {
                        lTotalLen_u16 = UdpDataLength_u16 + RBA_ETHIPV4_HDRLENGTH;

                        /* Fill the Length in the header. */
                        /* 1. Total length - Present at offset RBA_ETHIPV4_ONETIME_TOTLENGTH_OFFSET (0) */
                        TcpIp_WriteU16( (&(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ONETIME_TOTLENGTH_OFFSET])),
                                         lTotalLen_u16 );

                        /* Update the fragmentation ID field in the IP header. */
                        /* 2. Fragmentation ID - Present at offset RBA_ETHIPV4_ONETIME_IPV4FRAGID_OFFSET(2). */
                        TcpIp_WriteU16( (&(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ONETIME_IPV4FRAGID_OFFSET])),
                                        lFragID_u16 );

                        /* Update the fragmentation offset field in the IP header */
                        /* 3. Fragmentation flags and fragment offset - Present at offset RBA_ETHIPV4_ONETIME_FRAGFIELD_OFFSET(4) (Shall be set to 0 if no fragmentation occurs). */
                        TcpIp_WriteU16( (&(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ONETIME_FRAGFIELD_OFFSET])),
                                        0U );

                        /* Set the application DataPtr same as the passed input parameter. */
                        lDataBufferList_pu8[0]  = ZeroCopyArg_pcst->ZeroCopyData_pcst->DataBufferList_ppu8[0];

                        lZeroCopyTxDataInfo_st.DataBufferList_ppu8 = &lDataBufferList_pu8[0];

                        /* Set the application data length same as the passed input parameter DataLength_u16. */
                        lZeroCopyTxDataInfo_st.DataLen_u16 = ZeroCopyArg_pcst->ZeroCopyData_pcst->DataLen_u16;

                        /* Store the Tx confirmation flag. */
                        lZeroCopyArg_st.TxConfFlag_b = ZeroCopyArg_pcst->TxConfFlag_b;

                        /* Set trigger transmit flag to TRUE. */
                        lZeroCopyArg_st.TriggerTx_b  = TRUE;
                    }

                    /* -------------------------------------- */
                    /* Construct Zero Copy argument           */
                    /* -------------------------------------- */
                    /* Construction of first part : Reuse(Ethernet,Ip) + OneTime(Ip) */

                     /* Reuse(Ethernet,Ip) */
                     lZeroCopyContinuousHeaders_ast[0U].HeaderPartBuffer_pu8 = &(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_ETHERNET_OFFSET]);
                     lZeroCopyContinuousHeaders_ast[0U].HeaderPartLen_u8     = (RBA_ETHIPV4_ZEROCOPY_REUSEHDR_ETHERNET_LEN + RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_FIRST_LEN);

                     /* OneTime(Ip) */
                     lZeroCopyContinuousHeaders_ast[1U].HeaderPartBuffer_pu8 = lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8;    /* Header block 1 is for OneTime IP */
                     lZeroCopyContinuousHeaders_ast[1U].HeaderPartLen_u8     = lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_IPONETIMEHDR_BUFFER_OFFSET].HeaderPartLen_u8;

                     /* If there is no IPv4 fragmentation or we are in the first IPv4 fragment */
                     /* 3 header buffers are given in argument (ReuseHeader, IpOneTimeHeader, UdpOneTimeHeader) */
                     if( lAllocatedHeaderCnt_u8 == 3U )
                     {
                         /* Construction of second part : Reuse(Ip,Udp) + OneTime(Udp) */

                         /* Reuse(Ip,Udp) */
                         lZeroCopyContinuousHeaders_ast[2U].HeaderPartBuffer_pu8 = &(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_LAST_OFFSET]);
                         lZeroCopyContinuousHeaders_ast[2U].HeaderPartLen_u8     = (RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_LAST_LEN + RBA_ETHIPV4_ZEROCOPY_REUSEHDR_UDP_LEN);

                         /* OneTime(Udp) */
                         lZeroCopyContinuousHeaders_ast[3U].HeaderPartBuffer_pu8 = lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_UDPONETIMEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8;    /* Header block 2 is for OneTime Udp */
                         lZeroCopyContinuousHeaders_ast[3U].HeaderPartLen_u8     = lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_UDPONETIMEHDR_BUFFER_OFFSET].HeaderPartLen_u8;

                         /* In total 4 headers pointers are passed to EthDriver */
                         lContinuousHeaderCount_u8 = 4U;
                     }
                         /* If we are in case of subsequent IPv4 fragment */
                         /* 2 header buffers are given in argument (ReuseHeader, IpOneTimeHeader) */
                     else
                     {
                         /* Construction of second part : Reuse(Ip) */

                         /* Reuse(Ip) */
                         lZeroCopyContinuousHeaders_ast[2U].HeaderPartBuffer_pu8 = &(lZeroCopyAllocatedHeaders_st[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_BUFFER_OFFSET].HeaderPartBuffer_pu8[RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_LAST_OFFSET]);
                         lZeroCopyContinuousHeaders_ast[2U].HeaderPartLen_u8     = (RBA_ETHIPV4_ZEROCOPY_REUSEHDR_IPV4_LAST_LEN);

                         /* In total 3 headers pointers are passed to EthDriver */
                         lContinuousHeaderCount_u8 = 3U;
                     }

                     /* -------------------------------------- */
                     /* Call lower layer                       */
                     /* -------------------------------------- */

                     /* Store the updated buffer list and buffer count for the frame/fragment. */
                     lZeroCopyArg_st.ZeroCopyAllocatedHeaders_ast = lZeroCopyContinuousHeaders_ast;
                     lZeroCopyArg_st.AllocatedHeaderCount_u8 = lContinuousHeaderCount_u8;
                     lZeroCopyArg_st.ZeroCopyData_pcst = &lZeroCopyTxDataInfo_st;

                    /* Trigger EthIf_ZeroCopyTransmit. */
                    lRetValStdType_en = EthIf_ZeroCopyTransmit(  lLocalAddrConfig_pcst->EthIfCtrlRef_u8,
                                                                 &lZeroCopyArg_st,
                                                                 TxId_pu32 );
                }

                /* Return TCPIP_OK if the transmit request/s processed successfully */
                if(  E_OK == lRetValStdType_en )
                {
                    lTcpIpRetVal_en = TCPIP_OK;
                }
            }
        }
        else
#endif
        {
            /* If frames are not allowed to be fragmented, trigger EthIf_ZeroCopyTransmit() with the passed input parameters. */
#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
            /* Store the buffer list and buffer count. */
            lZeroCopyArg_st.ZeroCopyAllocatedHeaders_ast = ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast;
            lZeroCopyArg_st.AllocatedHeaderCount_u8      = ZeroCopyArg_pcst->AllocatedHeaderCount_u8;
            lZeroCopyArg_st.ZeroCopyData_pcst            = ZeroCopyArg_pcst->ZeroCopyData_pcst;

            /* Store the Tx confirmation flag. */
            lZeroCopyArg_st.TxConfFlag_b = ZeroCopyArg_pcst->TxConfFlag_b;

            /* Set trigger transmit flag to TRUE. */
            lZeroCopyArg_st.TriggerTx_b  = TRUE;

            /* Trigger EthIf_ZeroCopyTransmit. */
            lRetValStdType_en = EthIf_ZeroCopyTransmit(  lLocalAddrConfig_pcst->EthIfCtrlRef_u8,
                                                         &lZeroCopyArg_st,
                                                         TxId_pu32 );

            /* Return TCPIP_OK if the transmit request/s processed successfully */
            if(  E_OK == lRetValStdType_en )
            {
                lTcpIpRetVal_en = TCPIP_OK;
            }
#endif
        }
    }

#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_OFF)

    /* UdpDataLength_u16 is used only in dynamic mode, hence void cast is necessary if dynamic mode is disabled. */
    (void)UdpDataLength_u16;

#endif

    return lTcpIpRetVal_en;
}

/********************************************************************************************************************/
/* rba_EthIPv4_Prv_ZeroCopyTransmit_DetChecks()  This API checks the development errors (DETs) for                  */
/*                                               rba_EthIPv4_ZeroCopyTransmit() API.                                */
/*                                                                                                                  */
/* Synchronous, Reentrant for different SocketIds. Non reentrant for the same SocketId                              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8           LocalAddrId on which data is to be transmitted.                                         */
/* ZeroCopyArg_pcst         ZeroCopy argument containing the allocated headers and the pointer to the UL data to be */
/*                          transmitted.                                                                            */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* LocalAddrConfig_ppcst - Pointer to the localAddress config structure for the passed LocalAddrId.                 */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : No development error detected.                                                      */
/*                     - E_NOT_OK : Development error detected.                                                     */
/*                                                                                                                  */
/********************************************************************************************************************/
static Std_ReturnType rba_EthIPv4_Prv_ZeroCopyTransmit_DetChecks
                                                          ( uint8 LocalAddrId_u8,
                                                            const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                            const TcpIp_LocalAddrConfig_to ** LocalAddrConfig_ppcst )
{
    /* Declare local variables */
    Std_ReturnType                   lRetValStdType_en;
    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;

    /* Check for DET errors */
    /* Report DET if rba_EthIPv4 module is uninitialised */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthIPv4_CtrlCfg_pcpcst ), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID,      \
                                            RBA_ETHIPV4_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    /* LocalAddrId_u8 is considered as invalid if it exceeds number of configured local address and also if it's not an IPv4 localAddress */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),      \
                                             RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Report DET if ZeroCopyArg_pcst is NULL_PTR. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst ), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID,      \
                                      RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET if ZeroCopyAllocatedHeaders_ast is NULL_PTR. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast ), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID,      \
                                          RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET: At least 1 header block should be passed by TcpIp. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 < 1U ), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Report DET if ZeroCopyData_pcst is NULL_PTR. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyData_pcst ), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID,      \
                                           RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET if DataBufferList_ppu8 is NULL */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyData_pcst->DataBufferList_ppu8 ), RBA_ETHIPV4_E_ZEROCOPYTRANSMIT_API_ID, RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Get localAddressId properties from TcpIp configuration */
    lRetValStdType_en = TcpIp_GetLocalAddrIdProperties( LocalAddrId_u8 ,
                                                        LocalAddrConfig_ppcst );

    return lRetValStdType_en;
}

/********************************************************************************************************************/
/* rba_EthIPv4_ZeroCopyFillHeader()  This API prefill the reusable part of the IPv4 header                          */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8           LocalAddrId on which data is to be transmitted                                          */
/* Priority_u8              Priority value which shall be used for the 3-bit PCP field of the VLAN tag              */
/* DestIpAddr_u32           Destination IP address to which the frame has to be transmitted.                        */
/* IpHeader_pcst            Data structure containing the fields of the IP header.                                  */
/* UdpDataLength_u16        Udp data length to be transmitted.                                                      */
/*                                                                                                                  */
/* Output parameters:                                                                                               */
/* ZeroCopyArg_pcst         ZeroCopy argument containing the allocated headers                                      */
/*                          and the pointer to the UL data to be transmitted                                        */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* TcpIp_ReturnType      TCPIP_OK                  : IPv4 filling successful                                        */
/*                       TCPIP_E_NOT_OK            : IPv4 filling failed (general failure)                          */
/*                       TCPIP_E_ARP_CACHE_MISS    : IPv4 filling failed because of ARP cache miss)                 */
/********************************************************************************************************************/
TcpIp_ReturnType rba_EthIPv4_ZeroCopyFillHeader
                                            ( uint8 LocalAddrId_u8,
                                              uint8 Priority_u8,
                                              uint32 DestIpAddr_u32,
                                              const TcpIp_IpHeader_tst * IpHeader_pcst,
                                              uint16 UdpDataLength_u16,
                                              const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst )
{

    /* Declare local variables */
    uint16                           lVerHdrLenToS_u16;
    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;
    Std_ReturnType                   lRetValStdType_en;
    TcpIp_ReturnType                 lTcpIpRetVal_en;
    uint32                           lSrcIPAddr_u32;
    uint8                            lDestEthAddr_au8[6] = {0,0,0,0,0,0};

#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
    uint16 lFragFlagOffset_u16 = 0U;
#endif

    /* Initialize local variables. */
    lTcpIpRetVal_en       = TCPIP_E_NOT_OK;

    /* Check whether any development errors are present. */
    /* Fetch the pointers to localAddrId configuration structure, if no DETs are present. */
    lRetValStdType_en = rba_EthIPv4_Prv_ZeroCopyFillHeader_DetChecks( LocalAddrId_u8,
                                                                      IpHeader_pcst,
                                                                      ZeroCopyArg_pcst,
                                                                      &lLocalAddrConfig_pcst );

    if( lRetValStdType_en != E_NOT_OK )
    {
        /* Trigger API to fetch the destination MAC address for the DestIpAddr_u32. */
        lTcpIpRetVal_en = rba_EthIPv4_GetDestEthAddr( lLocalAddrConfig_pcst,
                                                      DestIpAddr_u32,
                                                      &lDestEthAddr_au8[0] );

        if( lTcpIpRetVal_en == TCPIP_OK )
        {
            /* Update IP version number, IPv4 header length and Type of Service field values */
            lVerHdrLenToS_u16   = ( (uint16)( (uint16)( RBA_ETHIPV4_IPV4_VERSION_NUM ) << 12u )  |
                                    (uint16)( (uint16)( RBA_ETHIPV4_HDRLENGTH / 4u ) << 8u )     |
                                    (uint16)( (uint16)( IpHeader_pcst->DiffServiceCodepoint_u8 ) << 2u ) );

            /* If frames are allowed to be fragmented, then the corresponding IPv4 header fields in the reuse header shall be filled. */
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
            if (ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 1U)
            {
                /* 1. Version, IHL, ToS -  Present at offset RBA_ETHIPV4_REUSE_VERHDRLENTOS_OFFSET. */
                TcpIp_WriteU16( ( &(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_VERHDRLENTOS_OFFSET])),
                                  lVerHdrLenToS_u16 );

                /* 2. Ttl -                Present at offset RBA_ETHIPV4_REUSE_TTL_OFFSET. */
                ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_TTL_OFFSET] = IpHeader_pcst->Ttl_u8;

                /* 3. TP layer Protocol -  Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_TP_PROTOCOL. */
                ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_TP_PROTOCOL_OFFSET] = (uint8)IpHeader_pcst->ProtoType_en;

                /* Currently IP header checksum for zero copy transmission is being done in hardware. Set the values of the checksum to zero. */
                /* 4. Header Checksum   -  Present at offset RBA_ETHIPV4_REUSE_HDRCHECKSUMFIRSTBYTE_OFFSET and +1. */
                ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_HDRCHECKSUMFIRSTBYTE_OFFSET]    = 0U;

                ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_HDRCHECKSUMSECONDBYTE_OFFSET] = 0U;

                /* Set source IP in the frame to assigned local IP  */
                lSrcIPAddr_u32 = lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32;

                /* Update the source IPv4 address into the IPv4 header */
                /* 5. Source IP address -  Present at offset RBA_ETHIPV4_REUSE_SRCIPV4_ADDR_OFFSET */
                TcpIp_WriteU32( ( &(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_SRCIPV4_ADDR_OFFSET]) ),
                                  lSrcIPAddr_u32 );

                /* Update the destination address value in the IPv4 header */
                /* 6. Dest IP address -    Present at offset RBA_ETHIPV4_REUSE_DESTIPV4_ADDR_OFFSET. */
                TcpIp_WriteU32(( &(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_DESTIPV4_ADDR_OFFSET]) ),
                                 DestIpAddr_u32 );
            }
            else
#endif
            {
                /* If frames are not allowed to be fragmented, fill the complete IPv4 header in the allocated reuse header. */
#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)

                /* 1. Version, IHL, ToS -  Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_IPV4VER_HDRLEN_TOS_UB. */
                TcpIp_WriteU16( ( &(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_IPV4VER_HDRLEN_TOS_UB])),
                                 lVerHdrLenToS_u16 );

                /* Fill the Length in the header. */
                /* 2. Total length - Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_TOTLEN_UB. */
                TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_TOTLEN_UB])),
                               ( UdpDataLength_u16 + RBA_ETHIPV4_HDRLENGTH ));

                /* Update the fragmentation ID field in the IP header. */
                /* 3. Fragmentation ID - Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_FRAGID_UB. */
                TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_FRAGID_UB])),
                                RBA_ETHIPV4_REUSE_NON_FRAG_FIXED_ID );

                /* Don't fragment flag shall be set.  */
                lFragFlagOffset_u16 = ((uint16)1u) << RBA_ETHIPV4_DONT_FRAGMENT_FLAG_POS;

                /* Update the fragmentation offset field in the IP header */
                /* 4. Fragmentation flags and fragment offset - Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_FRAGOFFSET_UB. */
                TcpIp_WriteU16( (&(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_FRAGOFFSET_UB])),
                               lFragFlagOffset_u16 );

                /* 5. Ttl -                Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_IPHDRCHECKSUM_UB. */
                ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_TTL_OFFSET] = IpHeader_pcst->Ttl_u8;

                /* 6. TP layer Protocol -  Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_TP_PROTOCOL. */
                ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_TP_PROTOCOL] = (uint8)IpHeader_pcst->ProtoType_en;

                /* Currently IP header checksum for zero copy transmission is being done in hardware. Set the values of the checksum to zero. */
                /* 7. Header Checksum   -  Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_IPHDRCHECKSUM_UB] and +1. */
                ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_IPHDRCHECKSUM_UB]    = 0U;

                ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_IPHDRCHECKSUM_UB+1U] = 0U;

                /* Set source IP in the frame to assigned local IP  */
                lSrcIPAddr_u32 = lLocalAddrConfig_pcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32;

                /* Update the source IPv4 address into the IPv4 header */
                /* 8. Source IP address -  Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_SRCIPV4ADDR_OFFSET */
                TcpIp_WriteU32( ( &(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_SRCIPV4ADDR_OFFSET]) ),
                                 lSrcIPAddr_u32 );

                /* Update the destination address value in the IPv4 header */
                /* 9. Dest IP address -    Present at offset RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_DESTIPV4ADDR_OFFSET. */
                TcpIp_WriteU32(( &(ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8[RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET+RBA_ETHIPV4_DESTIPV4ADDR_OFFSET]) ),
                                DestIpAddr_u32 );

#endif
            }


            /* Call EthIf_ZeroCopyFillHeader() API, to fill the remaining header part. */
            lRetValStdType_en = EthIf_ZeroCopyFillHeader( lLocalAddrConfig_pcst->EthIfCtrlRef_u8,
                                                          ETHIF_FRAMETYP_IPv4,
                                                          Priority_u8,
                                                          &lDestEthAddr_au8[0],
                                                          ZeroCopyArg_pcst );

            /* If return value is E_NOT_OK, set TcpIp return value to TCPIP_E_NOT_OK. */
            if( lRetValStdType_en != E_OK )
            {
                lTcpIpRetVal_en = TCPIP_E_NOT_OK;
            }
        }
    }

#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_OFF)

           /* UdpDataLength_u16 is used only in static mode, hence void cast is necessary if static mode is disabled. */
           (void)UdpDataLength_u16;
#endif

    return lTcpIpRetVal_en;
}

/********************************************************************************************************************/
/* rba_EthIPv4_Prv_ZeroCopyFillHeader_DetChecks() -  This API checks the development errors (DETs) for              */
/*                                                   rba_EthIPv4_ZeroCopyFillHeader() API.                          */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8           LocalAddrId on which data is to be transmitted                                          */
/* IpHeader_pcst            Data structure containing the fields of the IP header.                                  */
/* ZeroCopyArg_pcst         ZeroCopy argument containing the allocated headers                                      */
/*                          and the pointer to the UL data to be transmitted                                        */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* LocalAddrConfig_ppcst - Pointer to the localAddress config structure for the passed LocalAddrId.                 */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : No development error detected.                                                      */
/*                     - E_NOT_OK : Development error detected.                                                     */
/*                                                                                                                  */
/********************************************************************************************************************/
static Std_ReturnType rba_EthIPv4_Prv_ZeroCopyFillHeader_DetChecks
                                                        ( uint8 LocalAddrId_u8,
                                                          const TcpIp_IpHeader_tst * IpHeader_pcst,
                                                          const Eth_ZeroCopyArg_tst * ZeroCopyArg_pcst,
                                                          const TcpIp_LocalAddrConfig_to ** LocalAddrConfig_ppcst )
{
    /* Declare local variables */
    Std_ReturnType                   lRetValStdType_en;

    /* Check for DET errors */
    /* Report DET if rba_EthIPv4 module is uninitialised */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthIPv4_CtrlCfg_pcpcst ), RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID,      \
                                            RBA_ETHIPV4_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    /* LocalAddrId_u8 is considered as invalid if it exceeds number of configured local address and also if it's not an IPv4 localAddress */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),      \
                                              RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Report DET if ZeroCopyArg_pcst is NULL_PTR. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst ), RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID,      \
                                         RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET if ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast is NULL_PTR. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast ), RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID,      \
                                           RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET: At least 1 header block should be passed by TcpIp (Reuse) */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 < 1U ), RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Report DET if HeaderPartBuffer_pu8 for Reuse header is NULL */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartBuffer_pu8 ), RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET: In static mode of zerocopy transmission, the length of the Reuse header must be upto the destination IP address offset length( including the 4 bytes length of the destination IP address). */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 == 1U) && ( ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartLen_u8 <  (RBA_ETHIPV4_REUSE_NON_FRAG_OFFSET + RBA_ETHIPV4_DESTIPV4ADDR_OFFSET + 4U) ) ), \
                                            RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Report DET: In dynamic mode of zerocopy transmission, the length of the Reuse header must be upto the destination IP address offset length added with the dynamic mode reuse header offset.( including the 4 bytes length of the destination IP address). */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( ( ZeroCopyArg_pcst->AllocatedHeaderCount_u8 > 1U) && ( ZeroCopyArg_pcst->ZeroCopyAllocatedHeaders_ast[0U].HeaderPartLen_u8 < (RBA_ETHIPV4_REUSE_DESTIPV4_ADDR_OFFSET + 4U) ) ), \
                                            RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

   /* Report DET if IpHeader_pcst is NULL_PTR. */
   RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == IpHeader_pcst ), RBA_ETHIPV4_E_ZEROCOPYFILLHEADER_API_ID,      \
                                          RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Get localAddressId properties from TcpIp configuration */
    lRetValStdType_en = TcpIp_GetLocalAddrIdProperties( LocalAddrId_u8 ,
                                                        LocalAddrConfig_ppcst );

    return lRetValStdType_en;
}

#define RBA_ETHIPV4_STOP_SEC_CODE_FAST
#include "rba_EthIPv4_MemMap.h"

#endif /* #if ( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON ) */
#endif /* #if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) ) */
