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
#ifndef __SP1K_VIDEO_API_H__
#define __SP1K_VIDEO_API_H__

/** \file sp1k_video_api.h
 * @brief		Spca1528 video related interface
 */
#include "video_clip.h"
#include "video_pb.h"
#include "sp1k_def.h"
#include "app_video_view.h"
#include "Video_dtstmp.h"
#include "video_slclip.h"
#include "video_mem.h"
#include "sp1k_zoom_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

typedef enum {
	SP1K_VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG = VIDEO_ZOOM_CTRL_PARA_CURRENT_LOG,
	SP1K_VIDEO_ZOOM_CTRL_PARA_MAX = VIDEO_ZOOM_CTRL_PARA_MAX,
}SP1K_VIDEO_ZOOM_CTRL_PARA;


 #ifdef SP1K_DOXY_GEN
 /**
 * @fn void sp1kVideoRecTimeGet(void)
 * @brief	Obtain video recording elapsed time in milliseconds
 * @retval video recording elapsed time in millisecond unit
 */
UINT32 sp1kVideoRecTimeGet(void);

 /**
 * @fn void sp1kVideoRecAudFmtSet(UINT8 fmt)
 * @brief	Configure audio data format for video clip
 *
 *          The function must be called before video record,otherwise the data format will be unknown.
 * @param[in] fmt 	audio data format - 0: PCM ,1: ADPCM
 * @retval none
 */
void sp1kVideoRecAudFmtSet(UINT8 fmt);

 /**
 * @fn void sp1kVideoRecFrameRateSet(UINT8 rate)
 * @brief	Setting video record frame rate.
 *
 *         The function must be called before video record,otherwise the frame rate will be unknown.
 * @param[in] rate		video frame rate - 0: 30fps ,1: 15fps,2: 60fps
 * @retval none
 */
void sp1kVideoRecFrameRateSet(UINT8 rate);

 /**
 * @fn UINT32 sp1kVideoPvFreeAddrGet(void)
 * @brief	Obtain the starting physical address of system free buffer. The unit is word.
 * @retval the statring of system free physical buffer address in word unit
 */
UINT32 sp1kVideoPvFreeAddrGet(void);

 /**
 * @fn UINT8 sp1kVideoRecStatusGet(void)
 * @brief	Obtain the video record status
 * @retval the video clip status -idle :0,normal run :1,pause :2
 */
UINT8 sp1kVideoRecStatusGet(void);

 /**
 * @fn void sp1kVideoRecStart(void)
 * @brief	Starts video recording process
 * @retval none
 * @see sp1kVideoRecStop()
 */
void sp1kVideoRecStart(void);

 /**
 * @fn void sp1kVideoRecStop(void)
 * @brief	Stops video recording process
 * @retval none
 * @see sp1kVideoRecStart()
 */
void sp1kVideoRecStop(void);

 /**
 * @fn void sp1kVideoRecZoomFacSet(UINT16 zfac, UINT8 videozoom)
 * @brief	Set the zoom factor in video mode
 * @param[in] zfac	 specify current zoom factor. The factor range is 100(1.0X) to 400(4.0X).
 * @param[in] videozoom 	1: video mode, 0: other mode
 * @retval none
 * @see sp1kVideoZoomFctGet()
 */
UINT8 sp1kVideoRecZoomFacSet(UINT16 zfac, UINT8 videozoom);

  /**
 * @fn void sp1kVideoZoomFctGet(void)
 * @brief	Obtain the zoom factor in video mode
 * @retval zoom factor value.range is 100 to 400
 * @see sp1kVideoRecZoomFacSet()
 */
UINT16 sp1kVideoZoomFctGet(void);

 /**
 * @fn void sp1kVideoRecSizeGet(UINT16 * width, UINT16 * height)
 * @brief	Otain video size .
 * @param[out] width	 Pointer to the  image width
 * @param[out] height 	 Pointer to the  image height
 * @retval none
 * @see sp1kVideoRecSizeSet()
 */
