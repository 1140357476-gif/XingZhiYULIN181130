/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/

/**
 * @file		hal_audio.c
 * @brief		Hal API for AUDIO module
 * @author		kai.wang
 * @since		2010-01-01
 * @date		2010-04-21
 */

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "hal.h"
#include "hal_device.h"
#include "hal_audio.h"
#include "hal_global.h"
#include "hal_moe.h"
#include "asicreg.h"
#include "initio.h"
#include "reg_def.h"
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
#define FOR_ANY_SMAPLERATE
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
static UINT8 S_0x2640;
static UINT8 S_0x2648;
static UINT8 S_0x2670;
static UINT8 S_0x2677;
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
xdata UINT32 ringBufSize;
moe_queue_t alcQueue[2] _at_ 0xbfc0;//48byte

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern UINT8 HAL_GlobalICVerGet(void);
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

//-----------------------------------------------------------------------------
//hwAudStart
//-----------------------------------------------------------------------------

/**
 * @fn        UINT32 HAL_AudStart(UINT32 mode, UINT32 smpRate, UINT32 smpBit, UINT32 chn)
 * @brief     start audio record/play
 * @param     [in] mode :1 play; 0: record
 * @param     [in] smpRate
 * @param     [in] smpBit
 * @param     [in] chn: 1: mono ; 2 : stereo
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Kai.wang
 * @since     2010-5-5
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_AudStart(
	UINT32 mode,
	UINT32 smpRate,
	UINT32 smpBit,
	UINT32 chn
)
{
	UINT8 PLL_Sel;
	UINT8 audclkdiv;

	XBYTE[0x2014] |= 0xcc;	//audio/audio adc/dac i2s clock enable
	XBYTE[0x2015] |= 0x01;	//auido mclk enable
	XBYTE[0x2016] |= 0x02;	//audio 48m clock enable
	XBYTE[REG_Audio_VrefPwrEn] |= 0x05;	//reference voltage

	/* Set audio Record and Play mode  */
	if( mode == 1 ) { /* audio play. */
		XBYTE[REG_Audio_I2SBClkSrc] = 0x00;		//I2S bit clock source ,playback mode
		XBYTE[REG_Audio_AudDACPwrEn] = 0x21;		//audio DAC/SPEAKER/LINEOUT power on
		XBYTE[REG_Audio_AudAdcPwrEn] &= ~0x01;		//audio ADC power down
		
		XBYTE[REG_Audio_AudStereoMode] &= ~0x30;	// 8-bit, mono.
		if( smpBit == 16 ) {
			XBYTE[REG_Audio_AudStereoMode] |= 0x20;	// 16-bit.
		}
		if( chn == 2 ) {
			XBYTE[REG_Audio_AudStereoMode] |= 0x10;	// stereo.
		}
		//XBYTE[0x268c] |= 0x04;	//swap playback LR channel
	}  
	else {/* audio record. */
		XBYTE[REG_Audio_I2SBClkSrc] = 0x01;		//I2S bit clock source ,record mode
		XBYTE[REG_Audio_AudAdcPwrEn] |= 0x01;		//audio ADC/MIC/MIC amp power on
		XBYTE[REG_Audio_AudDACPwrEn] = 0x00;		//audio DAC/SPEAKER/LINEOUT power down

		HAL_AudSwRst(0xBF);

		XBYTE[REG_Audio_AudStereoMode] &= ~0x03;	// 8-bit, mono.
		if( smpBit == 16 ) {
			XBYTE[REG_Audio_AudStereoMode] |= 0x02;	// 16-bit.
		}
		if( chn == 2 ) {
			XBYTE[REG_Audio_AudStereoMode] |= 0x01;	// stereo.
		}
	}

	/* Set audio clock: (Sampling Rate) & PLL & Mode */
	switch(smpRate) {
		//73.728MHZ
		case 48000:
			//printf("	Sampling Rate: 48000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x05;
			break;

		case 32000:
			//printf("	Sampling Rate: 32000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x08;
			break;

		case 24000:
			//printf("	Sampling Rate: 24000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x0b;
			break;

		case 16000:
			//printf("	Sampling Rate: 16000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x11;
			break;

		case 12000:
			//printf("	Sampling Rate: 12000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x17;
			break;

		case 8000:
			//printf("	Sampling Rate: 8000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x23;
			break;
		//67.737MHZ
		case 44100:
			//printf("	Sampling Rate: 44100\n");
			PLL_Sel = 0x01;
			audclkdiv	= 0x05;
			break;

		case 22050:
			//printf("	Sampling Rate: 22050\n");
			PLL_Sel = 0x01;
			audclkdiv	= 0x0B;
			break;

		case 11025:
			//printf("	Sampling Rate: 11025\n");
			PLL_Sel = 0x01;
			audclkdiv	= 0x17;
			break;

		default:
			//printf("	Invalid Sampling Rate\n");
			/* default setting  44.1KHZ*/
			PLL_Sel = 0x01;
			audclkdiv	= 0x05;
			break;
	}

	/* Select audio PLL 0: 67.737MHz  1: 73.728MHz */
	XBYTE[0x2082] = (XBYTE[0x2082] & 0x03)|(PLL_Sel<<1);

	/* Set audio clock divider */
	XBYTE[0x202a] = audclkdiv;

	//if( mode == 0 ) { /* record */
		
		//printf("IIS shift: 0 \n");
		//XBYTE[REG_Audio_I2SRecMode]=0x00;

		//printf("Turn on boost \n");
		//HAL_AudPGAGainSet(0x00,0x02);
	//}
		 
	XBYTE[REG_Audio_DataThdIntSts] = 0x03;		//clear audio data over/under threshold interrupt

	//XBYTE[REG_Audio_I2SRecMode] = 0x01;
	//HAL_GlobalTimerWait(180); //for avoid snap key pluse!

	if( mode == 0 ) {
		XBYTE[REG_Audio_I2SRecMode]=0x00;
		XBYTE[REG_Auido_AudDramMode] = 0x07;		// drammode = 0
		
		XBYTE[REG_Audio_AudEn] = 0x01;
	}
	else {
		HAL_AudPGAGainSet(0x00,0x03);
		XBYTE[REG_Auido_AudDramMode] = 0x04;		// drammode = 0;audio play stop when outidx = inidx
		
		XBYTE[REG_Audio_AudEn] = 0x02;
	}
}

