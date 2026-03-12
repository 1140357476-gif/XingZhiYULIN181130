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
 * @file      hal_audio.h
 * @brief     hal audio header file.
 * @author    kai.wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef _HAL_AUDIO_H__
#define _HAL_AUDIO_H__
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 #include "general.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	HAL_AUD_SAMPLE_8BIT = 0,
	HAL_AUD_SAMPLE_16BIT = 1,
} HAL_AudSampleBitMode_e;
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
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void HAL_AudStart(UINT32 mode, UINT32 smpRate, UINT32 smpBit, UINT32 chn);
void HAL_AudSoundPlayCfg(UINT32 smpRate, UINT32 smpBit, UINT32 chn);
void HAL_AudSoundPlayTrg(UINT32 index);
void HAL_AudStop(UINT32 mode);
void HAL_AudPause(void);
void HAL_AudResume(void);
void HAL_AudThdSet(UINT32 thd);
void HAL_AudRingBufferSet(UINT32 sdramAddrByte, UINT32 sizeByte);
void HAL_AudInIndexSet(UINT32 index);
void HAL_AudOutIndexSet(UINT32 index);
UINT8 HAL_AudIntStsGet(UINT16 mode);
void HAL_AudVolumeSet(UINT8 volnum_integer,	UINT8 voldenum_decimal);
void HAL_AudInIndexGet(UINT32 *index);
void HAL_AudOutIndexGet(UINT32 *index);
UINT8 HAL_AudRingBufCntRead(UINT32 *pAudBufCnt);
void HAL_AudAdpcmSet(UINT8 mode, UINT8 on, UINT32 blockSize);
void HAL_AudAgcWindowSet(UINT16 window);
void HAL_AudAgcStateGet(UINT32 *eng, UINT32 *window, UINT8 *cnt);
void HAL_AudPGAGainGet(UINT8 *gain,	UINT8 *boost);
void HAL_AudPGAGainSet(UINT8 gain, UINT8 boost);
UINT8 HAL_AudALCModeSet(UINT8 mode);
UINT8 HAL_AudUsbModeSet(UINT8 mode);
void HAL_AudAgcCfg(UINT32 dst1, UINT16 windowsize, UINT32 cmdaddr, UINT8 bitmode);
void HAL_AudAgcValGet(UINT32 src1, UINT32 cmdaddr, UINT8 bitmode, UINT16 *agcval);
void HAL_AudSpeakerPwrSet(UINT8 on);
void HAL_AudPwrSave(UINT8 on);
void HAL_AudSwRst(UINT8 rstval);
void HAL_AudI2SSet(UINT8 en);
#endif  /* _HAL_AUDIO_H__ */

