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
#ifndef __SP1K_AUD_API_H__
#define __SP1K_AUD_API_H__

/** \file sp1k_aud_api.h
 * @brief Spca1628 audio related interface which include: a)Initial setting, b) Parameter setting, C) Mode setting, d) Status getting of REC/PB. 
 */
 
#include "Audio_rw.h"
#include "sp1k_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum {
	SP1K_AUD_PARAM_ALC_OPT = 0,
	SP1K_AUD_PARAM_ALC_WIN_SIZE,	
	SP1K_AUD_PARAM_ALC_GAIN_SET,
	SP1K_AUD_PARAM_ALC_MAX_GAIN_IDX,
	SP1K_AUD_PARAM_ALC_MIN_GAIN_IDX,
	SP1K_AUD_PARAM_ALC_MAX_VOL,
	SP1K_AUD_PARAM_ALC_MIN_VOL,
	SP1K_AUD_PARAM_ALC_DEFAULT_GAIN_IDX,
	SP1K_AUD_PARAM_ALC_GAIN_UP_SPEED,
	SP1K_AUD_PARAM_ALC_GAIN_DOWN_SPEED,
	SP1K_AUD_PARAM_ALC_DIGITAL_VOL,	
	SP1K_AUD_PARAM_ALC_SOUND_CTRL,
	SP1K_AUD_PARAM_ALC_MAX
} audALCParam_e;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 #ifdef SP1K_DOXY_GEN
 /**
 * @fn UINT8 sp1kAudioPbStatusGet(void)
 * @brief	obtain the audio play status
 * @retval the audio play status
 *\n         SP1K_AUD_PB_IDLE  : 0
 *\n         SP1K_AUD_PB_RUN   : 1
 *\n         SP1K_AUD_PB_PAUSE : 2
 *\n         SP1K_AUD_PB_FW    : 4
 *\n         SP1K_AUD_PB_RW    : 8
 */
UINT8 sp1kAudioPbStatusGet(void);

 /**
 * @fn void sp1kAudioPbStart(UINT16 ID)
 * @brief	Play a audio file
 * @param[in] ID DCF file index. Range is 1 to 9999
 * @retval none
 */
void sp1kAudioPbStart(UINT16 ID);

 /**
 * @fn void sp1kAudioPbStop(void)
 * @brief	Abort audio  playback process 
 * @retval none
 */
void sp1kAudioPbStop(void);
 
  /**
 * @fn void sp1kAudioPbRWSpeedSet(UINT8 speed)
 * @brief	Specify speed of rewind mode
 * @param[in] speed	 specify new speed.
 * @retval none
 * @see sp1kAudioPbSpeedGet()
 */
void sp1kAudioPbRWSpeedSet(UINT8 speed);

 /**
 * @fn void sp1kAudioPbFWSpeedSet(UINT8 speed)
 * @brief	Specify speed of forward mode
 * @param[in] speed	 specify new speed
 * @retval none
 * @see sp1kAudioPbSpeedGet()
 */
void sp1kAudioPbFWSpeedSet(UINT8 speed);

  /**
 * @fn UINT8 sp1kAudioPbSpeedGet(void)
 * @brief	Get the current audio play speed 
 * @retval current audio play speed
 *\n       1: 1X
 *\n       2: 2X
 *\n       4: 4X
 *\n       8: 8X
 * @see sp1kAudioPbRWSpeedSet() 
 */
UINT8 sp1kAudioPbSpeedGet(void);

  /**
 * @fn void sp1kAudioPbResume(void)
 * @brief	Resumes audio playback process..  
 */
void sp1kAudioPbResume(void);

  /**
 * @fn void sp1kAudioPbPause(void)
 * @brief	Pauses audio playback  process.
 */
void sp1kAudioPbPause(void);

 /**
 * @fn void sp1kImageVoiRecSet(UINT32 addr, UINT16 nameIdx)
 * @brief	Specify vioce memo record setting
 *
 *			The function is designed for video memo recording
 * @param[in] addr	Image voice record buffer start address.
 *\n 			    The buffer that  must be allocated before recording. 
 *\n 				The buffer space must bigger than 512K bytes.				
 * @param[in] nameIdx 	audio file index. The index must same as current image file index.
 * @retval none
 */
void sp1kImageVoiRecSet(UINT32 addr, UINT16 nameIdx);

 /**
 * @fn void sp1kImageVoiRecImgNameSet(UINT8 *name)
 * @brief	Specify vioce memo record setting
 *
 *			The function is designed for video memo recording
 * @param[in] name 	audio filename. The filename must same as current image filename.
 * @retval none
 */
