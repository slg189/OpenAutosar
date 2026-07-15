/*----------------------------------------------------------------------------*/
/************** *SAMPLE* of MemMap.h. Not for production purposes *************/
/*----------------------------------------------------------------------------*/
#define MEM_VENDOR_ID        (11U)
#define MEM_AR_MAJOR_VERSION (4U)
#define MEM_AR_MINOR_VERSION (0U)
#define MEM_AR_PATCH_VERSION (3U)
#define MEM_SW_MAJOR_VERSION (5U)
#define MEM_SW_MINOR_VERSION (6U)
#define MEM_SW_PATCH_VERSION (3U)


/* -------------------------------------------------------------------------- */
/*                        TASKING table trace                                 */
/* -------------------------------------------------------------------------- */
/*    Sectiontype          Name           prefix Description
*    ------------------------------------------------------------------------
*    code                 .text          program code
*    neardata             .zdata         initialized __near data
*    fardata              .data          initialized __far data
*    nearrom              .zrodata       constant __near data
*    farrom               .rodata        constant __far data
*    nearbss              .zbss          uninitialized __near data (cleared)
*    farbss               .bss           uninitialized __far data (cleared)
*    nearnoclear          .zbss          uninitialized __near data
*    farnoclear           .bss           uninitialized __far data
*    a0data               .data_a0       initialized __a0 data
*    a0rom                .rodata_a0     constant __a0 data
*    a0bss                .bss_a0        uninitialized __a0 data (cleared)
*    a1data               .data_a1       initialized __a1 data
*    a1rom                .rodata_a1     constant __a1 data
*    a1bss                .bss_a1        uninitialized __a1 data (cleared)
*    a8data               .data_a8       initialized __a8 data
*    a8rom                .rodata_a8     constant __a8 data
*    a8bss                .bss_a8        uninitialized __a8 data (cleared)
*    a9data               .data_a9       initialized __a9 data
*    a9rom                .rodata_a9     constant __a9 data
*    a9bss                .bss_a9        uninitialized __a9 data (cleared)
*-----------------------------------------------------------------------------*/

/*******************************************************************************
**                      Default section mapping                               **
*******************************************************************************/

#define MEMMAP_ERROR

#if defined (START_WITH_IF)
/* -------------------------------------------------------------------------- */
/* CPU0 code sections                                                         */
/* -------------------------------------------------------------------------- */
#elif defined CPU0_START_SEC_CODE
   #undef CPU0_START_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code="Code.Cpu0"
   #pragma optimize R
   #endif
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_CODE
   #undef CPU0_STOP_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code restore
   #pragma endoptimize
   #endif
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU0 variable sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU0_START_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu0.8bit"
	#endif
	#undef CPU0_START_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss restore
	#endif
	#undef CPU0_STOP_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
	
#elif defined CPU0_START_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu0.16bit"
	#pragma align 2
	#endif
	#undef CPU0_START_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU0_STOP_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu0.32bit"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU0_STOP_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu0.64bit"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU0_STOP_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu0.Unspecified"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU0_STOP_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu0.8bit"
	#endif
	#undef CPU0_START_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata restore
	#endif
	#undef CPU0_STOP_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
	
#elif defined CPU0_START_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu0.16bit"
	#pragma align 2
	#endif
	#undef CPU0_START_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU0_STOP_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu0.32bit"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU0_STOP_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu0.64bit"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU0_STOP_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR	

#elif defined CPU0_START_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu0.Unspecified"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU0_STOP_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU0 constant sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU0_START_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu0.8bit"
	#endif
	#undef CPU0_START_SEC_CONST_8
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom restore
	#endif
	#undef CPU0_STOP_SEC_CONST_8
   #undef MEMMAP_ERROR
	
#elif defined CPU0_START_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu0.16bit"
	#pragma align 2
	#endif
	#undef CPU0_START_SEC_CONST_16
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU0_STOP_SEC_CONST_16
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu0.32bit"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_CONST_32
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU0_STOP_SEC_CONST_32
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu0.64bit"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_CONST_64
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU0_STOP_SEC_CONST_64
   #undef MEMMAP_ERROR

