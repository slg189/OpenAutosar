/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */




/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/

#include "WdgM.h"           /* For visibility of prototype of function WdgM_CheckpointReached() */
#include "WdgM_Bsw_User.h"  /* For visibility of prototype of function WdgM_Rb_Cfg_IndicateCheckPointReached_...() */

/*
**********************************************************************************************************************
* Function Definition
**********************************************************************************************************************
*/

#define WDGM_START_SEC_CODE
#include "WdgM_MemMap.h"

/* Function to indicate check point reached.
 * Make copy of the following function if more than one check point reached is to be indicated.
 * Schedule the function/s in Os.
 * In case it is for the indication of Alive Supervision of a task,it's suggested to schedule it at the top of the task
 */
/* Note: Name of the function "WdgM_Rb_Cfg_IndicateCheckPointReached_<name_corresponding_to_checkpoint>" shall not
 * exceed 60 characters */
// void WdgM_Rb_Cfg_IndicateCheckPointReached_<name_corresponding_to_checkpoint>(void)
// {
//     /* Additional logic can be added if needed */

//     /* Report check point reached to WdgM */
//     (void)WdgM_CheckpointReached(WdgMConf_WdgMSupervisedEntity_<configured_supervised_entity_short_name>,
//                            WdgMConf_WdgMCheckpoint_<configured_checkpoint_short_name>);
// }

#define WDGM_STOP_SEC_CODE
#include "WdgM_MemMap.h"
