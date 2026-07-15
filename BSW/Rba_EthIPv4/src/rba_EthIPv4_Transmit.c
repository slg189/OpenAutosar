
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

/*
 ***************************************************************************************************
 * Global Variables
 ***************************************************************************************************
 */

#define RBA_ETHIPV4_START_SEC_VAR_INIT_16
#include "rba_EthIPv4_MemMap.h"
/*********************************************************************************************************************/
/* The IPv4 header ID of the next outgoing IP packet      */
uint16 rba_EthIPv4_FragID_u16 = 0x1234;
/*********************************************************************************************************************/
#define RBA_ETHIPV4_STOP_SEC_VAR_INIT_16
#include "rba_EthIPv4_MemMap.h"

/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */
#define RBA_ETHIPV4_START_SEC_CODE
#include "rba_EthIPv4_MemMap.h"

#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
static void rba_EthIPv4_FragBufHandler_AllocateBuf(
                                            const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                            uint32 BufLength_u32,
                                            uint8 * * Buffer_ppu8 );

static void rba_EthIPv4_FragBufHandler_ReleaseBuf(
                                            const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                            const uint8 * Buffer_pcu8 );
#endif


#if (RBA_ETHIPV4_UPDATE_TPLAYERCHKSUM_ENABLED)           /* If rba_EthIPv4_UpdateTPLayerChkSum() API is enabled */
static Std_ReturnType rba_EthIPv4_UpdateTPLayerChkSum( const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                                       const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                       uint8 * Buffer_pu8,
                                                       const uint16 TPLayerLength_cu16,
                                                       const boolean IsFragFrame_cb );
#endif

static Std_ReturnType rba_EthIPv4_GetSrcIP( const TcpIp_LocalAddrConfig_to * const LocalAddrConfig_cpcst,
                                            uint16 TotLength_u16,
                                            rba_EthIPv4_HdrFields_tst * IPv4_HdrFields_pst);

static Std_ReturnType rba_EthIPv4_GetTTL( const TcpIp_LocalAddrConfig_to * const LocalAddrConfig_cpcst,
                                          uint16 TotLength_u16,
                                          rba_EthIPv4_HdrFields_tst * IPv4_HdrFields_pst);

static Std_ReturnType rba_EthIPv4_FillHdr( const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                           const rba_EthIPv4_HdrFields_tst * IPv4_HdrFields_pcst,
                                           uint8 * Buffer_pu8 );

static Std_ReturnType rba_EthIPv4_Prv_ProvideTxBuffer_DetChecks(
                                                                    uint8 LocalAddrId_u8,
                                                                    const uint8 * BufIdx_pcu8,
                                                                    uint8 * * Buffer_ppu8,
                                                                    const uint16 * ReqLengthIn_pcu16,
                                                                    const TcpIp_LocalAddrConfig_to ** LocalAddrConfig_ppcst,
                                                                    const rba_EthIPv4_CtrlCfg_tst **  CtrlCfg_ppcst );

static Std_ReturnType rba_EthIPv4_Transmit_Prv_DetChecks(
                                                             uint8 LocalAddrId_u8,
                                                             uint32 DestIpAddr_u32,
                                                             const uint8 * Buffer_pcu8,
                                                             const TcpIp_IpHeader_tst * IpHeader_pcst,
                                                             uint16 TotLength_u16,
                                                             const TcpIp_LocalAddrConfig_to ** LocalAddrConfig_ppcst,
                                                             const rba_EthIPv4_CtrlCfg_tst ** CtrlCfg_ppcst );

/********************************************************************************************************************/
/* rba_EthIPv4_ProvideTxBuffer() - This API provides transmission buffer to the upper layer.                        */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8      - Local address index on which data is to be transmitted.                                   */
/*  DestIpAddr_u32      - Destination IP address to which the data is to be transmitted.                            */
/*  FramePrio_u8        - Priority value which shall be used for the 3-bit PCP field of the VLAN tag                */
/*                                                                                                                  */
/* InOut Parameters :                                                                                               */
/*  ReqLengthIn_pu16    - Length of buffer requested                                                                */
/*                                                                                                                  */
/* Out Parameters :                                                                                                 */
/*  BufIdx_pu8          - Index to the buffer allocated by EthDrv                                                   */
/*  Buffer_ppu8         - Starting address of the buffer                                                            */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  BufReq_ReturnType   - Result of buffer request.                                                                 */
/*                                                                                                                  */
/********************************************************************************************************************/
BufReq_ReturnType rba_EthIPv4_ProvideTxBuffer(
                                                uint8 LocalAddrId_u8,
                                                uint32 DestIpAddr_u32,
                                                uint8 FramePrio_u8,
                                                uint8 * BufIdx_pu8,
                                                uint8 * * Buffer_ppu8,
                                                uint16 * ReqLengthIn_pu16 )
{
    /* Local variable declaration */
    const rba_EthIPv4_CtrlCfg_tst *         lCtrlCfg_pst;
    const TcpIp_LocalAddrConfig_to *        lLocalAddrConfig_pcst;
    BufReq_ReturnType                       lRetValProvideTxBuffer_en;
    Std_ReturnType                          lRetValStdType_en;
    TcpIp_ReturnType                        lRetValTcpIpType_en;
    uint8                                   lDesEthAddr_au8[RBA_ETHIPV4_MACADDR_LEN] = {0,0,0,0,0,0};

    /* Initialise the local variables */
    lRetValProvideTxBuffer_en   = BUFREQ_E_NOT_OK;
    lRetValStdType_en           = E_NOT_OK;
    lRetValTcpIpType_en         = TCPIP_E_NOT_OK;
    lCtrlCfg_pst                = NULL_PTR;

    /* Check whether any development errors are present. */
    /* Fetch the pointers to localAddrId and TcpIpCtrl configuration structures, if no DETs are present. */
    lRetValStdType_en = rba_EthIPv4_Prv_ProvideTxBuffer_DetChecks( LocalAddrId_u8,
                                                                   BufIdx_pu8,
                                                                   Buffer_ppu8,
                                                                   ReqLengthIn_pu16,
                                                                   &lLocalAddrConfig_pcst,
                                                                   &lCtrlCfg_pst );

    /* Check whether the local address id is of type unicast */
    if( ( E_NOT_OK != lRetValStdType_en ) &&
        ( TCPIP_UNICAST == lLocalAddrConfig_pcst->IpAddressType_en ) )
    {
        /* Check whether the EthIf controller is in valid state for communication  */
        /* Communication is allowed  */
        /* 1. If EthIf controller state is ONLINE */
        /* 2. If EthIf controller state is STARTUP or SHUTDOWN and address assignment method is DHCP for the requested localAddrId*/
        lRetValStdType_en = TcpIp_ChkComAllowed( LocalAddrId_u8 );

        /* Check whether the destination IP and destination MAC address are in ARP table */
        if( E_OK == lRetValStdType_en )
        {   /* Check whether the destination IP is valid and if so, get the destination MAC address. */
            lRetValTcpIpType_en = rba_EthIPv4_GetDestEthAddr( lLocalAddrConfig_pcst,
                                                              DestIpAddr_u32,
                                                              lDesEthAddr_au8);
        }
    }

    if (TCPIP_OK == lRetValTcpIpType_en)
    {
        /* Check whether the requested buffer length does not exceed the maximum value of uint16 value reduced by IPv4 header length. */
        /* If it does not exceed, then add the header length and check whether the value is less than or equal to the EthIfMTU size. */
        /* If it is less than or equal to MTU size then fragmentation is not required. If it exceeds MTU size, check for fragmentation support.*/
        if((( (*ReqLengthIn_pu16) <= (uint16)(TCPIP_MAXUINT16 - RBA_ETHIPV4_HDRLENGTH )) &&
           (( (*ReqLengthIn_pu16) + RBA_ETHIPV4_HDRLENGTH ) <= lCtrlCfg_pst->EthIfCtrlMtu_cu16 )))
        {
            /* Increment buffer length by IPv4 header length  */
            (*ReqLengthIn_pu16 ) = ( (*ReqLengthIn_pu16)  + RBA_ETHIPV4_HDRLENGTH );

            /* Call the EthIf_ProvideTxBuffer() to allocate memory for the data to be transmitted */
            lRetValProvideTxBuffer_en = EthIf_ProvideTxBuffer(  lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ,
                                                                ETHIF_FRAMETYP_IPv4 ,
                                                                FramePrio_u8,
                                                                BufIdx_pu8,
                                                                Buffer_ppu8,
                                                                ReqLengthIn_pu16 );

            /* EthIf provided requested buffer length  */
            if( BUFREQ_OK == lRetValProvideTxBuffer_en )
            {
                /* Store the destination MAC in the IPv4 header fields as it is required in transmit path. */
                **Buffer_ppu8           = lDesEthAddr_au8[0];
                *((*Buffer_ppu8) +  1 ) = lDesEthAddr_au8[1];
                *((*Buffer_ppu8) +  2 ) = lDesEthAddr_au8[2];
                *((*Buffer_ppu8) +  3 ) = lDesEthAddr_au8[3];
                *((*Buffer_ppu8) +  4 ) = lDesEthAddr_au8[4];
                *((*Buffer_ppu8) +  5 ) = lDesEthAddr_au8[5];

               /* Increment the buffer pointer and decrement the buffer length by IPv4 header length  */
                ( * Buffer_ppu8 ) = ( ( * Buffer_ppu8) + RBA_ETHIPV4_HDRLENGTH );
                ( *ReqLengthIn_pu16 ) = ( ( *ReqLengthIn_pu16 ) - RBA_ETHIPV4_HDRLENGTH );

            }
            else if( ( BUFREQ_E_OVFL == lRetValProvideTxBuffer_en ) &&             /* If EthIf_ProvideTxBuffer() returned BUFREQ_E_OVFL  */
                    ( (*ReqLengthIn_pu16) > RBA_ETHIPV4_HDRLENGTH ) )             /* Check whether the buffer length which EthIf can provide is at least a byte more than the required for IPv4 header  */
            {
                (*ReqLengthIn_pu16) = (*ReqLengthIn_pu16) - RBA_ETHIPV4_HDRLENGTH;
            }
            else
            {
                (*ReqLengthIn_pu16) = 0;
            }
        }
#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
        else if(NULL_PTR != lCtrlCfg_pst->TxFragBuf_cpu8)   /* Check whether the fragmentation is enabled for the controller */
        {
            /* Provide IPv4 buffer */
            rba_EthIPv4_FragBufHandler_AllocateBuf( lCtrlCfg_pst,
                                                    ((*ReqLengthIn_pu16) + RBA_ETHIPV4_HIDDEN_HDR_LEN),
                                                    Buffer_ppu8 );

            /* Check whether IPv4 could provide requested buffer (If IPV4 has no sufficient buffer rba_EthIPv4_FragBufHandler_AllocateBuf() will return with Buffer_ppu8 set to NULL_PTR.)  */
            if(NULL_PTR != (*Buffer_ppu8))
            {
                lRetValProvideTxBuffer_en = BUFREQ_OK;

                *((*Buffer_ppu8) + RBA_ETHIPV4_HIDDEN_HDR_FRAMEPRIO_OFFSET ) = FramePrio_u8;             /* Store frame priority and destination MAC as hidden header */
                *((*Buffer_ppu8) + RBA_ETHIPV4_HIDDEN_HDR_DSTMAC_OFFSET ) = lDesEthAddr_au8[0];
                *((*Buffer_ppu8) + RBA_ETHIPV4_HIDDEN_HDR_DSTMAC_OFFSET + 1 ) = lDesEthAddr_au8[1];
                *((*Buffer_ppu8) + RBA_ETHIPV4_HIDDEN_HDR_DSTMAC_OFFSET + 2 ) = lDesEthAddr_au8[2];
                *((*Buffer_ppu8) + RBA_ETHIPV4_HIDDEN_HDR_DSTMAC_OFFSET + 3 ) = lDesEthAddr_au8[3];
                *((*Buffer_ppu8) + RBA_ETHIPV4_HIDDEN_HDR_DSTMAC_OFFSET + 4 ) = lDesEthAddr_au8[4];
                *((*Buffer_ppu8) + RBA_ETHIPV4_HIDDEN_HDR_DSTMAC_OFFSET + 5 ) = lDesEthAddr_au8[5];

                /* Increment the buffer pointer by hidden header length  */
                 ( * Buffer_ppu8 ) = ( ( * Buffer_ppu8) + RBA_ETHIPV4_HIDDEN_HDR_LEN );
            }
        }
        else
        { /* Do nothing */
        }
#endif
    }

    return ( lRetValProvideTxBuffer_en );
}


