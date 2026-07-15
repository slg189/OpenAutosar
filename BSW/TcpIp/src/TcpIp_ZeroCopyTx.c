

#include "TcpIp.h"
#ifdef TCPIP_CONFIGURED
#include "TcpIp_Cbk.h"

/* File is compiled only if ZeroCopy feature and Udp protocol are enabled */
#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
#if ( TCPIP_UDP_ENABLED == STD_ON )
#include "rba_EthUdp.h"

#include "TcpIp_Prv.h"


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_VAR_FAST_INIT_UNSPECIFIED
#include "TcpIp_MemMap.h"

TcpIp_ZeroCopyDescriptor_tst             TcpIp_ZeroCopyDescriptors_ast[TCPIP_ZEROCOPY_DESCRIPTOR_COUNT];
uint16                                   TcpIp_DynSockAllocatedZeroCopyDesc_au16[TCPIP_SOCKETMAX];

#define TCPIP_STOP_SEC_VAR_FAST_INIT_UNSPECIFIED
#include "TcpIp_MemMap.h"


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

/* DD: All static functions are declared as LOCAL_INLINE for speed optimization */
/* ZeroCopy transmission is used to execute faster transmission than normal Autosar fuunctions. */

#define TCPIP_START_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

LOCAL_INLINE Std_ReturnType TcpIp_AllocateDescriptor( TcpIp_SocketIdType SocketId_u16,
                                                      uint16 * AllocatedZeroCopyDescriptor_pu16 );

LOCAL_INLINE void TcpIp_InvalidateAllocatedHeaders( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst );

LOCAL_INLINE Std_ReturnType TcpIp_AllocateHeaders( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                                   uint16 DataLength_u16,
                                                   Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_past,
                                                   uint8 * AllocatedHeaderCount_pu8,
                                                   boolean * PreFillRequired_pb );

LOCAL_INLINE uint16 TcpIp_SearchReuseHeader(const TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pcst,
                                            uint16 DataLength_u16,
                                            boolean * PreFillRequired_pb );

LOCAL_INLINE void TcpIp_AllocateReuseHeader( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                             uint16 FetchedReuseHeader_u16,
                                             uint16 DataLength_u16,
                                             Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_past,
                                             uint8 * AllocatedHeaderCount_pu8);

#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
LOCAL_INLINE void TcpIp_AllocateIpOneTimeHeader( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                                 uint8 NumberOfIpHeaders_u8,
                                                 Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_past,
                                                 uint8 * AllocatedHeaderCount_pu8 );

LOCAL_INLINE void TcpIp_AllocateUdpOneTimeHeader( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                                  Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_past,
                                                  uint8 * AllocatedHeaderCount_pu8 );
#endif

LOCAL_INLINE void TcpIp_CancelAllocateHeaders( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                               const Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_pacst,
                                               uint8 AllocatedHeaderCount_u8,
                                               boolean PreFillRequired_b);

LOCAL_INLINE void TcpIp_StoreTxIdInfo( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                       uint32 TxId_u32 );

LOCAL_INLINE void TcpIp_ReleaseHeaders( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                        uint16 TxIdIdx_u16 );

LOCAL_INLINE uint16 TcpIp_IncrIndexWithWrapAround( uint16 IndexToIncr_u16,
                                                   uint16 WrapAroundCondition_u16 );

#define TCPIP_STOP_SEC_CODE_FAST
#include "TcpIp_MemMap.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define TCPIP_START_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

