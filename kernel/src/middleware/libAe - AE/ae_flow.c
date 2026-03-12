/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#include "general.h"
#include "asicreg.h"
#include "initio.h"
#include "main.h"
#include "front_logi.h"
#include "sp1k_rsvblk_api.h"
#include "utility.h"
#include "math.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "reg_def.h"
#include "awb_api.h"
#include "sp1k_ae_api.h"
#include "ae_api.h"
#include "sp1k_fd_api.h" 
#include "hal_dram.h"
#include "ae_def.h"
#include "ae_debug.h"
#include "intr.h"
#include "cdsp_task.h"
#include "hal_front_common.h"
#include "hal_front.h"
#include "icat_stub.h"
#include "pwrlog.h"
#include "hal_moe.h"
#include "hal_fd.h"
#include "os_api.h"
#include "pv_task.h"
#include "hal_gprm.h"
#include "pv_photoframe.h"
#if (DBG_PV_PF_TIMING == 1)
#include "gpio_api.h"
#endif


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* debug. */
#define __FILE	__FILE_ID_LIB_AE_FLOW__

/* FD AE Ctrl */
#define ONE_WT					0	/* Normal is muit weight */
#define AE_TAGET_LUMA_REGION	0	
#define AE_SPEED				0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* AE Windows, II AE */
#define WIN_H_PT				8
#define WIN_V_PT				8
#define WIN_PT					(WIN_H_PT * WIN_V_PT)
	
/* RGB Windows */
#define WIN_H_PT2				5
#define WIN_V_PT2				5
#define WIN_PT2					(WIN_H_PT2 * WIN_V_PT2)
	
/* AE Windows, II AE */
#define AE_TARGET_LEVEL			128
#define AE_CONVERGE_ZONE		7	//5
	
/* 10bit RGB Windows */
#define AE_TARGET_LEVEL_10BIT		60
#define AE_CONVERGE_ZONE_10BIT		25
#define AE_ACCUM_LUMA_MIN_10BIT		8
#define AE_1EV_STEP_10BIT			10
#define AE_JUMP_STEP_10BIT			40
#define AE_UP_THR_10BIT				768
#define AE_LOW_THR_10BIT			32

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/***********************************************
 *	0: Ae Window
 *	1: II AE
 *	2: RGB Window(need used log2 library) 
***********************************************/

UINT8 aeProc = 1;

xdata UINT8 aeCount;	//default value = 0
xdata UINT8 aeGainExpSetEn = 1;
xdata UINT8	G_AEReady;
xdata UINT8	G_GAIN;	//default value = 0
xdata UINT32 G_SHUTTER;	//default value = 0
xdata UINT8	G_AETblSel = 60;	/* Default 60Hz */
xdata UINT8	G_CaptureCompensationGAIN;	//default value = 0
xdata UINT8	G_Negative;	/* default value = 0, mantis bug 0041469 for II AE, AE Window is not used, default value = 0 */
xdata UINT8 G_aeSpeed;	/* default value = SP1K_AE_JUMP_STEP_NORMAL */

UINT8 gAeTgtLvl = AE_TARGET_LEVEL;
UINT8 gAeMinCvgZon = AE_CONVERGE_ZONE;
UINT8 gAeMaxCvgZon = AE_CONVERGE_ZONE;
UINT16 gAeTgtLvl_16 = AE_TARGET_LEVEL_10BIT;
UINT16 gAeMinCvgZon_16 = AE_CONVERGE_ZONE_10BIT;
UINT16 gAeMaxCvgZon_16 = AE_CONVERGE_ZONE_10BIT;

static UINT8 aeCalculateLumaMode = LUMA_NORMAL;

UINT32 normalizationFactor;
UINT32 aeWinSize;

SINT16 gId = 49;

UINT8 g_AE_Stable;
UINT8 ForceAEDo = 1;
UINT8 G_luma;
UINT16 G_lumaU16;
UINT8 aeYMax;
UINT8 aeYMin;

UINT8 Ggain_mode_En;	//default value = 0

//UINT16  g_AE_Contrast_Ratio;
//SINT16   gId_r = 30;
//SINT8     GlbIdx=0; 
//UINT8     GlbFlag=0;
//UINT8   gain_mode = 0;
//SINT16  gain_idx = 0;
//xdata UINT8 awb_gain_update=0;

#if AE_NO_USED
UINT16 GlbGain;
UINT16 ae_globle_gain = 64;
UINT8 base_one = 64;
#endif


static UINT8 aeMoeDebugCnt;	//default value = 0

UINT8 aePass;//default value = 0

/***********************************************
 *							II MOE SETTING 										
 ***********************************************/
UINT16 aeIIYuvSrcSizeMode;	/* II AE used, when pccam 160*120 size ii src need switch 1st path */
UINT32 aeIIAddrSrc;
UINT32 aeIIAddrQueue;
UINT32 aeIIAddrOut;
//UINT32 aeIIAddrOutRes;
UINT32 aeIIAddrTmp;
UINT32 aeMoeAddrQueue;
UINT32 aeMoeAddrWp;
UINT32 aeMoeAddrOut;
UINT32 aeMoeAddrTmp;
UINT32 aeMoeAddrLstWnd;
UINT16 aeWndAbsErr;
UINT8* aeWpPt;
UINT16 aeWpSum,aeWpSum0;
extern osSysMode_t osSysMode;

UINT8 xdata aeMoeOut[16] _at_ 0xbff0;
UINT8 xdata aeMoeWp[100] _at_ 0xbf00;
UINT8 aeMoeNew = 0;
UINT8 aeFdEn = 0;
UINT8 aeFdLoadBack_flag = 0;
UINT8 aeCmdTest = 0;
UINT8 aeSyncStatus = AE_STATUS_PROC_TRIGGER;
UINT8 aeCalPvOB = 0;    //fan.xiao add for ob calibration
UINT8 aeDyTdSwitchByPass;

UINT8 G_FrameIntervalOption = 0;

/***********************************************
 *                          8*8  y AE Weight                                        
 ***********************************************/
code UINT8 AeWt[2][3][WIN_H_PT * WIN_V_PT] = {
	{//AE WIN
		{	
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1
		},
		{ 
			0,0,1,1,1,1,0,0,
			0,1,1,2,2,1,1,0,
			1,1,2,3,3,2,1,1,
			1,2,3,4,4,3,2,1,
			1,2,3,4,4,3,2,1,
			1,1,2,3,3,2,1,1,
			0,1,1,2,2,1,1,0,
			0,0,1,1,1,1,0,0
		},
		{	
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,1,48,48,1,0,0,
			0,0,1,48,48,1,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0
		}
	},
	{//II AE
		{	
			0,1,1,1,1,1,1,0,
			0,1,1,1,1,1,1,0,
			0,1,1,1,1,1,1,0,
			0,1,1,1,1,1,1,0,
			0,1,1,1,1,1,1,0,
			0,1,1,1,1,1,1,0,
			0,1,1,1,1,1,1,0,
			0,1,1,1,1,1,1,0
		},
		{	
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,1,2,3,3,2,1,0,
			0,2,3,4,4,3,2,0,
			0,2,3,4,4,3,2,0,
			0,1,2,3,3,2,1,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0
		},
		{	
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,1,1,0,0,0,
			0,0,1,48,48,1,0,0,
			0,0,1,48,48,1,0,0,
			0,0,0,1,1,0,0,0,
			0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0
		}
	}
};

/***********************************************
 *                          5*5  RGB AE Weight                                        
 ***********************************************/
code UINT8 AeWt2[3][WIN_H_PT2 * WIN_V_PT2] = {
	{	
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1
	},

	{	
		0,1,1,1,0,
		1,2,3,2,1,	
		1,3,4,3,1,
		1,2,3,2,1,
		0,1,1,1,0
	},

	{	
		0,0,0,0,0,
		0,1,1,1,0,
		0,1,48,1,0,
		0,1,1,1,0,
		0,0,0,0,0,
	}
};

/***********************************************
 *                  5*4 Calibration RGB AE Weight                                        
 ***********************************************/
code UINT8 AeCalWt2[2][5 * 4] = {
	{	
		0,0,0,0,0,
		0,0,0,0,0,	
		0,0,0,0,0,
		1,0,0,0,0
	},

	{	
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1,
		1,1,1,1,1
	}
};

/***********************************************
 *                  Global gain table                                        
 ***********************************************/
#if AE_NO_USED
code UINT16 Glb_gain_tbl[41] = {
	64,														//normal
	69,   74,  79,   84,   91,  97,  104, 111, 119, 128,	//add Level 1
	137, 147, 158, 169, 181, 194, 208, 223, 239, 256,		//add Level 2
	274, 294, 315, 338, 362, 388, 416, 446, 478, 512,		//add Level 3
	549, 588, 630, 676, 724, 776, 832, 891, 955, 1024		//add Level 4
};
#endif

/***********************************************
 *                  ae H,V normalizing factor                                        
 ***********************************************/
code UINT16 acctbl[8] = {
	2, 4, 8, 16, 32, 64,128,256
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
//extern xdata raw_pixel_t AWBBuff[1024];
extern SysDynamicEnhan SysDynamicValue;
extern xdata UINT8 G_StdLumi;
extern xdata UINT8 G_ae_max_exp_idx;
extern xdata UINT8 G_ae_min_exp_idx;
extern xdata UINT8 G_ae_max_gain_idx;
extern xdata UINT8 G_ae_min_gain_idx;
extern xdata UINT8 G_ae_min_awbgain_idx;
extern xdata UINT8 G_ae_max_awbgain_idx;
extern xdata UINT8 G_ae_init_complete;
extern xdata UINT8 G_AEReady;
extern xdata UINT8 G_AESetting;
extern xdata UINT8 G_GAIN;
extern xdata UINT32	G_SHUTTER;
extern xdata UINT8 G_AETblSel;
extern AECQ AECmdQue[3];
extern UINT8 aeMeteringMode;
extern UINT8 G_PvDyTDEn;
extern UINT8 G_PvDyTDSwitchClkEn;
extern UINT8 G_PvDyTDSwitchClkEnTmp;
extern UINT8 cdspPvDyTDSwitchFlowStatus;
extern CaptureHeaderAeInformation capHdrInfor;

#if (FD_ENABLE == 1)
extern UINT8 fdKeyRtX;
extern UINT8 fdKeyRtY;
extern UINT8 fdKeyRtS;
extern UINT8  fdFaceNum;
extern UINT8 fdEn;
#endif

/**************************************************************************
 *                 CALL BACK							                  *
 **************************************************************************/
static UINT8* (*paeUserDefineMeter)(UINT8 w, UINT8 h);

/**************************************************************************
 *                 PROTOTYPE							                  *
 **************************************************************************/
#if AE_NO_USED 
/**
 * @fn		  void aeGlobleGainSet(UINT16 gain)
 * @brief
 * @param	  NONE
 * @retval	  NONE
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void
aeGlobleGainSet(
	UINT16 gain
)
{
	if(ae_globle_gain != gain) {	
		awbGlobeGainSet(gain);
	}
	/*
	UINT16 r, g, b;

	if(ae_globle_gain == gain)
		return;

	ae_globle_gain = gain;
	r = (set_rgain * ae_globle_gain) / base_one;
	b = (set_bgain * ae_globle_gain) / base_one;
	r = r>0x7ff? 0x7ff: r;
	b = b>0x7ff? 0x7ff: b;
	g = ae_globle_gain>0x7ff? 0x7ff: ae_globle_gain;
	HAL_SetWBGainSet((UINT16)r,(UINT16)g,(UINT16)b,(UINT16)g);
	halCdspWBGainSet( 1 );
	*/
}
#endif

/**
 * @fn		  UINT8 aeFlowGetCurLuma(void)
 * @brief
 * @param	  NONE
 * @retval	  Current AE Window Luma
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
aeFlowGetCurLuma(
	void
)
{
	return G_luma;
}

/**
 * @fn		  UINT16 aeFlowGetCurLumaU16(void)
 * @brief
 * @param	  NONE
 * @retval	  Current AE Window Luma
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT16
aeFlowGetCurLumaU16(
	void
)
{
	return G_lumaU16;
}

#if AE_NO_USED 
/**
 * @fn		  void aeDefaultProc(void)
 * @brief
 * @param	  NONE
 * @retval	  AE Process
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeDefaultProc(
	void
)
{
}
#endif

/**
 * @fn		  void aeNormalizationFactorSet(void)
 * @brief
 * @param	  NONE
 * @retval	  AE Process
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeNormalizationFactorSet(
	UINT8 winMode
)
{
	if (winMode == AE_WIN_MODE_RGB && aeProc == 2) {
		normalizationFactor = (2*(UINT32) acctbl[XBYTE[REG_Cdsp_AEhaccfactor]&0x07]) * (2*(UINT32) acctbl[(XBYTE[REG_Cdsp_AEhaccfactor]>>4)&0x07]);
	}
	else {
		normalizationFactor = ((UINT32) acctbl[XBYTE[REG_Cdsp_AEhaccfactor]&0x07]) * ((UINT32) acctbl[(XBYTE[REG_Cdsp_AEhaccfactor]>>4)&0x07]);
	}
	aeWinSize = ((UINT32) XBYTE[REG_Cdsp_AEhsize]) * ((UINT32) XBYTE[REG_Cdsp_AEvsize]);
}

/**
 * @fn		  void aeForceSet(void)
 * @brief
 * @param	  NONE
 * @retval	  AE Process
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeForceSet(
	void
)
{
	G_AEReady = 1;
}

/**
 * @fn		  void aeTblSelSet(UINT8 AETblSel)
 * @brief
 * @param	  NONE
 * @retval	  AE Process
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeTblSelSet(
	UINT8 AETblSel
)
{
	G_AETblSel = AETblSel;
	ForceAEDo = 1;
}

#if AE_NO_USED
/**
 * @fn		  void aeFrmInvalidSet(void)
 * @brief
 * @param	  NONE
 * @retval	  AE Process
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeFrmInvalidSet(
	void
)
{
	G_AEReady = 0;
}
#endif

#if AE_NO_USED
/**
 * @fn		  UINT8 aeGetCount(void)
 * @brief
 * @param	  NONE
 * @retval	  AE Process
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
aeGetCount(
	void
)
{
	return aeCount;
}
#endif

/**
 * @brief	ae Set Calculate Luma Mode
 * @param	mode		: LUMA_NORMAL, LUMA_CAL_COLOR_CHECKER, LUMA_CAL_LSC
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
void
aeCalculateLumaModeSet(
	UINT8 mode
)
{
	aeCalculateLumaMode = mode;
}

#if AE_NO_USED
/**
 * @brief	ae Get Calculate Luma Mode
 * @return	LUMA_NORMAL, LUMA_CAL_COLOR_CHECKER, LUMA_CAL_LSC
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
UINT8
aeCalculateLumaModeGet(
	void
)
{
	return aeCalculateLumaMode;
}
#endif

/**
 * @fn		  void aeIIMoeAddrInit(void)
 * @brief          ae II MOE Address initial
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeIIMoeAddrInit(
	void
)
{
	//******************************************************************//
	//	(320*240YUV) -> 160*120 II scale to 64*64 II (aeIIAddrTmp)->8*8 II(aeIIAddrOut)	//
	//	aeMoeAddrLstWnd = 8*8*4 bytes ; 128 word (move detect used )					//
	//	aeIIAddrOut = 8*8*4 bytes ; 128 word (8*8 II used)							//
	//	aeIIAddrQueue = 256 bytes ; 128 word (II queue)								//
	//	aeMoeAddrQueue = 256 bytes ; 128 word (MOE queue)							//
	//	aeMoeAddrWp = 8*8*4 bytes (MOE 2bytes); 128 word							//
	//	aeIIAddrTmp = 64*64*2 bytes (160*120 II scale to 64*64 II temp buffer)			//
	//	aeIIAddrSrc = 320*240*2 bytes (320 * 240 YUV)								//
	//	aeMoeAddrTmp = AE WIN & RGB WIN HW output addr, II AE equal aeIIAddrOut(8*8II)	//
	//******************************************************************//
	
	aeMoeAddrLstWnd = K_SDRAM_DPSStartAddr + (13*512ul) - 20 - 128;//8
	aeIIAddrOut = K_SDRAM_DPSStartAddr + (13*512ul);//8
	//aeIIAddrOutRes = aeIIAddrOut-20;
	HAL_GprmDramFillExt((aeIIAddrOut-20)<<1, 40, 0, 0, 0);
	aeIIAddrQueue = aeIIAddrOut + 128;//8
	aeMoeAddrQueue=aeIIAddrQueue + 128;	//8
	aeMoeAddrWp=K_SDRAM_CodeSize+(((UINT16)aeMoeWp -0x4000)>>1);//aeMoeAddrQueue + 128;
	aeMoeAddrOut = K_SDRAM_CodeSize+(((UINT16)aeMoeOut -0x4000)>>1);//aeMoeAddrQueue+128;
	
	
	aeIIAddrTmp = K_SDRAM_PvAeAddr;	
	aeMoeAddrTmp = K_SDRAM_PvAeWndAddr;
	aeIIAddrSrc = K_SDRAM_Pv2BufAddrB;//fdGetCdspBuf(0,2);
	//aeIIAddrSrc = fdGetCdspBuf(2,2);

	if (osSysMode!=OS_SYS_MODE_STILL_PV) {
		aeIIAddrTmp = K_SDRAM_AviAeAddr;
		aeMoeAddrTmp = K_SDRAM_AviAeWndAddr;

		switch (aeIIYuvSrcSizeMode) {

			case AE_PCCAM_320_240:
				aeIIAddrSrc = K_SDRAM_Avi2PvwBufAddrB;
				break;
			case AE_PCCAM_160_120:
				aeIIAddrSrc = K_SDRAM_AviPvwBufAddrB;
				break;
			default:
				ASSERT(0,1);
				break;
		}
	}

	if (aeProc==1) {
		aeMoeAddrTmp = aeIIAddrTmp;
	}
}

/**
 * @fn		  void aeCreateIIQueue(void)
 * @brief          ae Create II Queue
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeCreateIIQueue(
	void
)
{
	UINT8 pthsize[] = {8};
	UINT8 ptvsize[] = {8};
	UINT16 pvhsize,pvvsize;
	UINT8 aeIIResX = 64;
	UINT8 aeIIResY = 64;
	UINT32 addrs[2];	
	UINT8 buf[sizeof(fd_iijobtable_t)*3];
	UINT16 aeIIYuvSrcHSize;
	fd_iijobtable_t xdata* pt = (fd_iijobtable_t xdata*)buf;

	pvSizeGet(&pvhsize,&pvvsize);
	
	aeCreateIIQueuePrintf("ae II:%u,%u\n",pvhsize,pvvsize);

	switch (aeIIYuvSrcSizeMode) {
		case AE_PCCAM_320_240:
			aeIIYuvSrcHSize = 320;
			break;
		case AE_PCCAM_160_120:
			aeIIYuvSrcHSize = 160;
			break;
		default:
			ASSERT(0,0);
			break;
	}
	
	HAL_FdIISIIQueue (				
		aeIIYuvSrcHSize, //UINT16 yuvsrcvsize, 
		0,  0,	aeIIYuvSrcHSize,  (UINT32)aeIIYuvSrcHSize*pvvsize/pvhsize, 
		aeIIResX,  aeIIResY, 
		aeIIAddrSrc,  aeIIAddrTmp,  0,//siisrcaddr,
		(UINT8*)pt++,
		0
	);

	addrs[0] = aeIIAddrOut;
	HAL_FdIIQueue (			
		aeIIAddrTmp,	//(w)
		aeIIResX,  aeIIResY, 
		pthsize,  ptvsize, 
		addrs, //(w)
		(UINT8*)pt++,
		0, //(w)
		1
	);	
	
	HAL_DramWriteBytesAddr(aeIIAddrQueue, buf, sizeof(fd_iijobtable_t)*3, 0);
	
	//dramFileSave("II", aeIIAddrQueue, 32*3);
	
	XBYTE[REG_II_IntrEnable] = 0x01;//intr
	XBYTE[REG_II_ModuleControl] = 0x00;//enable
}

/**
 * @fn		  void aeCreateMoeQueue(void)
 * @brief          ae Create MOE Queue
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeCreateMoeQueue(
	void
)
{	
	UINT8 buf[256];
	//UINT8 xdata* buf = G_ucStorData;
	moe_queue_t xdata* queue = (moe_queue_t xdata*)buf;
	moe_cmd_t cmd;// = new moe_cmd_t();
	UINT16 size;	
	UINT8 idx;
	UINT8 winLength = 8*8;
	UINT8 bitn = 0;
	
	if (aeProc == 2) {
		bitn = 1;
		winLength = 5*5*4;
	}
	else if (aeProc == 1) {	
		bitn = 1;
		/*
		HAL_MoeDefCmd(&cmd);
		cmd.equation = MOE_EQUATION_Min;
		cmd.func = MOE_FUNC_PointWithCommon;
		cmd.ROIWidth = 40;
		cmd.min = 0;
		cmd.max = 0;
		cmd.src1 = aeIIAddrOut;
		cmd.cp = 0;
		cmd.dst = aeIIAddrOut-20;//[6];
		HAL_MoeConvertToQueue(&cmd, queue++);
		*/
		HAL_MoeDefCmd(&cmd);
		cmd.equation = MOE_EQUATION_Add;
		cmd.ROIWidth = 8*8*2;
		cmd.min = 0;
		//cmd.max = 1;
		cmd.src1 = aeIIAddrOut;
		cmd.src1Bit = 1;
		cmd.src2= aeIIAddrOut-18;
		cmd.src2Bit = 1;
		cmd.dst = aeMoeAddrTmp;//[6];
		cmd.dstSign = 1;
		cmd.dstBit = 1;
		HAL_MoeConvertToQueue(&cmd, queue++);
		
		cmd.equation = MOE_EQUATION_Subtract;
		cmd.src1 = aeMoeAddrTmp;
		cmd.src2= aeIIAddrOut-16;
		cmd.dst = aeMoeAddrTmp;//[6];
		HAL_MoeConvertToQueue(&cmd, queue++); 
		
		cmd.src1 = aeMoeAddrTmp;
		cmd.src2= aeIIAddrOut-2;
		cmd.dst = aeMoeAddrTmp;//[6];
		HAL_MoeConvertToQueue(&cmd, queue++); 

		cmd.func = MOE_FUNC_PointWithCommon;
		cmd.equation = MOE_EQUATION_Max;
		cmd.dstBit = 0;
		cmd.src1 = aeMoeAddrTmp;
		cmd.cp= 0;
		cmd.dst = aeMoeAddrTmp;//[6];
		HAL_MoeConvertToQueue(&cmd, queue++); 
	}

	HAL_MoeDefCmd(&cmd);
	cmd.ROIWidth = winLength;
	cmd.equation = MOE_EQUATION_Summation;
	cmd.func = MOE_FUNC_PointWithPoint;	
	cmd.sum = 0;
	cmd.max=0xffffffff;	
	cmd.src1Bit = bitn;	
	cmd.src2Bit = 0;
	cmd.dstBit = 1;
	cmd.src1Shift = 4;
	cmd.src2Shift = 4;
	cmd.src1= aeMoeAddrTmp;
	cmd.src2 = aeMoeAddrWp;//addr+128;	
	cmd.dst = aeMoeAddrOut;//[6];
	//cmd.CmdQueueEnd = 1;
	HAL_MoeConvertToQueue(&cmd, queue++);

	/* move dected */
	#if AE_MOVE_DETECT_EN
	HAL_MoeDefCmd(&cmd);//dy=abs(y-lstY)
	cmd.ROIWidth = 8*8;
	cmd.equation = MOE_EQUATION_Absolute;
	cmd.func = MOE_FUNC_PointWithPoint;
	cmd.src1Bit = bitn;
	cmd.src1= aeMoeAddrTmp;
	cmd.src2 = aeMoeAddrLstWnd;
	cmd.dst = aeMoeAddrLstWnd;//[6];	
	HAL_MoeConvertToQueue(&cmd, queue++);

	HAL_MoeDefCmd(&cmd);//sum(dy)
	cmd.ROIWidth = 8*8;
	cmd.equation = MOE_EQUATION_Summation;
	cmd.func = MOE_FUNC_PointWithCommon;	
	//cmd.sum = 0;
	//cmd.min = 0;
	//cmd.max=0xffffffff;
	//cmd.src1Bit = 0;
	//cmd.src2Bit = 0;
	//cmd.dstBit = 1;
	//cmd.src1Shift = 0;
	//cmd.src2Shift = 0;
	cmd.src1 = aeMoeAddrLstWnd;
	cmd.cp = 1;	
	cmd.dst = aeMoeAddrOut+4;//[6];
	//cmd.CmdQueueEnd = 1;
	HAL_MoeConvertToQueue(&cmd, queue++);

	//HAL_MoeDefCmd(&cmd);//copy y to lsty
	//cmd.ROIWidth = 8*8;
	cmd.equation = MOE_EQUATION_Max;
	//cmd.func = MOE_FUNC_PointWithPoint;
	cmd.src1= aeMoeAddrTmp;
	cmd.cp = 0;
	cmd.dst = aeMoeAddrLstWnd;//[6];	
	HAL_MoeConvertToQueue(&cmd, queue++);
	#endif
	
	size = (UINT16)((UINT8 xdata*)queue) - (UINT16)buf;	
	idx = size/sizeof(moe_queue_t)-1;
	
	aeCreateMoeQueuePrintf("\naeCreateMoeQueue %u,%bu,%x,%x\n",size,idx,(UINT16)(UINT8 xdata*)queue,(UINT16)buf);
	
	HAL_MoeSetQueueEnd((moe_queue_t *) buf, idx);
	HAL_DramWriteBytesAddr(aeMoeAddrQueue, buf,256, 0);
	
	aeCreateMoeQueuePrintf("aeCreateMoeQueue Idx:%bx\n",idx);
	
	//dramFileSave("MOE", aeMoeAddrQueue, (UINT16)((UINT8*)queue) - (UINT16)buf);

}