/********************************************************************************************************************/
/* rba_EthIPv4_Prv_ProvideTxBuffer_DetChecks() - This API checks the development errors (DETs) for                  */
/*                                               rba_EthIPv4_ProvideTxBuffer() API.                                 */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8      - Local address index on which data is to be transmitted.                                   */
/*  BufIdx_pcu8          - Index to the buffer allocated by EthDrv                                                  */
/*  Buffer_ppu8         - Starting address of the buffer                                                            */
/*  ReqLengthIn_pcu16    - Length of buffer requested                                                               */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* LocalAddrConfig_ppcst - Pointer to the localAddress config structure for the passed LocalAddrId.                 */
/* CtrlCfg_ppcst          - Pointer to the IPv4 configuration structure corresponding to the requested EthIfCtrl.   */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : No development error detected.                                                      */
/*                     - E_NOT_OK : Development error detected.                                                     */
/*                                                                                                                  */
/********************************************************************************************************************/
/* MR12 RULE 8.13 VIOLATION: Buffer_ppu8 is not modified in the static function. But it is modified in the caller function and hence it is retained as pointer to variable. */
static Std_ReturnType rba_EthIPv4_Prv_ProvideTxBuffer_DetChecks(
                                                                    uint8 LocalAddrId_u8,
                                                                    const uint8 * BufIdx_pcu8,
                                                                    uint8 * * Buffer_ppu8,
                                                                    const uint16 * ReqLengthIn_pcu16,
                                                                    const TcpIp_LocalAddrConfig_to ** LocalAddrConfig_ppcst,
                                                                    const rba_EthIPv4_CtrlCfg_tst **  CtrlCfg_ppcst )
{
    Std_ReturnType                          lRetValStdType_en;
    const TcpIp_LocalAddrConfig_to *        lLocalAddrConfig_pcst;
    const rba_EthIPv4_CtrlCfg_tst  *        lCtrlCfg_pst;

    /* Initialise the local variables. */
    lCtrlCfg_pst                = NULL_PTR;

    /* Check for DET errors */
    /* Report DET if rba_EthIPv4 module is uninitialised */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthIPv4_CtrlCfg_pcpcst ), RBA_ETHIPV4_E_PROVIDE_TXBUFFER_API_ID,     \
                                            RBA_ETHIPV4_E_NOTINIT, E_NOT_OK )

    /* Report DET if input parameter LocalAddrId_u8 is invalid */
    /* LocalAddrId_u8 is considered as invalid if it exceeds number of configured local address and also if it's not an IPv4 localAddress */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),             \
                                            RBA_ETHIPV4_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Get the configuration information associated with the requested localAddressId  */
    lRetValStdType_en = TcpIp_GetLocalAddrIdProperties( LocalAddrId_u8 ,
                                                        LocalAddrConfig_ppcst  );

    if(E_OK == lRetValStdType_en)
    {
        lLocalAddrConfig_pcst = *LocalAddrConfig_ppcst;

        /* Initialize the pointer to requested controller configuration */
        lCtrlCfg_pst = rba_EthIPv4_CtrlCfg_pcpcst[lLocalAddrConfig_pcst->EthIfCtrlRef_u8];
    }

    /* Report DET if local address ID is invalid */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( (NULL_PTR == lCtrlCfg_pst),             \
                                            RBA_ETHIPV4_E_PROVIDE_TXBUFFER_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    *CtrlCfg_ppcst          = lCtrlCfg_pst;

    /* Report DET if BufIdx_pu8 is a Null pointer */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == BufIdx_pcu8 ), RBA_ETHIPV4_E_PROVIDE_TXBUFFER_API_ID,                 \
                                            RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET if Buffer_ppu8 is a Null pointer */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == Buffer_ppu8 ), RBA_ETHIPV4_E_PROVIDE_TXBUFFER_API_ID,                \
                                            RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET if ReqLengthIn_pu16 is a Null pointer */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ReqLengthIn_pcu16 ), RBA_ETHIPV4_E_PROVIDE_TXBUFFER_API_ID,           \
                                            RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* If there was no return from the DET checks, then return E_OK as all DET checks passed. */
    return E_OK;
}

/********************************************************************************************************************/
/* rba_EthIPv4_Transmit() - API transmit an IPv4 packet                                                             */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8      - Local address index on which data is to be transmitted.                                   */
/*  DestIpAddr_u32      - Destination IP address to which the data is to be transmitted.                            */
/*  BufIdx_u8           - Index to the buffer allocated by EthDrv                                                   */
/*  Buffer_pu8          - Starting address of the buffer                                                            */
/*  IpHeader_pcst       - structure contains values required to fill out IPv4 header                                */
/*  TotLength_u16       - Total length of upper layer data                                                          */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : No development error detected.                                                      */
/*                     - E_NOT_OK : Development error detected.                                                     */
/*                                                                                                                  */
/********************************************************************************************************************/
/* HIS METRIC PATH VIOLATION IN rba_EthIPv4_Transmit: The number of path is increased due to the consecutive if blocks which are required for the processing of various data length transmission requests. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
TcpIp_ReturnType rba_EthIPv4_Transmit(
                                         uint8 LocalAddrId_u8,
                                         uint32 DestIpAddr_u32,
                                         uint8 * Buffer_pu8,
                                         uint8 BufIdx_u8,
                                         const TcpIp_IpHeader_tst * IpHeader_pcst,
                                         uint16 TotLength_u16 )
{
    /* Local variable declaration  */
    rba_EthIPv4_HdrFields_tst       lIPv4_HdrFields_tst;
    TcpIp_ReturnType                lRetValTcpIpType_en;
    Std_ReturnType                  lRetValStdType_en;
    uint8                           lDestEthAddr_au8[RBA_ETHIPV4_MACADDR_LEN];
    uint8                           lDestEthAddrOffset_u8;
    const rba_EthIPv4_CtrlCfg_tst * lCtrlCfg_pst;
    uint16                          lLoopIdx_u16;
    uint8 *                         lDrvBufPtr_pu8;
    uint8                           lDrvBufIdx_u8;
    uint16                          lNumberOfFragments_u16;
    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;

#if (RBA_ETHIPV4_UPDATE_TPLAYERCHKSUM_ENABLED)                   /* If rba_EthIPv4_UpdateTPLayerChkSum() API is enabled */
    TcpIp_PseudoHdr_tst     lPseudoHdr_st;
    boolean                 lIsFragBuf_b;
#endif

#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
    uint16      lFragPayloadSize8_u16;
    uint16      lByteCopyLoopIdx_u16;
    uint16      lFragOffset_u16;
    uint8       lFramePrio_u8;
    BufReq_ReturnType      lRetValBufReqType_en;
#endif

    /* Initialise the local variables */
    lRetValTcpIpType_en     = TCPIP_E_NOT_OK;
    lRetValStdType_en       = E_NOT_OK;
    lDestEthAddr_au8[0u]    = 0u;
    lDrvBufIdx_u8           = BufIdx_u8;
    lDrvBufPtr_pu8          = NULL_PTR;
    lLoopIdx_u16            = 0u;
    lCtrlCfg_pst            = NULL_PTR;

    lNumberOfFragments_u16  = 0u;
    lDestEthAddrOffset_u8   = RBA_ETHIPV4_HDRLENGTH;   /* Position of destination MAC stored corresponding to IPv4 payload in case of buffer allocated from driver */