#elif defined CPU0_START_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu0.Unspecified"
	#pragma align 4
	#endif
	#undef CPU0_START_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU0_STOP_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU0_STOP_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR
   
/* -------------------------------------------------------------------------- */
/* CPU1 code sections                                                         */
/* -------------------------------------------------------------------------- */
#elif defined CPU1_START_SEC_CODE
   #undef CPU1_START_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code="Code.Cpu1"
   #pragma optimize R
   #endif
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_CODE
   #undef CPU1_STOP_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code restore
   #pragma endoptimize
   #endif
   #undef MEMMAP_ERROR
/* -------------------------------------------------------------------------- */
/* CPU1 variable sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU1_START_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu1.8bit"
	#endif
	#undef CPU1_START_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss restore
	#endif
	#undef CPU1_STOP_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
	
#elif defined CPU1_START_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu1.16bit"
	#pragma align 2
	#endif
	#undef CPU1_START_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU1_STOP_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu1.32bit"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU1_STOP_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu1.64bit"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU1_STOP_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu1.Unspecified"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU1_STOP_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu1.8bit"
	#endif
	#undef CPU1_START_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata restore
	#endif
	#undef CPU1_STOP_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
	
#elif defined CPU1_START_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu1.16bit"
	#pragma align 2
	#endif
	#undef CPU1_START_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU1_STOP_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu1.32bit"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU1_STOP_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu1.64bit"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU1_STOP_SEC_VAR_INIT_64	
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu1.Unspecified"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU1_STOP_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU1 constant sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU1_START_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu1.8bit"
	#endif
	#undef CPU1_START_SEC_CONST_8
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom restore
	#endif
	#undef CPU1_STOP_SEC_CONST_8
   #undef MEMMAP_ERROR
	
#elif defined CPU1_START_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu1.16bit"
	#pragma align 2
	#endif
	#undef CPU1_START_SEC_CONST_16
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU1_STOP_SEC_CONST_16
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu1.32bit"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_CONST_32
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU1_STOP_SEC_CONST_32
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu1.64bit"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_CONST_64
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU1_STOP_SEC_CONST_64
   #undef MEMMAP_ERROR

#elif defined CPU1_START_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu1.Unspecified"
	#pragma align 4
	#endif
	#undef CPU1_START_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU1_STOP_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU1_STOP_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU2 code sections                                                         */
/* -------------------------------------------------------------------------- */
#elif defined CPU2_START_SEC_CODE
   #undef CPU2_START_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code="Code.Cpu2"
   #pragma optimize R
   #endif
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_CODE
   #undef CPU2_STOP_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code restore
   #pragma endoptimize
   #endif
   #undef MEMMAP_ERROR
/* -------------------------------------------------------------------------- */
/* CPU2 variable sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU2_START_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu2.8bit"
	#endif
	#undef CPU2_START_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss restore
	#endif
	#undef CPU2_STOP_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
	
#elif defined CPU2_START_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu2.16bit"
	#pragma align 2
	#endif
	#undef CPU2_START_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU2_STOP_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu2.32bit"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU2_STOP_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu2.64bit"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU2_STOP_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu2.Unspecified"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU2_STOP_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu2.8bit"
	#endif
	#undef CPU2_START_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata restore
	#endif
	#undef CPU2_STOP_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
	
#elif defined CPU2_START_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu2.16bit"
	#pragma align 2
	#endif
	#undef CPU2_START_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU2_STOP_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu2.32bit"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU2_STOP_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu2.64bit"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU2_STOP_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu2.Unspecified"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU2_STOP_SEC_VAR_INIT_UNSPECIFIED  
   #undef MEMMAP_ERROR 	

/* -------------------------------------------------------------------------- */
/* CPU2 constant sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU2_START_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu2.8bit"
	#endif
	#undef CPU2_START_SEC_CONST_8
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom restore
	#endif
	#undef CPU2_STOP_SEC_CONST_8
   #undef MEMMAP_ERROR
	
#elif defined CPU2_START_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu2.16bit"
	#pragma align 2
	#endif
	#undef CPU2_START_SEC_CONST_16
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU2_STOP_SEC_CONST_16
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu2.32bit"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_CONST_32
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU2_STOP_SEC_CONST_32
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu2.64bit"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_CONST_64
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU2_STOP_SEC_CONST_64
   #undef MEMMAP_ERROR

#elif defined CPU2_START_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu2.Unspecified"
	#pragma align 4
	#endif
	#undef CPU2_START_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU2_STOP_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU2_STOP_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU3 code sections                                                         */
