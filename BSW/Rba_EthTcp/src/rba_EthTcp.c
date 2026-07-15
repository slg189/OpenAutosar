

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) )

#if( TCPIP_TLS_ENABLED == STD_ON )
#include "rba_EthTls.h"
#endif/* TCPIP_TLS_ENABLED == STD_ON */

#include "TcpIp_Prv.h"

#include "rba_EthTcp.h"
#include "rba_EthTcp_Prv.h"


/*
 ***************************************************************************************************
 * Global variables
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_VAR_INIT_8
#include "rba_EthTcp_MemMap.h"
/* Flag to indicate if Tcp_Init has been executed or not since power ON */
boolean rba_EthTcp_InitFlag_b = FALSE;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_8
#include "rba_EthTcp_MemMap.h"

#define RBA_ETHTCP_START_SEC_VAR_INIT_16
#include "rba_EthTcp_MemMap.h"
/* In case port parameter is specified as TCPIP_PORT_ANY, the TCP/IP stack shall choose the local port automatically */
/* from the range 49152(0xC000) to 65535(0xFFFF) and shall update the parameter to the chosen value */
uint16 rba_EthTcp_PortAny_u16 = TCPIP_LOCAL_PORT_ANY_START;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_16
#include "rba_EthTcp_MemMap.h"

#define RBA_ETHTCP_START_SEC_VAR_CLEARED_16
#include "rba_EthTcp_MemMap.h"
/* Total number of opened Tcp sockets */
uint16 rba_EthTcp_SockConnCntr_u16 = 0U;
#define RBA_ETHTCP_STOP_SEC_VAR_CLEARED_16
#include "rba_EthTcp_MemMap.h"

#define RBA_ETHTCP_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthTcp_MemMap.h"
/* Pointer to the currently used Tcp configuration */
const rba_EthTcp_Config_to * rba_EthTcp_CurrConfig_cpo = NULL_PTR;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthTcp_MemMap.h"

#if ( RBA_ETHTCP_ISN_CLOCK_DRIVEN_ENABLED == STD_ON || RBA_ETHTCP_ISN_CLOCK_HASH_BASED_ENABLED == STD_ON )
#define RBA_ETHTCP_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthTcp_MemMap.h"
/* Context used for the generation of the Initial Sequence Number */
rba_EthTcp_IsnGeneratorContext_tst rba_EthTcp_IsnContext_st;
#define RBA_ETHTCP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthTcp_MemMap.h"
#endif


/*
 ***************************************************************************************************
 * Static function declaration
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"
static void rba_EthTcp_PrepareNextFrame( TcpIp_SocketIdType TcpDynSockTblIdx_uo );
#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"


/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */

#define RBA_ETHTCP_START_SEC_CODE
#include "rba_EthTcp_MemMap.h"

/*********************************************************************************************************************************/
/** rba_EthTcp_Init()   - By this API, the rba_EthTcp module is initialised if the configuration pointer is valid               **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpConfig_cpo       - Pointer to the configuration data of the rba_EthTcp module                                            **/
/**                                                                                                                             **/
/** Parameters (inout):   None                                                                                                  **/
/**                                                                                                                             **/
/** Parameters (out):     None                                                                                                  **/
/**                                                                                                                             **/
/** Return value:         None                                                                                                  **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_Init( const rba_EthTcp_Config_to * TcpConfig_cpo )
{
    /* Declare local variables */
    TcpIp_SocketIdType lIdx_uo;

    /* Check for DET errors */
    /* Report DET if TcpConfig_cpo is NULL pointer */
    RBA_ETHTCP_CHECK_DETERROR_VOID( (TcpConfig_cpo == NULL_PTR), RBA_ETHTCP_E_INIT, RBA_ETHTCP_E_NULL_PTR );

    /* -------------------------------------- */
    /* Initialize ISN generator               */
    /* -------------------------------------- */
#if ( RBA_ETHTCP_ISN_CLOCK_DRIVEN_ENABLED == STD_ON || RBA_ETHTCP_ISN_CLOCK_HASH_BASED_ENABLED == STD_ON )
    rba_EthTcp_InitIsnGenerator();
#endif

    /* ---------------------------------------*/
    /* Initialize Tcp configuration pointer   */
    /* -------------------------------------- */
     rba_EthTcp_CurrConfig_cpo = TcpConfig_cpo;

    /* -------------------------------------- */
    /* Initialize Tcp sockets                 */
    /* -------------------------------------- */

    /* Loop through all Tcp sockets */
    for ( lIdx_uo = 0U; lIdx_uo < TCPIP_TCPSOCKETMAX; lIdx_uo++ )
    {
        /* Initialize the dynamic table entry of the Tcp socket */
        rba_EthTcp_ResetTcpTableEntry( lIdx_uo );
    }

    /* -------------------------------------- */
    /* Initialize Reorder buffers             */
    /* -------------------------------------- */
#if (RBA_ETHTCP_REORDER_ENABLED == STD_ON)
    rba_EthTcp_ReorderBufferInit();
#endif

    /* -------------------------------------- */
    /* Initialize global variables            */
    /* -------------------------------------- */

    /* Initialise the rba_EthTcp_MeasurementData_u32 */