#if (RBA_ETHIPV4_UPDATE_TPLAYERCHKSUM_ENABLED)                   /* If rba_EthIPv4_UpdateTPLayerChkSum() API is enabled */
    lIsFragBuf_b            = FALSE;
#endif

#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
    lFragPayloadSize8_u16   = 0u;
    lFragOffset_u16         = 0u;
    lFramePrio_u8           = 0u;
#endif

    /* Check whether any development errors are present. */
    /* Fetch the pointers to localAddrId and TcpIpCtrl configuration structures, if no DETs are present. */
    lRetValStdType_en = rba_EthIPv4_Transmit_Prv_DetChecks(  LocalAddrId_u8,
                                                             DestIpAddr_u32,
                                                             Buffer_pu8,
                                                             IpHeader_pcst,
                                                             TotLength_u16,
                                                             &lLocalAddrConfig_pcst,
                                                             &lCtrlCfg_pst );

    if( E_NOT_OK != lRetValStdType_en )
    {
        /* Initializing members of IpHeader_pcst which specifies values to be set in IPv4 header */
        lIPv4_HdrFields_tst.ProtoType_en       = IpHeader_pcst->ProtoType_en;
        lIPv4_HdrFields_tst.TTL_u8             = IpHeader_pcst->Ttl_u8;  /* It will be later set to RBA_ETHIPV4_TTL_VALUE if  */
                                                                         /*  if IpHeader_pcst->Ttl_u8 is zero, source/remote IP is AutoIp or if TTL configuration is disabled */
        lIPv4_HdrFields_tst.DstIp_u32          = DestIpAddr_u32;
        lIPv4_HdrFields_tst.SrcIp_u32          = 0uL;    /* Initialize Source IP address to zero to avoid code guide warning. */
        lIPv4_HdrFields_tst.TotalLen_u16       = 0u;
        lIPv4_HdrFields_tst.FragFlagOffset_u16 = 0u;

        rba_EthIPv4_FragID_u16++;
        lIPv4_HdrFields_tst.FragId_u16        = rba_EthIPv4_FragID_u16;
        lIPv4_HdrFields_tst.Sockowner_u8      = IpHeader_pcst->SockOwner_u8;
        lIPv4_HdrFields_tst.DiffServiceCodepoint_u8 = IpHeader_pcst->DiffServiceCodepoint_u8;

#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
        /* Set the flag to indicate whether the API is called with driver buffer or IPv4 fragmentation buffer. */
        if( (NULL_PTR != lCtrlCfg_pst->TxFragBuf_cpu8) &&                                         /* Check if fragmentation is enabled for the controller                     */
            (Buffer_pu8 >= lCtrlCfg_pst->TxFragBuf_cpu8) &&                                       /* Check whether the buffer address is within the fragmentation buffer pool */
            (Buffer_pu8 < (lCtrlCfg_pst->TxFragBuf_cpu8 + lCtrlCfg_pst->TxFragBufSize_cu32) ) )
        {
            lIsFragBuf_b = TRUE;     /* The API is called with IPv4 fragmentation buffer */

            /* Position of destination MAC stored corresponding to IPv4 payload in case of buffer allocated from fragmentation buffer pool */
            lDestEthAddrOffset_u8 = RBA_ETHIPV4_HIDDEN_HDR_LEN - RBA_ETHIPV4_HIDDEN_HDR_DSTMAC_OFFSET;

            /* Extract the frame priority from the hidden header */
            lFramePrio_u8 = *((Buffer_pu8 - RBA_ETHIPV4_HIDDEN_HDR_LEN) + RBA_ETHIPV4_HIDDEN_HDR_FRAMEPRIO_OFFSET);

            /* FragPayloadSize8 contains number of 8 byte blocks and ensures that all fragments except last one contains upper layer payload in terms of 8 byte block. */
            lFragPayloadSize8_u16 = (uint16)( (lCtrlCfg_pst->EthIfCtrlMtu_cu16 - RBA_ETHIPV4_HDRLENGTH)/8u );

            /* ((FragPayloadSize8 * 8) - 1 ) is required in the numerator of the expression to increase the number of fragments by one if the Payload is not an integral multiple of (FragPayloadSize8 * 8). */
            lNumberOfFragments_u16 = ( TotLength_u16 + ((lFragPayloadSize8_u16 * 8u) - 1u) ) / (lFragPayloadSize8_u16 * 8u);
        }
        else /* If the API is called with driver buffer, EthIf_Transmit() shall be called only once even if there is an internal error. */
#endif
        {
            lNumberOfFragments_u16 = 1u;
        }

        /* Get the destination MAC stored in the IPv4 header fields */
        lDestEthAddr_au8[0] = *(Buffer_pu8 - lDestEthAddrOffset_u8);
        lDestEthAddr_au8[1] = *( (Buffer_pu8 - lDestEthAddrOffset_u8) + 1u);
        lDestEthAddr_au8[2] = *( (Buffer_pu8 - lDestEthAddrOffset_u8) + 2u);
        lDestEthAddr_au8[3] = *( (Buffer_pu8 - lDestEthAddrOffset_u8) + 3u);
        lDestEthAddr_au8[4] = *( (Buffer_pu8 - lDestEthAddrOffset_u8) + 4u);
        lDestEthAddr_au8[5] = *( (Buffer_pu8 - lDestEthAddrOffset_u8) + 5u);


        /* Get source IP value to be set in the frame to be transmitted. */
        /* These APIs additionally validate message length and transport layer protocol. */
        lRetValStdType_en = rba_EthIPv4_GetSrcIP( lLocalAddrConfig_pcst,
                                                  TotLength_u16,
                                                  &lIPv4_HdrFields_tst);

		/* Get TTL value to be set in the frame to be transmitted. */
        lRetValStdType_en = (lRetValStdType_en == E_NOT_OK) ? E_NOT_OK : rba_EthIPv4_GetTTL( lLocalAddrConfig_pcst,
                                                                                            TotLength_u16,
                                                                                            &lIPv4_HdrFields_tst);

#if (RBA_ETHIPV4_UPDATE_TPLAYERCHKSUM_ENABLED)  /* If rba_EthIPv4_UpdateTPLayerChkSum() API is enabled */
        if( E_OK == lRetValStdType_en )
        {
            /* Update pseudo header information */
            lPseudoHdr_st.IpDomainType_t = TCPIP_AF_INET;
            lPseudoHdr_st.ProtoType_en = IpHeader_pcst->ProtoType_en;
            lPseudoHdr_st.RemoteIpAddr_un.IPv4Addr_u32 = DestIpAddr_u32;
            lPseudoHdr_st.LocalIpAddr_un.IPv4Addr_u32 = lIPv4_HdrFields_tst.SrcIp_u32;

            /* Update the checksum for transport layer protocol */
            lRetValStdType_en = rba_EthIPv4_UpdateTPLayerChkSum( lCtrlCfg_pst,
                                                                 &lPseudoHdr_st,
                                                                 Buffer_pu8,
                                                                 TotLength_u16,
                                                                 lIsFragBuf_b );
        }
#endif

        /* If there is some internal error and the buffer is from IPv4 buffer pool, there is no need to execute the following loop,   */
        /* as releasing buffer happens outside the loop  */
        /* If there is some internal error and the buffer is from driver buffer pool, the following loop shall be executed once  */
        /* to release the driver buffer, as releasing buffer happens via EthIf_Transmit(length = 0)  */
#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )                  /* If fragmentation is enabled */
        if((E_NOT_OK == lRetValStdType_en) &&
           (FALSE != lIsFragBuf_b))
        {
            lNumberOfFragments_u16 = 0u;
        }
#endif

        /* Call EthIf_Transmit() based on the conditions set above */
        /* MR12 RULE 15.4 VIOLATION: Avoiding multiple break statements will make code unreadable */
        for(lLoopIdx_u16 = 0u; lLoopIdx_u16 < lNumberOfFragments_u16; lLoopIdx_u16++)
        {

#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
            /* If the API is called with IPv4 fragmentation buffer */
            if(FALSE != lIsFragBuf_b)
            {
                lIPv4_HdrFields_tst.FragFlagOffset_u16 = 0u;                /* This value corresponds to byte 6  and 7 of IPv4 header (fragmentation flags and fragmentation offset ) */
                lFragOffset_u16 = lLoopIdx_u16 * lFragPayloadSize8_u16;     /* Calculate fragmentation offset                                                                         */

                if(lLoopIdx_u16 == (lNumberOfFragments_u16 - 1u) )          /* Check whether the current fragment is the last fragment to be transmitted       */
                {                                                           /* If so buffer size shall be set to length of the remaining message + IPv4 header */
                    lIPv4_HdrFields_tst.TotalLen_u16 = (TotLength_u16 - (lFragOffset_u16 * 8u)) + RBA_ETHIPV4_HDRLENGTH;
                }
                else
                {                                                                                            /* If the fragment is not the last fragment buffer length shall be set to */
                    lIPv4_HdrFields_tst.TotalLen_u16 = (lFragPayloadSize8_u16 * 8u) + RBA_ETHIPV4_HDRLENGTH;  /* payload per fragment as calculated + IPv4 header length                */
                    lIPv4_HdrFields_tst.FragFlagOffset_u16 = ((uint16)1u) << RBA_ETHIPV4_MORE_FRAGMENT_FLAG_POS;       /* More fragment flag shall be set                                        */
                }

                /* Combine fragmentation flag and fragmentation offset into a single word */
                lIPv4_HdrFields_tst.FragFlagOffset_u16 = lIPv4_HdrFields_tst.FragFlagOffset_u16 | lFragOffset_u16;

                /* Call the EthIf_ProvideTxBuffer() to allocate memory for the fragment  */
                lRetValBufReqType_en = EthIf_ProvideTxBuffer(  lLocalAddrConfig_pcst->EthIfCtrlRef_u8 ,
                                                               ETHIF_FRAMETYP_IPv4 ,
                                                               lFramePrio_u8,
                                                               &lDrvBufIdx_u8,
                                                               &lDrvBufPtr_pu8,
                                                               &lIPv4_HdrFields_tst.TotalLen_u16 );

                /* If driver successfully provided the buffer copy upper payload into the driver buffer */
                if(BUFREQ_OK == lRetValBufReqType_en)
                {
                    for(lByteCopyLoopIdx_u16 = 0u ; lByteCopyLoopIdx_u16 < (lIPv4_HdrFields_tst.TotalLen_u16 - RBA_ETHIPV4_HDRLENGTH); lByteCopyLoopIdx_u16++)
                    {
                        lDrvBufPtr_pu8[RBA_ETHIPV4_HDRLENGTH + lByteCopyLoopIdx_u16] = Buffer_pu8[(lFragOffset_u16 * 8u ) + lByteCopyLoopIdx_u16 ];   /* IPv4 buffer index shall start from Fragmentation Offset * 8 */
                    }

                    /* Update the transmit buffer with IPv4 header fields already fetched */
                    lRetValStdType_en = rba_EthIPv4_FillHdr( lCtrlCfg_pst,
                                                             &lIPv4_HdrFields_tst,
                                                             lDrvBufPtr_pu8);
                }
                else    /* If the driver failed to provide the requested length abort the transmission */
                {
                    lRetValStdType_en = E_NOT_OK;
                    break;
                }
            }
            else
#endif
            {
                /* Decrement the buffer pointer and increment the buffer length by IPv4 header length */
                lDrvBufPtr_pu8 = Buffer_pu8 - RBA_ETHIPV4_HDRLENGTH;
                lDrvBufIdx_u8  = BufIdx_u8;

                /* If there are no internal error or rba_EthIPv4_Transmit() is not called with zero length, */
                /* set the message length to requested length + IPv4 header length and update buffer for IPv4 header fields */
                if( E_OK == lRetValStdType_en )
                {
                    lIPv4_HdrFields_tst.TotalLen_u16 = TotLength_u16 + RBA_ETHIPV4_HDRLENGTH;

                    /* Update the transmit buffer with IPv4 header fields already fetched */
                    lRetValStdType_en = rba_EthIPv4_FillHdr( lCtrlCfg_pst,
                                                             &lIPv4_HdrFields_tst,
                                                             lDrvBufPtr_pu8 );
                }
            }

            /* If rba_EthIPv4_FillHdr() returns E_NOT_OK release the buffer by calling EthIf_Transmit() with zero length. */
            if(E_NOT_OK == lRetValStdType_en)
            {
                lIPv4_HdrFields_tst.TotalLen_u16 = 0u;
            }

            /* Call the EthIf function to transmit the IPv4 packet */
            lRetValStdType_en = EthIf_Transmit( lLocalAddrConfig_pcst->EthIfCtrlRef_u8,             /* EthIf controller index */
                                                lDrvBufIdx_u8,                /* Buffer index provided by Eth_TxBuffer module */
                                                ETHIF_FRAMETYP_IPv4,          /* IPv4 frame type */
                                                FALSE,                        /* TxConfirmation - FALSE - Not required - SWS_TCPIP_00131 */
                                                lIPv4_HdrFields_tst.TotalLen_u16,               /* Tx data length - IPv4 payload including IPv4 header */
                                                ( &(lDestEthAddr_au8[0u]) ) ); /* Destination MAC address */

            /* If rba_EthIPv4_FillHdr() returns E_NOT_OK abort the remaining transmissions */
            if( (0u == lIPv4_HdrFields_tst.TotalLen_u16) || ( lRetValStdType_en != E_OK ))
            {
                break;
            }

        }

#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
        /* Allocated IPv4 fragmentation buffer shall be released */
        if(FALSE != lIsFragBuf_b)
        {
            /*Move the buffer pointer to start of hidden header */
            Buffer_pu8 -= RBA_ETHIPV4_HIDDEN_HDR_LEN;

            rba_EthIPv4_FragBufHandler_ReleaseBuf( lCtrlCfg_pst,
                                                   Buffer_pu8 );
        }
#endif

        /* Return TCPIP_OK if the transmit request is processed successfully */
        if( ( (0u == TotLength_u16) || (0u != lIPv4_HdrFields_tst.TotalLen_u16) ) && (E_OK == lRetValStdType_en) )
        {
            lRetValTcpIpType_en = TCPIP_OK;
        }
    }  /* if ( NULL_PTR != lCtrlCfg_pst ) */

    return ( lRetValTcpIpType_en );
}

