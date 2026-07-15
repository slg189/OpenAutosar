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
#ifndef SOAD_CFG_SCHM_H
#define SOAD_CFG_SCHM_H

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/

/* Inline function prototypes */

/* The exclusive area UdpSupervisionTimer is used to avoid race condition when UdpSupervisionTimer is modified*/
LOCAL_INLINE void SchM_Enter_SoAd_UdpSupervisionTimer( void );
LOCAL_INLINE void SchM_Exit_SoAd_UdpSupervisionTimer( void );

/* The exclusive area nPduTxTimer is used to avoid race condition when nPduTxTimer is modified*/
LOCAL_INLINE void SchM_Enter_SoAd_nPduUdpTxTimer( void );
LOCAL_INLINE void SchM_Exit_SoAd_nPduUdpTxTimer( void );

/* The exclusive area SoAd_RoutingGroupStatus is used to avoid race condition when rg status flag is enabled / Disabled */
LOCAL_INLINE void SchM_Enter_SoAd_RoutingGroupStatus( void );
LOCAL_INLINE void SchM_Exit_SoAd_RoutingGroupStatus( void );

/* The exclusive area ReserveFreeDynSoc is used to avoid race condition while reserving free socket */
LOCAL_INLINE void SchM_Enter_SoAd_ReserveFreeDynSoc( void );
LOCAL_INLINE void SchM_Exit_SoAd_ReserveFreeDynSoc( void );

/* The exclusive area SoAd_ULIfUdpTxConfirm is used to avoid race condition when SoAd_ULIfUdpTxConfirm flag is modified */
LOCAL_INLINE void SchM_Enter_SoAd_ULIfUdpTxConfirm( void );
LOCAL_INLINE void SchM_Exit_SoAd_ULIfUdpTxConfirm( void );

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
/* The exclusive area MeasurementData is used to avoid race condition when MeasurementData is modified */
LOCAL_INLINE void SchM_Enter_SoAd_MeasurementData( void );
LOCAL_INLINE void SchM_Exit_SoAd_MeasurementData( void );
#endif

/* The exclusive area RxBufferAccess is used to avoid race condition when RxBuffer is modified */
LOCAL_INLINE void SchM_Enter_SoAd_RxBufferAccess( void );
LOCAL_INLINE void SchM_Exit_SoAd_RxBufferAccess( void );

/* The exclusive area RxBufferAccess is used to avoid race condition when there is socket open close request */
LOCAL_INLINE void SchM_Enter_SoAd_OpenCloseReq( void );
LOCAL_INLINE void SchM_Exit_SoAd_OpenCloseReq( void );

/* The exclusive area RxBufferAccess is used to avoid race condition when socket mode is modified */
LOCAL_INLINE void SchM_Enter_SoAd_SocConModeProperties( void );
LOCAL_INLINE void SchM_Exit_SoAd_SocConModeProperties( void );

/* The exclusive area RxBufferAccess is used to avoid race condition when remote address is modified */
LOCAL_INLINE void SchM_Enter_SoAd_RemoteAddrUpdate( void );
LOCAL_INLINE void SchM_Exit_SoAd_RemoteAddrUpdate( void );

/* The exclusive area RxBufferAccess is used to avoid race condition when Tx/Rx request is modified */
LOCAL_INLINE void SchM_Enter_SoAd_TxRxReq( void );
LOCAL_INLINE void SchM_Exit_SoAd_TxRxReq( void );

/* The exclusive area RxBufferAccess is used to avoid race condition when IF  Pdu is accessed concurrently for Tx  */
LOCAL_INLINE void SchM_Enter_SoAd_IfPduInUse( void );
LOCAL_INLINE void SchM_Exit_SoAd_IfPduInUse( void );

/* The exclusive area RxBufferAccess is used to avoid race condition when Tp  Pdu is accessed concurrently for Tx  */
LOCAL_INLINE void SchM_Enter_SoAd_TpPduInUse( void );
LOCAL_INLINE void SchM_Exit_SoAd_TpPduInUse( void );

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_UdpSupervisionTimer( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_UdpSupervisionTimer( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_nPduUdpTxTimer( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_nPduUdpTxTimer( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_RoutingGroupStatus( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_RoutingGroupStatus( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_ReserveFreeDynSoc( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_ReserveFreeDynSoc( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_ULIfUdpTxConfirm( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}


/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_ULIfUdpTxConfirm( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

#if ( SOAD_GETANDRESET_MEASUREMENTDATA_API == STD_ON )
/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_MeasurementData( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_MeasurementData( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_RxBufferAccess( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_RxBufferAccess( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_OpenCloseReq( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_OpenCloseReq( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_SocConModeProperties( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_SocConModeProperties( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_RemoteAddrUpdate( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_RemoteAddrUpdate( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_TxRxReq( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_TxRxReq( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_IfPduInUse( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_IfPduInUse( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_SoAd_TpPduInUse( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_SoAd_TpPduInUse( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif /* ETHIF_CFG_SCHM_H */

