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
#include "initio.h"
#include "front_logi.h"
#include "sp1k_cdsp_api.h"
#include "sp1k_awb_api.h"//wangjg++
#include "ae_api.h"
#include "sp1k_rsvblk_api.h"
#include "asicreg.h"
#include "sp1k_calibration_api.h"
#include "hal_front.h"
#include "hal.h"
#include "dbg_def.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
#define AWB_SNAP_JUMP 40
#define AAA_AWB_DEFAULT_SPEED 3

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define __FILE	__FILE_ID_SP1K_AAA_API__

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A
 **************************************************************************/
//static UINT8 frontDataFmt;
//static UINT8 aaaParamInitFlag = 0;
//UINT8 aaaParam[ SP1K_AWB_PARAM_MAX ];
//static UINT32 aaaWaitReadyMinTime = 0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S
 **************************************************************************/
extern UINT8 g_AE_Stable;
extern UINT8 g_AE_LowLight;
extern UINT8 G_ae_init_complete;
extern UINT8 g_AWB_Stable;
//extern UINT8 gAwbGainMax;
extern SINT16 gId;
extern xdata UINT8 G_ae_max_exp_idx;
extern xdata UINT8 G_ae_min_exp_idx;
extern UINT8 G_luma;
extern UINT8 gAeTgtLvl;
extern xdata UINT8 G_calibrationMode;
extern xdata UINT8 calibrationAEOn;
//extern xdata UINT16 awbCtcParam[];
//extern void AAA_AWBJumpToTargetGain(void);
extern UINT8 g_AE_Stable;
//extern xdata UINT8 aaaCwbFId;
//extern UINT32 aaaCwbTempBuf;

//extern SINT16 aaaCustomRGainShift;
//extern SINT16 aaaCustomBGainShift;

//extern void AAA_AWBCustomWbEvaluate(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/

/**
 * @brief	Initialize AEAWB setting
 * @param	mode	: disable/Auto, 0/1
 * @author	Huiling Chou
 * @todo        Check AE/AWB Win address should move to common buffer
 * @todo        configuration should be set from resource files.
 * @since	2008-3-17
 */
#if TAG_TODO
UINT8
sp1kAeAwbInit(
	void
)
{
	UINT16 para1;


	#if 1
		frontDeviceInfoGet(0, FRONT_DEV_INFO_DATA_FMT, &para1, NULL);
		frontDataFmt = para1;
	#else  /* test, force raw. */
		frontDataFmt = FRONT_DATA_FMT_RAW;
	#endif

	#if TAG_TODO
		aaaCwbTempBuf = ( K_SDRAM_PvAwbWndAddr + 1024 );
	#endif
	
	HAL_FrontIntrEnable(1, 0x63);  /* enable all 4 interrupts. */

	sp1kAeSetModeEn(AE_DISABLE); /* disable AE */
	sp1kAeWinConfig();

	sp1kAwbDisable();
	//awbThresholdInit(0); /* TODO: configuration should be moved to resource */
	//AAA_AWBWinConfig();
	//XBYTE[0x2114] = XBYTE[0x2114] | 0x10; /* will: it shouldn't change it here */

	#if TAG_TODO
		G_ae_init_complete = 0;
		sp1kAeParamSet(SP1K_AE_TARGET_LUMA, 120);
		sp1kAeParamSet(SP1K_AE_EV_COMP, SP1K_AE_EXP_COMP_00);
		sp1kAeParamSet(SP1K_AE_METERING, SP1K_AE_METER_MULTI);
		sp1kAeParamSet(SP1K_AE_FRAME_RATE, SP1K_FRAME_RATE_30);
		sp1kAeParamSet(SP1K_AE_USING_AWB_GAIN, 1);
		G_ae_init_complete = 1;

		//	if( aaaParamInitFlag == 0 ) {
		//		aaaParam[ SP1K_AWB_Jump_Scale ] = AAA_AWB_DEFAULT_SPEED; // default speed

		//		aaaParamInitFlag = 1;	
		//	}

		g_AE_Stable = 0;	/* Clear AE stable flag */
	#endif

	return 0;
}
#endif