#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
    rba_EthTcp_MeasurementData_u32 = 0U;
#endif/* (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON) */

    /* Reset Tcp socket connection counter */
    rba_EthTcp_SockConnCntr_u16 = 0U;

    /* Set Tcp to initialized state */
    rba_EthTcp_InitFlag_b = TRUE;

    return;
}


/*********************************************************************************************************************************/
/** rba_EthTcp_MainFunction - By this API, the rba_EthTcp module is requested to handle the TCP process and send TCP frame to   **/
/**                         rba_EthIpv4 submodule                                                                               **/
/**                                                                                                                             **/
/** Parameters (in):          None                                                                                              **/
/**                                                                                                                             **/
/** Parameters (inout):       None                                                                                              **/
/**                                                                                                                             **/
/** Parameters (out):         None                                                                                              **/
/**                                                                                                                             **/
/** Return value:             None                                                                                              **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
void rba_EthTcp_MainFunction( void )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
    TcpIp_SocketIdType              lIdx_uo;
#if(( TCPIP_TLS_ENABLED == STD_ON ) && ( RBA_ETHTCP_TXWNDSIZE > 0 ))
    Std_ReturnType                  lRetVal_en;
#endif

    /* Check for DET errors */
    /* Report DET if TcpConfig_cpo is NULL pointer */
    RBA_ETHTCP_CHECK_DETERROR_VOID( (rba_EthTcp_InitFlag_b == FALSE), RBA_ETHTCP_E_MAINFUNCTION, RBA_ETHTCP_E_NOTINIT );

    /* -------------------------------------- */
    /* Increment ISN generator                */
    /* -------------------------------------- */
#if ( RBA_ETHTCP_ISN_CLOCK_DRIVEN_ENABLED == STD_ON || RBA_ETHTCP_ISN_CLOCK_HASH_BASED_ENABLED == STD_ON)
    rba_EthTcp_IncrIsnGenerator();
#endif

    /* -------------------------------------- */
    /* Loop through all Tcp sockets           */
    /* -------------------------------------- */
    for ( lIdx_uo = 0U; lIdx_uo < TCPIP_TCPSOCKETMAX; lIdx_uo++ )
    {
        /* Get Tcp dynamic table entry at index lIdx_u8 */
        lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[lIdx_uo] ) );

        /* Process further only if socket is opened for communication or reserved to send RESET frame */
        if(( lTcpDynSockTbl_pst->SockState_en == RBA_ETHTCP_SOCK_OPENED_E )  || ( lTcpDynSockTbl_pst->SockState_en == RBA_ETHTCP_SOCK_RESERVED_E ))
        {
            /* -------------------------------------- */
            /* Increment timer of the Tcp socket      */
            /* -------------------------------------- */
            rba_EthTcp_IncrTimer( lIdx_uo );

            /* --------------------------------------- */
            /* Handle frame sending for the Tcp socket */
            /* --------------------------------------- */
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
            /* Tcp frame with payload needs to be sent */
            if ( (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_DATA_E)             || (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_ZWP_E) )
            {
                rba_EthTcp_SndData( lIdx_uo);
            }
#endif
            /* Tcp frame without payload needs to be sent (only the Tcp header) */
            if ( (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_SYN_E)              || (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_SYN_ACK_E)            ||
                 (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_ACK_FOR_SYN_ACK_E)  || (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_ACK_E)                ||
                 (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_FIN_ACK_E)          || (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_ACK_FOR_FIN_ACK_E)    ||
                 (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_RST_E)              || (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_RST_ACK_E)            ||
                 (lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_SEND_KEEP_ALIVE_E) )
            {
                rba_EthTcp_SndFlag( lIdx_uo);
            }

            /* -------------------------------------- */
            /* If needed, prepare next frame to send  */
            /* -------------------------------------- */

            /* Prepare the next frame to be sent by Tcp */
            rba_EthTcp_PrepareNextFrame( lIdx_uo );

#if(( TCPIP_TLS_ENABLED == STD_ON ) && ( RBA_ETHTCP_TXWNDSIZE > 0 ))
            /* Check if Secure connection is requested and Tcp buffer is empty*/
            if((lTcpDynSockTbl_pst->TlsRequestedConnId_uo < RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE)) /*&& \
               (RBA_ETHTCP_TXWNDSIZE == (rba_EthTcp_GetTxBufFreeLen(lIdx_uo))) )*/
            {
                if((RBA_ETHTCP_TXWNDSIZE == rba_EthTcp_GetTxBufFreeLen(lIdx_uo)))
            {
                /* Trigger the rba_EthTls handshake sequence by calling TcpIp_TlsOpenConnection API */
                 lRetVal_en = rba_EthTls_OpenConnection(lIdx_uo, &lTcpDynSockTbl_pst->TlsRequestedConnId_uo);

                 /* If rba_EthTls open connection is successful */
                 if(E_OK == lRetVal_en )
                 {
                     /* assign the rba_Ethtls connection id in dynamic table for future access */
                     lTcpDynSockTbl_pst->TlsConnectionId_uo =  lTcpDynSockTbl_pst->TlsRequestedConnId_uo;

                     lTcpDynSockTbl_pst->TlsRequestedConnId_uo = RBA_ETHTCP_TLS_CONN_ID_DEFAULT_VALUE;
                 }

            }
            }
#endif/*(( TCPIP_TLS_ENABLED == STD_ON ) && ( RBA_ETHTCP_TXWNDSIZE > 0 )) */
        }
    }

    return;
}


