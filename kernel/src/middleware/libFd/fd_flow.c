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
 * @file      fd_func.c
 * @brief     Brief
 * @author    wangjg
 * @since     2010-04-01
 * @date      2010-04-01
 * @res file	"FDTBL090.BIN"
 * @version	10070901
*/


/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "fd_def.h"
#include "osd_api.h"
#include "os_api.h"
#include "hal_moe.h"
#include "reg_def.h"
#include "fd_debug.h"
#include "cmd.h"
#include "video_mem.h"
#if (FD_ENABLE == 1)
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_LIB_FD_FLOW__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define FD_MODE_Face 	0x00
#define FD_MODE_Smile	0x01
#define FD_MODE_Blind	0x02

#define FD_COUNTER_Sub	0x03

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         A R R A Y    M A C R O S                         *
 **************************************************************************/
code UINT8 fdHSizeFace[] = {
	//face
	FD_II_H_SIZE_0,	FD_II_H_SIZE_1,	FD_II_H_SIZE_2,	FD_II_H_SIZE_3,	
	FD_II_H_SIZE_4,	FD_II_H_SIZE_5,	FD_II_H_SIZE_6,	FD_II_H_SIZE_7,	
	FD_II_H_SIZE_8,	FD_II_H_SIZE_9,	FD_II_H_SIZE_10,	FD_II_H_SIZE_11,
	FD_II_H_SIZE_12, 
	//smile(0~12)
	SMILE_II_H_SIZE_0,	SMILE_II_H_SIZE_1,	SMILE_II_H_SIZE_2,	SMILE_II_H_SIZE_3,	
	SMILE_II_H_SIZE_4,	SMILE_II_H_SIZE_5,	SMILE_II_H_SIZE_6,	SMILE_II_H_SIZE_7,	
	SMILE_II_H_SIZE_8,	SMILE_II_H_SIZE_9,	SMILE_II_H_SIZE_10,	SMILE_II_H_SIZE_11,
	SMILE_II_H_SIZE_12, 	
	//smile(13~25)
	SMILE_II_H_SIZE_13,	SMILE_II_H_SIZE_14,	SMILE_II_H_SIZE_15,	SMILE_II_H_SIZE_16,	
	SMILE_II_H_SIZE_17,	SMILE_II_H_SIZE_18,	SMILE_II_H_SIZE_19,	SMILE_II_H_SIZE_20,	
	SMILE_II_H_SIZE_21,	SMILE_II_H_SIZE_22,	SMILE_II_H_SIZE_23,	SMILE_II_H_SIZE_24,
	SMILE_II_H_SIZE_25, 
};	

#if (USE_ARRAY_MACROS == 1)
code UINT8 fdStepVSizeArray[] = {
	//face
	FD_II_V_SIZE_0,	FD_II_V_SIZE_1,	FD_II_V_SIZE_2,	FD_II_V_SIZE_3,	
	FD_II_V_SIZE_4,	FD_II_V_SIZE_5,	FD_II_V_SIZE_6,	FD_II_V_SIZE_7,	
	FD_II_V_SIZE_8,	FD_II_V_SIZE_9,	FD_II_V_SIZE_10,	FD_II_V_SIZE_11,
	FD_II_V_SIZE_12, 
	//smile(0~12)
	SMILE_II_V_SIZE_0,	SMILE_II_V_SIZE_1,	SMILE_II_V_SIZE_2,	SMILE_II_V_SIZE_3,	
	SMILE_II_V_SIZE_4,	SMILE_II_V_SIZE_5,	SMILE_II_V_SIZE_6,	SMILE_II_V_SIZE_7,	
	SMILE_II_V_SIZE_8,	SMILE_II_V_SIZE_9,	SMILE_II_V_SIZE_10,	SMILE_II_V_SIZE_11,
	SMILE_II_V_SIZE_12, 
	//smile(13~25)
	SMILE_II_V_SIZE_13,	SMILE_II_V_SIZE_14,	SMILE_II_V_SIZE_15,	SMILE_II_V_SIZE_16,	
	SMILE_II_V_SIZE_17,	SMILE_II_V_SIZE_18,	SMILE_II_V_SIZE_19,	SMILE_II_V_SIZE_20,	
	SMILE_II_V_SIZE_21,	SMILE_II_V_SIZE_22,	SMILE_II_V_SIZE_23,	SMILE_II_V_SIZE_24,
	SMILE_II_V_SIZE_25,
};

code UINT8 fdStepRadiusArray[] = {
	//face
	FD_STEP_RADIUS_0,	FD_STEP_RADIUS_1,	FD_STEP_RADIUS_2,	FD_STEP_RADIUS_3,	
	FD_STEP_RADIUS_4,	FD_STEP_RADIUS_5,	FD_STEP_RADIUS_6,	FD_STEP_RADIUS_7,	
	FD_STEP_RADIUS_8,	FD_STEP_RADIUS_9,	FD_STEP_RADIUS_10,	FD_STEP_RADIUS_11,	
	FD_STEP_RADIUS_12,
	//smile(0~12)
	SMILE_STEP_RADIUS_0,	SMILE_STEP_RADIUS_1,	SMILE_STEP_RADIUS_2,	SMILE_STEP_RADIUS_3,	
	SMILE_STEP_RADIUS_4,	SMILE_STEP_RADIUS_5,	SMILE_STEP_RADIUS_6,	SMILE_STEP_RADIUS_7,	
	SMILE_STEP_RADIUS_8,	SMILE_STEP_RADIUS_9,	SMILE_STEP_RADIUS_10,	SMILE_STEP_RADIUS_11,	
	SMILE_STEP_RADIUS_12,
	//smile(13~25)
	SMILE_STEP_RADIUS_13,	SMILE_STEP_RADIUS_14,	SMILE_STEP_RADIUS_15,	SMILE_STEP_RADIUS_16,	
	SMILE_STEP_RADIUS_17,	SMILE_STEP_RADIUS_18,	SMILE_STEP_RADIUS_19,	SMILE_STEP_RADIUS_20,	
	SMILE_STEP_RADIUS_21,	SMILE_STEP_RADIUS_22,	SMILE_STEP_RADIUS_23,	SMILE_STEP_RADIUS_24,	
	SMILE_STEP_RADIUS_25,
};

code UINT16 fdStepAreaArray[] = {
	//face
	FD_STEP_AREA_0,	FD_STEP_AREA_1,	FD_STEP_AREA_2,	FD_STEP_AREA_3,	
	FD_STEP_AREA_4,	FD_STEP_AREA_5,	FD_STEP_AREA_6,	FD_STEP_AREA_7,	
	FD_STEP_AREA_8,	FD_STEP_AREA_9,	FD_STEP_AREA_10,	FD_STEP_AREA_11,	
	FD_STEP_AREA_12,
	//smile(0~12)
	SMILE_STEP_AREA_0,	SMILE_STEP_AREA_1,	SMILE_STEP_AREA_2,	SMILE_STEP_AREA_3,	
	SMILE_STEP_AREA_4,	SMILE_STEP_AREA_5,	SMILE_STEP_AREA_6,	SMILE_STEP_AREA_7,	
	SMILE_STEP_AREA_8,	SMILE_STEP_AREA_9,	SMILE_STEP_AREA_10,	SMILE_STEP_AREA_11,	
	SMILE_STEP_AREA_12,
	//smile(13~25)
	SMILE_STEP_AREA_13,	SMILE_STEP_AREA_14,	SMILE_STEP_AREA_15,	SMILE_STEP_AREA_16,	
	SMILE_STEP_AREA_17,	SMILE_STEP_AREA_18,	SMILE_STEP_AREA_19,	SMILE_STEP_AREA_20,	
	SMILE_STEP_AREA_21,	SMILE_STEP_AREA_22,	SMILE_STEP_AREA_23,	SMILE_STEP_AREA_24,	
	SMILE_STEP_AREA_25,

};

code UINT8 fdStepFacArray[] = {
	//face
	FD_STEP_FAC_0,	FD_STEP_FAC_1,	FD_STEP_FAC_2,	FD_STEP_FAC_3,	
	FD_STEP_FAC_4,	FD_STEP_FAC_5,	FD_STEP_FAC_6,	FD_STEP_FAC_7,	
	FD_STEP_FAC_8,	FD_STEP_FAC_9,	FD_STEP_FAC_10,	FD_STEP_FAC_11,	
	FD_STEP_FAC_12,
	//smile(0~12)
	SMILE_STEP_FAC_0,	SMILE_STEP_FAC_1,	SMILE_STEP_FAC_2,	SMILE_STEP_FAC_3,	
	SMILE_STEP_FAC_4,	SMILE_STEP_FAC_5,	SMILE_STEP_FAC_6,	SMILE_STEP_FAC_7,	
	SMILE_STEP_FAC_8,	SMILE_STEP_FAC_9,	SMILE_STEP_FAC_10,	SMILE_STEP_FAC_11,	
	SMILE_STEP_FAC_12,
	//smile(13~25)
	SMILE_STEP_FAC_13,	SMILE_STEP_FAC_14,	SMILE_STEP_FAC_15,	SMILE_STEP_FAC_16,	
	SMILE_STEP_FAC_17,	SMILE_STEP_FAC_18,	SMILE_STEP_FAC_19,	SMILE_STEP_FAC_20,	
	SMILE_STEP_FAC_21,	SMILE_STEP_FAC_22,	SMILE_STEP_FAC_23,	SMILE_STEP_FAC_24,	
	SMILE_STEP_FAC_25,

};
#endif

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT8 *fdHSizeSub = fdHSizeFace+FD_DECTED_Cnt;

UINT32 fdTblAddr;
UINT32 fdJobAddr;//[4];
UINT32 fdJobAddrSub;
UINT32 fdOutAddr;

UINT32 fdIISrcAddr;
UINT32 fdSIIAddr;

UINT32 fdIIDstAddr[FD_II_CNT+1];
UINT8 fdIIResultCnt[FD_II_CNT+1];
UINT8 fdRtSize[FD_FACE_CNT*6];

UINT8  fdFaceMax=15;
UINT8  fdFaceNum=0;
UINT8  fdRotate;

UINT8 fdKeyRtX=0;	//key face, upper left x coordinate axis
UINT8 fdKeyRtY=0;	//key face, upper left y coordinate axis
UINT8 fdKeyRtS=0;	//key face, diameter
UINT8 fdKeyStep=13;	//key face, average of II step
UINT8 fdKeyIdx=0;	//key face, index

UINT8 fdCounter;
UINT8 fdFlow;
UINT8 fdStep; 

UINT8 fdCdspIdx=0xff;
UINT32 fdDBData=0;

UINT8 fdDbgCnt=0;//0xff;
UINT8 fdDbgFDOutCnt;

