/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



/* Rule BSW_HeaderInc_009: Protection against Multiple Inclusion */
#ifndef TCPIP_CFG_SCHM_H
#define TCPIP_CFG_SCHM_H

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/

/* Inline function prototypes */
/* The exclusive area ExclusiveAreaCurrAsgnedAddrInProcess is used to avoid race condition during CurrAsgnedAddr State change. */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess( void );
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess( void );

/* The exclusive area ExclusiveAreaRandNumGenerating is used to avoid race condition if TcpIp_RandNumXor64 is reentered. */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaRandNumGenerating( void );
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaRandNumGenerating( void );

/* The exclusive area ExclusiveAreaSockOperations is used to avoid race condition if Binding a socket or sending data and Terminate all sockets is called at the same time. */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaSockOperations( void );
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaSockOperations( void );

/* The exclusive area ExclusiveAreaReqRelAddrAssign is used to avoid race conditions whenever a new request or release IP assignment method is triggered. */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign( void );
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign( void );

/* The exclusive area ExclusiveAreaReqRelAddrAssign is used to avoid race conditions whenever a requested assignment priority is being accessed by TcpIp_LocalIpAddrAssignmentChg() in different execution contexts. */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaRequestedAssignPrioAccess( void );
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaRequestedAssignPrioAccess( void );

/* The exclusive area ExclusiveAreaLocalIPChgParamAccess is used to avoid race conditions whenever a Local IP change parameters are being accessed or modified in APIs TcpIp_Mainfunction() and TcpIp_LocalIpAddrAssignmentChg(). */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaLocalIPChgParamAccess( void );
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaLocalIPChgParamAccess( void );

/* The exclusive area ExclusiveAreaManualStaticIPAddrAccess is used to avoid race conditions whenever a Manual Static IP address parameters are being accessed or modified in APIs TcpIp_Mainfunction() and TcpIp_RequestIpAddrAssignment(). */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaManualStaticIPAddrAccess( void );
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaManualStaticIPAddrAccess( void );

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
    /* The AdressUnassignmentOngoing is used to avoid race condition during CurrAsgnedAddr State change */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaCurrAsgnedAddrInProcess( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
    /* The AdressUnassignmentOngoing is used to avoid race condition during CurrAsgnedAddr State change */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaRandNumGenerating( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
    /* The RandNumGenerating is used to avoid race condition if TcpIp_RandNumXor64 is called at the same time */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaRandNumGenerating( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
    /* The RandNumGenerating is used to avoid race condition if TcpIp_RandNumXor64 is called at the same time */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaSockOperations( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaSockOperations( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}


/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaReqRelAddrAssign( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
    /* The ReqAddrAssign is used to avoid race condition during storing/updating the request IP address global structure variable. */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaReqRelAddrAssign( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
    /* The ReqAddrAssign is used to avoid race condition during storing/updating the request IP address global structure variable. */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaRequestedAssignPrioAccess( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
    /* The ReleaseAddrAssign is used to avoid race condition during storing/updating the request IP address global structure variable. */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaRequestedAssignPrioAccess( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
    /* The ReleaseAddrAssign is used to avoid race condition during storing/updating the request IP address global structure variable. */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaLocalIPChgParamAccess( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
    /* The ReleaseAddrAssign is used to avoid race condition during storing/updating the request IP address global structure variable. */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaLocalIPChgParamAccess( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
    /* The ReleaseAddrAssign is used to avoid race condition during storing/updating the request IP address global structure variable. */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_TcpIp_ExclusiveAreaManualStaticIPAddrAccess( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
    /* The ReleaseAddrAssign is used to avoid race condition during storing/updating the request IP address global structure variable. */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_TcpIp_ExclusiveAreaManualStaticIPAddrAccess( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
    /* The ReleaseAddrAssign is used to avoid race condition during storing/updating the request IP address global structure variable. */
}

#endif /* TCPIP_CFG_SCHM_H */