/**
 * @fn        UINT32 HAL_AudSoundPlayCfg(UINT32 smpRate, UINT32 smpBit, UINT32 chn)
 * @brief     config audio sound parameter
 * @param     [in] smpRate
 * @param     [in] smpBit
 * @param     [in] chn
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Kai.wang
 * @since     2010-5-5
 * @todo      N/A
 * @bug       N/A
*/
void
HAL_AudSoundPlayCfg(
	UINT32 smpRate,
	UINT32 smpBit,
	UINT32 chn
)
{
	UINT8 PLL_Sel;
	UINT8 audclkdiv;

	XBYTE[0x2014] |= 0xcc;	//audio/audio adc/dac i2s clock enable
	XBYTE[0x2015] |= 0x01;
	XBYTE[0x2016] |= 0X02;

	XBYTE[REG_Audio_VrefPwrEn] = 0x05;
	XBYTE[REG_Audio_I2SBClkSrc] = 0x00;
	XBYTE[REG_Audio_AudDACPwrEn] = 0x21;
	XBYTE[REG_Audio_AudStereoMode] &= ~0x30;  // 8-bit, mono.
	if( smpBit == 16 ) {
		XBYTE[REG_Audio_AudStereoMode] |= 0x20;	// 16-bit.
	}
	if( chn == 2 ) {
		XBYTE[REG_Audio_AudStereoMode] |= 0x10;	// stereo.
	}

	/* Set audio clock: (Sampling Rate) & PLL & Mode */
	switch(smpRate) {
		//73.728MHZ
		case 48000:
			//printf("	Sampling Rate: 48000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x05;
			break;

		case 32000:
			//printf("	Sampling Rate: 32000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x08;
			break;

		case 24000:
			//printf("	Sampling Rate: 24000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x0b;
			break;

		case 16000:
			//printf("	Sampling Rate: 16000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x11;
			break;

		case 12000:
			//printf("	Sampling Rate: 12000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x17;
			break;

		case 8000:
			//printf("	Sampling Rate: 8000\n");
			PLL_Sel = 0x00;
			audclkdiv	= 0x23;
			break;

		//67.737MHZ
		case 44100:
			//printf("	Sampling Rate: 44100\n");
			PLL_Sel = 0x01;
			audclkdiv	= 0x05;
			break;

		case 22050:
			//printf("	Sampling Rate: 22050\n");
			PLL_Sel = 0x01;
			audclkdiv	= 0x0B;
			break;

		case 11025:
			//printf("	Sampling Rate: 11025\n");
			PLL_Sel = 0x01;
			audclkdiv	= 0x17;
			break;

		default:
			//printf("	Invalid Sampling Rate\n");
			/* default setting */
			PLL_Sel = 0x01;
			audclkdiv	= 0x05;
			break;
	}

	/* Select audio PLL 0: 73.728MHz  1: 67.737MHz */
	XBYTE[0x2082] = (XBYTE[0x2082] & 0x03)|(PLL_Sel<<1);

	/* Set audio clock divider */
	XBYTE[0x202a] = audclkdiv;
	
	XBYTE[REG_Audio_AudEn] |= 0x02;	// enable audio play.

#if 0
	HAL_AudReset();
	HAL_AudInIndexSet(inIdx);
//	HAL_AudOutIndexSet(0);

#endif
}

/**
 * @fn		  void HAL_AudSoundPlayTrg(void)
 * @brief	  trig audio sound play
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudSoundPlayTrg(
	UINT32 index
)
{
	XBYTE[REG_Audio_DataThdIntSts] = 0x03;

	XBYTE[REG_Auido_AudDramMode] = 0x04;						// drammode = 0

//	XBYTE[REG_Audio_DataThdIntSts] = 0x3;						/* clear more/less interrupt status 	   */
//	XBYTE[0x26f0] = 0x00;
	HAL_AudInIndexSet(index);	
}

