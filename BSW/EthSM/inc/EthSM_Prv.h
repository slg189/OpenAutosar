

#ifndef ETHSM_PRV_H_
#define ETHSM_PRV_H_

/*
***************************************************************************************************
* Includes
***************************************************************************************************
*/
#if (ETHSM_DEV_ERROR_DETECT == STD_ON)
#include "Det.h"
#endif /* (ETHSM_DEV_ERROR_DETECT == STD_ON) */


/*
 ***************************************************************************************************
 * DET Defines
 ***************************************************************************************************
 */

/* API IDs for DET Support */
#define ETHSM_SID_ETHSM_INIT                0x1U
#define ETHSM_SID_ETHSM_GETVERSIONINFO      0x2U
#define ETHSM_SID_ETHSM_REQUESTCOMMODE      0x3U
#define ETHSM_SID_ETHSM_GETCURRENTCOMMODE   0x4U
#define ETHSM_SID_ETHSM_MAINFUNCTION        0x5U
#define ETHSM_SID_ETHSM_TRCVLINKSTATECHG    0x6U
#define ETHSM_SID_ETHSM_TCPIPMODEINDICATION 0x8U
#define ETHSM_SID_ETHSM_CTRLMODEINDICATION  0x9U

/* Development Error Codes for DET Support */
#define ETHSM_E_INVALID_NETWORK_MODE        0x1U    /* Invalid communication mode requested */
#define ETHSM_E_UNINIT                      0x2U    /* Module was not initialized */
#define ETHSM_E_PARAM_POINTER               0x3U    /* Invalid pointer in parameter list */
#define ETHSM_E_INVALID_NETWORK_HANDLE      0x4U    /* Invalid parameter in parameter list */
#define ETHSM_E_INVALID_TCPIP_MODE          0x5U    /* Invalid parameter in parameter list. In SWS macro name is in mixed case( ETHSM_E_INVALID_TcpIpMode ) */
#define ETHSM_E_INVALID_TRCV_LINK_STATE     0x6U    /* Invalid parameter in parameter list */
#define ETHSM_E_PARAM_CONTROLLER            0x7U    /* Invalid parameter in parameter list */


/*
 ***************************************************************************************************
 * DET Check
 ***************************************************************************************************
 */

/* No reporting to DET module if Development Detection Error is OFF */
/* According to CAP Coding Guideline, Development Error detection shall not be controlled by configuration switch DevErrorDetect for safety reasons. */
/* This behavior is compliant to runtime-error behavior and a deviation from AUTOSAR */
#if (ETHSM_DEV_ERROR_DETECT == STD_ON)
#define ETHSM_CALL_DET_REPORTERROR(API, ERROR) (void)Det_ReportError(ETHSM_MODULE_ID, ETHSM_INSTANCE_ID, API, ERROR);
#else
#define ETHSM_CALL_DET_REPORTERROR(API, ERROR)
#endif

/* DET For functions with return type as 'void' */
#define ETHSM_REPORT_ERROR_RET_VOID( CONDITION, API, ERROR )            \
    if(CONDITION)                                                       \
    {                                                                   \
       ETHSM_CALL_DET_REPORTERROR(API, ERROR)                           \
       return;                                                          \
    }                                                                   \

/* DET For functions with return type as 'Std_ReturnType' */
#define ETHSM_REPORT_ERROR_RET_VALUE( CONDITION, API, ERROR, RETVAL )   \
    if(CONDITION)                                                       \
    {                                                                   \
       ETHSM_CALL_DET_REPORTERROR(API, ERROR)                           \
       return (RETVAL);                                                 \
    }                                                                   \


#endif /* ETHSM_PRV_H_ */