UINT32 fdDbgSize;

//UINT8 fdDbgSaveIdx=0;

UINT8 fdDbgStage0 = FD_DATABASE_STAGE_0;
UINT8 fdDbgStage1 = FD_DATABASE_STAGE_1;
UINT8 fdDbgStage2 = FD_DATABASE_STAGE_2;

//UINT32 fdDbgT0,fdDbgT1,fdDbgT2,fdDbgT3;
UINT8 fdDbgSlow2En = 1;
UINT8 fdEn = 0;
//UINT8 fdEnCnt = 10;
UINT32 fdTimer[10];
UINT8 fdDone=0;
UINT8 fdDrawMode = 0;//bit0:show all/Key(only), bit1:show sub(mouse/eye)

#if (USE_ARRAY_MACROS == 1)
UINT8 jumpIdx = 0;
#endif

#if (FD_SAVE_FACE_YUV_SRC_DBG == 1)
UINT8 fdSaveFaceYuvSrcDbgFlag;
#endif

#if (FD_SAVE_FACE_II_DBG == 1)
UINT8 fdSaveFaceIIDbgFlag;
#endif

#if (FD_SAVE_SMILE_II_DBG == 1 || FD_SAVE_SMILE_YUV_SRC_DBG == 1)
UINT8 fdSaveSmileIIDbgFlag;
UINT8 fdSaveSmileIIDbgPath = 1;
UINT8 fdSaveSmileYuvSrcDbgFlag;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT32 fdGetCdspBuf(UINT8 idx)
 * @brief     fd get pre cdsp write buf
 * @param     [in] idx:cdsp write buf idx
 * @retval    return = buf addr(w)
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
UINT32 
fdGetCdspBuf(
	UINT8 idx,
	UINT8 path
)
{
	#if 1
	code UINT16 bufs[]={REG_Dram_MpgSrc3Addr_B0,REG_Dram_MpgSrc0Addr_B0,REG_Dram_MpgSrc1Addr_B0,REG_Dram_MpgSrc2Addr_B0};
	//code UINT16 bufs[]={REG_Dram_DispSb3_StAddr_B0,REG_Dram_DispSb0_StAddr_B0,REG_Dram_DispSb1_StAddr_B0,REG_Dram_DispSb2_StAddr_B0};
	UINT16 cdspbufreg = bufs[idx];
	UINT32 addr;
	UINT8 *pt = (unsigned char *)&addr;

	if (path == 2) {
		cdspbufreg+=0x50;	/* 2path */
	}

	pt[3] = XBYTE[cdspbufreg];
	pt[2] = XBYTE[cdspbufreg+1];
	pt[1] = XBYTE[cdspbufreg+2];
	pt[0] = XBYTE[cdspbufreg+3];

	return addr;

	#else
	switch(idx)
	{
		case 0:
			return K_SDRAM_PvBufAddrD;
		case 1:
			return K_SDRAM_PvBufAddrA;
		case 2:
			return K_SDRAM_PvBufAddrB;
		case 3:
			return K_SDRAM_PvBufAddrC;
		default:
			return 0;
	}
	#endif
}

/**
 * @fn        void fdDraw(void)
 * @brief     draw result rect of fd
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
fdDraw(
	void
)
{
	UINT16 x;
	UINT8 y;
	UINT8 cnt =0,i=0;
	UINT8 flag=0;
	UINT32 dstaddr;//yuvsrcaddr[(XBYTE[REG_Dram_MpgSb_CdspWRegIdx])&0x03];
	UINT8 *fdRtPosx	=fdRtSize+FD_FACE_CNT; 
	UINT8 *fdRtPosy	=fdRtSize+FD_FACE_CNT*2; 
	UINT8 *fdRtShow	=fdRtSize+FD_FACE_CNT*3; 
	//UINT8 *fdRtWp	=fdRtSize+FD_FACE_CNT*4; 	

	UINT8 val = (fdDrawMode & 0x2) ? (fdFaceNum+2) : fdFaceNum;//fdDbgSubCnt0+fdDbgSubCnt1;

	if (fdFaceNum == 0) {
		return;
	}
	
	osdIndexSave();
	dstaddr = fdGetCdspBuf(HAL_CdspWriteBufIdxGet(),2);  /* will: use hal fucntion */

	XBYTE[REG_Gprm_DatFmt_CpEn] = 0x02;	/* bit1: skip to read source data in dram */

	XBYTE[REG_Gprm_OsdIdx0] = 0xff;//0x10;	/* Y */
	XBYTE[REG_Gprm_OsdIdx1] = 0x80;//0x00; 	/* U */
	XBYTE[REG_Gprm_OsdIdx2] = 0x80;//0x00; 	/* V */
	
	/* set gprmdb_staddr. */
	XBYTE[REG_Dram_Gprmdb_StAddr_B0] = READ8(&dstaddr, 3);
	XBYTE[REG_Dram_Gprmdb_StAddr_B1] = READ8(&dstaddr, 2);
	XBYTE[REG_Dram_Gprmdb_StAddr_B2] = READ8(&dstaddr, 1);
	XBYTE[REG_Dram_Gprmdb_StAddr_B3] = READ8(&dstaddr, 0);

	/* set gprmdb_hsize. */
	XBYTE[REG_Dram_Gprmdb_HSize_Low] = 0x40;
	XBYTE[REG_Dram_Gprmdb_HSize_High] = 1;

	/* set gprmdb_vsize. */
	XBYTE[REG_Dram_Gprmdb_VSize_Low] = 0xF0;
	XBYTE[REG_Dram_Gprmdb_VSize_High] = 0;

	/* set imgpromode to copy-paste. */
	XBYTE[REG_Gprm_OpMode] = 0x03;
	XBYTE[REG_Gprm_PstThd] = 0x00;

	/* set w,h high byte is 0 */
	XBYTE[REG_Gprm_BgbwidthH] = 0;
	XBYTE[REG_Gprm_BgbheightH] = 0;

	/* set y high byte is 0 */
	XBYTE[REG_Gprm_BgbvoffH] = 0;//READ8(&y, 0);
	
	//while(cnt--)
	
	for (i = 0; i < val; i++) {
		UINT8 rtsize = fdRtSize[i]>>1;//<<1;
		UINT16 xcor = (((UINT16)fdRtPosx[i])<<1);//(((unsigned char *)&(fdRtpos[cnt]))[1])<<1;
		UINT8 ycor = (((UINT16)fdRtPosy[i])<<1);//(((unsigned char *)&(fdRtpos[cnt]))[0])<<1;

		if (rtsize%2) {
			rtsize +=1;
		}
		
		if (i >= fdFaceNum) {
			UINT8 ii= i-fdFaceNum;

			//rtsize = fdDbgSubSize[ii]>>1;//[ii]>>1;
			//xcor = ((UINT16)fdDbgSubPosX[ii])<<1;
			//ycor = ((UINT16)fdDbgSubPosY[ii])<<1;

			if (rtsize == 0 || ycor == 0 || xcor == 0) {
				osdIndexRestore();
				return;
			}
			/* pink */
			XBYTE[REG_Gprm_OsdIdx0] = 0x69;//0x4c;	/* Y */
			XBYTE[REG_Gprm_OsdIdx1] = 0xd4;//0x55; 	/* U */
			XBYTE[REG_Gprm_OsdIdx2] = 0xea;//0xff;	/* V */
		}
		else {
			//if (++cnt > fdFaceMax)	{
			//	osdIndexRestore();
			//	return;
			//}

			if (rtsize==0 ||ycor==0 ||xcor==0) {
				continue;
			}

			if (!(fdRtShow[i]&FD_SHOW_Draw)) {
				continue;	
			}

			if ((fdDrawMode&0x1) && !(fdRtShow[i]&FD_SHOW_Key)) {
				continue;
			}

			if (fdRtShow[i]&FD_SHOW_Smile) {
				/* blue */
				XBYTE[REG_Gprm_OsdIdx0] = 0x4c;//0xe1;//0x1d;//0x4c;	/* Y */
				XBYTE[REG_Gprm_OsdIdx1] = 0x55;//0x01;//0xff;//0x55;	/* U */
				XBYTE[REG_Gprm_OsdIdx2] = 0xff;//0x94;//0x6c;//0xff;	/* V */
				flag = 1;
			}
			else if(fdRtShow[i]&FD_SHOW_Key) {
				/* green */
				XBYTE[REG_Gprm_OsdIdx0] = 0x10;	/* Y */
				XBYTE[REG_Gprm_OsdIdx1] = 0x00;	/* U */
				XBYTE[REG_Gprm_OsdIdx2] = 0x00;	/* V */
				flag = 1;
			}		
			else if(flag)
			{
				/* white */
				XBYTE[REG_Gprm_OsdIdx0] = 0xff;//0x10;	/* Y */
				XBYTE[REG_Gprm_OsdIdx1] = 0x80;//0x00;	/* U */
				XBYTE[REG_Gprm_OsdIdx2] = 0x80;//0x00;	/* V */
				flag = 0;
			}		
		}
		
		x = xcor;
		y = ycor;
		XBYTE[REG_Gprm_BgbhoffL] = READ8(&x, 1);	/* set Bgbhoff, must align to 2. */
		XBYTE[REG_Gprm_BgbhoffH] = READ8(&x, 0);
		XBYTE[REG_Gprm_BgbvoffL] = y;//READ8(&y, 1);	/* set Bgbvoff. */
		//XBYTE[REG_Gprm_BgbvoffH] = READ8(&y, 0);			
		XBYTE[REG_Gprm_BgbwidthL] = rtsize;	/* set Bgbwidth. */
		//XBYTE[REG_Gprm_BgbwidthH] = 0;		
		XBYTE[REG_Gprm_BgbheightL] = 3;	/* set Bgbheight. */
		//XBYTE[REG_Gprm_BgbheightH] = 0;			
		XBYTE[REG_Gprm_Trg_IntSts] = 0x11;	/* trigger. */
		
		while((XBYTE[REG_Gprm_Trg_IntSts] & 0x1)==0x0);	/* wait operating done. */

		x = xcor+rtsize*3;
		//y = ycor;
		
		XBYTE[REG_Gprm_BgbhoffL] = READ8(&x, 1);	/* set Bgbhoff, must align to 2. */
		XBYTE[REG_Gprm_BgbhoffH] = READ8(&x, 0);
		//XBYTE[REG_Gprm_BgbvoffL] = READ8(&y, 1);	/* set Bgbvoff. */
		//XBYTE[REG_Gprm_BgbvoffH] = READ8(&y, 0);			
		//XBYTE[REG_Gprm_BgbwidthL] = rtsize;	/* set Bgbwidth. */
		//XBYTE[REG_Gprm_BgbwidthH] = 0;		
		//XBYTE[REG_Gprm_BgbheightL] = 2;	/* set Bgbheight. */
		//XBYTE[REG_Gprm_BgbheightH] = 0;			
		XBYTE[REG_Gprm_Trg_IntSts] = 0x11;	/* trigger. */
		
		while((XBYTE[REG_Gprm_Trg_IntSts] & 0x1)==0x0); /* wait operating done. */

		//x = xcor+rtsize*3;
		y = ycor+rtsize*4;
		//XBYTE[REG_Gprm_BgbhoffL] = READ8(&x, 1);	/* set Bgbhoff, must align to 2. */
		//XBYTE[REG_Gprm_BgbhoffH] = READ8(&x, 0);
		XBYTE[REG_Gprm_BgbvoffL] = y;//READ8(&y, 1);	/* set Bgbvoff. */
		//XBYTE[REG_Gprm_BgbvoffH] = READ8(&y, 0);			
		//XBYTE[REG_Gprm_BgbwidthL] = rtsize;	/* set Bgbwidth. */
		//XBYTE[REG_Gprm_BgbwidthH] = 0;		
		//XBYTE[REG_Gprm_BgbheightL] = 2;	/* set Bgbheight. */
		//XBYTE[REG_Gprm_BgbheightH] = 0;			
		XBYTE[REG_Gprm_Trg_IntSts] = 0x11;	/* trigger. */
		while((XBYTE[REG_Gprm_Trg_IntSts] & 0x1)==0x0);	/* wait operating done. */

		
		x = xcor;//+rtsize*3;
		//y = ycor+rtsize<<2;
		XBYTE[REG_Gprm_BgbhoffL] = READ8(&x, 1);	/* set Bgbhoff, must align to 2. */
		XBYTE[REG_Gprm_BgbhoffH] = READ8(&x, 0);
		//XBYTE[REG_Gprm_BgbvoffL] = READ8(&y, 1);	/* set Bgbvoff. */
		//XBYTE[REG_Gprm_BgbvoffH] = READ8(&y, 0);			
		//XBYTE[REG_Gprm_BgbwidthL] = rtsize;	/* set Bgbwidth. */
		//XBYTE[REG_Gprm_BgbwidthH] = 0;		
		//XBYTE[REG_Gprm_BgbheightL] = 2;	/* set Bgbheight. */
		//XBYTE[REG_Gprm_BgbheightH] = 0;			
		XBYTE[REG_Gprm_Trg_IntSts] = 0x11;	/* trigger. */
		while((XBYTE[REG_Gprm_Trg_IntSts] & 0x1)==0x0);	/* wait operating done. */
		
		
		//x = xcor;
		y = ycor;
		//XBYTE[REG_Gprm_BgbhoffL] = READ8(&x, 1);	/* set Bgbhoff, must align to 2. */
		//XBYTE[REG_Gprm_BgbhoffH] = READ8(&x, 0);
		XBYTE[REG_Gprm_BgbvoffL] = y;//READ8(&y, 1);	/* set Bgbvoff. */
		//XBYTE[REG_Gprm_BgbvoffH] = READ8(&y, 0);			
		XBYTE[REG_Gprm_BgbwidthL] = 3;	/* set Bgbwidth. */
		//XBYTE[REG_Gprm_BgbwidthH] = 0;		
		XBYTE[REG_Gprm_BgbheightL] = rtsize;	/* set Bgbheight. */
		//XBYTE[REG_Gprm_BgbheightH] = 0;			
		XBYTE[REG_Gprm_Trg_IntSts] = 0x11;	/* trigger. */
		while((XBYTE[REG_Gprm_Trg_IntSts] & 0x1)==0x0);	/* wait operating done. */

		//x = xcor;
		y = ycor+rtsize*3+2;
		//XBYTE[REG_Gprm_BgbhoffL] = READ8(&x, 1);	/* set Bgbhoff, must align to 2. */
		//XBYTE[REG_Gprm_BgbhoffH] = READ8(&x, 0);
		XBYTE[REG_Gprm_BgbvoffL] = y;//READ8(&y, 1);	/* set Bgbvoff. */
		//XBYTE[REG_Gprm_BgbvoffH] = READ8(&y, 0);			
		//XBYTE[REG_Gprm_BgbwidthL] = 2;	/* set Bgbwidth. */
		//XBYTE[REG_Gprm_BgbwidthH] = 0;		
		//XBYTE[REG_Gprm_BgbheightL] = rtsize;	/* set Bgbheight. */
		//XBYTE[REG_Gprm_BgbheightH] = 0;			
		XBYTE[REG_Gprm_Trg_IntSts] = 0x11;	/* trigger. */
		while((XBYTE[REG_Gprm_Trg_IntSts] & 0x1)==0x0);	/* wait operating done. */

		x = xcor+rtsize*4;
		//y = ycor+rtsize*3;
		XBYTE[REG_Gprm_BgbhoffL] = READ8(&x, 1);	/* set Bgbhoff, must align to 2. */
		XBYTE[REG_Gprm_BgbhoffH] = READ8(&x, 0);
		//XBYTE[REG_Gprm_BgbvoffL] = READ8(&y, 1);	/* set Bgbvoff. */
		//XBYTE[REG_Gprm_BgbvoffH] = READ8(&y, 0);			
		//XBYTE[REG_Gprm_BgbwidthL] = 2;	/* set Bgbwidth. */
		//XBYTE[REG_Gprm_BgbwidthH] = 0;		
		//XBYTE[REG_Gprm_BgbheightL] = rtsize;	/* set Bgbheight. */
		//XBYTE[REG_Gprm_BgbheightH] = 0;			
		XBYTE[REG_Gprm_Trg_IntSts] = 0x11;	/* trigger. */
		while((XBYTE[REG_Gprm_Trg_IntSts] & 0x1)==0x0);	/* wait operating done. */

		//x = xcor+rtsize*4;
		y = ycor;
		//XBYTE[REG_Gprm_BgbhoffL] = READ8(&x, 1);	/* set Bgbhoff, must align to 2. */
		//XBYTE[REG_Gprm_BgbhoffH] = READ8(&x, 0);
		XBYTE[REG_Gprm_BgbvoffL] = y;//READ8(&y, 1);	/* set Bgbvoff. */
		//XBYTE[REG_Gprm_BgbvoffH] = READ8(&y, 0);			
		//XBYTE[REG_Gprm_BgbwidthL] = 2;	/* set Bgbwidth. */
		//XBYTE[REG_Gprm_BgbwidthH] = 0;		
		//XBYTE[REG_Gprm_BgbheightL] = rtsize;	/* set Bgbheight. */
		//XBYTE[REG_Gprm_BgbheightH] = 0;			
		XBYTE[REG_Gprm_Trg_IntSts] = 0x11;	/* trigger. */
		while((XBYTE[REG_Gprm_Trg_IntSts] & 0x1)==0x0);	/* wait operating done. */
	}	
	
	osdIndexRestore();
}