/**
 * @fn		  void HAL_AudStop(UINT32 mode)
 * @brief	  stop audio rec/play
 * @param	  [in] mode
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudStop(
	UINT32 mode
)
{
	if( mode == 0 ) {  // stop audio record.
	//if( sp1kAudParamGet(SP1K_AUD_PARAM_ALC_OPT) == 1 ) {
	//		XBYTE[REG_Audio_AgcFuncEN] = 0x00;

	//		XBYTE[REG_Audio_VolumeIntEn] = 0x00;
	//}

		XBYTE[REG_Audio_AudEn] &= ~0x01;
	}
	else {	// stop audio play.
		XBYTE[REG_Audio_AudEn] &= ~0x02;
		XBYTE[REG_Audio_DataThdIntSts] |= 0x02;
		XBYTE[REG_Audio_AudDACPwrEn] = 0x00;		//audio DAC/SPEAKER/LINEOUT power down
		XBYTE[REG_Audio_AudEn] &= ~0x02;
	}
}

/**
 * @fn		  void HAL_AudPause(void)
 * @brief	  pause audio rec/play
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudPause(
	void
)
{
	XBYTE[REG_Audio_AudPause] = 0x01;
	if(XBYTE[0x20ff] == 0x00) {//mantis #43386
		XBYTE[REG_Audio_RecLRSwap] = 0x01;		//when record pause, this bit must be set
	}	
}

/**
 * @fn		  void HAL_AudResume(void)
 * @brief	  resume audio rec/play
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudResume(
	void
)
{
	XBYTE[REG_Audio_AudPause] = 0x00;
}

/**
 * @fn		  void HAL_AudThdSet(UINT32 thd)
 * @brief	  set the threshold value of audio data number in the audio buffer
 * @param	  [in] thd
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudThdSet(
	UINT32 thd
)
{
	thd = thd >> 1;

	XBYTE[REG_Audio_AudDataCnthd0]= *((UINT8*)&thd+3    );
	XBYTE[REG_Audio_AudDataCnthd1]= *((UINT8*)&thd+2    );
	XBYTE[REG_Audio_AudDataCnthd2]= *((UINT8*)&thd+1    );
}

/**
 * @fn		  void HAL_AudRingBufferSet(UINT32 sdramAddr, UINT32 sizeByte)
 * @brief	  set the audio ring buffer
 * @param	  [in] sdramAddrByte: Audio ring buffer starting address
 * @param	  [in] sizeByte: The allocation size of the audio buffer in Dram,
				the maximum size is 1M word
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_AudRingBufferSet(
	UINT32 sdramAddrByte, // byte address
	UINT32 sizeByte
)
{
	if (sizeByte > 0x200000) { //max size is 1M word
		return;
	}
	/* ring buffer start address		  */
	XBYTE[REG_Audio_StartAddr0] = (sdramAddrByte >> 1 ) & 0xFF;
	XBYTE[REG_Audio_StartAddr1] = (sdramAddrByte >> 9 ) & 0xFF;
	XBYTE[REG_Audio_StartAddr2] = (sdramAddrByte >> 17) & 0xFF;
	XBYTE[REG_Audio_StartAddr3] = (sdramAddrByte >> 25) & 0xFF;

	ringBufSize = (UINT32)sizeByte;
	//printf("here init=%ld\n",sizeByte);
	/* ring buffer size */
	XBYTE[REG_Audio_BufSize0] = (((sizeByte/2)-1) >>0 )& 0xFF;
	XBYTE[REG_Audio_BufSize1] = (((sizeByte/2)-1) >>8 )& 0xFF;
	XBYTE[REG_Audio_BufSize2] = (((sizeByte/2)-1) >>16)& 0xFF;

	/* reset ring buffer */
	HAL_AudInIndexSet( 0 );
	HAL_AudOutIndexSet( 0 );
}

/**
 * @fn		  void HAL_AudInIndexSet(UINT32 index)
 * @brief	  set the audio ring buffer input index
 * @param	  [in] index: 
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudInIndexSet(
	UINT32 index
)
{
	/* ring buffer input index */
	XBYTE[REG_Audio_RingBufInIdx2] = ((index/2) >> 16) & 0xFF;
	XBYTE[REG_Audio_RingBufInIdx1] = ((index/2) >>  8) & 0xFF;
	XBYTE[REG_Audio_RingBufInIdx0] = ((index/2) >>  0) & 0xFF;

	XBYTE[REG_Audio_DataThdIntSts] = 0x02;
}

/**
 * @fn		  void HAL_AudOutIndexSet(UINT32 index)
 * @brief	  set the audio ring buffer output index
 * @param	  [in] index
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudOutIndexSet(
	UINT32 index
)
{
	/* ring buffer input index */
	XBYTE[REG_Audio_RingBufOutIdx2] = ((index/2) >> 16) & 0xFF;
	XBYTE[REG_Audio_RingBufOutIdx1] = ((index/2) >>  8) & 0xFF;
	XBYTE[REG_Audio_RingBufOutIdx0] = ((index/2) >>  0) & 0xFF;

	XBYTE[REG_Audio_DataThdIntSts] = 0x01;
}

