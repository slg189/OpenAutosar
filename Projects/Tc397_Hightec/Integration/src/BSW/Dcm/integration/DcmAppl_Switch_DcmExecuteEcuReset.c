/*
 * This is a template file. It defines integration functions necessary to complete RTA-BSW.
 * The integrator must complete the templates before deploying software containing functions defined in this file.
 * Once templates have been completed, the integrator should delete the #error line.
 * Note: The integrator is responsible for updates made to this file.
 *
 * To remove the following error define the macro NOT_READY_FOR_TESTING_OR_DEPLOYMENT with a compiler option (e.g. -D NOT_READY_FOR_TESTING_OR_DEPLOYMENT)
 * The removal of the error only allows the user to proceed with the building phase
 */




#include "Mcu.h"
#include "Dcm_Cfg_Prot.h"
#include "DcmCore_DslDsd_Inf.h"
#include "Rte_Dcm.h"
#if ((DCM_CFG_DSPUDSSUPPORT_ENABLED != DCM_CFG_OFF) && (DCM_CFG_DSP_ECURESET_ENABLED != DCM_CFG_OFF))
/*TESTCODE-START
#include "DcmTest.h"
TESTCODE-END*/
#define DCM_NO_RESET                           0x00u
#define DCM_ECURESET_HARD_RESET                0x01u
#define DCM_ECURESET_KEY_OFF_ON_RESET          0x02u
#define DCM_ECURESET_SOFT_RESET                0x03u
#define DCM_ENABLE_RAPID_POWER_SHUTDOWN_RESET  0x04u
#define DCM_DISABLE_RAPID_POWER_SHUTDOWN_RESET 0x05u

#define DCM_START_SEC_CODE /*Adding this for memory mapping*/
#include "Dcm_MemMap.h"
 /**
 *  @ingroup DCM_TPL
 *  DcmAppl_Switch_DcmExecuteEcuReset :-\n
 *  Inform the application to execute reset where the reset is triggered due to EcuReset service
 *  when used for jump to boot/drive
 *
 * @param[in]  ResetType_u8 : ResetType used for the EcuReset(e.g. DCM_NO_RESET,DCM_ENABLE_RAPID_POWER_SHUTDOWN_RESET or DCM_DISABLE_RAPID_POWER_SHUTDOWN_RESET)
 * @param[out] None
 * @retval     None
 *
 */
 void DcmAppl_Switch_DcmExecuteEcuReset(uint8 ResetType_u8)
 {
    switch (ResetType_u8)
	 {
		case DCM_ECURESET_HARD_RESET:
		 	/*Use dummy API */
			 Mcu_PerformReset();
			 break;
		case DCM_ECURESET_KEY_OFF_ON_RESET:
		 	/*Use dummy API */
			 Mcu_PerformReset();
			 break;
		case DCM_ECURESET_SOFT_RESET:
		 	/*Use dummy API */
			 Mcu_PerformReset();
			 break;
		case DCM_ENABLE_RAPID_POWER_SHUTDOWN_RESET:
		 	/*Use dummy API */

			 break;
		case DCM_DISABLE_RAPID_POWER_SHUTDOWN_RESET:
		 	/*Use dummy API */

			 break;
		default:
			break;

	 }
 }

#define DCM_STOP_SEC_CODE /*Adding this for memory mapping*/
#include "Dcm_MemMap.h"

#endif
