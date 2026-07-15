/*
 * Xcp_OverlayMem.h
 *
 *  Created on: Dec 06, 2019
 *      Author: ZCO7SGH
 */


#ifndef OverlayManager_H_
#define OverlayManager_H_


/**********************************************************************************************************************
 * INCLUDES
 *********************************************************************************************************************/
#include "Std_Types.h"
#include "IfxCpu_reg.h"
#include "IfxScu_reg.h"

/*******************************************************************************
* Macro Definitions
*******************************************************************************/
extern uint32 __RODATA_CALIB_START;
extern uint32 __OVERLAY_RAM_START;

#define SIZE_KB(size)       ((uint32)(size * 1024UL))
#define CALFLASH_START_ADDR (&__RODATA_CALIB_START)
#define CALRAM_START_ADDR   (&__OVERLAY_RAM_START)
#define CAL_MEM_SIZE        SIZE_KB(16)

#define CONVERT_TO_OVERLAYMEM_Addr(addr)  \
    ((uint32)addr - CALFLASH_START_ADDR + CALRAM_START_ADDR)

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void Overlay_Init(void);
extern void Overlay_Enable(void);
extern void Overlay_Disable(void);
extern void Overlay_CheckEnabled(boolean *enabled);
extern void Overlay_Sync(void);



#endif /*OverlayManager_H_*/

/**********************************************************************************************************************
 *  END OF FILE
 *********************************************************************************************************************/