void sp1kImageVoiRecImgNameSet(UINT8 *name);

 /**
 * @fn UINT8 sp1kAudioRecStatusGet(void)
 * @brief	Obtain the audio record status			
 * @retval The audio record staus : status is 0: idle, 1: busy, 2: pause
 */
UINT8 sp1kAudioRecStatusGet(void);

 /**
 * @fn void sp1kAudioRecStart(void)
 * @brief	 Start recording audio process
 * @retval none
 */
void sp1kAudioRecStart(void);

 /**
 * @fn void sp1kAudioRecStop(void)
 * @brief	 Stop audio recording process
 * @retval none
 */
void sp1kAudioRecStop(void);
 
 /**
 * @fn void sp1kAudioRecFreqSet(UINT8 freq)
 * @brief	Specifies audio sampling rate
 *
 *			The function must be called before audio record or the sample rate will be unknown.
 * @param[in] freq 	audio sample rate - 1: 8Khz, 2:11.025khz, :12khz, 16khz, 5:22.05khz, 6:32khz, 7:44.1khz, 8:48khz
 * @retval none
 */
void sp1kAudioRecFreqSet(UINT8 freq);

  /**
 * @fn void sp1kAudioRecPause(void)
 * @brief	Pauses audio recording process 
 */
void sp1kAudioRecPause(void);

  /**
 * @fn void sp1kAudioRecResume(void)
 * @brief	Resumes audio recording process 
 */
void sp1kAudioRecResume(void);

 /**
 * @fn void sp1kAudioRecFmtSet(UINT8 fmt)
 * @brief	Set audio  data format
 *
 *			The function must be called before audio record or the data format will be unknown.
 * @param[in] fmt 	audio record data format - 0: PCM ,1: ADPCM
 * @retval none
 */
void sp1kAudioRecFmtSet(UINT8 fmt);

 /**
 * @fn UINT32 sp1kAudioRecTimeGet(void)
 * @brief	Obtain Audio recording elapsed time in milliseconds 
 * @retval audio record time in millisecond unit.
 */
UINT32 sp1kAudioRecTimeGet(void);

 /**
 * @fn void sp1kAudioRecTypeSet(UINT8 type)
 * @brief	Specify audio record type.
 *
 *	        The type can be normal audio or  image voice.  The function must be called before start audio record.
 * @param[in] type 
 *		  	0: Normal audio record.
 *			1: Image memo record. Add audio memo when play a still image in playback mode.
 * @retval NONE
 * @see sp1kAudioRecTypeGet() 
 */
void sp1kAudioRecTypeSet(UINT8 type);

 /**
 * @fn UINT8 sp1kAudioRecTypeGet(void)
 * @brief Obtain audio record type setting
 * @retval audio record type - 0: normal audio record, 1: voice memo record
 * @see sp1kAudioRecTypeSet() 
 */
UINT8 sp1kAudioRecTypeGet(void);
 
 #else
#define sp1kAudioPbStatusGet    audiopbStatusGet
#define sp1kAudioPbStart   audioPbStart
#define sp1kAudioPbStop    audioPbStop
#define sp1kAudioPbRWSpeedSet   audioPbRWSpeedSet
#define sp1kAudioPbFWSpeedSet   audioPbFWSpeedSet
#define sp1kAudioPbSpeedGet     audioPbSpeedGet
#define sp1kAudioPbResume  audioPbResume
#define sp1kAudioPbPause   audioPbPause

#define sp1kImageVoiRecSet      imageVoiRecSet
#define sp1kImageVoiRecImgNameSet  imageVoiRecImgNameSet
#define sp1kAudioRecStatusGet   audioRecStatusGet
#define sp1kAudioRecStart   audioRecStart
#define sp1kAudioRecStop   audioRecStop
#define sp1kAudioRecFreqSet     audioRecFreqSet
#define sp1kAudioRecPause  audioRecPause
#define sp1kAudioRecResume audioRecResume
#define sp1kAudioRecFmtSet      audioRecFmtSet
#define sp1kAudioRecTimeGet audioRecTimeGet
#define sp1kAudioRecTypeSet audioRecTypeSet
#define sp1kAudioRecTypeGet audioRecTypeGet
#endif

#define SP1K_AUD_PB_IDLE  AUD_PB_IDLE
#define SP1K_AUD_PB_RUN   AUD_PB_RUN
#define SP1K_AUD_PB_PAUSE AUD_PB_PAUSE
#define SP1K_AUD_PB_FW    AUD_PB_FW
#define SP1K_AUD_PB_RW    AUD_PB_RW

