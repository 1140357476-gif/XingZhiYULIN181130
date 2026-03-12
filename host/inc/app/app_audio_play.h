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

#ifndef __APP_AUDIO_PLAY_H__
#define __APP_AUDIO_PLAY_H__

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define	EVENT_NULL 		0
#define	EVENT_CARD_PLUG_IN		1
#define	EVENT_CARD_PLUG_OUT  	2
#define	EVENT_USB_PLUG_IN		3
#define	EVENT_USB_PLUG_OUT 		4
#define	EVENT_TV_PLUG_IN 		5
#define	EVENT_TV_PLUG_OUT 		6

#define VOICE_TYPE_BEEP			0
#define VOICE_TYPE_SHUTTER		1
#define VOICE_TYPE_OPEN		2
#define VOICE_TYPE_CLOSE		3

#define VOICE_PLAYLEVLE_HIGH	1 // The voice must be played 
#define VOICE_PLAYLEVLE_LOW	0 // if range of the msg  is 0x10 to 0x1f,voice play

#define APP_AUD_PB_SPD_NORMAL	1
#define APP_AUD_PB_SPD2			2
#define APP_AUD_PB_SPD4			4
#define APP_AUD_PB_SPD8			8 

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

void appAudioPlay(UINT16 msg);
void appAudioPlayInit(void); 
void appAudioPlayPause(void);
void appAudioPlayOK(void);
void appMediaPlayDispBufSet(UINT32 addr);
void appVoiceLoad(UINT8 index ,UINT32 addr);
void appVoicePlay(UINT16 msg,UINT8 voiceType , UINT8 playLevel);
void appVoicePreview(UINT8 item,UINT8 value);
void appAudioPlayRWSpeedSet(UINT8 spd);
void appAudioPlayFWSpeedSet(UINT8 spd);
UINT8 appAudioPlaySpeedGet(void);
void appBuzzer(void);

#endif /*__APP_AUDIO_PLAY_H__*/

