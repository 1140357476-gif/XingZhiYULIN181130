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
 * @file      audio_alc.c
 * @brief     initial ALC parameter
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "initio.h"
#include "timer.h"
#include "common.h"
#include "doslink.h"
//#include "dosusr.h"
#include "cardlink.h"
//#include "cardfunc.h"
#include "audio_rw.h"
#include "os_api.h"
#include "os_main.h"
#include "app_main.h"
#include "sp1k_aud_api.h"
#include "hal.h"
#include "dcf_api.h" 
#include "hal_audio.h"		
#include "dbg_def.h"
#include "func_ptr.h"
#include "hal_global.h"
#include "reg_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/*for alc intial don't modify S*/  
#define ALC_LOW_THD_LSB       0x00		
#define ALC_LOW_THD_MSB       0x01 //4096

#define ALC_HIGH_THD_LSB      0xA8
#define ALC_HIGH_THD_MSB      0x61 //2500

#define ALC_WINDOW_SIZE	0x800	//16bit mode ,don't modify this value!!

//#define ALC_WINDOW_SIZE_LSB   0xff	//0xA6
//#define ALC_WINDOW_SIZE_MSB   0x07  //0x0E

#define ALC_MAX_VOL_GAP_LSB	  0x20
#define ALC_MAX_VOL_GAP_MSB   0x00

#define ALC_GAIN_UP_SPEED     0x02
#define ALC_GAIN_DOWN_SPEED   0x05

#define ALC_SILENCE_GAIN_DOWN_SPEED   0x05

#define ALC_VOLUME_UP_STEP    0x05
#define ALC_VOLUME_DOWN_STEP  0x02
/*for alc intial don't modify E*/ 

#define ALC_SATURATE_CNT_MAX  226
#define ALC_SATURATE_FRAME_CNT_MAX  100
 
#define ALC_SILENT_CNT_MAX    420
#define ALC_SILENT_CNT_MIN    350
#define ALC_SILENT_FRAME_CNT_MAX  10
#define ALC_SILENT_RECOVER_FRAME_CNT 300

#define ALC_VOLUME_TBL_MAX_INDEX 43  // for test HW
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define ALC_DEBUG
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/* Should be static variable because the variable had been linked to specified bank */

static code UINT8 alc_volume_tbl[ALC_VOLUME_TBL_MAX_INDEX]= {
	0x78, 0x74, 0x70, 0x6c	, 0x68, 0x64, 0x60	, 0x5c, 0x58, 0x54,
	0x50, 0x4c, 0x48, 0x44	, 0x40, 0x3c, 0x38	, 0x34, 0x30, 0x2c,
	0x28, 0x24, 0x20, 0x1c,  0x18, 0x14, 0x10, 0x0c, 0x08, 0x04, 
	0x00, 0x0d, 0x09, 0x05,  0x01, 0x0e, 0x0a, 0x06, 0x02, 0x0f, 
	0x0b, 0x07, 0x03
};

xdata UINT16 low_thr_frame_cnt=0, sat_thr_frame_cnt=0;
xdata UINT16 audio_curr_energy,audio_avg_energy;
xdata UINT8 alc_state = 2;
xdata SINT8 alc_gain_idx, pre_alc_gain_idx;
xdata UINT16 high_thr_cnt,silent_cnt;
xdata UINT8 init_frame_cnt;
xdata UINT8 frame_count;
xdata UINT16 hold_frame_cnt, silence_frame_cnt, silence_recover_cnt;

xdata UINT8 alc_converge_high;// =    (50);
//xdata UINT8 alc_converge_high[5] = {50, 82, 114, 146, 178};
//xdata UINT8 alc_converge_low  =    (30);
xdata UINT8 alc_converge_low  ;//=    (30);

//xdata UINT8 alc_default_gain_index =35;
xdata UINT8 alc_default_gain_index ;//= 38 ;//=32;

xdata UINT8 alc_vol_up_step  =  1;
xdata UINT8 alc_vol_down_step = 1;//modify for improving audio rec
//xdata UINT8 alc_vol_up_step  =  5;
//xdata UINT8 alc_vol_down_step = 2;//modify for improving audio rec

xdata UINT8 alc_hold_frame_cnt_max_gain_up = 10;//40
//xdata UINT8 alc_hold_frame_cnt_max_gain_up = 2;//40
//xdata UINT8 alc_silcence_frame_cnt_max_gain_down =4;

xdata UINT8 alc_hold_frame_cnt_max_gain_down = 2;
//xdata UINT8 alc_hold_frame_cnt_max_gain_down = 5;

xdata UINT8 alc_vol_min_gain_index = 0;
//xdata UINT8 alc_vol_max_gain_index = 43;
xdata UINT8 alc_vol_max_gain_index = 42;

xdata UINT8 alc_gain_down[3];//add for improving audio rec

/* add for FPGA test */
#if 0
xdata UINT8 FWGain[512];
xdata UINT8 HWGain[512]; 
#endif
xdata UINT16 G_WindowSize = ALC_WINDOW_SIZE;	//16bit mode ,don't modify this value!!
xdata UINT8 G_Sound_Ctrl;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 HAL_GlobalICVerGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
      
//-----------------------------------------------------------------------------
//audioAlcInit
//-----------------------------------------------------------------------------

/**
 * @fn        void audioAlcInit(void)
 * @brief     audio record alc initiate
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    CS
 * @since     2010-5-19
 * @todo      N/A
 * @bug       N/A
*/
void
audioAlcInit(
	void
)
{
	XBYTE[REG_Audio_VolumeIntEn] = 0x04;
	//XBYTE[REG_Audio_WindowSize0] = ALC_WINDOW_SIZE_LSB-1;
	//XBYTE[REG_Audio_WindowSize1] = ALC_WINDOW_SIZE_MSB;	
	XBYTE[REG_Audio_AgcLowThd0] = ALC_LOW_THD_LSB;
	XBYTE[REG_Audio_AgcLowThd1] = ALC_LOW_THD_MSB;
	XBYTE[REG_Audio_AgcHighThd0] = ALC_HIGH_THD_LSB;
	XBYTE[REG_Audio_AgcHighThd1] = ALC_HIGH_THD_MSB;
	HAL_AudAgcWindowSet(G_WindowSize-1);
	//XBYTE[0x26d2] = ALC_MAX_VOL_GAP_LSB;
	//XBYTE[0x26d9] = ALC_GAIN_UP_SPEED; //hold fram cnt gain up max
	//XBYTE[0x26da] = ALC_GAIN_DOWN_SPEED; //hold fram cnt gain down max
	//XBYTE[0x26db] = ALC_SILENCE_GAIN_DOWN_SPEED; //silence frame cnt  max
	//XBYTE[0x26dc] = ALC_VOLUME_UP_STEP; //gain up step
	//XBYTE[0x26dd] = ALC_VOLUME_DOWN_STEP; //gain down step	
	HAL_AudSwRst(0x10);	//agc function swrs
	//XBYTE[REG_Audio_AmplifierGain] = alc_volume_tbl[alc_default_gain_index];
	alc_gain_idx = alc_default_gain_index;
	pre_alc_gain_idx = alc_default_gain_index;
	audio_curr_energy = 0;
	audio_avg_energy = 40;//60;
	low_thr_frame_cnt = 0;
	sat_thr_frame_cnt = 0;
	init_frame_cnt = 0;
	high_thr_cnt = 0;
	silent_cnt = 0;
	alc_state = 2;
	hold_frame_cnt = 0;
	silence_frame_cnt = 0;
	silence_recover_cnt = 0;
	if(HAL_GlobalICVerGet() == 0x00){	
		HAL_AudALCModeSet(0);//disable HW ALC
	}
	else{
		HAL_AudALCModeSet(1);//HW ALC
	}
	//XBYTE[REG_Audio_AgcFuncEN] = 0x0f;  //enable FPGA test mode
	//add for improving audio rec		
	alc_gain_down[0]=alc_vol_down_step;
	alc_gain_down[1]=alc_vol_down_step<<1;
	alc_gain_down[2]=(alc_vol_down_step<<1)+alc_vol_down_step;
	//alc_gain_down[3]=alc_vol_down_step<<2;
	//alc_gain_down[4]=(alc_vol_down_step<<2)+alc_vol_down_step;
	//add for improving audio rec
}