void sp1kVideoRecSizeGet(UINT16 * width, UINT16 * height);

  /**
 * @fn void sp1kVideoRecSizeSet(UINT16 size)
 * @brief	Set the video image size .The function must be called before video clip.
 * @param[in] size	 specify the video image size . 640: VGA(640X480), 320: QVGA(320X240)
 * @retval none
 * @see sp1kVideoRecSizeGet()
 */
void sp1kVideoRecSizeSet(UINT16 size);

  /**
 * @fn void sp1kVideoRecResume(void)
 * @brief	Resume video clip process
 * @see sp1kVideoRecPause()
 */
void sp1kVideoRecResume(void);

   /**
 * @fn void sp1kVideoRecPause(void)
 * @brief	Puse cideo clip process
 * @see sp1kVideoRecResume()
 */
void sp1kVideoRecPause(void);

  /**
 * @fn void sp1kVideoRecQTableSet(UINT8 value)
 * @brief	Setting video clip quality table .
 * @param[in] Secify the video clip quality level
 *\n            0: Super fine
 *\n            1: Fine
 *\n            2: Normal
 * @retval none
 */
void sp1kVideoRecQTableSet(UINT8 value);

 /**
 * @fn void sp1kVideoRecAudSampleRateSet(UINT16 rt)
 * @brief	Configure audio sample rate for video clip
 *
 *          The function must be called before video record,otherwise the sample rate will be unknown.
 * @param[in] rt 	audio sample rate - 48000,44100,32000,24000,22050,16000,12000,11025,8000.
 * @retval none
 */
void sp1kVideoRecAudSampleRateSet(UINT16 rt);

 /**
 * @fn void sp1kVideoRecAudSampleRateGet(UINT16* rt)
 * @brief	Get audio sample rate for video clip
 * @param[out] rt 	audio sample rate - 48000,44100,32000,24000,22050,16000,12000,11025,8000.
 * @retval none
 */
void sp1kVideoRecAudSampleRateGet(UINT16* rt);

 /**
 * @fn void sp1kVideoRecStampCfg(UINT8 mode, UINT8 aligFmt, UINT8 recMode)
 * @brief	Configure video stamp
 * @param[in] mode  Secify video stamp mode. See VDStampMode_t, they are VD_STAMP_MODE_DATE, VD_STAMP_MODE_IMG.
 *\n                and VD_STAMP_MODE_IMG_DATE.
 * @param[in] aligFmt  Secify video stamp display position both in horizontal and vertical use logic OR.
 *\n                   Horizontal positions are VD_STAMP_LEFT, VD_STAMP_CENTER and VD_STAMP_TOP.
 *\n                   Vertical positions are VD_STAMP_TOP, VD_STAMP_VCENTER, VD_STAMP_BOTTOM.
 * @param[in] recMode  Secify video stamp recording mode. See VDStampRecMode_t, they are VD_STAMP_REC_MODE_ALL_FRAME,
 *\n                   VD_STAMP_REC_MODE_FST_FRAME, VD_STAMP_REC_MODE_END_FRAME, and
 *\n                   VD_STAMP_REC_MODE_FST_END_FRAME.
 * @retval none
 * @see
 */
void sp1kVideoRecStampCfg(UINT8 mode, UINT8 aligFmt, UINT8 recMode);

 /**
 * @fn UINT8 sp1kVideoRecStampImgResLoad(UINT16 resFileId, UINT16 stampW, UINT16 stampH)
 * @brief	load image resource file for video stamp
 * @param[in] resFileId  Secify the video stamp's resource file id.
 *\n                     Fill black if resFikeId is zero.
 * @param[in] stampW  Secify stamp image width.
 * @param[in] stampH  Secify stamp image height.
 * @retval SUCCESS or FAIL
 * @see
 */