#define SP1K_AUD_ITEM_OPEN_ONE  0x01
#define SP1K_AUD_ITEM_OPEN_TWO 0x02
#define SP1K_AUD_ITEM_OPEN_THREE 0x03
#define SP1K_AUD_ITEM_BEEP 0x04
#define SP1K_AUD_ITEM_SHUTTER 0x05
#define SP1K_AUD_ITEM_VOLUME 0x06

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
/* suny mark. Functions are not used now,but may be used in future*/
/**
 * @fn UINT8 sp1kAudParamSet( UINT8 attrId, UINT8 value )
 * @brief	Set audio related parameter
 * @param[in] attrId   audio parameter
 *\n	                         SP1K_AUD_PARAM_ALC_OPT ,
 *\n	                         SP1K_AUD_PARAM_ALC_GAIN_SET,
 *\n	                         SP1K_AUD_PARAM_ALC_MAX_GAIN_IDX,
 *\n	                         SP1K_AUD_PARAM_ALC_MIN_GAIN_IDX,
 *\n	                         SP1K_AUD_PARAM_ALC_MAX_VOL,
 *\n	                         SP1K_AUD_PARAM_ALC_MIN_VOL,
 *\n	                         SP1K_AUD_PARAM_ALC_DEFAULT_GAIN_IDX,
 *\n	                         SP1K_AUD_PARAM_ALC_GAIN_UP_SPEED,
 *\n	                         SP1K_AUD_PARAM_ALC_GAIN_DOWN_SPEED,
 *\n	                         SP1K_AUD_PARAM_ALC_DIGITAL_VOL,	
 *\n	                         SP1K_AUD_PARAM_ALC_SOUND_CTRL,
 * @param[in] value  Value of of each setting above
 *\n	                         0: ALC off, 1: ALC on
 *\n	                         0~42: set PGA val,
 *\n	                         set Pga max gain index  max is 42,
 *\n	                         set Pga min gain index  max is 0,
 *\n	                         set max audio volume val,
 *\n	                         set min audio volume val,
 *\n	                         set default gain index,
 *\n	                         set gain up speed,
 *\n	                         set gain down speed,
 *\n	                         0: soundctrl off ,1 : soundctrl on,	                          
 * @retval error code -  SUCCESS: 0, FAIL: 1
 */
UINT8 sp1kAudParamSet( UINT8 attrId, UINT8 value );

/**
 * @fn UINT8 sp1kAudParamGet( UINT8 attrId )
 * @brief	Get audio related parameter
 * @param[in] attrId   audio parameter
 *\n	                         SP1K_AUD_PARAM_ALC_OPT ,
 *\n	                         SP1K_AUD_PARAM_ALC_GAIN_SET,
 *\n	                         SP1K_AUD_PARAM_ALC_MAX_GAIN_IDX,
 *\n	                         SP1K_AUD_PARAM_ALC_MIN_GAIN_IDX,
 *\n	                         SP1K_AUD_PARAM_ALC_MAX_VOL,
 *\n	                         SP1K_AUD_PARAM_ALC_MIN_VOL,
 *\n	                         SP1K_AUD_PARAM_ALC_DEFAULT_GAIN_IDX,
 *\n	                         SP1K_AUD_PARAM_ALC_GAIN_UP_SPEED,
 *\n	                         SP1K_AUD_PARAM_ALC_GAIN_DOWN_SPEED,
 *\n	                         SP1K_AUD_PARAM_ALC_DIGITAL_VOL,	
 *\n	                         SP1K_AUD_PARAM_ALC_SOUND_CTRL,
 *\n	                         0: soundctrl off ,1 : soundctrl on,	                          
 * @retval                Value of of each setting above
 */
UINT8 sp1kAudParamGet( UINT8 attrId );

 /**
 * @fn void sp1kAudioPlay(UINT32 audAddr )
 * @brief	Parse sound wav file and initiate audio module. 
 * @param[in] audAddr  	The starting of audio buffer address in word unit
 */
void sp1kAudioPlay(UINT32 audAddr );

 /**
 * @fn void sp1kAudioVolumeSet(UINT8 value)
 * @brief	Specify audio play volume
 * @param[in] value  	Set volume parameter.0: mute , 1: low volume , 2: middle volume, 3: high volume
 */
void sp1kAudioVolumeSet(UINT8 value);

 /**
 * @fn void sp1kAudioPlayInit(void)
 * @brief	Init the audio play parameter
 *
 * @The function must be called before any audio play services
 * @retval none
 */
void sp1kAudioPlayInit(void);

/**
 * @fn UINT16 sp1kAudioAgcValGet(void)
 * @brief	Get the audio agc value of an audio fram 
 * @         The function must be called After ALC enable!
 * @param none
 * @retval  return = agc value
 */
UINT16 sp1kAudioAgcValGet(void);

#endif  /* __SP1K_AUD_API_H__ */

