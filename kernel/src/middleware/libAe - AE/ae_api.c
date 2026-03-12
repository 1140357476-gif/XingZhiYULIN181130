/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
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

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "asicreg.h"
#include "ae_api.h"
#include "ae_def.h"
#include "ae_debug.h"
#include "front_logi.h"
#include "initio.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_calibration_api.h"
#include "hal.h"
#include "hal_cdsp.h"
#include "dbg_def.h"
#include "hal_front.h"
#include "hal_global.h"
#include "reg_def.h"
#include "sp1k_ae_api.h"
#include "sp1k_front_api.h"
#include "gpio_api.h"
#include "hal_fd.h"
#include "awb_api.h"
#include "cdsp_task.h"
#include "icat_stub.h"
#include "sp1k_osd_api.h"
#include "os_api.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
#define __FILE	__FILE_ID_LIB_AE_API__

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define AE_COMP_RANGE (SP1K_AE_EXP_COMP_P20+1)
#define G_3AFreq		3

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/
xdata UINT8 G_StdLumi;
xdata UINT16 G_StdLumi_16;
xdata UINT8 G_ae_max_exp_idx = 200;
xdata UINT8 G_ae_min_exp_idx = 0;
xdata UINT8 G_ae_max_gain_idx;
xdata UINT8 G_ae_min_gain_idx;
xdata UINT8 G_ae_min_awbgain_idx;
xdata UINT8 G_ae_max_awbgain_idx;
xdata UINT8 G_ae_init_complete = 0;
xdata UINT8 G_ae_cur_mode = 0;
xdata UINT8 G_ExpGainSetPos = 0x00;			//AE_EXP_GAIN_SET_POS_VD_RISING, AE_EXP_GAIN_SET_POS_VVALID_FALLING
xdata UINT8 G_GainAfterExp = 0x01;			//AE_GAIN_AFTER_EXP_OFF, AE_GAIN_AFTER_EXP_ON
xdata UINT8 G_AESetting = AE_SET_FINISH	;	//AE_SET_FINISH:  setting ae start, AE_SET_FINISH: setting ae finish
xdata UINT8 G_ae_frameRate;

xdata UINT8 PreGain = 0;
xdata UINT32 PreShutter = 0;

xdata UINT8 G_AEEN = AE_DISABLE;

code SINT8 ae_comp[AE_COMP_RANGE] = {
	#if 0
	-40 ,   -38 ,   -36 ,   -34 ,   -32 ,
	-30 ,   -28 ,   -26 ,   -24 ,   -22 ,
	-20 ,   -18 ,   -16 ,   -14 ,   -12 ,
	-10 ,   -8  ,   -6  ,   -4  ,   -2  ,
	0   ,
	2   ,   4   ,   6   ,   8   ,   10  ,
	12  ,   14  ,   16  ,   18  ,   20  ,
	22  ,   24  ,   26  ,   28  ,   30  ,
	32  ,   34  ,   36  ,   38  ,   40
	#else
	-90 ,	-88 ,	-84 ,	-81 ,	-78 ,
	-74 ,	-69 ,	-66 ,	-63 ,	-58 ,
	-53 ,	-48 ,	-43 ,	-39 ,	-33 ,
	-27 ,	-21 ,	-15 ,	-9	,	-4	,
	0	,
	3	,	8	,	17	,	30	,	36	,
	41	,	47	,	53	,	59	,	64	,
	70	,	76	,	82	,	87	,	93	,
	99	,	105 ,	110 ,	115 ,	120 ,
	#endif
};
code SINT8 ae_comp_idx[AE_COMP_RANGE] = {
	-20 ,	-19 ,	-18 ,	-17 ,	-16 ,
	-15 ,	-14 ,	-13 ,	-12 ,	-11 ,
	-10 ,	-9  ,	-8 ,	-7  ,	-6  ,
	-5  ,	-4  ,	-3 ,	-2	,	-1	,
	0	,
	1	,	2	,	3	,	4	,	5	,
	6	,	7	,	8	,	9	,	10	,
	11	,	12	,	13	,	14	,	15	,
	16	,	17  ,	18  ,	19  ,	20  ,
};

xdata UINT8 aeMeteringMode;

static xdata UINT8 aeCompIdx;
static UINT8 shutter = 0;
UINT32 aeStartTime = 0;
UINT8 isoMode;	//default value = 0 : SP1K_AE_ISO_AUTO
UINT8 isoValue = SP1K_AE_ISO_100;	//default value = SP1K_AE_ISO_100
UINT8 antiShakeFps;	//default value = 0

CaptureHeaderAeInformation capHdrInfor;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
extern UINT8 Ggain_mode_En;
extern UINT8 gAeTgtLvl;
extern UINT16 gAeTgtLvl_16;

extern xdata UINT8 G_calibrationMode;

extern xdata UINT8 G_AEReady;
extern xdata UINT8	G_GAIN;
extern xdata UINT32 G_SHUTTER;
extern xdata UINT8 G_AETblSel;
extern xdata	UINT8	G_CaptureCompensationGAIN;

extern xdata UINT8 calibrationAEOn;
extern xdata UINT8 calibrationAWBOn;

extern UINT8 g_AWB_Video_Record_On;
extern xdata UINT8 awbFixMode;


static UINT8 frontDataFmt;

extern UINT8 g_AE_Stable;
extern SINT16 gId;
extern UINT8 G_luma;

//extern UINT16  g_AE_Contrast_Ratio;

extern UINT8 aeProc;

extern xdata UINT8 aeCount;


extern UINT8 aeYMax;
extern UINT8 aeYMin;
extern UINT8 aeGainExpSetEn;

extern xdata	UINT8	G_Negative;
extern SysDynamicEnhan SysDynamicValue;