/********************************************************************************************************************/
/* rba_EthIPv4_Transmit_Prv_DetChecks() - This API checks the development errors (DETs) for                         */
/*                                            rba_EthIPv4_Transmit_Prv_DetChecks() API                              */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8      - Local address index on which data is to be transmitted.                                   */
/*  DestIpAddr_u32      - Destination IP address to which the data is to be transmitted.                            */
/*  Buffer_pu8          - Starting address of the buffer                                                            */
/*  IpHeader_pcst       - structure contains values required to fill out IPv4 header                                */
/*  TotLength_u16       - Total length of upper layer data                                                          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/* LocalAddrConfig_ppcst - Pointer to the localAddress config structure for the passed LocalAddrId.                 */
/* CtrlCfg_ppcst          - Pointer to the IPv4 configuration structure corresponding to the requested EthIfCtrl.   */
/*                                                                                                                  */
/* Return                                                                                                           */
/*   Std_ReturnType    - E_OK : No development error detected. The function shall do further processing.            */
/*                     - E_NOT_OK : Development error detected. The function shall exit without processing further. */
/*                                                                                                                  */
/********************************************************************************************************************/

static Std_ReturnType rba_EthIPv4_Transmit_Prv_DetChecks(
                                                             uint8 LocalAddrId_u8,
                                                             uint32 DestIpAddr_u32,
                                                             const uint8 * Buffer_pcu8,
                                                             const TcpIp_IpHeader_tst * IpHeader_pcst,
                                                             uint16 TotLength_u16,
                                                             const TcpIp_LocalAddrConfig_to ** LocalAddrConfig_ppcst,
                                                             const rba_EthIPv4_CtrlCfg_tst ** CtrlCfg_ppcst )
{
    Std_ReturnType                   lRetValStdType_en;
    const rba_EthIPv4_CtrlCfg_tst  * lCtrlCfg_pst;
    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;

    /* Initialize local variables. */
    lCtrlCfg_pst                   = NULL_PTR;

    /* Check for DET errors */
    /* Report DET if rba_EthIPv4 module is uninitialised */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthIPv4_CtrlCfg_pcpcst ), RBA_ETHIPV4_E_TRANSMIT_API_ID,      \
                                            RBA_ETHIPV4_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    /* LocalAddrId_u8 is considered as invalid if it exceeds number of configured local address and also if it's not an IPv4 localAddress */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),      \
                                            RBA_ETHIPV4_E_TRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Get localAddressId properties from TcpIp configuration */
    lRetValStdType_en = TcpIp_GetLocalAddrIdProperties( LocalAddrId_u8 ,
                                                        ( LocalAddrConfig_ppcst ) );

    lLocalAddrConfig_pcst  = *LocalAddrConfig_ppcst;

    if(E_OK == lRetValStdType_en)
    {
        /* Initialize the pointer to requested controller configuration */
        lCtrlCfg_pst = rba_EthIPv4_CtrlCfg_pcpcst[lLocalAddrConfig_pcst->EthIfCtrlRef_u8] ;

    }

    /* Report DET if requested controller is not configured for IPv4 or the localAddress is not of type UNICAST */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( (NULL_PTR == lCtrlCfg_pst) || (TCPIP_UNICAST != lLocalAddrConfig_pcst->IpAddressType_en) ),             \
                                             RBA_ETHIPV4_E_TRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    *CtrlCfg_ppcst           = lCtrlCfg_pst;

    /* Report DET if BufIdx_pu8 is a Null pointer */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == Buffer_pcu8 ), RBA_ETHIPV4_E_TRANSMIT_API_ID,                  \
                                            RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET if IpHeader_pcst is a Null pointer */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == IpHeader_pcst ), RBA_ETHIPV4_E_TRANSMIT_API_ID,               \
                                            RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Report DET if the transport layer protocol is TCP and IPv4 frame length is greater than MTU */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( (IpHeader_pcst->ProtoType_en == TCPIP_IPPROTO_TCP) && ((TotLength_u16 + RBA_ETHIPV4_HDRLENGTH) > lCtrlCfg_pst->EthIfCtrlMtu_cu16) , \
                                             RBA_ETHIPV4_E_TRANSMIT_API_ID, RBA_ETHIPV4_E_MSGSIZE, E_NOT_OK )

    #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
     /* Report to DET if buffer length is less than UDP header and destination IP is broadcast */
     /* Use for the below DET: For Dhcp communication during initial IP assignment process source IP shall be set zero. To verify whether the frame is of Dhcp communication, IPv4 fetches port fields of the Udp header.  */
     RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( (TotLength_u16 != 0) && ( TotLength_u16 < RBA_ETHUDP_HDRLENGTH ) && (TCPIP_IPPROTO_UDP == IpHeader_pcst->ProtoType_en) && (TCPIP_IPADDR_BROADCAST == DestIpAddr_u32) , \
                                             RBA_ETHIPV4_E_TRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )
    #endif

    /* If there was no return from the DET checks, then return E_OK as all DET checks passed. */
    return E_OK;
}

