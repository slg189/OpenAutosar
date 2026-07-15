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
#ifndef RBA_ETHUDP_CFG_SCHM_H
#define RBA_ETHUDP_CFG_SCHM_H

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/

/* Inline function prototypes */
/* The exclusive area ExclusiveAreaSockState is used to avoid race condition when rba_EthUdp_GetSocket() is reentered */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaSockState( void );
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaSockState( void );

/* The exclusive area ExclusiveAreaUsedSocCnt is used to avoid race condition when the total number of allocated sockets is incremented / decremented */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaUsedSocCnt( void );
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaUsedSocCnt( void );

/* The exclusive area ExclusiveAreaOngoingOperation is used to avoid race condition when API using global socket resources and closing of the socket are executed concurrently */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaOngoingOperation( void );
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaOngoingOperation( void );

/* The exclusive area ExclusiveAreaLocalAddrInUse is used to avoid race condition when rba_EthUdp_Bind() is reentered for different SocketIds. */
/* It avoids that 2 sockets will be bound to the same LocalAddrId and Port */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaLocalAddrInUse( void );
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaLocalAddrInUse( void );

/* The exclusive area ExclusiveAreaPortAny is used to avoid race condition when the global variable storing the current port to be used in wildcard range is incremented */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaPortAny( void );
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaPortAny( void );

/* The exclusive area ExclusiveAreaSockFramePrio is used to avoid race condition when the global variable storing the frame priority is updated in rba_EthUdp_Bind() and rba_EthUdp_ChangeParameter() APIs */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaSockFramePrio( void );
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaSockFramePrio( void );

/* The exclusive area ExclusiveAreaMeasurementData is used to avoid race condition when the measurement data is incremented */
#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaMeasurementData( void );
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaMeasurementData( void );
#endif

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaSockState( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaSockState( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaUsedSocCnt( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaUsedSocCnt( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaOngoingOperation( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaOngoingOperation( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaLocalAddrInUse( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaLocalAddrInUse( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaPortAny( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaPortAny( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaSockFramePrio( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaSockFramePrio( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
LOCAL_INLINE void SchM_Enter_rba_EthUdp_ExclusiveAreaMeasurementData( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}
#endif

/* Inline function definition : Exit critical section */
#if (TCPIP_ENABLE_GETANDRESET_MEAS_DATA == STD_ON)
LOCAL_INLINE void SchM_Exit_rba_EthUdp_ExclusiveAreaMeasurementData( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

#endif /* ETHIF_CFG_SCHM_H */

