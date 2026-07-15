/*******************************************************************************
**                                                                            **
** Copyright (C) Infineon Technologies (2020)                                 **
**                                                                            **
** All rights reserved.                                                       **
**                                                                            **
** This document contains proprietary information belonging to Infineon       **
** Technologies. Passing on and copying of this document, and communication   **
** of its contents is not permitted without prior written authorization.      **
**                                                                            **
********************************************************************************
**                                                                            **
**  FILENAME  : Compiler.h                                                    **
**                                                                            **
**  VERSION   : 1.40.0_9.0.0                                                  **
**                                                                            **
**  DATE      : 2020-01-17                                                    **
**                                                                            **
**  VARIANT   : NA                                                            **
**                                                                            **
**  PLATFORM  : Infineon AURIX2G                                              **
**                                                                            **
**  AUTHOR    : DL-AUTOSAR-Engineering                                        **
**                                                                            **
**  VENDOR    : Infineon Technologies                                         **
**                                                                            **
**  TRACEABILITY : [cover parentID={FB97B5EA-8796-4991-B969-37114769C0BA}]    **
**                                                                            **
**  DESCRIPTION  : Compiler Abstraction Macros                                **
**                                                                            **
**  [/cover]                                                                  **
**                                                                            **
**  SPECIFICATION(S) : AUTOSAR_SWS_CompilerAbstraction, AUTOSAR Release 4.2.2 **
**                                                                            **
**  MAY BE CHANGED BY USER : no                                               **
**                                                                            **
*******************************************************************************/
#ifndef COMPILER_H
#define COMPILER_H

/******************************************************************************
**                      Includes                                             **
******************************************************************************/
#include "Compiler_Cfg.h"

/******************************************************************************
**                      Global Macro Definitions                             **
******************************************************************************/
/*
 * Vendor ID of the dedicated implementation of this module according
 * to the AUTOSAR vendor list
 */
#define COMPILER_VENDOR_ID        (17u)


#define COMPILER_AR_RELEASE_MAJOR_VERSION    (4u)
#define COMPILER_AR_RELEASE_MINOR_VERSION    (2u)
#define COMPILER_AR_RELEASE_REVISION_VERSION (2u)


#define FUNC(rettype, memclass) rettype
#define P2VAR(ptrtype, memclass, ptrclass) ptrtype *
#define P2CONST(ptrtype, memclass, ptrclass) const ptrtype *
#define CONSTP2VAR(ptrtype, memclass, ptrclass) ptrtype * const
#define CONSTP2CONST(ptrtype, memclass, ptrclass) const ptrtype * const
#define P2FUNC(rettype, ptrclass, fctname) rettype (*fctname)
#define CONST(consttype, memclass) const consttype
#define VAR(vartype, memclass) vartype
#define FUNC_P2CONST( type , AUTOMATIC, ECUM_CALLOUT_CODE ) const type *

// #if defined(__HIGHTEC__)
// #define _GNU_C_TRICORE_ 1U
// /*
//  * HIGHTEC V4.9.2.0
//  */

// #endif

// #if defined (__TASKING__)
// #define _TASKING_C_TRICORE_ 1U
// /*
//  * TASKING V6.3R1P1
//  * TASKING V6.2R2P2
//  */

// #endif

// #if defined(__DCC__)
// #define _DIABDATA_C_TRICORE_ 1U
// /*
//  * WINDRIVER 5.9.6.4
//  */

// #endif

// #if defined (__ghs__)
// #define _GHS_C_TRICORE_ 1U
// /*
//    * GREEN HILLS V2018.1.5 R9.15.1
//  */

// #endif

/*used for local non static variables*/
#define AUTOMATIC

#define TYPEDEF

#define NULL_PTR ((void *)0)

/*abstraction of the keyword inline in functions with static scope*/
#ifdef _GNU_C_TRICORE_

#ifndef INLINE
#define INLINE      __inline__
#endif

#ifndef LOCAL_INLINE
#define LOCAL_INLINE    static __inline__
#endif

#endif

#ifdef _TASKING_C_TRICORE_

#ifndef INLINE
#define INLINE      inline
#endif

#ifndef LOCAL_INLINE
#define LOCAL_INLINE    static inline
#endif

#endif

#ifdef _DIABDATA_C_TRICORE_

#ifndef INLINE
#define INLINE      __inline__
#endif

#ifndef LOCAL_INLINE
#define LOCAL_INLINE  static __inline__
#endif

#endif

#ifdef _GHS_C_TRICORE_

#ifndef INLINE
#define INLINE     __inline__
#endif

#ifndef LOCAL_INLINE
#define LOCAL_INLINE   static inline
#endif

#endif


/******************************************************************************
**                      Global Type Definitions                              **
******************************************************************************/

/******************************************************************************
**                      Global Constant Declarations                         **
******************************************************************************/

/******************************************************************************
**                      Global Variable Declarations                         **
******************************************************************************/

/******************************************************************************
**                      Global Function Declarations                         **
******************************************************************************/
#endif /* } COMPILER_H  */

