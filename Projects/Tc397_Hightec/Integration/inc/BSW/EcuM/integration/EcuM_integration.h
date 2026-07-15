/*
 * EcuM_integration.h
 *
 *  Created on: May 15, 2019
 *      Author: AGT1HC
 */

#ifndef INTEGRATION_BSW_INCLUDE_ECUM_INTEGRATION_H_
#define INTEGRATION_BSW_INCLUDE_ECUM_INTEGRATION_H_

#include "EcuM.h"

#define ECUM_MAINFUNCTION_TIMEOUT	20
extern void Loop_EcuM_MainFunc(void);
extern void VectorTable_Init(sint8 coreID);


#endif /* INTEGRATION_BSW_INCLUDE_ECUM_INTEGRATION_H_ */
