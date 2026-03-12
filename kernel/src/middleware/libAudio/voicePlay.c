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
 * @file      voiceplay.c
 * @brief     middle ware  API for audio playback module
 * @author    sun yong
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "smcfunc.h"
#include "app_ui_para.h"
#include "app_main.h"
#include "pb_api.h"
#include "initio.h"
#include "pv_task.h"
#include "sp1k_aud_api.h"
#include "app_ui_para.h"
#include "app_menu_micro.h"
#include "sp1k_rsvblk_api.h"
#include "os_msg.h"
#include "audio_rw.h"
#include "gpio.h"
#include "hal_audio.h"
#include "hal.h"
#include "hal_global.h"
#include "hal_gprm.h"
#include "hal_dram.h"
#include "sp1k_gpio_api.h"
#include "sp1k_hal_api.h"
#include "disp_api.h"
#include "reg_def.h"

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
//xdata UINT8  VoiceData = 0;
xdata UINT8 voicePlayFlag=TRUE;
xdata UINT32 voiceSize = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT16 readDramWord(UINT32 addr) ;
UINT32 readDramDWord(UINT32 addr) ;

/**
 * @fn        void sp1kAudioPlay(UINT32 audAddr)
 * @brief     sp1k Sound Play
 * @param     [in] audAddr:  sound date source address
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void sp1kAudioPlay(UINT32 audAddr )USING_0
{
	UINT32 freq;
	UINT16 adpcm;
	UINT16 aBlock;
	UINT32 size;
	UINT16 chnl;
	UINT16 smpBit;
	UINT32 addr;

	addr = audAddr;
	addr = addr + 0x14/2;
	adpcm = readDramWord(addr);
	addr = addr +1;
	chnl = readDramWord(addr);
	addr = addr +1;
	freq = readDramDWord(addr);
	addr = addr +2;
	addr = addr +2;
	aBlock = readDramWord(addr);
	addr = addr +1;
	smpBit = readDramWord(addr);
	addr = addr +1;
	addr = addr +2;
	size = readDramDWord(addr);
	addr = addr +2;
#if 0
	printf("adpcm: %d\n",adpcm);
	printf("chnl: %d\n",chnl);
	printf("freq: %ld\n",freq);
	printf("aBlock: %d\n",aBlock);
	printf("smpBit: %d\n",smpBit);
	printf("size: %ld\n",size);
#endif
	//HAL_AudStop(1); //for hdmi beep voice
#if 0  // [0034893] mattwang remove.
	//XBYTE[0x2030] |= 0x20;
	//pull high gpio5
	HAL_GpioByteOutSet(HAL_GPIO_BYTE_GEN0, 0x20, 0xFF);
	//XBYTE[0x2038] |= 0x20;
	HAL_GpioByteDirSet(HAL_GPIO_BYTE_GEN0, 0x20, 0xFF);  // set gpio[5] as output.
	//XBYTE[0x201A] &= ~0x80;
	HAL_GpioByteFuncSet(HAL_GPIO_BYTE_GEN0, 0x20, 0x00);  // set gpio[5] to func.
#endif
	//HAL_AudReset();
#if 0  // [0034893] mattwang remove.
	XBYTE[REG_Audio_AudEn] = 0x00;
	XBYTE[REG_Audio_AudRst] = 0x02;
	XBYTE[REG_Audio_AudRst] = 0x00;
	XBYTE[REG_Audio_DataThdIntEn] |= 0x02;
#endif

	if( adpcm == 0x0001 ) {
		HAL_AudAdpcmSet( 1, 0, 0);
	}
	else {
		HAL_AudAdpcmSet( (UINT32)1, (UINT32)1,	(UINT32)aBlock);
	}
	if(size%4) {
		size = ((size>>4)<<4);
	}
	HAL_AudRingBufferSet( addr<<1, (UINT32)size);
	voiceSize = size;
	//HAL_AudInIndexSet(size);
	HAL_AudThdSet( (UINT32)8/*REC_CHUNK_SIZE*/);
	HAL_AudSoundPlayCfg((UINT32)freq, (UINT32)smpBit, (UINT32)chnl );
	HAL_AudSoundPlayTrg(voiceSize);
	HAL_AudPause();

	if(pkgChipIdGet()== 0x1626)
	{
		if((0x20 != sp1kTV_PlugGet()) || (0x04 == dispDevActGet() )) {	//TV plug in or HDMI plug in
			HAL_AudSpeakerPwrSet(0);
		}
		else {
			HAL_AudSpeakerPwrSet(1);
		}
	}
	else
	{
		if((0x04 != sp1kTV_PlugGet()) || (0x04 == dispDevActGet() )) {	//TV plug in
			HAL_AudSpeakerPwrSet(0);
		}
		else {
			HAL_AudSpeakerPwrSet(1);
		}
	}

	XBYTE[REG_Audio_DataThdIntEn] |= 0x02;
	HAL_AudResume();
}