#if 0
UINT32 fdYSum,fdDYSum;
#define _ABS(a,b) (a>b)?(a-b):(b-a)
/**
 * @fn        void fdFuncExt(void)
 * @brief     
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void fdFuncExt(void)
{
	UINT32 ysum;
	//UINT32 dysum;
	//moe_cmd_t cmd;
	//moe_queue_t queue;
	//ysum
	UINT32 addr = fdIISrcAddr+160ul*120*2-2;
	UINT32 addr0;
	UINT32 addr1;

	HAL_DramReadBytesAddr(addr, &ysum, 4, 2);
	fdDYSum= _ABS(ysum,fdYSum);
	fdYSum= ysum;
	printf("%8lu,%8lu,%8lu,%8lu-%8lu\n",fdDYSum,fdDYSum>>8,fdDYSum>>16,fdDYSum>>24,ysum/(160*120));
}
#endif

/**
 * @fn        void fdGroup(void)
 * @brief     
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
fdGroup(
	void
)	        
{
	UINT8 xdata* pt = G_ucStorData;
	UINT32 addr = fdOutAddr;	
	
	UINT8 xdata* fdRtPosx	=fdRtSize+FD_FACE_CNT;		/* face queue, upper left x coordinate axis */
	UINT8 xdata* fdRtPosy	=fdRtSize+FD_FACE_CNT*2;	/* face queue, upper left y coordinate axis */
	UINT8 xdata* fdRtShow	=fdRtSize+FD_FACE_CNT*3; 	/* face queue, bit[7:4]show type, bit[3:0] average of II step */
	UINT8 xdata* fdRtWp		=fdRtSize+FD_FACE_CNT*4;	/* face queue, weight */

	UINT8 fdRes = 5;
	UINT8 delayCnt = 10;

	UINT32 faceMax = FD_FACE_CNT*4;
	UINT8 buf[FD_FACE_CNT*4*7ul];
	UINT8 xdata* rs = buf;//pt+512;	/* radius */
	UINT8 xdata* xs = rs+faceMax;	/* center x coordinate axis */
	UINT8 xdata* ys = xs+faceMax; 	/* center y coordinate axis */
	UINT8 xdata* ws = ys+faceMax;	/* weight */
	UINT8 xdata* cs = ws+faceMax;	/* continuous number of time */
	UINT8 xdata* is = cs+faceMax;	/* sum of ii step number */
	UINT8 xdata* ms = is+faceMax;	/* show type */
	//UINT8 keys[FD_FACE_CNT];
	
	UINT8 n,m,i,c;
	UINT16 k,k0,k1;
	UINT8 idx;//,step;

	UINT8  x1, y1, r1, d;
	UINT8  x2, y2, r2, xmin, ymin, xmax, ymax;

	UINT8 key;
	//UINT8 keyIdx=0xff;
	//UINT8 keyIdx0;
	//UINT8 keyCnt0=0,keyCnt1=0;
	UINT8 xdata *idxs = (UINT8 xdata *)G_ucStorData;
	UINT8 xdata *keys = (UINT8 xdata *)(idxs+FD_FACE_CNT);
	UINT8 faceCnt =0;

	UINT16 rstlen = FD_DECTED_OutCnt*2;
	UINT8 rstcnt;
	UINT16 cnt = FD_SIZE_RstFace;	
	UINT8 wp,show;//,flag=0;
	UINT8 x,y,r;
	static UINT8 rmaxlst=0xff;
	UINT8 rmax = 0;

	//UINT32 timers[8];
	HAL_DmaSetDramAddr(addr);
	HAL_DmaSetSramAddr(0x0000);
	HAL_DmaDo(0, 1, cnt, 1, 0);

	fdGroupPrintf("dma0-%lx,%u\n",addr,cnt);

	#if 0
	HAL_GprmDramFillExt(addr << 1, cnt, 0, 0, 0);
	
	HAL_DramSetAddr(addr, 0);
	
	for (n=0;n<cnt/2;n++) {
		HAL_DramWriteByte(0,0);
	}
	#endif

	/* read out current fd result */
	n = 0;	//current group count
	//fdDbgFDOutCnt = 0;
	for (k = 0, k0 = 0, k1 = 0, m = 0; k < cnt; k += 2) {
		if (k >= k0) {
			if (k) {
				k = k1;
				m++;
				if (m >= FD_DECTED_Cnt) {
					break;
				}
			}
			rstcnt = fdIIResultCnt[m];
			fdIIResultCnt[m] = 0;
			k0 = k1+(rstcnt<<1);
			k1 += rstlen;						
			//fdDbgFDOutCnt+=rstcnt;

			#if (USE_ARRAY_MACROS == 1)			
			x = fdHSizeFace[m];
			y = fdStepVSizeArray[m];
			r = fdStepRadiusArray[m];
			#else
			x = fdHSizeFace[m];
			y = fdStepVSize(x);
			r = fdStepRadius(x);
			#endif

			fdGroupPrintf("%3x,%3x,%2bx,%2bx\n",k,k0,m,rstcnt);
			
		}

		/* current, check result cnt not zero & output value not zero */
		if (rstcnt && pt[k] && pt[k+1]) {

			/* center point */
			x1 = (UINT16)pt[k]*160/x + r;
			y1 = (UINT16)pt[k+1]*120/y + r;
			r1 = r;

			c = 1;			

			d = r1;
			xmin = x1-d;
			ymin = y1-d;
			xmax = x1+d;
			ymax = y1+d;

			/* current, merge to old group */
			for (i = 0; i < n; i++) {
				x2 = xs[i];
				y2 = ys[i];
				r2 = rs[i];

				if(	  (	x2>=xmin 	&& x2<=xmax 	&& y2>=ymin 	&& y2<=ymax)
					||(	x1>=x2-r2 	&& x1<=x2+r2	&&  y1>=y2-r2 	&& y1<=y2+r2)) {
					if ((x1>x2?x1-x2:x2-x1)>fdRes) {
						xs[i] = ((UINT16)x1+x2)>>1;
					}
					if ((y1>y2?y1-y2:y2-y1)>fdRes) {
						ys[i] = ((UINT16)y1+y2)>>1;
					}
					if (r2<r1) {	
						rs[i] = r1;
					}
					is[i] += m;					
					cs[i]++;
					c = 0;
					break;
				}				
			}

			/* current, creat new group */
			if (c) {				
				xs[n] = x1;
				ys[n] = y1;
				rs[n] = r1;
				cs[n] = 1;
				ws[n] = 0;
				is[n] = m;
				ms[n] = 0;
				n++;
				
				if (n >= faceMax) {
					break;
				}
			}
		}
	}
	
	for (m = 0; m < fdFaceNum; m++) {
		//if (fdRtWp[m]) {
			r1 = fdRtSize[m]>>1;
			x1 = fdRtPosx[m]+r1;
			y1 = fdRtPosy[m]+r1;			

			c = 1;
			d = r1;
			xmin = x1-d;
			ymin = y1-d;
			xmax = x1+d;
			ymax = y1+d;	
			
			for (i = 0; i < n; i++) {
				x2 = xs[i];
				y2 = ys[i];
				r2 = rs[i];

				if ((	x2>=xmin 	&& x2<=xmax 	&& y2>=ymin 	&& y2<=ymax)
					||(	x1>=x2-r2 	&& x1<=x2+r2	&&  y1>=y2-r2 	&& y1<=y2+r2)) {
					if ((x1>x2?x1-x2:x2-x1)>fdRes) {
						xs[i] = ((UINT16)x1+x2)>>1;
						//xs[i] = (x1*3+x2)>>2;
					}
					else {
						xs[i] = x1;
					}
					if ((y1>y2?y1-y2:y2-y1)>fdRes) {
						ys[i] = ((UINT16)y1+y2)>>1;
						//ys[i] = (y1*3+y2)>>2;
					}
					else {
						ys[i] = y1;
					}
					//if (r2<r1) {
					//	rs[i] = r1;
					//}
					//if(((r2>r1)?(r2-r1):(r1-r2))>fdResolution)
					if (r1<r2) {
						rs[i] = ((UINT16)r1*3+r2)>>2;
					}
					else {
						//rs[i] = r2;
						rs[i] = ((UINT16)r2*3+r1)>>2;
					}
					
					is[i] += fdRtShow[m] & FD_SHOW_Step;	
					ms[i] = fdRtShow[m];

					//if (fdRtShow[i]&FD_SHOW_Key) {
					//	keyIdx0 = i;
					//}
					cs[i]++;
					c = 0;
					break;
				}				
			}
			
			if (c) {				
				xs[n] = x1;
				ys[n] = y1;
				rs[n] = r1;
				cs[n] = 0;
				ws[n] = fdRtWp[m];
				is[n] = fdRtShow[m] & FD_SHOW_Step;
				ms[n] = fdRtShow[m];
				//if(fdRtShow[i]&FD_SHOW_Key)
				//	keyIdx0 = n;
				n++;
				if (n>=faceMax) {
					break;
				}
			}
		//}
	}
	
	//fdGetGTimer(fdTimer+1);
	
	idx=0;
	for (m = 0; m < n; m++) {	
		//fdPrintfGroup("-0-%2bx,%2bx,cs:%2bx,%2bx-%2bx\n",m,idx,cs[m],ws[m],fdRtWp[idx]);

		/* weight settng and show type settng */
		wp = 0;
		if (cs[m] > 1) {						//vaild		
			wp = delayCnt;
			show = FD_SHOW_Valid;
			
			if (ms[m] & FD_SHOW_Key) {			//vaild, pre-frame is valid face and draw face and Key face
				wp++;
			}
			
			if (ms[m] & FD_SHOW_Draw) {			//vaild, pre-frame is valid face and draw face
				wp++;
			}
			
			if (ms[m] & FD_SHOW_Valid) {		//vaild, pre-frame is valid face but not show face
				wp++;
			}
		}
		else if (cs[m] == 1) {					//new		
			wp = 1;
			show = 0;							//new, set show type is zero 
		}
		else if (ws[m]) {						//old		
			wp = ws[m]-1;
			
			if (ws[m] > delayCnt-1) {		
				show = ms[m] & (FD_SHOW_Valid);	//old, set show type is valid	
			}
			else {
				show = 0;						//old, set show type is zero	
			}
		}
		else {									//drop
			continue;
		}

		/* (x,y,r) boundary check, center point */
		x = xs[m];
		y = ys[m];
		r = rs[m];
		
		if (x > 150 || y > 110 ) {
			continue;
		}
		
		if (r < 10) {
			r = 10;
		}
		else if (r > 50) {
			r = 50;
		}
		
		if (x < r) {
			x = r+1;
		}
		
		if (y < r) {
			y = r+1;
		}
		
		if (x > 157-r) {
			x = 157-r;
		}
		
		if (y > 117-r) {
			y = 117-r;
		}
		
		if (r > (rmaxlst<<1)) {
			r = rmaxlst;
		}
		
		if (r > rmax) {
			rmax = r;
		}
		
		/* save to face queue, (fdRtPosx,fdRtPosy,fdRtSize) is on left coordinate and diameter */
		fdRtWp[idx] = wp;			//save weight
		c = is[m]/(cs[m]+1);		//average ii level size
		fdRtShow[idx] = show | c;	//save average ii level size and show type
		fdRtPosx[idx] = x-r;		//save x 	(on left)		
		fdRtPosy[idx] = y-r;		//save y 	(on left)		
		fdRtSize[idx] = r<<1;		//save r 	(diameter)		
		
		/* key weight setting */
		if (show) {

			fdRtShow[idx] |= FD_SHOW_Draw;	//show type set draw

			/* center key weight */
			x2 = (x>80) ? x-80 : 80-x;	
			y2 = (y>60) ? y-60 : 60-y;
			r2 = r;
			key = 140-x2-y2;

			/* pre-frame show type key weight */
			c = ms[m];
			if (c & FD_SHOW_Key) {
				key += 30;
			}
			if (c & FD_SHOW_Draw) {
				key += 30;
			}
			if (c & FD_SHOW_Valid) {
				key += 30;
			}

			//fdGroupPrintf("(%bu,%bu)%bx,%bu,%bu\n",x2,y2,c,cs[m],key);

			/* order to key weight */
			c = (fdFaceMax > faceCnt) ? faceCnt : fdFaceMax;			
			keys[c] = key;
			idxs[c] = idx;			
			for (i = c; i > 0; i--) {
				if (key > keys[i-1]) {
					keys[i] = keys[i-1];
					keys[i-1] = key;
					idxs[i] = idxs[i-1];
					idxs[i-1] = idx;
					//fdGroupPrintf("%bu-%bu\n",i,idx);
				}else {
					break;
				}
			}
			
			/* limit max draw face num */
			if (c == fdFaceMax) {
				fdRtShow[idxs[c]] &= ~FD_SHOW_Draw;
			}

			faceCnt++;			
		}
		
		idx++;

		if (idx>=FD_FACE_CNT) {
			break;
		}
	}
	
	//fdGroupPrintf("-\n");
	
	//fdGetGTimer(fdTimer);
	if (rmax) {
		rmaxlst = rmax;
	}
	//fdGroupPrintf("-1-%2bu,%2bu,%2bu,%2bu\n",fdFaceNum,fdKeyStep,fdRotate,fdDbgFDOutCnt );
	//fdGetGTimer(timers+0);
	
	/* lack face */
	if ((!fdFaceNum && !idx) ||(fdRotate && faceCnt)) {
		//rotate database
		code UINT8 rotate[]={FD_ROTATE_Normal,FD_ROTATE_Rotate90,/*FD_ROTATE_Rotate180,*/FD_ROTATE_Rotate270};
		fdRotate++;
		if (fdRotate>sizeof(rotate)) {
			fdRotate=0;
		}
		HAL_FdDBRotateSet(rotate[fdRotate]);
	}	
	
	//fdGetGTimer(timers+1);
		
	/* key face set to (fdKeyRtX,fdKeyRtY,fdKeyRtS), on left coordinate and diameter */
	fdFaceNum = idx;
	if (fdFaceMax && faceCnt) {
		UINT8 keyIdx = idxs[0];
		fdRtShow[keyIdx] |= FD_SHOW_Key;
		fdKeyRtS = fdRtSize[keyIdx];
		fdKeyRtX = fdRtPosx[keyIdx];
		fdKeyRtY = fdRtPosy[keyIdx];
		fdKeyStep = fdRtShow[keyIdx] & FD_SHOW_Step;
		fdKeyIdx = keyIdx;
	}
	else {
		fdKeyRtS = 0;
		fdKeyRtX = 0;
		fdKeyRtY = 0;
		fdKeyStep = FD_DECTED_Cnt;		
	}	
	//fdGetGTimer(timers+2);
	//fdGroupPrintf("%2bu,%2bu,%2bu\n",faceCnt,fdFaceNum,fdDbgFDOutCnt);
	fdGroupPrintf("%bu(%2bu-%2bu-%2bu-%2bu-%2bu-%2bu)(%2bu,%2bu)%2bu,%2bu\n",fdStep,fdFaceNum,idx,faceCnt,n,fdDbgFDOutCnt,fdFaceMax,fdKeyIdx,fdKeyStep,fdRotate);
	//fdGroupPrintf("%bu-(%lu,%lu,%lu)\n",fdFaceNum,timers[0]-fdTimer[0],timers[1]-timers[0],timers[2]-timers[1])	;
}