/**
 * @brief	Specify AE paramaters
 * @param	selector - 0: target lumma;
 * @author	Huiling Chou
 * @todo
 * @since	2008-3-17
 */

#if TAG_TODO 
void
sp1kAeParamSet(
	UINT8 selector,
	UINT16 val
)
{
	if ( frontDataFmt == FRONT_DATA_FMT_RAW ) {
		switch ( selector ) {
		case SP1K_AE_TARGET_LUMA: // 64
			aeSetStdLumi((UINT8) val);
			break;
		case SP1K_AE_EV_COMP: // 64
			aeSetExpComp((UINT8) val);
			break;
		case SP1K_AE_FRAME_RATE:
			/* BACKWARD COMPATIBLE CONVERSION!!! */
			if ( val >= SP1K_FRAME_RATE_TOT ) {
				switch ( val ) {
				case 300:
					val = SP1K_FRAME_RATE_30;
					break;
				case 301:
					val = SP1K_FRAME_RATE_25;
					break;
				case 150:
					val = SP1K_FRAME_RATE_15;
					break;
				case 151:
					val = SP1K_FRAME_RATE_12P5;
					break;
				}
			}
			aeSetFrameRateChange(val);
			break;
		case SP1K_AE_METERING:
			aeSetMeterChange((UINT8) val);
			break;
		#if TAG_TODO	
			case SP1K_AE_USING_AWB_GAIN:
				gAwbGainMax = val;
				break;
		#endif
		case SP1K_AE_CONVERAGE_ZONE: // 5
			break;
		case SP1K_AE_GAINSHUTTERTBL: /* 0:60 or 1:50 */
			break;
		case SP1K_AW_H_WINDOW_SIZE: // 8
			break;
		case SP1K_AW_V_WINDOW_SIZE: // 8
			break;
		}
	}
	else {  // frontDataFmt == FRONT_DATA_FMT_YUV
		switch (selector) {
		case SP1K_AE_TARGET_LUMA:
			frontAeTargetLuminanceSet(0, val);
			break;
		case SP1K_AE_EV_COMP:
			frontAeExposureCompensationSet(0, val);
			break;
		case SP1K_AE_FRAME_RATE:
			frontAeFrameRateSet(0, val);
			break;
		}
	}
}
#endif

#if TAG_TODO
UINT8
sp1kAeStatusGet(
	UINT8 selector,
	UINT8 *val
)
{
	SINT16 tmp;
	//frontPrevCapabDesc_t *pcap;
	static UINT8 yuv_ae_cnt = 0;
       UINT8 Ev10gid;

	*val = 0;
	if ( frontDataFmt == FRONT_DATA_FMT_RAW ) {
		switch ( selector ) {
		case SP1K_AE_IsReady: // 64
			*val = g_AE_Stable;
			break;
		case SP1K_AE_LowLight: // 64
			if((gId == G_ae_min_exp_idx) && (G_luma < (gAeTgtLvl-15)))
				*val = 1;
			else
				*val = 0;
			break;
		case SP1K_AE_Count: // 64
			*val = AAA_AECOUNT();
			break;
		case SP1K_AE_EV_GET:
			if((gId == G_ae_max_exp_idx) && (G_luma > (gAeTgtLvl+15))) {
				*val = 255;
			}
			else if((gId == G_ae_min_exp_idx) && (G_luma < (gAeTgtLvl-15)))  {
				*val = 0;
			}
			else {
				//frontCapabilityGet(0, &pcap, NULL);
				Ev10gid=Sp1kCalibrationEvValueGet();
				tmp = 100 + gId - Ev10gid/*pcap->ev10ExpIdx*/;
				*val = (UINT8) tmp; //gId + 50;
			}
			break;
		}
	}
	else {  // frontDataFmt == FRONT_DATA_FMT_YUV
		switch ( selector ) {
		case SP1K_AE_IsReady:
			//frontAeIsReadyGet(0, val);
			*val = 1;  /* always ready. */
			break;
		case SP1K_AE_Count: // 64
			*val = yuv_ae_cnt;
			if(yuv_ae_cnt == 0)
				yuv_ae_cnt = 128;
			else
				yuv_ae_cnt = 0;
			break;
		}
	}

	return SUCCESS;
}