/********************************************************************************************************************************/
/* TcpIp_ZeroCopyTransmit()  - This service transmits data via UDP ZeroCopy API to a remote node. The transmission of the data  */
/*                        is immediately performed within this function call.                                                   */
/*                                                                                                                              */
/* Service ID           - 0x80                                                                                                  */
/* Sync/Async           - Synchronous                                                                                           */
/* Reentrancy           - Non Reentrant                                                                                         */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* SocketId_u16         - Socket identifier of the related local socket resource.                                               */
/* ZeroCopyData_pcst    - Structure containing the ZeroCopy UL data to be transmitted                                           */
/* RemoteAddr_pcst      - IP address and port of the remote host to transmit to.                                                */
/* TxConfFlag_b         - Flag indicating if TxConf need to be monitored (if TRUE, TxId will be returned in out argument).      */
/* ConnChanged_b        - Flag indicating if the connection parameters have changed (new Reuse header will be allocated).       */
/*                                                                                                                              */
/* Out Parameters :                                                                                                             */
/* TxId_pu32            - Out argument storing the TxId (valid only if TxConfFlag_b is set to TRUE).                            */
/*                                                                                                                              */
/* Return value:                                                                                                                */
/* TcpIp_ReturnType    - TCPIP_OK: UDP message has been forwarded to EthIf for transmission.                                    */
/*                       TCPIP_E_NOT_OK: UDP message could not be sent because of a permanent error, e.g. message is too long.  */
/*                       TCPIP_E_ARP_CACHE_MISS: UDP message could not be sent because of an ARP cache miss, ARP request has    */
/*                       been sent and upper layer may retry transmission by calling this function later again.                 */
/********************************************************************************************************************************/
/* HIS METRIC PATH VIOLATION IN TcpIp_ZeroCopyUdpTransmit: The number of path is increased due to the consecutive if blocks which are required for the execution of various DET checks. Deviation is not critical as it has no functional impact and the unit has 100% code coverage. */
TcpIp_ReturnType TcpIp_ZeroCopyUdpTransmit( TcpIp_SocketIdType SocketId_u16,
                                            const Eth_ZeroCopyTxDataInfo_tst * ZeroCopyData_pcst,
                                            const TcpIp_SockAddrType * RemoteAddr_pcst,
                                            boolean TxConfFlag_b,
                                            boolean ConnChanged_b,
                                            uint32 * TxId_pu32 )
{
    /* Declare local variables */
    uint16                              lZeroCopyDescriptorIdx_u16;
    TcpIp_ReturnType                    lTcpIpRetVal_en;
    Std_ReturnType                      lRetVal_en;
    uint32                              lTxId_u32;
    Eth_ZeroCopyArg_tst                 lZeroCopyArg_st;
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    Eth_ZeroCopyTxHeaderInfo_tst        lZeroCopyAllocatedHeaders_ast[1U+TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX+1U]; /* One for reuse header + One per IP fragment + One for UDP variable part */
#else
    Eth_ZeroCopyTxHeaderInfo_tst        lZeroCopyAllocatedHeaders_ast[1U]; /* for reuse header  */
#endif
    uint8                               lAllocatedHeaderCount_u8;
    uint8                               lUL_SockOwnerIdx_u8;
    boolean                             lPreFillRequired_b;

    /* Check for DET errors */
    /* Check whether TcpIp_Init() has been called or not */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_NOTINIT, TCPIP_E_NOT_OK )

    /* Check whether socket Id is valid */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( (TCPIP_UDP_SOCKET_OFFSET > SocketId_u16) || (TCPIP_SOCKETMAX <= SocketId_u16) ), TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_INV_ARG, TCPIP_E_NOT_OK )

    /* Check whether the DataPtr_pu8 is a null pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == ZeroCopyData_pcst ),TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Check whether the RemoteAddrPtr is a null pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == RemoteAddr_pcst ),TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Check whether the TxId_pu32 is a null pointer */
    TCPIP_DET_REPORT_ERROR_RET_VALUE( ( NULL_PTR == TxId_pu32 ),TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_NULL_PTR, TCPIP_E_NOT_OK )

    /* Local variable initialization */
    lTcpIpRetVal_en = TCPIP_E_NOT_OK;
    lTxId_u32 = 0xFFFFFFFFU;

    /* -------------------------------------- */
    /* Get TcpIp ZeroCopy Descriptor          */
    /* -------------------------------------- */

    lZeroCopyDescriptorIdx_u16 = TcpIp_DynSockAllocatedZeroCopyDesc_au16[SocketId_u16];

    /* If the socket already own Zero Copy descriptor */
    if( lZeroCopyDescriptorIdx_u16 < TCPIP_ZEROCOPY_DESCRIPTOR_COUNT )
    {
        lRetVal_en = E_OK;
    }
    /* A Zero Copy descriptor is not already reserved by the socket */
    else
    {
        /* Allocate a descriptor */
        lRetVal_en = TcpIp_AllocateDescriptor( SocketId_u16, &lZeroCopyDescriptorIdx_u16 );
    }

    /* In case the socket own a Zero Copy descriptor and the state is valid for a ZeroCopy transmission */
    if( (lRetVal_en != E_NOT_OK) &&
        (TcpIp_ZeroCopyDescriptors_ast[lZeroCopyDescriptorIdx_u16].DescState_en == TCPIP_ZEROCOPY_DESC_ALLOCATED) )
    {
        /* -------------------------------------- */
        /* Handle connection change               */
        /* -------------------------------------- */

        /* If the connection parameters have changed, then the reuse header need to be invalidated as it cannot be used anymore */
        if( ConnChanged_b != FALSE )
        {
            TcpIp_InvalidateAllocatedHeaders( &TcpIp_ZeroCopyDescriptors_ast[lZeroCopyDescriptorIdx_u16] );
        }

        /* -------------------------------------- */
        /* Allocate headers                       */
        /* -------------------------------------- */

        /* Initialize the allocated header count and PreFill required flag */
        lAllocatedHeaderCount_u8 = 0U;
        lPreFillRequired_b = FALSE;

        /* When new transmit is requested, TcpIp allocates the header buffers and pass them to lower layer */
        lRetVal_en = TcpIp_AllocateHeaders( &TcpIp_ZeroCopyDescriptors_ast[lZeroCopyDescriptorIdx_u16],
                                            ZeroCopyData_pcst->DataLen_u16,
                                            &lZeroCopyAllocatedHeaders_ast[0U],
                                            &lAllocatedHeaderCount_u8,
                                            &lPreFillRequired_b );

        /* If reuse header and one-time header blocks have been successfully get */
        if( lRetVal_en != E_NOT_OK )
        {
            /* -------------------------------------- */
            /* Construct Zero Copy argument           */
            /* -------------------------------------- */

            /* Construct ZeroCopy arg to be passed to Udp layer */
            lZeroCopyArg_st.ZeroCopyAllocatedHeaders_ast = &lZeroCopyAllocatedHeaders_ast[0U]; /* Pointer to the list of Tx headers */
            lZeroCopyArg_st.AllocatedHeaderCount_u8 = lAllocatedHeaderCount_u8;                /* Number of allocted Tx headers */
            lZeroCopyArg_st.ZeroCopyData_pcst = ZeroCopyData_pcst;                             /* Application data */
            lZeroCopyArg_st.TxConfFlag_b = TxConfFlag_b;                                       /* TxConf to be monitored */
            lZeroCopyArg_st.TriggerTx_b = FALSE;                                               /* Initialize trigger tx flag to FALSE */


            /* -------------------------------------- */
            /* Call lower layer                       */
            /* -------------------------------------- */

            /* If the PreFill is required which mean a new Reuse header was allocated */
            if( lPreFillRequired_b != FALSE )
            {
                /* Call PreFill API */
                /* Prefill only the reuse headers */
                lTcpIpRetVal_en = rba_EthUdp_ZeroCopyFillHeader( (SocketId_u16 - TCPIP_UDP_SOCKET_OFFSET),  /* Udp socket id */
                                                              RemoteAddr_pcst,                               /* Remote address */
                                                              &lZeroCopyArg_st,                             /* ZeroCopy argument */
                                                              &lUL_SockOwnerIdx_u8 );                       /* UL socket owner */

                /* If it is a success */
                if( lTcpIpRetVal_en == TCPIP_OK )
                {
                    /* Save the socket owner which will be used for further ZeroCopyTxConfirmation */
                    TcpIp_ZeroCopyDescriptors_ast[lZeroCopyDescriptorIdx_u16].UL_SockOwnerIdx_u8 = lUL_SockOwnerIdx_u8;

                    /* Execute ZeroCopy Tx */
                    lTcpIpRetVal_en = rba_EthUdp_ZeroCopyTransmit( (SocketId_u16 - TCPIP_UDP_SOCKET_OFFSET),  /* Udp socket id */
                                                                    RemoteAddr_pcst,                          /* Remote address */
                                                                    &lZeroCopyArg_st,                         /* ZeroCopy argument */
                                                                    &lTxId_u32 );                             /* TxId returned for this current transmission if the transmission need to be monitored for TxConfirmation */
                }
            }

            /* No PreFill required */
            else
            {
                /* Execute ZeroCopy Tx */
                lTcpIpRetVal_en = rba_EthUdp_ZeroCopyTransmit( (SocketId_u16 - TCPIP_UDP_SOCKET_OFFSET),  /* Udp socket id */
                                                                RemoteAddr_pcst,                           /* Remote address */
                                                                &lZeroCopyArg_st,                         /* ZeroCopy argument */
                                                                &lTxId_u32 );                             /* TxId returned for this current transmission if the transmission need to be monitored for TxConfirmation */
            }

            /* If it is a success */
            if( lTcpIpRetVal_en == TCPIP_OK )
            {
                /* Set TxId to default value in out argument */
                *TxId_pu32 = 0xFFFFFFFFU;

                /* If TxConfFlag_b is TRUE, then the TxId returned in out argument need to be stored and mapped to the allocated buffers */
                /* All already allocated buffers are associated with the TxId (all the previously allocated buffers will be released once this TxId will be confirmed) */
                if( (TxConfFlag_b != FALSE) && (lTxId_u32 != 0xFFFFFFFFU) )
                {
                    TcpIp_StoreTxIdInfo( &TcpIp_ZeroCopyDescriptors_ast[lZeroCopyDescriptorIdx_u16],
                                         lTxId_u32 );

                    /* Copy the TxId for the current transmit request in out argument */
                    *TxId_pu32 = lTxId_u32;
                }
            }
            else
            {
                /* Cancel the header allocation as the transmission did not occur */
                TcpIp_CancelAllocateHeaders( &TcpIp_ZeroCopyDescriptors_ast[lZeroCopyDescriptorIdx_u16],
                                             &lZeroCopyAllocatedHeaders_ast[0U],
                                             lAllocatedHeaderCount_u8,
                                             lPreFillRequired_b );

            }
        }

        /* If failure happened */
        if( lTcpIpRetVal_en != TCPIP_OK )
        {
            /* -------------------------------------- */
            /* Check for descriptor release           */
            /* -------------------------------------- */

            /* Check if the ZeroCopy descriptor is still in use */
            lRetVal_en = TcpIp_ZeroCopyIsResetAllowed( lZeroCopyDescriptorIdx_u16 );

            /* When ZeroCopyTransmission is executed and failure happen (for example Udp returned E_NOT_OK because socket is not yet bound) */
            /* then it need to be checked if the ZeroCopy descriptor can be released */
            /* This is required to avoid that descriptor will remain allocated without any need. */
            if( lRetVal_en == E_OK )
            {
                /* Clear for given TcpIp socket */
                TcpIp_DynSockAllocatedZeroCopyDesc_au16[SocketId_u16] = 0xFFFFU;
                TcpIp_ZeroCopyClearDesc( lZeroCopyDescriptorIdx_u16 );
            }
        }
    }

    return lTcpIpRetVal_en;
}


/********************************************************************************************************************************/
/* TcpIp_ZeroCopyReset()  - This function reset all ZeroCopy structures for a particular socket                                 */
/*                                                                                                                              */
/* Service ID             - 0x81                                                                                                */
/* Sync/Async             - Synchronous                                                                                         */
/* Reentrancy             - Non Reentrant                                                                                       */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* SocketId_u16            - Socket id for which the ZeroCopy structures need to be cleared.                                    */
/*                                                                                                                              */
/* Return  :               - void                                                                                               */
/********************************************************************************************************************************/
void TcpIp_ZeroCopyReset( TcpIp_SocketIdType SocketId_u16 )
{
    /* Declare local variables */
    uint16                              lZeroCopyDescriptorIdx_u16;
    Std_ReturnType                      lRetValStdType_en;

    /* Check for DET errors */
    /* Check whether TcpIp_Init() has been called or not */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_ZEROCOPYRESET_API_ID, TCPIP_E_NOTINIT )

    /* Check whether socket Id is valid */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( (TCPIP_UDP_SOCKET_OFFSET > SocketId_u16) || (TCPIP_SOCKETMAX <= SocketId_u16) ), TCPIP_E_ZEROCOPYRESET_API_ID, TCPIP_E_INV_ARG )

    /* Get the Zero Copy descriptor index */
    lZeroCopyDescriptorIdx_u16 = TcpIp_DynSockAllocatedZeroCopyDesc_au16[SocketId_u16];

    /* If the socket own a ZeroCopy descriptor */
    if( lZeroCopyDescriptorIdx_u16 < TCPIP_ZEROCOPY_DESCRIPTOR_COUNT )
    {
        /* Clear the link between SocketId and ZeroCopy descriptor */
        /* Even if the ZeroCopy descriptor is still in use, the link need to be cleared so that if new transmission need to be executed on the same socket, a new ZeroCopy descriptor will be allocated  */
        TcpIp_DynSockAllocatedZeroCopyDesc_au16[SocketId_u16] = 0xFFFFU;

        /* Check whether the ZeroCopy descriptor is not already CLOSED */
        if( TcpIp_ZeroCopyDescriptors_ast[lZeroCopyDescriptorIdx_u16].DescState_en != TCPIP_ZEROCOPY_DESC_CLOSED )
        {
            /* Set the descriptor state to REQ_CLOSE which means that the descriptor need to be closed */
            /* No more execution of ZeroCopy APIs will be allowed in that state */
            TcpIp_ZeroCopyDescriptors_ast[lZeroCopyDescriptorIdx_u16].DescState_en = TCPIP_ZEROCOPY_DESC_REQ_CLOSE;

            /* Check if the reset of the descriptor is now allowed */
            /* Reset is allowed in this context in case there is no transmission in progress (no allocated header in use) */
            lRetValStdType_en = TcpIp_ZeroCopyIsResetAllowed( lZeroCopyDescriptorIdx_u16 );

            /* If it is permitted to reset the ZeroCopy descriptor in this context */
            if( lRetValStdType_en == E_OK )
            {
                /* Clear for given TcpIp socket */
                TcpIp_ZeroCopyClearDesc( lZeroCopyDescriptorIdx_u16 );
            }
            else
            {
                /* ZeroCopy descriptor is in use in ongoing Tx and reset need to be tried again in next TcpIp MainFunction */
                /* In case headers are allocated and in use for transmission on the Driver level, then we need to wait for the ZeroCopyTxConfirmation */
            }
        }
    }

    return;
}


