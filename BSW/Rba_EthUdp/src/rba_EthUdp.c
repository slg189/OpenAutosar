
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )
#include "rba_EthUdp.h"
#include "rba_EthUdp_Cfg_SchM.h"

#include "TcpIp_Prv.h"
#include "rba_EthUdp_Prv.h"

#if ( TCPIP_DEV_ERROR_DETECT != STD_OFF )
#include "Det.h"
#if (!defined(DET_AR_RELEASE_MAJOR_VERSION) || (DET_AR_RELEASE_MAJOR_VERSION != TCPIP_AR_RELEASE_MAJOR_VERSION))
#error "DET AUTOSAR major version undefined or mismatched"
#endif
#if (!defined(DET_AR_RELEASE_MINOR_VERSION) || (DET_AR_RELEASE_MINOR_VERSION != TCPIP_AR_RELEASE_MINOR_VERSION))
#error "DET AUTOSAR minor version undefined or mismatched"
#endif
#endif /* ( TCPIP_DEV_ERROR_DETECT != STD_OFF ) */


/*
 ***************************************************************************************************
 * Global Variables
 ***************************************************************************************************
 */

#define RBA_ETHUDP_START_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthUdp_MemMap.h"

const rba_EthUdp_Config_to *      rba_EthUdp_Config_pco = NULL_PTR;   /* Pointer will be initialized in the Init Function */
uint16                            rba_EthUdp_UsedSocCnt_u16 = 0U;
uint16                            rba_EthUdp_PortAny_u16 = TCPIP_LOCAL_PORT_ANY_START;

#define RBA_ETHUDP_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "rba_EthUdp_MemMap.h"

/*
 ***************************************************************************************************
 * Interface functions
 ***************************************************************************************************
 */
#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

/********************************************************************************************************************/
/* rba_EthUdp_Init()            This function Initializes the rba_EthUdp module                                     */
/*                                                                                                                  */
/* Synchronous, Non Reentrant (from [SWS_TcpIp_00002])                                                              */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* CurrConfig_pcst              Configuration data structure for rba_EthUdp module                                  */
/*                                                                                                                  */
/* Return type :                void                                                                                */
/********************************************************************************************************************/
void rba_EthUdp_Init( const rba_EthUdp_Config_to * CurrConfig_pcst )
{
    /* Local variable declaration */
    TcpIp_SocketIdType                     lSocIdx_u16;

    /* Report DET if TcpIp_CurrConfig_pco is not intialized */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( ( NULL_PTR == TcpIp_CurrConfig_pco ),
                                          RBA_ETHUDP_E_INIT_API_ID, RBA_ETHUDP_E_INIT_FAILED )

    /* Report DET if CurrConfig_pcst does not contain a valid configuration */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID( ( CurrConfig_pcst != ( TcpIp_CurrConfig_pco->EthUdpConfig_pco) ),
                                          RBA_ETHUDP_E_INIT_API_ID, RBA_ETHUDP_E_INV_ARG )

   /* Initialize socket table */
   for(lSocIdx_u16 = 0; lSocIdx_u16 < TCPIP_UDPSOCKETMAX; lSocIdx_u16++)
   {
       /* Reset socket properties to initial value */
       rba_EthUdp_ResetSockProperties( lSocIdx_u16 );
   }

    /* Reset EthUdp measurement data variable */
#if ( TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
    rba_EthUdp_InvalidIpPortDropCnt_u32 = 0UL;
#endif

    /* Initialize global variables */
    rba_EthUdp_PortAny_u16       = TCPIP_LOCAL_PORT_ANY_START;
    rba_EthUdp_UsedSocCnt_u16    = 0U;
    rba_EthUdp_Config_pco        = CurrConfig_pcst;
}


/********************************************************************************************************************/
/* rba_EthUdp_MainFunction()    By this API, the rba_EthUdp module handle the connection closing scenario           */
/*                                                                                                                  */
/* Scheduled function (from [SWS_TcpIp_00026])                                                                      */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* CurrConfig_pcst              None                                                                                */
/*                                                                                                                  */
/* Return type :                void                                                                                */
/********************************************************************************************************************/
void rba_EthUdp_MainFunction( void )
{
    /* Local variable declaration */
    TcpIp_SocketIdType         lSocIdx_u16;
    Std_ReturnType             lRetVal_en;

    /* Checks: Report DET if rba_EthUdp module is uninitialised */
    RBA_ETHUDP_DET_REPORT_ERROR_RET_VOID((NULL_PTR == rba_EthUdp_Config_pco), RBA_ETHUDP_E_MAINFUNCTION_API_ID, RBA_ETHUDP_E_NOTINIT)

   /* Loop through all socket to execute closing if required */
   for( lSocIdx_u16 = 0U; lSocIdx_u16 < TCPIP_UDPSOCKETMAX; lSocIdx_u16++ )
   {
       /* If the current socket is in REQ_CLOSE state */
       if( rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].SockState_en == RBA_ETHUDP_SOCK_REQ_CLOSE )
       {
           /* Check if closing of the socket is possible now */
           /* No other operation on the socket should be in progress in the other execution contexts in order to close the socket now */
           lRetVal_en = rba_EthUdp_IsCloseOperationAllowed( lSocIdx_u16 );

           /* Check whether the socket closing is allowed to be executed now */
           if( lRetVal_en == E_OK )
           {
               /* Inform upper layer about closing of socket, if Up_TcpIpEvent() API is configured for the socket owner. */
               if( TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].UL_SockOwnerIdx_u8].Up_TcpIpEvent_pfct != NULL_PTR )
               {
                   TcpIp_CurrConfig_pco->SockOwnerConfig_pacst[rba_EthUdp_DynSockTbl_ast[lSocIdx_u16].UL_SockOwnerIdx_u8].Up_TcpIpEvent_pfct( TCPIP_UDP_SOCKET_OFFSET + lSocIdx_u16,
                                                                                                                                               TCPIP_UDP_CLOSED );
               }

#if( TCPIP_ZEROCOPY_TX_SUPPORT == STD_ON )
                /* Call TcpIp callback to close the ZeroCopy descriptor */
                /* Required if the Udp socket was used by at least one ZeroCopy transmission to release the allocated TcpIp descriptor */
                TcpIp_ZeroCopyReset( (lSocIdx_u16 + TCPIP_UDP_SOCKET_OFFSET) );
#endif

               /* Enter critical section */
               /* (Decrementation is not an atomic operation and need to be done in a critical section) */
               SchM_Enter_rba_EthUdp_ExclusiveAreaUsedSocCnt();

               /* Decrement used socket counter by 1 */
               /* This instruction need to be executed before the socket is set to CLOSED state in order to avoid out of boundary value for rba_EthUdp_UsedSocCnt_u16 in case socket closing is executed concurently */
               rba_EthUdp_UsedSocCnt_u16--;

               /* Exit critical section */
               SchM_Exit_rba_EthUdp_ExclusiveAreaUsedSocCnt();

               /* Reset socket properties to initial value */
               rba_EthUdp_ResetSockProperties( lSocIdx_u16 );
           }
       }
   }
}

#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#endif /* #if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */
