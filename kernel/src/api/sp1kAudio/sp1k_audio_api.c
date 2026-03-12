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
 * @file		sp1k_audio_api.c
 * @brief		
 * @author		Phil Lin
 * @since		2009-11-25
 * @date		2009-11-25
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
 #include "general.h"
 #include "sp1k_aud_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern xdata UINT8 alc_state;

extern void audioAlcInit();

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
//-----------------------------------------------------------------------------
// sp1kAudParamSet
//-----------------------------------------------------------------------------
#if TAG_TODO
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
				audioAlcInit();
			};
			if (value == 0) {
				XBYTE[REG_Audio_AgcFuncEN] = 0x00;
				XBYTE[REG_Audio_VolumeIntEn] = 0x00;
				alc_state = 0xff;
			};
			break;
		case SP1K_AUD_PARAM_ALC_GAIN_SET:
			if(value >= ALC_VOLUME_TBL_MAX_INDEX) value = ALC_VOLUME_TBL_MAX_INDEX;
			XBYTE[REG_Audio_AgcFuncEN] = 0x00;
			XBYTE[REG_Audio_AmplifierGain] = alc_volume_tbl[value-1];
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
			alc_hold_fram_cnt_max_gain_up = value;
			break;
		case SP1K_AUD_PARAM_ALC_GAIN_DOWN_SPEED:
			alc_hold_frame_cnt_max_gain_down = value;
			break;
		case SP1K_AUD_PARAM_ALC_DIGITAL_VOL:
			HAL_AudVolumeSet(value, 6 );
			break;
		default:break;
	};
//TAG_TODO	
	value = value;
		
	return 0;
}
#endif 
#if TAG_TODO
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
		default:break;
	};
			
	return 0xff;
}
#endif //TAG_TODO