//-----------------------------------------------------------------------------
//sp1kVoiceInit
//-----------------------------------------------------------------------------
/**
 * @fn        void sp1kAudioPlayInit(void)
 * @brief     sp1kAudioPlayInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    kai.wang
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void sp1kAudioPlayInit(void)
{
	devAttr_t* pDevAttr;

	/* add for avoid audio record po-sound ,kai++ */
	XBYTE[REG_Audio_AudAdcPwrEn] |= 0x30; //MIC bias-voltage and amplifier power on
	/* add for avoid audio record po-sound ,kai-- */

	HAL_AudAdpcmSet( 1, 0, 0);
	HAL_GprmDramFill(K_SDRAM_CommonEndAddr, 12, 0);
	HAL_AudRingBufferSet( K_SDRAM_CommonEndAddr<<1, (UINT32)12);
	voiceSize = 12;
	HAL_AudSoundPlayCfg(11025,16,1);
	HAL_AudResume();
	HAL_AudSoundPlayTrg(voiceSize);
	HAL_AudStop( 0 );
	/*For HDMI beep Voice*/
	HAL_AudPause();

	sp1kNandRsvRead(0x13, (K_SDRAM_KeySoundAddr<< 1));
	/*add by phil@08.04.16 for device management*/
	pDevAttr = sp1kDevHdlGet();
	pDevAttr[DEV_ID_SPEAKER].devSts = DEV_STS_RDY;
	pDevAttr[DEV_ID_SPEAKER].devEn = 0;
}

//-----------------------------------------------------------------------------
//sp1kVoiceSet
//-----------------------------------------------------------------------------
/**
 * @fn        void sp1kAudioVolumeSet(UINT8 value)
 * @brief     volume set - each step increase 1X
 * @param     [in] value
 * @retval    NONE
 * @see       NULL
 * @author    SunYong
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
void sp1kAudioVolumeSet(UINT8 value)
{
	UINT8 VoiceData;

	if(value > 10) {
		value = 10;
	}
	switch(value)
	{
			case 0:
				VoiceData=0;
				break;
			case 1:
				VoiceData=1;
				break;
			case 2:
				VoiceData=2;
				break;
			case 3:
				VoiceData=3;
				break;
			case 4:
				VoiceData=4;
				break;
			case 5:
				VoiceData=5;
				break;
			case 6:
				VoiceData=6;
				break;
			case 7:
				VoiceData=7;
				break;
			case 8:
				VoiceData=8;
				break;
			case 9:
				VoiceData=9;
				break;
			case 10:
				VoiceData=10;
				break;
	}
	HAL_AudVolumeSet(VoiceData, 0x00 );
	//printf("0x26c7=%bx,0x26c8=%bx\n",XBYTE[0x26c7],XBYTE[0x26c8]);
}

//-----------------------------------------------------------------------------
//readDramWord
//-----------------------------------------------------------------------------
/**
 * @fn        UINT16 readDramWord(UINT32 addr)
 * @brief     readDramWord
 * @param     [in] addr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    SunYong
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT16 readDramWord(UINT32 addr) USING_0
{
	UINT8 tmp0,tmp1;
	UINT16 value;

	HAL_DramStartAddrSet(addr, 1);
	HAL_DramWordDataRead(&tmp0, &tmp1);
	value = (UINT16)tmp0 |((UINT16)tmp1<<8);

	return value;
}

//-----------------------------------------------------------------------------
//readDramDWord
//-----------------------------------------------------------------------------
/**
 * @fn        UINT32 readDramDWord(UINT32 addr)
 * @brief     readDramDWord
 * @param     [in] addr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    SunYong
 * @since     2010-5-14
 * @todo      N/A
 * @bug       N/A
*/
UINT32 readDramDWord(UINT32 addr) USING_0
{
	UINT8 tmp0,tmp1,tmp2,tmp3;
	UINT32 value;

	HAL_DramStartAddrSet(addr, 1);
	HAL_DramWordDataRead(&tmp0, &tmp1);
	HAL_DramWordDataRead(&tmp2, &tmp3);
	value = (UINT32)tmp0 |((UINT32)tmp1<<8) |((UINT32)tmp2<<16) |((UINT32)tmp3<<24);

	return value;
}