/**
 * @fn		  void aeIITrig(void)
 * @brief          Trig II
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeIITrig(
	void
)
{
	if (!G_AEEN) {
		return;
	}

	if (aeCalculateLumaMode != LUMA_NORMAL) {
		return;
	}
	
	if (aeProc != 1) {
		aeMoeTrigApi();
		return;
	}
	
	if (XBYTE[REG_II_Status] & 0x01) {
		UINT32 queaddr = aeIIAddrQueue;
		XBYTE[REG_II_CmdQueueAddr0] = ((unsigned char *)&(queaddr))[3];
		XBYTE[REG_II_CmdQueueAddr1] = ((unsigned char *)&(queaddr))[2];
		XBYTE[REG_II_CmdQueueAddr2] = ((unsigned char *)&(queaddr))[1];
		XBYTE[REG_II_CmdQueueAddr3] = ((unsigned char *)&(queaddr))[0];
		XBYTE[REG_II_CmdSrc] = 1; 	  //Queue Trigger mode	
		XBYTE[REG_II_Intr] = 0x03;	//Clear done interrupt
		XBYTE[REG_II_CmdTrigger] = 0x01; 	//Trigger
		iiDbgTrig = II_Trigger_Ae;
		aeIntrPutChar('I');
	}
}

/**
 * @fn		  void aeFdAeModeSet(UINT8 enable)
 * @brief          FD AE wp setting flow
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void
aeFdAeModeSet(
	UINT8 enable
)
{
	aeFdEn = enable;

	if (aeFdEn == 0) {
		aeFdLoadBack_flag = 1;
	}
}

#if (FD_ENABLE == 1)
/**
 * @fn		  void aeMoeFdWp(void)
 * @brief          FD AE wp setting flow
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeMoeFdWp(
	void
)
{
	static UINT8 centerLstIdx = 0xff;
	static UINT8 loadBackFlag = 0;
	static UINT8 loadBackCount = 0x0;
	static UINT8 diffCnt1 = 0;
	static UINT8 savefdKeyRtS = 0xff;
	UINT8 RtS_Diff_Flag = 0;
	UINT8 r,x,y,x0,x1,y0,y1,idx;
	UINT8 centerIdx = 0xff;

	if (aeFdEn && fdFaceNum && fdKeyRtS) {	
		r = fdKeyRtS >>1;
		x = fdKeyRtX + r;
		y = fdKeyRtY + r;		
		centerIdx = (x/20) + (y/15) * 8;

		/* center not diff and Rts cheng */
		if (fdKeyRtS > savefdKeyRtS) {
			if ((fdKeyRtS - savefdKeyRtS)>5) {
				RtS_Diff_Flag = 1;
			}
			else {
				RtS_Diff_Flag = 0;
			}
		}
		else {
			if ((savefdKeyRtS - fdKeyRtS)>5) {
				RtS_Diff_Flag = 1;
			}
			else {
				RtS_Diff_Flag = 0;
			}
		}
		
		aeMoeFdWpPrintf("in centerIdx=%bx,centerLstIdx=%bx,aeWpSum=%d\n",centerIdx,centerLstIdx,aeWpSum);

		/* center point diff and Rts diff need to set new wp */
		if (centerIdx != centerLstIdx || RtS_Diff_Flag == 1) {
			diffCnt1++;
			
			aeMoeFdWpPrintf(">");
			
			if (diffCnt1 > 1) {

				savefdKeyRtS = fdKeyRtS;

				#if AE_TAGET_LUMA_REGION
				gAeMinCvgZon = AE_CONVERGE_ZONE*3;
				gAeMaxCvgZon = AE_CONVERGE_ZONE*3;
				#endif

				#if AE_SPEED
				G_aeSpeed = 1;
				#endif
				
				aeMoeFdWpPrintf("In\n");
				
				#if ONE_WT
				if (x<20) {
					x0 = 1; /* II AE x0=0 or x1=7 not use */
				}
				else {
					x0 = x/20;
				}
				y0 = y/15;
				#else
				if (fdKeyRtX<20) {
					x0 = 1; /* II AE x0=0 or x1=7 not use */
				}
				else {	
					x0 = (fdKeyRtX)/20;
				}
				y0 = (fdKeyRtY)/15;

				if (fdKeyRtX+fdKeyRtS >=140) {
					x1 = 6; /* II AE x0=0 or x1=7 not use */
				}
				else {
					x1 = (fdKeyRtX+fdKeyRtS)/20;
				}
				y1 = (fdKeyRtY+fdKeyRtS)/15;
				#endif

				/* ae wp = zero and wpsum = zero */
				memset(aeMoeWp,0,64);
				aeWpSum = 0;

				/* set ae wp (face region) */
				#if ONE_WT
				idx = y0<<3;
				aeMoeWp[idx+x0]+=1;
				aeWpSum+=1;
				#else
				aeMoeFdWpPrintf("x0=%bu,x1=%bu,y0=%bu,y1=%bu\n",x0,x1,y0,y1);
				
				for (y=y0;y<=y1;y++)	{
					idx = y<<3;
					for (x=x0;x<=x1;x++)	{
						aeMoeWp[idx+x]+=1;
						aeWpSum+=1;
					}
				}
				#endif

				#if 0
				for (y=0;y<64;y++) {
					if (y%8==0) {
						aeMoeFdWpPrintf("\n");
					}
					aeMoeFdWpPrintf("%bu,",aeMoeWp[y]);
				}
				printf("\n");
				#endif
						
				centerLstIdx = centerIdx;				
				loadBackFlag = 1;				
				diffCnt1 = 0;
			}

			loadBackCount = 0;
		}
		else {//(centerIdx == centerLstIdx)
			diffCnt1 = 0;
			aeMoeFdWpPrintf("-");
		}
		loadBackCount = 0;
	}
	else {

		if (loadBackFlag == 1 || aeFdLoadBack_flag == 1) {
			loadBackCount++;
			
			aeMoeFdWpPrintf("*");
			
			if (aeFdLoadBack_flag == 1 || loadBackCount > 5) {
				memcpy(aeMoeWp,aeWpPt,64);
				aeWpSum = aeWpSum0;
				
				aeMoeFdWpPrintf("back\n");
				
				#if 0
				for (y=0;y<64;y++) {
					if (y%8==0) {
						aeMoeFdWpPrintf("\n");
					}
					aeMoeFdWpPrintf("%bu,",aeMoeWp[y]);
				}
				aeMoeFdWpPrintf("\n");
				#endif

				loadBackFlag = 0;
				loadBackCount = 0;
				diffCnt1 = 0;
				centerLstIdx = 0xff;
				savefdKeyRtS = 0xff;
				aeFdLoadBack_flag = 0;

				#if AE_TAGET_LUMA_REGION
				gAeMinCvgZon = AE_CONVERGE_ZONE;
				gAeMaxCvgZon = AE_CONVERGE_ZONE;
				#endif

				#if AE_SPEED
				G_aeSpeed = 0;
				#endif
			}
		}
	}
}
#endif

/**
 * @fn		  void aeMoeTrig(void)
 * @brief          Trig MOE
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeMoeTrig(
	void
)
{	
	if(!G_AEEN){ 
		return;
	}

	if (aeCalculateLumaMode != LUMA_NORMAL) {
		return;
	}

	if (aeDyTdSwitchByPass == 1) {
		aeDyTdSwitchByPass = 0;
		return;
	}
	
	aeIntrPutChar('M');	

	#if (FD_ENABLE == 1)
	if (fdEn) {
		aeMoeFdWp();
	}
	#endif

	if (aePass == 0 || (aeProc == 1) || ((aeProc == 2) && XBYTE[REG_Cdsp_evnt1] & 0x04) || ((aeProc == 0) && XBYTE[REG_Cdsp_evnt1] & 0x04)) {	
		#if (DBG_PV_PF_TIMING == 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
		#endif
		aeMoeNew = HAL_MoeTriggerQueueIntr(aeMoeAddrQueue,MOE_Trigger_Ae);

		XBYTE[REG_Cdsp_evnt1] = 0x04;
		#if (DBG_PV_PF_TIMING == 1)
		gpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);
		#endif
	}
	else {
		aeMoeNew = 0;
	}
	
}

/**
 * @fn		  void aeInit(void)
 * @brief          
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeInit(
	UINT8 proc
)
{
	aeSysInfo("ae init: %bx\n",proc);
	
	aeProc = proc ;

	/* moe reset */
	HAL_MoeReset();
	
	if (aeProc == 1) {
		HAL_FdEnable(0, 0);		
	}

	/* II AE, AE Win, RGB AE address initial */
	aeIIMoeAddrInit();

	/* II AE queue create */
	if (aeProc == 1) {
		aeCreateIIQueue();
	}

	/* II AE, AE Win, RGB AE moe queue create */
	aeCreateMoeQueue();
	
	if (aeProc == 1) {
		HAL_FdEnable(1, 0);		
	}
	
	HAL_MoeEnable();

	//aeMeterChangeSet(0xff);
}