/********************************************************************************************************************************/
/* TcpIp_ZeroCopyTxConfirmation()  - Callback function invoked when a transmission marked with TxId is completed                */
/*                                   This is a trigger to release the previously allocated headers                              */
/*                                                                                                                              */
/* Service ID              - 0x82                                                                                               */
/* Sync/Async              - Synchronous                                                                                        */
/* Reentrancy              - Non Reentrant                                                                                      */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* EthIfCtrlIdx_u8         - Index of the EthIf controller.                                                                     */
/* TxId_u32                - TxId which identify the transmission which is now completed                                        */
/*                                                                                                                              */
/* Return  :               - void                                                                                               */
/********************************************************************************************************************************/
void TcpIp_ZeroCopyTxConfirmation( uint8 EthIfCtrlIdx_u8,
                                   uint32 TxId_u32 )
{
    /* Declare local variables */
    uint16                              lCurrentIdx_u16;
    boolean                             lFoundFlag_b;

    /* Check for DET errors */
    /* Check whether TcpIp_Init() has been called or not */
    TCPIP_DET_REPORT_ERROR_RET_VOID( ( FALSE == TcpIp_InitFlag_b ), TCPIP_E_ZEROCOPYTXCONFIRMATION_API_ID, TCPIP_E_NOTINIT )

    /* EthIfCtrlIdx_u8 is not used (kept for future use) */
    (void)EthIfCtrlIdx_u8;

    /* Initialize found flag */
    lFoundFlag_b = FALSE;

    /* Search the ZeroCopy descriptor that match with the received TxConfirmation */
    for( lCurrentIdx_u16 = 0U; lCurrentIdx_u16 < TCPIP_ZEROCOPY_DESCRIPTOR_COUNT; lCurrentIdx_u16++ )
    {
        /* If the descriptor is in correct state, socket owner is valid, and oldest stored TxId is valid */
        if( ((TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].DescState_en == TCPIP_ZEROCOPY_DESC_ALLOCATED)  ||
             (TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].DescState_en == TCPIP_ZEROCOPY_DESC_REQ_CLOSE)) &&
             (TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].UL_SockOwnerIdx_u8 != 0xFFU) &&
             (TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].OldestTxIdIdx_u16 != 0xFFFFU) )
        {
            /* If the argument TxId match the oldest saved TxId */
            if( TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].TxIdList_ast[TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].OldestTxIdIdx_u16].TxId_u32 == TxId_u32 )
            {
                /* Release TcpIp allocated headers */
                TcpIp_ReleaseHeaders( &TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16],
                                      TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].OldestTxIdIdx_u16 );

                /* Call UL ZeroCopyTxConfirmation */
                if( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].UL_SockOwnerIdx_u8].Up_ZeroCopyTxConfirmation_pfct != NULL_PTR )
                {
                    TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].UL_SockOwnerIdx_u8].Up_ZeroCopyTxConfirmation_pfct( TcpIp_ZeroCopyDescriptors_ast[lCurrentIdx_u16].LinkedSocket_u16,
                                                                                                                                                                     TxId_u32 );
                }

                lFoundFlag_b = TRUE;

                /* No break statement */
                /* We need to check all the ZeroCopy descriptors in case the same TxId is used in multiple descriptors (possible in case of ZeroCopyMarkLastTx) */
            }
        }
    }

    /* Tx Id in argument is not valid */
    TCPIP_DET_REPORT_ERROR_RET_VOID( (lFoundFlag_b == FALSE), TCPIP_E_ZEROCOPYTXCONFIRMATION_API_ID, TCPIP_E_INV_ARG )

    /* lFoundFlag_b is unused if DET is OFF. */
    (void)lFoundFlag_b;

    return;
}


/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

/********************************************************************************************************************************/
/* TcpIp_ZeroCopyIsResetAllowed()  - This function checks if the reset of a ZeroCopy descriptor is allowed                      */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* DescId_u16              - ZeroCopy descriptor id.                                                                            */
/*                                                                                                                              */
/* Std_ReturnType          - E_OK or E_NOT_OK - Result of operation                                                             */
/********************************************************************************************************************************/
Std_ReturnType TcpIp_ZeroCopyIsResetAllowed( uint16 DescId_u16 )
{
    /* Declare local variables */
    Std_ReturnType                      lRetValStdType_en;
    TcpIp_ZeroCopyDescriptor_tst*       lZeroCopyDescriptor_pst;
    boolean                             lIncompleteTxIdEntry_b;
    uint16                              lSubIdx_u16;
    uint32                              lTxId_u32;

    /* Initialize local variable to E_OK */
    lRetValStdType_en = E_OK;

    /* Get ZeroCopy Descriptor */
    lZeroCopyDescriptor_pst = &TcpIp_ZeroCopyDescriptors_ast[DescId_u16];

    /* --------------------------------------------------- */
    /* Check for ongoing Tx                                */
    /* --------------------------------------------------- */

    /* Check if ongoing ZeroCopy transmission is still in progress on the EthDriver side */
    /* Only Reuse header need to be check for ReuseCntr value because OneTime header are always used along to a Reuse header */
    /* If no Reuse header is used in ongoing transmission, then also no OneTime header is used  */
    for( lSubIdx_u16 = 0U; lSubIdx_u16 < TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF; lSubIdx_u16++ )
    {
        /* If Reuse header is in use */
        if( lZeroCopyDescriptor_pst->ReuseHeaderList_ast[lSubIdx_u16].ReuseCntr_u16 > 0U )
        {
            /* It is not possible to release the ZeroCopy descriptor */
            lRetValStdType_en = E_NOT_OK;
            break;
        }
    }

    /* --------------------------------------------------- */
    /* Check for partially filled TxIdList entry           */
    /* --------------------------------------------------- */

    /* If the current TxIdList entry is open for filling ongoing transmissions information, we need to check if the current TxIdList entry has been already partially filled */
    /* If it is partially filled, that means that transmissions have been executed but the last one is not marked with TxId. */
    /* The TxConfirmation will never be received and ZeroCopy descriptor will be blocked if we do not execute special prcessing */
    if( lZeroCopyDescriptor_pst->TxIdList_ast[lZeroCopyDescriptor_pst->CurrentTxIdIdx_u16].TxId_u32 == 0xFFFFFFFFU )
    {
        /* Set a flag to FALSE */
        lIncompleteTxIdEntry_b = FALSE;

        /* Check if the current TxIdList entry is containing information about Reuse header buffers used in ongoing transmission at Driver level */
        /* Which means that the current TxIdList entry has already been partially filled */
        for( lSubIdx_u16 = 0U; lSubIdx_u16 < TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF; lSubIdx_u16++ )
        {
            if( lZeroCopyDescriptor_pst->TxIdList_ast[lZeroCopyDescriptor_pst->CurrentTxIdIdx_u16].ReuseHeaderCntrMap_au16[lSubIdx_u16] > 0U )
            {
                lIncompleteTxIdEntry_b = TRUE;
                break;
            }
        }

        /* In case there is pending entry in the TxIdList */
        if( lIncompleteTxIdEntry_b != FALSE )
        {
            /* Call API to mark the latest executed transmission on driver level */
            EthIf_ZeroCopyMarkLastTx( TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lZeroCopyDescriptor_pst->LocalAddrId_u8].EthIfCtrlRef_u8,
                                      &lTxId_u32 );

            /* Store the TxId in TcpIp. It will be used for the last ZeroCopyTxConfirmation to release the remaining header buffer */
            TcpIp_StoreTxIdInfo( &TcpIp_ZeroCopyDescriptors_ast[DescId_u16],
                                 lTxId_u32 );
        }
    }
    else
    {
        /* All TxIdList entries in TcpIp are complete and full */
        /* There are not partially filled TxIdList entry and therefore it is ensured that all the allocated headers will received a ZeroCopyTxConfirmation */
        /* It is only required to wait for the final ZeroCopyTxConfirmation */
    }

    return lRetValStdType_en;
}


