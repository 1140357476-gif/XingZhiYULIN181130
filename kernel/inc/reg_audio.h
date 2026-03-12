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
 * @file        reg_audio.h
 * @brief      Audio register definition
 * @author   kai.wang
 * @since     2011-02-23
 * @date      2011-02-23
*/

#ifndef __REG_AUDIO_H__
#define __REG_AUDIO_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	REG_Audio_DataThdIntSts	 (0x2600)
#define REG_Audio_VolumeIntSts   (0x2601)
#define REG_Audio_AudAdcIntsts   (0x2602)
#define REG_Audio_SarAdcIntsts   (0x2603)
#define REG_Audio_DataThdIntEn   (0x2608)
#define REG_Audio_VolumeIntEn    (0x2609)
#define REG_Audio_AudAdcIntEn    (0x260a)
#define REG_Audio_SarAdcIntEn    (0x260b)

#define REG_Audio_StartAddr0	 (0x2610)
#define REG_Audio_StartAddr1	 (0x2611)
#define REG_Audio_StartAddr2	 (0x2612)
#define REG_Audio_StartAddr3	 (0x2613)
#define REG_Audio_BufSize0       (0x2614)
#define REG_Audio_BufSize1       (0x2615)
#define REG_Audio_BufSize2       (0x2616)
#define REG_Audio_DataCnt0       (0x2618)
#define REG_Audio_DataCnt1       (0x2619)
#define REG_Audio_DataCnt2       (0x261a)
#define REG_Audio_SnapRingIdx    (0x261b)
#define REG_Audio_RingBufInIdx0  (0x2620)
#define REG_Audio_RingBufInIdx1  (0x2621)
#define REG_Audio_RingBufInIdx2  (0x2622)
#define REG_Audio_RingBufOutIdx0 (0x2624)
#define REG_Audio_RingBufOutIdx1 (0x2625)
#define REG_Audio_RingBufOutIdx2 (0x2626)

#define REG_Audio_AudDataCnthd0  (0x2628)
#define REG_Audio_AudDataCnthd1  (0x2629)
#define REG_Audio_AudDataCnthd2  (0x262a)
#define REG_Audio_AudBufOvf      (0x262c)  //overflow
#define REG_Audio_AudBufUnf      (0x262d)  //underflow
#define REG_Auido_AudDramMode    (0x262e)
#define REG_Audio_AgcFuncEN      (0x2630)
#define REG_Audio_AlcSts         (0x2631)
#define REG_Audio_WindowSize0    (0x2632)
#define REG_Audio_WindowSize1    (0x2633)
#define REG_Audio_AgcLowThd0     (0x2634)
#define REG_Audio_AgcLowThd1     (0x2635)
#define REG_Audio_AgcHighThd0    (0x2636)
#define REG_Audio_AgcHighThd1    (0x2637)
#define REG_Audio_AgcValue0      (0x2638)
#define REG_Audio_AgcValue1      (0x2639)
#define REG_Audio_AgcValue2      (0x263a)
#define REG_Audio_AgcValue3      (0x263b)
#define REG_Audio_FrameCnt       (0x263c)
#define REG_Audio_SaturatinCnt   (0x263d)
#define REG_Audio_SlienceCnt     (0x263e)
#define REG_Audio_AudAdcPwrEn    (0x2640)
#define REG_Audio_AmplifierGain  (0x2641)
#define REG_Audio_AdcHpEn        (0x2642)
#define REG_Audio_Adovrs         (0x2643)
#define REG_Audio_PgaZCDetecEn   (0x2644)
#define REG_Audio_TestMode       (0x2645)
#define REG_Audio_SarAdcPwrEn    (0x2648)
#define REG_Audio_SarChnlSel     (0x2649)
#define REG_Audio_SarClkSel      (0x264a)
#define REG_Audio_TpChnlSel      (0x264b)
#define REG_Audio_SnapSarAdcVal0  (0x264c)
#define REG_Audio_SnapSarAdcVal1  (0x264d)
#define REG_Audio_SarDelayCnt0   (0x264e)
#define REG_Audio_SarDelayCnt1   (0x264f)
#define REG_Audio_AudPause       (0x2650)
#define REG_Audio_AdpcmCoderEn   (0x2654)
#define REG_Audio_PageMode       (0x2655)
#define REG_Audio_AudStereoMode  (0x2658)
#define REG_Audio_AudRst         (0x2660)
#define REG_Audio_AudEn          (0x2668)
#define REG_Audio_AudDACPwrEn    (0x2670)
#define REG_Audio_DACDataFmt     (0x2671)
#define REG_Audio_AutoMute       (0x2672)
#define REG_Audio_DacSDMDither   (0x2673)
#define REG_Audio_SpeakerGain    (0x2674)
#define REG_Audio_LineoutGain    (0x2675)
#define REG_Audio_OutputVerf     (0x2676)
#define REG_Audio_VrefPwrEn      (0x2677)
#define REG_Audio_I2SRecMode     (0x2688)
#define REG_Audio_I2SBClkSrc     (0x2689)
#define REG_Audio_PlyLRSwap      (0x268c)
#define REG_Audio_RecLRSwap      (0x268d)
#define REG_Audio_DeboDelayCnt0  (0x269e)
#define REG_Audio_DeboDelayCnt1  (0X269f)
#define REG_Audio_DigtalGain0    (0x26c7)
#define REG_Audio_DigtalGain1    (0x26c8)


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

#endif  /* __REG_DISP_H__ */