/**
 * @fn		  void aeIntrProc(void)
 * @brief          ae procress flow
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeIntrProc(
	void
)
{
	UINT8 luma;
	UINT16 luma_16;
	UINT16 ywsum = aeWpSum;
	UINT32 ysum,avy,wdiv;
	UINT16 ymax,ymin;
	UINT16 wndErr=0;
	SINT32 log2X, log2Y, ret, round, p, d; 
	SINT16 val;
	UINT8 sign;
	UINT8 tdLimitGainIdx, idxCvgZon;
	frontPrevCapabDesc_t *pcap;

	/* check ae en */
	if ( !G_AEEN ) {
		aeIntrPutChar('^');
		return;
	}
	
	/* switch normal, cal-lsc, cal-colorchecker flow  */
	if (aeCalculateLumaMode == LUMA_NORMAL) {
		
		/* moe debug */
		aeIntrDebug();

		/* check moe update */
		if ( !aeMoeNew ) {			
			aeIntrPutChar('-');	
			aeIntrPutChar('\n');	
			return;
		}
		
		/* get moe output information */
		WRITE8(ysum, 3, aeMoeOut[0]);
		WRITE8(ysum, 2, aeMoeOut[1]);
		WRITE8(ysum, 1, aeMoeOut[2]);
		WRITE8(ysum, 0, aeMoeOut[3]);
		WRITE8(ymin, 1, aeMoeOut[4]);
		WRITE8(ymin, 0, aeMoeOut[5]);
		WRITE8(ymax, 1, aeMoeOut[6]);
		WRITE8(ymax, 0, aeMoeOut[7]);
		//ymax = aeMoeOut[4];
		//ymin = aeMoeOut[6];
		//WRITE8(ywsum, 1, aeMoeOut[8]);
		//WRITE8(ywsum, 0, aeMoeOut[9]);
		WRITE8(wndErr, 1, aeMoeOut[8]);
		WRITE8(wndErr, 0, aeMoeOut[9]);
		aeWndAbsErr = wndErr;

		#if 0
		aeIntrPutUint16(aeWndAbsErr);
		#endif

		aeMoeNew = 0;

		/* calculate luma */
		switch (aeProc) {
			case 0:	/* AE Win */
				AEWINaeIntrPutChar('$');
				avy = ysum * normalizationFactor;
				wdiv = (UINT32)ywsum * aeWinSize;
				luma = (UINT8)(avy/wdiv);
				AEWINaeIntrPutUint8(luma);
				AEWINaeIntrPutChar('\n');
				break;
			case 1:	/* II AE */
				IIaeIntrPutChar('#');
				luma = (UINT8)(ysum/ywsum);
				IIaeIntrPutUint8(luma);
				IIaeIntrPutChar('\n');
				break;
			case 2:	/* RGB AE */
				RGBaeIntrPutChar('!');
				ysum = ysum / 10;
				avy = ysum * normalizationFactor;
				wdiv = (UINT32)(ywsum/10) * aeWinSize;
				luma_16 = (UINT32)(avy/wdiv);
				RGBaeIntrPutUint16(luma_16);
				RGBaeIntrPutChar('\n');
				break;
			default:
				/* not this case */
				INTR_ASSERT(0, 1); 
				break;
		}

		/* pre ae information */
		capHdrInfor.ae.luma[0] = G_luma;
		capHdrInfor.ae.gId[0] = gId;
		capHdrInfor.ae.shutter[0] = G_SHUTTER;
		capHdrInfor.ae.gain[0] = G_GAIN;

		/* set G_luma, aeYMax, aeYMin */
		switch (aeProc){
			case 0:
			case 1:
				G_luma = luma;
				//g_AE_Contrast_Ratio = ymax;//-ymin;
				aeYMax = ymax;
				aeYMin = ymin;
				//g_AE_Contrast_Ratio =(ymax-luma)*2;
				break;
			case 2:
				G_lumaU16 = luma_16;
				G_luma = luma = (UINT8)(luma_16 >> 2);	/* 10bit target luma to 8bit target luma */
				//g_AE_Contrast_Ratio = ymax;//-ymin;
				aeYMax = (ymax >> 2);
				aeYMin = (ymin >> 2);
				//g_AE_Contrast_Ratio =(ymax-luma)*2;
				break;
			default:
				/* not this case */
				INTR_ASSERT(0, 1); 
				break;
		}	

		MAXaeIntrPutUint16(ymax);
		MAXaeIntrPutChar(',');
		MAXaeIntrPutUint16(ymin);
		MAXaeIntrPutChar(',');
		MAXaeIntrPutUint32(ysum);
		MAXaeIntrPutChar(',');
		MAXaeIntrPutUint16(ywsum);
		MAXaeIntrPutChar('\n');
		
	}
	else{
		luma = aeCalibrationCurLumaGet();		
		G_luma = luma;
		CALaeIntrPutUint8(luma);
		CALaeIntrPutChar('\n');
	}

	/* current ae information */
	capHdrInfor.ae.luma[1] = G_luma;
	
	aeCount ++;

	#if (II_AE_MOTION_DET_EN == 0)

	if (aeProc == 2) {
		GIDaeIntrPutUint16(luma_16);
	}
	else {
		GIDaeIntrPutUint8(luma);
	}
	
	GIDaeIntrPutChar(',');
	GIDaeIntrPutUint16((UINT16)gId);
	GIDaeIntrPutChar('-');
	GIDaeIntrPutChar('>');

	if (aeCmdTest == 0) {

		/* ae step algo. */
		switch (aeProc) {
			case 0:	//AE Win
			case 1:	//II AE
				if (ForceAEDo == 0) {
					if ((luma>=(gAeTgtLvl-gAeMinCvgZon))&&(luma<=(gAeTgtLvl+gAeMaxCvgZon))) {
						g_AE_Stable = 1;
						
						/* current ae information */
						capHdrInfor.ae.gId[1] = gId;
						capHdrInfor.ae.stableCondition = g_AE_Stable;
						capHdrInfor.ae.shutter[1] = G_SHUTTER;
						capHdrInfor.ae.gain[1] = G_GAIN;
						
						GIDaeIntrPutChar('s');
						GIDaeIntrPutChar('\n');		
						return;
					}
					else {		
						g_AE_Stable = 0;
						aeGidStepSet(luma);	
					}
				}
				break;
			case 2:	//RGB Win AE
				if (ForceAEDo == 0) {
					if ((luma_16>=(gAeTgtLvl_16-gAeMinCvgZon_16))&&(luma_16<=(gAeTgtLvl_16+gAeMaxCvgZon_16))) {
						g_AE_Stable = 1;

						/* current ae information */
						capHdrInfor.ae.gId[1] = gId;
						capHdrInfor.ae.stableCondition = g_AE_Stable;
						capHdrInfor.ae.shutter[1] = G_SHUTTER;
						capHdrInfor.ae.gain[1] = G_GAIN;
						
						GIDaeIntrPutChar('s');
						GIDaeIntrPutChar('\n');
						return;
					}
					else if (luma_16 < AE_ACCUM_LUMA_MIN_10BIT) {
						gId = G_ae_min_exp_idx;
					}
					else if (luma_16 < AE_LOW_THR_10BIT) {
						gId = gId - AE_JUMP_STEP_10BIT;
					}
					else if (luma_16 > AE_UP_THR_10BIT) {
						gId = gId + AE_JUMP_STEP_10BIT;
					}
					else {
						ret = log2xyND((SINT32)luma_16, (SINT32)gAeTgtLvl_16, &log2X, &log2Y);
						
						if (ret == LOG2_DOMAIN_ERR) {
							gId = G_ae_min_exp_idx;
						}
						else {
							/* ret == 0 */
							p = log2X * AE_1EV_STEP_10BIT;
							d = log2Y;
							sign = p >> 31;
							round = (sign ^ (SINT32)((UINT32)d / 2)) - sign;
							val = (SINT16)((p + round) / d);

							if (val == 0) {
								g_AE_Stable = 1;

								/* current ae information */
								capHdrInfor.ae.gId[1] = gId;
								capHdrInfor.ae.stableCondition = g_AE_Stable;
								capHdrInfor.ae.shutter[1] = G_SHUTTER;
								capHdrInfor.ae.gain[1] = G_GAIN;
								
								GIDaeIntrPutChar('s');
								GIDaeIntrPutChar('\n');
								return;
							}
							else {
								gId = gId + val;
							}
						}
					}
				}
				break;
			default:
				/* not this case */
				ASSERT(1, 0); 
				break;
		}
	}

	/* check ae table min exposure index */
	if(gId<G_ae_min_exp_idx) {
		gId = G_ae_min_exp_idx;
	}

	/* check ae table max exposure index */
	if(gId>G_ae_max_exp_idx) {
		gId = G_ae_max_exp_idx;
	}

	/* current ae information */
	capHdrInfor.ae.gId[1] = gId;
	capHdrInfor.ae.stableCondition = g_AE_Stable;
	
	GIDaeIntrPutUint16((UINT16)gId);
	GIDaeIntrPutChar('\n');

	/* get ae table information */
	if(!aeCalPvOB){
		frontIntrCapabilityGet(0, &pcap, NULL);
		switch (G_AETblSel) {
			case SP1K_FLICKER_60:
				G_GAIN = pcap->ae60hzTbl[gId].gain;
				G_SHUTTER = pcap->ae60hzTbl[gId].shutter;
				G_CaptureCompensationGAIN =  pcap->ae60hzTbl[gId].captureCompensationGainIdx;
				break;
			case SP1K_FLICKER_50:
				G_GAIN = pcap->ae50hzTbl[gId].gain;
				G_SHUTTER = pcap->ae50hzTbl[gId].shutter;
				G_CaptureCompensationGAIN = pcap->ae50hzTbl[gId].captureCompensationGainIdx;
				break;
			default:
				/* not this case */
				INTR_ASSERT(0, 1); 
				break;
		}
	}
	else{
		G_CaptureCompensationGAIN = 0;	
		return;  		/*Don't change G_AEReady*/	
	}

	/* current ae information */
	capHdrInfor.ae.shutter[1] = G_SHUTTER;
	capHdrInfor.ae.gain[1] = G_GAIN;
	
	INFaeIntrPutUint16((UINT16)gId);
	INFaeIntrPutChar(',');
	INFaeIntrPutUint8(G_GAIN);
	INFaeIntrPutChar(',');
	INFaeIntrPutUint32(G_SHUTTER);
	INFaeIntrPutChar(',');
	INFaeIntrPutUint8(G_CaptureCompensationGAIN);
	INFaeIntrPutChar(',');
	
	/* preview dynamic enhan */
	if (aeCmdTest == 0 && ForceAEDo == 0 && G_AEReady == AE_NOT_READY) {
		cdspPvDynamicEnhanFlow();
	}

	/* preview dynamic tempoal denoise */
	if (aeCmdTest == 0 && ForceAEDo == 0 && G_AEReady == AE_NOT_READY) {
		if (cdspPvDyTDSwitchFlowStatus == 0) {
			if (G_PvDyTDEn == 1) {
				
				tdLimitGainIdx = (UINT8)pcap->prevDynamicTDTbl->enTDGainIdx;

				if (G_PvDyTDSwitchClkEnTmp == FRONT_PV_DYNAMIC_TD_ENABLE) {
					idxCvgZon = (UINT8)pcap->prevDynamicTDTbl->idxCvgZon;
				}
				else {
					idxCvgZon = 0;
				}

				if (G_GAIN > (tdLimitGainIdx - idxCvgZon)) {
					G_PvDyTDSwitchClkEn = FRONT_PV_DYNAMIC_TD_ENABLE;
				}
				else {
					G_PvDyTDSwitchClkEn = FRONT_PV_DYNAMIC_TD_DISABLE;
				}
			}
			else {
				G_PvDyTDSwitchClkEn = FRONT_PV_DYNAMIC_TD_DISABLE;
			}

			if (G_PvDyTDSwitchClkEnTmp != G_PvDyTDSwitchClkEn) {
				aeDyTdSwitchByPass = 0;//disable bypass ae flow
			}
		}
	}
	
	ForceAEDo = 0;
	G_AEReady = AE_READY;
	#endif
}

