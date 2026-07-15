/*
 **********************************************************************************************************************
 *
 * COPYRIGHT RESERVED, ETAS GmbH, 2017. All rights reserved.
 * The reproduction, distribution and utilization of this document as well as the communication of its contents to
 * others without explicit authorization is prohibited. Offenders will be held liable for the payment of damages.
 * All rights reserved in the event of the grant of a patent, utility model or design.
 *
 **********************************************************************************************************************
 ************************************************************************************************
 * Component : Wdg.h
 * Date      : Jan 28 2019
 * Version   : 1.0
 * Description  : This module implements header file for functions supplied for Wdg wrapper.
 *                Note: This file contains sample implementation only. It is not part of the
 *                      production deliverables.
 ***********************************************************************************************
*/

#ifndef _WDG_H_
#define _WDG_H_

/*
 **********************************************************************************************************************
 * Includes
 **********************************************************************************************************************
*/
#include "Std_Types.h"
#include "WdgIf_Types.h"
#include "Wdg_17_Scu.h"


#define WDG_AR_RELEASE_MAJOR_VERSION      (4U)
#define WDG_AR_RELEASE_MINOR_VERSION      (2U)
#define WDG_AR_RELEASE_REVISION_VERSION   (2U)

/*
 **********************************************************************************************************************
 * Define/Macros
 **********************************************************************************************************************
 */
#define MAINFUNCTION_WDG_TRIGGER 1 //ms

#define Wdg_SetMode Wdg_17_Scu_SetMode
#define Wdg_SetTriggerCondition Wdg_17_Scu_SetTriggerCondition
/*
 **********************************************************************************************************************
 * Type definitions
 **********************************************************************************************************************
 */


/*
 **********************************************************************************************************************
 * Extern declarations
 **********************************************************************************************************************
 */

#endif /* _WDG_H_ */

/*
 **********************************************************************************************************************
 * End of the file
 **********************************************************************************************************************
 */