#endif
#if  0 //wangjg
void
sp1kAwbScaleSet(
	UINT8 selector,
	UINT8 val
)  //Jim@0527 - AWB Scale Adjust
{
	if ( selector >= SP1K_AWB_PARAM_MAX ) {
		return;
	}

	switch ( selector ) {
		case SP1K_AWB_Jump_Scale:
			if ( frontDataFmt == FRONT_DATA_FMT_RAW ) {
				//wangjg - invalid
				if ( val == SP1K_AWB_SPEED_FAST_ONCE ) {  // We will let wb gain direct jump to target.
					AAA_AWBJumpToTargetGain();
				}
				else {
					aaaParam[ selector ] = val;
				}
			}

			break;

		case SP1K_AWB_PARAM_CWB_FID:
			aaaCwbFId = val;

			break;

		case SP1K_AWB_PARAM_WAIT_READY_MIN_TIME:
			aaaWaitReadyMinTime = (UINT16)val * 10;

			break;

		case SP1K_AWB_PARAM_CUSTOM_R_GAIN_SHIFT:
			aaaCustomRGainShift = (SINT16)((SINT8)val);

			break;

		case SP1K_AWB_PARAM_CUSTOM_B_GAIN_SHIFT:
			aaaCustomBGainShift = (SINT16)((SINT8)val);

			break;
	}

	return;
}
#endif
#if 0
UINT8
sp1kAwbStatusGet(
	UINT8 selector,
	UINT8 *val
)		//Jim@0514 - For snap after AWB stable
{
	static UINT8 yuv_awb_cnt = 0;
	*val = 0;
	if ( frontDataFmt == FRONT_DATA_FMT_RAW ) {
		switch ( selector ) {
		case SP1K_AWB_IsReady: // 64
			*val = g_AWB_Stable;
			break;
		case SP1K_AWB_Count: // 64
			*val = AAA_AWBCOUNT();
			break;
		}
	}
	else {  // frontDataFmt == FRONT_DATA_FMT_YUV
		switch ( selector ) {
		case SP1K_AWB_IsReady:
			//frontAwbIsReadyGet(0, val);
			*val = 1;  /* always ready. */
			break;
		case SP1K_AWB_Count: // 64
			*val = yuv_awb_cnt;
			if ( yuv_awb_cnt == 0 ) {
				yuv_awb_cnt = 128;
			}
			break;
		}
	}

	return SUCCESS;
}
#endif
#if 0
void
sp1kAwbParaInit(
	UINT8 fileId
)
{
	UINT16 i;
	UINT8 sts;
	UINT8 RLSB, RMSB;


	do {
		sts = sp1kNandRsvRead(fileId, (K_SDRAM_CommonEndAddr<< 1));
	} while(!sts);

	M_DRAM_SetStartAddress((UINT32)K_SDRAM_CommonEndAddr, 1);

	for ( i = 0; i < AWB_CTC_PARAM_LENGTH; i++ ) {
		M_DRAM_ReadWord(RLSB, RMSB);
		awbCtcParam[ i ] = (UINT16)(RMSB&0xff)<<8|(RLSB&0xff);
	}

	return;
}
#endif

