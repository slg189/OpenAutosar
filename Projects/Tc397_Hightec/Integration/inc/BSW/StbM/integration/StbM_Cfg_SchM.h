#ifndef STBM_CFG_SCHM_H
#define STBM_CFG_SCHM_H

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
 */
/*The integrator include the required header file*/


/*
 **********************************************************************************************************************
 * Define/Macros
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */

/*
 **********************************************************************************************************************
 * INLINE function declarations
 **********************************************************************************************************************
 */

/* Interrupt lock to suspend all interrupts */
LOCAL_INLINE void SchM_Enter_StbM_Time_Source(void);
/* Interrupt lock to resume all interrupts */
LOCAL_INLINE void SchM_Exit_StbM_Time_Source(void);

/* Os resource lock to lock the resource for API's */
LOCAL_INLINE void SchM_Enter_StbM_Sync_OsResource(void);
/* Os resource lock to unlock the resource for API's*/
LOCAL_INLINE void SchM_Exit_StbM_Sync_OsResource(void);

/*
 **********************************************************************************************************************
 * INLINE function Definitions
 **********************************************************************************************************************
 */

LOCAL_INLINE void SchM_Enter_StbM_Time_Source(void)
{
    /*The integrator shall place his code here which would disable/lock the interrupt*/
}

LOCAL_INLINE void SchM_Exit_StbM_Time_Source(void)
{
    /*The integrator shall place his code here which would enable/unlock the interrupt*/
}

LOCAL_INLINE void SchM_Enter_StbM_Sync_OsResource(void)
{
    /*The integrator shall use the "OsResource" name that is used to lock the resource for Os API's*/
    /* Example : GetResource(STBM_RESOURCE); */

    /*Os Resource is configured for Os API's. It is not possible to use the common locks for OS API's
     Interrupt locks or Spinlocks can not be used for Os API's . Hence a Os Resource lock has to be configured.
     In Os configuration a Resource has to be configured and all the tasks and ISR's should be linked with the Resource,
     which are running on the core*/
}

LOCAL_INLINE void SchM_Exit_StbM_Sync_OsResource(void)
{
    /*The integrator shall use the "OsResource" name that is used to unlock the resource for Os API's */
    /* Example : ReleaseResource(STBM_RESOURCE);*/

    /*Os Resource is configured for Os API's. It is not possible to use the common locks for OS API's
     Interrupt locks or Spinlocks can not be used for Os API's . Hence a Os Resource lock has to be configured.
     In Os configuration a Resource has to be configured and all the tasks and ISR's should be linked with the Resource,
     which are running on the core*/
}

/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

#endif /* STBM_CFG_SCHM_H */

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */
