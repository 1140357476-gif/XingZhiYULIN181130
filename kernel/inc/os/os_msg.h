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
#ifndef __OS_MSG_H__
#define __OS_MSG_H__

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MSG_CTRL_IDX_GROUP_SHIFT	(8 + 4)
#define MSG_CTRL_IDX_GROUP_MASK		((UINT16)0x0F << MSG_CTRL_IDX_GROUP_SHIFT) //0xF000
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define DZOOM_OSD_MSG_TO_HOST	1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef UINT16 msg_t;
 
typedef enum msgCtrlType_e {
	MSG_CTRL_TYPE_HOST = 0x00,	//for host flow message used
	MSG_CTRL_TYPE_BTN  = 0x01,  //for button driver message used
	MSG_CTRL_TYPE_SYS  = 0x02,	//for system internal message used
	MSG_CTRL_TYPE_MAX  = 0x03,
} msgCtrlType_t;

typedef enum msgCtrlFlush_e {
	MSG_CTRL_FLUSH_HOST = 0x01 << 0,
	MSG_CTRL_FLUSH_BTN	= 0x01 << 1,
	MSG_CTRL_FLUSH_SYS  = 0x01 << 2,
	MSG_CTRL_FLUSH_ALL  = MSG_CTRL_FLUSH_HOST | MSG_CTRL_FLUSH_BTN | MSG_CTRL_FLUSH_SYS,
} msgCtrlFlush_t;

 
typedef struct MainMsgQue_s
{
	struct MainMsgQue_s* pQue;
	msg_t* QueStart;
	msg_t* QueEnd;
	msg_t* QueIn;
	msg_t* QueOut;
	UINT16 QueEntries;
	UINT16 QueSize;
	msg_t* MsgBuf;
	UINT8 bLock;
} MainMsgQue_t;

typedef enum {
	SP1K_MSG_NULL = 0x0000,
		
	/* media msg */
	
	SP1K_MSG_MEDIA_REC_OK         	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0800,//0x2800
	SP1K_MSG_MEDIA_REC_FAIL       	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0801,//0x2801
	SP1K_MSG_MEDIA_REC_DISK_FULL  	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0802,//0x2802
	SP1K_MSG_MEDIA_PLAY_OK        	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0803,//0x2803
	SP1K_MSG_MEDIA_PLAY_FAIL      	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0804,//0x2804
	SP1K_MSG_MEDIA_PLAY_END       	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0805,//0x2805
	SP1K_MSG_MEDIA_FF_TO_END      	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0806,//0x2806
	SP1K_MSG_MEDIA_RW_TO_END      	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0807,//0x2807
	SP1K_MSG_MEDIA_SOUND_FINISH   	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0808,//0x2808
	SP1K_MSG_MEDIA_PLAY_START     	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0809,//0x2809
	SP1K_MSG_MEDIA_REC_START      	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x080a,//0x280a
	SP1K_MSG_MEDIA_REC_STOP       	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x080b,//0x280b
	SP1K_MSG_MEDIA_VOICE_MEMO_END 	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x080c,//0x280c
	SP1K_MSG_MEDIA_REC_FILE_MAX 	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x080d,//0x280d
	SP1K_MSG_MEDIA_VIDO_CAP_ERROR 	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x080e,//0x280e
	SP1K_MSG_MEDIA_VIDO_CAP_FINISH 	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x080f,//0x280f
	/* misc control */

	SP1K_MSG_KEY_PRESS_SELFSNAP 	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0900,//0x2900
	SP1K_MSG_SELFSNAP_BEEP      	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0901,//0x2901
	SP1K_MSG_CMD_PANEL_OFF      	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0902,//0x2902
	SP1K_MSG_CMD_PANEL_ON       	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0903,//0x2903
	SP1K_MSG_3A_WAIT_PROC       	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0904,//0x2904
	SP1K_MSG_SELFSNAP_COUNT       	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0905,//0x2905
	SP1K_MSG_TIMER_COUNT			= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0906,//0x2906
	SP1K_MSG_GUI_PEN_SLIDE			= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0907,//0x2907
	SP1K_MSG_GUI_PEN_UP				= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0908,//0x2908
	SP1K_MSG_GUI_PEN_REPEATE		= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0909,//0x2909
	SP1K_MSG_DO_RESET				= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x090a,//0x290a

	/* slide show msg */
	
	SP1K_MSG_SLIDE_SHOW_START  	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0b00,//0x2b00
	SP1K_MSG_SLIDE_SHOW_STOP   	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0b01,//0x2b01
	SP1K_MSG_SLIDE_SHOW_NEXT   	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0b02,//0x2b02
	SP1K_MSG_SLIDE_SHOW_FINISH 	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0b03,//0x2b03

	/* DPS */
	
	SP1K_MSG_DPS_NEW_HDISCVRY       	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0c01,//0x2c01	//discvry dps printer
	SP1K_MSG_DPS_NEW_CONFIG_PRINTER 	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0c02,//0x2c02	//printer send new configure info
	SP1K_MSG_DPS_NEW_DEVICE_STATUS  	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0c03,//0x2c03 //printer send new device status
	SP1K_MSG_DPS_NEW_JOB_STATUS     	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0c04,//0x2c04 //printer send new job status
	SP1K_MSG_DPS_START_JOB          	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0c05,//0x2c05 //print job start
	SP1K_MSG_DPS_ERROR              	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0c06,//0x2c06
	SP1K_MSG_DPS_ERROR_SIDC         	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0c07,//0x2c07
	sp1k_MSG_DPS_NOT_FIND_PRINTER		= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0c08,//0x2c08

	/* fd */
	SP1K_MSG_FD_SMILE         	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0d01,//0x2d01
	SP1K_MSG_EYE_BLINK			= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0d02,//0x2d02
	SP1K_MSG_MD_ACTION			= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0d03,//0x2d03
	sp1k_MSG_MD_UPDATE_TIME		= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0d04,//0x2d04
	sp1k_MSG_REC_SEAMLESS_DISK_FULL = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0d05,//0x2d04
	sp1k_MSG_REC_SEAMLESS_CLOSEFILE = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0d06,//0x2d04
	sp1k_MSG_REC_SEAMLESS_NOT_CHANGENAME = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0d07,//0x2d04

	/* dzoom */
	#if (DZOOM_OSD_MSG_TO_HOST == 0) //linjiecheng move to host msg queue
	SP1K_MSG_DZOOM_OSD_UPDATE  	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0e01,//0x2e01
	#else
	SP1K_MSG_DZOOM_OSD_UPDATE  	= ((UINT16)MSG_CTRL_TYPE_HOST << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0e01,//0x0e01
	#endif
	SP1K_MSG_DZOOM_3A_UPDATE  	= ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0e02,//0x2e02

	/* panorama */
	SP1K_MSG_GMV_BY_FRAME_CALCULUS = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0f01,//0x2f01
	SP1K_MSG_SUM_OF_GMV_OSD_DRAW = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0f02,//0x2f02
	SP1K_MSG_GMV_TOO_FAST_OSD_DRAW = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0f03,//0x2f03
	SP1K_MSG_GMV_TOO_SLOW_OSD_DRAW = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0f04,//0x2f04
	SP1K_MSG_PANORAMA_FINISH = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0f05,//0x2f05

	SP1K_MSG_UNKNOWN_SYS = ((UINT16)MSG_CTRL_TYPE_SYS << MSG_CTRL_IDX_GROUP_SHIFT) | 0x0fff,//0x2fff
} osMsgList_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void osMsgRcv(void);
void osMsgInit (void);
UINT8 osMsgQuery (void);
UINT8 osMsgAccept (UINT16* msg);
UINT8 osMsgPost (UINT16 msg);
UINT8 osMsgPostFront (UINT16 msg);	
UINT8 osMsgPostForce (UINT16 msg);
UINT8 osMsgWait(UINT16 msg);
void osMsgFlush (UINT8 msgGrp);
UINT8 osMsgSeek(UINT16 msg);
UINT8 osMsgIntrSeek(UINT16 msg);
void osMsgLock (UINT8 bLock, msgCtrlFlush_t msgGrp);
#endif /*__OS_MSG_H__*/