/**
 * @fn		  void aeDynamicMeterChangeSet(void)
 * @brief          Dynamic Meter Change set when ae procress flow running
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeDynamicMeterChangeSet(
	void
)
{
	//UINT8 sts;
	if (aeMeteringMode == SP1K_AE_METER_USER_DEFINE) {
		aePass = 1;
		//while (aeSyncStatus != AE_STATUS_PROC_TRIGGER);
		//ENTER_CRITICAL(sts);
		aeMeterChangeSet(SP1K_AE_METER_USER_DEFINE);
		//EXIT_CRITICAL(sts);
		aePass = 0;
	}
}

/**
 * @fn		  void aeWindowArrayGet(void)
 * @brief          get current ae window luma array
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeWindowArrayGet(
	UINT8 *pArray
)
{
	UINT8 i, msb, lsb, *p8;
	UINT32 iiArray[64];
	
	switch (aeProc) {
		case 0:	
			M_DRAM_SetStartAddress(aeMoeAddrTmp, 1);
			for ( i=0 ; i< WIN_H_PT*WIN_V_PT ;i+=2 ) {
				M_DRAM_ReadWord(lsb, msb);
				pArray[i] = (UINT8)(((UINT32)lsb * normalizationFactor)/aeWinSize);
				pArray[i+1] = (UINT8)(((UINT32)msb * normalizationFactor)/aeWinSize);
			}				
			break;
		case 1:
			M_DRAM_SetStartAddress(aeIIAddrOut, 1);
			for ( i=0 ; i< WIN_H_PT*WIN_V_PT ;i+=1 ) {
				p8 = (UINT8*)&iiArray[i];
				M_DRAM_ReadWord(*(p8+3), *(p8+2));//LL,LH
				M_DRAM_ReadWord(*(p8+1), *(p8+0));//HL,HH
				//printf("%lu\n",iiArray[i]);
			}

			for ( i=0 ; i< WIN_H_PT*WIN_V_PT ;i+=1 ) {
				if (i == 0) {
					pArray[i] = (UINT8)(iiArray[0]);
				}
				else if (i > 0 && i < 8) {
					pArray[i] = (UINT8)(iiArray[i]-iiArray[i-1]);
				}
				else if (i >= 8 && ((i%8) == 0)) {
					pArray[i] = (UINT8)(iiArray[i]-iiArray[i-8]);
				}
				else {
					pArray[i] = (UINT8)(iiArray[i] + iiArray[i-9] - iiArray[i-1] - iiArray[i-8]);
				}
			}
			break;
		case 2:
			/* not this case */
			ASSERT(0, 1); 
			break;
		default:
			/* not this case */
			ASSERT(0, 1); 
			break;
	}
}

/**
 * @fn		  void aeWindowDimensionGet(void)
 * @brief          get ae window array dimension h size, v size
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeWindowDimensionGet(
	UINT8 *h,
	UINT8 *v
)
{
	switch (aeProc) {
		case 0:	
			*h = WIN_H_PT;
			*v = WIN_V_PT;
			break;
		case 1:
			*h = WIN_H_PT;
			*v = WIN_V_PT;
			break;
		case 2:
			*h = WIN_H_PT2;
			*v = WIN_V_PT2;
			break;
		default:
			/* not this case */
			ASSERT(0, 1); 
			break;
	}
}

/**
 * @fn		  void aeMeterChangeSet(void)
 * @brief          ae procress flow
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8 
aeMeterChangeSet(
	UINT8 mode
)
{
	UINT8 *pt;
	UINT8 i = 64;
	UINT16 wps=0;
	UINT8 *pwp = (aeProc <= 1) ? AeWt[aeProc][0] : AeWt2[0];
	UINT8 wwp = (aeProc <= 1) ? WIN_PT : WIN_PT2;

	if (aePass == 0) {
		aeSysInfo("ae meter:%bx",aeProc);
	}

	switch (mode) {
		case SP1K_AE_METER_MULTI:
			pt = pwp;
			aeSysInfo("M\n");
			break;
		case SP1K_AE_METER_CENTER:
			pt = pwp + wwp;
			aeSysInfo("C\n");
			break;
		case SP1K_AE_METER_SPOT:
			pt = pwp + wwp + wwp;
			aeSysInfo("S\n");
			break;
		case SP1K_AE_METER_USER_DEFINE:
			if (aePass == 0) {
				aeSysInfo("U\n");
			}
			if (paeUserDefineMeter != NULL ) {
				if (aeProc <= 1) {
					pt = (*paeUserDefineMeter)(WIN_H_PT,WIN_V_PT);
				}
				else {
					pt = (*paeUserDefineMeter)(WIN_H_PT2,WIN_V_PT2);
				}

				if (pt == NULL) {
					/* no weight, user metering array too small */
					ASSERT(0, 0); 
				}
			}
			else {
				/* no weight */
				ASSERT(0, 0); 
			}
			break;
		default:
			/* not this case */
			ASSERT(0, 1); 
			return mode;
			break;
	}
	
	aeMeteringMode = mode;
	aeWpPt = pt;

	switch (aeProc) {
		case 0:
			for (i = 0; i < 64; i++) {
				aeMoeWp[i] = pt[i];
				wps += pt[i];
			}
			break;
		case 1:
			for (i = 0; i < 64; i++) {
				if (((i % 8 == 0) || (i % 8 == 7)) && (pt[i] != 0)) {
					aeMoeWp[i] = 0;
				}
				else {
					aeMoeWp[i] = pt[i];
					wps += pt[i];
				}
			}
			break;
		case 2:
			for (i = 0; i < 100; i += 4) {
				/* R,Gr,B,Gb */
				aeMoeWp[i] = pt[i/4]*3;	/* R */
				aeMoeWp[i+1] = pt[i/4]*3;	/* Gr */
				aeMoeWp[i+2] = pt[i/4]*1;	/* B */
				aeMoeWp[i+3] = pt[i/4]*3;	/* Gb */
				wps += aeMoeWp[i] + aeMoeWp[i+1] + aeMoeWp[i+2] + aeMoeWp[i+3];			
			}
			break;
		default:
			/* not this case */
			ASSERT(0, 1); 
			break;
	}
	
	aeWpSum = aeWpSum0 = wps;
	
	return mode;
}