/********************************************************************************************************************************/
/* TcpIp_ZeroCopyClearDesc()  - This function reset a particular ZeroCopy descriptor                                            */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* DescId_u16              - ZeroCopy descriptor id.                                                                            */
/*                                                                                                                              */
/* Return  :               - void                                                                                               */
/********************************************************************************************************************************/
void TcpIp_ZeroCopyClearDesc( uint16 DescId_u16 )
{
    /* Declare local variables */
    TcpIp_ZeroCopyDescriptor_tst*   lZeroCopyDescriptor_pst;
    uint16                          lSubIdx_u16;
    uint16                          lMapIdx_u16;

    /* Get ZeroCopy Descriptor */
    lZeroCopyDescriptor_pst = &TcpIp_ZeroCopyDescriptors_ast[DescId_u16];

    lZeroCopyDescriptor_pst->LinkedSocket_u16 = 0xFFFFU;

    lZeroCopyDescriptor_pst->LocalAddrId_u8 = 0xFFU;
    lZeroCopyDescriptor_pst->UL_SockOwnerIdx_u8 = 0xFF;

    lZeroCopyDescriptor_pst->CurrentReuseHeaderIdx_u16 = 0U;
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    lZeroCopyDescriptor_pst->NbFreeIpOneTimeHeader_u16 = TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF*TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX;
    lZeroCopyDescriptor_pst->UnconfirmedIpOneTimeHeaderIdx_u16 = 0U;
    lZeroCopyDescriptor_pst->NxtFreeIpOneTimeHeaderIdx_u16 = 0U;

    lZeroCopyDescriptor_pst->NbFreeUdpOneTimeHeader_u16 = TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF;
    lZeroCopyDescriptor_pst->UnconfirmedUdpOneTimeHeaderIdx_u16 = 0U;
    lZeroCopyDescriptor_pst->NxtFreeUdpOneTimeHeaderIdx_u16 = 0U;
#endif
    lZeroCopyDescriptor_pst->OldestTxIdIdx_u16 = 0xFFFFU;
    lZeroCopyDescriptor_pst->CurrentTxIdIdx_u16 = 0U;

    /* Clear reuse headers and Udp one-time headers */
    for( lSubIdx_u16 = 0U; lSubIdx_u16 < TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF; lSubIdx_u16++ )
    {
        lZeroCopyDescriptor_pst->ReuseHeaderList_ast[lSubIdx_u16].ReuseCntr_u16 = 0U;
#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
        lZeroCopyDescriptor_pst->ReuseHeaderList_ast[lSubIdx_u16].LinkedLength_u16 = 0U;
#endif
        lZeroCopyDescriptor_pst->ReuseHeaderList_ast[lSubIdx_u16].Lock_b = FALSE;
        lZeroCopyDescriptor_pst->ReuseHeaderList_ast[lSubIdx_u16].InvalidFlag_b = FALSE;
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
        lZeroCopyDescriptor_pst->UdpOneTimeHeaderList_ast[lSubIdx_u16].Lock_b = FALSE;
#endif
    }
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    /* Clear Ip one-time headers */
    for( lSubIdx_u16 = 0U; lSubIdx_u16 < (TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF*TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX); lSubIdx_u16++ )
    {
        lZeroCopyDescriptor_pst->IpOneTimeHeaderList_ast[lSubIdx_u16].Lock_b = FALSE;
    }
#endif
    /* Clear TxIdList */
    for( lSubIdx_u16 = 0U; lSubIdx_u16 < TCPIP_ZEROCOPY_MAX_MONITORED_TXCONF; lSubIdx_u16++ )
    {
        lZeroCopyDescriptor_pst->TxIdList_ast[lSubIdx_u16].TxId_u32 = 0xFFFFFFFFU;
        for( lMapIdx_u16 = 0U; lMapIdx_u16 < TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF; lMapIdx_u16++ )
        {
            lZeroCopyDescriptor_pst->TxIdList_ast[lSubIdx_u16].ReuseHeaderCntrMap_au16[lMapIdx_u16] = 0U;
        }
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
        lZeroCopyDescriptor_pst->TxIdList_ast[lSubIdx_u16].EndIpOneTimeHeaderIdx_u16 = 0U;
        lZeroCopyDescriptor_pst->TxIdList_ast[lSubIdx_u16].EndUdpOneTimeHeaderIdx_u16 = 0U;
#endif
    }

    /* Set the descriptor state to CLOSED */
    lZeroCopyDescriptor_pst->DescState_en = TCPIP_ZEROCOPY_DESC_CLOSED;

    return;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/********************************************************************************************************************************/
/* TcpIp_AllocateDescriptor()  - This function reset a particular ZeroCopy descriptor                                           */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* SocketId_u16                         - Socket id for which a new ZeroCopy descriptor need to be allocated.                   */
/* AllocatedZeroCopyDescriptor_pu16     - Index of the allocated ZeroCopy descriptor.                                           */
/*                                                                                                                              */
/* Std_ReturnType          - E_OK or E_NOT_OK - Result of operation                                                             */
/********************************************************************************************************************************/
LOCAL_INLINE Std_ReturnType TcpIp_AllocateDescriptor( TcpIp_SocketIdType SocketId_u16,
                                                      uint16 * AllocatedZeroCopyDescriptor_pu16 )
{
    /* Declare local variables */
    Std_ReturnType              lRetValStdType_en;
    Std_ReturnType              lRetValSocketInfo_en;
    uint16                      lDescIdx_u16;
    TcpIp_LocalAddrIdType       lLocalAddrId_u8;
    uint16                      lLocalPort_u16;

    /* Local variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* Allocate a descriptor */
    for( lDescIdx_u16 = 0U; lDescIdx_u16 < TCPIP_ZEROCOPY_DESCRIPTOR_COUNT; lDescIdx_u16++ )
    {
        /* Current descriptor is not reserved by any socket */
        if( TcpIp_ZeroCopyDescriptors_ast[lDescIdx_u16].DescState_en == TCPIP_ZEROCOPY_DESC_CLOSED )
        {
            /* Get the local address id and local port that is bound to SocketId */
            lRetValSocketInfo_en = rba_EthUdp_GetSocketInfo( (SocketId_u16 - TCPIP_UDP_SOCKET_OFFSET),
                                                             &lLocalAddrId_u8,
                                                             &lLocalPort_u16 );

            if( lRetValSocketInfo_en == E_OK )
            {

                /* If the ZeroCopy mode is STATIC or DYNAMIC, it is allowed to allocated a descriptor */
                /* In case of TCPIP_ZEROCOPY_DISABLED, ZeroCopy transmission on that local address are disabled */
                if(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[lLocalAddrId_u8].ZeroCopyTxModeType_en != TCPIP_ZEROCOPY_DISABLED)
                {
                    /* Make the link between socket table and the allocated Zero Copy descriptor table */
                    TcpIp_ZeroCopyDescriptors_ast[lDescIdx_u16].LinkedSocket_u16 = SocketId_u16;
                    TcpIp_DynSockAllocatedZeroCopyDesc_au16[SocketId_u16] = lDescIdx_u16;

                    /* Store the local address id */
                    TcpIp_ZeroCopyDescriptors_ast[lDescIdx_u16].LocalAddrId_u8 = lLocalAddrId_u8;

                    /* Set the descriptor to allocated state */
                    TcpIp_ZeroCopyDescriptors_ast[lDescIdx_u16].DescState_en = TCPIP_ZEROCOPY_DESC_ALLOCATED;

                    /* Set out argument */
                    *AllocatedZeroCopyDescriptor_pu16 = lDescIdx_u16;
                    lRetValStdType_en = E_OK;
                }
                else
                {
                    /* Report DET as ZeroCopy transmission is called on a local address which is configured with TCPIP_ZEROCOPY_DISABLED */
                    TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_INV_ARG )
                }
            }
            break;
        }

    }

    return lRetValStdType_en;
}


/********************************************************************************************************************************/
/* TcpIp_InvalidateAllocatedHeaders()  - This function invalidates previously allocated Reuse headers when connection parameter */
/*                                     changes                                                                                  */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst  - Pointer to the ZeroCopy descriptor structure.                                                      */
/*                                                                                                                              */
/* Return  :               - void                                                                                               */
/********************************************************************************************************************************/
LOCAL_INLINE void TcpIp_InvalidateAllocatedHeaders( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst )
{
    /* Declare local variables */
    uint16              lLoopIdx_u16;

    /* If the connection changes, then the Reuse headers used until now are not valid anymore. Therefore, set the invalid flag */
    /* The ReuseCntr is not reset to 0 as only the ZeroCopyTxConfirmation can release completely the allocated header */
    for( lLoopIdx_u16 = 0U; lLoopIdx_u16 < TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF; lLoopIdx_u16++ )
    {
        /* If the Reuse header is locked (means it was filled) */
        if(ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].Lock_b != FALSE)
        {
            /* If ReuseCntr is greater than 0 which means the header is currently used in ongoing Tx */
            if(ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].ReuseCntr_u16 > 0U)
            {
                /* Set invalid flag */
                /* The invalid flag will be reset and the header released in the next ZeroCopyTxConfirmation */
                ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].InvalidFlag_b = TRUE;
            }

            /* If ReuseCntr is 0 which means that the header is filled but not used in outgoing Tx */
            else
            {
                /* Release completely the header */
                ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].Lock_b = FALSE;
            }
        }
    }

    return;
}