/********************************************************************************************************************/
/* rba_EthIPv4_GetPseudoHeader() - This API provides the structure containing pseudo header information(protocol    */
/* type, local IP and Remote IP in Tx/Rx buffer) required for Udp  checksum calculation.                            */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrId_u8        - Index of local address requesting configuration information                             */
/*  ULSocketOwner_u8      - Upper layer socket owner.                                                               */
/*  BufLength_u16         - Buffer length                                                                           */
/*                                                                                                                  */
/* Inout Parameters :                                                                                               */
/*  lPseudoHdr_pst   - This structure contains IPv4 header field values to be used in transmitted frame             */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  Std_ReturnType       - E_OK if the pseudoHeader structure was filled correctly else E_NOT_OK.                   */
/*                                                                                                                  */
/********************************************************************************************************************/
Std_ReturnType rba_EthIPv4_GetPseudoHeader( uint8   LocalAddrId_u8,
                                            uint8   ULSocketOwner_u8,
                                            uint16  BufLength_u16,
                                            TcpIp_PseudoHdr_tst  * lPseudoHdr_pst )
{
    /* Local variable declaration */
    const TcpIp_LocalAddrConfig_to                                      * lLocalAddrConfig_pcst;
    rba_EthIPv4_HdrFields_tst                                           lIPv4_HdrFields_st;
    Std_ReturnType                                                      lRetVal_en;

    /* Local variable Initialization */
    lRetVal_en              = E_NOT_OK;

    /* Report DET if rba_EthIPv4 module is uninitialised */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthIPv4_CtrlCfg_pcpcst ), RBA_ETHIPV4_E_GETPSEUDOHEADER_API_ID,      \
                                            RBA_ETHIPV4_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    /* LocalAddrId_u8 is considered as invalid if it exceeds number of configured local address and also if it's not an IPv4 localAddress */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),      \
                                            RBA_ETHIPV4_E_GETPSEUDOHEADER_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )


    /* Get localAddressId properties from TcpIp configuration */
    lRetVal_en = TcpIp_GetLocalAddrIdProperties( LocalAddrId_u8 ,
                                               ( &lLocalAddrConfig_pcst ) );


    if ( E_OK == lRetVal_en )
    {

        /* Report DET if the localAddress is not of type UNICAST */
        RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( TCPIP_UNICAST != lLocalAddrConfig_pcst->IpAddressType_en ),             \
                                                    RBA_ETHIPV4_E_GETPSEUDOHEADER_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

        /* Initialize protocol, destination IP address and socket owner. */
        lIPv4_HdrFields_st.ProtoType_en = lPseudoHdr_pst->ProtoType_en;
        lIPv4_HdrFields_st.DstIp_u32    = lPseudoHdr_pst->RemoteIpAddr_un.IPv4Addr_u32;
        lIPv4_HdrFields_st.Sockowner_u8 = ULSocketOwner_u8;

        /* Get source IPv4 address */
        lRetVal_en = rba_EthIPv4_GetSrcIP( lLocalAddrConfig_pcst,
                                           BufLength_u16,
                                           &lIPv4_HdrFields_st );

        if (E_OK == lRetVal_en)
        {
            lPseudoHdr_pst->LocalIpAddr_un.IPv4Addr_u32 = lIPv4_HdrFields_st.SrcIp_u32;
        }
        else
        {
            /* Nothing to do: propagate the error */
            lRetVal_en = E_NOT_OK;
        }

    }
    else
    {
        /* Nothing to do: propagate the error */
        lRetVal_en = E_NOT_OK;
    }

    return lRetVal_en;

}

/********************************************************************************************************************/
/* rba_EthIPv4_GetSrcIP() - This API sets source IP value to be used in the transmitted frame.                      */
/*                              Additionally it validates message length and transport layer protocol.              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrConfig_cpcst - Pointer to the localAddress configuration structure present in TcpIp.                   */
/*  TotLength_u16         - Transmission length as requested by upper layer.                                        */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  IPv4_HdrFields_pst   - This structure contains IPv4 header field values to be set in transmitted frame          */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  Std_ReturnType       - E_OK if length is non-zero and transport layer protocol is valid, else E_NOT_OK          */
/*                                                                                                                  */
/********************************************************************************************************************/

static Std_ReturnType rba_EthIPv4_GetSrcIP( const TcpIp_LocalAddrConfig_to * const LocalAddrConfig_cpcst,
                                            uint16 TotLength_u16,
                                            rba_EthIPv4_HdrFields_tst * IPv4_HdrFields_pst)
{
    /* Local variable declaration */
    Std_ReturnType      lRetValStdType_en;

    lRetValStdType_en = E_NOT_OK;

    /* Check whether the message length is non-zero and transport layer protocol is valid  */
    if( (0 != TotLength_u16) &&
        ( (TCPIP_IPPROTO_UDP  == IPv4_HdrFields_pst->ProtoType_en) || (TCPIP_IPPROTO_TCP  == IPv4_HdrFields_pst->ProtoType_en) ||  (TCPIP_IPPROTO_ICMP  == IPv4_HdrFields_pst->ProtoType_en) ) )
    {
        /* Set source IP in the frame to assigned local IP  */
        IPv4_HdrFields_pst->SrcIp_u32 = LocalAddrConfig_cpcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32;

        #if ( TCPIP_DHCP_CLIENT_ENABLED == STD_ON )
        /* If the frame to be transmitted is Dhcp frame with destination IP  broadcast and Dhcp IP is not yet assigned to the EthIfCtrl, set source IP in the IPv4 header to 0 */
        if( (TCPIP_IPPROTO_UDP == IPv4_HdrFields_pst->ProtoType_en) &&
            (TCPIP_IPADDR_BROADCAST == IPv4_HdrFields_pst->DstIp_u32) )
        {
             /* If the transmission request is from Dhcp (socket owner is DHCP) and current IP assignment method is not Dhcp ( Dhcp IP is not yet assigned ), set source IP address field in the message to 0  */
             if( ( IPv4_HdrFields_pst->Sockowner_u8 == SOCK_OWNER_DHCP ) &&
                 ( TCPIP_IPADDR_ASSIGNMENT_DHCP != LocalAddrConfig_cpcst->CurrAsgnedAddr_pst->CurrAsgnedAddrMeth_en ) )
             {
                 IPv4_HdrFields_pst->SrcIp_u32 = 0;
             }
        }
        #endif

        lRetValStdType_en = E_OK;
    }

    return lRetValStdType_en;

}

/********************************************************************************************************************/
/* rba_EthIPv4_GetTTL() - This API sets TTL value to be used in the transmitted frame.                              */
/*                              Additionally it validates message length and transport layer protocol.              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrConfig_cpcst - Pointer to the localAddress configuration structure present in TcpIp.                   */
/*  TotLength_u16         - Transmission length as requested by upper layer.                                        */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  IPv4_HdrFields_pst   - This structure contains IPv4 header field values to be set in transmitted frame          */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  Std_ReturnType       - E_OK if length is non-zero and transport layer protocol is valid, else E_MOT_OK          */
/*                                                                                                                  */
/********************************************************************************************************************/

static Std_ReturnType rba_EthIPv4_GetTTL( const TcpIp_LocalAddrConfig_to * const LocalAddrConfig_cpcst,
                                          uint16 TotLength_u16,
                                          rba_EthIPv4_HdrFields_tst * IPv4_HdrFields_pst)
{
    /* Local variable declaration */
    Std_ReturnType      lRetValStdType_en;

    lRetValStdType_en = E_NOT_OK;

    /* Check whether the message length is non-zero and transport layer protocol is valid  */
    if( (0 != TotLength_u16) &&
        ( (TCPIP_IPPROTO_UDP  == IPv4_HdrFields_pst->ProtoType_en) || (TCPIP_IPPROTO_TCP  == IPv4_HdrFields_pst->ProtoType_en) ||  (TCPIP_IPPROTO_ICMP  == IPv4_HdrFields_pst->ProtoType_en) ) )
    {
        /* TTL value shall be RBA_ETHIPV4_TTL_VALUE, if */
        /* 1. Source IP is AutoIp                       */
        /* 2. Destination IP AutoIp                     */
        /* 3. If configuration parameter tcpIpIpTtlConfigurationEnabled is disabled for the requested EthIfCtrl */
        /* 4. If upper layer provides TTL value as 0    */
        if ( ( FALSE == rba_EthIPv4_CtrlCfg_pcpcst[ LocalAddrConfig_cpcst->EthIfCtrlRef_u8 ]->TtlEnabled_cb ) ||
             ( ( IPv4_HdrFields_pst->DstIp_u32 & TCPIP_AUTOIP_MASK ) == TCPIP_AUTOIP_NET ) ||
             ( ( IPv4_HdrFields_pst->SrcIp_u32 & TCPIP_AUTOIP_MASK ) == TCPIP_AUTOIP_NET ) ||
             ( 0 == IPv4_HdrFields_pst->TTL_u8 ) )
        {
            IPv4_HdrFields_pst->TTL_u8 = RBA_ETHIPV4_TTL_VALUE;
        }

        lRetValStdType_en = E_OK;
    }

    return lRetValStdType_en;

}