/**
 * @fn        void fdInitAddr(UINT32 startAddrW)
 * @brief     initialize fd addr
 * @param     [in] startAddrW(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
fdInitAddr(
	UINT32 addr
)
{
	UINT8 i;
	//UINT32 addr;

	//addr=K_SDRAM_PvFdIIAddr;

	HAL_GprmDramFillExt(addr<<1, K_SDRAM_FdIIBufSize, 0, 0, 0);

	fdTblAddr = addr;
	fdOutAddr = fdTblAddr + FD_SIZE_DataBase/2;

	fdIISrcAddr = fdOutAddr+FD_SIZE_RstFace/2 + FD_SIZE_RstSub;//face +2 sub
	fdSIIAddr = fdIISrcAddr+160ul*120*2;

	addr = fdSIIAddr+160ul*120*2;
	
	for (i = 0; i < FD_II_CNT; i++) {
		fdIIDstAddr[i] = addr;
		addr = addr + 160ul*120*2;
		//addr = addr + (UINT32)fdGetHSize(i)*fdGetVSize(i)*2;
	}
	
	fdIIDstAddr[FD_II_CNT] = fdIISrcAddr;

	fdJobAddr = K_SDRAM_DPSStartAddr + (14*512ul);
	fdJobAddrSub = fdJobAddr+FD_SIZE_Queue / 2;//(sizeof(fd_iijobtable_t)*14ul/2);

	fdDbgSize = (addr-K_SDRAM_PvFdIIAddr) * 2;
	
}

/**
 * @fn        void fdInitQueue(UINT8 *pHsize,UINT32 jobAddr)
 * @brief     initialize fd II Queue
 * @param    
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
fdInitQueue(
	UINT8 *pHsize,
	UINT32 jobAddr
)
{
	UINT8 idx;
	UINT8 pthsize[FD_II_CNT];
	UINT8 ptvsize[FD_II_CNT];
	UINT8 hsize;
	
	for (idx=0;idx<FD_II_CNT;idx++) {
		hsize=pHsize[idx];
		pthsize[idx] = hsize;
		ptvsize[idx] = fdStepVSize(hsize);
	}

	HAL_FdIIJobQueue( 
		320, //240, 
		0, 0, 320, 240, 
		160, 120, 
		0,  fdIISrcAddr,  fdSIIAddr,
		FD_SIZE_IIX, FD_SIZE_IIY, pthsize, ptvsize, fdIIDstAddr,
		G_ucStorData,
		jobAddr,
		FD_II_CNT
	);
}

/**
 * @fn        void fdProcFace(void)
 * @brief     fd procress
 * @param    
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
fdProcFace(
	void
)
{
	UINT8 cdspIdx = HAL_CdspWriteBufIdxGet();
	UINT32 dbData, addr;
	
	fdProcFacePrintf("NUM:%2bu-%lu,FD:%lu,II:%lu,DB:%lu,DR:%lu,GP:%lu(%lu-%lu-%lu)\n",fdFaceNum,fdTimer[3]-fdTimer[2],fdTimer[9]-fdTimer[8],fdTimer[8]-fdTimer[7],fdTimer[6]-fdTimer[7],fdTimer[4]-fdTimer[5],fdTimer[3]-fdTimer[4],fdTimer[1]-fdTimer[4],fdTimer[0]-fdTimer[1],fdTimer[3]-fdTimer[0]);
	
	fdGetGTimer(fdTimer+2);
	
	HAL_DramSetAddr(fdTblAddr, 1);
	HAL_DramRead32(&dbData);
	
	/* dram data modify */
	if (dbData!=fdDBData) {	
		sp1kFdReset();
		return;
	}
	
	if (cdspIdx!=fdCdspIdx) {
		addr = fdGetCdspBuf(cdspIdx,2);
		
		//fdProcFacePrintf("fd:%lx,%bx,%bx\n",addr,cdspIdx,fdCdspIdx);
		
		fdCdspIdx= cdspIdx;
	
		HAL_DramSetAddr(fdJobAddr+7, 0);
		HAL_DramWrite32(addr);
		HAL_DramSetAddr(fdJobAddr+7+16, 0);
		HAL_DramWrite32(addr);
	}	
	
	fdGetGTimer(fdTimer+7);

	#if (USE_ARRAY_MACROS == 1)
	jumpIdx = 0;
	//printf("\nfd II tg jumpIdx=%bx\n",jumpIdx);
	#endif

	/* save face ii data */
	fdSaveFaceII();

	/* save face yuv source */
	fdSaveFaceYuvSrc(addr);
	
	if (HAL_FdIITrig(fdJobAddr,0)) {
		iiDbgTrig = II_Trigger_Fd;
	}
	
	if (fdFlow!=FD_FLOW_Face) {		
		HAL_FdSetDataBase(FD_ADDR_DBFace);
		HAL_FdWinStepSet(FD_FACE_SUB_WIN_STEP);
		HAL_FdStage1Set(fdDbgStage0);
		HAL_FdBufMaxCntSet(FD_DECTED_OutCnt);
		HAL_FdTagEnable(0,0);

		fdFlow = FD_FLOW_Face;
	}
	
	fdGetGTimer(fdTimer+6);
	
	//fdIntr();
	//HAL_DmaSetDramAddr(fdOutAddr);
	//HAL_DmaSetSramAddr(0x0000);
	//HAL_DmaDo(0, 1, FD_SIZE_RstFace, 1, 0);
	
	fdGetGTimer(fdTimer+5);
	
	//fdDraw();
	
	fdGetGTimer(fdTimer+4);
	
	fdGroup();
	
	//fdFuncExt();
	
	fdGetGTimer(fdTimer+3);
}
#endif