extern void sp1kOsdNumberShow(UINT16 num, UINT8 x, UINT8 y, UINT8 fillbit ,UINT8 attrib);
extern void sp1kOsdLayoutAttrGet(UINT8* hNum, UINT8* vNum);

extern UINT8 aeSyncStatus;
extern osSysMode_t osSysMode;

extern UINT8 photoEnable;
extern UINT8 HAL_GlobalICVerGet(void);
extern UINT8 gAeMinCvgZon;
extern UINT8 gAeMaxCvgZon;

extern UINT8 G_FrameIntervalOption;

extern xdata UINT8 G_aeSpeed;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @brief	AE Ready Get
 * @param	mode		: mode
 *                  start			: start
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
void
aeExposureGainSet(
	UINT8 vdCount
)
{
	vdCount = vdCount;

	/* check ae ready & shutter flag */
	if ((G_AEReady == AE_READY) && (shutter == 0)) {
		EGTaeIntrPutChar('*');
		G_AESetting = AE_SET_START;

		aeTimingGpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x10);
			
		/*do shutter setting*/
		if (G_SHUTTER != PreShutter) {
			frontIntrExposureTimeSet(0,G_SHUTTER,1,0);
			PreShutter = G_SHUTTER;
		}

		aeTimingGpioByteOutSet(GPIO_BYTE_GEN1, 0x10, 0x00);

		if ((G_GainAfterExp == AE_GAIN_AFTER_EXP_ON) && (G_FrameIntervalOption == 0)) {
			shutter = 1;
		}
		else {
			aeTimingGpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x20);

			/*do gain setting*/
			if (PreGain != G_GAIN) {
				frontIntrGainSet(0,G_GAIN,1,0);
				PreGain=G_GAIN;
			}

			aeTimingGpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);

			shutter = 0;
			G_AESetting = AE_SET_FINISH;
		}
		
		G_AEReady = AE_NOT_READY;
		
	}
	else if (shutter == 1) {

			aeTimingGpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x20);			
			/*do gain setting;*/
			if (PreGain != G_GAIN) {
				frontIntrGainSet(0,G_GAIN,1,0);
				PreGain = G_GAIN;
			}

			aeTimingGpioByteOutSet(GPIO_BYTE_GEN1, 0x20, 0x00);

			shutter = 0;
			G_AESetting = AE_SET_FINISH;
			
	}
}

/**
 * @brief	Enable/Disable AE control
 * @param	mode	: disable/Auto, 0/1
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
void
aeModeEnableSet(
	UINT8 enable
)
{
	#ifndef AAA_ON
	enable = 0;
	#endif

	/* check sensor output data format */
	switch (frontDataFmt) {
		case FRONT_DATA_FMT_RAW:
			/* check calibration AE ctrl */
			if ( calibrationAEOn == (UINT8)0 || ( calibrationAEOn == 0 ) ) {
				enable = AE_DISABLE;
			}

			/* ae enable or disable */
			switch (enable) {
				case AE_DISABLE:
					if (aeProc != 1) {
						HAL_CdspAEWinEnSet(0, 0, 0);				/* ae disable, ae holdsvd disable, ae sizezf disable */
						HAL_CdspAEWinHoldSet(1);					/* ae hold enable */
						HAL_CdspAEWinTest(0);						/* ae test disable */
						XBYTE[REG_Cdsp_Cdspinten_lo] &= (~0x04);	/* AE window done intr svc disable */
						//XBYTE[REG_Cdsp_pSkipwr] |= 0x04;			/* skip writing AE data to DRAM enable */
					}
					
					while (G_AESetting != AE_SET_FINISH);			/* disable auto ae after wait ae setting finish */
					
					G_AEEN = AE_DISABLE;
					aeGainExpSetEn = AE_DISABLE;
					G_AEReady = AE_NOT_READY;
					g_AE_Stable = 0;
					aeSyncStatus = AE_STATUS_PROC_TRIGGER;

					if (aeProc == 1) {
						HAL_FdEnable(0, 0);
					}

					aeSysInfo("ae disable\n");
					break;
				case AE_ENABLE:
					if (aeProc != 1) {
						HAL_CdspAEWinEnSet(1, 1, 0);				/* ae enable, ae holdsvd enable, ae sizezf disable */
						HAL_CdspAEWinHoldSet(0);					/* ae hold disable */
						HAL_CdspAEWinTest(0);						/* ae test disable */
						//XBYTE[REG_Cdsp_Cdspinten_lo] |= 0x04;		/* AE window done intr svc enable */
						HAL_CdspVDramSkipWrSet(CDSP_SKIP_IMG,0);	/* don't skip writing IMAGE data to DRAM */
						HAL_CdspVDramSkipWrSet(CDSP_SKIP_AE,0);		/* don't skip writing AE data to DRAM */
						
						XBYTE[REG_Cdsp_evnt1] = 0x04;
						
						while(XBYTE[REG_Cdsp_pSkipwr]&0x04);
					}

					while (G_AESetting != AE_SET_FINISH);			/* enable auto ae after wait ae setting finish */
					
					aeInit(aeProc);
					
					G_AEEN = AE_ENABLE;
					aeGainExpSetEn = AE_ENABLE;
					
					aeSysInfo("ae enable\n");
					break;
				default:
					/* not this case */
					ASSERT(0, 1); 
					break;
			}
			
			break;
		case FRONT_DATA_FMT_YUV:
			switch (enable) {
				case AE_DISABLE:
					G_AEEN = AE_DISABLE;
					aeGainExpSetEn = AE_DISABLE;
					G_AEReady = AE_NOT_READY;
					break;
				case AE_ENABLE:
					G_AEEN = AE_ENABLE;
					aeGainExpSetEn = AE_ENABLE;
					G_AEReady = AE_NOT_READY;
					break;
				default:
					/* not this case */
					ASSERT(0, 1); 
					break;
			}
			break;
		default:
			/* not this case */
			ASSERT(0, 1); 
			break;
	}

	G_ae_cur_mode = enable;
}

/**
 * @brief	Get Enable/Disable AE Status
 * @param	mode	: disable/Auto, 0/1
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
UINT8
aeModeEnableGet(
	void
)
{
	return G_ae_cur_mode;
}

/**
 * @brief	Enable/Disable AE control
 * @param	mode	: disable/Auto, 0/1
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
void
aeWinModeSet(
	UINT8 aeMode
)
{
	if (aeProc == 1) {
		return;
	}

	HAL_CdspAEWinModeSet(aeMode);		//ae mode set 0:y window, 1:RGB window
}

#if 0
/**
 * @brief	AE Meter Change
 * @param	mode	: SP1K_AE_METER_MULTI /
 *                                  SP1K_AE_METER_CENTER /
 *                                  SP1K_AE_METER_SPOT
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */

UINT8
aeMeterChangeSet(
	UINT8 mode
)
{
	if ((mode!=SP1K_AE_METER_MULTI)
	&& (mode!=SP1K_AE_METER_CENTER)
	&& (mode!=SP1K_AE_METER_SPOT)) {
		return FAIL;
	}

	aeMeteringMode = mode;

	aeSetMetering();
	
	return SUCCESS;
}
#endif

/**
 * @brief	AE Meter Get
 * @param	
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
UINT8
aeMeteringModeGet(
	void
)
{
	return aeMeteringMode;
}

/**
 * @brief	AE Std Lun Set
 * @param	lumi	: Lumi
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
UINT8
aeStdLumiSet(
	UINT8 lumi
)
{
	G_StdLumi = lumi;
	G_StdLumi_16 = lumi << 1;

	return SUCCESS;
}

/**
 * @brief	AE Exp Comp Set
 * @param	light		: light
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
UINT8
aeExpCompSet(
	UINT8 light
)
{
	aeCompIdx = light;

	gAeTgtLvl = G_StdLumi + ae_comp[light];
	gAeTgtLvl_16 = G_StdLumi_16 + (ae_comp[light] << 1);

	return SUCCESS;
}

/**
 * @brief	AE Exp Comp Get
 * @param	
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
SINT8
aeExpCompGet(
	void
)
{
	return ae_comp[ aeCompIdx ];
}

/**
 * @brief	AE Exp Comp Idx Get
 * @param	
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
SINT8
aeExpCompIdxGet(
	void
)
{
	return ae_comp_idx[ aeCompIdx ];
}

/**
 * @brief	Frame Rate Change
 * @param	framerate	: framerate
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
UINT8
aeFrameRateChangeSet(
	UINT16 framerate
)
{
	frontPrevCapabDesc_t *pcap;


	frontCapabilityGet(0, &pcap, NULL);
	G_ae_max_exp_idx = pcap->aeMaxIdxTbl[framerate&0x01];
	G_ae_min_exp_idx = pcap->aeMinIdxTbl[framerate&0x01];
	G_ae_max_gain_idx = pcap->gainMaxIdx;
	G_ae_min_gain_idx = pcap->gainMinIdx;

	if ((framerate == SP1K_FRAME_RATE_50) || (framerate == SP1K_FRAME_RATE_25) || (framerate == SP1K_FRAME_RATE_20) ||
		(framerate == SP1K_FRAME_RATE_12P5) || (framerate == SP1K_FRAME_RATE_SP_50HZ)) {
		aeTblSelSet(SP1K_FLICKER_50);
	}
	else {  /* (framerate == SP1K_FRAME_RATE_60) || (framerate == SP1K_FRAME_RATE_30) || (framerate == SP1K_FRAME_RATE_24) || (framerate == SP1K_FRAME_RATE_15) || (framerate == SP1K_FRAME_RATE_SP_60HZ) */
		aeTblSelSet(SP1K_FLICKER_60);
	}

	return SUCCESS;
}

/**
 * @brief	AE Window Config
 * @param	
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
void
aeWinConfigSet(
	UINT16 hSize,
	UINT16 vSize,
	UINT16 aeDispFactor
)
{
	UINT8 aeWinMode;
	UINT16 aeHSize,aeVSize,aeScaleHSize,aeScaleVSize,hScaleSize,vScaleSize;
	UINT16 aeHOffset,aeVOffset,aeH1stScaleFact,aeV1stScaleFact,aeRgbHOffSet,aeRgbVOffSet;
	UINT8 AEhaccfactor,AEvaccfactor;
	UINT8 divNum, interval, distanceH, distanceV, subNumH, subNumV;
	UINT16 rhSize,rvSize;

	if (aeProc == 1) {
		return;
	}

	rhSize = hSize;
	rvSize = vSize;

	hSize = (UINT32)hSize*99/100;
	vSize = (UINT32)vSize*99/100;

	aeWinMode = HAL_CdspAEWinModeGet();

	/* windows matrix set */
	if ( aeWinMode == AE_WIN_MODE_Y ) {	
		divNum = 8;	/* Y Window, 8*8 */
		interval = 7;
		distanceH = 2;	/* h interval 2 */
		distanceV = 2;	/* v interval 2 */
		subNumH = interval * distanceH;
		subNumV = interval * distanceV;
	}
	else {								
		divNum = 5;	/* RGB Window 5*5 */
		interval = 4;
		distanceH = 2;	/* h interval 2 */
		distanceV = 2;	/* v interval 2 */
		subNumH = interval * distanceH;
		subNumV = interval * distanceV;
	}

	/* H,V windows size set */
	aeHSize = (hSize-2-subNumH-(rhSize-hSize))/divNum;	
	aeVSize = (vSize-2-subNumV-(rvSize-vSize))/divNum;	

	if (aeWinMode == AE_WIN_MODE_RGB) {
		aeHSize = (aeHSize >> 1 << 1);
		aeVSize = (aeVSize >> 1 << 1);

		/* H,V offset set */
		aeHOffset  = (hSize-(subNumH)-(aeHSize*divNum)) >> 0;
		aeVOffset  = (vSize-(subNumV)-(aeVSize*divNum)) >> 0;
	}
	else {
		/* H,V offset set */
		aeHOffset  = (hSize-(subNumH)-(aeHSize*divNum)) >> 1;
		aeVOffset  = (vSize-(subNumV)-(aeVSize*divNum)) >> 1;
	}

	if (aeDispFactor > 100) {
		hScaleSize = (UINT32)hSize*100/aeDispFactor;
		vScaleSize = (UINT32)vSize*100/aeDispFactor;
		aeScaleHSize = (UINT32)aeHSize*100/aeDispFactor;
		aeScaleVSize = (UINT32)aeVSize*100/aeDispFactor;
		//aeHOffset += (aeHSize - aeScaleHSize)<<1;
		//aeVOffset += (aeVSize - aeScaleVSize)<<1;

		aeHOffset += (hSize - hScaleSize)>>1;
		aeVOffset += (vSize - vScaleSize)>>1;

		aeHSize = aeScaleHSize;
		aeVSize = aeScaleVSize;

		
		aeH1stScaleFact = ((UINT16)XBYTE[REG_Cdsp_YUVhscalefactor2_high]<<8) & XBYTE[REG_Cdsp_YUVhscalefactor2_low];
		aeV1stScaleFact = ((UINT16)XBYTE[REG_Cdsp_YUVvscalefactor2_high]<<8) & XBYTE[REG_Cdsp_YUVvscalefactor2_low];

		//printf("aeH1stScaleFact=%d,%d\n",aeH1stScaleFact,aeV1stScaleFact);

		aeHOffset += (UINT32)aeH1stScaleFact * (hSize-hScaleSize) / 65536;
		aeHOffset += (UINT32)aeV1stScaleFact * (vSize-vScaleSize) / 65536;

		//aeHOffset += ((UINT32)hSize - hScaleSize)/((UINT32)65536/aeH1stScaleFact);
		//aeVOffset += ((UINT32)vSize - vScaleSize)/((UINT32)65535/aeV1stScaleFact);
		
	}

	/* H,V normalizing factor set */
	if (aeHSize > 128)		AEhaccfactor = 7;
	else if (aeHSize> 64)	AEhaccfactor = 6;
	else if (aeHSize > 32)	AEhaccfactor = 5;
	else if (aeHSize > 16)	AEhaccfactor = 4;
	else if (aeHSize > 8)	AEhaccfactor = 3;
	else if (aeHSize > 4)	AEhaccfactor = 2;
	else if (aeHSize > 2)	AEhaccfactor = 1;
	else if (aeHSize >= 0)	AEhaccfactor = 0;
	if (aeVSize > 128)		AEvaccfactor = 7;
	else if (aeVSize > 64)	AEvaccfactor = 6;
	else if (aeVSize > 32)	AEvaccfactor = 5;
	else if (aeVSize > 16)	AEvaccfactor = 4;
	else if (aeVSize > 8)	AEvaccfactor = 3;
	else if (aeVSize > 4)	AEvaccfactor = 2;
	else if (aeVSize > 2)	AEvaccfactor = 1;
	else if (aeVSize >= 0)	AEvaccfactor = 0;

	if (aeWinMode == AE_WIN_MODE_RGB) {
		AEhaccfactor = AEhaccfactor-1;
		AEvaccfactor = AEvaccfactor-1;
	
		/* RGB win need add crop offset */
		//if (((XBYTE[ REG_Cdsp_AEwinen ] & 0x08)>>3) == 1) {
			if (XBYTE[ REG_Cdsp_Cropen ] & 0x01) {
				aeRgbHOffSet = ( UINT16 ) (0x0f00&(UINT16)(XBYTE[ REG_Cdsp_pcrophoffset_high ]<<8)) | XBYTE[ REG_Cdsp_pcrophoffset_low ];

				aeHOffset += aeRgbHOffSet;
			}
			if ( ( XBYTE[ REG_Cdsp_Cropen ] & 0x02 ) >> 1) {
				aeRgbVOffSet = ( UINT16 ) (0x0f00&(UINT16)(XBYTE[ REG_Cdsp_pcropvoffset_high ]<<8)) | XBYTE[ REG_Cdsp_pcropvoffset_low ];

				aeVOffset += aeRgbVOffSet;
			}
		//}
	}

	/* all reg set */
	HAL_CdspAEwinoffset( aeHOffset, aeVOffset);				/* H,V offset set */
	HAL_CdspAEwinsizeset( aeHSize, aeVSize );				/* H,V windows size set */
	HAL_CdspAEwinfactorset( AEhaccfactor, AEvaccfactor );	/* H,V normalizing factor set */
	HAL_CdspAEwinintvlset( distanceH, distanceV );			/* set ae window interval */
	HAL_CdspAEwinCntset(divNum, divNum);					/* set ae window cnt */
	if (osSysMode!=OS_SYS_MODE_STILL_PV) {
		HAL_CdspAEwinAddrset( K_SDRAM_AviAeWndAddr );			/* set ae output address */
	}
	else {
		HAL_CdspAEwinAddrset( K_SDRAM_PvAeWndAddr );			/* set ae output address */
	}
	
	aeNormalizationFactorSet(aeWinMode);
	
	aeWinConfigSetPrintf("aeWinConfigSet->WinMode:%bu\n",aeWinMode);
	aeWinConfigSetPrintf("hSize=%d, vSize=%d\n",hSize,vSize);
	aeWinConfigSetPrintf("aeHSize=%d, aeVSize=%d\n",aeHSize,aeVSize);
	aeWinConfigSetPrintf("aeHOffset=%d, aeVOffset=%d\n",aeHOffset,aeVOffset);
	aeWinConfigSetPrintf("AEhaccfactor=%bu, AEvaccfactor=%bu\n",AEhaccfactor,AEvaccfactor);
	aeWinConfigSetPrintf("AEwinHintvl=%bu, AEwinVintvl=%bu\n",distanceH,distanceV);
	aeWinConfigSetPrintf("AEwinHCnt=%bu, AEwinVCnt=%bu\n",divNum,divNum);
	aeWinConfigSetPrintf("K_SDRAM_PvAeWndAddr=0x%lx\n",K_SDRAM_PvAeWndAddr);//3866360
	
}