/* -------------------------------------------------------------------------- */
#elif defined CPU3_START_SEC_CODE
   #undef CPU3_START_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code="Code.Cpu3"
   #pragma optimize R
   #endif
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_CODE
   #undef CPU3_STOP_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code restore
   #pragma endoptimize
   #endif
   #undef MEMMAP_ERROR
/* -------------------------------------------------------------------------- */
/* CPU3 variable sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU3_START_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu3.8bit"
	#endif
	#undef CPU3_START_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss restore
	#endif
	#undef CPU3_STOP_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
	
#elif defined CPU3_START_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu3.16bit"
	#pragma align 2
	#endif
	#undef CPU3_START_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU3_STOP_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu3.32bit"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU3_STOP_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu3.64bit"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU3_STOP_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu3.Unspecified"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU3_STOP_SEC_VAR_CLEARED_UNSPECIFIED   
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu3.8bit"
	#endif
	#undef CPU3_START_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata restore
	#endif
	#undef CPU3_STOP_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
	
#elif defined CPU3_START_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu3.16bit"
	#pragma align 2
	#endif
	#undef CPU3_START_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU3_STOP_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu3.32bit"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU3_STOP_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu3.64bit"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU3_STOP_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu3.Unspecified"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU3_STOP_SEC_VAR_INIT_UNSPECIFIED	
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU3 constant sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU3_START_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu3.8bit"
	#endif
	#undef CPU3_START_SEC_CONST_8
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom restore
	#endif
	#undef CPU3_STOP_SEC_CONST_8
   #undef MEMMAP_ERROR
	
#elif defined CPU3_START_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu3.16bit"
	#pragma align 2
	#endif
	#undef CPU3_START_SEC_CONST_16
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU3_STOP_SEC_CONST_16
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu3.32bit"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_CONST_32
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU3_STOP_SEC_CONST_32
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu3.64bit"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_CONST_64
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU3_STOP_SEC_CONST_64
   #undef MEMMAP_ERROR

#elif defined CPU3_START_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu3.Unspecified"
	#pragma align 4
	#endif
	#undef CPU3_START_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU3_STOP_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU3_STOP_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU4 code sections                                                         */
/* -------------------------------------------------------------------------- */
#elif defined CPU4_START_SEC_CODE
   #undef CPU4_START_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code="Code.Cpu4"
   #pragma optimize R
   #endif
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_CODE
   #undef CPU4_STOP_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code restore
   #pragma endoptimize
   #endif
   #undef MEMMAP_ERROR
/* -------------------------------------------------------------------------- */
/* CPU4 variable sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU4_START_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu4.8bit"
	#endif
	#undef CPU4_START_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss restore
	#endif
	#undef CPU4_STOP_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
	
#elif defined CPU4_START_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu4.16bit"
	#pragma align 2
	#endif
	#undef CPU4_START_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU4_STOP_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu4.32bit"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU4_STOP_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu4.64bit"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU4_STOP_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu4.Unspecified"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU4_STOP_SEC_VAR_CLEARED_UNSPECIFIED   
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu4.8bit"
	#endif
	#undef CPU4_START_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata restore
	#endif
	#undef CPU4_STOP_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
	
#elif defined CPU4_START_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu4.16bit"
	#pragma align 2
	#endif
	#undef CPU4_START_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU4_STOP_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu4.32bit"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU4_STOP_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu4.64bit"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU4_STOP_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu4.Unspecified"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU4_STOP_SEC_VAR_INIT_UNSPECIFIED	
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU4 constant sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU4_START_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu4.8bit"
	#endif
	#undef CPU4_START_SEC_CONST_8
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom restore
	#endif
	#undef CPU4_STOP_SEC_CONST_8
   #undef MEMMAP_ERROR
	
#elif defined CPU4_START_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu4.16bit"
	#pragma align 2
	#endif
	#undef CPU4_START_SEC_CONST_16
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU4_STOP_SEC_CONST_16
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu4.32bit"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_CONST_32
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU4_STOP_SEC_CONST_32
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu4.64bit"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_CONST_64
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU4_STOP_SEC_CONST_64
   #undef MEMMAP_ERROR

#elif defined CPU4_START_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu4.Unspecified"
	#pragma align 4
	#endif
	#undef CPU4_START_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU4_STOP_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU4_STOP_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU5 code sections                                                         */