/********************************************************************************************************************************/
/* TcpIp_AllocateHeaders()     - This function allocates necessary Reuse header and OneTime headers needed for                  */
/*                                     executing a ZeroCopy transmission                                                        */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst         - Pointer to the ZeroCopy descriptor structure.                                               */
/* DataLength_u16                 - Length of UL data to be transmitted.                                                        */
/*                                                                                                                              */
/* Out Parameters :                                                                                                             */
/* ZeroCopyAllocatedHeaders_past  - Pointer to a list of structure containing the allocated header buffer information.          */
/* AllocatedHeaderCount_pu8       - Number of allocated header buffers.                                                         */
/* PreFillRequired_pb             - Boolean indicating if the Reuse header need to be newly filled.                             */
/*                                                                                                                              */
/* Std_ReturnType                 - E_OK or E_NOT_OK - Result of operation                                                      */
/********************************************************************************************************************************/
LOCAL_INLINE Std_ReturnType TcpIp_AllocateHeaders( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                                   uint16 DataLength_u16,
                                                   Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_past,
                                                   uint8 * AllocatedHeaderCount_pu8,
                                                   boolean * PreFillRequired_pb )
{
    /* Declare local variables */
    Std_ReturnType                      lRetValStdType_en;
    uint16                              lFetchedReuseHeader_u16;
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    Std_ReturnType                      lRetVal_en;
    uint8                               lNumberOfIpHeaders_u8;
#endif

    /* Local variable initialization */
    lRetValStdType_en = E_NOT_OK;

    /* If the current TxIdList entry is open for filling ongoing transmissions information */
    if( ZeroCopyDescriptor_pst->TxIdList_ast[ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16].TxId_u32 == 0xFFFFFFFFU )
    {
        /* --------------------------------------------------- */
        /* Search Reuse header                                 */
        /* --------------------------------------------------- */
        lFetchedReuseHeader_u16 = TcpIp_SearchReuseHeader(ZeroCopyDescriptor_pst, DataLength_u16, PreFillRequired_pb);

        /* Check if Reuse Header is valid */
        if (lFetchedReuseHeader_u16 < TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF)
        {
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
            if(TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[ZeroCopyDescriptor_pst->LocalAddrId_u8].ZeroCopyTxModeType_en == TCPIP_ZEROCOPY_DYNAMIC_MODE)
            {
                /* ---------------------------------------------------- */
                /* Count the number of required one-time headers        */
                /* ---------------------------------------------------- */

                /* Initialize the number of IP headers to 0 */
                lNumberOfIpHeaders_u8 = 0U;

                /* Check if there will be fragmentation and get how many IPv4 fragments will be present in the frame */
                lRetVal_en = rba_EthUdp_CheckFragmentation( (ZeroCopyDescriptor_pst->LinkedSocket_u16 - TCPIP_UDP_SOCKET_OFFSET),
                                                            DataLength_u16,
                                                            &lNumberOfIpHeaders_u8 );

                if(lRetVal_en == E_OK)
                {
                    /* In Dynamic Mode: */
                    /* Check if there is enough IP one-time header buffers available in the ZeroCopyDescriptor and */
                    /* Check if there is enough UDP one-time header buffers available in the ZeroCopyDescriptor */
                    /* Reason: The IP and UDP one-time header are only needed in Dynamic Mode as fragmentaion is ON and Checksum is also computed by software */
                    if( (lNumberOfIpHeaders_u8 <= TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX)  &&                 /* Number of IP header to allocate does not exceed TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX */
                        (ZeroCopyDescriptor_pst->NbFreeIpOneTimeHeader_u16 >= lNumberOfIpHeaders_u8) &&      /* As much buffer as Ip fragments */
                        (ZeroCopyDescriptor_pst->NbFreeUdpOneTimeHeader_u16 >= 1U ))        /* One buffer required for Udp header part */
                    {
                        /* Allocate reuse header */
                        TcpIp_AllocateReuseHeader( ZeroCopyDescriptor_pst,
                                                   lFetchedReuseHeader_u16,
                                                   DataLength_u16,
                                                   &ZeroCopyAllocatedHeaders_past[0U],
                                                   AllocatedHeaderCount_pu8 );

                        /* Allocate IP OneTime headers */
                        TcpIp_AllocateIpOneTimeHeader( ZeroCopyDescriptor_pst,
                                                       lNumberOfIpHeaders_u8,
                                                       &ZeroCopyAllocatedHeaders_past[0U],
                                                       AllocatedHeaderCount_pu8 );

                        /* Allocate UDP OneTime header */
                        TcpIp_AllocateUdpOneTimeHeader( ZeroCopyDescriptor_pst,
                                                        &ZeroCopyAllocatedHeaders_past[0U],
                                                        AllocatedHeaderCount_pu8 );

                        lRetValStdType_en = E_OK;
                    }
                    else
                    {
                        /* Report DET as there is no header buffer anymore */
                        /* UL should not transmit more than the configured number of header buffers */
                        TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_INV_ARG )
                    }
                }
            }
            else
#endif
            {
#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
                /* Allocate reuse header */
                TcpIp_AllocateReuseHeader( ZeroCopyDescriptor_pst,
                                           lFetchedReuseHeader_u16,
                                           DataLength_u16,
                                           &ZeroCopyAllocatedHeaders_past[0U],
                                           AllocatedHeaderCount_pu8 );
                lRetValStdType_en = E_OK;
#endif
            }

        }
        else
        {
            /* Report DET as there is no header buffer anymore */
            /* UL should not transmit more than the configured number of header buffers */
            TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_INV_ARG )
        }
    }
    else
    {
        /* Report DET as there is no memory to monitor the current transmission */
        /* UL should not monitor transmission for TxConfirmation more than TCPIP_ZEROCOPY_MAX_MONITORED_TXCONF */
        TCPIP_DET_REPORT_ERROR_TRUE_NO_RET( TCPIP_E_ZEROCOPYTRANSMIT_API_ID, TCPIP_E_INV_ARG )
    }

    return lRetValStdType_en;
}

