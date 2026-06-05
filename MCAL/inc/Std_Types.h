/******************************************************************************
 * \file    Std_Types.h
 * \brief   AUTOSAR 标准类型定义 (Specification of Standard Types)
 * \details 项目级公共类型, 供 MCAL / CDD / BSW / ASW 共用。
 *****************************************************************************/
#ifndef STD_TYPES_H
#define STD_TYPES_H

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef signed char    sint8;
typedef signed short   sint16;
typedef signed int     sint32;
typedef unsigned char  boolean;

typedef uint8 Std_ReturnType;

#ifndef TRUE
#define TRUE  ((boolean)1U)
#endif
#ifndef FALSE
#define FALSE ((boolean)0U)
#endif

#ifndef NULL_PTR
#define NULL_PTR ((void *)0)
#endif

#define E_OK     ((Std_ReturnType)0x00U)
#define E_NOT_OK ((Std_ReturnType)0x01U)

#define STD_HIGH 0x01U
#define STD_LOW  0x00U
#define STD_ON   0x01U
#define STD_OFF  0x00U

#endif /* STD_TYPES_H */