/*
 ***************************************************************************************************
 * Static functions
 ***************************************************************************************************
 */

/*********************************************************************************************************************************/
/** rba_EthTcp_PrepareNextFrame - This function prepare the next frame to be sent by Tcp                                        **/
/**                                                                                                                             **/
/** Parameters (in):                                                                                                            **/
/** TcpDynSockTblIdx_uo         - Tcp dynamic socket table index                                                                **/
/**                                                                                                                             **/
/** Parameters (inout):           None                                                                                          **/
/**                                                                                                                             **/
/** Parameters (out):             None                                                                                          **/
/**                                                                                                                             **/
/** Return value:                 None                                                                                          **/
/**                                                                                                                             **/
/*********************************************************************************************************************************/
static void rba_EthTcp_PrepareNextFrame( TcpIp_SocketIdType TcpDynSockTblIdx_uo )
{
    /* Declare local variables */
    rba_EthTcp_DynSockTblType_tst * lTcpDynSockTbl_pst;
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
    uint16                          lRemainDataLen_u16;
    uint16                          lUnackedDataLen_u16;
#endif

    /* Get Tcp dynamic table entry at index lIdx_u8 */
    lTcpDynSockTbl_pst = ( & ( rba_EthTcp_DynSockTbl_ast[TcpDynSockTblIdx_uo] ) );

    /* If no frame need to be sent in the next MainFunction, then it is needed to prepare the next frame if any */
    if( lTcpDynSockTbl_pst->TcpFlag_en == RBA_ETHTCP_NOTHING_E )
    {
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
        /* Get how much data in Tcp Tx buffer is ready to be sent */
        lRemainDataLen_u16 = rba_EthTcp_GetTxBufSndLen( TcpDynSockTblIdx_uo );

        /* Get the length of unacked data in the Tcp Tx buffer */
        lUnackedDataLen_u16 = rba_EthTcp_GetTxBufUnackedLen( TcpDynSockTblIdx_uo );
#endif

        /* -------------------------------------- */
        /* Prepare SYN                            */
        /* -------------------------------------- */

        /* If a SYN is postponed */
        if( lTcpDynSockTbl_pst->PostponeSyn_b != FALSE )
        {
            /* Set the sequence number and acknowledgement number of the next frame to be sent */
            rba_EthTcp_SetSeqNum( TcpDynSockTblIdx_uo, lTcpDynSockTbl_pst->IniSndSeqNum_u32 );
            rba_EthTcp_SetAckNum( TcpDynSockTblIdx_uo, 0U );

            /* Send a SYN in the next MainFunction */
            (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_SYN_E );
        }

        /* -------------------------------------- */
        /* Prepare FIN-ACK                        */
        /* -------------------------------------- */

        /* If connection state is ESTABLISHED or CLOSE_WAIT and a FIN is postponed */
        if( ((lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E) || (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSE_WAIT_E)) &&
             (lTcpDynSockTbl_pst->PostponeFin_b != FALSE) )
        {
#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
            /* Check if the FIN ACK can be sent in the next main function */
            /* (FIN ACK can only be sent after all the data have been sent and acknowledged) */
#if ( RBA_ETHTCP_RETX_ENABLED == STD_ON )
            if ( (lRemainDataLen_u16 == 0U) && (lUnackedDataLen_u16 == 0U) )
#else
            if ( lRemainDataLen_u16 == 0U )
#endif
#endif
            {
                /* We can send our FIN ACK in the next MainFunction */
                /* (There is no more data to be sent) */
                (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_FIN_ACK_E );
            }
        }

#if ( RBA_ETHTCP_TXWNDSIZE > 0 )
        /* -------------------------------------- */
        /* Prepare DATA                           */
        /* -------------------------------------- */

        /* Check if DATA can be sent in the next main function */
        if ( ((lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_ESTABLISHED_E) || (lTcpDynSockTbl_pst->State_en == RBA_ETHTCP_CONN_CLOSE_WAIT_E)) &&
             (lRemainDataLen_u16 > 0U)                                              &&          /* DATA pending to be sent */
             (lTcpDynSockTbl_pst->RcvdWndSize_u16 > lUnackedDataLen_u16) )                      /* The window size of the remote is greater than the length of unacknowledged data */
        {
            /* Send DATA in the next MainFunction */
            (void)rba_EthTcp_SetNextSendFrame( TcpDynSockTblIdx_uo, RBA_ETHTCP_SEND_DATA_E );
        }
#endif
    }

    return;
}


#define RBA_ETHTCP_STOP_SEC_CODE
#include "rba_EthTcp_MemMap.h"

#endif /* #if ( defined(TCPIP_TCP_ENABLED) && ( TCPIP_TCP_ENABLED == STD_ON ) ) */