/**
 * @fn		  UINT8 HAL_AudIntStsGet(UINT16 mode)
 * @brief	  get the interrupter state when audio data over/under threshold
 * @param	  [in] mode:
				0: over threshold interrupt
				1: under threshold interrupt
 * @retval	  return the interrupt state
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
HAL_AudIntStsGet(
	UINT16 mode
)
{
	UINT8 sts;

	if( mode == 0 ) {
		sts = XBYTE[REG_Audio_DataThdIntSts] & 0x01;
	}
	else {
		sts = ( XBYTE[REG_Audio_DataThdIntSts] >> 1 ) & 0x01;
	}

	return sts;
}

/**
 * @fn		  void HAL_AudVolumeSet(UINT8 volnum_integer, UINT8 voldenum_float)
 * @brief	  set the digital gain of both audio playback/recording
 * @param	  [in] volnum_integer 
 * @param	  [in] voldenum_decimal: decimal=XBYTE[0x26c7]/256
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudVolumeSet(
	UINT8 volnum_integer,
	UINT8 voldenum_decimal
)
{
	XBYTE[REG_Audio_DigtalGain1] = volnum_integer;
	XBYTE[REG_Audio_DigtalGain0] = voldenum_decimal;
}

/**
 * @fn		  void HAL_AudInIndexGet(UINT32 *index)
 * @brief	  get audio ring buffer index
 * @param	  [in] *index
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudInIndexGet(
	UINT32 *index
)
{
//	UINT32 temp = 0xffffffff;
	if(XBYTE[0x20ff] == 0x00) {
		XBYTE[0x2021] = 0x1f;
		XBYTE[REG_Audio_SnapRingIdx] = 1;
		XBYTE[0x2021] = 0x07;
	}
	else {
		XBYTE[REG_Audio_SnapRingIdx] = 1;
	}
	*((UINT8*)index  ) = 0;
	*((UINT8*)index+1) = XBYTE[REG_Audio_RingBufInIdx2];
	*((UINT8*)index+2) = XBYTE[REG_Audio_RingBufInIdx1];
	*((UINT8*)index+3) = XBYTE[REG_Audio_RingBufInIdx0];
/*
	while( temp != *index ) {
		temp = *index;

	*((UINT8*)index  ) = 0;
	*((UINT8*)index+1) = XBYTE[REG_Audio_RingBufInIdx2];
	*((UINT8*)index+2) = XBYTE[REG_Audio_RingBufInIdx1];
	*((UINT8*)index+3) = XBYTE[REG_Audio_RingBufInIdx0];
	}
*/
	*index = *index << 1;
}

/**
 * @fn		  void HAL_AudOutIndexGet(UINT32 *index)
 * @brief	  get audio ring buffer index
 * @param	  [in] *index
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudOutIndexGet(
	UINT32 *index
)
{
//	UINT32 temp = 0xffffffff;
	if(XBYTE[0x20ff] == 0x00) {
		XBYTE[0x2021] = 0x1f;
		XBYTE[REG_Audio_SnapRingIdx] = 2;
		XBYTE[0x2021] = 0x07;
	}
	else {
		XBYTE[REG_Audio_SnapRingIdx] = 2;
	}

	*((UINT8*)index  ) = 0;
	*((UINT8*)index+1) = XBYTE[REG_Audio_RingBufOutIdx2];
	*((UINT8*)index+2) = XBYTE[REG_Audio_RingBufOutIdx1];
	*((UINT8*)index+3) = XBYTE[REG_Audio_RingBufOutIdx0];

/*
	while( temp != *index ) {
		temp = *index;

	*((UINT8*)index  ) = 0;
	*((UINT8*)index+1) = XBYTE[REG_Audio_RingBufOutIdx2];
	*((UINT8*)index+2) = XBYTE[REG_Audio_RingBufOutIdx1];
	*((UINT8*)index+3) = XBYTE[REG_Audio_RingBufOutIdx0];
	}
*/
	*index = *index << 1;
}

