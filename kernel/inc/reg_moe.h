/**************************************************************************
 *         Copyright(c) 2010 by i-Catch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of i-Catch Tech-  *
 *  nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of i-Catch Technology Co., Ltd.                 *
 *                                                                        *
 *  i-Catch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  i-Catch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      REG_MOE.h
 * @brief     define moe register macros
 * @author    wangjg
 * @since     2010-03-01
 * @date      2010-04-21
*/

#ifndef _REG_MOE_H_
#define _REG_MOE_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define REG_MOE_Version	0x20100305

#define REG_Moe_Intr						0x2e00//MOE_STRUCT_REG_INTR
#define REG_Moe_IntrEnable					0x2e01//MOE_STRUCT_REG_INTR
#define REG_Moe_ModuleControl				0x2e02//0x01
#define REG_Moe_OperationMode				0x2e03
#define REG_Moe_CmdQueueAddr0				0x2e04
#define REG_Moe_CmdQueueAddr1				0x2e05
#define REG_Moe_CmdQueueAddr2				0x2e06
#define REG_Moe_CmdQueueAddr3				0x2e07
#define REG_Moe_Div0Data0					0x2e08
#define REG_Moe_Div0Data1					0x2e09
#define REG_Moe_CmdTrigger					0x2e0c
#define REG_Moe_SumOutputEn				0x2e0d //0305

#define REG_Moe_CmdStatus					0x2e10//R 
#define REG_Moe_Month						0x2e11//R 
#define REG_Moe_Date						0x2e12//R 
#define REG_Moe_Hour						0x2e13//R  //??
#define REG_Moe_CmdFetchAddr0				0x2e14
#define REG_Moe_CmdFetchAddr1				0x2e15
#define REG_Moe_CmdFetchAddr2				0x2e16
#define REG_Moe_CmdFetchAddr3				0x2e17
#define REG_Moe_DataSum0					0x2e18//R 
#define REG_Moe_DataSum1					0x2e19//R 
#define REG_Moe_DataSum2					0x2e1a//R 
#define REG_Moe_DataSum3					0x2e1b//R 
#define REG_Moe_DataMin0					0x2e1c//R 
#define REG_Moe_DataMin1					0x2e1d//R 
#define REG_Moe_DataMax0					0x2e1e//R 
#define REG_Moe_DataMax1					0x2e1f//R 
//cmd
#define REG_Moe_CmdOpt						0x2e20//0303
#define REG_Moe_CmdFunc					0x2e21
#define REG_Moe_CmdROIWidth0				0x2e22
#define REG_Moe_CmdROIWidth1				0x2e23
#define REG_Moe_CmdMin0					0x2e24
#define REG_Moe_CmdMin1					0x2e25
#define REG_Moe_CmdMin2					0x2e26
#define REG_Moe_CmdMin3					0x2e27
#define REG_Moe_CmdMax0					0x2e28
#define REG_Moe_CmdMax1					0x2e29
#define REG_Moe_CmdMax2					0x2e2a
#define REG_Moe_CmdMax3					0x2e2b
#define REG_Moe_CmdCP0						0x2e2c
#define REG_Moe_CmdCP1						0x2e2d

#define REG_Moe_CmdSrc10					0x2e30
#define REG_Moe_CmdSrc11					0x2e31
#define REG_Moe_CmdSrc12					0x2e32
#define REG_Moe_CmdSrc13					0x2e33
#define REG_Moe_CmdSrc20					0x2e34
#define REG_Moe_CmdSrc21					0x2e35
#define REG_Moe_CmdSrc22					0x2e36
#define REG_Moe_CmdSrc23					0x2e37
#define REG_Moe_CmdDst0					0x2e38
#define REG_Moe_CmdDst1					0x2e39
#define REG_Moe_CmdDst2					0x2e3a
#define REG_Moe_CmdDst3					0x2e3b

#define REG_Moe_bdpxvsize_low			0x2e3c
#define REG_Moe_bdpxvsize_high			0x2e3d

#define REG_Moe_PSelect						0x2e3e//
#define REG_Moe_TSelect						0x2e3f
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
//REG_Moe_ModuleControl
typedef struct	
{
	UINT8 swrst:1;
	UINT8 null0:3;
	UINT8 disclkgate:1;
	UINT8 null1:3;
}moe_reg_module_control_t;

//REG_Moe_Intr
//REG_Moe_IntrEnable
typedef struct 
{
	UINT8 cmdQueueIntr:1;
	UINT8 cmdIntr:1;
	UINT8 div0Intr:1;
	UINT8 shiftOverflow:1;
	UINT8 null0:4;
                                
}moe_reg_intr_t;

//REG_Moe_CmdStatus
typedef struct 
{
	UINT8 queueBusy:1;//0: is busy 1:not process
	UINT8 cmdBusy:1;//0: is busy 1:not process
	UINT8 null0:6;
                                
}moe_reg_status_t;

//REG_Moe_CmdFunc
typedef struct 
{	
	UINT8 func:1;// 1¡¯h0 : point to point operation 1¡¯h1 : operation with common point
	UINT8 exchange:1;// Source 1 & Source 2 exchange
	UINT8 sum:1;// 1¡¯h1 : div-sum 1¡¯h0 : mul-sum
	UINT8 null0:1;

	UINT8 equation:3;
	//h0: Add 
	//h1: Subtract 
	//h2: Multiply 
	//h3: Division 
	//h4: Absolute difference
	//h5: Max
	//h6: Min
	//h7: summation

	UINT8 null1:1;
}moe_reg_func_t;

//REG_Moe_CmdOpt
typedef struct
{
	UINT8 null0:4;
	UINT8 src1ClipEn:1;
	UINT8 src2ClipEn:1;
	UINT8 dstClipEn:1;
	UINT8 null1:1;
}moe_reg_opt_t;

//REG_Moe_CmdSrc13
//REG_Moe_CmdSrc23
//REG_Moe_CmdDst3
typedef struct
{	
	UINT8 addr3:1;// src addr	
	UINT8 sign:1;// 0: unsigned 1:signed	
	UINT8 format:1;// 0: 8bit 1: 16bit	
	UINT8 shift:5;// shift  :src left/ dest right
}moe_reg_addr_t; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/




#endif
