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
 * @file      REG_II.h
 * @brief     define moe register macros
 * @author    wangjg
 * @since     2012-02-24
 * @date      2012-02-24
*/

#ifndef __REG_II_H__
#define __REG_II_H__

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
#define REG_II_Version	0x20120224

#define REG_II_Intr							0x2e40//R[0]iiTotal,[1]iiSub,[2]iiOvflow
#define REG_II_IntrEnable					0x2e41//[0]iiTotal,[1]iiSub,[2]iiOvflow
#define REG_II_ModuleControl				0x2e42//[0]iiRst,[4]iiClockGating
#define REG_II_CmdSrc						0x2e43//0:cpu control,1:Auto
#define REG_II_CmdQueueAddr0				0x2e44//[7:0]
#define REG_II_CmdQueueAddr1				0x2e45//[15:8]
#define REG_II_CmdQueueAddr2				0x2e46//[23:16]
#define REG_II_CmdQueueAddr3				0x2e47//[24]
#define REG_II_CmdTrigger					0x2e4c//W

#define REG_II_Status						0x2e50//R[0]iiTotal Queue,[1]operation (0:busy,1:idle)
#define REG_II_FetchAddr0					0x2e54
#define REG_II_FetchAddr1					0x2e55
#define REG_II_FetchAddr2					0x2e56
#define REG_II_FetchAddr3					0x2e57
#define REG_II_Dummy						0x2e5e//R 
#define REG_II_TSelect						0x2e5f//R 

//cmd
#define REG_II_Func							0x2e61//[1:0]--0:scale II,1:scale SII,2:II Scaleing Down
#define REG_II_ROIHSize0					0x2e62
#define REG_II_ROIHSize1					0x2e63
#define REG_II_ROIVSize0					0x2e64
#define REG_II_ROIVSize1					0x2e65
#define REG_II_HFactorIni0					0x2e66
#define REG_II_HFactorIni1					0x2e67
#define REG_II_VFactorIni0					0x2e68
#define REG_II_VFactorIni1					0x2e69
#define REG_II_HScaleFactor0				0x2e6a
#define REG_II_HScaleFactor1				0x2e6b
#define REG_II_VScaleFactor0				0x2e6c
#define REG_II_VScaleFactor1				0x2e6d

#define REG_II_SrcAddr0						0x2e70
#define REG_II_SrcAddr1						0x2e71
#define REG_II_SrcAddr2						0x2e72
#define REG_II_SrcAddr3						0x2e73
#define REG_II_SrcHSize0					0x2e74
#define REG_II_SrcHSize1					0x2e75
#define REG_II_SrcHOff2					0x2e76
#define REG_II_SrcHOff3					0x2e77
#define REG_II_SrcVOff0					0x2e78
#define REG_II_SrcVOff1					0x2e79
#define REG_II_DstAddr0					0x2e7a
#define REG_II_DstAddr1					0x2e7b
#define REG_II_DstAddr2					0x2e7c
#define REG_II_DstAddr3					0x2e7d
#define REG_II_DstHSize					0x2e7e

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



#endif