/**
 * @fn		  UINT8 HAL_AudRingBufCntRead(UINT32 mode, UINT32 *pAudBufCnt)
 * @brief	  read the accumulated audio data size(words)in the SDram audio buffer
 * @param	  [in] pAudBufCnt
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
HAL_AudRingBufCntRead(
	UINT32 *pAudBufCnt
)
{
	UINT32 inIdx;
	

	*((UINT8*)&inIdx  ) = 0;
	*((UINT8*)&inIdx+1) = XBYTE[REG_Audio_DataCnt2];
	*((UINT8*)&inIdx+2) = XBYTE[REG_Audio_DataCnt1];
	*((UINT8*)&inIdx+3) = XBYTE[REG_Audio_DataCnt0];
	
	*pAudBufCnt = (inIdx << 1) % ringBufSize;

	//printf("a:%lx\n",inIdx );
	return SUCCESS;  

}

/**
 * @fn		  void HAL_AudAdpcmSet(UINT32 mode, UINT32 on, UINT32 blockSize)
 * @brief	  set audio Adpcm encode/decode parameter
 * @param	  [in] mode: 
 *				0: audio record
 *				1: audio play
 * @param	  [in] on: 
 *				0: PCM 
 *				1: ADPCM
 * @param	  [in] blockSize: 
 *				 256 byte
 *				 512 byte
 *				 1024 byte
 *				 2048 byte
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudAdpcmSet(
	UINT8 mode,
	UINT8 on,
	UINT32 blockSize
)
{
	if( mode == 0 ) {  // audio record.
		XBYTE[REG_Audio_AdpcmCoderEn] = on;
	}
	else {	// audio play.
		XBYTE[REG_Audio_AdpcmCoderEn] = on << 1;
	}

	if( blockSize == (UINT32) 256 ) {
		XBYTE[REG_Audio_PageMode] = 0;
	}
	else if(blockSize == (UINT32) 512 ) {
		XBYTE[REG_Audio_PageMode] = 1;
	}
	else if(blockSize == (UINT32) 1024 ) {
		XBYTE[REG_Audio_PageMode] = 2;
	}
	else if(blockSize == (UINT32) 2048 ) {
		XBYTE[REG_Audio_PageMode] = 3;
	}
	else {
		XBYTE[REG_Audio_PageMode] = 0;
	}
}

/**
 * @fn		  void HAL_AudAgcWindowSet(UINT16 window)
 * @brief	  set agc window
 * @param	  [in] window
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudAgcWindowSet(
	UINT16 window
)
{
	XBYTE[REG_Audio_WindowSize1] = *((UINT8*)&window);
	XBYTE[REG_Audio_WindowSize0] = *((UINT8*)&window + 1 );

//	printf( "win %d, REG_Audio_WindowSize0 [%bx][%bx]\n", window, XBYTE[REG_Audio_WindowSize0], XBYTE[REG_Audio_WindowSize1] );
}

/**
 * @fn		  void HAL_AudAgcStateGet(UINT32 *eng, UINT32 *window, UINT8 *cnt)
 * @brief	  get alc parameter 
 * @param	  [in] *eng: agc value 
 * @param	  [in] *window: vindow size
 * @param	  [in] *cnt: audio window count
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudAgcStateGet(
	UINT32 *eng,
	UINT32 *window,
	UINT8 *cnt
)
{
	*((UINT8*)eng  ) = XBYTE[REG_Audio_AgcValue3];
	*((UINT8*)eng+1) = XBYTE[REG_Audio_AgcValue2];
	*((UINT8*)eng+2) = XBYTE[REG_Audio_AgcValue1];
	*((UINT8*)eng+3) = XBYTE[REG_Audio_AgcValue0];

	*cnt = XBYTE[REG_Audio_FrameCnt];

	*((UINT8*)window  ) = 0;
	*((UINT8*)window+1) = 0;
	*((UINT8*)window+2) = XBYTE[REG_Audio_WindowSize1];
	*((UINT8*)window+3) = XBYTE[REG_Audio_WindowSize0];
}

/**
 * @fn		  void HAL_AudPGAGainGet(UINT32 *gain, UINT32 *boost)
 * @brief	  get boost gain and programmable gain
 * @param	  [in] gain
 * @param	  [in] boost
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudPGAGainGet(
	UINT8 *gain,
	UINT8 *boost
)
{
	*gain  =  XBYTE[REG_Audio_AmplifierGain] >> 2;
	*boost =  XBYTE[REG_Audio_AmplifierGain] & 0x03;
}

/**
 * @fn		  void HAL_AudPGAGainSet(UINT32 gain, UINT32 boost)
 * @brief	  set programmable gain
 * @param	  [in] gain
 * @param	  [in] boost
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
void
HAL_AudPGAGainSet(
	UINT8 gain,
	UINT8 boost
)
{
	UINT8 status;

	ENTER_CRITICAL(status);
	boost = (boost > 3) ? 3 : boost;

	XBYTE[REG_Audio_AmplifierGain] = ( ( (UINT8)gain ) << 2 ) | (UINT8)boost;
	EXIT_CRITICAL(status);
}

/**
 * @fn		  UINT8 HAL_AudALCModeSet(UINT8 mode)
 * @brief	  set ALC auto mode
 * @param	  [in] mode:
				1: HW ALC
				0: FW ALC
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
HAL_AudALCModeSet(
	UINT8 mode
)
{
	if ( mode == 1 ) { //HW ALC
		XBYTE[REG_Audio_AgcFuncEN] = 0x03;
		//XBYTE[REG_Audio_AgcFuncEN] &= 0xfb; /*HW ALC*/
		//XBYTE[REG_Audio_AgcFuncEN] |= 0x02; /*enable hw ALC*/
	}
	else if(mode == 0) { //FW ALC
		XBYTE[REG_Audio_AgcFuncEN] = 0x01;   //FOR Version A IC bug,shoud enable hw ALC
		//XBYTE[REG_Audio_AgcFuncEN] &= 0xfd; /*diable hw ALC*/
		//XBYTE[REG_Audio_AgcFuncEN] |= 0x04; /*FW ALC*/
	
	}
	else {
		return FAIL;
	}
	return SUCCESS;
}

