/******************************************************************************
 * \file    Compiler.h
 * \brief   AUTOSAR 编译器抽象 (Specification of Compiler Abstraction)
 *****************************************************************************/
#ifndef COMPILER_H
#define COMPILER_H

#define AUTOMATIC
#define TYPEDEF
#define STATIC   static
#define INLINE   inline
#define FUNC(rettype, memclass)            rettype
#define P2VAR(ptrtype, memclass, ptrclass) ptrtype *
#define P2CONST(ptrtype, memclass, ptrclass) const ptrtype *

#endif /* COMPILER_H */
