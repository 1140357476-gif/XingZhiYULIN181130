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
 * @file      fd_def.h
 * @brief     Brief
 * @author   wangjg
 * @since     2010-04-01
 * @date      2010-04-01
*/
#ifndef _FD_DEF_H__
#define _FD_DEF_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_fd.h"
#include "sp1k_fd_api.h" 

#include "initio.h"
#include "utility.h"
#include "intr.h"
#include "os_msg.h"

#include "hal_fd.h"
#include "hal.h"
#include "hal_Dram.h"
#include "hal_gprm.h"
#include "hal_cdsp.h"

#include "sp1k_rsvblk_api.h"
#include "dbg_def.h"

/**************************************************************************
 *                          H W       C O N S T A N T S                            *
 **************************************************************************/
/* HW DataBase Stage */
#define FD_DATABASE_STAGE_0		19	//fd flow used
#define FD_DATABASE_STAGE_1		19	//smile flow used
#define FD_DATABASE_STAGE_2		17	//nonsmile flow used

/* HW FD Sub-Win Step(Pixel) */
#define FD_FACE_SUB_WIN_STEP	FD_SUBWIN_MOVE_STEP_2	//face
#define FD_SMILE_SUB_WIN_STEP	FD_SUBWIN_MOVE_STEP_2	//smile, nonsmile

/* HW FD Detected Output Max Count */
#define FD_DECTED_OutCnt		16	//face	
#define FD_DECTED_OutCntSub 	8	//smile, nonsmile

/**************************************************************************
 *                          F W      C O N S T A N T S                            *
 **************************************************************************/

/* DataBase Resource File ID */
#define RES_ID_Face 			0x46
#define RES_ID_Smile 			0x4a
#define RES_ID_NonSmile			0x4b
#define RES_ID_Eye 				0x4c
#define RES_ID_CloseEye 		0x4d

/* FD Test Command Option */
#define FD_TEST					1

/* FD II Number (Real = FD_II_CNT[Sub-II] + 1[160*120-II]) */
#define FD_II_CNT				12

/* Face Queue Size */
#define FD_FACE_CNT				20

/* FD II Source H,V Size */
#define FD_SIZE_IIX				160ul
#define FD_SIZE_IIY				120ul

/* FW FD Detected Face Size Level (Based on FD II Number) */
#define FD_DECTED_Cnt			13

#define FD_SIZE_RstFace			(FD_DECTED_OutCnt*FD_DECTED_Cnt*2)//416ul //16*13ul*2
#define FD_SIZE_RstSub			(FD_DECTED_OutCntSub*(FD_DECTED_Cnt-1)*2)//208ul//104ul //4*13*2

#define FD_SIZE_Queue			(sizeof(fd_iijobtable_t)*14ul)

/* define sii h size */
#define FD_SII_HSIZE			160

/* define main(fd), sub0(smile), sub1(nonsmile) database size */
#define FD_SIZE_DBMain			20000ul
#define FD_SIZE_DBSub			30000ul
#define FD_SIZE_DataBase		(FD_SIZE_DBMain+FD_SIZE_DBSub*2)//80000ul

/* fd(main), sub0(smile), sub1(nonsmile) database dram address */
#define FD_ADDR_DBFace			fdTblAddr
#define FD_ADDR_DBSub0			(FD_ADDR_DBFace+FD_SIZE_DBMain/2)
#define FD_ADDR_DBSub1			(FD_ADDR_DBSub0+FD_SIZE_DBSub/2)

/* fd, smile, nonsmile ii queue dram address */
#define FD_ADDR_QueueFace		fdJobAddr
#define FD_ADDR_QueueSub0		(FD_ADDR_QueueFace+FD_SIZE_Queue/2)
#define FD_ADDR_QueueSub1		(FD_ADDR_QueueFace+FD_SIZE_Queue)
#define FD_ADDR_QueueSub2		(FD_ADDR_QueueFace+FD_SIZE_Queue/2*3)

/* mask */
#define FD_SHOW_Step			0x0f
#define FD_SHOW_Draw			0x10
#define FD_SHOW_Key				0x20
#define FD_SHOW_Smile			0x40
#define FD_SHOW_Valid			0x80

#define FD_WP_Delay				0x0f
#define FD_WP_Count				0xf0

/* smile dynamic adjust based on 1 path h size */
#define DYNAMIC_SMILE_CTRL_SRC		0	//0:normal flow(1 path 320*240 yuv, 2 path 640*480 yuv)
										//1:dynamic switch 1,2 path smile yuv src
										//2:always see 2 path(2 path 320*240)
										