/**
 * @brief	AE Global Gain Enable
 * @param	GlbGnEn		: GlbGnEn
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
void
aeGlobalGainEnableSet(
	UINT8 GlbGnEn
)
{
	Ggain_mode_En = GlbGnEn;
}

/**
 * @brief	Initialize AE setting
 * @param	
 * @author	Huiling Chou
 * @todo        Check AE Win address should move to common buffer
 * @todo        configuration should be set from resource files.
 * @since	2008-3-17
 */
UINT8 
aeInitial(
	void
)
{
	UINT16 para1, hSize, vSize;
	
	/* ae initial start */
	G_ae_init_complete = 0;

	frontDeviceInfoGet(0, FRONT_DEV_INFO_DATA_FMT, &para1, NULL);
	frontDataFmt = para1;

	switch (frontDataFmt) {
		case FRONT_DATA_FMT_RAW:

			if (photoEnable == 0) {
				switch (HAL_GlobalICVerGet()) {
					case 0x00:
						aeProc = 1;
						break;
					case 0x01:
						aeProc = 0;
						break;
				}
			}

			/* ae system dbg set */
			aeSysDbgSet();

			/* ae system II or MOE engine initial & buffer allocat */
			aeInit(aeProc);

			/* ae win or rgb win config setting */
			if (aeProc != 1) {
				//XBYTE[REG_Front_VDRIntNum] = 0x21;
				//HAL_FrontIntrEnable(1, 0x63);  /* enable all 4 interrupts. */
				//HAL_FrontIntrEnable(0, 0x60);  /* disable all 4 interrupts. */ 

				frontPreviewSizeGet(0, &hSize, &vSize);

				if (aeProc == 0) {
					aeWinModeSet(AE_WIN_MODE_Y);
				}
				else {
					aeWinModeSet(AE_WIN_MODE_RGB);
				}
				
				aeCalculateLumaModeSet(LUMA_NORMAL);
				sp1kAeSetModeEn(AE_DISABLE);	/* disable AE */
				aeWinConfigSet(hSize, vSize, 100);

				XBYTE[REG_Cdsp_pSkipwr] &= (~0x04);	//don't skip writing AE data to DRAM	disable
			}

			/* ae system parameter set */
			sp1kAeParamSet(SP1K_AE_TARGET_LUMA, 120);//120
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
			sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_MULTI);
			//sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_30);
			//sp1kAeParamSet(SP1K_AE_USING_AWB_GAIN, 1);

			/* Clear AE stable flag */
			g_AE_Stable = 0;
			break;
		case FRONT_DATA_FMT_YUV:
			
			#if (II_AE_MOTION_DET_EN == 1)
			/* ae system set II AE */
			aeProc = 1;
			
			/* ae system dbg set */
			aeSysDbgSet();

			/* ae system II or MOE engine initial & buffer allocat */
			aeInit(aeProc);
			#endif

			/* ae system parameter set */
			sp1kAeParamSet(SP1K_AE_TARGET_LUMA, 120);//120
			sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
			sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_MULTI);
			//sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_30);
			//sp1kAeParamSet(SP1K_AE_USING_AWB_GAIN, 1);
			
			/* set AE stable always ready */
			g_AE_Stable = 1;		
			break;
		default:
			/* not this case */
			ASSERT(0, 1); 
			break;
	}

	capHdrInfor.ae.aeProc = aeProc;

	/* ae initial complete */
	G_ae_init_complete = 1;

	return SUCCESS;

}

