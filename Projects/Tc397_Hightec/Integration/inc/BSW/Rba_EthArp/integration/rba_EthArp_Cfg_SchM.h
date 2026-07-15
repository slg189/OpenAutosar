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
#ifndef RBA_ETHARP_CFG_SCHM_H
#define RBA_ETHARP_CFG_SCHM_H

/*
**********************************************************************************************************************
* Inline functions
**********************************************************************************************************************
*/

/* Inline function prototypes */
/* The exclusive area rba_EthArp_UpdateTable is used to avoid race condition during arp table read-write access */

LOCAL_INLINE FUNC( void, RBA_ETHIF_CODE )  SchM_Enter_rba_EthArp_UpdateTable(void)
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}

LOCAL_INLINE FUNC( void, RBA_ETHIF_CODE )  SchM_Exit_rba_EthArp_UpdateTable(void)
{
    /* Integrator shall place his code for unlocking/enabling the interrupt */
}


#endif /* RBA_ETHARP_CFG_SCHM_H */