/**************************************************************************
 *                              D E F A U L T                               *
 **************************************************************************/
/* Default value */
#define DEFAULT_RES_ID			RES_ID_Face
#define DEFAULT_ROTATE			FD_ROTATE_Normal
#define DEFAULT_DB_ADDRESS		FD_ADDR_DBFace

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define FD_PROC_V1	1

#define fdStepFac(hsize)		2560/hsize
#define fdStepRadius(hsize)		1200/hsize
#define fdStepArea(hsize)		(((UINT32)hsize*hsize)<<12)/5625//(((UINT32)hsize*hsize)<<22)/5760000
#define fdStepVSize(hsize)		(((UINT16)hsize*3)>>2)

/**************************************************************************
 *                            A R R A Y  M A C R O S                               *
 **************************************************************************/
#define USE_ARRAY_MACROS	1

/* face array macros */
#define FD_II_H_SIZE_0	24
#define FD_II_H_SIZE_1	28
#define FD_II_H_SIZE_2	33
#define FD_II_H_SIZE_3	39
#define FD_II_H_SIZE_4	45
#define FD_II_H_SIZE_5	53
#define FD_II_H_SIZE_6	62
#define FD_II_H_SIZE_7	73
#define FD_II_H_SIZE_8	85
#define FD_II_H_SIZE_9	100
#define FD_II_H_SIZE_10	117
#define FD_II_H_SIZE_11	137
#define FD_II_H_SIZE_12	160

#define FD_II_V_SIZE_0	fdStepVSize(FD_II_H_SIZE_0)
#define FD_II_V_SIZE_1	fdStepVSize(FD_II_H_SIZE_1)
#define FD_II_V_SIZE_2	fdStepVSize(FD_II_H_SIZE_2)
#define FD_II_V_SIZE_3	fdStepVSize(FD_II_H_SIZE_3)
#define FD_II_V_SIZE_4	fdStepVSize(FD_II_H_SIZE_4)
#define FD_II_V_SIZE_5	fdStepVSize(FD_II_H_SIZE_5)
#define FD_II_V_SIZE_6	fdStepVSize(FD_II_H_SIZE_6)
#define FD_II_V_SIZE_7	fdStepVSize(FD_II_H_SIZE_7)
#define FD_II_V_SIZE_8	fdStepVSize(FD_II_H_SIZE_8)
#define FD_II_V_SIZE_9	fdStepVSize(FD_II_H_SIZE_9)
#define FD_II_V_SIZE_10	fdStepVSize(FD_II_H_SIZE_10)
#define FD_II_V_SIZE_11	fdStepVSize(FD_II_H_SIZE_11)
#define FD_II_V_SIZE_12	fdStepVSize(FD_II_H_SIZE_12)

#define FD_STEP_RADIUS_0	fdStepRadius(FD_II_H_SIZE_0)
#define FD_STEP_RADIUS_1	fdStepRadius(FD_II_H_SIZE_1)
#define FD_STEP_RADIUS_2	fdStepRadius(FD_II_H_SIZE_2)
#define FD_STEP_RADIUS_3	fdStepRadius(FD_II_H_SIZE_3)
#define FD_STEP_RADIUS_4	fdStepRadius(FD_II_H_SIZE_4)
#define FD_STEP_RADIUS_5	fdStepRadius(FD_II_H_SIZE_5)
#define FD_STEP_RADIUS_6	fdStepRadius(FD_II_H_SIZE_6)
#define FD_STEP_RADIUS_7	fdStepRadius(FD_II_H_SIZE_7)
#define FD_STEP_RADIUS_8	fdStepRadius(FD_II_H_SIZE_8)
#define FD_STEP_RADIUS_9	fdStepRadius(FD_II_H_SIZE_9)
#define FD_STEP_RADIUS_10	fdStepRadius(FD_II_H_SIZE_10)
#define FD_STEP_RADIUS_11	fdStepRadius(FD_II_H_SIZE_11)
#define FD_STEP_RADIUS_12	fdStepRadius(FD_II_H_SIZE_12)