//-----------------------------------------------------------------------------
//audioAlcCal_Intr
//-----------------------------------------------------------------------------
/**
 * @brief	audio alc calculate and gain set
 * @param	NONE
 * @retval	NONE
 * @todo
 * @author	CS
 * @since	2008-7-26
 */
void
audioAlcCal_Intr(
	void
)
{
       UINT16 temp;//add for audio rec
	//alc_state = 0xff;
	//CPrintf1("Idx=%bd",alc_gain_idx);
	
	if (!G_Sound_Ctrl){ //for sound control
		
		switch (alc_state) {
			case 0:
				break;
			case 1: // normal process
				pre_alc_gain_idx = alc_gain_idx;
				//printf("1idx = %bu\n", alc_gain_idx);
			
				//if(high_thr_cnt > ALC_SATURATE_CNT_MAX) {
				//	sat_thr_frame_cnt++;
				//}
				//else {
				//	sat_thr_frame_cnt = 0;
				//};	

				//if(sat_thr_frame_cnt > ALC_SATURATE_FRAME_CNT_MAX) {
				//	alc_gain_idx -= ALC_VOLUME_DOWN_STEP;
				//	hold_frame_cnt = 0;
				//}
				//else {
					if(audio_curr_energy > alc_converge_high) {
						hold_frame_cnt++;
						if(hold_frame_cnt >	alc_hold_frame_cnt_max_gain_down) {
						  //modify for improving audio rec
							temp=(audio_curr_energy-alc_converge_high)>>3;
							if(temp>2)
								temp=2;
							//printf("step=%bu\n",alc_gain_down[temp]);	
							alc_gain_idx-=alc_gain_down[temp];
							//alc_gain_idx -= ALC_VOLUME_DOWN_STEP;
							//end modify for improving audio rec 
							//hold_frame_cnt = alc_hold_frame_cnt_max_gain_down+1;
							hold_frame_cnt = 0;
							//printf("Aidx = %bu\n", alc_gain_idx);

						};
					}
					else if(audio_curr_energy < alc_converge_low) {
						hold_frame_cnt++;
						if(hold_frame_cnt >	alc_hold_frame_cnt_max_gain_up) {
							alc_gain_idx += alc_vol_up_step;
							//hold_frame_cnt = alc_hold_frame_cnt_max_gain_up+1;
							hold_frame_cnt = 0;	
							//printf("Bidx = %bu\n", alc_gain_idx);
						};
					}
					else {
						hold_frame_cnt = 0;
					};
				//}
					
				//printf("I%bd\n",alc_gain_idx);
				//printf("A%u\n",audio_curr_energy); 
				audio_avg_energy = (audio_avg_energy*31 + audio_curr_energy)/32;
				if((alc_gain_idx == alc_vol_max_gain_index) && (audio_avg_energy < (alc_converge_low>>1))) {
					//silent_cnt++;
					//if(silent_cnt > ALC_SILENT_FRAME_CNT_MAX) 
					alc_state = 2;
					//printf("Cidx = %bu\n", alc_gain_idx);
				}
				else {
					silent_cnt = 0;
				};
				
				if(alc_gain_idx < alc_vol_min_gain_index) {
					alc_gain_idx = alc_vol_min_gain_index;
					//printf("Didx = %bu\n", alc_gain_idx);
				}	

				if(alc_gain_idx > alc_vol_max_gain_index) {
					alc_gain_idx = alc_vol_max_gain_index;
				//	if((alc_gain_idx == pre_alc_gain_idx) && (silent_cnt > ALC_SILENT_CNT_MAX)){
				//		low_thr_frame_cnt++;
				//	}
				//	else {
				//		low_thr_frame_cnt = 0;
				//	};
					//printf("Eidx = %bu\n", alc_gain_idx);
				}
				//else {
				//	low_thr_frame_cnt = 0;
				//};

						
				//if(low_thr_frame_cnt > ALC_SILENT_FRAME_CNT_MAX) {
				//	alc_state = 2;
				//	break;
				//};

				if(alc_gain_idx != pre_alc_gain_idx) {
					//printf("Fidx = %bu\n", alc_gain_idx);
					XBYTE[REG_Audio_AmplifierGain] = alc_volume_tbl[alc_gain_idx];
				}

				break;
			case 2: //silence process
				//printf("2idx = %bu\n", alc_gain_idx);
				silence_frame_cnt++;
				if(silence_frame_cnt > alc_hold_frame_cnt_max_gain_up) {
					pre_alc_gain_idx = alc_gain_idx;
					alc_gain_idx -= alc_vol_down_step;
					//printf("Gidx = %bu\n", alc_gain_idx);
					if(alc_gain_idx < alc_default_gain_index) {
						alc_gain_idx = alc_default_gain_index;
						//printf("Hidx = %bu\n", alc_gain_idx);
					}
					silence_frame_cnt = 0;
					//printf("Iidx = %bu\n", alc_gain_idx);
					XBYTE[REG_Audio_AmplifierGain] = alc_volume_tbl[alc_gain_idx];
				};
				//printf("conhi=%bu,conlo=%bu\n",alc_converge_high,alc_converge_low);
				if(audio_curr_energy > (alc_converge_high)) {
						alc_state = 1;
						audio_avg_energy = 40;
						silence_frame_cnt = 0;
						hold_frame_cnt = 0;
	                  //add for improving audio rec
						temp=(audio_curr_energy-alc_converge_high)>>3;
						if(temp>2)
							temp=2;
						alc_gain_idx-=alc_gain_down[temp];
						//printf("Jidx = %bu\n", alc_gain_idx);
							XBYTE[REG_Audio_AmplifierGain] = alc_volume_tbl[alc_gain_idx];
	                  //add end
				}	

				break;
			default:break;
		};
	}
	//printf("REG_Audio_AmplifierGain=%bx\n",XBYTE[REG_Audio_AmplifierGain]);
	#ifdef ALC_DEBUG
	//	CPrintf2("I%bd,%u",alc_gain_idx,audio_curr_energy);
	//	printf("A%u\n",audio_avg_energy);
	//	printf("alc_idx = %bd,alc_state = %bu\n", alc_gain_idx, alc_state);
	//	printf("cur_energy = %u\n",audio_curr_energy);
	//	printf("%u\n",audio_curr_energy);
	//	printf("high_cnt = %u, silent_cnt = %u\n",high_thr_cnt, silent_cnt);
	//	printf("init_frame_cnt = %bu\n",init_frame_cnt);
	//	printf("low_thr_frame_cnt = %bu\n",low_thr_frame_cnt);
	#endif	
}