/**
 * @brief	Specify AE paramaters
 * @param	selector - 0: target lumma;
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
void
aeParamSet(
	UINT8 selector,
	UINT16 val
)
{
	UINT8 currAeEn;

	switch (frontDataFmt) {
		case FRONT_DATA_FMT_RAW:
			switch ( selector ) {
				case SP1K_AE_PARA_TARGET_LUMA: // 64
					aeStdLumiSet((UINT8) val);
					break;
				case SP1K_AE_PARA_EV_COMP: // 64
					aeExpCompSet((UINT8) val);
					break;
				case SP1K_AE_PARA_FRAME_RATE:
					/* BACKWARD COMPATIBLE CONVERSION!!! */
					currAeEn = aeModeEnableGet();
					aeModeEnableSet(AE_DISABLE);

					switch ( val ) {
						case SP1K_FRAME_RATE_60:
							val = SP1K_FRAME_RATE_60;						
							frontFrameRateSet(0, 60);
							break;
						case SP1K_FRAME_RATE_50:
							val = SP1K_FRAME_RATE_50;
							frontFrameRateSet(0, 50);
							break;
						case SP1K_FRAME_RATE_30:
							val = SP1K_FRAME_RATE_30;
							frontFrameRateSet(0, 30);
							break;
						case SP1K_FRAME_RATE_25:
							val = SP1K_FRAME_RATE_25;
							frontFrameRateSet(0, 25);
							break;
						case SP1K_FRAME_RATE_24:
							val = SP1K_FRAME_RATE_24;
							frontFrameRateSet(0, 24);
							break;
						case SP1K_FRAME_RATE_20:
							val = SP1K_FRAME_RATE_20;
							frontFrameRateSet(0, 20);
							break;
						case SP1K_FRAME_RATE_15:
							val = SP1K_FRAME_RATE_15;
							frontFrameRateSet(0, 30);
							break;
						case SP1K_FRAME_RATE_12P5:
							val = SP1K_FRAME_RATE_12P5;
							frontFrameRateSet(0, 25);
							break;
						case SP1K_FRAME_RATE_SP_60HZ:
							val = SP1K_FRAME_RATE_SP_60HZ;
							frontFrameRateSet(0, SP1K_FRAME_RATE_SP_60HZ);
							break;
						case SP1K_FRAME_RATE_SP_50HZ:
							val = SP1K_FRAME_RATE_SP_50HZ;
							frontFrameRateSet(0, SP1K_FRAME_RATE_SP_50HZ);
							break;
						default:
							/* not this case */
							ASSERT(0, 1); 
							break;						
					}
					G_ae_frameRate=val;
					
					aeFrameRateChangeSet(val);
					aeModeEnableSet(currAeEn);
					break;
				case SP1K_AE_PARA_METERING:
					aeMeterChangeSet((UINT8) val);
					break;
				case SP1K_AE_PARA_GID:
					gId = (UINT8)val;
					break;
				case SP1K_AE_PARA_CAP_ISO_MODE:
					isoMode = (UINT8)val;
					break;
				case SP1K_AE_PARA_CAP_ISO_VALUE:
					isoValue = (UINT8)val;
					break;
				case SP1K_AE_PARA_CAP_ANTISHAKE_FPS:
					antiShakeFps = (UINT8)val;
					break;
				case SP1K_AE_PARA_MAX_COVER_ZONE:
					gAeMaxCvgZon = (UINT8)val;
					break;
				case SP1K_AE_PARA_MIN_COVER_ZONE:
					gAeMinCvgZon = (UINT8)val;
					break;
				case SP1K_AE_PARA_SPEED:
					G_aeSpeed = (UINT8)val;
					break;
				//case SP1K_AE_USING_AWB_GAIN:
					//gAwbGainMax = val;
					//break;
				//case SP1K_AE_CONVERAGE_ZONE: // 5
					//break;
				//case SP1K_AE_GAINSHUTTERTBL: /* 0:60 or 1:50 */
					//break;
				//case SP1K_AW_H_WINDOW_SIZE: // 8
					//break;
				//case SP1K_AW_V_WINDOW_SIZE: // 8
				//	break;
				default:
					/* not this case */
					ASSERT(0, 1); 
					break;
			}
			break;
		case FRONT_DATA_FMT_YUV:
			switch (selector) {
				case SP1K_AE_PARA_TARGET_LUMA:
					frontAeTargetLuminanceSet(0, val);
					#if (II_AE_MOTION_DET_EN == 1)
					aeStdLumiSet((UINT8) val);
					#endif
					break;
				case SP1K_AE_PARA_EV_COMP:
					frontAeExposureCompensationSet(0, val);
					#if (II_AE_MOTION_DET_EN == 1)
					aeExpCompSet((UINT8) val);
					#endif
					break;
				case SP1K_AE_PARA_FRAME_RATE:
					frontAeFrameRateSet(0, val);
					G_ae_frameRate=val;
					break;
				#if (II_AE_MOTION_DET_EN == 1)
				case SP1K_AE_PARA_METERING:
					aeMeterChangeSet((UINT8) val);
					break;
				#endif
				default:
					/* not this case */
					ASSERT(0, 1); 
					break;
			}
			break;
		default:
			/* not this case */
			ASSERT(0, 1); 
			break;
	}
}

/**
 * @brief	AE Status Get
 * @param	selector		: selector
 *                  val			: val
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
UINT8
aeStatusGet(
	UINT8 selector,
	UINT8 *val
)
{
	SINT16 tmp;
	//frontPrevCapabDesc_t *pcap;
	static UINT8 yuv_ae_cnt = 0;
	UINT8 Ev10gid;

	*val = 0;

	switch (frontDataFmt) {
		case FRONT_DATA_FMT_RAW:
			switch ( selector ) {
				case SP1K_AE_STS_IS_READY: // 64
					*val = g_AE_Stable;
					break;
				case SP1K_AE_STS_LOW_LIGHT: // 64
					if ((gId == G_ae_min_exp_idx) && (G_luma < (gAeTgtLvl-15))) {
						*val = 1;
					}
					else {
						*val = 0;
					}
					break;
				case SP1K_AE_STS_COUNT: // 64
					*val = aeCount;//aeGetCount();
					break;
				case SP1K_AE_STS_EV:
					if ((gId == G_ae_max_exp_idx) && (G_luma > (gAeTgtLvl+15))) {
						*val = 255;
					}
					else if ((gId == G_ae_min_exp_idx) && (G_luma < (gAeTgtLvl-15))) {
						*val = 0;
					}
					else {
						//frontCapabilityGet(0, &pcap, NULL);
						//Ev10gid=Sp1kCalibrationEvValueGet();
						tmp = 100 + gId - Ev10gid/*pcap->ev10ExpIdx*/;
						*val = (UINT8) tmp; //gId + 50;
					}
					break;
				case SP1K_AE_STS_G_GAIN:
					*val = (UINT8)G_GAIN;
					break;
		 		case SP1K_AE_STS_GID:
					*val = (UINT8)gId;
					break;
				case SP1K_AE_STS_FREQ:
					*val = (UINT8)G_AETblSel;
					break;
				case SP1K_AE_STS_CAP_COMPENSATION_GAIN:
					*val = (UINT8)G_CaptureCompensationGAIN;
					break;
				case SP1K_AE_STS_FRAME_RATE:
					*val = (UINT8)G_ae_frameRate;
					break;
				case SP1K_AE_STS_CAP_ISO_MODE:
					*val = (UINT8)isoMode;
					break;
				case SP1K_AE_STS_CURRENT_LUMA:
					*val = (UINT8)G_luma;
					break;
				case SP1K_AE_STS_CAP_ISO_VALUE:
					*val = (UINT8)isoValue;
					break;
				case SP1K_AE_STS_CAP_ANTISHAKE_FPS:
					*val = (UINT8)antiShakeFps;
					break;
				default:
					/* not this case */
					ASSERT(0, 1); 
					break;
			}
			break;
		case FRONT_DATA_FMT_YUV:
			switch ( selector ) {
				case SP1K_AE_STS_IS_READY:
					//frontAeIsReadyGet(0, val);
					*val = 1;  /* always ready. */
					break;
				case SP1K_AE_STS_COUNT: // 64
					*val = yuv_ae_cnt;
					if (yuv_ae_cnt == 0) {
						yuv_ae_cnt = 128;
					}
					else {
						yuv_ae_cnt = 0;
					}
					break;
				case SP1K_AE_STS_FRAME_RATE:
					*val = (UINT8)G_ae_frameRate;
					break;	
				default:
					/* not this case */
					ASSERT(0, 1); 
					break;
			}
			break;
		default:
			/* not this case */
			ASSERT(0, 1); 
			break;
	}
	
	return SUCCESS;
}