/* -------------------------------------------------------------------------- */
#elif defined CPU5_START_SEC_CODE
   #undef CPU5_START_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code="Code.Cpu5"
   #pragma optimize R
   #endif
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_CODE
   #undef CPU5_STOP_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code restore
   #pragma endoptimize
   #endif
   #undef MEMMAP_ERROR
/* -------------------------------------------------------------------------- */
/* CPU5 variable sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU5_START_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu5.8bit"
	#endif
	#undef CPU5_START_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss restore
	#endif
	#undef CPU5_STOP_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
	
#elif defined CPU5_START_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu5.16bit"
	#pragma align 2
	#endif
	#undef CPU5_START_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU5_STOP_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu5.32bit"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU5_STOP_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu5.64bit"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU5_STOP_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.Cpu5.Unspecified"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef CPU5_STOP_SEC_VAR_CLEARED_UNSPECIFIED   
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu5.8bit"
	#endif
	#undef CPU5_START_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata restore
	#endif
	#undef CPU5_STOP_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
	
#elif defined CPU5_START_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu5.16bit"
	#pragma align 2
	#endif
	#undef CPU5_START_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU5_STOP_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu5.32bit"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU5_STOP_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu5.64bit"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU5_STOP_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.Cpu5.Unspecified"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef CPU5_STOP_SEC_VAR_INIT_UNSPECIFIED	
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* CPU5 constant sections                                                     */
/* -------------------------------------------------------------------------- */
#elif defined CPU5_START_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu5.8bit"
	#endif
	#undef CPU5_START_SEC_CONST_8
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom restore
	#endif
	#undef CPU5_STOP_SEC_CONST_8
   #undef MEMMAP_ERROR
	
#elif defined CPU5_START_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu5.16bit"
	#pragma align 2
	#endif
	#undef CPU5_START_SEC_CONST_16
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU5_STOP_SEC_CONST_16
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu5.32bit"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_CONST_32
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU5_STOP_SEC_CONST_32
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu5.64bit"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_CONST_64
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU5_STOP_SEC_CONST_64
   #undef MEMMAP_ERROR

#elif defined CPU5_START_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.Cpu5.Unspecified"
	#pragma align 4
	#endif
	#undef CPU5_START_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined CPU5_STOP_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef CPU5_STOP_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR   

/* -------------------------------------------------------------------------- */
/* Shared code sections                                                       */
/* -------------------------------------------------------------------------- */
#elif defined SHARED_START_SEC_CODE
   #undef SHARED_START_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code="Code.Shared"
   #pragma optimize R
   #endif
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_CODE
   #undef SHARED_STOP_SEC_CODE
   #ifdef _TASKING_C_TRICORE_
   #pragma section code restore
   #pragma endoptimize
   #endif
   #undef MEMMAP_ERROR
/* -------------------------------------------------------------------------- */
/* Shared variable sections                                                   */
/* -------------------------------------------------------------------------- */
#elif defined SHARED_START_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.shared.8bit"
	#endif
	#undef SHARED_START_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_CLEARED_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss restore
	#endif
	#undef SHARED_STOP_SEC_VAR_CLEARED_8
   #undef MEMMAP_ERROR
	
