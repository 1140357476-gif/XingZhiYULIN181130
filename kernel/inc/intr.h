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

#ifndef __INTR_H__
#define __INTR_H__
//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------

typedef enum {
	//shift opration is slow than using const variable
	INTR_SRC_TYPE_INTR0_N = 0x0001,	//(0x0001 << 0)
	INTR_SRC_TYPE_GLOBAL = 0x0003,		//((0x0001 << 1) | (0x0001 << 0))
	INTR_SRC_TYPE_AUD = 0x0005,		//((0x0001 << 2) | (0x0001 << 0))
	INTR_SRC_TYPE_FMGPIO = 0x0009,	//((0x0001 << 3) | (0x0001 << 0))
	INTR_SRC_TYPE_USB = 0x0011,		//((0x0001 << 4) | (0x0001 << 0))
	INTR_SRC_TYPE_DRAM = 0x0021,		//((0x0001 << 5) | (0x0001 << 0))
	INTR_SRC_TYPE_TVENC = 0x0041,		//((0x0001 << 6) | (0x0001 << 0))
	INTR_SRC_TYPE_DMA = 0x0081,		//((0x0001 << 7) | (0x0001 << 0))
	INTR_SRC_TYPE_FRONT = 0x0101,		//((0x0001 << 8) | (0x0001 << 0))
	INTR_SRC_TYPE_CDSP = 0x0201,		//((0x0001 << 9) | (0x0001 << 0))
	INTR_SRC_TYPE_JPG = 0x0401,		//((0x0001 << 10) | (0x0001 << 0))
	INTR_SRC_TYPE_GPRM = 0x0801,		//((0x0001 << 11) | (0x0001 << 0))
	INTR_SRC_TYPE_FD = 0x1001,			//((0x0001 << 12) | (0x0001 << 0))
} intrType_t;

typedef UINT8 (code* pIntrFunc)(void);

/**/
//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------
#define intrPutChar(c) INTR_putchar(c)
#define intrGetChar(v)  (((v)<=9)?('0'+(v)):('A'+(v)-10))
#define intrPutUint8(val) 	intrPutChar(intrGetChar((val)>>4));\
						intrPutChar(intrGetChar((val)&0x0f))
				
#define intrPutUint16(val) intrPutUint8((val)>>8);\
						intrPutUint8((val)&0xff)

#define intrPutUint32(val) INTR_putUINT32(val)//intrPutUint16((val)>>16);\
						//intrPutUint16((val)&0xffff);

/* dbg. */
#define _intrPutChar(c)		/intrPutChar(c)
#define _intrGetChar(v)		/intrGetChar(v)
#define _intrPutUint8(val)	/intrPutUint8(val)
#define _intrPutUint16(val)	/intrPutUint16(val)
#define _intrPutUint32(val)	/intrPutUint32(val)

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------
void INTR_Int0(void);
void INTR_Enter(void);
void INTR_Leave(void);
void INTR_DisableInterrupt(void);
void INTR_EnableInterrupt(void);
void INTR_CtlInOutEnable(void);
char putchar (char c);
void INTR_putUINT32(UINT32 val);
#endif /*__INTR_H__*/