/**
 * @fn        void fdSvcProc(void* ptr)
 * @brief     fdSvcProc
 * @param     [in] ptr
 * @retval    NONE
 * @author    wangjg
 * @since     2010-6-17
*/
void 
fdSvcProc(
	void* ptr
)
{	
#if (FD_ENABLE == 1)
#if (FD_PROC_V1 == 2)

	UINT8 vd = (*(UINT8*)ptr) & 0x03;
	static UINT8 rstCnt = 0;
	/*
	if (fdDbgCnt) {
		if (fdDbgCnt!=0xff) {
			fdDbgCnt--;
		}
		fdSvcProcPrintf("num:%2bu,key:%2bu,out:%2bu,rot:%2bu,flw:%bx\n"
			,fdFaceNum
			,fdKeyStep
			,fdDbgFDOutCnt
			,fdRotate
			,fdFlow
			);		
	}
	*/
	fdSvcProcPrintf("%2bu,%2bu,%2bu,%2bu,%2bu",vd,fdStep,fdFlow,fdDone,rstCnt );
	fdSvcProcPrintf("--%2bu,%2bu,%2bu\n",fdFaceNum,fdKeyStep,fdRotate );
	if (fdDone == FD_FLOW_Face && fdSmileEn && fdFaceNum && fdKeyRtS && (fdKeyStep<FD_DECTED_Cnt))	{
		fdStep = 0;
		fdDone = 0xff;
		rstCnt = 0;
		fdSvcProcPrintf("S\n");
		fdProcSmile();		
	}
	else if (fdDone == FD_FLOW_Smile) {
		fdStep = 0;
		fdDone = 0xff;
		rstCnt = 0;
		fdSvcProcPrintf("N\n");
		fdProcNonSmile();		
	}
	else if (fdDone != 0xff) {
		fdStep = 0;
		fdDone = 0xff;		
		rstCnt = 0;
		fdSvcProcPrintf("F\n");
		fdProcFace();
		
	}
	else if (rstCnt++>8) {
		fdFlow = 0xff;
		fdStep = 0;
		fdDone = FD_FLOW_Face;
		rstCnt = 0;
		fdSvcProcPrintf("R\n");
		HAL_FdRst();		
	}
	else {
		//fdSvcProcPrintf("D\n");
		//fdDraw();
	}
#elif (FD_PROC_V1 == 1)

	UINT8 vd = (*(UINT8*)ptr) & 0x03;
	static UINT8 rstCnt = 0;
	/*
	if (fdDbgCnt) {
		if(fdDbgCnt!=0xff)fdDbgCnt--;		
		fdSvcProcPrintf("num:%2bu,key:%2bu,out:%2bu,rot:%2bu,flw:%bx\n"
			,fdFaceNum
			,fdKeyStep
			,fdDbgFDOutCnt
			,fdRotate
			,fdFlow
			);		
	}
	*/
	fdSvcProcPrintf("%2bu,%2bu,%2bu,%2bu,%2bu",vd,fdStep,fdFlow,fdDone,rstCnt );
	fdSvcProcPrintf("--(%2bu,%2bu)%2bu,%2bu\n",fdFaceNum,fdDbgFDOutCnt,fdKeyStep,fdRotate );
	if (fdDone == FD_FLOW_Face && fdSmileEn && fdFaceNum && fdKeyRtS && 
			(fdKeyStep<FD_DECTED_Cnt) && (vd==3))	{
		fdStep = 0;
		fdDone = 0xff;
		rstCnt = 0;
		fdSvcProcPrintf("S\n");
		fdProcSmile();
	}
	else if (fdDone == FD_FLOW_Smile) {
		fdStep = 0;
		fdDone = 0xff;
		rstCnt = 0;
		fdSvcProcPrintf("N\n");
		fdProcNonSmile();		
	}
	else if (fdDone != 0xff && (vd==3)) {
		fdStep = 0;
		fdDone = 0xff;
		rstCnt = 0;
		fdSvcProcPrintf("F\n");
		fdProcFace();			
	}
	else if (rstCnt++>16) {		
		fdFlow = 0xff;
		fdStep = 0;
		fdDone = 0xfe;
		rstCnt = 0;
		fdSvcProcPrintf("R\n");
		HAL_FdRst();		
	}
	else {
		//fdSvcProcPrintf("D\n");
		//fdDraw();
	}

#else
	UINT8 vd = (*(UINT8*)ptr) & 0x03;
	UINT8 cnt = 0x03;
	
	if (vd==3) {		
		fdStep = 0;

		if (fdCounter==cnt && fdSmileEn) {	/* smile */
			fdProcSmile();
			fdCounter++;
		}
		else if (fdCounter>cnt && fdSmileEn) { /* smile */
			if (fdDbgSlow2En) {
				fdProcNonSmile();
			}
			fdCounter=0;
		}
		else {			
			fdProcFace();
			fdCounter++;
		}
		
		if (fdDbgCnt) {
			if (fdDbgCnt!=0xff) {
				fdDbgCnt--;
			}
			/*
			fdSvcProcPrintf("num:%2bu,max:%2bx,rot:%2bu,step:%2bu,flow:%2bu,%2bu,(%2bu)\n"
				,fdFaceNum,fdFaceMax,fdRotate,fdStep
				,fdCounter
				,fdDbgFDOutCnt
				,fdKeyStep
				);
			*/
			fdSvcProcPrintf("num:%2bu,key:%2bu,out:%2bu,rot:%2bu,flw:%bx\n"
				,fdFaceNum
				,fdKeyStep
				,fdDbgFDOutCnt
				,fdRotate
				,fdFlow
				);		
		}
	}
#endif

	/*
	else if(vd==0 && fdMode==FD_MODE_Smile && fdCounter>FD_COUNTER_Sub)
	{
		fdStep=fdKeyStep;	
		fdSetDB(2);
		fdTrig();
		fdCounter=0;
	}
	*/
	fdDraw();
#else 
	ptr = ptr;
#endif	
}

/**
 * @fn        void fdIntr(void)
 * @brief     trigger fdDetect() by interrupt (It will called by interrupt)
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void
fdIntr(
	void
) 
{
#if (FD_ENABLE == 1)
	UINT8 lv = fdStep++;
	UINT32 iiaddr;
	UINT8  iihsize;

	UINT32 fdoutaddr ;
	UINT8 scalefactor;
	UINT16 areafactor;

	UINT8 strx=0;
	UINT8 stry=0; 
	UINT8 endx;
	UINT8 endy;
	
	if (!fdEn) {
		return;
	}
	
	fdDone = 0xff;
	if (lv) {
		UINT32 out0,out1,cnt;
		WRITE8(out0, 3, XBYTE[REG_FD_OutCurrentAddr0]);
		WRITE8(out0, 2, XBYTE[REG_FD_OutCurrentAddr1]);
		WRITE8(out0, 1, XBYTE[REG_FD_OutCurrentAddr2]);
		WRITE8(out0, 0, XBYTE[REG_FD_OutCurrentAddr3]);
		WRITE8(out1, 3, XBYTE[REG_FD_OutStartAddr0]);
		WRITE8(out1, 2, XBYTE[REG_FD_OutStartAddr1]);
		WRITE8(out1, 1, XBYTE[REG_FD_OutStartAddr2]);
		WRITE8(out1, 0, XBYTE[REG_FD_OutStartAddr3]);
		
		cnt = out0-out1;
		fdIIResultCnt[lv-1] = (cnt>FD_DECTED_OutCnt)?0:cnt;
		fdDbgFDOutCnt += fdIIResultCnt[lv-1];
	}
	else {
		fdDbgFDOutCnt = 0;
		fdGetGTimer(fdTimer+8);
		//intrPutChar('R');
		//intrPutChar('\n');
	}
	
	if (fdFlow==FD_FLOW_Face) {
		if (lv>=FD_DECTED_Cnt ) {		
			fdDone = fdFlow;
			fdGetGTimer(fdTimer+9);
			return;		
		}
		iihsize=fdHSizeFace[lv];
		fdoutaddr = fdOutAddr+(lv*FD_DECTED_OutCnt);

		//intrPutChar('F');
		//intrPutUint8(jumpIdx);
	}
	else {
		if (lv>=FD_DECTED_Cnt-1) {		
			fdDone = fdFlow;
			fdGetGTimer(fdTimer+9);
			return;		
		}
		iihsize=fdHSizeSub[lv];
		fdoutaddr = fdOutAddr+FD_SIZE_RstFace/2+(lv*FD_DECTED_OutCntSub);
		if (fdFlow==2) {
			fdoutaddr+=FD_SIZE_RstSub/2;
		}
		//intrPutChar('S');
		//intrPutUint8(jumpIdx);
	}
	
	iiaddr=fdIIDstAddr[lv];
	endx=iihsize;

	#if (USE_ARRAY_MACROS == 1)
	endy=fdStepVSizeArray[lv + jumpIdx];
	scalefactor=fdStepFacArray[lv + jumpIdx];
	areafactor=fdStepAreaArray[lv + jumpIdx];
	#else
	endy=fdStepVSize(iihsize);
	scalefactor=fdStepFac(iihsize);
	areafactor=fdStepArea(iihsize);
	#endif

	//intrPutUint8(endx);
	//intrPutUint8(endy);

	XBYTE[REG_Dram_IIAddr2_B0] = ((unsigned char *)&(iiaddr))[3];
	XBYTE[REG_Dram_IIAddr2_B1] = ((unsigned char *)&(iiaddr))[2];
	XBYTE[REG_Dram_IIAddr2_B2] = ((unsigned char *)&(iiaddr))[1];
	XBYTE[REG_Dram_IIAddr2_B3] = ((unsigned char *)&(iiaddr))[0];
	XBYTE[REG_Dram_II_HSize2] = iihsize;
	
	XBYTE[REG_FD_OutStartAddr0] = ((unsigned char *)&(fdoutaddr))[3];
	XBYTE[REG_FD_OutStartAddr1] = ((unsigned char *)&(fdoutaddr))[2];
	XBYTE[REG_FD_OutStartAddr2] = ((unsigned char *)&(fdoutaddr))[1];
	XBYTE[REG_FD_OutStartAddr3] = ((unsigned char *)&(fdoutaddr))[0];	
	

	XBYTE[REG_FD_SaleFactor] = scalefactor;
	XBYTE[REG_FD_Areafactor0] = ((unsigned char *)&(areafactor))[1];
	XBYTE[REG_FD_Areafactor1] = ((unsigned char *)&(areafactor))[0];
	

	XBYTE[REG_FD_ROIx0] = strx;
	XBYTE[REG_FD_ROIy0] = stry;
	XBYTE[REG_FD_ROIx1] = endx;
	XBYTE[REG_FD_ROIy1] = endy;
	

	if (!XBYTE[REG_FD_Trigger]) {
		iiDbgTrig = II_Trigger_Fd;
		XBYTE[REG_FD_Trigger]   = 1;
	}
#endif
}

/**
 * @fn        void sp1kFdReset(UINT32 stardAddrW)
 * @brief     initialize  libfd
 * @param     [in] stardAddrW(w)
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-4-28
 * @todo      N/A
 * @bug       N/A
*/
void 
sp1kFdReset(
	void
)
{	
#if (FD_ENABLE == 1)
	UINT8 sts = sp1kFdDisable();
	UINT8 proc = osSysModeGet();

	fdSysInfo("fd Reset:%bx\n",proc);
	
	if (proc==OS_SYS_MODE_STILL_PV) {
		fdInitAddr(K_SDRAM_PvFdIIAddr);
	}
	else {
		fdInitAddr(K_SDRAM_AviCapVLCBufAddrC);
		//fdInitAddr(vidRecMemAddrGet(VID_MEM_IDX_VIDEO_REC_VLC_C));
	}

	fdInitQueue(fdHSizeFace,FD_ADDR_QueueFace);
	fdHSizeSub = fdHSizeFace+FD_DECTED_Cnt;
	fdInitQueue(fdHSizeSub,FD_ADDR_QueueSub0);
	fdInitQueue(fdHSizeSub+FD_DECTED_Cnt,FD_ADDR_QueueSub1);
	//fdInitQueue(fdHSizeSub+FD_DECTED_Cnt*2,FD_ADDR_QueueSub2);
	
	HAL_FdDBRotateSet(DEFAULT_ROTATE);
	HAL_FdSIISet(fdSIIAddr,FD_SII_HSIZE);	
	HAL_DramFrmBufNumSet(2);
	
	sp1kNandRsvRead(DEFAULT_RES_ID, DEFAULT_DB_ADDRESS<<1);
	//sp1kNandRsvRead(RES_ID_Smile, FD_ADDR_DBSub0<<1);
	//sp1kNandRsvRead(RES_ID_NonSmile, FD_ADDR_DBSub1<<1);

	if (fdSmileEn) {
		sp1kFdSetSmile(1);
	}
	
	HAL_DramSetAddr(fdTblAddr, 2);
	HAL_DramRead32(&fdDBData);
	fdRotate = 0xff;
	
	if (sts) {
		fdEnable(sts);
	}
#endif
}

/**
 * @fn        UINT32 sp1kFdParamSet(UINT8 idx, UINT32 val)
 * @brief     sp1kFdParamSet
 * @param     [in] idx
 * @param     [in] val	:write 0xffffffff return param val
 * @retval    return = lst value
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-9
 * @todo      N/A
 * @bug       N/A
*/
UINT32 
sp1kFdParamSet(
	UINT8 idx,
	UINT32 val
)
{	
#if (FD_ENABLE == 1)
	code UINT8 xdata* tbl[]={		
		&fdFaceMax,		
		//&fdResolution,
		//&fdMode,
		//read only
		&fdFaceNum,
		&fdRotate,
		&fdDrawMode,
		&fdIISrcAddr,
	};
	UINT8 xdata* pt = tbl[idx];
	UINT16 xdata* pt16 = (UINT16 xdata*)pt;
	UINT32 xdata* pt32 = (UINT32 xdata*)pt;
	//UINT32 res =*pt;
	
	if (idx>=FD_PARAM_Num) {
		return 0;
	}
	else if (idx>=FD_PARAM_UINT32) {
		if (val!= 0xffffffff) {
			*pt32 = val;
		}
		return *pt32;
	}
	else if (idx>=FD_PARAM_UINT16) {
		if (val!= 0xffffffff) {
			*pt16 = val;
		}
		return *pt16;
	}
	else {// if(idx>=FD_PARAM_UINT8)
		if (val!= 0xffffffff) {
			*pt = val;
		}
		return *pt;
	}
#else
	idx = idx;
	val = val;
	return 0;
#endif
}