/********************************************************************************************************************************/
/* TcpIp_SearchReuseHeader()    - Search the Reuse Header index for Static and Dynamic Mode Type                                */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst       - Pointer to the ZeroCopy descriptor structure.                                                 */
/* DataLength_u16               - Length of UL data to be transmitted.                                                          */
/*                                                                                                                              */
/* Output Parameters :                                                                                                          */
/* PreFillRequired_pb           - Boolean indicating if the Reuse header need to be newly filled.                               */
/*                                                                                                                              */
/* Return                       - uint16                                                                                        */
/********************************************************************************************************************************/
LOCAL_INLINE uint16 TcpIp_SearchReuseHeader( const TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pcst,
                                             uint16 DataLength_u16,
                                             boolean * PreFillRequired_pb )
{
    /* Declare local variables */
    uint16                              lCurrentReuseHeaderIdx_u16;
    uint16                              lLoopIdx_u16;
    uint16                              lFetchedReuseHeader_u16;
    uint8                               lFoundPrio_u8;

#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_OFF)
    (void)DataLength_u16;
#endif

    /* Initialize search variables */
    lFetchedReuseHeader_u16 = 0xFFFFU;
    lFoundPrio_u8 = 0xFFU;

    /******************************* Priority of match ***************************************/
    /* Prio 1 In Dynamic, reuse header valid and locked is found */
    /* Prio 1 In Static reuse header valid and locked and same linked length is found */
    /* Prio 2 In Dynamic, reuse header valid and it is not locked then set prefill to True  */
    /* Prio 2 In Static reuse header valid and not locked set prefill */
    /* Prio 3 In Static reuse header valid and locked, different linked length is found and Reuse counter is 0 set prefill to True*/

    /* Search a Reuse header from the current reuse header and length associated with the Reuse Header */
    /* MR12 RULE 15.4 VIOLATION: The break instruction is used for speed optimization. */
    lCurrentReuseHeaderIdx_u16 = ZeroCopyDescriptor_pcst->CurrentReuseHeaderIdx_u16;
    for( lLoopIdx_u16 = 0U; lLoopIdx_u16 < TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF; lLoopIdx_u16++ )
    {
        if( lLoopIdx_u16 > 0U)
        {
            lCurrentReuseHeaderIdx_u16 = TcpIp_IncrIndexWithWrapAround( lCurrentReuseHeaderIdx_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF );
        }

        /* If the Reuse header is invalid which means that it cannot be used */
        if( ZeroCopyDescriptor_pcst->ReuseHeaderList_ast[lCurrentReuseHeaderIdx_u16].InvalidFlag_b != FALSE)
        {
            continue;
        }

        /* If the Reuse header is locked which means it is already prefilled */
        if( ZeroCopyDescriptor_pcst->ReuseHeaderList_ast[lCurrentReuseHeaderIdx_u16].Lock_b != FALSE )
        {
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
            /* Check the Zero Copy tranmission Mode is Static Mode or not */
            if (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[ZeroCopyDescriptor_pcst->LocalAddrId_u8].ZeroCopyTxModeType_en == TCPIP_ZEROCOPY_DYNAMIC_MODE)
            {
                /* Prio 1 DYNAMIC */
                /* Matching Reuse header is found */
                /* Set the PreFill header out argument to FALSE because the Reuse header is already filled */
                lFetchedReuseHeader_u16 = lCurrentReuseHeaderIdx_u16;
                *PreFillRequired_pb = FALSE;
                break;
            }
            else
#endif
            {
#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
                /* If the Mode Type is Static than check the length linked with the reuse header is same as the Data length */
                if(ZeroCopyDescriptor_pcst->ReuseHeaderList_ast[lCurrentReuseHeaderIdx_u16].LinkedLength_u16 == DataLength_u16)
                {
                    /* Prio 1 STATIC */

                    /* Matching Reuse header is found */
                    /* No need to prefill the Reuse header again */
                    lFetchedReuseHeader_u16 = lCurrentReuseHeaderIdx_u16;
                    *PreFillRequired_pb = FALSE;
                    break;
                }
                /* Check the current reuse header indes is used in on going transnmission */
                else if(ZeroCopyDescriptor_pcst->ReuseHeaderList_ast[lCurrentReuseHeaderIdx_u16].ReuseCntr_u16 == 0U)
                {
                    /* Prio 3 STATIC */
                    /* If an unused reuse header index is already found do not overwrite it with index of reuse header that is already filled */
                    if( lFoundPrio_u8 > 3U )
                    {
                        /* Matching Reuse header is found */
                        /* Set the PreFill header out argument to TRUE because the Reuse header need to be newly filled */
                        lFetchedReuseHeader_u16 = lCurrentReuseHeaderIdx_u16;
                        *PreFillRequired_pb = TRUE;
                        lFoundPrio_u8 = 3U;
                    }
                }
                else
                {
                    /* There is ongoing tx with the current reuse header index so it cannot be reallocated */
                }
#endif
            }
        }
        /* In both Static and Dynamic Mode if Reuse header is not locked it can be used for transmission with prefill set to TRUE */
        else
        {
            /* Prio 2 DYNAMIC */
            /* Prio 2 STATIC */

            /* In Static Mode continue to find the Reuse Header Index with the header already filled and Data length same as the Reuse Header length */
            /* In Dynamic Mode continue to find the Reuse Header Index with the header already filled*/

            if( lFoundPrio_u8 > 2U )
            {
                /* Matching Reuse header is found */
                /* Set the PreFill header out argument to TRUE because the Reuse header need to be newly filled */
                lFetchedReuseHeader_u16 = lCurrentReuseHeaderIdx_u16;
                *PreFillRequired_pb = TRUE;
                lFoundPrio_u8 = 2U;
            }

        }
    }
    return lFetchedReuseHeader_u16;
}

/********************************************************************************************************************************/
/* TcpIp_AllocateReuseHeader() - This function executes the allocation of the Reuse header                                      */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst           - Pointer to the ZeroCopy descriptor structure.                                             */
/* FetchedReuseHeader_u16           - Index of the Reuse header to be allocated.                                                */
/* DataLength_u16                    - Length of UL data to be transmitted.                                                         */
/*                                                                                                                              */
/* Out Parameters :                                                                                                             */
/* ZeroCopyAllocatedHeaders_past    - Pointer to a list of structure containing the allocated header buffer information.        */
/* AllocatedHeaderCount_pu8         - Number of allocated header buffers.                                                       */
/*                                                                                                                              */
/* Return  :                        - void                                                                                      */
/********************************************************************************************************************************/
LOCAL_INLINE void TcpIp_AllocateReuseHeader( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                             uint16 FetchedReuseHeader_u16,
                                             uint16 DataLength_u16,
                                             Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_past,
                                             uint8 * AllocatedHeaderCount_pu8 )
{
    /* Allocate */
    ZeroCopyDescriptor_pst->ReuseHeaderList_ast[FetchedReuseHeader_u16].Lock_b = TRUE;
    ZeroCopyDescriptor_pst->ReuseHeaderList_ast[FetchedReuseHeader_u16].ReuseCntr_u16++;

    /* Update currently used header */
    ZeroCopyDescriptor_pst->CurrentReuseHeaderIdx_u16 = FetchedReuseHeader_u16;

#if (TCPIP_ZEROCOPY_STATIC_MODE_PRESENT == STD_ON)
    /* Update the current Packet data length in currently used header */
    ZeroCopyDescriptor_pst->ReuseHeaderList_ast[FetchedReuseHeader_u16].LinkedLength_u16 = DataLength_u16;
#else
    (void)DataLength_u16;
#endif

    /* Construct out argument for Reuse header buffer */
    ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderId_u16 = FetchedReuseHeader_u16;
    ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderPartBuffer_pu8 = ZeroCopyDescriptor_pst->ReuseHeaderList_ast[FetchedReuseHeader_u16].HeaderPart_au8;
    if (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[ZeroCopyDescriptor_pst->LocalAddrId_u8].ZeroCopyTxModeType_en == TCPIP_ZEROCOPY_DYNAMIC_MODE)
    {
        ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderPartLen_u8 = 36U; /* Reuse header block is 36 bytes long */
    }
    else
    {
        ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderPartLen_u8 = 46U; /* Reuse header block is 46 bytes long */
    }
    (*AllocatedHeaderCount_pu8)++;

    /* Update TxId entry */
    ZeroCopyDescriptor_pst->TxIdList_ast[ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16].ReuseHeaderCntrMap_au16[FetchedReuseHeader_u16]++;

    return;
}

/********************************************************************************************************************************/
/* TcpIp_AllocateIpOneTimeHeader() - This function executes the allocation of the Ip OneTime header                             */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst           - Pointer to the ZeroCopy descriptor structure.                                             */
/* NumberOfIpHeaders_u8             - Number of Ip OneTime headers to be allocated.                                             */
/*                                                                                                                              */
/* Out Parameters :                                                                                                             */
/* ZeroCopyAllocatedHeaders_past    - Pointer to a list of structure containing the allocated header buffer information.        */
/* AllocatedHeaderCount_pu8         - Number of allocated header buffers.                                                       */
/*                                                                                                                              */
/* Return  :                        - void                                                                                      */
/********************************************************************************************************************************/
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
LOCAL_INLINE void TcpIp_AllocateIpOneTimeHeader( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                                 uint8 NumberOfIpHeaders_u8,
                                                 Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_past,
                                                 uint8 * AllocatedHeaderCount_pu8 )
{
    /* Declare local variables */
    uint16                              lCurrentIpOneTimeHeader_u16;
    uint8                               lCurrentIndex_u8;

    /* Compute the start one-time header */
    lCurrentIpOneTimeHeader_u16 = ZeroCopyDescriptor_pst->NxtFreeIpOneTimeHeaderIdx_u16;

    /* For all the one-time header blocks that need to be allocated */
    for( lCurrentIndex_u8 = 0U; lCurrentIndex_u8 < NumberOfIpHeaders_u8; lCurrentIndex_u8++)
    {
        /* If we are at the first iteration, no incrementation need to be done as we need to process the start index */
        if( lCurrentIndex_u8 > 0U)
        {
            lCurrentIpOneTimeHeader_u16 = TcpIp_IncrIndexWithWrapAround( lCurrentIpOneTimeHeader_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF*TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX);
        }

        /* Allocate */
        ZeroCopyDescriptor_pst->IpOneTimeHeaderList_ast[lCurrentIpOneTimeHeader_u16].Lock_b = TRUE;
        ZeroCopyDescriptor_pst->NbFreeIpOneTimeHeader_u16--;

        /* Construct out argument for Ip OneTime header buffer */
        ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderId_u16 = lCurrentIpOneTimeHeader_u16;
        ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderPartBuffer_pu8 = ZeroCopyDescriptor_pst->IpOneTimeHeaderList_ast[lCurrentIpOneTimeHeader_u16].HeaderPart_au8;
        ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderPartLen_u8 = 6U;  /* IP OneTime header block is 6 bytes long */
        (*AllocatedHeaderCount_pu8)++;
    }

    /* Update TxId entry */
    ZeroCopyDescriptor_pst->TxIdList_ast[ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16].EndIpOneTimeHeaderIdx_u16 = lCurrentIpOneTimeHeader_u16;

    /* Update next free header */
    ZeroCopyDescriptor_pst->NxtFreeIpOneTimeHeaderIdx_u16 = TcpIp_IncrIndexWithWrapAround( lCurrentIpOneTimeHeader_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF*TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX);

    return;
}
#endif