#define FD_STEP_FAC_0	fdStepFac(FD_II_H_SIZE_0)
#define FD_STEP_FAC_1	fdStepFac(FD_II_H_SIZE_1)
#define FD_STEP_FAC_2	fdStepFac(FD_II_H_SIZE_2)
#define FD_STEP_FAC_3	fdStepFac(FD_II_H_SIZE_3)
#define FD_STEP_FAC_4	fdStepFac(FD_II_H_SIZE_4)
#define FD_STEP_FAC_5	fdStepFac(FD_II_H_SIZE_5)
#define FD_STEP_FAC_6	fdStepFac(FD_II_H_SIZE_6)
#define FD_STEP_FAC_7	fdStepFac(FD_II_H_SIZE_7)
#define FD_STEP_FAC_8	fdStepFac(FD_II_H_SIZE_8)
#define FD_STEP_FAC_9	fdStepFac(FD_II_H_SIZE_9)
#define FD_STEP_FAC_10	fdStepFac(FD_II_H_SIZE_10)
#define FD_STEP_FAC_11	fdStepFac(FD_II_H_SIZE_11)
#define FD_STEP_FAC_12	fdStepFac(FD_II_H_SIZE_12)

#define FD_STEP_AREA_0	fdStepArea(FD_II_H_SIZE_0)
#define FD_STEP_AREA_1	fdStepArea(FD_II_H_SIZE_1)
#define FD_STEP_AREA_2	fdStepArea(FD_II_H_SIZE_2)
#define FD_STEP_AREA_3	fdStepArea(FD_II_H_SIZE_3)
#define FD_STEP_AREA_4	fdStepArea(FD_II_H_SIZE_4)
#define FD_STEP_AREA_5	fdStepArea(FD_II_H_SIZE_5)
#define FD_STEP_AREA_6	fdStepArea(FD_II_H_SIZE_6)
#define FD_STEP_AREA_7	fdStepArea(FD_II_H_SIZE_7)
#define FD_STEP_AREA_8	fdStepArea(FD_II_H_SIZE_8)
#define FD_STEP_AREA_9	fdStepArea(FD_II_H_SIZE_9)
#define FD_STEP_AREA_10	fdStepArea(FD_II_H_SIZE_10)
#define FD_STEP_AREA_11	fdStepArea(FD_II_H_SIZE_11)
#define FD_STEP_AREA_12	fdStepArea(FD_II_H_SIZE_12)

/* smile array macros */
#define SMILE_II_H_SIZE_0	22
#define SMILE_II_H_SIZE_1	23
#define SMILE_II_H_SIZE_2	25
#define SMILE_II_H_SIZE_3	27
#define SMILE_II_H_SIZE_4	29
#define SMILE_II_H_SIZE_5	30
#define SMILE_II_H_SIZE_6	32
#define SMILE_II_H_SIZE_7	35
#define SMILE_II_H_SIZE_8	37
#define SMILE_II_H_SIZE_9	39
#define SMILE_II_H_SIZE_10	42
#define SMILE_II_H_SIZE_11	45
#define SMILE_II_H_SIZE_12	160

#define SMILE_II_V_SIZE_0		fdStepVSize(SMILE_II_H_SIZE_0)
#define SMILE_II_V_SIZE_1		fdStepVSize(SMILE_II_H_SIZE_1)
#define SMILE_II_V_SIZE_2		fdStepVSize(SMILE_II_H_SIZE_2)
#define SMILE_II_V_SIZE_3		fdStepVSize(SMILE_II_H_SIZE_3)
#define SMILE_II_V_SIZE_4		fdStepVSize(SMILE_II_H_SIZE_4)
#define SMILE_II_V_SIZE_5		fdStepVSize(SMILE_II_H_SIZE_5)
#define SMILE_II_V_SIZE_6		fdStepVSize(SMILE_II_H_SIZE_6)
#define SMILE_II_V_SIZE_7		fdStepVSize(SMILE_II_H_SIZE_7)
#define SMILE_II_V_SIZE_8		fdStepVSize(SMILE_II_H_SIZE_8)
#define SMILE_II_V_SIZE_9		fdStepVSize(SMILE_II_H_SIZE_9)
#define SMILE_II_V_SIZE_10	fdStepVSize(SMILE_II_H_SIZE_10)
#define SMILE_II_V_SIZE_11	fdStepVSize(SMILE_II_H_SIZE_11)
#define SMILE_II_V_SIZE_12	fdStepVSize(SMILE_II_H_SIZE_12)