void soundctl (void)
{
	UINT32 outIdx;
	HAL_AudInIndexGet(&outIdx);
		if(outIdx<0x200)
			outIdx= 0;
		outIdx = outIdx-0x200  ;
		HAL_AudOutIndexSet( (UINT32)outIdx);

}

static void audioSoundCtlSet(UINT8 en)
{
	//sp1kAudParamSet(SP1K_AUD_PARAM_ALC_GAIN_SET,alc_default_gain_index);
	XBYTE[REG_Audio_AmplifierGain] = alc_volume_tbl[alc_default_gain_index];
	if(en) {
		HAL_AudAdpcmSet( (UINT32)AUD_RECORD_MODE, (UINT32)0, (UINT32)0);
		HAL_AudRingBufferSet((UINT32)(K_SDRAM_SoundCtrlAddr<<1), (UINT32)K_SDRAM_SoundCtrlSize<<1);
		HAL_AudOutIndexSet(0);
		HAL_AudPause();
		HAL_AudStart( 0, 11025 , 16, 1 );
		HAL_GlobalTimerWait(150);	//for avoid snap key pluse!
		HAL_AudResume();
		osActiveFuncSet(pFunc_SoundCtrl);
	}
	else{
		//audRecCtrl.recStatus = AUD_REC_IDLE;
		ACTIVE_FUNC_CLR();
		HAL_AudStop(0);

	}
}

