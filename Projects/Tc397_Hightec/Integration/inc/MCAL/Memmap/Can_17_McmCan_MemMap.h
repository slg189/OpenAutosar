/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2018)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME  : Can_17_McmCan_MemMap.h                                                      **
**                                                                            **
**  VERSION   :  <to be filled>                                               **
**                                                                            **
**  DATE      : 28/01/2019                                                            **
**                                                                            **
**  VARIANT   : NA                                                            **
**                                                                            **
**  PLATFORM  : Infineon AURIX2G                                              **
**                                                                            **
**  AUTHOR    : DL-AUTOSAR-Engineering                                        **
**                                                                            **
**  VENDOR    : Infineon Technologies                                         **
**                                                                            **
**  TRACEABILITY :                                                            **
**                                                                            **
**  DESCRIPTION  : This file contains the mapping of Memory Section           **
**                                                                            **
**  [/cover]                                                                  **
**                                                                            **
**  SPECIFICATION(S) : Specification of CAN_17_MCMCAN Driver,                      **
**   AUTOSAR Release <4.2.2>                                                  **
**                                                                            **
**  MAY BE CHANGED BY USER : Yes                                              **
**                                                                            **
*******************************************************************************/
#define MEMMAP_ERROR
#if (1)
/*DATA Section*/
/*To be used for all global or static variables.*/
/*Variable to be cleared at startup or reset is palced here - .bss*/
#if defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.Cpu0.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.Cpu0.Unspecified" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.Cpu0.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.Cpu0.Unspecified" "ClearedData.Cpu0.Unspecified" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.Cpu1.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.Cpu1.Unspecified" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.Cpu1.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.Cpu1.Unspecified" "ClearedData.Cpu1.Unspecified" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.Cpu2.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.Cpu2.Unspecified" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.Cpu2.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.Cpu2.Unspecified" "ClearedData.Cpu2.Unspecified" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.Cpu3.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.Cpu3.Unspecified" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.Cpu3.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.Cpu3.Unspecified" "ClearedData.Cpu3.Unspecified" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.Cpu4.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.Cpu4.Unspecified" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.Cpu4.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.Cpu4.Unspecified" "ClearedData.Cpu4.Unspecified" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.Cpu5.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.Cpu5.Unspecified" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.Cpu5.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.Cpu5.Unspecified" "ClearedData.Cpu5.Unspecified" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE0_U32
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.Cpu0.32bit"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.Cpu0.32bit" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.Cpu0.32bit"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.Cpu0.32bit" "ClearedData.Cpu0.32bit" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE0_U32
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE0_U32
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE0_U32
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U32
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.LmuNC.32bit"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.LmuNC.32bit" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.LmuNC.32bit"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.LmuNC.32bit" "ClearedData.LmuNC.32bit" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U32
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U32
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U32
#undef MEMMAP_ERROR