#elif defined SHARED_START_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.shared.16bit"
	#pragma align 2
	#endif
	#undef SHARED_START_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_CLEARED_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef SHARED_STOP_SEC_VAR_CLEARED_16
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.shared.32bit"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_CLEARED_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef SHARED_STOP_SEC_VAR_CLEARED_32
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.shared.64bit"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_CLEARED_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef SHARED_STOP_SEC_VAR_CLEARED_64
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farbss "ClearedData.shared.Unspecified"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_VAR_CLEARED_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_CLEARED_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farbss restore
	#endif
	#undef SHARED_STOP_SEC_VAR_CLEARED_UNSPECIFIED  
   #undef MEMMAP_ERROR 

#elif defined SHARED_START_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.shared.8bit"
	#endif
	#undef SHARED_START_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_INIT_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata restore
	#endif
	#undef SHARED_STOP_SEC_VAR_INIT_8
   #undef MEMMAP_ERROR
	
#elif defined SHARED_START_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.shared.16bit"
	#pragma align 2
	#endif
	#undef SHARED_START_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_INIT_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef SHARED_STOP_SEC_VAR_INIT_16
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.shared.32bit"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_INIT_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef SHARED_STOP_SEC_VAR_INIT_32
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.shared.64bit"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_VAR_INIT_64
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_INIT_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef SHARED_STOP_SEC_VAR_INIT_64	
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section fardata "InitData.shared.Unspecified"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_VAR_INIT_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section fardata restore
	#endif
	#undef SHARED_STOP_SEC_VAR_INIT_UNSPECIFIED
   #undef MEMMAP_ERROR	

/* -------------------------------------------------------------------------- */
/* Shared constant sections                                                   */
/* -------------------------------------------------------------------------- */
#elif defined SHARED_START_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.shared.8bit"
	#endif
	#undef SHARED_START_SEC_CONST_8
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_CONST_8
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom restore
	#endif
	#undef SHARED_STOP_SEC_CONST_8
   #undef MEMMAP_ERROR
	
#elif defined SHARED_START_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.shared.16bit"
	#pragma align 2
	#endif
	#undef SHARED_START_SEC_CONST_16
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_CONST_16
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef SHARED_STOP_SEC_CONST_16
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.shared.32bit"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_CONST_32
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_CONST_32
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef SHARED_STOP_SEC_CONST_32
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.shared.64bit"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_CONST_64
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_CONST_64
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef SHARED_STOP_SEC_CONST_64
   #undef MEMMAP_ERROR

#elif defined SHARED_START_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma section farrom "Const.shared.Unspecified"
	#pragma align 4
	#endif
	#undef SHARED_START_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR
#elif defined SHARED_STOP_SEC_CONST_UNSPECIFIED
	#ifdef _TASKING_C_TRICORE_
	#pragma align restore
	#pragma section farrom restore
	#endif
	#undef SHARED_STOP_SEC_CONST_UNSPECIFIED
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* Calibration sections                                                       */
/* -------------------------------------------------------------------------- */
#elif defined CALIB_START_SEC_CONST_UNSPECIFIED
   #undef  CALIB_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="Calib_unspec"
      #pragma align 4
   #elif __GNUC__
      #pragma section ".rodata.Calib_unspec" a 4
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR
#elif defined CALIB_STOP_SEC_CONST_UNSPECIFIED
    #undef CALIB_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
    #pragma align restore
    #pragma section farrom restore
   #elif __GNUC__
      #pragma section
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR

#elif defined CALIB_START_SEC_CONST_BOOL
   #undef  CALIB_START_SEC_CONST_BOOL
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="Calib_bool"
      #pragma align 1
   #elif __GNUC__
      #pragma section ".rodata.Calib_bool" a 1
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR
#elif defined CALIB_STOP_SEC_CONST_BOOL
    #undef CALIB_STOP_SEC_CONST_BOOL
   #ifdef _TASKING_C_TRICORE_
    #pragma align restore
    #pragma section farrom restore
   #elif __GNUC__
      #pragma section
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR
#elif defined CALIB_START_SEC_CONST_8
   #undef  CALIB_START_SEC_CONST_8
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="Calib_8"
      #pragma align 1
   #elif __GNUC__
      #pragma section ".rodata.Calib_8" a 1
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR
#elif defined CALIB_STOP_SEC_CONST_8
   #undef CALIB_STOP_SEC_CONST_8
   #ifdef _TASKING_C_TRICORE_
      #pragma align restore
      #pragma section farrom restore
   #elif __GNUC__
      #pragma section
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR
#elif defined CALIB_START_SEC_CONST_16
   #undef  CALIB_START_SEC_CONST_16
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="Calib_16"
      #pragma align 2
   #elif __GNUC__
      #pragma section ".rodata.Calib_16" a 2
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR
#elif defined CALIB_STOP_SEC_CONST_16
   #undef CALIB_STOP_SEC_CONST_16
   #ifdef _TASKING_C_TRICORE_
      #pragma align restore
      #pragma section farrom restore
   #elif __GNUC__
      #pragma section
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR
#elif defined CALIB_START_SEC_CONST_32
   #undef  CALIB_START_SEC_CONST_32
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="Calib_32"
      #pragma align 4
   #elif __GNUC__
    #pragma section ".rodata.Calib_32" a 4
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR
#elif defined CALIB_STOP_SEC_CONST_32
   #undef  CALIB_STOP_SEC_CONST_32
   #ifdef _TASKING_C_TRICORE_
      #pragma align restore
      #pragma section farrom restore
   #elif __GNUC__
    #pragma section
   #elif _DIABDATA_C_TRICORE_
   #endif
   #undef MEMMAP_ERROR

/* -------------------------------------------------------------------------- */
/* Boot Mode Header of Tricore devices                                        */
/* -------------------------------------------------------------------------- */
/*BMHD_0_ORIG*/
#elif defined (BMHD_0_ORIG_START_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_0_ORIG_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="BMHD_0_ORIG_CONST_FAR_UNSPECIFIED"
   #elif __GNUC__
      #pragma section ".BMHD_0_ORIG_CONST_FAR_UNSPECIFIED"
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST ".BMHD_0_ORIG_CONST_FAR_UNSPECIFIED" far-absolute R
   #endif
   #undef MEMMAP_ERROR
#elif defined (BMHD_0_ORIG_STOP_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_0_ORIG_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom restore
   #elif __GNUC__
   #pragma section
   #define   BMHD_0_ORIG_STOP_SEC_CONST_UNSPECIFIED
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST R
   #endif
   #undef MEMMAP_ERROR

/*BMHD_0_COPY*/
#elif defined (BMHD_0_COPY_START_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_0_COPY_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="BMHD_0_COPY_CONST_FAR_UNSPECIFIED"
   #elif __GNUC__
      #pragma section ".BMHD_0_COPY_CONST_FAR_UNSPECIFIED"
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST ".BMHD_0_COPY_CONST_FAR_UNSPECIFIED" far-absolute R
   #endif
   #undef MEMMAP_ERROR
#elif defined (BMHD_0_COPY_STOP_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_0_COPY_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom restore
   #elif __GNUC__
   #pragma section
   #define   BMHD_0_COPY_STOP_SEC_CONST_UNSPECIFIED
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST R
   #endif   
   #undef MEMMAP_ERROR

/*BMHD_1_ORIG*/
#elif defined (BMHD_1_ORIG_START_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_1_ORIG_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="BMHD_1_ORIG_CONST_FAR_UNSPECIFIED"
   #elif __GNUC__
      #pragma section ".BMHD_1_ORIG_CONST_FAR_UNSPECIFIED"
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST ".BMHD_1_ORIG_CONST_FAR_UNSPECIFIED" far-absolute R
   #endif
   #undef MEMMAP_ERROR
#elif defined (BMHD_1_ORIG_STOP_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_1_ORIG_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom restore
   #elif __GNUC__
   #pragma section
   #define   BMHD_1_ORIG_STOP_SEC_CONST_UNSPECIFIED
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST R
   #endif
   #undef MEMMAP_ERROR

/*BMHD_1_COPY*/
#elif defined (BMHD_1_COPY_START_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_1_COPY_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="BMHD_1_COPY_CONST_FAR_UNSPECIFIED"
   #elif __GNUC__
      #pragma section ".BMHD_1_COPY_CONST_FAR_UNSPECIFIED"
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST ".BMHD_1_COPY_CONST_FAR_UNSPECIFIED" far-absolute R
   #endif
   #undef MEMMAP_ERROR
#elif defined (BMHD_1_COPY_STOP_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_1_COPY_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom restore
   #elif __GNUC__
   #pragma section
   #define   BMHD_1_COPY_STOP_SEC_CONST_UNSPECIFIED
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST R
   #endif  
   #undef MEMMAP_ERROR

/*BMHD_2_ORIG*/
#elif defined (BMHD_2_ORIG_START_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_2_ORIG_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="BMHD_2_ORIG_CONST_FAR_UNSPECIFIED"
   #elif __GNUC__
      #pragma section ".BMHD_2_ORIG_CONST_FAR_UNSPECIFIED"
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST ".BMHD_2_ORIG_CONST_FAR_UNSPECIFIED" far-absolute R
   #endif
   #undef MEMMAP_ERROR
#elif defined (BMHD_2_ORIG_STOP_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_2_ORIG_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom restore
   #elif __GNUC__
   #pragma section
   #define   BMHD_2_ORIG_STOP_SEC_CONST_UNSPECIFIED
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST R
   #endif
   #undef MEMMAP_ERROR
   
/*BMHD_2_COPY*/
#elif defined (BMHD_2_COPY_START_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_2_COPY_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="BMHD_2_COPY_CONST_FAR_UNSPECIFIED"
   #elif __GNUC__
      #pragma section ".BMHD_2_COPY_CONST_FAR_UNSPECIFIED"
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST ".BMHD_2_COPY_CONST_FAR_UNSPECIFIED" far-absolute R
   #endif
   #undef MEMMAP_ERROR
#elif defined (BMHD_2_COPY_STOP_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_2_COPY_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom restore
   #elif __GNUC__
   #pragma section
   #define   BMHD_2_COPY_STOP_SEC_CONST_UNSPECIFIED
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST R
   #endif 
   #undef MEMMAP_ERROR
/*BMHD_3_ORIG*/
#elif defined (BMHD_3_ORIG_START_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_3_ORIG_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="BMHD_3_ORIG_CONST_FAR_UNSPECIFIED"
   #elif __GNUC__
      #pragma section ".BMHD_3_ORIG_CONST_FAR_UNSPECIFIED"
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST ".BMHD_3_ORIG_CONST_FAR_UNSPECIFIED" far-absolute R
   #endif
   #undef MEMMAP_ERROR
#elif defined (BMHD_3_ORIG_STOP_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_3_ORIG_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom restore
   #elif __GNUC__
   #pragma section
   #define   BMHD_3_ORIG_STOP_SEC_CONST_UNSPECIFIED
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST R
   #endif
   #undef MEMMAP_ERROR
/*BMHD_3_COPY*/
#elif defined (BMHD_3_COPY_START_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_3_COPY_START_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom="BMHD_3_COPY_CONST_FAR_UNSPECIFIED"
   #elif __GNUC__
      #pragma section ".BMHD_3_COPY_CONST_FAR_UNSPECIFIED"
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST ".BMHD_3_COPY_CONST_FAR_UNSPECIFIED" far-absolute R
   #endif
   #undef MEMMAP_ERROR
#elif defined (BMHD_3_COPY_STOP_SEC_CONST_UNSPECIFIED)
   #undef      BMHD_3_COPY_STOP_SEC_CONST_UNSPECIFIED
   #ifdef _TASKING_C_TRICORE_
      #pragma section farrom restore
   #elif __GNUC__
   #pragma section
   #define   BMHD_3_COPY_STOP_SEC_CONST_UNSPECIFIED
   #elif _DIABDATA_C_TRICORE_
      #pragma section CONST R
   #endif
   #undef MEMMAP_ERROR    

/*---------------------------------------------------------------- ---------------------------------------------------*/
/*------------------------------------------End of default section mapping--------------------------------------------*/
/*---------------------------------------------------------------- ---------------------------------------------------*/
#endif  /* START_WITH_IF */

#if defined (MEMMAP_ERROR)
   /* "MemMap.h, the section is undefined" */
  // #error "MemMap.h, the section is undefined"
#endif   
/*------------------------------------------------------End of file---------------------------------------------------*/