UINT8 sp1kAudParamSet(
	UINT8 attrId,
	UINT8 value
)
{
	UINT8 temp;
	temp = attrId;
	if ( attrId > SP1K_AUD_PARAM_ALC_MAX ) {
		return FAIL;
	};

	switch (attrId) {	
		case SP1K_AUD_PARAM_ALC_OPT:
			if (value == 1) {
				HAL_AudAgcCfg(K_SDRAM_AlcMoeExtraBufAddr, G_WindowSize, K_SDRAM_AlcMoeCommandBufAddr, HAL_AUD_SAMPLE_16BIT);
				audioAlcInit();
			}
			else {
				XBYTE[REG_Audio_AgcFuncEN] = 0x00;
				XBYTE[REG_Audio_VolumeIntEn] = 0x00;
				alc_state = 0xff;
			};
			break;
		case SP1K_AUD_PARAM_ALC_GAIN_SET:
			if(value >= ALC_VOLUME_TBL_MAX_INDEX) value = ALC_VOLUME_TBL_MAX_INDEX-1;
			//XBYTE[REG_Audio_AgcFuncEN] = 0x00;
			XBYTE[REG_Audio_AmplifierGain] = alc_volume_tbl[value];
			break;
		case SP1K_AUD_PARAM_ALC_MAX_GAIN_IDX:
			if (value < ALC_VOLUME_TBL_MAX_INDEX) {
				alc_vol_max_gain_index = value;
			}
			else {
				alc_vol_max_gain_index = ALC_VOLUME_TBL_MAX_INDEX - 1;
			}
			break;
		case SP1K_AUD_PARAM_ALC_MIN_GAIN_IDX:
			if (value >= 0) {
				alc_vol_min_gain_index = value;
			}
			else {
				alc_vol_min_gain_index = 0;
			};
			break;
		case SP1K_AUD_PARAM_ALC_MAX_VOL:
			alc_converge_high = value;
			break;
		case SP1K_AUD_PARAM_ALC_MIN_VOL:
			alc_converge_low = value;
			break;
		case SP1K_AUD_PARAM_ALC_DEFAULT_GAIN_IDX:
			alc_default_gain_index = value;
			break;
		case SP1K_AUD_PARAM_ALC_GAIN_UP_SPEED:
			alc_hold_frame_cnt_max_gain_up = value;
			break;
		case SP1K_AUD_PARAM_ALC_GAIN_DOWN_SPEED:
			alc_hold_frame_cnt_max_gain_down = value;
			break;
		case SP1K_AUD_PARAM_ALC_DIGITAL_VOL:
			HAL_AudVolumeSet(value, 0x00 );
			break;
		case SP1K_AUD_PARAM_ALC_SOUND_CTRL:
			G_Sound_Ctrl = value;
			audioSoundCtlSet(G_Sound_Ctrl);			
			break;
		default:break;
	};
	//value = value;
	return SUCCESS;
}

UINT8 sp1kAudParamGet(
	UINT8 attrId
)
{
	UINT8 temp;
	temp = attrId;

	if ( attrId > SP1K_AUD_PARAM_ALC_MAX ) {
		return FAIL;
	};

	switch (attrId) {
		case SP1K_AUD_PARAM_ALC_OPT:
			if(alc_state == 0xff) {
				return 0;
			}
			else {
				return 1;
			}		
			break;
		case SP1K_AUD_PARAM_ALC_SOUND_CTRL:
			return G_Sound_Ctrl;
		default:break;
	};
			
	return 0xff;
}

UINT16 sp1kAudioAgcValGet(
	void
)
{
	return audio_curr_energy;
}
