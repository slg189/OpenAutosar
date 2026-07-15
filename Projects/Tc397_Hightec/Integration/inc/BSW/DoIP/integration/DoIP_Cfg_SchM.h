/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */



#ifndef DOIP_CFG_SCHM_H
#define DOIP_CFG_SCHM_H

LOCAL_INLINE FUNC(void, EA_CODE) SchM_Enter_DoIP(void);
LOCAL_INLINE FUNC(void, EA_CODE) SchM_Exit_DoIP(void);

LOCAL_INLINE FUNC(void, EA_CODE) SchM_Enter_DoIP(void)
{
	/*The integrator shall place his code here which would disable/lock the interrupt*/
}

LOCAL_INLINE FUNC(void, EA_CODE) SchM_Exit_DoIP(void)
{
	/* The integrator shall place here the code which would unlock the interrupts */
}

#endif /*DOIP_CFG_SCHM_H*/