/**
 * @fn		  UINT8 HAL_AudUsbModeSet(UINT8 mode)
 * @brief	  set usb audio mode
 * @param	  [in] mode:
 *				0: audio data is sent to Dram audio Buffer
 *				1: audio data is sent to USB's audio ISO pipe
 * @retval	  return = SUCCESS / FAIL
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT8
HAL_AudUsbModeSet(
	UINT8 mode
)
{
	if ( mode == 0 ) {
		XBYTE[REG_Audio_AudEn] &= 0xfb; 
	}
	else if (mode == 1) {
		XBYTE[REG_Audio_AudEn] |= 0x04;
	}
	else {
		return FAIL;
	}
	return SUCCESS;
}

/**
 * @fn        void HAL_AudAgcCfg(UINT32 dst1, UINT16 windowsize, UINT32 cmdaddr, UINT8 bitmode)
 * @brief     config MOE CMD queue
 * @param     [in] dst1: MOE operating destination word address(Extra buffer address)
 * @param     [in] windowsize: number of samples in the window
 * @param     [in] cmdaddr: MOE command que start address
 * @param     [in] bitmode: 1: 16bit record 0: 8bit record
 * @retval    NONE
 * @see       NULL
 * @author    Kai.wang
 * @since     2010-7-10
 * @todo      N/A
 * @bug       N/A
*/
void HAL_AudAgcCfg(	
	UINT32 dst1,
	UINT16 windowsize,  
	UINT32 cmdaddr,
	UINT8 bitmode
)
{
	//UINT8 sts;
	moe_cmd_t cmd;
	//moe_queue_t queue[2];
#if 1
	HAL_MoeDefCmd(&cmd);
	cmd.func = MOE_FUNC_PointWithCommon;
	cmd.equation = MOE_EQUATION_Absolute;
	cmd.ROIWidth = windowsize;
	cmd.max = 0xffff;
	if(bitmode == HAL_AUD_SAMPLE_16BIT )
	{
		cmd.cp = 0x0000;
		//cmd.src2Sign = 1;
	}
	else if ( bitmode == HAL_AUD_SAMPLE_8BIT)
	{
		cmd.cp = 0x0080;
		//cmd.src2Bit = 1;
	}
	cmd.dst = dst1;
	cmd.dstBit = 1;
	HAL_MoeConvertToQueue(&cmd, alcQueue);
	cmd.CmdQueueEnd = 1;
	cmd.equation = MOE_EQUATION_Summation;
	cmd.max = 0xffffffff;
	cmd.src1 = dst1;
	cmd.src1Bit = 1;
	HAL_MoeConvertToQueue(&cmd, alcQueue+1);

	cmdaddr = cmdaddr;
	//bitmode=bitmode;
#else	
	//ENTER_CRITICAL(sts);
	if ( bitmode == HAL_AUD_SAMPLE_16BIT ) { //16bit
	#if TRUE
		HAL_MoeDefCmd(&cmd);
		cmd.func = MOE_FUNC_PointWithCommon;
		cmd.equation = MOE_EQUATION_Absolute;
		cmd.ROIWidth = windowsize;
		cmd.max = 0xffff;
		cmd.cp = 0x0000;
		cmd.src2Sign = 1;
		cmd.dst = dst1;
		cmd.dstBit = 1;
		HAL_MoeConvertToQueue(&cmd, alcQueue);
		cmd.CmdQueueEnd = 1;
		cmd.equation = MOE_EQUATION_Summation;
		cmd.max = 0xffffffff;
		cmd.src1 = dst1;
		cmd.src1Bit = 1;
		HAL_MoeConvertToQueue(&cmd, alcQueue+1);

		cmdaddr = cmdaddr;
	#else
		//printf("ptrE:%p,%bx\n",ppmem,*ppmem);
		M_DRAM_SetStartAddress(cmdaddr,	0);
		//first cmd , abs diff , 12 words
		M_DRAM_WriteWord(0x02,0x41);
		M_DRAM_WriteWord(*((UINT8*)&windowsize +1),*((UINT8*)&windowsize +0));
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(0xff,0xff);
		M_DRAM_WriteWord(0,0);		
		
		//M_DRAM_WriteWord(*((UINT8*)&src1 +3),*((UINT8*)&src1 +2));
		//M_DRAM_WriteWord(*((UINT8*)&src1 +1),*((UINT8*)&src1 +0)|0x06);
		M_DRAM_SetStartAddress(cmdaddr+8,0);
		M_DRAM_WriteWord(0,0);	
		M_DRAM_WriteWord(0, 0x02);
		M_DRAM_WriteWord(*((UINT8*)&dst1 +3),*((UINT8*)&dst1 +2));
		M_DRAM_WriteWord(*((UINT8*)&dst1 +1),*((UINT8*)&dst1 +0)|0x04);				
		//second cmd , summation , 12 words				
		M_DRAM_WriteWord(0x03,0x71);
		M_DRAM_WriteWord(*((UINT8*)&windowsize +1),*((UINT8*)&windowsize +0));
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(0xff,0xff);
		M_DRAM_WriteWord(0xff,0xff);
		M_DRAM_WriteWord(*((UINT8*)&dst1 +3),*((UINT8*)&dst1 +2));
		M_DRAM_WriteWord(*((UINT8*)&dst1 +1),*((UINT8*)&dst1 +0)|0x04);	
		M_DRAM_WriteWord(0,0);	
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(*((UINT8*)&dst1 +3),*((UINT8*)&dst1 +2));
		M_DRAM_WriteWord(*((UINT8*)&dst1 +1),*((UINT8*)&dst1 +0)|0x04);
	#endif
	}
	else if ( bitmode == HAL_AUD_SAMPLE_8BIT){ //8bit
	#if TRUE
		HAL_MoeDefCmd(&cmd);
		cmd.func = MOE_FUNC_PointWithCommon;
		cmd.equation = MOE_EQUATION_Absolute;
		cmd.ROIWidth = windowsize;
		cmd.max = 0xffff;
		cmd.cp = 0x0080;
		cmd.src2Bit = 1;
		cmd.dst = dst1;
		cmd.dstBit = 1;
		HAL_MoeConvertToQueue(&cmd, alcQueue);
		cmd.CmdQueueEnd = 1;
		cmd.equation = MOE_EQUATION_Summation;
		cmd.max = 0xffffffff;
		cmd.src1 = dst1;
		cmd.src1Bit = 1;
		HAL_MoeConvertToQueue(&cmd, alcQueue+1);
	#else
		M_DRAM_SetStartAddress(cmdaddr,	0);
		//first cmd , abs diff , 12 words
		M_DRAM_WriteWord(0x02,0x41);
		M_DRAM_WriteWord(*((UINT8*)&windowsize +1),*((UINT8*)&windowsize +0));
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(0xff,0xff);
		M_DRAM_WriteWord(0,0);		
		
		//M_DRAM_WriteWord(*((UINT8*)&src1 +3),*((UINT8*)&src1 +2));
		//M_DRAM_WriteWord(*((UINT8*)&src1 +1),*((UINT8*)&src1 +0)|0x40);
		M_DRAM_SetStartAddress(cmdaddr+8,0);
		M_DRAM_WriteWord(0x80,0);	
		M_DRAM_WriteWord(0,0x40);
		M_DRAM_WriteWord(*((UINT8*)&dst1 +3),*((UINT8*)&dst1 +2));
		M_DRAM_WriteWord(*((UINT8*)&dst1 +1),*((UINT8*)&dst1 +0)|0x04);				
		//second cmd , summation , 12 words				
		M_DRAM_WriteWord(0x03,0x71);
		M_DRAM_WriteWord(*((UINT8*)&windowsize +1),*((UINT8*)&windowsize +0));
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(0xff,0xff);
		M_DRAM_WriteWord(0xff,0xff);
		M_DRAM_WriteWord(*((UINT8*)&dst1 +3),*((UINT8*)&dst1 +2));
		M_DRAM_WriteWord(*((UINT8*)&dst1 +1),*((UINT8*)&dst1 +0)|0x04);	
		M_DRAM_WriteWord(0,0);	
		M_DRAM_WriteWord(0,0);
		M_DRAM_WriteWord(*((UINT8*)&dst1 +3),*((UINT8*)&dst1 +2));
		M_DRAM_WriteWord(*((UINT8*)&dst1 +1),*((UINT8*)&dst1 +0)|0x04);
	#endif
	}
	//EXIT_CRITICAL(sts);
#endif
}

