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
#ifndef ETHIF_CFG_SCHM_H
#define ETHIF_CFG_SCHM_H

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/

/* Integrator shall place his code for locking/disabling interrupts
 * The lock shall be nestable (other locks shall be allowed when this lock is set) because external functions are used during locking
*/

/* SchM_EthIf_EthCtrlState() lock is used when when EthIfPhysCtrl state is changed and in parallel, this state is used */
/* Hint: It is not allowed to call Eth API in the same time the EthCtrl state is changed. */
/* For example if EthCtrl state is changed to DOWN in EthIf_MainFunctionState(), it is not */
/* permitted to call Eth_Transmit or any other Eth API in the same time. */
/* The access to the section where EthCtrl state is changed is under protection. */
LOCAL_INLINE void SchM_Enter_EthIf_EthCtrlState( void );
LOCAL_INLINE void SchM_Exit_EthIf_EthCtrlState( void );

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
/* SchM_EthIf_SetTrcvState() lock is used when transceiver state is updated in EthIf_TrcvModeIndication() API and in EthIf_Prv_SetTransceiverMode() API. */
LOCAL_INLINE void SchM_Enter_EthIf_SetTrcvState( void );
LOCAL_INLINE void SchM_Exit_EthIf_SetTrcvState( void );
#endif

#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
/* SchM_EthIf_UpdatePhysAddrFilter() lock is used when the virtual PhyAddr filter is getting updated in EthIf_UpdatePhysAddrFilter() API. */
/* The lock also is used when vitual PhyAddr filter is reset in EthIf_Prv_ResetPhysAddrFilterReferencesForEthHwCtrlIdx() API and EthIf_Prv_ResetPhysAddrFilterReferences() API. */
LOCAL_INLINE void SchM_Enter_EthIf_UpdatePhysAddrFilter( void );
LOCAL_INLINE void SchM_Exit_EthIf_UpdatePhysAddrFilter( void );
#endif

#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
/* SchM_EthIf_UpdateMeasurementData() lock is used when the measurement data gets updated in EthIf_Prv_IncMeasurementData() API and EthIf_GetAndResetMeasurementData() API. */
LOCAL_INLINE void SchM_Enter_EthIf_UpdateMeasurementData( void );
LOCAL_INLINE void SchM_Exit_EthIf_UpdateMeasurementData( void );
#endif


#if( ETHIF_POLL_ETHTRCV_SIGNALQUALITY == STD_ON )
/* SchM_EthIf_TrcvSignalQuality() lock is used when signal quality data strcture for Trcv is updated/read in */
/* EthIf_Prv_SignalQualityProcessing() API, EthIf_GetTrcvSignalQuality() API and EthIf_ClearTrcvSignalQuality() API. */
LOCAL_INLINE void SchM_Enter_EthIf_TrcvSignalQuality( void );
LOCAL_INLINE void SchM_Exit_EthIf_TrcvSignalQuality( void );
#endif

#if( ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON )
/* SchM_EthIf_TrcvSignalQuality() lock is used when signal quality data strcture for Trcv is updated/read in */
/* EthIf_Prv_SignalQualityProcessing() API, EthIf_GetSwitchPortSignalQuality() API and EthIf_ClearSwitchPortSignalQuality() API. */
LOCAL_INLINE void SchM_Enter_EthIf_SwtPortSignalQuality( void );
LOCAL_INLINE void SchM_Exit_EthIf_SwtPortSignalQuality( void );
#endif


/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_EthIf_EthCtrlState( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}

/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_EthIf_EthCtrlState( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_EthIf_SetTrcvState( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}
#endif

#if( ETHIF_ETHIFCTRL_TRCV_REFERENCED == STD_ON )
/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_EthIf_SetTrcvState( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_EthIf_UpdatePhysAddrFilter( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}
#endif

#if( ETHIF_UPDATE_PHYS_ADDR_FILTER_SUPPORT == STD_ON )
/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_EthIf_UpdatePhysAddrFilter( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_EthIf_UpdateMeasurementData( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}
#endif

#if( ETHIF_ENABLE_GETANDRESET_MEAS_DATA == STD_ON )
/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_EthIf_UpdateMeasurementData( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

#if( ETHIF_POLL_ETHTRCV_SIGNALQUALITY == STD_ON )
/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_EthIf_TrcvSignalQuality( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}
#endif

#if( ETHIF_POLL_ETHTRCV_SIGNALQUALITY == STD_ON )
/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_EthIf_TrcvSignalQuality( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

#if( ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON )
/* Inline function definition : Enter critical section */
LOCAL_INLINE void SchM_Enter_EthIf_SwtPortSignalQuality( void )
{
    /* Integrator shall place his code for locking/disabling the interrupt */
}
#endif

#if( ETHIF_POLL_ETHSWTPORT_SIGNALQUALITY == STD_ON )
/* Inline function definition : Exit critical section */
LOCAL_INLINE void SchM_Exit_EthIf_SwtPortSignalQuality( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

#endif /* ETHIF_CFG_SCHM_H */