#define SMILE_STEP_RADIUS_0		fdStepRadius(SMILE_II_H_SIZE_0)
#define SMILE_STEP_RADIUS_1		fdStepRadius(SMILE_II_H_SIZE_1)
#define SMILE_STEP_RADIUS_2		fdStepRadius(SMILE_II_H_SIZE_2)
#define SMILE_STEP_RADIUS_3		fdStepRadius(SMILE_II_H_SIZE_3)
#define SMILE_STEP_RADIUS_4		fdStepRadius(SMILE_II_H_SIZE_4)
#define SMILE_STEP_RADIUS_5		fdStepRadius(SMILE_II_H_SIZE_5)
#define SMILE_STEP_RADIUS_6		fdStepRadius(SMILE_II_H_SIZE_6)
#define SMILE_STEP_RADIUS_7		fdStepRadius(SMILE_II_H_SIZE_7)
#define SMILE_STEP_RADIUS_8		fdStepRadius(SMILE_II_H_SIZE_8)
#define SMILE_STEP_RADIUS_9		fdStepRadius(SMILE_II_H_SIZE_9)
#define SMILE_STEP_RADIUS_10	fdStepRadius(SMILE_II_H_SIZE_10)
#define SMILE_STEP_RADIUS_11	fdStepRadius(SMILE_II_H_SIZE_11)
#define SMILE_STEP_RADIUS_12	fdStepRadius(SMILE_II_H_SIZE_12)

#define SMILE_STEP_FAC_0	fdStepFac(SMILE_II_H_SIZE_0)
#define SMILE_STEP_FAC_1	fdStepFac(SMILE_II_H_SIZE_1)
#define SMILE_STEP_FAC_2	fdStepFac(SMILE_II_H_SIZE_2)
#define SMILE_STEP_FAC_3	fdStepFac(SMILE_II_H_SIZE_3)
#define SMILE_STEP_FAC_4	fdStepFac(SMILE_II_H_SIZE_4)
#define SMILE_STEP_FAC_5	fdStepFac(SMILE_II_H_SIZE_5)
#define SMILE_STEP_FAC_6	fdStepFac(SMILE_II_H_SIZE_6)
#define SMILE_STEP_FAC_7	fdStepFac(SMILE_II_H_SIZE_7)
#define SMILE_STEP_FAC_8	fdStepFac(SMILE_II_H_SIZE_8)
#define SMILE_STEP_FAC_9	fdStepFac(SMILE_II_H_SIZE_9)
#define SMILE_STEP_FAC_10	fdStepFac(SMILE_II_H_SIZE_10)
#define SMILE_STEP_FAC_11	fdStepFac(SMILE_II_H_SIZE_11)
#define SMILE_STEP_FAC_12	fdStepFac(SMILE_II_H_SIZE_12)

#define SMILE_STEP_AREA_0		fdStepArea(SMILE_II_H_SIZE_0)
#define SMILE_STEP_AREA_1		fdStepArea(SMILE_II_H_SIZE_1)
#define SMILE_STEP_AREA_2		fdStepArea(SMILE_II_H_SIZE_2)
#define SMILE_STEP_AREA_3		fdStepArea(SMILE_II_H_SIZE_3)
#define SMILE_STEP_AREA_4		fdStepArea(SMILE_II_H_SIZE_4)
#define SMILE_STEP_AREA_5		fdStepArea(SMILE_II_H_SIZE_5)
#define SMILE_STEP_AREA_6		fdStepArea(SMILE_II_H_SIZE_6)
#define SMILE_STEP_AREA_7		fdStepArea(SMILE_II_H_SIZE_7)
#define SMILE_STEP_AREA_8		fdStepArea(SMILE_II_H_SIZE_8)
#define SMILE_STEP_AREA_9		fdStepArea(SMILE_II_H_SIZE_9)
#define SMILE_STEP_AREA_10	fdStepArea(SMILE_II_H_SIZE_10)
#define SMILE_STEP_AREA_11	fdStepArea(SMILE_II_H_SIZE_11)
#define SMILE_STEP_AREA_12	fdStepArea(SMILE_II_H_SIZE_12)

/* smile array macros */
#define SMILE_II_H_SIZE_13	48
#define SMILE_II_H_SIZE_14	50
#define SMILE_II_H_SIZE_15	52
#define SMILE_II_H_SIZE_16	55
#define SMILE_II_H_SIZE_17	57
#define SMILE_II_H_SIZE_18	59
#define SMILE_II_H_SIZE_19	62
#define SMILE_II_H_SIZE_20	65
#define SMILE_II_H_SIZE_21	67
#define SMILE_II_H_SIZE_22	70
#define SMILE_II_H_SIZE_23	73
#define SMILE_II_H_SIZE_24	77
#define SMILE_II_H_SIZE_25	160