UINT8 sp1kVideoRecStampImgResLoad(UINT16 resFileId, UINT16 stampW, UINT16 stampH);

 /**
 * @fn void sp1kVideoRecStampStsSet(UINT8 en)
 * @brief	Enable or disable video stamp
 * @param[in] en  Secify video stamp status. 1:enable, 0:disable.
 * @retval none
 * @see sp1kVideoRecStampStsGet()
 */
void sp1kVideoRecStampStsSet (UINT8 en);

 /**
 * @fn UINT8 sp1kVideoRecStampStsGet(void)
 * @brief	Get if the stamp status
 * @param  none
 * @retval Video stamp status. 1:enable, 0:disable.
 * @see sp1kVideoRecStampStsSet()
 */
UINT8 sp1kVideoRecStampStsGet(void);

/**
 * @fn UINT8 sp1kVideoRecParamSet( UINT8 attrId, UINT16 value );
 * @brief	It configures video record parameter, such as video stamp setting.
 * @param[in] attrId	 the parameter id which is defined in VDParam_e
 * @param[in] value      the configure value of video record parameter
 * @retval error code -  success: 0, fail: 1
 * @see sp1kVideoRecParamGet()
 */
UINT8 sp1kVideoRecParamSet( UINT8 attrId, UINT16 value );

/**
 * @fn UINT16 sp1kVideoRecParamGet( UINT8 attrId );
 * @brief	It gets video record parameter.
 * @param[in] attrId	 the parameter id which is defined in VDParam_e
 * @retval the parameter value with 16 bit number
 * @see sp1kVideoRecParamSet()
 */
UINT16 sp1kVideoRecParamGet( UINT8 attrId );

 /**
 * @fn void sp1kVideoRecAudioStsSet(UINT8 en)
 * @brief	Enable or disable audio stream
 * @param[in] en  Secify audio stream status. 1:enable(default), 0:disable.
 * @retval none
 * @see sp1kVideoRecAudioStsGet()
 */
void sp1kVideoRecAudioStsSet(UINT8 en);

 /**
 * @fn UINT8 sp1kVideoRecAudioStsGet(void)
 * @brief	Get if the audio stream is enable
 * @param  none
 * @retval Audio stream status. 1:enable, 0:disable.
 * @see sp1kVideoRecAudioStsSet()
 */
UINT8 sp1kVideoRecAudioStsGet(void);

 /**
 * @fn void sp1kVideoRecBitRateSet(UINT32 bitRate)
 * @brief	Set bit rate for video recording. The bit rate control is disable in default.
 * @param[in] bitRate Specify bit rate in bit per second unit. The bit rate control is disable if bitRate is zero.
 * @retval none
 */
void sp1kVideoRecBitRateSet(UINT32 bitRate);

 /**
 * @fn void sp1kVideoPbStart(UINT16 ID)
 * @brief	Play a DCF-compatibile video file
 * @param[in] ID	 the DCF file index.
 * @retval none
 */
void sp1kVideoPbStart(UINT16 ID);

 /**
 * @fn void sp1kVideoPbStop(void)
 * @brief	Abort the video playback process
 * @retval none
 * @see sp1kVideoPbStart(), sp1kVideoPbResume()
 */
void sp1kVideoPbStop(void);

 /**
 * @fn void sp1kVideoPbStatusGet(void)
 * @brief	Obtain the video play status
 * @retval  the video play status
 *\n        0: idle
 *\n        1: normal run
 *\n        2: pause
 *\n        3: forward
 *\n        4: rewind
 * @see sp1kVideoPbStart()
 */

UINT8 sp1kVideoPbStatusGet(void);

  /**
 * @fn void sp1kVideoPbResume(void)
 * @brief	Resume the video playback operation
 * @see sp1kVideoPbStart(), sp1kVideoPbPause()
 */
void sp1kVideoPbResume(void);

  /**
 * @fn void sp1kVideoPbPause(void)
 * @brief	Pause the video playback process.
 * @see sp1kVideoPbStart(), sp1kVideoPbResume()
 */
void sp1kVideoPbPause(void);

  /**
 * @fn UINT8 sp1kVideoPbSpeedGet(void)
 * @brief	Get the  current video play speed
 * @retval current video play speed
 *\n        1:  1X
 *\n        2:  2x
 *\n        4:  4x:
 *\n        8:  8x
 * @see sp1kVideoPbFWSpeedSet(), sp1kVideoPbRWSpeedSet()
 */
UINT8 sp1kVideoPbSpeedGet(void);

 /**
 * @fn void sp1kVideoPbFWSpeedSet(UINT8 speed)
 * @brief	Set the forward speed of video play
 * @param[in] speed	Specifies forward speed.
 * @retval none
 * @see sp1kVideoPbRWSpeedSet()
 */
void sp1kVideoPbFWSpeedSet(UINT8 speed);

  /**
 * @fn void sp1kVideoPbRWSpeedSet(UINT8 speed)
 * @brief	Set the rewind  speed of video play
 * @param[in] speed	Specifies rewind speed.
 * @retval none
 * @see sp1kVideoPbFWSpeedSet()
 */

void sp1kVideoPbRWSpeedSet(UINT8 speed);

  /**
 * @fn void sp1kVideoPbSizeGet(UINT16 * phSize, UINT16 * pvSize)
 * @brief	Obtains video display buffer dimension
 * @param[out] phSize	the horizontal size of the display buffer
 * @param[out] pvSize	the vertical size of the display buffer
 * @retval none
 */

void sp1kVideoPbSizeGet(UINT16 * phSize, UINT16 * pvSize);

 /**
 * @fn void sp1kVideoPbSlowFWSpeedSet(UINT8 speed)
 * @brief	Set the slow forward speed of video play
 * @param[in] speed	Specifies forward speed.
 * @retval none
 * @see sp1kVideoPbFWSpeedSet()
 */

void sp1kVideoPbSlowFWSpeedSet(UINT8 speed);

  /**
 * @fn sp1kVideoRecYUVFormatSet(UINT8 yuvformat)
 * @brief	Set the YUV format when BRC
 * @param[in] speed	YUV format before JPEG.
 * @retval none
 * @see sp1kVideoRecYUVFormatSet()
 */

 void sp1kVideoRecYUVFormatSet(UINT8 yuvformat);

    /**
 * @fn sp1kVideoQTabUpdate(UINT8 qval[10])
 * @brief	Set the Q table when BRC
 * @param[in] speed	Q table.
 * @retval none
 * @see sp1kVideoQTabUpdate()
 */

 void sp1kVideoQTabUpdate(UINT8 qval[10]);

	/**
* @fn void sp1kVideoZoomResLoad(void)
* @brief   load video zoom table parameter resource file
* @retval the statring of system free physical buffer address in word unit
*/
void sp1kVideoZoomResLoad(UINT32 zoomFactorTbl,UINT32 regTbl,UINT32 regValueTbl);

/**
 * @fn void sp1kVideoSeamlessLPRecStsSet (UINT8 en)
 * @brief	Enable or disable VideoSeamlessLPRec
 * @param[in] en  Secify VideoSeamlessLPRec  status. 1:enable, 0:disable.
 * @retval none
 */
void sp1kVideoSeamlessLPRecStsSet (UINT8 en);
/**
 * @fn void sp1kVideoSeamlessLPRecFilesizeSet(UINT32 filesize)
 * @brief	 VideoSeamlessLPRec  each file size set
 * @param[in] en  Secify VideoSeamlessLPRec  file size.
 * @retval none
 */
void sp1kVideoSeamlessLPRecFilesizeSet(UINT32 filesize);
/**
 * @fn void sp1kVideoSeamlessLPRecTimeSet(UINT32 rectime)
 * @brief	 VideoSeamlessLPRec  each file rec time set
 * @param[in] en  Secify VideoSeamlessLPRec  rec time.
 * @retval none
 */
void sp1kVideoSeamlessLPRecTimeSet(UINT32 rectime);
/**
 * @fn void  sp1kVideoRecFrontRateSet
 * @brief	 video clip front rate interval set
 * @param[in] frameRate :  frame Rate.
 * @retval none
 */
void sp1kVideoRecFrontRateSet ( UINT8  frontRate );
/**
 * @fn void  sp1kVideoRecFrontRateGet
 * @brief	 video clip front rate interval Get
 * @param[in] frameRate :none
 * @retval none
 */
UINT8 sp1kVideoRecFrontRateGet();

/**
 * @fn UINT8 sp1kVideoSLRecMemCheck
 * @brief	 sp1kVideoSLRecMemCheck
 * @retval Card is full or not,and delete file.
 */
UINT8 sp1kVideoSLRecMemCheck();

/**
 * @fn UINT8 videoRecStampSet(UINT8 pasteThr, UINT16 stampW, UINT16 stampH, UINT16 fontW,UINT16 fontH);
 * @brief	 set stamp param and initialize stamp
 * @param[in]: pasteThr : used in hal_copy or Jpeg , y thr
 * @param[in]: stampW/H: set background width/height
 * @param[in]: fontW/H: set font width/height
 * @retval none
 */
UINT8 sp1kVideoRecStampSet(UINT8 pasteThr, UINT16 stampW, UINT16 stampH, UINT16 fontW,UINT16 fontH);
/**
 * @fn video_osd_text_t* videoRecOsdTextGet(stamp_text_type type);
 * @brief	 video rec osd text struct get
 * @param[in]: stamp_text_type type --select time/date
 * @retval none
 */
video_osd_text_t* sp1kVideoRecOsdTextGet(stamp_text_type type);
/**
 * @fn        UINT8 sp1kVideoRecStampSet(UINT16 stampW, UINT16 stampH, UINT16 fontW, tW,UINT16 fontH)
 * @brief     set stamp w,h and font w,h
 * @param     [in] stampW
 * @param     [in] stampH
 * @param     [in] fontW
 * @param     [in] fontH
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-12
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kVideoRecStampSet(UINT16 stampW, UINT16 stampH, UINT16 fontW,UINT16 fontH);

/**
 * @fn       void sp1kVideoRecFrameBufNumSet(UINT8 bufnum)
 * @brief     Set pv frame buf num
 * @param     [in] bufnum
 * @see       NULL
 * @author    rf.wang
 * @since     2011-8-22
 * @todo      N/A
 * @bug       N/A
*/
void sp1kVideoRecFrameBufNumSet(UINT8 bufnum);
/**
 * @fn       UINT8 sp1kVideoRecFrameBufNumGet()
 * @brief     Get pv frame buf num
 * @param     [in] NONE
 * @param[out] bufnum:pv frame buf num
 * @see       NULL
 * @author    rf.wang
 * @since     2011-8-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kVideoRecFrameBufNumGet(UINT8 bufnum);
/**
 * @fn       UINT8 sp1kVidRecFramebufModeGet();
 * @brief     Get pv frame buf num,this function is just used to decide whether to change frame buf or not
 * @param     [in] NONE
 * @param[out] bufnum:pv frame buf num
 * @see       NULL
 * @author    rf.wang
 * @since     2011-8-22
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1KVideoRecFramebufModeGet();
/**
 * @fn      void sp1kVidRecFramebufModeSet(UINT8 mode);
 * @brief     Set pv frame buf num,this function is just used to decide whether to change frame buf or not
 * @param     [in] NONE
 * @param[out] bufnum:pv frame buf num
 * @see       NULL
 * @author    rf.wang
 * @since     2011-8-22
 * @todo      N/A
 * @bug       N/A
*/
void sp1kVideoRecFramebufModeSet(UINT8 mode);

/**
 * @fn		UINT8 sp1kVideoCustomUpdate(UINT32 dramAddrW);
 * @brief		Update data which saved in the dram.
 * \n			The data will be saved at the end of video frame chunk and its index flag is 0x00000000.
 * \n			The destination offset and length are 4byte alignment.
 * @param	[in] dramAddrW	Specify the dram addr of data. The first 2 byte field specifies data length which include itself.
 * @retval 	SUCCESS/FAIL
 * @see 		NULL
*/
UINT8 sp1kVideoCustomUpdate(UINT32 dramAddrW);

/**
 * @fn		UINT32 sp1kVideoRecRemainTimeGet();
 * @brief		return remain time of rec(s)
 * @retval 	SUCCESS/FAIL
 * @see 		NULL
*/
UINT32 sp1kVideoRecRemainTimeGet();

/**
 * @fn		  UINT16 sp1kVideoZoomCtrlParaGet(UINT8 item)
 * @brief	  video view zoom system crtrl parameter get
 * @param	 item : typedef enum SP1K_VIDEO_ZOOM_CTRL_PARA
 * @retval	  parameter value (UINT16)
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
UINT16 sp1kVideoZoomCtrlParaGet(UINT8 item);

/**
 * @fn		  void sp1kVideoZoomCtrlParaSet(UINT8 item,	UINT16 value)
 * @brief	  video view zoom system crtrl parameter set
 * @param	 item : typedef enum SP1K_VIDEO_ZOOM_CTRL_PARA
 * @param	  value : parameter value (UINT16)
 * @retval	  NONE
 * @author	  LinJieCheng
 * @since	  2010-10-07
*/
void
sp1kVideoZoomCtrlParaSet(UINT8 item,UINT16 value);
#else
#define	sp1kVideoRecStatusGet    videoRecStatusGet
#define sp1kVideoRecStart   videoRecStart
#define sp1kVideoRecStop   videoRecStop
#define sp1kvideoRecTimeGet  videoRecTimeGet
#define sp1kvideoRecAudFmtSet videoRecAudFmtSet
#define sp1kVideoRecFrameRateSet videoRecFrameRateSet
#define sp1kVideoRecAviFrameRateSet videoRecAviFrameRateSet
#define sp1kVideoRecSizeGet     videoRecSizeGet
#define sp1kVideoRecSizeSet     videoRecSizeSet
#define sp1kVideoRecResume videoRecResume
#define sp1kVideoRecPause  videoRecPause
#define sp1kVideoRecQTableSet   videoRecQTableSet
#define sp1kVideoPvFreeAddrGet videoPvFreeAddrGet
#define sp1kVideoRecAudSampleRateSet videoRecAudSampleRateSet
#define sp1kVideoRecAudSampleRateGet videoRecAudSampleRateGet

#define sp1kVideoRecStampCfg videoRecStampCfg
#define sp1kVideoRecStampImgResLoad videoRecStampImgResLoad
#define sp1kVideoRecStampStsSet videoRecStampStsSet
#define sp1kVideoRecStampStsGet videoRecStampStsGet
#define sp1kVideoRecStampDateFmtSet videoRecStampDateFmtSet
#define sp1kVideoRecStampDateFmtGet videoRecStampDateFmtGet
#define sp1kVideoRecStampAligFmtGet		videoRecStampAligFmtGet


#define sp1kVideoRecParamSet videoRecParamSet
#define sp1kVideoRecParamGet videoRecParamGet

#define sp1kVideoRecAudioStsSet videoRecAudioStsSet
#define sp1kVideoRecAudioStsGet videoRecAudioStsGet

#define sp1kVideoRecBitRateSet videoRecBitRateSet
#define sp1kVideoRecBitRateGet videoRecBitRateGet

#define sp1kVideoRecYUVFormatSet     videoRecYUVFormatSet
#define sp1kVideoQTabUpdate    videoQTabUpdate

#define sp1kVideoRecZoomFacSet(para1,para2)  zoomDevZoomSet(SP1K_ZOOM_LOGI_VIDEO_MODE,0,para1,para2)
#define sp1kVideoZoomFctGet()     com_zoomDevZoomFactorGet(SP1K_ZOOM_LOGI_VIDEO_MODE)
#define sp1kVideoZoomResLoad(para1,para2,para3)	zoomDevResourceLoad(SP1K_ZOOM_LOGI_VIDEO_MODE,para1,para2,para3)