/**
 * @brief	AE Ready Get
 * @param	mode		bit0:waitAe, Bit1:waitAwb, bit2:DarkWait
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */
UINT8 aeReadyGet(UINT8 mode)
{
	UINT32 aeNowTime;
	UINT8 darkLuma;
	
	#if 0
	AEHeaderInformation aeHdrInf;

	aeHdrInf.aeProc = 0x25;
	aeHdrInf.stableCondition = 0x5A;
	aeHdrInf.luma[0] = 0xf0;
	aeHdrInf.luma[1] = 0xf0;
	aeHdrInf.gId[0] = 0x2525;
	aeHdrInf.gId[1] = 0x2525;
	aeHdrInf.shutter[0] = 0x12345678;
	aeHdrInf.shutter[1] = 0x12345678;
	aeHdrInf.gain[0] = 0x2020;
	aeHdrInf.gain[1] = 0x2020;		
	aeCaptureHeaderAeInformationSet(HEADER_INFORMATION_AE_CONFIG,&aeHdrInf);
	#endif
	
	/* calibration mode used */
	if ( ( G_calibrationMode == 1 ) || ( calibrationAEOn == 0 ) ) {
		aeStartTime=0;
		return SUCCESS;
	}

	/* get start time */
	if ( !aeStartTime ) {
		aeCount = 0;
		sp1kAwbParamSet(AWB_PARA_Stable, 0);
		HAL_GlobalTimerRead10us(&aeStartTime);
		aeReadyGetPrintf("s>>\n");
		aeReadyGetPrintf("aeStartTime=0x%lx\n",aeStartTime);
		return FAIL;
	}
	
	HAL_GlobalTimerRead10us(&aeNowTime);
	aeReadyGetPrintf("aeNowTime=0x%lx\n",aeNowTime);

	/* set dark luma value */
	switch (aeProc) {
		case 0:
		case 1:
			darkLuma = 80;
			break;
		case 2:
			darkLuma = 8;
			break;
	}

	/* check ae setting shutter & gain flow finish */
	if (G_AESetting == AE_SET_FINISH) {

		UINT8 awbStable = sp1kAwbParamGet(AWB_PARA_Stable);
		
		/* check time out */	
		if (((aeNowTime > aeStartTime) && ((aeNowTime - aeStartTime) > AE_WAIT_TIME_10US)) ||
			((aeNowTime < aeStartTime) && (((~aeStartTime) + aeNowTime + 1)) > AE_WAIT_TIME_10US)) {
			aeReadyGetPrintf("t<<");
			aeReadyGetPrintf("%bx,%bx,%lx,%lx\n",g_AE_Stable,aeCount,aeStartTime,aeNowTime);
			aeStartTime = 0;
			capHdrInfor.ae.stableCondition = 2;
			return SUCCESS;
		}

		/* check ae stable flag */
		if (((!(mode & 0x1)) || (g_AE_Stable && aeCount > 0) ) && 
			((!(mode & 0x2)) || awbStable)) {
			aeReadyGetPrintf("s<<");
			aeReadyGetPrintf("%bx,%bx,%lx,%lx\n",g_AE_Stable,aeCount,aeStartTime,aeNowTime);
			aeStartTime = 0;
			capHdrInfor.ae.stableCondition = 3;
			return SUCCESS;
		}
		
		/* check aeCount number */
		#if 0		
		if (aeCount > maxCount) {
			if (!sp1kAwbParamGet(AWB_PARA_Stable)) {
				return FAIL;//waiting awb ready
			}
			aeReadyGetPrintf("m<<");
			aeReadyGetPrintf("%bx,%bx,%lx,%lx\n",g_AE_Stable,aeCount,aeStartTime,aeNowTime);				
			aeStartTime = 0;
			capHdrInfor.ae.stableCondition = 4;
			sp1kAwbParamSet(AWB_PARA_RunIdx, 0);
			return SUCCESS;
		}
		#endif

		/* check gId and luma */
		if (((mode & 0x4) == 0x04) && gId <= G_ae_min_exp_idx && G_luma < darkLuma) {			
			aeReadyGetPrintf("g<<");
			aeReadyGetPrintf("%bx,%bx,%lx,%lx\n",g_AE_Stable,aeCount,aeStartTime,aeNowTime);
			aeStartTime = 0;
			capHdrInfor.ae.stableCondition = 5;
			return SUCCESS;
		}
	}
	
	return FAIL;
		
}