/**
 * @fn        UINT8 fdEnble(UINT8 en)
 * @brief     fdEnble
 * @param     [in] en
 * @retval    
 * @author    wangjg
 * @since     2010-6-21
*/
UINT8 
fdEnable(
	UINT8 en
)
{
#if (FD_ENABLE == 1)

	fdSysInfo("fd En:%bx\n",en);

	if (en==0) {
		//HAL_FDIntrEnable(0,0,0,0);
		HAL_FDIntrEnable(0,0,0,0);
		HAL_FdEnable(0,0);
		
		HAL_FdEnable(1,0);
		HAL_FDIntrEnable(1,0,0,0);
		
		fdCdspIdx=0xff;	//renew jobQueue
		fdFlow = 0xff;	//reload database
		fdFaceNum=0;
		fdCounter = 1;	

		fdKeyRtX=0;
		fdKeyRtY=0;
		fdKeyRtS=0;
		fdKeyStep=13;
		fdKeyIdx=0;	

		fdEn = 0;
		fdStep = 0;
		//sp1kFdSetSmile(0);
	}
	else if (en!=0xff) {		
		HAL_FDIntrEnable(1,0,0,1);
		HAL_FdEnable(1,1);
		fdEn = 1;
		fdDone = 0xfe;
	}
	else {
		HAL_FdEnable(0,0);
		HAL_FdEnable(0,0);
		HAL_FdEnable(1,1);
	}
	return sp1kVdProcEnable(VD_PROC_FD,en)?1:0;
#else
	en = en;
	return 0;
#endif
}

#if (FD_SAVE_FACE_YUV_SRC_DBG == 1)
/**
 * @fn        void fdSaveFaceYuvSrcDbg(void)
 * @brief     save face yuv source data (yuv 422)
 * @param     NULL
 * @retval    
 * @author    wangjg
 * @since     2010-6-21
*/
#include "utility.h"
void
fdSaveFaceYuvSrcDbg(
	UINT32 srcYuvAddr
)
{
	if (fdSaveFaceYuvSrcDbgFlag == 1) {
		dramFileSave("FDYU0000.YUV",srcYuvAddr,K_SDRAM_Preview2BufSizeA*2);//
		fdSaveFaceYuvSrcDbgFlag = 0;
	}
}
#endif

#if (FD_SAVE_FACE_II_DBG == 1)
/**
 * @fn        void fdSaveFaceIIDbg(void)
 * @brief     save face ii data
 * @param     NULL
 * @retval    
 * @author    wangjg
 * @since     2010-6-21
*/
#include "utility.h"
void
fdSaveFaceIIDbg(
	void
)
{
	if (fdSaveFaceIIDbgFlag == 1) {
		//while (iiDbgTrig != II_Trigger_Fd);
		dramFileSave("SRC00001.BIN",fdIIDstAddr[0],(UINT32)FD_II_H_SIZE_0*FD_II_V_SIZE_0*4);
		dramFileSave("SRC00002.BIN",fdIIDstAddr[1],(UINT32)FD_II_H_SIZE_1*FD_II_V_SIZE_1*4);
		dramFileSave("SRC00003.BIN",fdIIDstAddr[2],(UINT32)FD_II_H_SIZE_2*FD_II_V_SIZE_2*4);
		dramFileSave("SRC00004.BIN",fdIIDstAddr[3],(UINT32)FD_II_H_SIZE_3*FD_II_V_SIZE_3*4);
		dramFileSave("SRC00005.BIN",fdIIDstAddr[4],(UINT32)FD_II_H_SIZE_4*FD_II_V_SIZE_4*4);
		dramFileSave("SRC00006.BIN",fdIIDstAddr[5],(UINT32)FD_II_H_SIZE_5*FD_II_V_SIZE_5*4);
		dramFileSave("SRC00007.BIN",fdIIDstAddr[6],(UINT32)FD_II_H_SIZE_6*FD_II_V_SIZE_6*4);
		dramFileSave("SRC00008.BIN",fdIIDstAddr[7],(UINT32)FD_II_H_SIZE_7*FD_II_V_SIZE_7*4);
		dramFileSave("SRC00009.BIN",fdIIDstAddr[8],(UINT32)FD_II_H_SIZE_8*FD_II_V_SIZE_8*4);
		dramFileSave("SRC00010.BIN",fdIIDstAddr[9],(UINT32)FD_II_H_SIZE_9*FD_II_V_SIZE_9*4);
		dramFileSave("SRC00011.BIN",fdIIDstAddr[10],(UINT32)FD_II_H_SIZE_10*FD_II_V_SIZE_10*4);
		dramFileSave("SRC00012.BIN",fdIIDstAddr[11],(UINT32)FD_II_H_SIZE_11*FD_II_V_SIZE_11*4);
		dramFileSave("SRC00013.BIN",fdIIDstAddr[12],(UINT32)FD_II_H_SIZE_12*FD_II_V_SIZE_12*4);
		dramFileSave("SRC00SII.BIN",fdSIIAddr,(UINT32)160*120*4);
		fdSaveFaceIIDbgFlag = 0;
	}
}
#endif

#if (FD_SAVE_SMILE_YUV_SRC_DBG == 1)
/**
 * @fn        void fdSaveSmileYuvSrcDbg(void)
 * @brief     save smile yuv source data (yuv 422)
 * @param     NULL
 * @retval    
 * @author    wangjg
 * @since     2010-6-21
*/
#include "utility.h"
void
fdSaveSmileYuvSrcDbg(
	UINT32 srcYuvAddr
)
{
	UINT32 byteSize;
	
	if (fdSaveSmileYuvSrcDbgFlag == 1) {
		if (fdSaveSmileIIDbgPath == 1) {
			byteSize = K_SDRAM_PreviewBufSizeA*2;
		}
		else {
			byteSize = K_SDRAM_Preview2BufSizeA*2;
		}
		dramFileSave("SMYU0000.YUV",srcYuvAddr,byteSize);//
		fdSaveSmileYuvSrcDbgFlag = 0;
	}
}
#endif


#if (FD_SAVE_SMILE_II_DBG == 1)
/**
 * @fn        void fdSaveSmileIIDbg(void)
 * @brief     save smile ii data
 * @param     NULL
 * @retval    
 * @author    wangjg
 * @since     2010-6-21
*/
#include "utility.h"
void
fdSaveSmileIIDbg(
	void
)
{
	UINT32 addr;
	if (fdSaveSmileIIDbgFlag == 1) {
		if (fdJobAddrSub == FD_ADDR_QueueSub0) {
			dramFileSave("SMC00001.BIN",fdIIDstAddr[0],(UINT32)SMILE_II_H_SIZE_0*SMILE_II_V_SIZE_0*4);
			dramFileSave("SMC00002.BIN",fdIIDstAddr[1],(UINT32)SMILE_II_H_SIZE_1*SMILE_II_V_SIZE_1*4);
			dramFileSave("SMC00003.BIN",fdIIDstAddr[2],(UINT32)SMILE_II_H_SIZE_2*SMILE_II_V_SIZE_2*4);
			dramFileSave("SMC00004.BIN",fdIIDstAddr[3],(UINT32)SMILE_II_H_SIZE_3*SMILE_II_V_SIZE_3*4);
			dramFileSave("SMC00005.BIN",fdIIDstAddr[4],(UINT32)SMILE_II_H_SIZE_4*SMILE_II_V_SIZE_4*4);
			dramFileSave("SMC00006.BIN",fdIIDstAddr[5],(UINT32)SMILE_II_H_SIZE_5*SMILE_II_V_SIZE_5*4);
			dramFileSave("SMC00007.BIN",fdIIDstAddr[6],(UINT32)SMILE_II_H_SIZE_6*SMILE_II_V_SIZE_6*4);
			dramFileSave("SMC00008.BIN",fdIIDstAddr[7],(UINT32)SMILE_II_H_SIZE_7*SMILE_II_V_SIZE_7*4);
			dramFileSave("SMC00009.BIN",fdIIDstAddr[8],(UINT32)SMILE_II_H_SIZE_8*SMILE_II_V_SIZE_8*4);
			dramFileSave("SMC00010.BIN",fdIIDstAddr[9],(UINT32)SMILE_II_H_SIZE_9*SMILE_II_V_SIZE_9*4);
			dramFileSave("SMC00011.BIN",fdIIDstAddr[10],(UINT32)SMILE_II_H_SIZE_10*SMILE_II_V_SIZE_10*4);
			dramFileSave("SMC00012.BIN",fdIIDstAddr[11],(UINT32)SMILE_II_H_SIZE_11*SMILE_II_V_SIZE_11*4);
			dramFileSave("SMC00013.BIN",fdIIDstAddr[12],(UINT32)SMILE_II_H_SIZE_12*SMILE_II_V_SIZE_12*4);
			dramFileSave("SRC00SII.BIN",fdSIIAddr,(UINT32)160*120*4);

		}
		else if (fdJobAddrSub == FD_ADDR_QueueSub1) {
			dramFileSave("SMC00014.BIN",fdIIDstAddr[0],(UINT32)SMILE_II_H_SIZE_13*SMILE_II_V_SIZE_13*4);
			dramFileSave("SMC00015.BIN",fdIIDstAddr[1],(UINT32)SMILE_II_H_SIZE_14*SMILE_II_V_SIZE_14*4);
			dramFileSave("SMC00016.BIN",fdIIDstAddr[2],(UINT32)SMILE_II_H_SIZE_15*SMILE_II_V_SIZE_15*4);
			dramFileSave("SMC00017.BIN",fdIIDstAddr[3],(UINT32)SMILE_II_H_SIZE_16*SMILE_II_V_SIZE_16*4);
			dramFileSave("SMC00018.BIN",fdIIDstAddr[4],(UINT32)SMILE_II_H_SIZE_17*SMILE_II_V_SIZE_17*4);
			dramFileSave("SMC00019.BIN",fdIIDstAddr[5],(UINT32)SMILE_II_H_SIZE_18*SMILE_II_V_SIZE_18*4);
			dramFileSave("SMC00020.BIN",fdIIDstAddr[6],(UINT32)SMILE_II_H_SIZE_19*SMILE_II_V_SIZE_19*4);
			dramFileSave("SMC00021.BIN",fdIIDstAddr[7],(UINT32)SMILE_II_H_SIZE_20*SMILE_II_V_SIZE_20*4);
			dramFileSave("SMC00022.BIN",fdIIDstAddr[8],(UINT32)SMILE_II_H_SIZE_21*SMILE_II_V_SIZE_21*4);
			dramFileSave("SMC00023.BIN",fdIIDstAddr[9],(UINT32)SMILE_II_H_SIZE_22*SMILE_II_V_SIZE_22*4);
			dramFileSave("SMC00024.BIN",fdIIDstAddr[10],(UINT32)SMILE_II_H_SIZE_23*SMILE_II_V_SIZE_23*4);
			dramFileSave("SMC00025.BIN",fdIIDstAddr[11],(UINT32)SMILE_II_H_SIZE_24*SMILE_II_V_SIZE_24*4);
			dramFileSave("SMC00026.BIN",fdIIDstAddr[12],(UINT32)SMILE_II_H_SIZE_25*SMILE_II_V_SIZE_25*4);
			dramFileSave("SRC00SII.BIN",fdSIIAddr,(UINT32)160*120*4);

		}

		addr= fdGetCdspBuf(0,1);
		dramFileSave("ONEP0000.YUV",addr,(UINT32)640*480*2);
		addr= fdGetCdspBuf(1,1);
		dramFileSave("ONEP0001.YUV",addr,(UINT32)640*480*2);
		addr= fdGetCdspBuf(2,1);
		dramFileSave("ONEP0002.YUV",addr,(UINT32)640*480*2);
		addr= fdGetCdspBuf(3,1);
		dramFileSave("ONEP0003.YUV",addr,(UINT32)640*480*2);


		addr= fdGetCdspBuf(0,2);
		dramFileSave("TWOP0000.YUV",addr,(UINT32)320*240*2);
		addr= fdGetCdspBuf(1,2);
		dramFileSave("TWOP0001.YUV",addr,(UINT32)320*240*2);
		addr= fdGetCdspBuf(2,2);
		dramFileSave("TWOP0002.YUV",addr,(UINT32)320*240*2);
		addr= fdGetCdspBuf(3,2);
		dramFileSave("TWOP0003.YUV",addr,(UINT32)320*240*2);
		fdSaveSmileIIDbgFlag = 0;
	}
}
#endif

