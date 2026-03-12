/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__
//=============================================================================
//Symbol
//=============================================================================
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
#ifndef NULL
#define NULL		((void *)0)
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef SUCCESS
#define SUCCESS	0
#endif

#ifndef FAIL
#define FAIL		1
#endif

#ifndef UNKNOWN
#define UNKNOWN		(-1)
#endif

#ifndef ENABLE
#define ENABLE		1
#endif

#ifndef DISABLE
#define DISABLE		0
#endif

#define BOOL 	UINT8

//#define BYTE		unsigned char
//#define WORD	unsigned short
//#define DOUBLEWORD  	unsigned long
//------------------------------------------------------------------------------
//TYPE DEFINE
//------------------------------------------------------------------------------
// 使用unsigned char代替bit，因为bit不是标准C类型
typedef unsigned char BIT;

typedef unsigned char UINT8;
typedef unsigned short int UINT16;
typedef unsigned long UINT32;

typedef signed char SINT8;
typedef signed short int SINT16;
typedef signed long SINT32;


typedef float FINT32; 

#endif /*__COMMON_H__*/