#define SMILE_II_V_SIZE_13	fdStepVSize(SMILE_II_H_SIZE_13)
#define SMILE_II_V_SIZE_14	fdStepVSize(SMILE_II_H_SIZE_14)
#define SMILE_II_V_SIZE_15	fdStepVSize(SMILE_II_H_SIZE_15)
#define SMILE_II_V_SIZE_16	fdStepVSize(SMILE_II_H_SIZE_16)
#define SMILE_II_V_SIZE_17	fdStepVSize(SMILE_II_H_SIZE_17)
#define SMILE_II_V_SIZE_18	fdStepVSize(SMILE_II_H_SIZE_18)
#define SMILE_II_V_SIZE_19	fdStepVSize(SMILE_II_H_SIZE_19)
#define SMILE_II_V_SIZE_20	fdStepVSize(SMILE_II_H_SIZE_20)
#define SMILE_II_V_SIZE_21	fdStepVSize(SMILE_II_H_SIZE_21)
#define SMILE_II_V_SIZE_22	fdStepVSize(SMILE_II_H_SIZE_22)
#define SMILE_II_V_SIZE_23	fdStepVSize(SMILE_II_H_SIZE_23)
#define SMILE_II_V_SIZE_24	fdStepVSize(SMILE_II_H_SIZE_24)
#define SMILE_II_V_SIZE_25	fdStepVSize(SMILE_II_H_SIZE_25)

#define SMILE_STEP_RADIUS_13	fdStepRadius(SMILE_II_H_SIZE_13)
#define SMILE_STEP_RADIUS_14	fdStepRadius(SMILE_II_H_SIZE_14)
#define SMILE_STEP_RADIUS_15	fdStepRadius(SMILE_II_H_SIZE_15)
#define SMILE_STEP_RADIUS_16	fdStepRadius(SMILE_II_H_SIZE_16)
#define SMILE_STEP_RADIUS_17	fdStepRadius(SMILE_II_H_SIZE_17)
#define SMILE_STEP_RADIUS_18	fdStepRadius(SMILE_II_H_SIZE_18)
#define SMILE_STEP_RADIUS_19	fdStepRadius(SMILE_II_H_SIZE_19)
#define SMILE_STEP_RADIUS_20	fdStepRadius(SMILE_II_H_SIZE_20)
#define SMILE_STEP_RADIUS_21	fdStepRadius(SMILE_II_H_SIZE_21)
#define SMILE_STEP_RADIUS_22	fdStepRadius(SMILE_II_H_SIZE_22)
#define SMILE_STEP_RADIUS_23	fdStepRadius(SMILE_II_H_SIZE_23)
#define SMILE_STEP_RADIUS_24	fdStepRadius(SMILE_II_H_SIZE_24)
#define SMILE_STEP_RADIUS_25	fdStepRadius(SMILE_II_H_SIZE_25)

#define SMILE_STEP_FAC_13	fdStepFac(SMILE_II_H_SIZE_13)
#define SMILE_STEP_FAC_14	fdStepFac(SMILE_II_H_SIZE_14)
#define SMILE_STEP_FAC_15	fdStepFac(SMILE_II_H_SIZE_15)
#define SMILE_STEP_FAC_16	fdStepFac(SMILE_II_H_SIZE_16)
#define SMILE_STEP_FAC_17	fdStepFac(SMILE_II_H_SIZE_17)
#define SMILE_STEP_FAC_18	fdStepFac(SMILE_II_H_SIZE_18)
#define SMILE_STEP_FAC_19	fdStepFac(SMILE_II_H_SIZE_19)
#define SMILE_STEP_FAC_20	fdStepFac(SMILE_II_H_SIZE_20)
#define SMILE_STEP_FAC_21	fdStepFac(SMILE_II_H_SIZE_21)
#define SMILE_STEP_FAC_22	fdStepFac(SMILE_II_H_SIZE_22)
#define SMILE_STEP_FAC_23	fdStepFac(SMILE_II_H_SIZE_23)
#define SMILE_STEP_FAC_24	fdStepFac(SMILE_II_H_SIZE_24)
#define SMILE_STEP_FAC_25	fdStepFac(SMILE_II_H_SIZE_25)