#define sp1kVideoSmoothZoomSet(para1)	zoomDevZoomSmoothSet(SP1K_ZOOM_LOGI_VIDEO_MODE,para1)//videoSmoothZoomSet
#define sp1kVideoSmoothZoomGet() com_zoomDevZoomSmoothGet(SP1K_ZOOM_LOGI_VIDEO_MODE)//videoSmoothZoomGet

#define sp1kVideoSmoothZoomStatusGet() com_zoomDevZoomSmoothStatusGet(SP1K_ZOOM_LOGI_VIDEO_MODE)//VideoSmoothZoomStatusGet

#define sp1kVideoZoom3APreSet()	 zoomDevZoomSmooth3APreSet(SP1K_ZOOM_LOGI_VIDEO_MODE)//videoZoom3APreSet
#define sp1kVideoZoom3APostSet()	zoomDevZoomSmooth3APostSet(SP1K_ZOOM_LOGI_VIDEO_MODE)//videoZoom3APostSet

#define sp1kVideoTeleSet()		zoomDevZoomSmoothTeleSet(SP1K_ZOOM_LOGI_VIDEO_MODE)//videoTeleSet
#define sp1kVideoWideSet()		zoomDevZoomSmoothWideSet(SP1K_ZOOM_LOGI_VIDEO_MODE)//videoWideSet

#define sp1kVideoZoomCtrlParaSet(para1,para2)  zoomDevZoomCtrlParaSet(SP1K_ZOOM_LOGI_VIDEO_MODE,para1,para2)//videoZoomCtrlParaSet
#define sp1kVideoZoomCtrlParaGet(para1) com_zoomDevZoomCtrlParaGet(SP1K_ZOOM_LOGI_VIDEO_MODE,para1)//videoZoomCtrlParaGet

#define sp1kVideoStorageWriteSpeedTest	videoStorageWriteSpeedTest

#define sp1kVideoPbResume  		videoPbResume
#define sp1kVideoPbPause   		videoPbPause
#define sp1kVideoPbSpeedGet     videoPbSpeedGet
#define sp1kVideoPbStatusGet    videoPbStatusGet
#define sp1kVideoPbFWSpeedSet   videoPbFWSpeedSet
#define sp1kVideoPbRWSpeedSet   videoPbRWSpeedSet
#define sp1kVideoPbSizeGet      videoPbSizeGet
#define sp1kVideoPbStart    videoPbStart
#define sp1kVideoPbStop  		videoPbStop

#define sp1kVideoPbSlowFWSpeedSet   videoPbSlowFWSpeedSet
#define sp1kVideoPbSlowRWSpeedSet	videoPbSlowRWSpeedSet

#endif
#define sp1kVideoSeamlessLPRecStsSet videoSeamlessLPRecStsSet
#define sp1kVideoSeamlessLPRecFilesizeSet videoSeamlessLPRecFilesizeSet
#define sp1kVideoSeamlessLPRecTimeSet  videoSeamlessLPRecTimeSet
#define sp1kVideoRecFrontRateSet  videoRecFrontRateInterval
#define sp1kVideoRecFrontRateGet  videoRecFrontRateGet
#define sp1kVideoSLRecMemCheck videoSLRecMemCheck
#define sp1kVideoRecStampSet videoRecStampSet
#define sp1kVideoRecOsdTextGet videoRecOsdTextGet
#define sp1kVideoRecFrameBufNumSet vidRecFrameBufNumSet
#define sp1kVideoRecFrameBufNumGet vidRecFrameBufNumGet
#define sp1KVideoRecFramebufModeGet  vidRecFramebufModeGet
#define sp1KVideoRecFramebufModeSet  vidRecFramebufModeSet
#define sp1kVideoCustomUpdate 	videoCustomUpdate
#define sp1kVideoRecRemainTimeGet videoRecRemainTimeGet
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

#endif //__SP1K_VIDEO_API_H__