#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.LmuNC.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.LmuNC.Unspecified" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.LmuNC.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.LmuNC.Unspecified" "ClearedData.LmuNC.Unspecified" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U16
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.LmuNC.16bit"
#pragma align 2
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.LmuNC.16bit" aw 2
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.LmuNC.16bit"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.LmuNC.16bit" "ClearedData.LmuNC.16bit" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U16
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U16
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U16
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U8
#ifdef _TASKING_C_TRICORE_
#pragma section farbss "ClearedData.LmuNC.8bit"
#elif defined _GNU_C_TRICORE_
#pragma section "ClearedData.LmuNC.8bit" aw
#elif defined _GHS_C_TRICORE_
#pragma ghs section bss = ".ClearedData.LmuNC.8bit"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.LmuNC.8bit" "ClearedData.LmuNC.8bit" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U8
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U8
#ifdef _TASKING_C_TRICORE_
#pragma section farbss restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U8
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_INIT_QM_GLOBAL_U32
#ifdef _TASKING_C_TRICORE_
#pragma section fardata "InitData.LmuNC.32bit"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "InitData.LmuNC.32bit" aw 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section data = ".InitData.LmuNC.32bit"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.LmuNC.32bit"  "ClearedData.LmuNC.32bit" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_INIT_QM_GLOBAL_U32
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_INIT_QM_GLOBAL_U32
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section fardata restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_INIT_QM_GLOBAL_U32
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_VAR_INIT_QM_GLOBAL_U8
#ifdef _TASKING_C_TRICORE_
#pragma section fardata "InitData.LmuNC.8bit"
#elif defined _GNU_C_TRICORE_
#pragma section "InitData.LmuNC.8bit" aw
#elif defined _GHS_C_TRICORE_
#pragma ghs section data = ".InitData.LmuNC.8bit"
#elif defined _DIABDATA_C_TRICORE_
#pragma section DATA "InitData.LmuNC.8bit"  "ClearedData.LmuNC.8bit" far-absolute RW
#endif
#undef  CAN_17_MCMCAN_START_SEC_VAR_INIT_QM_GLOBAL_U8
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_INIT_QM_GLOBAL_U8
#ifdef _TASKING_C_TRICORE_
#pragma section fardata restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_INIT_QM_GLOBAL_U8
#undef MEMMAP_ERROR

/***************** Static Global Constants Sections ************************/
#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE0_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Const.Cpu0.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Const.Cpu0.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Const.Cpu0.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Const.Cpu0.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE0_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE1_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Const.Cpu1.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Const.Cpu1.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Const.Cpu1.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Const.Cpu1.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE1_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE2_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Const.Cpu2.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Const.Cpu2.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Const.Cpu2.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Const.Cpu2.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE2_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE3_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Const.Cpu3.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Const.Cpu3.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Const.Cpu3.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Const.Cpu3.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE3_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE4_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Const.Cpu4.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Const.Cpu4.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Const.Cpu4.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Const.Cpu4.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE4_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE5_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Const.Cpu5.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Const.Cpu5.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Const.Cpu5.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Const.Cpu5.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE5_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_GLOBAL_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom  "Const.Cpu0.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section  "Const.Cpu0.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Const.Cpu0.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Const.Cpu0.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_GLOBAL_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR

/******************* Config Data Sections *************************/
#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_GLOBAL_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Config.Cpu0.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Config.Cpu0.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Config.Cpu0.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Config.Cpu0.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_GLOBAL_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE0_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Config.Cpu0.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Config.Cpu0.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Config.Cpu0.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Config.Cpu0.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE0_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE1_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Config.Cpu1.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Config.Cpu1.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Config.Cpu1.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Config.Cpu1.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE1_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE2_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Config.Cpu2.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Config.Cpu2.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Config.Cpu2.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Config.Cpu2.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE2_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE3_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Config.Cpu3.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Config.Cpu3.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Config.Cpu3.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Config.Cpu3.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE3_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE4_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Config.Cpu4.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Config.Cpu4.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Config.Cpu4.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Config.Cpu4.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE4_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR

#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE5_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma section farrom "Config.Cpu5.Unspecified"
#pragma align 4
#elif defined _GNU_C_TRICORE_
#pragma section "Config.Cpu5.Unspecified" a 4
#elif defined _GHS_C_TRICORE_
#pragma ghs section rodata = ".Config.Cpu5.Unspecified"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CONST "Config.Cpu5.Unspecified" far-absolute R
#endif
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE5_UNSPECIFIED
#ifdef _TASKING_C_TRICORE_
#pragma align restore
#pragma section farrom restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR

/* Code Sections */
/*
To be used for mapping code to application block, boot block, external flash etc.
codePeriod is the typical period time value and unit of the ExecutableEntitys in this MemorySection.
The name part _{codePeriod} is optional. Units are:
- US microseconds
- MS milli second
- S second
*/
#elif defined CAN_17_MCMCAN_START_SEC_CODE_QM_GLOBAL
#ifdef _TASKING_C_TRICORE_
#pragma section code "Code.Cpu0"
#elif defined _GNU_C_TRICORE_
#pragma section "Code.Cpu0" ax
#elif defined _GHS_C_TRICORE_
#pragma ghs section text = ".Code.Cpu0"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CODE "Code.Cpu0" RX
#endif
#undef  CAN_17_MCMCAN_START_SEC_CODE_QM_GLOBAL
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CODE_QM_GLOBAL
#ifdef _TASKING_C_TRICORE_
#pragma section code restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_17_MCMCAN_STOP_SEC_CODE_QM_GLOBAL
#undef MEMMAP_ERROR

#elif defined CAN_START_SEC_CODE
#ifdef _TASKING_C_TRICORE_
#pragma section code "Code.Cpu0"
#elif defined _GNU_C_TRICORE_
#pragma section "Code.Cpu0" ax
#elif defined _GHS_C_TRICORE_
#pragma ghs section text = ".Code.Cpu0"
#elif defined _DIABDATA_C_TRICORE_
#pragma section CODE "Code.Cpu0" RX
#endif
#undef  CAN_START_SEC_CODE
#undef MEMMAP_ERROR
#elif defined CAN_STOP_SEC_CODE
#ifdef _TASKING_C_TRICORE_
#pragma section code restore
#elif defined _GNU_C_TRICORE_
#pragma section
#elif defined _GHS_C_TRICORE_
#pragma ghs section
#elif defined _DIABDATA_C_TRICORE_
#pragma section 
#endif
#undef  CAN_START_SEC_CODE
#undef MEMMAP_ERROR

#endif

#else  /*#if ((APP_SW == DEMO_APP) || (APP_SW == TEST_APP))*/ 
/***********************************EMPTY SECTIONS***********************************/
#if defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U16
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U16
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U16
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U16
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U8
#undef  CAN_17_MCMCAN_START_SEC_VAR_CLEARED_QM_GLOBAL_U8
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U8
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_CLEARED_QM_GLOBAL_U8
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_INIT_QM_GLOBAL_U32
#undef  CAN_17_MCMCAN_START_SEC_VAR_INIT_QM_GLOBAL_U32
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_INIT_QM_GLOBAL_U32
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_INIT_QM_GLOBAL_U32
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_VAR_INIT_QM_GLOBAL_U8
#undef  CAN_17_MCMCAN_START_SEC_VAR_INIT_QM_GLOBAL_U8
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_VAR_INIT_QM_GLOBAL_U8
#undef  CAN_17_MCMCAN_STOP_SEC_VAR_INIT_QM_GLOBAL_U8
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE0_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE0_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE1_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE1_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE2_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE2_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE3_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE3_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE4_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE4_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_CORE5_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE5_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONST_QM_GLOBAL_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONST_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONST_QM_GLOBAL_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONST_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_GLOBAL_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_GLOBAL_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_GLOBAL_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE0_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE0_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE0_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE1_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE1_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE1_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE2_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE2_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE2_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE3_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE3_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE3_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE4_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE4_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE4_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE5_UNSPECIFIED
#undef  CAN_17_MCMCAN_START_SEC_CONFIG_DATA_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE5_UNSPECIFIED
#undef  CAN_17_MCMCAN_STOP_SEC_CONFIG_DATA_QM_CORE5_UNSPECIFIED
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_START_SEC_CODE_QM_GLOBAL
#undef  CAN_17_MCMCAN_START_SEC_CODE_QM_GLOBAL
#undef MEMMAP_ERROR
#elif defined CAN_17_MCMCAN_STOP_SEC_CODE_QM_GLOBAL
#undef  CAN_17_MCMCAN_STOP_SEC_CODE_QM_GLOBAL
#undef MEMMAP_ERROR
#endif

#endif /*#if ((APP_SW == DEMO_APP) || (APP_SW == TEST_APP))*/ 

#if defined MEMMAP_ERROR
#error "Can_17_McmCan_MemMap.h, wrong pragma command"
#endif
