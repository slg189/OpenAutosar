
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
 */

#include "TcpIp.h"

#if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) )
#include "rba_EthUdp.h"
#include "rba_EthUdp_Cfg_SchM.h"
#include "rba_EthUdp_Prv.h"

#define RBA_ETHUDP_START_SEC_CODE
#include "rba_EthUdp_MemMap.h"

/*
 ***************************************************************************************************
 * Private functions
 ***************************************************************************************************
 */

/********************************************************************************************************************/
/* rba_EthUdp_ActivateOngoingOperation()    This function increments the OperationOngoing counter by 1              */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16             - Socket identifier of the related local socket resource.                               */
/*                                                                                                                  */
/* Std_ReturnType           - E_OK or E_NOT_OK - Result of operation                                                */
/********************************************************************************************************************/
Std_ReturnType rba_EthUdp_ActivateOngoingOperation( TcpIp_SocketIdType SocketId_u16 )
{
    /* Local variable declaration */
    Std_ReturnType          lRetValStdType_en;

    /* Initialize local variable to E_NOT_OK */
    lRetValStdType_en = E_NOT_OK;

    /* Enter critical section (to avoid race condition if rba_EthUdp_IsCloseOperationAllowed is executed concurently) */
    SchM_Enter_rba_EthUdp_ExclusiveAreaOngoingOperation();

    /* Check whether the socket is not already CLOSED, REQ_CLOSE or CLOSING */
    if( (rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en != RBA_ETHUDP_SOCK_CLOSED) &&
        (rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en != RBA_ETHUDP_SOCK_REQ_CLOSE) &&
        (rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en != RBA_ETHUDP_SOCK_CLOSING) )
    {
        /* Check if the OperationOngoing counter is not at the uint8 boundary */
        if(rba_EthUdp_DynSockTbl_ast[SocketId_u16].OperationOngoing_u8 < 0xFFU )
        {
            /* Increment the number of ongoing operations */
            /* A counter need to be mainted in order to know if other APIs using global socket resources are executed concurrently */
            /* Each time an API which uses global socket resources is entered or re-entered, the counter is incremented (for example, RxIndication can be reentered so we need to know how many time API was reentered to release the OperationOngoing information) */
            /* Hint: The counter will never reach the uint8 boundary because rba_EthUdp_RxIndication() cannot be reentered more than 8 times and rba_EthUdp_Close() cannot be reentered more than 2 times */
            rba_EthUdp_DynSockTbl_ast[SocketId_u16].OperationOngoing_u8++;
            lRetValStdType_en = E_OK;
        }
    }

    /* Exit critical section */
    SchM_Exit_rba_EthUdp_ExclusiveAreaOngoingOperation();

    return lRetValStdType_en;
}


/********************************************************************************************************************/
/* rba_EthUdp_ReleaseOngoingOperation()     This function decrements the OperationOngoing counter by 1 if the       */
/*                                          ActiveStatus in argument is E_OK (which mean the operation activation   */
/*                                          was a success)                                                          */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16             - Socket identifier of the related local socket resource.                               */
/*                                                                                                                  */
/* Return type :            void                                                                                    */
/********************************************************************************************************************/
void rba_EthUdp_ReleaseOngoingOperation( TcpIp_SocketIdType SocketId_u16 )
{
    /* Enter critical section */
    SchM_Enter_rba_EthUdp_ExclusiveAreaOngoingOperation();

    /* Decrement the operation ongoing flag */
    if(rba_EthUdp_DynSockTbl_ast[SocketId_u16].OperationOngoing_u8 > 0U )
    {
        /* Decrementation is not an atomic operation and need to be done in a exclusive area */
        rba_EthUdp_DynSockTbl_ast[SocketId_u16].OperationOngoing_u8--;
    }

    /* Exit critical section */
    SchM_Exit_rba_EthUdp_ExclusiveAreaOngoingOperation();
}


/********************************************************************************************************************/
/* rba_EthUdp_IsCloseOperationAllowed()    This function put the socket in CLOSING state and return the information */
/*                                         whether the socket can be released immediately or if the releasing need  */
/*                                         to be delayed (due to another ongoing operation on the same socket)      */
/*                                                                                                                  */
/* Synchronous, Reentrant                                                                                           */
/*                                                                                                                  */
/* Input Parameters :                                                                                               */
/* SocketId_u16             - Socket identifier of the related local socket resource.                               */
/*                                                                                                                  */
/* Std_ReturnType           - E_OK or E_NOT_OK - Flag indicating if closing of the socket is permitted              */
/********************************************************************************************************************/
Std_ReturnType rba_EthUdp_IsCloseOperationAllowed( TcpIp_SocketIdType SocketId_u16 )
{
    /* Local variable declaration */
    Std_ReturnType          lRetValStdType_en;

    /* Initialize local variable to E_NOT_OK */
    lRetValStdType_en = E_NOT_OK;

    /* Enter critical section (to avoid race condition if rba_EthUdp_ActivateOngoingOperation is executed concurently) */
    SchM_Enter_rba_EthUdp_ExclusiveAreaOngoingOperation();

    /* Check whether the socket is not already CLOSED or CLOSING */
    if( (rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en != RBA_ETHUDP_SOCK_CLOSED) &&
        (rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en != RBA_ETHUDP_SOCK_CLOSING) )
    {
        /* Set the socket state to REQ_CLOSE */
        /* No execution of rba_EthUdp API will be allowed in that state */
        rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en = RBA_ETHUDP_SOCK_REQ_CLOSE;

        /* Check if no other rba_EthUdp APIs which uses global socket resources are executed concurrently */
        if( rba_EthUdp_DynSockTbl_ast[SocketId_u16].OperationOngoing_u8 == 0U )
        {
            /* Closing of the socket and releasing the resources now is possible */
            rba_EthUdp_DynSockTbl_ast[SocketId_u16].SockState_en = RBA_ETHUDP_SOCK_CLOSING;
            lRetValStdType_en = E_OK;
        }
        else
        {
            /* Another operation is already ongoing, the closing of the socket is not possible for the moment (will be executed in next MainFunction) */
        }
    }

    /* Exit critical section */
    SchM_Exit_rba_EthUdp_ExclusiveAreaOngoingOperation();

    return lRetValStdType_en;
}

#define RBA_ETHUDP_STOP_SEC_CODE
#include "rba_EthUdp_MemMap.h"

#endif /* #if ( defined(TCPIP_UDP_ENABLED) && ( TCPIP_UDP_ENABLED == STD_ON ) ) */