/********************************************************************************************************************/
/* rba_EthIPv4_FillHdr() - This API updates IPv4 header fields in the frame to be transmitted.                      */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  CtrlCfg_pcst       - Pointer to the IPv4 configuration structure corresponding to the requested EthIfCtrl.      */
/*  IPv4_HdrFields_pcst  - Contains information about all the required IPv4 header fileds.                          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  Buffer_pu8          - The driver buffer which will be filled by this API and passed to EthIf_Transmit() API.    */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  Std_ReturnType      - Returns E_OK, if filling of the buffer is successful.                                     */
/*                        Returns E_NOT_OK, if filling of the buffer fails.                                         */
/*                                                                                                                  */
/********************************************************************************************************************/
static Std_ReturnType rba_EthIPv4_FillHdr( const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                           const rba_EthIPv4_HdrFields_tst * IPv4_HdrFields_pcst,
                                           uint8 * Buffer_pu8 )
{

    uint16             lVerHdrLenToS_u16;
    Std_ReturnType     lRetValStdType_en;

#if (( RBA_ETHIPV4_SW_CHKSUM_TX_IPV4 == STD_ON ) && ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON ))
    uint16             lIPHdrChksum_u16;
#endif

    /* Update IP version number, IPv4 header length and Type of Service field values */
    lVerHdrLenToS_u16   = ( (uint16)( (uint16)( RBA_ETHIPV4_IPV4_VERSION_NUM ) << 12u )  |
                            (uint16)( (uint16)( RBA_ETHIPV4_HDRLENGTH / 4u ) << 8u )     |
                            (uint16)( (uint16)( IPv4_HdrFields_pcst->DiffServiceCodepoint_u8 ) << 2u ) );

    TcpIp_WriteU16( ( Buffer_pu8 + RBA_ETHIPV4_IPV4VER_HDRLEN_TOS_UB ), lVerHdrLenToS_u16 );

    /* Update the length of the IP packet into the corresponding field in the IP header */
    TcpIp_WriteU16( ( Buffer_pu8 + RBA_ETHIPV4_TOTLEN_UB ), IPv4_HdrFields_pcst->TotalLen_u16 );

    /* Update the fragmentation ID field in the IP header. */
    TcpIp_WriteU16( ( Buffer_pu8 + RBA_ETHIPV4_FRAGID_UB ), IPv4_HdrFields_pcst->FragId_u16 );

    /* Update the fragmentation offset field in the IP header */
    TcpIp_WriteU16( ( Buffer_pu8 + RBA_ETHIPV4_FRAGOFFSET_UB ), IPv4_HdrFields_pcst->FragFlagOffset_u16 );

    /* Update the TimeToLive value */
    Buffer_pu8[RBA_ETHIPV4_TTL_OFFSET] = IPv4_HdrFields_pcst->TTL_u8;

    /* Update the Protocol type value in the IPv4 header */
    Buffer_pu8[RBA_ETHIPV4_TP_PROTOCOL] = (uint8)IPv4_HdrFields_pcst->ProtoType_en;

    /* Checksum field shall be set to zero before the calculation */
    TcpIp_WriteU16( ( Buffer_pu8 + RBA_ETHIPV4_IPHDRCHECKSUM_UB ), 0 );

    /* Update the source IPv4 address into the IPv4 header */
    TcpIp_WriteU32( ( Buffer_pu8 + RBA_ETHIPV4_SRCIPV4ADDR_OFFSET ), IPv4_HdrFields_pcst->SrcIp_u32 );

    /* Update the destination address value in the IPv4 header */
    TcpIp_WriteU32( ( Buffer_pu8 + RBA_ETHIPV4_DESTIPV4ADDR_OFFSET ), IPv4_HdrFields_pcst->DstIp_u32 );

    /* If software checksum calculation is enabled, compute the checksum and fill the value in the IPv4 header */
#if ( RBA_ETHIPV4_SW_CHKSUM_TX_IPV4 == STD_ON )
    /* Check whether checksum offloading is enabled for the hardware referenced by the current TcpIpCtrl. */
    if( CtrlCfg_pcst->IPv4ChkSumOffloadingEnabled_cb == FALSE )
    {
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
        /* MR12 DIR 1.1 VIOLATION: The input parameter of TcpIp_OnesComplChkSum() is declared as (void*) for faster implmentation of checksum calculation. Low Risk */
        lRetValStdType_en = TcpIp_OnesComplChkSum( Buffer_pu8,
                                                   NULL_PTR,                  /* Pseudo header is not required for IPv4 */
                                                   RBA_ETHIPV4_HDRLENGTH,
                                                   &lIPHdrChksum_u16);

        if( lRetValStdType_en == E_OK )
        {
            /* Store the calculated checksum in the corresponding field of the IPv4 header */
            TcpIp_WriteU16( ( Buffer_pu8 + RBA_ETHIPV4_IPHDRCHECKSUM_UB ), lIPHdrChksum_u16 );
        }

#else
        /* Report DET: If TcpIp_OnesComplChkSum() is not available */
        RBA_ETHIPV4_CALL_DET_REPORTERROR( RBA_ETHIPV4_E_TRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG )

        lRetValStdType_en = E_NOT_OK;
#endif
    }
    else
    {
        /* If checksum offloading is enabled for the TcpIpCtrl, then return E_OK as checksum will be calculated by the hardware. */
        lRetValStdType_en = E_OK;
    }

#else
    /* CtrlCfg_pcst is used only when software checksum calculation is enabled. */
    (void)CtrlCfg_pcst;

    lRetValStdType_en = E_OK;
#endif

    return lRetValStdType_en;

}

/********************************************************************************************************************/
/* rba_EthIPv4_GetDestEthAddr() - This API provides the destination MAC address for requested destination IP.       */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  LocalAddrConfig_cpcst - Pointer to the localAddress configuration structure present in TcpIp.                   */
/*  DestIpAddr_u32        - Destination IP address to which the data is to be transmitted.                          */
/*                                                                                                                  */
/* Output Parameters :                                                                                              */
/*  DestEthAddr_pu8      - Destination MAC for which frame will be transmitted to.     .                            */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  TcpIp_ReturnType    - Returns TCPIP_OK, if the destination MAC is successfully updated                          */
/*                        Returns TCPIP_E_ARP_CACHE_MISS, if the operation failed because of an ARP cache miss.     */
/*                        Returns TCPIP_E_NOT_OK, if the operation failed due to failure of internal function calls.*/
/*                                                                                                                  */
/********************************************************************************************************************/
TcpIp_ReturnType rba_EthIPv4_GetDestEthAddr(  const TcpIp_LocalAddrConfig_to * const LocalAddrConfig_cpcst,
                                              uint32 DestIpAddr_u32,
                                              uint8 * DestEthAddr_pu8 )
{
    /* Variable declaration */
    TcpIp_ReturnType          lRetValTcpIpType_en;
    TcpIp_AddressType         lRetValAddrType_en;
    uint32                    lLocalIPv4Addr_u32;
    uint32                    lNetMask_u32;

    /* Initialization of variables */
    lRetValTcpIpType_en     = TCPIP_E_NOT_OK;
    lNetMask_u32            = 0x00uL;
    lLocalIPv4Addr_u32      = 0x00uL;
    lRetValAddrType_en      = TCPIP_NONE;

    /* Get source IP address and netmask assigned to the localAddress */
    lLocalIPv4Addr_u32 = LocalAddrConfig_cpcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.IpAddr_u32;
    lNetMask_u32       = LocalAddrConfig_cpcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.NetMask_u32;

    /* Check if destination IP is a unicast/multicast/broadcast address? */
    lRetValAddrType_en = rba_EthIPv4_ChkIPv4AddrType( DestIpAddr_u32, lLocalIPv4Addr_u32, lNetMask_u32 );

    switch(lRetValAddrType_en)
    {
        case TCPIP_UNICAST:
        {
            /* Check if ARP module is enabled */
            #if ( TCPIP_ARP_ENABLED == STD_ON )

            /* Proceed with getting destination MAC if destination IP is valid */
            /* Destination IP is considered valid, if                     */
            /* 1. local assigned IP and destination IP are different      */
            /* 2. Destination IP is not loop back IP                      */
            /* 3. Destination IP is not in reserved IP range 240.0.0.0/4  */
            if( ( lLocalIPv4Addr_u32 != DestIpAddr_u32 )  &&
                ( RBA_ETHIPV4_LOOPBACK != ( DestIpAddr_u32 & RBA_ETHIPV4_LOOPBACK_MASK ) ) &&
                ( RBA_ETHIPV4_RESERVED_IP != ( DestIpAddr_u32 & RBA_ETHIPV4_RESERVED_IP) ) )
            {
                /* Check whether the destination IP address is outside the same subnet */
                /* If so, overwrite the destination IP to default router IP */
                if ( ( ( DestIpAddr_u32     & lNetMask_u32 )      != ( lLocalIPv4Addr_u32 & lNetMask_u32 ) ) &&     /* Check whether source and destination IP are in different subnet AND */
                     ( ( DestIpAddr_u32     & TCPIP_AUTOIP_MASK ) != TCPIP_AUTOIP_NET  ) &&                         /* Check whether destination IP is not AutoIp AND                      */
                     ( ( lLocalIPv4Addr_u32 & TCPIP_AUTOIP_MASK ) != TCPIP_AUTOIP_NET ) )                           /* Check whether source IP is not AutoIp                               */
                {
                    /* Get the default router IP address (overwrite the destination IP to default router IP). */
                    DestIpAddr_u32 = LocalAddrConfig_cpcst->CurrAsgnedAddr_pst->IpAddrParams_un.IPv4Par_st.DftRtr_u32;
                }

                if( 0u != DestIpAddr_u32 )           /* If the frame is outside the subnet check whether the default router IP is configured (not zero)  */
                {
                    /* Get the MAC address of the destination node */
                    lRetValTcpIpType_en = rba_EthArp_GetDestEthAddr( LocalAddrConfig_cpcst->EthIfCtrlRef_u8,
                                                                     DestIpAddr_u32,
                                                                     DestEthAddr_pu8 );
                }
            }
            #endif  /* #if ( TCPIP_ARP_ENABLED == STD_ON ) */
        }
        break;

        case TCPIP_MULTICAST:
        {
            /* Get multicast MAC address for Multicast destination IP address */
            rba_EthIPv4_GetIPMulticastDestEthAddr(  DestIpAddr_u32,
                                                    DestEthAddr_pu8 );
            lRetValTcpIpType_en = TCPIP_OK;
        }
        break;

        case TCPIP_BROADCAST:
        {
            /* For broadcast IP set destination MAC to FF:FF:FF:FF:FF:FF */
            DestEthAddr_pu8[0] = 0xFFu;
            DestEthAddr_pu8[1] = 0xFFu;
            DestEthAddr_pu8[2] = 0xFFu;
            DestEthAddr_pu8[3] = 0xFFu;
            DestEthAddr_pu8[4] = 0xFFu;
            DestEthAddr_pu8[5] = 0xFFu;

            lRetValTcpIpType_en = TCPIP_OK;
        }
        break;

        default:
        {   /* Do nothing */
        }
        break;
    }

    return ( lRetValTcpIpType_en );
}

#if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )

/********************************************************************************************************************/
/* rba_EthIPv4_FragBufHandler_AllocateBuf() - This API allocates IPv4 static buffer for fragmentation handling.     */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  CtrlCfg_pcst      - Pointer to the IPv4 configuration structure corresponding to the requested EthIf controller.*/
/*  BufLength_u32     - Length of IPv4 buffer that needs to be allocated.                                           */
/*                                                                                                                  */
/* Out Parameters :                                                                                                 */
/*  Buffer_ppu8   - If sufficient free buffer is found the parameter contains base address of the allocated buffer  */
/*                  else if sufficient buffer is not found the parameter contains NULL_PTR.                         */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  void                                                                                                            */
/*                                                                                                                  */
/********************************************************************************************************************/

static void rba_EthIPv4_FragBufHandler_AllocateBuf(
                                            const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                            uint32 BufLength_u32,
                                            uint8 * * Buffer_ppu8 )
{
    /* Local variable declaration */
    uint16    lBufTblIdx_u16;
    uint16    lLoopIdx_u16;
    rba_EthIPv4_FragBufTbl_tst *    lBufTbl_pst;

    /* Initialize the buffer table index to invalid value (size of table) */
    lBufTblIdx_u16 = CtrlCfg_pcst->TxFragBufTblSize_cu16;

    /* Initialize the pointer to the fragmentation buffer table of the requested controller */
    lBufTbl_pst = CtrlCfg_pcst->TxFragBufTbl_cpst;

    /* Make total length word aligned */
    if((BufLength_u32 % 4u ) != 0u )
    {
        BufLength_u32 += (4u - (BufLength_u32 % 4u));
    }

    /* Loop through fragmentation table to find the table index which has minimum length that satisfies the request */
    for(lLoopIdx_u16  = 0; lLoopIdx_u16 < CtrlCfg_pcst->TxFragBufTblSize_cu16; lLoopIdx_u16 ++)
    {
        if ( ( BufLength_u32  <= lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32 ) &&                                   /* Check whether table entry specified by loop index has sufficient length                                */
             ( (lBufTblIdx_u16 == CtrlCfg_pcst->TxFragBufTblSize_cu16) ||                                           /* Check whether searching through fragmentation table has not yet found entry that satisfies the request */
               (lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32 < lBufTbl_pst[lBufTblIdx_u16].FreeBufLength_u32 ) ) )   /* Check whether the length at current index is less than the length at already stored index              */
        {
            lBufTblIdx_u16 = lLoopIdx_u16;
        }
    }

    /* Check whether searching through fragmentation buffer table has found valid entry */
    if(lBufTblIdx_u16 == CtrlCfg_pcst->TxFragBufTblSize_cu16)
    {
        *Buffer_ppu8 = NULL_PTR;                                                                                    /* If valid entry is not found update output pointer to NULL_PTR */
    }
    else
    {
        *Buffer_ppu8 = ( (CtrlCfg_pcst->TxFragBuf_cpu8) + (lBufTbl_pst[lBufTblIdx_u16].FreeBufPos_u32) );           /* If valid entry is found update output pointer to the base address of the buffer to be allocated */

        /*Buffer length shall be stored as hidden header in IPv4 fragmentation buffer as buffer length is required for buffer handling algorithm and there is possibility of upper layer calling rba_EthIPv4_Transmit() with length zero.*/
        /* Stored buffer length includes additional twelve bytes used to store frame length, frame priority, destination MAC and word alignment correction. And buffer length is stored in big endian format.*/
        **Buffer_ppu8  = (uint8)( ((BufLength_u32/4u) & 0xFF00u ) >> 8u );       /* Length shall be stored in terms of 4 bytes word. This is required as upper layer of IPv4 can request for length up to 65535 and, with */
        *((*Buffer_ppu8) + 1u) = (uint8)((BufLength_u32/4u) & 0x00FFu);           /* the addition of hidden header and word alignment correction it can lead to wrap around. */

        lBufTbl_pst[lBufTblIdx_u16].FreeBufPos_u32 += BufLength_u32;             /* Update buffer table entry - Increment free buffer position and decrement length             */
        lBufTbl_pst[lBufTblIdx_u16].FreeBufLength_u32 -= BufLength_u32;

        if(0u == lBufTbl_pst[lBufTblIdx_u16].FreeBufLength_u32)                                                      /* If there is no more buffer is available from the current buffer position clear the buffer position value */
        {
            lBufTbl_pst[lBufTblIdx_u16].FreeBufPos_u32 = 0;
        }
    }
}


/********************************************************************************************************************/
/* rba_EthIPv4_FragBufHandler_ReleaseBuf() - This API releases IPv4 static buffer for fragmentation handling.       */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  CtrlCfg_pcst      - Pointer to the IPv4 configuration structure corresponding to the requested EthIf controller.*/
/*  Buffer_pcu8   - Contains base address of the buffer to be released.                                             */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  void                                                                                                            */
/*                                                                                                                  */
/********************************************************************************************************************/

static void rba_EthIPv4_FragBufHandler_ReleaseBuf(
                                            const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                            const uint8 * Buffer_pcu8 )
{
    /* Local variable declaration */
    uint16    lBufTblIdx_u16;
    uint16    lFirstMergIdx_u16;
    uint16    lLoopIdx_u16;
    uint32    lReleaseBufPos_u32;
    uint32    lBufLength_u32;
    rba_EthIPv4_FragBufTbl_tst *    lBufTbl_pst;

    /* Initialize the buffer table index to invalid value (size of table) */
    lBufTblIdx_u16 = CtrlCfg_pcst->TxFragBufTblSize_cu16;

    /* Initialize the pointer to the fragmentation buffer table of the requested controller */
    lBufTbl_pst = CtrlCfg_pcst->TxFragBufTbl_cpst;

    /* Extract the buffer length stored in the hidden header */
    lBufLength_u32 = (uint16)((uint16)(Buffer_pcu8[0]) << 8u) | (Buffer_pcu8[1]);

    /* Get the buffer length in terms of bytes (While providing buffer, the length is stored in terms of 4 bytes ) */
    lBufLength_u32 = lBufLength_u32 * 4u;

    lReleaseBufPos_u32 = (uint32)(Buffer_pcu8 - (CtrlCfg_pcst->TxFragBuf_cpu8));                               /* Find the position within the IPv4 buffer corresponding to the buffer to be released */

    /* MR12 RULE 15.4 VIOLATION: Avoiding multiple break statements will make code unreadable */
    for(lLoopIdx_u16 = 0; lLoopIdx_u16 < CtrlCfg_pcst->TxFragBufTblSize_cu16; lLoopIdx_u16 ++ )                  /* Loop through fragmentation buffer table of the controller                    */
    {
        if( (lReleaseBufPos_u32 + lBufLength_u32) == lBufTbl_pst[lLoopIdx_u16].FreeBufPos_u32 )                   /* Check whether buffer to be released coincides with the beginning of already existing free buffer chunk  */
        {
            lBufTbl_pst[lLoopIdx_u16].FreeBufPos_u32 =  lReleaseBufPos_u32;                                      /* Update free buffer position and length.                                                                  */
            lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32 += lBufLength_u32;
            break;
        }
        else if( (lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32 + lBufTbl_pst[lLoopIdx_u16].FreeBufPos_u32) == lReleaseBufPos_u32)   /* Check whether buffer to be released coincides with the end of already existing free buffer chunk  */
        {
            lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32 += lBufLength_u32;                                           /* Update free buffer length */

            break;
        }
        else
        {
            if( (0u == lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32) &&                                               /* Find the index of the free buffer table which has length element zero */
                (CtrlCfg_pcst->TxFragBufTblSize_cu16 == lBufTblIdx_u16) )
            {
                lBufTblIdx_u16 = lLoopIdx_u16;
            }
        }
    }

    if(CtrlCfg_pcst->TxFragBufTblSize_cu16 == lLoopIdx_u16)                                                        /* If the buffer to be released cannot be merged to any of the entries in the free buffer table */
    {
        if(CtrlCfg_pcst->TxFragBufTblSize_cu16 != lBufTblIdx_u16)                                                  /* Create new entry in the table if table has a free entry */
        {
            lBufTbl_pst[lBufTblIdx_u16].FreeBufPos_u32 =  lReleaseBufPos_u32;
            lBufTbl_pst[lBufTblIdx_u16].FreeBufLength_u32 = lBufLength_u32;
        }
        else                                                                                                        /* If the table has no free entry report DET error. */
        {
            RBA_ETHIPV4_DET_REPORT_ERROR_TRUE_RET_VOID( RBA_ETHIPV4_E_FRAGBUFHANDLER_API_ID, RBA_ETHIPV4_E_BUFRELEASE_FAILED )
        }
    }
    else
    {
        lFirstMergIdx_u16 = lLoopIdx_u16;                                                                           /* Locally store the index of free buffer table for which buffer to be released is merged */

        /* MR12 RULE 15.4 VIOLATION: Avoiding multiple break statements will make code unreadable */
        for(lLoopIdx_u16 = lFirstMergIdx_u16 + 1u; lLoopIdx_u16 < CtrlCfg_pcst->TxFragBufTblSize_cu16; lLoopIdx_u16 ++ )     /* Continue looping through free buffer table */
        {
            if( (lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32 + lBufTbl_pst[lLoopIdx_u16].FreeBufPos_u32) == lBufTbl_pst[lFirstMergIdx_u16].FreeBufPos_u32)   /* If the current buffer chunk in the table entry coincides with beginning of */
            {                                                                                                                                                /* already merged entry.                                                       */
                lBufTbl_pst[lFirstMergIdx_u16].FreeBufPos_u32 = lBufTbl_pst[lLoopIdx_u16].FreeBufPos_u32;                                                    /* Update free buffer position and length of the already merged entry           */
                lBufTbl_pst[lFirstMergIdx_u16].FreeBufLength_u32 += lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32;
                lBufTbl_pst[lLoopIdx_u16].FreeBufPos_u32 = 0;                                                                                                /* Clear the position and length element of the current entry                   */
                lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32 = 0;
                break;
            }
            else if( (lBufTbl_pst[lFirstMergIdx_u16].FreeBufLength_u32 + lBufTbl_pst[lFirstMergIdx_u16].FreeBufPos_u32) == lBufTbl_pst[lLoopIdx_u16].FreeBufPos_u32)   /* If the current buffer chunk in the table entry coincides with end of */
            {                                                                                                                                                          /* already merged entry.                                                 */
                lBufTbl_pst[lFirstMergIdx_u16].FreeBufLength_u32 += lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32;                                                       /* Update free buffer length of the already merged entry                 */
                lBufTbl_pst[lLoopIdx_u16].FreeBufPos_u32 = 0;                                                                                                          /* Clear the position and length element of the current entry            */
                lBufTbl_pst[lLoopIdx_u16].FreeBufLength_u32 = 0;

                break;
            }
            else
            {  /* Do nothing */
            }
        }
    }
}
#endif

/********************************************************************************************************************/
/* rba_EthIPv4_UpdateTPLayerChkSum() - This API updates checksum for transport layer protocol.                      */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/*  CtrlCfg_pcst       - Pointer to the IPv4 configuration structure corresponding to the requested EthIfCtrl.      */
/*  PseudoHdr_pcst     - Pointer to structure containing information about protocol, source IP  and destination IP. */
/*  Buffer_pu8         - Pointer to transport layer header.                                                         */
/*  TPLayerLength_cu16 - Length of transport layer frame.                                                           */
/*  IsFragFrame_cb     - Indicates whether the frame undergoes fragmentation.                                       */
/*                                                                                                                  */
/* Return                                                                                                           */
/*  Std_ReturnType     - E_OK       if checksum update is successful                                                */
/*                       E_NOT_OK   if checksum update failed                                                       */
/*                                                                                                                  */
/********************************************************************************************************************/
#if (RBA_ETHIPV4_UPDATE_TPLAYERCHKSUM_ENABLED)   /* If rba_EthIPv4_UpdateTPLayerChkSum() API is enabled */
static Std_ReturnType rba_EthIPv4_UpdateTPLayerChkSum( const rba_EthIPv4_CtrlCfg_tst * CtrlCfg_pcst,
                                                       const TcpIp_PseudoHdr_tst * PseudoHdr_pcst,
                                                       uint8 * Buffer_pu8,
                                                       const uint16 TPLayerLength_cu16,
                                                       const boolean IsFragFrame_cb )
{

    /* Local variable declaration */
    Std_ReturnType    lRetValStdType_en;
    boolean           lCalculateChkSum_b;

#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
    uint16            lTPLayerChksum_u16;
#endif

    /* Local variable initialization */
    lRetValStdType_en = E_OK;

    /* Checksum for transport layer shall be calculated by software if                  */
    /* 1. The protocol is ICMP and software checksum calculation for ICMP is enabled OR */
    /* 2. the frame undergoes fragmentation                                             */
    /* Note: TCP software checksum calculation is taken care by rba_EthTcp module.      */
    if(TCPIP_IPPROTO_ICMP == PseudoHdr_pcst->ProtoType_en)
    {
        lCalculateChkSum_b = (
        #if (RBA_ETHIPV4_SW_CHKSUM_TX_ICMP == STD_ON)
        ( CtrlCfg_pcst->IcmpChkSumOffloadngEnabled_cb == FALSE ) ||
        #endif
        #if ( RBA_ETHIPV4_FRAG_ENABLED == STD_ON )
        ( (IsFragFrame_cb) )                                  ||
        #endif
        FALSE );   /* OR with FALSE is required to avoid compilation issues when any of the above compiler switches are STD_OFF */

        if(FALSE != lCalculateChkSum_b)     /* If for the requested frame transport layer checksum needs to be calculated by software */
        {
#if ( TCPIP_ONES_COMPL_CHKSUM_ENABLED == STD_ON )
            /* MR12 DIR 1.1 VIOLATION: The input parameter of TcpIp_OnesComplChkSum() is declared as (void*) for faster implmentation of checksum calculation. Low Risk */
            lRetValStdType_en = TcpIp_OnesComplChkSum(Buffer_pu8,
                                                      NULL_PTR,
                                                      TPLayerLength_cu16,
                                                      &lTPLayerChksum_u16 );

            if( lRetValStdType_en == E_OK )
            {
                /* Write the computed checksum value in the checksum field. */
                TcpIp_WriteU16( &Buffer_pu8[TCPIP_ICMPCHKSUM_UB], lTPLayerChksum_u16 );
            }
#else
                (void)TPLayerLength_cu16;
                (void)Buffer_pu8;

                /* Report DET: If TcpIp_OnesComplChkSum() is not available */
                RBA_ETHIPV4_CALL_DET_REPORTERROR( RBA_ETHIPV4_E_TRANSMIT_API_ID, RBA_ETHIPV4_E_INV_ARG )

                lRetValStdType_en = E_NOT_OK;
#endif
        }
    }

    /* This API parameter will not be in use if the fragmentation is disabled*/
    (void)IsFragFrame_cb;

    /* CtrlCfg_pcst is used only when UDP and ICMP software checksum validation is enabled.  */
    (void)CtrlCfg_pcst;

    return lRetValStdType_en;
}
#endif

#define RBA_ETHIPV4_STOP_SEC_CODE
#include "rba_EthIPv4_MemMap.h"

#define RBA_ETHIPV4_START_SEC_CODE_FAST
#include "rba_EthIPv4_MemMap.h"

/********************************************************************************************************************/
/* rba_EthIPv4_CheckFragmentation()  The API returns the number of IPv4 fragments required to transmit              */
/*                                   the data with length provided in input parameter.                              */
/* Note: If the number of fragments is returned as one, it means fragmentation is not required.                     */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* LocalAddrId_u8           LocalAddrId on which data is to be transmitted.                                         */
/* DataLength_u16           Data length to be transmitted.                                                          */
/*                                                                                                                  */
/* Output parameters:                                                                                               */
/* NumOfIPFrags_pu8         Number of fragments required to transmit the frame.                                     */
/*                                                                                                                  */
/* Return type :                                                                                                    */
/* Std_ReturnType        E_OK                      : Function successful                                            */
/*                       E_NOT_OK                  : Function failed to process.                                    */
/********************************************************************************************************************/
Std_ReturnType rba_EthIPv4_CheckFragmentation
                                              ( uint8 LocalAddrId_u8,
                                                uint16 DataLength_u16,
                                                uint8 * NumOfIPFrags_pu8  )
{
    /* Local variable declaration  */
    Std_ReturnType                  lRetValStdType_en;
    const TcpIp_LocalAddrConfig_to * lLocalAddrConfig_pcst;
    uint16      lFragPayloadSize8_u16;

    /* Initialise the local variables */
    lRetValStdType_en       = E_NOT_OK;
    lLocalAddrConfig_pcst   = NULL_PTR;

    /* Check for DET errors */
    /* Report DET if rba_EthIPv4 module is uninitialised */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == rba_EthIPv4_CtrlCfg_pcpcst ), RBA_ETHIPV4_E_CHECKFRAGMENTATION_API_ID,      \
                                            RBA_ETHIPV4_E_NOTINIT, E_NOT_OK )

    /* Report DET if local address ID is invalid */
    /* LocalAddrId_u8 is considered as invalid if it exceeds number of configured local address and also if it's not an IPv4 localAddress */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( LocalAddrId_u8 >= ( TcpIp_CurrConfig_pco->NumLocalAddrConfig_u8 ) ),      \
                                             RBA_ETHIPV4_E_CHECKFRAGMENTATION_API_ID, RBA_ETHIPV4_E_INV_ARG, E_NOT_OK )

    /* Report DET if NumOfIPFrags_pu8 is a NULL_PTR. */
    RBA_ETHIPV4_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == NumOfIPFrags_pu8 ), RBA_ETHIPV4_E_CHECKFRAGMENTATION_API_ID,      \
                                            RBA_ETHIPV4_E_NULL_PTR, E_NOT_OK )

    /* Get localAddressId properties from TcpIp configuration */
    lRetValStdType_en = TcpIp_GetLocalAddrIdProperties( LocalAddrId_u8 ,
                                                        ( &lLocalAddrConfig_pcst ) );

    if(E_OK == lRetValStdType_en)
    {
        if( ( DataLength_u16 + RBA_ETHIPV4_HDRLENGTH ) > rba_EthIPv4_CtrlCfg_pcpcst[lLocalAddrConfig_pcst->EthIfCtrlRef_u8]->EthIfCtrlMtu_cu16 )
        {
            /* FragPayloadSize8 contains number of 8 byte blocks and ensures that all fragments except last one contains upper layer payload in terms of 8 byte block. */
            lFragPayloadSize8_u16 = (uint16)( (rba_EthIPv4_CtrlCfg_pcpcst[lLocalAddrConfig_pcst->EthIfCtrlRef_u8]->EthIfCtrlMtu_cu16 - RBA_ETHIPV4_HDRLENGTH)/8u );

            lFragPayloadSize8_u16 = (lFragPayloadSize8_u16 * 8u);

            /* ((FragPayloadSize8 * 8) - 1 ) is required in the numerator of the expression to increase the number of fragments by one if the Payload is not an integral multiple of (FragPayloadSize8 * 8). */
            *NumOfIPFrags_pu8 = (uint8) (( DataLength_u16 + (lFragPayloadSize8_u16 - 1u) ) / (lFragPayloadSize8_u16));
        }
        else
        {
            *NumOfIPFrags_pu8 = 1;
        }
    }

    return lRetValStdType_en;
}

#define RBA_ETHIPV4_STOP_SEC_CODE_FAST
#include "rba_EthIPv4_MemMap.h"

#endif /* #if ( defined(TCPIP_IPV4_ENABLED) && ( TCPIP_IPV4_ENABLED == STD_ON ) ) */