/**
 * @fn		  void aeProcReset(void)
 * @brief          ae proc flow reset
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void 
aeProcReset(
	void
)
{
	aeInit(aeProc);
}

/**
 * @fn		  void aeCalibrationCurLumaGet(void)
 * @brief          Calibration Mode Get Current Luma
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
aeCalibrationCurLumaGet(
	void
)
{
	UINT8	rgb[20*2*4];
	UINT8	y[20];
	UINT16	R,GR,B,GB;
	UINT8 	idx;
	UINT16	ysum;
	UINT16	ywtsum;
	UINT32 	avy,wdiv;

	

	if (aeCalculateLumaMode != LUMA_NORMAL) {
		aeIntrPutChar('C');	
		ysum = ywtsum = 0;
		
		// ------------------------------------- //
		//	read R,G,B value or y value from DRAM 	//
		// ------------------------------------- //	
		HAL_CdspAEWinRead(160, &rgb);		//rgb ae window

		for (idx=0;idx<(5*4);idx++) {
			R =(UINT16)rgb[idx*8+1]*256 + (UINT16)rgb[idx*8];
			GR=(UINT16)rgb[idx*8+3]*256 + (UINT16)rgb[idx*8+2];
			B =(UINT16)rgb[idx*8+5]*256 + (UINT16)rgb[idx*8+4];
			GB=(UINT16)rgb[idx*8+7]*256 + (UINT16)rgb[idx*8+6];
			y[idx] = (UINT8)(0.299 * (FINT32)R + 0.587 * (FINT32)(((FINT32)GR + (FINT32)GB)/2) + 0.114 * (FINT32)B);
			ysum += (UINT32)(y[idx]*AeCalWt2[aeCalculateLumaMode-1][idx]);
			ywtsum += AeCalWt2[aeCalculateLumaMode-1][idx];				
		}

		// -------------------------------- //
		//		        luma average		     //
		// -------------------------------- //
		//rgb mode
		avy = ysum * ((UINT32) acctbl[XBYTE[REG_Cdsp_AEhaccfactor]&0x07]) * ((UINT32) acctbl[(XBYTE[REG_Cdsp_AEhaccfactor]>>4)&0x07]);
		wdiv = (UINT32)ywtsum* ((UINT32) XBYTE[REG_Cdsp_AEhsize]) * ((UINT32) XBYTE[REG_Cdsp_AEvsize]);

		avy /= wdiv;

		return(UINT8)avy;
	}
	
}

/**
 * @fn		  void aeGidStepSet(UINT8 luma)
 * @brief          gId Step Set function.
 * @param[in]	  iso : 
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void
aeGidStepSet(
	UINT8 luma
)
{
#if 1
	SINT16 val;

	switch (G_Negative) {
		case 0://noraml case
			//if (GlbFlag==0 || Ggain_mode_En==0){
				if(luma>(gAeTgtLvl+120))		val=10;
				else if (luma>(gAeTgtLvl+103))	val=9;
				else if (luma>(gAeTgtLvl+88))	val=8;
				else if (luma>(gAeTgtLvl+74))	val=7;
				else if (luma>(gAeTgtLvl+61))	val=6;
				else if (luma>(gAeTgtLvl+49))	val=5;
				else if (luma>(gAeTgtLvl+38))	val=4;
				else if (luma>(gAeTgtLvl+27))	val=3;
				else if (luma>(gAeTgtLvl+17))	val=2;
				else if (luma>(gAeTgtLvl+ 8))	val=1;
				else if (luma>(gAeTgtLvl+ 0))	val=0;
				else if (luma>(gAeTgtLvl- 8)) 	val=-1;
				else if (luma>(gAeTgtLvl-15))	val=-2;
				else if (luma>(gAeTgtLvl-22))	val=-3;
				else if (luma>(gAeTgtLvl-29))	val=-4;
				else if (luma>(gAeTgtLvl-35))	val=-5;
				else if (luma>(gAeTgtLvl-40))	val=-6;
				else if (luma>(gAeTgtLvl-46))	val=-7;
				else if (luma>(gAeTgtLvl-51))	val=-8;
				else if (luma>(gAeTgtLvl-55))	val=-9;
				else							val=-10;//(luma>(gAeTgtLvl-60))
			//}
			break;
		case 1://II AE case
			//if (GlbFlag==0 || Ggain_mode_En==0){
				if(luma>(gAeTgtLvl+120))		val=-10;
				else if (luma>(gAeTgtLvl+103))	val=-9;
				else if (luma>(gAeTgtLvl+88))	val=-8;
				else if (luma>(gAeTgtLvl+74))	val=-7;
				else if (luma>(gAeTgtLvl+61))	val=-6;
				else if (luma>(gAeTgtLvl+49))	val=-5;
				else if (luma>(gAeTgtLvl+38))	val=-4;
				else if (luma>(gAeTgtLvl+27))	val=-3;
				else if (luma>(gAeTgtLvl+17))	val=-2;
				else if (luma>(gAeTgtLvl+ 8))	val=-1;
				else if (luma>(gAeTgtLvl+ 0))	val=0;
				else if (luma>(gAeTgtLvl- 8)) 	val=1;
				else if (luma>(gAeTgtLvl-15))	val=2;
				else if (luma>(gAeTgtLvl-22))	val=3;
				else if (luma>(gAeTgtLvl-29))	val=4;
				else if (luma>(gAeTgtLvl-35))	val=5;
				else if (luma>(gAeTgtLvl-40))	val=6;
				else if (luma>(gAeTgtLvl-46))	val=7;
				else if (luma>(gAeTgtLvl-51))	val=8;
				else if (luma>(gAeTgtLvl-55))	val=9;
				else							val=10;//(luma>(gAeTgtLvl-60))
			//}
			break;
		default:
			/* not this case */
			INTR_ASSERT(0, 1); 
			break;
	}

	//intrPutUint16((UINT16)(val));
	//intrPutChar(',');
	//intrPutUint16((UINT16)(gId));
	//intrPutChar(',');

	switch (G_aeSpeed) {
		case SP1K_AE_JUMP_STEP_NORMAL://noraml speed
			gId+=val;
			break;
		case SP1K_AE_JUMP_STEP_DIV2://noraml speed/2
			if (val >= 0) {
				val+=1;
			}
			else {
				val-=1;
			}
			val/=2;
			gId+=val;
			break;
		case SP1K_AE_JUMP_STEP_DIV3://noraml speed/3
			if (val >= 0) {
				val+=2;
			}
			else {
				val-=2;
			}
			val/=3;
			gId+=val;
			break;
		case SP1K_AE_JUMP_STEP_DIV4://noraml speed/4
			if (val >= 0) {
				val+=3;
			}
			else {
				val-=3;
			}
			val/=4;
			gId+=val;
			break;
		default:
			/* not this case */
			INTR_ASSERT(0, 1); 
			break;
	}
	//intrPutUint16((UINT16)(gId));
	//intrPutChar('\n');
#else
	switch (G_Negative) {
		case 0:
			switch (G_aeSpeed) {
				case 0:
					//if (GlbFlag==0 || Ggain_mode_En==0){
						if(luma>(gAeTgtLvl+120))		gId+=10;
						else if (luma>(gAeTgtLvl+103))	gId+=9;
						else if (luma>(gAeTgtLvl+88))	gId+=8;
						else if (luma>(gAeTgtLvl+74))	gId+=7;
						else if (luma>(gAeTgtLvl+61))	gId+=6;
						else if (luma>(gAeTgtLvl+49))	gId+=5;
						else if (luma>(gAeTgtLvl+38))	gId+=4;
						else if (luma>(gAeTgtLvl+27))	gId+=3;
						else if (luma>(gAeTgtLvl+17))	gId+=2;
						else if (luma>(gAeTgtLvl+ 8))	gId+=1;
						else if (luma>(gAeTgtLvl+ 0))	gId+=0;
						else if (luma>(gAeTgtLvl- 8)) 	gId-=1;
						else if (luma>(gAeTgtLvl-15))	gId-=2;
						else if (luma>(gAeTgtLvl-22))	gId-=3;
						else if (luma>(gAeTgtLvl-29))	gId-=4;
						else if (luma>(gAeTgtLvl-35))	gId-=5;
						else if (luma>(gAeTgtLvl-40))	gId-=6;
						else if (luma>(gAeTgtLvl-46))	gId-=7;
						else if (luma>(gAeTgtLvl-51))	gId-=8;
						else if (luma>(gAeTgtLvl-55))	gId-=9;
						else							gId-=10;//(luma>(gAeTgtLvl-60))
					//}
					break;
				case 1:
					//if (GlbFlag==0 || Ggain_mode_En==0){		
						if(luma>(gAeTgtLvl+120))		gId+=5;
						else if (luma>(gAeTgtLvl+103))	gId+=5;
						else if (luma>(gAeTgtLvl+88))	gId+=4;
						else if (luma>(gAeTgtLvl+74))	gId+=4;
						else if (luma>(gAeTgtLvl+61))	gId+=3;
						else if (luma>(gAeTgtLvl+49))	gId+=3;
						else if (luma>(gAeTgtLvl+38))	gId+=2;
						else if (luma>(gAeTgtLvl+27))	gId+=2;
						else if (luma>(gAeTgtLvl+17))	gId+=1;
						else if (luma>(gAeTgtLvl+ 8))	gId+=1;
						else if (luma>(gAeTgtLvl+ 0))	gId+=0;
						else if (luma>(gAeTgtLvl- 8)) 	gId-=1;
						else if (luma>(gAeTgtLvl-15))	gId-=1;
						else if (luma>(gAeTgtLvl-22))	gId-=2;
						else if (luma>(gAeTgtLvl-29))	gId-=2;
						else if (luma>(gAeTgtLvl-35))	gId-=3;
						else if (luma>(gAeTgtLvl-40))	gId-=3;
						else if (luma>(gAeTgtLvl-46))	gId-=4;
						else if (luma>(gAeTgtLvl-51))	gId-=4;
						else if (luma>(gAeTgtLvl-55))	gId-=5;
						else							gId-=5;//(luma>(gAeTgtLvl-60))
					//}
					break;
				default:
					/* not this case */
					INTR_ASSERT(0, 1); 
					break;
			}
			break;
		case 1:
			switch (G_aeSpeed) {
				case 0:
					//if (GlbFlag==0 || Ggain_mode_En==0){
						if(luma>(gAeTgtLvl+120))		gId-=10;
						else if (luma>(gAeTgtLvl+103))	gId-=9;
						else if (luma>(gAeTgtLvl+88))	gId-=8;
						else if (luma>(gAeTgtLvl+74))	gId-=7;
						else if (luma>(gAeTgtLvl+61))	gId-=6;
						else if (luma>(gAeTgtLvl+49))	gId-=5;
						else if (luma>(gAeTgtLvl+38))	gId-=4;
						else if (luma>(gAeTgtLvl+27))	gId-=3;
						else if (luma>(gAeTgtLvl+17))	gId-=2;
						else if (luma>(gAeTgtLvl+ 8))	gId-=1;
						else if (luma>(gAeTgtLvl+ 0))	gId+=0;
						else if (luma>(gAeTgtLvl- 8)) 	gId+=1;
						else if (luma>(gAeTgtLvl-15))	gId+=2;
						else if (luma>(gAeTgtLvl-22))	gId+=3;
						else if (luma>(gAeTgtLvl-29))	gId+=4;
						else if (luma>(gAeTgtLvl-35))	gId+=5;
						else if (luma>(gAeTgtLvl-40))	gId+=6;
						else if (luma>(gAeTgtLvl-46))	gId+=7;
						else if (luma>(gAeTgtLvl-51))	gId+=8;
						else if (luma>(gAeTgtLvl-55))	gId+=9;
						else							gId+=10;//(luma>(gAeTgtLvl-60))
					//}
					break;
				case 1:
					//if (GlbFlag==0 || Ggain_mode_En==0){		
						if(luma>(gAeTgtLvl+120))		gId-=5;
						else if (luma>(gAeTgtLvl+103))	gId-=5;
						else if (luma>(gAeTgtLvl+88))	gId-=4;
						else if (luma>(gAeTgtLvl+74))	gId-=4;
						else if (luma>(gAeTgtLvl+61))	gId-=3;
						else if (luma>(gAeTgtLvl+49))	gId-=3;
						else if (luma>(gAeTgtLvl+38))	gId-=2;
						else if (luma>(gAeTgtLvl+27))	gId-=2;
						else if (luma>(gAeTgtLvl+17))	gId-=1;
						else if (luma>(gAeTgtLvl+ 8))	gId-=1;
						else if (luma>(gAeTgtLvl+ 0))	gId+=0;
						else if (luma>(gAeTgtLvl- 8)) 	gId+=1;
						else if (luma>(gAeTgtLvl-15))	gId+=1;
						else if (luma>(gAeTgtLvl-22))	gId+=2;
						else if (luma>(gAeTgtLvl-29))	gId+=2;
						else if (luma>(gAeTgtLvl-35))	gId+=3;
						else if (luma>(gAeTgtLvl-40))	gId+=3;
						else if (luma>(gAeTgtLvl-46))	gId+=4;
						else if (luma>(gAeTgtLvl-51))	gId+=4;
						else if (luma>(gAeTgtLvl-55))	gId+=5;
						else							gId+=5;//(luma>(gAeTgtLvl-60))
					//}
					break;
				default:
					/* not this case */
					INTR_ASSERT(0, 1); 
					break;
			}
			break;
		default:
			/* not this case */
			INTR_ASSERT(0, 1); 
			break;
	}