#if TAG_TODO
UINT8
sp1kAAAReadyGet(
	UINT8 mode,
	UINT8 start
)
{
	UINT8 aeRdy = 0; //Fast ae poweron
	UINT8 aeCnt = 0;
	static UINT8 aeStartCnt = 0;
	UINT8 awbRdy = 0; //Fast awb poweron
	UINT8 awbCnt = 0;
	static UINT8 awbStartCnt = 0;
	static UINT32 startTime = 0;
	static UINT8 waitFlag = 0;
	UINT32 nowTime;

	mode = mode;

	if( ( G_calibrationMode == 1 ) || ( calibrationAEOn == 0 ) ) {
		return SUCCESS;
	}
	if (start != 0 && waitFlag == 0)
			return FAIL;
	else
			ASSERT( !( start != 0 && waitFlag == 0 ), 0 );

	if( waitFlag == 2 ) {
		HAL_GlobalReadGTimer(&nowTime);

		if( nowTime - startTime >= aaaWaitReadyMinTime ) {
			waitFlag = 0;

			return SUCCESS;
		}
		else {
			waitFlag = 2;

			return FAIL;
		}
	}

	if(start == 0) {
		// keep AE start Cnt
		sp1kAeStatusGet(SP1K_AE_Count, &aeStartCnt);
		// keep AWB start Cnt
		//sp1kAwbStatusGet(SP1K_AWB_Count, &awbStartCnt);

		HAL_GlobalReadGTimer(&startTime);

		waitFlag = 1;

		return SUCCESS;
	}
	else {
		if( aeRdy == 0 || aeCnt < 10){
			sp1kAeStatusGet(SP1K_AE_IsReady, &aeRdy);
			sp1kAeStatusGet(SP1K_AE_Count, &aeCnt);
			aeCnt = ( ( (UINT16)aeCnt + SP1K_AAA_AE_CNT_RANGE ) - aeStartCnt ) % SP1K_AAA_AE_CNT_RANGE;
		}
		/*
		if(awbRdy == 0) {									   //Jim@0521 - Snap after AWB ready
			sp1kAwbScaleSet(SP1K_AWB_Jump_Scale,40);		//Jim@0527 - AWB Scale Adjust
			sp1kAwbStatusGet(SP1K_AWB_IsReady, &awbRdy);
			sp1kAwbStatusGet(SP1K_AWB_Count, &awbCnt);
			awbCnt= ( ( (UINT16)awbCnt+ SP1K_AAA_AWB_CNT_RANGE ) - awbStartCnt ) % SP1K_AAA_AWB_CNT_RANGE;
		}
		*/
		if( ( ( ( aeRdy != 0 && aeCnt > 3 ) /*&& awbRdy != 0*/) || aeCnt >= 10 ) ) {
			// to snap
			// Sometimes the video may want it is color continue between pv last frame and video 1st frame.
			// If you want that effect, pleae bypass the video here.
			//if(appCurrStateGet()== APP_STATE_STILL_VIEW) {
			//wangjg - invalid
			//sp1kAwbScaleSet(SP1K_AWB_Jump_Scale, SP1K_AWB_SPEED_FAST_ONCE ); // 20081222 wenguo, AWB jump to target//wangjg--
			//wangjg - invalid
//			sp1kAwbScaleSet(SP1K_AWB_Jump_Scale, AWB_SPEED); // 20081222 wenguo, restore PV AWB speed
			//}

			aeStartCnt = 0;
			awbStartCnt = 0;

			HAL_GlobalReadGTimer(&nowTime);

			if( nowTime - startTime >= aaaWaitReadyMinTime ) {
				waitFlag = 0;

				return SUCCESS;
			}
			else {
				waitFlag = 2;

				return FAIL;
			}
		}
		else {
			printf( "AE %bx, cnt %bu. AWB %bx, cnt %bu\n", aeRdy, aeCnt, awbRdy, awbCnt );

			return FAIL;
		}
	}
}

#endif

#if 0
UINT8
sp1kCwbSnap(
	void
)
{
	AAA_AWBCustomWbEvaluate();

	return 0;
}

#endif