#define SMILE_STEP_AREA_13	fdStepArea(SMILE_II_H_SIZE_13)
#define SMILE_STEP_AREA_14	fdStepArea(SMILE_II_H_SIZE_14)
#define SMILE_STEP_AREA_15	fdStepArea(SMILE_II_H_SIZE_15)
#define SMILE_STEP_AREA_16	fdStepArea(SMILE_II_H_SIZE_16)
#define SMILE_STEP_AREA_17	fdStepArea(SMILE_II_H_SIZE_17)
#define SMILE_STEP_AREA_18	fdStepArea(SMILE_II_H_SIZE_18)
#define SMILE_STEP_AREA_19	fdStepArea(SMILE_II_H_SIZE_19)
#define SMILE_STEP_AREA_20	fdStepArea(SMILE_II_H_SIZE_20)
#define SMILE_STEP_AREA_21	fdStepArea(SMILE_II_H_SIZE_21)
#define SMILE_STEP_AREA_22	fdStepArea(SMILE_II_H_SIZE_22)
#define SMILE_STEP_AREA_23	fdStepArea(SMILE_II_H_SIZE_23)
#define SMILE_STEP_AREA_24	fdStepArea(SMILE_II_H_SIZE_24)
#define SMILE_STEP_AREA_25	fdStepArea(SMILE_II_H_SIZE_25)

#define _ABS(x,y)				(((x)>(y))?((x)-(y)):((y)-(x)))
#define _DIS(x,y,x0,y0)			((_ABS(x,x0)+_ABS(y,y0))>>1)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	/* fd flow status */
	FD_FLOW_Face=0x00,
	FD_FLOW_Smile,	
	FD_FLOW_NonSmile,
	FD_FLOW_Eye,
	FD_FLOW_CloseEye,
}fd_flow_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern code UINT8 fdHSizeFace[];
extern UINT8 *fdHSizeSub;

extern UINT32 fdTblAddr;
extern UINT32 fdJobAddr;
extern UINT32 fdJobAddrSub;
extern UINT32 fdOutAddr;

extern UINT32 fdIISrcAddr;
extern UINT32 fdSIIAddr;

extern UINT32 fdIIDstAddr[];

extern UINT8 fdRtSize[];

extern UINT8  fdFaceMax;

extern UINT8  fdFaceNum;
extern UINT8  fdRotate;

extern UINT8 fdStep; 

extern UINT8 fdKeyRtX;
extern UINT8 fdKeyRtY;
extern UINT8 fdKeyRtS;
extern UINT8 fdKeyStep;
extern UINT8 fdKeyIdx;

extern UINT8 fdCounter;
extern UINT8 fdFlow;
extern UINT8 fdSmileEn;
extern UINT8 fdBlinkEn;
extern UINT32 fdBlinkAddr;
extern UINT8 fdSubCnt0;
extern UINT8 fdSubCnt1;

extern UINT8 fdCdspIdx;
extern UINT32 fdDBData;

extern UINT8 fdDbgCnt ;
extern UINT8 fdDbgSmileCnt;

extern UINT8 fdDbgFDOutCnt;

extern UINT32 fdDbgSize;

//extern UINT8 fdDbgSaveIdx;

//extern UINT8 xdata fdDbgSubSize[];
//extern UINT8 xdata* fdDbgSubPosX;//[2];
//extern UINT8 xdata* fdDbgSubPosY;//[2];

extern UINT8 fdSubStep;
extern UINT8 fdDbgStage0;
extern UINT8 fdDbgStage1;
extern UINT8 fdDbgStage2;

extern UINT32 fdDbgT0,fdDbgT1,fdDbgT2,fdDbgT3;
extern UINT8 fdDbgSlow2En;

extern UINT8 G_ucStorData[];

extern char INTR_putchar (char c); 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 fdGetCdspBuf(UINT8 idx, UINT8 path);

void fdDraw();
void fdGroup(void);
void fdProcFace();
UINT8 fdProcSmile();

void fdProcNonSmile();
void fdProcNonSmileIntr();

void fdProcEye();

void fdProcCloseEyeIntr();

UINT8 fdProcBlink(UINT32 addr);

UINT8 fdGroupSub(UINT8 isSnap);

void fdTrigIntr(UINT8 lv);

void fdSaveFaceYuvSrcDbg(UINT32 srcYuvAddr);
void fdSaveFaceIIDbg(void);
void fdSaveSmileYuvSrcDbg(UINT32 srcYuvAddr);
void fdSaveSmileIIDbg(void);





/**************************************************************************
 *               Notify               *
 **************************************************************************/
/*init fd notice:
 * copy "download\\A\\FDTBL090.BIN" to "sd:\\II\\FDTBL090.BIN"
 * uart cmd:
 * fd i
 * fd 1
*/
#endif