/**
 * @fn        void HAL_AudAgcValGet(UINT32 src1, UINT32 cmdaddr, UINT8 bitmode, UINT16 *agcval)
 * @brief     get agc value using MOE cmd
 * @param     [in] src1: MOE operating start word address(src1 start address)
 * @param     [in] cmdaddr: 
 * @param     [in] bitmode: 1: 16bit record 0: 8bit record
 * @param     [in] agcval: audio volume energy in a window
 * @retval    NONE
 * @see       NULL
 * @author    Kai.wang
 * @since     2010-7-10
 * @todo      N/A
 * @bug       N/A
*/
void HAL_AudAgcValGet(
	UINT32 src1,
	UINT32 cmdaddr,
	UINT8 bitmode,
	UINT16 *agcval
)
{ 
#if 1
	//UINT8 xdata *ppmem = NULL;
	//UINT8 sts/*,tbl[48],i*/;
	if (HAL_GlobalICVerGet() == 0x00) {

		UINT32 addr = K_SDRAM_CodeSize+(((UINT16)alcQueue -0x4000)>>1);
	
		moe_queue_t *queue = alcQueue;
		queue->src10 = *((UINT8*)&src1 +3);
		queue->src11 = *((UINT8*)&src1 +2);
		queue->src12 = *((UINT8*)&src1 +1);
		queue->src13 = *((UINT8*)&src1 +0)|((bitmode == HAL_AUD_SAMPLE_16BIT)?0x06:0x40);

		//moeDbgTrig=MOE_Trigger_Alc;
		HAL_MoeRunQueueIntr(addr,MOE_Trigger_Alc);

		cmdaddr = cmdaddr;
		*((UINT8*)agcval) = XBYTE[REG_Moe_DataSum3];
		*((UINT8*)agcval+1) = XBYTE[REG_Moe_DataSum2];
	}
	else {
		*((UINT8*)agcval) = XBYTE[0x26f1]; 
		*((UINT8*)agcval+1) = XBYTE[0x26f0]; 
	}
#else	
	ENTER_CRITICAL(sts);
	if ( bitmode == HAL_AUD_SAMPLE_16BIT) { //16bit
		//printf("cmdadd:%lx\n",cmdaddr);
		//M_DRAM_SetStartAddress(cmdaddr,	1);
		HAL_CpuRamMapSet(cmdaddr+6,(UINT16)(0x04)/*(K_SDRAM_AlcMoeCmdBufSize<<1)*/,&((void xdata *)ppmem));
		*ppmem = *((UINT8*)&src1 +3);
		ppmem++;
		*ppmem = *((UINT8*)&src1 +2);
		ppmem++;
		*ppmem = *((UINT8*)&src1 +1);
		ppmem++;
		*ppmem = *((UINT8*)&src1 +0)|0x06;
		/*for debug
		for(i=0;i<48;i+=2){
			M_DRAM_ReadWord(tbl[i], tbl[i+1]);
		}
		for(i=0;i<48;i++){
			printf("i:%bx: %bx\n",i,tbl[i]);
		}
		*/
	}
	else if ( bitmode == HAL_AUD_SAMPLE_8BIT) { //8bit
		HAL_CpuRamMapSet(cmdaddr+6,(UINT16)(0x04)/*(K_SDRAM_AlcMoeCmdBufSize<<1)*/,&((void xdata *)ppmem));
		*ppmem = *((UINT8*)&src1 +3);
		ppmem++;
		*ppmem = *((UINT8*)&src1 +2);
		ppmem++;
		*ppmem = *((UINT8*)&src1 +1);
		ppmem++;
		*ppmem = *((UINT8*)&src1 +0)|0x40;

	}	
	HAL_CpuRamMapRcv();	
	
	EXIT_CRITICAL(sts);
	// config cmdque
	if(XBYTE[0x0e02])XBYTE[0x0e02]=0;//wangjg++ enable moe
	XBYTE[0x2e03] = 0x01;
	XBYTE[0x2e04] = *((UINT8*)&cmdaddr +3);
	XBYTE[0x2e05] = *((UINT8*)&cmdaddr +2);
	XBYTE[0x2e06] = *((UINT8*)&cmdaddr +1);
	XBYTE[0x2e07] = *((UINT8*)&cmdaddr +0);
	XBYTE[0x2e00] =0x1f;
	XBYTE[0x2e0c] =0x01;  
	while(((XBYTE[0x2e00]&0x01) == 0) && (!XBYTE[0x2e02]));
	//*((UINT8*)agcval) = XBYTE[0x2e1b];
	//*((UINT8*)agcval+1) = XBYTE[0x2e1a];
	//*((UINT8*)agcval+2) = XBYTE[0x2e19];
	//*((UINT8*)agcval+3) = XBYTE[0x2e18];
#endif
	
}

