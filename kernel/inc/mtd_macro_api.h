/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		mtd_macro_api.h
 * @brief		Macros api for MTD driver
 * @author	Phil Lin
 * @since		2008-07-01
 * @date		2008-07-01
 */
#ifndef __MTD_MACRO_API_H__
#define __MTD_MACRO_API_H__
/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "mtd_general.h"
#include "smccon.h"
#include "Dbg_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define MTD_NandSendCmd(cmd)	\
do {\
	UINT8 tmp;\
	tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT);\
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x0A & (~WPBIT))|tmp;\
	XBYTE[REG_Fm_NandDataPort] = (cmd);\
	XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp;\
} while (0)

#if (MTC_NAND_TIMEOUT_SUPPORT && MTD_NAND_TIMEOUT_RST)
#define MTD_NandTimeOutRst()		MTD_NandSendCmd(K_SMC_Reset)
#else
#define MTD_NandTimeOutRst()
#endif /*MTD_NAND_TIMEOUT_RST*/

									
#if MTC_NAND_TIMEOUT_SUPPORT

#define MTD_NandAutoCmdTrig()	\
do {\
	UINT16 timeOut = MTD_NAND_AUTO_CMD_TIMEOUT;\
	while (1) {\
/*wait idle*/	if (XBYTE[REG_Fm_autosendtrg_nandstate] == 0x02) {\ 
/*trigger*/	XBYTE[REG_Fm_autosendtrg_nandstate] = 0x01;\  	
			break;\
		}\
		if (!(--timeOut)) {\
			MTD_NandTimeOutRst();\
			ASSERT(0, 1);\
			break;\
		}\
	}\											
} while(0)

#else

#define MTD_NandAutoCmdTrig()	\
do {\
	while (!(XBYTE[REG_Fm_autosendtrg_nandstate] == 0x02));\
	XBYTE[REG_Fm_autosendtrg_nandstate] = 0x01;\
} while(0)

#endif /*MTC_NAND_TIMEOUR_SUPPORT*/

#if MTC_NAND_TIMEOUT_SUPPORT

#define MTD_NandAutoCmdWait()	\
do {\
	UINT16 timeOut = MTD_NAND_AUTO_CMD_TIMEOUT;\
	while (1) {\
		if (XBYTE[REG_Fm_autosendtrg_nandstate] == 0x02) {\
			XBYTE[REG_Fm_cmdsize_addrsize] = 0;\
			break;\
		}\
		if (!(--timeOut)) {\
			MTD_NandTimeOutRst();\
			ASSERT(0, 1);\
			break;\
		}\
	}\										
} while(0)

#else

#define MTD_NandAutoCmdWait()	\
do {\
	while (XBYTE[REG_Fm_autosendtrg_nandstate] == 0x02);\
	XBYTE[REG_Fm_cmdsize_addrsize] = 0;\
} while(0)


#endif /*MTC_NAND_TIMEOUR_SUPPORT*/



#define MTD_ColumnLatch(col) \
do {\	
		UINT8 tmp; \
		tmp = XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT); \
		XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x06 & (~WPBIT))|tmp; \				
		XBYTE[REG_Fm_NandDataPort] = col & 0xff; \		
		XBYTE[REG_Fm_NandDataPort] = (col & 0xff00) >> 8; \	
		XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (0x02 & (~WPBIT))|tmp; \				
} while(0)
	



#define MTD_NandCtrl(mask)			do {XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] = (mask & (~WPBIT))|(XBYTE[REG_Fm_Nandcsnn_wpnn_ale_cle] & (WPBIT));} while(0)

#define MTD_EccDisable()				do {XBYTE[REG_Fm_ECCMask] = 0x01;} while(0)

#define MTD_EccEnable()				do {XBYTE[REG_Fm_ECCMask] = 0x00;} while(0)

#define MTD_EccClear()				do {XBYTE[REG_Fm_ECCRst] = 0x01;} while(0)

#define MTD_EccRedunFill(address)	\
do {\
/* 0*/	XBYTE[REG_Fm_NandDataPort] = 0xff;	\	
/* 1*/	XBYTE[REG_Fm_NandDataPort] = 0xff;	\
/* 2*/	XBYTE[REG_Fm_NandDataPort] = 0xff;	\	
/* 3*/	XBYTE[REG_Fm_NandDataPort] = 0xff;	\
/* 4*/	XBYTE[REG_Fm_NandDataPort] = 0xff;\
/* 5*/	XBYTE[REG_Fm_NandDataPort] = 0xff;\		
/* 6*/	XBYTE[REG_Fm_NandDataPort] = READ8((address), 0);\		
/* 7*/	XBYTE[REG_Fm_NandDataPort] = READ8((address), 1);\		
/* 8*/	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_3];\			
/* 9*/	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_4];\			
/*10*/	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_5];\			
/*11*/	XBYTE[REG_Fm_NandDataPort] = READ8((address), 0);\		
/*12*/	XBYTE[REG_Fm_NandDataPort] = READ8((address), 1);\		
/*13*/	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_0];\			
/*14*/	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_1];\			
/*15*/	XBYTE[REG_Fm_NandDataPort] = XBYTE[REG_Fm_ECC_2];\				
} while(0)

#define MTD_DMATrig()				do {XBYTE[REG_Dma_DmaStart] = 0x01;} while(0)

#define MTD_SelfShiftR8(a) \
{\
	WRITE8((a), 3, READ8((a), 2));\
	WRITE8((a), 2, READ8((a), 1));\
	WRITE8((a), 1, READ8((a), 0));\
	WRITE8((a), 0, 0); \
}

#define MTD_SelfShiftR16(a) \
{\
	WRITE8((a), 3, READ8((a), 2));\
	WRITE8((a), 2, READ8((a), 1));\
	WRITE8((a), 1, 0);\
	WRITE8((a), 0, 0); \
}

#define MTD_SelfShiftR24(a) \
{\
	WRITE8((a), 3, READ8((a), 2));\
	WRITE8((a), 2, 0);\
	WRITE8((a), 1, 0);\
	WRITE8((a), 0, 0); \
}

#define MTD_SelfShiftL8(a) \
{\
	WRITE8((a), 0, READ8((a), 1)); \
	WRITE8((a), 1, READ8((a), 2));\
	WRITE8((a), 2, READ8((a), 3));\
	WRITE8((a), 3, 0);\
}

#define MTD_SelfShiftL16(a) \
{\
	WRITE8((a), 0, READ8((a), 1)); \
	WRITE8((a), 1, READ8((a), 2));\
	WRITE8((a), 2, 0);\
	WRITE8((a), 3, 0);\
}

#define MTD_SelfShiftL24(a) \
{\
	WRITE8((a), 0, READ8((a), 1)); \
	WRITE8((a), 1, 0);\
	WRITE8((a), 2, 0);\
	WRITE8((a), 3, 0);\
}


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#endif /*__MTD_MACRO_API_H__*/