/**
 * @fn		  void aeInfoShow(void)
 * @brief	        AE show gId, G_GAIN info
 * @param	  NONE
 * @see            NULL
 * @author	  LinJieCheng
 * @since	  2010-6-12
 * @todo	  N/A
 * @bug 	  N/A
*/
void
aeInfoShow(
	void
)
{
	UINT8 OsdMaxX, OsdMaxY;
	//UINT8 aeTabIdx, curLuma, curContrast;

	sp1kOsdLayoutAttrGet(&OsdMaxX, &OsdMaxY);

	//sp1kAeAwbInfo(&aeTabIdx, &curLuma, &curContrast);
	sp1kOsdNumberShow(gId, OsdMaxX-7, OsdMaxY-1, 3, 0);
	sp1kOsdNumberShow(G_GAIN, OsdMaxX-3, OsdMaxY-1, 3, 0);
}

/**
 * @fn        void aeAwbInfo(UINT8* ptGid, ptGid,UINT8* ptLuma, ptLuma,UINT8* ptContrast)
 * @brief     set para to awb
 * @param     [in] ptGid
 * @param     [in] ptLuma
 * @param     [in] ptContrast
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-16
 * @todo      N/A
 * @bug       N/A
*/
void aeAwbInfo(UINT8* ptGid,UINT8* ptLuma,UINT8* ptymax, UINT8* ptymin)
{
	*ptGid = gId;
	*ptLuma = G_luma;
	//*ptContrast = g_AE_Contrast_Ratio;
	*ptymax = aeYMax;
	*ptymin = aeYMin;
}

/**
 * @brief	AE Set Negative Enable
 * @param	NegativeEn		: Negative mode en
 * @author	LinJieCheng
 * @todo
 * @since	2010-10-20
 */
void
aeNegativeEnableSet(
	UINT8 NegativeEn
)
{
	/* for II AE used */
	if (aeProc == 1) {
		G_Negative = NegativeEn;
	}
	else {
		G_Negative = 0;
	}
}

/**
 * @brief	AE Get G_GAIN Value for intr used
 * @param	NegativeEn		: Negative mode en
 * @author	LinJieCheng
 * @todo
 * @since	2010-10-20
 */
void
aeIntrCurGainValueGet(
	UINT8 *val
)
{
	*val = G_GAIN;
}


/**
 * @brief	ae System Debug Set
 * @param	N/A
 * @author	LinJieCheng
 * @todo
 * @since	2010-10-20
 */
void aeSysDbgSet(
	void
)
{
	switch (frontDataFmt) {
		case FRONT_DATA_FMT_RAW:
			aeSysInfo("RAW fmt\n");
			break;
		case FRONT_DATA_FMT_YUV:
			aeSysInfo("YUV fmt\n");
			break;
		default:
			/* not this case */
			aeSysInfo("FAIL fmt\n");
			ASSERT(0, 1); 
			break;
	}
	
	switch(aeProc) {
		case 0:
			aeSysInfo("aeProc = AE WIN\n");
			break;
		case 1:
			aeSysInfo("aeProc = II AE\n");
			break;
		case 2:
			aeSysInfo("aeProc = RGB WIN\n");
			break;
		default:
			/* not this case */
			aeSysInfo("aeProc = NOT THIS PROC\n");
			ASSERT(0, 1); 
			break;
	}

	/* set dbg gpio. */
	#if (0)
	gpioByteFuncSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	gpioByteInGateSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	gpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	#endif	
	aeStatusGpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	aeStatusGpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	aeTimingGpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	aeTimingGpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);
	aeCostGpioByteDirSet(GPIO_BYTE_GEN1, 0x30, 0x30);
	aeCostGpioByteOutSet(GPIO_BYTE_GEN1, 0x30, 0x00);

}

/**
 * @brief	aeCaptureHeaderAeInformationSet
 * @param	NegativeEn		: Negative mode en
 * @author	LinJieCheng
 * @todo
 * @since	2010-10-20
 */
void
aeCaptureHeaderAeInformationSet(
	UINT8 cfg,
	void *point
)
{	
	switch ( cfg ) {
		case HEADER_INFORMATION_AE_CONFIG:
			memcpy(&(capHdrInfor.ae),(AEHeaderInformation*)point,sizeof(AEHeaderInformation));
			break;
		case HEADER_INFORMATION_FLASH_CONFIG:
			memcpy(&(capHdrInfor.flash),(FlashHeaderInformation*)point,sizeof(FlashHeaderInformation));
			break;
		case HEADER_INFORMATION_BSHUTTER_CONFIG:
			memcpy(&(capHdrInfor.bShutter),(BShutterHeaderInformation*)point,sizeof(BShutterHeaderInformation));
			break;
		case HEADER_INFORMATION_MSHUTTER_CONFIG:
			memcpy(&(capHdrInfor.mShutter),(MShutterHeaderInformation*)point,sizeof(MShutterHeaderInformation));
			break;
		case HEADER_INFORMATION_CAPTURE_CONFIG:
			memcpy(&(capHdrInfor.capture),(CaptureHeaderInformation*)point,sizeof(CaptureHeaderInformation));
			break;
		default:
			ASSERT(0,1);
			break;
	}
}

#if (defined(ICAT_OPTION) || defined(AMCAP_OPTION))
UINT8 aeIntrGIdGet(void)
{
	return (UINT8)gId;
}
#endif