/********************************************************************************************************************************/
/* TcpIp_AllocateUdpOneTimeHeader() - This function executes the allocation of the Udp OneTime header                           */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst           - Pointer to the ZeroCopy descriptor structure.                                             */
/*                                                                                                                              */
/* Out Parameters :                                                                                                             */
/* ZeroCopyAllocatedHeaders_past    - Pointer to a list of structure containing the allocated header buffer information.        */
/* AllocatedHeaderCount_pu8         - Number of allocated header buffers.                                                       */
/*                                                                                                                              */
/* Return  :                        - void                                                                                      */
/********************************************************************************************************************************/
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
LOCAL_INLINE void TcpIp_AllocateUdpOneTimeHeader( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                                  Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_past,
                                                  uint8 * AllocatedHeaderCount_pu8 )
{
    /* Declare local variables */
    uint16                              lFetchedUdpOneTimeHeader_u16;

    /* Compute the next one-time header */
    lFetchedUdpOneTimeHeader_u16 = ZeroCopyDescriptor_pst->NxtFreeUdpOneTimeHeaderIdx_u16;

    /* Allocate */
    ZeroCopyDescriptor_pst->UdpOneTimeHeaderList_ast[lFetchedUdpOneTimeHeader_u16].Lock_b = TRUE;
    ZeroCopyDescriptor_pst->NbFreeUdpOneTimeHeader_u16--;

    /* Construct out argument for UDP one-time header buffer */
    ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderId_u16 = lFetchedUdpOneTimeHeader_u16;
    ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderPartBuffer_pu8 = ZeroCopyDescriptor_pst->UdpOneTimeHeaderList_ast[lFetchedUdpOneTimeHeader_u16].HeaderPart_au8;
    ZeroCopyAllocatedHeaders_past[(*AllocatedHeaderCount_pu8)].HeaderPartLen_u8 = 4U;  /* UDP OneTime header block is 4 bytes long */
    (*AllocatedHeaderCount_pu8)++;

    /* Update TxId entry */
    ZeroCopyDescriptor_pst->TxIdList_ast[ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16].EndUdpOneTimeHeaderIdx_u16 = lFetchedUdpOneTimeHeader_u16;

    /* Update next free header */
    ZeroCopyDescriptor_pst->NxtFreeUdpOneTimeHeaderIdx_u16 = TcpIp_IncrIndexWithWrapAround( lFetchedUdpOneTimeHeader_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF);

    return;
}
#endif

/********************************************************************************************************************************/
/* TcpIp_CancelAllocateHeaders()  - This function cancel the allocated header in case the current Tx is a failure               */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst         - Pointer to the ZeroCopy descriptor structure.                                               */
/* ZeroCopyAllocatedHeaders_past  - Pointer to a list of structure containing the allocated header buffer information.          */
/* AllocatedHeaderCount_u8        - Number of allocated header buffers.                                                         */
/* PreFillRequired_b              - Boolean indicating if the Reuse header was required to be newly filled.                     */
/*                                                                                                                              */
/* Return  :                      - void                                                                                        */
/********************************************************************************************************************************/
LOCAL_INLINE void TcpIp_CancelAllocateHeaders( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                               const Eth_ZeroCopyTxHeaderInfo_tst * ZeroCopyAllocatedHeaders_pacst,
                                               uint8 AllocatedHeaderCount_u8,
                                               boolean PreFillRequired_b )
{
    /* Declare local variables */
    uint16                      lReuseHeader_u16;
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    uint16                      lIpOneTimeHeaderStart_u16;
    uint16                      lIpOneTimeHeaderStop_u16;
    uint16                      lUdpOneTimeHeader_u16;
    uint16                      lCurrentIpOneTimeHeaderIdx_u16;
    uint8                       lLoopIdx_u8;
#endif
    /* Fetch the Reuse header id */
    lReuseHeader_u16 = ZeroCopyAllocatedHeaders_pacst[0U].HeaderId_u16;                                      /* Reuse header is at index 0 */

    /* -------------------------------------- */
    /* Cancel Reuse header allocation         */
    /* -------------------------------------- */

    /* If PreFill of the header was done */
    if( PreFillRequired_b != FALSE )
    {
        /* Deallocate the reuse header buffer */
        /* If PreFill was required, then the allocated header need to be unlocked otherwise invalid header might be reused in the next transmission (possible if rba_EthUdp_ZeroCopyFillHeader returned E_NOT_OK) */
        ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lReuseHeader_u16].Lock_b = FALSE;
    }

    /* Decrement the Reuse counter */
    ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lReuseHeader_u16].ReuseCntr_u16--;
    ZeroCopyDescriptor_pst->TxIdList_ast[ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16].ReuseHeaderCntrMap_au16[lReuseHeader_u16]--;

#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    if (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[ZeroCopyDescriptor_pst->LocalAddrId_u8].ZeroCopyTxModeType_en == TCPIP_ZEROCOPY_DYNAMIC_MODE)
    {
        /* Fetch the Ip one time and Udp one time header ids */
        lIpOneTimeHeaderStart_u16 = ZeroCopyAllocatedHeaders_pacst[1U].HeaderId_u16;                         /* Ip one time headers start at index 1 */
        lIpOneTimeHeaderStop_u16 = ZeroCopyAllocatedHeaders_pacst[AllocatedHeaderCount_u8-2U].HeaderId_u16;
        lUdpOneTimeHeader_u16 = ZeroCopyAllocatedHeaders_pacst[AllocatedHeaderCount_u8-1U].HeaderId_u16;     /* Udp one time headers is at last index */

        /* -------------------------------------- */
        /* Cancel Ip OneTime headers allocation   */
        /* -------------------------------------- */

        /* Cancel allocated IP one-time header buffers */
        lCurrentIpOneTimeHeaderIdx_u16 = lIpOneTimeHeaderStart_u16;
        for( lLoopIdx_u8 = 0U; ((lLoopIdx_u8 == 0U) || (lCurrentIpOneTimeHeaderIdx_u16 != lIpOneTimeHeaderStop_u16)); lLoopIdx_u8++ )
        {
            /* If we are at the first iteration, no incrementation need to be done as we need to process the start index */
            if( lLoopIdx_u8 > 0U)
            {
                lCurrentIpOneTimeHeaderIdx_u16 = TcpIp_IncrIndexWithWrapAround( lCurrentIpOneTimeHeaderIdx_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF*TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX );
            }

            /* Deallocate the header buffer */
            ZeroCopyDescriptor_pst->IpOneTimeHeaderList_ast[lCurrentIpOneTimeHeaderIdx_u16].Lock_b = FALSE;
            ZeroCopyDescriptor_pst->NbFreeIpOneTimeHeader_u16++;
        }

        /* Restore the next free header */
        ZeroCopyDescriptor_pst->NxtFreeIpOneTimeHeaderIdx_u16 = lIpOneTimeHeaderStart_u16;

        /* -------------------------------------- */
        /* Cancel Udp OneTime header allocation   */
        /* -------------------------------------- */

        /* Deallocate the UDP one-time header buffer */
        ZeroCopyDescriptor_pst->UdpOneTimeHeaderList_ast[lUdpOneTimeHeader_u16].Lock_b = FALSE;
        ZeroCopyDescriptor_pst->NbFreeUdpOneTimeHeader_u16++;

        /* Restore the next free header */
        ZeroCopyDescriptor_pst->NxtFreeUdpOneTimeHeaderIdx_u16 = lUdpOneTimeHeader_u16;
    }
#endif
    return;
}


/********************************************************************************************************************************/
/* TcpIp_StoreTxIdInfo()  - This function store the TxId related to current transmission                                        */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst   - Pointer to the ZeroCopy descriptor structure.                                                     */
/* TxId_u32                 - TxId identifying the current ZeroCopy transmission.                                               */
/*                                                                                                                              */
/* Return  :                - void                                                                                              */
/********************************************************************************************************************************/
LOCAL_INLINE void TcpIp_StoreTxIdInfo( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                       uint32 TxId_u32 )
{
    /* Store the indicated TxId along with the allocated buffers information in the TxIdList */
    ZeroCopyDescriptor_pst->TxIdList_ast[ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16].TxId_u32 = TxId_u32;

    /* If OldestTxIdIdx is not set */
    if( ZeroCopyDescriptor_pst->OldestTxIdIdx_u16 == 0xFFFFU )
    {
        ZeroCopyDescriptor_pst->OldestTxIdIdx_u16 = ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16;
    }

    /* Compute the next TxId entry to be used */
    ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16 = TcpIp_IncrIndexWithWrapAround( ZeroCopyDescriptor_pst->CurrentTxIdIdx_u16, TCPIP_ZEROCOPY_MAX_MONITORED_TXCONF);

    return;
}