/**
 * @fn        void fdTest(UINT8* pt)
 * @brief     fdEnble
 * @param     [in] en
 * @retval    
 * @author    wangjg
 * @since     2010-6-21
*/
#if 0
extern UINT32 xdata G_TimerThresholdRef;
extern UINT8 xdata G_TimerFlag;
#endif
void 
fdTest(
	UINT8* pt
)
{
#if (FD_ENABLE == 1)
	#if 0
	if (*pt == 'a') {
		G_TimerThresholdRef = 10;//10us uint
	}
	else if (*pt == 'b') {
		G_TimerThresholdRef = 100;//10us uint
	}
	else if (*pt == 'c') {
		G_TimerFlag = 0;
	}
	else if (*pt == 'd') {
		G_TimerFlag = 1;
	}
	#endif
#if (FD_DBG_CMD == 1)
	UINT32 val[5];
	//UINT32 rst;

	cmdArgGet(&pt, val, 5, 1);

	if (strncmp((UINT8*)val[0], "r", 1) == 0) {
		sp1kFdReset();
	}
	
	else if ((UINT8)val[0] == 1) {
		sp1kFdEnable();
		sp1kFdSetSmile((UINT8)val[1]);	
		sp1kFdParamSet(FD_PARAM_ShowMode, (UINT32)val[2]);
	}
	
	else if ((UINT8)val[0] == 0) {
		sp1kFdDisable();		
	}
	
	#if 0
	else if (strncmp((UINT8*)val[0], "p", 1) == 0) {
		fdDbgCnt = val[1];
		fdDbgSmileCnt = val[2];	
	}
	#endif
	
	else if (strncmp((UINT8*)val[0], "f", 1) == 0) {
		HAL_FdRst();
	}
	
	else if (strncmp((UINT8*)val[0], "x", 1) == 0) {
		fdFaceMax=(UINT8)val[1];
	}
	
	#if (FD_SAVE_FACE_YUV_SRC_DBG == 1)
	else if (strncmp((UINT8*)val[0], "fyuv", 4) == 0) {
		fdSaveFaceYuvSrcDbgFlag = 1;
	}
	#endif
	
	#if (FD_SAVE_FACE_II_DBG == 1)
	else if (strncmp((UINT8*)val[0], "iif", 3) == 0) {
		fdSaveFaceIIDbgFlag = 1;
	}
	#endif
	
	#if (FD_SAVE_SMILE_YUV_SRC_DBG == 1)
	else if (strncmp((UINT8*)val[0], "syuv", 4) == 0) {
		fdSaveSmileYuvSrcDbgFlag = 1;
	}
	#endif
	
	#if (FD_SAVE_SMILE_II_DBG == 1)
	else if (strncmp((UINT8*)val[0], "iis", 3) == 0) {
		fdSaveSmileIIDbgFlag = 1;
	}
	#endif

	#if 0
	else if (strncmp((UINT8*)val[0], "b", 1) == 0) {//blink
		fdTestPrintf("%lx\n",sp1kFdParamGet(FD_PARAM_IIAddr));
	}
	#endif

	#if 0
	else if ((UINT8)val[0] == 9) {
		fdCdspIdx=0xff;
	}
	#endif

	#if 0
	else if (strncmp((UINT8*)val[0], "l", 1) == 0) {
		switch(val[1]) {
			case 0:
				sp1kNandRsvRead(RES_ID_Face, FD_ADDR_DBFace<<1);
				break;
			case 1:
				fdBlinkEn =0;
				sp1kNandRsvRead(RES_ID_Smile, FD_ADDR_DBSub0<<1);
				sp1kNandRsvRead(RES_ID_NonSmile, FD_ADDR_DBSub1<<1);	
				break;
			case 2:
				fdBlinkEn =1;
				sp1kNandRsvRead(RES_ID_Eye, FD_ADDR_DBSub0<<1);
				sp1kNandRsvRead(RES_ID_CloseEye, FD_ADDR_DBSub1<<1);	
				break;
			case 3:
				fdBlinkEn =1;
				sp1kNandRsvRead(RES_ID_CloseEye, FD_ADDR_DBSub0<<1);
				sp1kNandRsvRead(RES_ID_Eye, FD_ADDR_DBSub1<<1);	
				break;
		}
	}
	#endif

	#if 0
	else if (strncmp((UINT8*)val[0], "d", 1) == 0) {
		switch(val[1]) {
			case 0:
				fdDbgStage0 = val[2];
				break;
			case 1:
				fdDbgStage1 = val[2];
				break;
			case 2:
				fdDbgStage2 = val[2];
				break;
			case 3:
				fdDbgSlow2En = val[2];
				break;
			case 10:
				dramFileSave("FACE", FD_ADDR_DBFace, 20000);
				dramFileSave("SMILE", FD_ADDR_DBSub0, 10000);
				dramFileSave("NON", FD_ADDR_DBSub1, 10000);
				dramFileSave("QUEUE", fdJobAddr, 448);
				dramFileSave("QSUB", fdJobAddrSub, 448);
				dramFileSave("FW", 0, 1024);
				break;
		}
	}
	#endif

	#if 0
	else if (strncmp((UINT8*)val[0], "x", 1) == 0) {
		fdFaceMax=val[1];	
	}
	#endif

	#if 0
	else if (strncmp((UINT8*)val[0], "f", 1) == 0) {
		fdFlow = 0xff;
	}
	#endif

	#if 0
	else if (strncmp((UINT8*)val[0], "s", 1) == 0) {
		fdDbgSaveIdx = val[1];	
	}
	#endif
	
	#if 0
	else if (strncmp((UINT8*)val[0], "m", 1) == 0) {
		sp1kFdEnable();
		rst = sp1kFdParamSet(FD_PARAM_Mode,val[1]);

		if(fdMode==1)
		{
			sp1kNandRsvRead(RES_ID_Smile, FD_ADDR_DBSub0<<1);//smile
			sp1kNandRsvRead(RES_ID_NonSmile, FD_ADDR_DBSub1<<1);//non
		}
		else if(fdMode==2)
		{
			sp1kNandRsvRead(RES_ID_Eye, FD_ADDR_DBSub0<<1);//eye
			sp1kNandRsvRead(RES_ID_CloseEye, FD_ADDR_DBSub1<<1);//close
		}
		else if(fdMode==3)
		{
			HAL_GprmDramFillExt(FD_ADDR_DBSub0<<1, 10000ul, 0, 1, 0);
			sp1kNandRsvRead(RES_ID_Smile, FD_ADDR_DBSub0<<1);//smile
		}
		else if(fdMode==4)
		{
			HAL_GprmDramFillExt(FD_ADDR_DBSub0<<1, 10000ul, 0, 1, 0);
			sp1kNandRsvRead(RES_ID_NonSmile, FD_ADDR_DBSub0<<1);//smile
		}
		else if(fdMode==5)
		{
			HAL_GprmDramFillExt(FD_ADDR_DBSub1<<1, 10000ul, 0, 1, 0);
			sp1kNandRsvRead(RES_ID_Smile, FD_ADDR_DBSub1<<1);//smile
		}
		else if(fdMode==6)
		{
			HAL_GprmDramFillExt(FD_ADDR_DBSub1<<1, 10000ul, 0, 1, 0);
			sp1kNandRsvRead(RES_ID_NonSmile, FD_ADDR_DBSub1<<1);//smile
		}
		else if(fdMode==7)
		{
			HAL_GprmDramFillExt(FD_ADDR_DBSub0<<1, 10000ul, 0, 1, 0);
		}
		else if(fdMode==8)
		{
			HAL_GprmDramFillExt(FD_ADDR_DBSub1<<1, 10000ul, 0, 1, 0);
		}
		else if(fdMode==9)
		{
			HAL_GprmDramFillExt(FD_ADDR_DBSub1<<1, 10000ul, 0xff, 1, 0);
		}
	}
	#endif

	#if 0
	else if (strncmp((UINT8*)val[0], "n", 1) == 0) {	 
		rst = sp1kFdParamSet(FD_PARAM_FaceNumMax,val[1]);
	}
	#endif

	#if 0
	else {
		fdTestPrintf("cdsp:%lx,%bx\n",fdGetCdspBuf(fdCdspIdx,2),fdCdspIdx);		
		fdTestPrintf("size:,%lu,%lu,%lu\n",K_SDRAM_FdIIBufSize,fdDbgSize,FD_SIZE_Queue);	 
		fdTestPrintf("addr:%lx,%lx,%lx,%lx,%lx,%lx,%lx,%lx\n",K_SDRAM_PvFdIIAddr,fdTblAddr,fdSIIAddr,fdIISrcAddr,fdOutAddr,fdJobAddr,fdJobAddrSub
			,K_SDRAM_CodeSentryRedirectAddr);
		
		for(i=0;i<13;i++) {
			fdTestPrintf("%lx\n",fdIIDstAddr[i]);
		}
	}
	#endif

#else 
	pt = pt;
#endif

#else
	pt = pt;
#endif
}