#endif
}

/**
 * @fn		  void aeCurrentTableCheck(UINT8 tableSelect)
 * @brief         Current AE Table Check Command used function.
 * @param[in]   tableSelect     typedef enum AE_TABLE_SELECT;
 * @retval	  NULL
 * @see          NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void aeCurrentTableCheck(
	UINT8 tableSelect
)
{
	UINT8 idx, eIdx;
	frontPrevCapabDesc_t *pcap;
	
	frontCapabilityGet(0, &pcap, NULL);

	switch (tableSelect) {
		case AE_TABLE_AE:
			aeSysInfo("AE TABLE [MIN]%bu -> [MAX]%bu\n",G_ae_min_exp_idx,G_ae_max_exp_idx);
			for (idx = G_ae_min_exp_idx, eIdx = G_ae_max_exp_idx; idx <= eIdx; idx++){
				switch (G_AETblSel) {
					case SP1K_FLICKER_60:
						aeSysInfo("IDX %bu\t{\t%d,\t%lu,\t%d,\t%d,\t%bu}\n",idx,pcap->ae60hzTbl[idx].ev,pcap->ae60hzTbl[idx].shutter,pcap->ae60hzTbl[idx].gain,pcap->ae60hzTbl[idx].iris,pcap->ae60hzTbl[idx].captureCompensationGainIdx);
						break;
					case SP1K_FLICKER_50:
						aeSysInfo("IDX %bu\t{\t%d,\t%lu,\t%d,\t%d,\t%bu}\n",idx,pcap->ae50hzTbl[idx].ev,pcap->ae50hzTbl[idx].shutter,pcap->ae50hzTbl[idx].gain,pcap->ae50hzTbl[idx].iris,pcap->ae50hzTbl[idx].captureCompensationGainIdx);
						break;
					default:
						/* not this case */
						ASSERT(0, 1); 
						break;
				}
			}
			break;
		case AE_TABLE_GAIN:
			aeSysInfo("GAIN TABLE [MIN]%bu -> [MAX]%bu\n",pcap->gainMinIdx, pcap->gainMaxIdx);
			for (idx = pcap->gainMinIdx, eIdx = pcap->gainMaxIdx; idx <= eIdx; idx++){
				aeSysInfo("IDX %bu\t{0x%04x}\n",idx,pcap->gainTbl[idx]);
			}
			break;
	}
}

/**
 * @fn		  void aePccamSrcSizeSet(UINT16 idx,UINT8 opt)
 * @brief          pccam mode ae src size set
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void
aePccamSrcSizeSet(
	UINT8 mode
)
{
	if (aeProc == 1) {
		switch (mode) {

			case AE_PCCAM_320_240:
				aeIIYuvSrcSizeMode = AE_PCCAM_320_240;
				break;
			case AE_PCCAM_160_120:
				aeIIYuvSrcSizeMode = AE_PCCAM_160_120;
				break;
			default:
				ASSERT(0,1);
				break;
		}
	}
}

/**
 * @fn        void aeCallbackReg(UINT32 id, void *pfunct)
 * @brief     register callback of ae
 * @param     [in] id : index of callback
 * @param     [in] pfunct : function point you wanna register
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-17
 * @todo      N/A
 * @bug       N/A
*/
void
aeCallbackReg(
	UINT32 id,
	void *pfunct
)
{
	switch ( id ) {
		case SP1K_AE_CALLBACK_USER_DEFINE_METER:
			//printf( "%s: %ld\n", __FILE__, (UINT32)__LINE__ );
			paeUserDefineMeter = pfunct;
			break;
		#if 0
		case SP1K_AE_CALLBACK_WINDOW_INFORMATION:
			//printf( "%s: %ld\n", __FILE__, (UINT32)__LINE__ );
			paeWindowInformation = pfunct;
			break;
		#endif
		default:
			/* no this case */
			ASSERT(0,0);
			break;
	}
}

/**
 * @fn		  void aeConfigForIQTunning(UINT16 idx,UINT8 opt)
 * @brief          ae Config For IQ Tunning
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void
aeIntrDebug(
	void
)
{
	/* ae moe fail */
	if (aeMoeNew == 0){
		aeMoeDebugCnt++;

		if (aeMoeDebugCnt > 8) {
			intrPutUint8(G_AEEN);
			intrPutUint8(aeMoeNew);
			intrPutUint8(aeProc);
			intrPutUint8(aeGainExpSetEn);
			intrPutUint8(moeDbgTrig);
			intrPutUint32(moeDbgAddr);
			intrPutUint8(iiDbgTrig);
			INTR_ASSERT(0,1);
		}
	}
	else {
		aeMoeDebugCnt = 0;
	}
}

#if (defined(ICAT_OPTION) || defined(AMCAP_OPTION))
/**
 * @fn		  void aeConfigForIQTunning(UINT16 idx,UINT8 opt)
 * @brief          ae Config For IQ Tunning
 * @param	  NONE
 * @retval	  
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void
aeConfigForIQTunning(
	UINT16 idx,
	UINT8 opt
)
{
	UINT32 shutter;
	UINT16 gain;

	if (idx>G_ae_max_exp_idx) {
		idx = G_ae_max_exp_idx;
	}
	else if (idx<G_ae_min_exp_idx) {
		idx = G_ae_min_exp_idx;
	}

	HAL_FrontAeGet( idx, opt, &shutter, &gain );

	G_SHUTTER = shutter;
	G_GAIN = gain;

	G_AEReady = AE_READY;
	aeGainExpSetEn = 1;

	while( !( G_AEReady == AE_NOT_READY && G_AESetting == AE_SET_FINISH) );

	HAL_FrontSignalWait(HAL_FRONT_WAIT_VSYNC, 1, 2);
	
	aeGainExpSetEn = 0;

}
#endif

void 
aeFrameIntervalSet(
	UINT8 en
)
{
	G_FrameIntervalOption = en;
}