/********************************************************************************************************************************/
/* TcpIp_ReleaseHeaders()  - This function release the allocated headers until the transmission marked with TxId                */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* ZeroCopyDescriptor_pst   - Pointer to the ZeroCopy descriptor structure.                                                     */
/* TxId_u32                 - TxId identifying the ZeroCopy transmission until all ZeroCopy header need to be released.         */
/*                                                                                                                              */
/* Return  :                - void                                                                                              */
/********************************************************************************************************************************/
LOCAL_INLINE void TcpIp_ReleaseHeaders( TcpIp_ZeroCopyDescriptor_tst * ZeroCopyDescriptor_pst,
                                        uint16 TxIdIdx_u16 )
{
    /* Declare local variables */
    uint16                              lLoopIdx_u16;
    uint16                              lNextTxIdIdx_u16;
#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    uint16                              lReleaseIdx_u16;
#endif

    /* -------------------------------------- */
    /* Release reuse headers                  */
    /* -------------------------------------- */

    /* Decrement the Reuse header Cntr by the values saved in the ReuseHeaderCntrMap_au16 */
    for( lLoopIdx_u16 = 0U; lLoopIdx_u16 < TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF; lLoopIdx_u16++ )
    {
        /* Decrement the ReuseCntr */
        if( ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].ReuseCntr_u16 >= ZeroCopyDescriptor_pst->TxIdList_ast[TxIdIdx_u16].ReuseHeaderCntrMap_au16[lLoopIdx_u16] )
        {
            ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].ReuseCntr_u16 -= ZeroCopyDescriptor_pst->TxIdList_ast[TxIdIdx_u16].ReuseHeaderCntrMap_au16[lLoopIdx_u16];
        }
        else
        {
            ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].ReuseCntr_u16 = 0U;
        }

        /* Reset the ReuseHeaderCntrMap_au16 for particular index to default value */
        ZeroCopyDescriptor_pst->TxIdList_ast[TxIdIdx_u16].ReuseHeaderCntrMap_au16[lLoopIdx_u16] = 0U;

        /* If the ReuseCntr reach 0 and the Invalid flag is set to TRUE */
        /* In case the Invalid flag is FALSE, the reuse header should not be unlocked as it might be still needed for further Rx (the filled content is still valid) */
        if( (ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].ReuseCntr_u16 == 0U) &&
            (ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].InvalidFlag_b == TRUE) )
        {
            /* Remove the lock only if it has invalid flag set */
            ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].Lock_b = FALSE;
            ZeroCopyDescriptor_pst->ReuseHeaderList_ast[lLoopIdx_u16].InvalidFlag_b = FALSE;
        }
    }

#if (TCPIP_ZEROCOPY_DYNAMIC_MODE_PRESENT == STD_ON)
    if (TcpIp_CurrConfig_pco->EthLocalAddrConfig_paco[ZeroCopyDescriptor_pst->LocalAddrId_u8].ZeroCopyTxModeType_en == TCPIP_ZEROCOPY_DYNAMIC_MODE)
    {
        /* -------------------------------------- */
        /* Release IP one-time headers            */
        /* -------------------------------------- */

        /* All reuse IP one-time header allocated before the call of TcpIp_ZeroCopyTransmit() with TxConf flag set TRUE need to be released */
        lReleaseIdx_u16 = ZeroCopyDescriptor_pst->UnconfirmedIpOneTimeHeaderIdx_u16;
        for( lLoopIdx_u16 = 0U; ((lLoopIdx_u16 == 0U) || (lReleaseIdx_u16 != ZeroCopyDescriptor_pst->TxIdList_ast[TxIdIdx_u16].EndIpOneTimeHeaderIdx_u16)); lLoopIdx_u16++ )
        {
            /* If we are at the first iteration, no incrementation need to be done as we need to process the start index */
            if( lLoopIdx_u16 > 0U)
            {
                /* Move to next reuse header in the release range */
                lReleaseIdx_u16 = TcpIp_IncrIndexWithWrapAround( lReleaseIdx_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF*TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX );
            }

            /* If the current element is locked */
            if( ZeroCopyDescriptor_pst->IpOneTimeHeaderList_ast[lReleaseIdx_u16].Lock_b != FALSE )
            {
                /* One time header is now assumed as unlocked */
                ZeroCopyDescriptor_pst->IpOneTimeHeaderList_ast[lReleaseIdx_u16].Lock_b = FALSE;
                ZeroCopyDescriptor_pst->NbFreeIpOneTimeHeader_u16++;
            }

            /* Move the unconfirmed reuse header index to the next one-time header index */
            ZeroCopyDescriptor_pst->UnconfirmedIpOneTimeHeaderIdx_u16 = TcpIp_IncrIndexWithWrapAround( ZeroCopyDescriptor_pst->UnconfirmedIpOneTimeHeaderIdx_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF*TCPIP_ZEROCOPY_MAX_IP_FRAGMENT_PER_TX );
        }

        /* -------------------------------------- */
        /* Release UDP one-time headers           */
        /* -------------------------------------- */

        /* All reuse UDP one-time header allocated before the call of TcpIp_ZeroCopyTransmit() with TxConf flag set TRUE need to be released */
        lReleaseIdx_u16 = ZeroCopyDescriptor_pst->UnconfirmedUdpOneTimeHeaderIdx_u16;
        for( lLoopIdx_u16 = 0U; ((lLoopIdx_u16 == 0U) || (lReleaseIdx_u16 != ZeroCopyDescriptor_pst->TxIdList_ast[TxIdIdx_u16].EndUdpOneTimeHeaderIdx_u16)); lLoopIdx_u16++ )
        {
            /* If we are at the first iteration, no incrementation need to be done as we need to process the start index */
            if( lLoopIdx_u16 > 0U)
            {
                /* Move to next reuse header in the release range */
                lReleaseIdx_u16 = TcpIp_IncrIndexWithWrapAround( lReleaseIdx_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF );
            }

            /* If the current element is locked */
            if( ZeroCopyDescriptor_pst->UdpOneTimeHeaderList_ast[lReleaseIdx_u16].Lock_b != FALSE )
            {
                /* One time header is now assumed as unlocked */
                ZeroCopyDescriptor_pst->UdpOneTimeHeaderList_ast[lReleaseIdx_u16].Lock_b = FALSE;
                ZeroCopyDescriptor_pst->NbFreeUdpOneTimeHeader_u16++;
            }

            /* Move the unconfirmed reuse header index to the next one-time header index */
            ZeroCopyDescriptor_pst->UnconfirmedUdpOneTimeHeaderIdx_u16 = TcpIp_IncrIndexWithWrapAround( ZeroCopyDescriptor_pst->UnconfirmedUdpOneTimeHeaderIdx_u16, TCPIP_ZEROCOPY_MAX_TX_BEFORE_CONF );
        }
    }
#endif

    /* -------------------------------------- */
    /* Clear TxId entry                       */
    /* -------------------------------------- */

    /* Clean TxId */
    ZeroCopyDescriptor_pst->TxIdList_ast[TxIdIdx_u16].TxId_u32 = 0xFFFFFFFFU;

    /* Recompute oldest TxId index */
    lNextTxIdIdx_u16 = TcpIp_IncrIndexWithWrapAround( ZeroCopyDescriptor_pst->OldestTxIdIdx_u16, TCPIP_ZEROCOPY_MAX_MONITORED_TXCONF);
    ZeroCopyDescriptor_pst->OldestTxIdIdx_u16 = (ZeroCopyDescriptor_pst->TxIdList_ast[lNextTxIdIdx_u16].TxId_u32 != 0xFFFFFFFFU) ? (lNextTxIdIdx_u16) : (0xFFFFU);

    return;
}


/********************************************************************************************************************************/
/* TcpIp_IncrIndexWithWrapAround()  - Incrementation function with wrap around condition                                        */
/*                                                                                                                              */
/* Input Parameters :                                                                                                           */
/* IndexToIncr_u16              - Value to be incremented.                                                                      */
/* WrapAroundCondition_u16      - Value for which wrap around will occurs.                                                      */
/*                                                                                                                              */
/* Return  :                    - uint16                                                                                          */
/********************************************************************************************************************************/
LOCAL_INLINE uint16 TcpIp_IncrIndexWithWrapAround( uint16 IndexToIncr_u16,
                                                   uint16 WrapAroundCondition_u16 )
{
    /* Declare local variables */
    uint16          lIncrementedIndex_u16;

    lIncrementedIndex_u16 = IndexToIncr_u16 + 1U;
    if( lIncrementedIndex_u16 >= WrapAroundCondition_u16 )
    {
        lIncrementedIndex_u16 = 0U;
    }

    return lIncrementedIndex_u16;
}

#define TCPIP_STOP_SEC_CODE_FAST
#include "TcpIp_MemMap.h"

#endif  /* TCPIP_ZEROCOPY_TX_SUPPORT */
#endif  /* TCPIP_UDP_ENABLED */

#endif /* #ifdef TCPIP_CONFIGURED */