/**
 * @fn		  void HAL_AudSpeakerPwrSet(UINT8 on)
 * @brief	  set the audio speaker power down or normal operation
 * @param	  [in] on
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A
*/

void HAL_AudSpeakerPwrSet(
	UINT8 on
)
{
	if ( on )	{
		XBYTE[REG_Audio_AudDACPwrEn] |= 0x10;
	}
	else {
		XBYTE[REG_Audio_AudDACPwrEn] &= ~0x10;
	}

}

/**
 * @fn		  void HAL_AudPwrSave(UINT8 on)
 * @brief	  set the audio module power consumption
 * @param	  [in] on : 1 : save power
 					 0 : resume
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-27
 * @todo	  N/A
 * @bug 	  N/A*/
void HAL_AudPwrSave(
	UINT8 on
){
	if ( on ) {
		S_0x2640 = XBYTE[REG_Audio_AudAdcPwrEn];
		S_0x2648 = XBYTE[REG_Audio_SarAdcPwrEn];
		S_0x2670 = XBYTE[REG_Audio_AudDACPwrEn];
		S_0x2677 = XBYTE[REG_Audio_VrefPwrEn];
		XBYTE[REG_Audio_AudAdcPwrEn] &= 0xCE; 	
		XBYTE[REG_Audio_SarAdcPwrEn] &= 0xE8;
		XBYTE[REG_Audio_AudDACPwrEn] &= 0xCE; 	
		XBYTE[REG_Audio_VrefPwrEn] &= 0xF8; 
	}
	else {
		XBYTE[REG_Audio_AudAdcPwrEn] = S_0x2640; 	
		XBYTE[REG_Audio_SarAdcPwrEn] = S_0x2648;
		XBYTE[REG_Audio_AudDACPwrEn] = S_0x2670; 	
		XBYTE[REG_Audio_VrefPwrEn] = S_0x2677; 
	}

}
/**
 * @fn		  void HAL_AudSwRst(UINT8 rst, UINT8 level)
 * @brief	  reset audio
 * @param	  [in] rstval:
 *				bit 0: syncinf mclk swrst
 *				bit 1: audio control swrst
 *				bit 2: syncinf mclk swrst
 *				bit 3: audclk swrst
 *				bit 4: agc unction swrst
 *				bit 5: ADPCM function swrst
 *				bit 6: SAR ADC function swrst
 *				bit 7: syncinf f48m swrst
 * @retval	  NONE
 * @see 	  NULL
 * @author	  Kai.wang
 * @since	  2010-4-28
 * @todo	  N/A
 * @bug 	  N/A
*/
void HAL_AudSwRst(
	UINT8 rstval
)
{
	XBYTE[REG_Audio_AudRst] |= rstval; //reset 
	
	XBYTE[REG_Audio_AudRst] &= ~rstval; //normal
}

/**
 * @fn        void HAL_AudI2SSet(UINT8 en)
 * @brief     set the audio output to GPIO19~22
 * @param     1: enable, 0: disable
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-9-9
 * @todo      N/A
 * @bug       N/A
*/
void HAL_AudI2SSet(
	UINT8 en
)
{
	if (en) {
		XBYTE[0x20f8] |= 0x04;
	} else {
		XBYTE[0x20f8] &= ~0x04;
	}
}
