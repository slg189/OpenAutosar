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
#ifndef RBA_ETHICMP_CFG_SCHM_H
#define RBA_ETHICMP_CFG_SCHM_H

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/

/* Inline function prototypes */
/* The exclusive area ExclusiveAreaIcmpFragmentedMsg is used to avoid race condition when ICMP messages of length greater than MTU is being transmitted. */
/* If ICMP frames (ICMP reply) greater than MTU size is being transmitted in interrupt context, then rba_EthIPv4_ProvideTxBuffer() and rba_EthIPv4_Transmit() shall be called in the same context. */
/* 
#if (RBA_ETHICMP_MSG_FRAGMENTATION_ALLOWED == STD_ON)
LOCAL_INLINE void SchM_Enter_rba_EthIcmp_ExclusiveAreaIcmpFragmentedMsg( void );
LOCAL_INLINE void SchM_Exit_rba_EthIcmp_ExclusiveAreaIcmpFragmentedMsg( void );
#endif

/* Inline function definition : Exit critical section */
#if (RBA_ETHICMP_MSG_FRAGMENTATION_ALLOWED == STD_ON)
LOCAL_INLINE void SchM_Enter_rba_EthIcmp_ExclusiveAreaIcmpFragmentedMsg( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

/* Inline function definition : Exit critical section */
#if (RBA_ETHICMP_MSG_FRAGMENTATION_ALLOWED == STD_ON)
LOCAL_INLINE void SchM_Exit_rba_EthIcmp_ExclusiveAreaIcmpFragmentedMsg( void )
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}
#endif

#endif /* RBA_ETHICMP_CFG_SCHM_H */
