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
 * @file      REG_FD.h
 * @brief     define moe register macros
 * @author    wangjg
 * @since     2012-02-24
 * @date      2012-02-24
*/

#ifndef __REG_FD_H__
#define __REG_FD_H__

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
#define REG_FD_Version	0x20120224

#define REG_FD_Intr							0x2e80//R[0]fdDone,[1]fd out of bound
#define REG_FD_IntrEnable					0x2e81//[0]fdDone
#define REG_FD_SWReset						0x2e82// 1:reset,0:normal
#define REG_FD_ClockGating					0x2e83// 
#define REG_FD_Trigger						0x2e84// w(1)trigger,r(1):busy,r(0)idle
#define REG_FD_DataBaseAddr0				0x2e88//[7:0]
#define REG_FD_DataBaseAddr1				0x2e89//[15:8]
#define REG_FD_DataBaseAddr2				0x2e8a//[23:16]
#define REG_FD_DataBaseAddr3				0x2e8b//[24]
#define REG_FD_OutStartAddr0					0x2e8c//[7:0]
#define REG_FD_OutStartAddr1					0x2e8d//[15:8]
#define REG_FD_OutStartAddr2					0x2e8e//[23:16]
#define REG_FD_OutStartAddr3					0x2e8f//[24]

#define REG_FD_OutCurrentAddr0					0x2e90//[7:0]
#define REG_FD_OutCurrentAddr1					0x2e91//[15:8]
#define REG_FD_OutCurrentAddr2					0x2e92//[23:16]
#define REG_FD_OutCurrentAddr3					0x2e93//[24]
#define REG_FD_OutMaxCnt0						0x2e94//[7:0]
#define REG_FD_OutMaxCnt1						0x2e95//[9:8]

#define REG_FD_SubWinStep						0x2e96//[1:0]1,2,4
#define REG_FD_SaleFactor						0x2e97//
#define REG_FD_Areafactor0						0x2e98//[7:0]
#define REG_FD_Areafactor1						0x2e99//[15:8]
#define REG_FD_Areafactor00						0x2e9a//[7:0]
#define REG_FD_Areafactor01						0x2e9b//[15:8]
#define REG_FD_ROIx0							0x2e9c//[7:0]
#define REG_FD_ROIy0							0x2e9d//[7:0]
#define REG_FD_ROIx1							0x2e9e//[7:0]
#define REG_FD_ROIy1							0x2e9f//[7:0]



#define REG_FD_DataBaseRotation						0x2eA0//[0],[1],[2]
#define REG_FD_WinSize								0x2ea1//[4:0]
#define REG_FD_SigmaOpt								0x2ea2// 0:disable,1:enable
#define REG_FD_StageMax								0x2ea4//[4:0]

#define REG_FD_DB0									0x2eac
#define REG_FD_DB1									0x2ead
#define REG_FD_DB2									0x2eae// 
#define REG_FD_DB3									0x2eaf//

#define REG_FD_IIGrantsCount0					0x2ec0
#define REG_FD_IIGrantsCount1					0x2ec1
#define REG_FD_IIGrantsCount2					0x2ec2
#define REG_FD_IIGrantsCount3					0x2ec3
#define REG_FD_SIIGrantsCount0					0x2ec4
#define REG_FD_SIIGrantsCount1					0x2ec5
#define REG_FD_SIIGrantsCount2					0x2ec6
#define REG_FD_SIIGrantsCount3					0x2ec7
#define REG_FD_FeatureGrantsCount0				0x2ec8
#define REG_FD_FeatureGrantsCount1				0x2ec9
#define REG_FD_FeatureGrantsCount2				0x2eca
#define REG_FD_FeatureGrantsCount3				0x2ecb

#define REG_FD_TagEnable						0x2ed0

#define REG_FD_Tag000						0x2ee0
#define REG_FD_Tag001						0x2ee1
#define REG_FD_Tag010						0x2ee2
#define REG_FD_Tag011						0x2ee3
#define REG_FD_Tag020					0x2ee4
#define REG_FD_Tag021					0x2ee5
#define REG_FD_Tag030					0x2ee6
#define REG_FD_Tag031					0x2ee7
#define REG_FD_Tag040					0x2ee8
#define REG_FD_Tag041					0x2ee9
#define REG_FD_Tag050					0x2eea
#define REG_FD_Tag051					0x2eeb
#define REG_FD_Tag060					0x2eec
#define REG_FD_Tag061					0x2eed
#define REG_FD_Tag070					0x2eee
#define REG_FD_Tag071					0x2eef

#define REG_FD_TSelect						0x2ef0//
#define REG_FD_fpobsel						0x2ef1
#define REG_FD_Probe0						0x2ef4
#define REG_FD_Probe1						0x2ef5
#define REG_FD_DateCode0					0x2efc
#define REG_FD_DateCode1					0x2efd
#define REG_FD_DateCode2					0x2efe
#define REG_FD_DateCode3					0x2eff

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
