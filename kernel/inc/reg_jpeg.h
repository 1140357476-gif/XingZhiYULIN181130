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
 * @file        reg_jpeg.h
 * @brief     jpeg  register definition
 * @author    rf.wang
 * @since     2011-02-28
 * @date      2011-02-28
*/
#ifndef __REG_JPEG_H__
#define __REG_JPEG_H__

/**************************************************************************
 *                  R E G I S T E R  A D D R E S S
 **************************************************************************/ 

//Interupt Control
#define REG_Jpeg_Intr_Event         (0x2800)
#define REG_Jpeg_Intr_Enable        (0x2804)

//Clock/Reset Control
#define REG_Jpeg_DisGatedClk        (0x2808)
#define REG_Jpeg_Swreset            (0x280c)

//JPEG Control
#define REG_Jpeg_Swcontrol          (0x2810)
#define REG_Jpeg_JpgCyc             (0x2812)
#define REG_Jpeg_YUV_Type           (0x2814)
#define REG_Jpeg_Sclbypss           (0x2815)
#define REG_Jpeg_Turboen            (0x2816)
#define REG_Jpeg_ImgWidth_Low       (0x2818)
#define REG_Jpeg_ImgWidth_High      (0x2819)
#define REG_Jpeg_ImgHeight_Low      (0x281a)
#define REG_Jpeg_ImgHeight_High     (0x281b)
#define REG_Jpeg_DstWidth_Low       (0x2820)
#define REG_Jpeg_DstWidth_High      (0x2821)
#define REG_Jpeg_DstHeight_Low      (0x2822)
#define REG_Jpeg_DstHeight_High     (0x2823)
#define REG_Jpeg_Rotation_Type      (0x2824)
#define REG_Jpeg_PbreScale          (0x2825)
#define REG_Jpeg_ZfPop              (0x2826)
#define REG_Jpeg_Hsf_Low            (0x2828)
#define REG_Jpeg_Hsf_High           (0x2829)
#define REG_Jpeg_Vsf_Low            (0x282a)
#define REG_Jpeg_Vsf_High           (0x282b)
#define REG_Jpeg_Hsfini_Low         (0x282c)
#define REG_Jpeg_Hsfini_High        (0x282d)
#define REG_Jpeg_Vsfini_Low         (0x282e)
#define REG_Jpeg_Vsfini_High        (0x282f)
#define REG_Jpeg_Noqtbl             (0x2830)
#define REG_Jpeg_Qvalueone          (0x2831)
#define REG_Jpeg_Enthumb            (0x2832)
#define REG_Jpeg_JFIF               (0x2833)
#define REG_Jpeg_Truncated          (0x2834)
#define REG_Jpeg_VLCbit             (0x2835)
#define REG_Jpeg_VLCSize_Byte0      (0x2838)
#define REG_Jpeg_VLCSize_Byte1      (0x2839)
#define REG_Jpeg_VLCSize_Byte2      (0x283a)
#define REG_Jpeg_JFIFend            (0x283b)
#define REG_Jpeg_Restartmcu_Low     (0x2840)
#define REG_Jpeg_Restartmcu_High    (0x2841)
#define REG_Jpeg_Blockend           (0x2842)
#define REG_Jpeg_StopEN             (0x2843)
#define REG_Jpeg_VLCBounden         (0x2844)
#define REG_Jpeg_VLCofset_Low       (0x2846)
#define REG_Jpeg_VLCofset_High      (0x2847)
#define REG_Jpeg_VLCBound_Byte0     (0x2848)
#define REG_Jpeg_VLCBound_Byte1     (0x2849)
#define REG_Jpeg_VLCBound_Byte2     (0x284a)
#define REG_Jpeg_Currenty           (0x284c)
#define REG_Jpeg_Mebistfail         (0x284e)
#define REG_Jpeg_Mebisterrmap       (0x284f)
#define REG_Jpeg_StampEn            (0x2850)
#define REG_Jpeg_Stampthr           (0x2851)
#define REG_Jpeg_Stamhoffset_Low    (0x2852)
#define REG_Jpeg_Stamhoffset_High   (0x2853)
#define REG_Jpeg_Stamvoffset_Low    (0x2854)
#define REG_Jpeg_Stamvoffset_High   (0x2855)
#define REG_Jpeg_Stmhsize           (0x2856)
#define REG_Jpeg_Stamvsize_Low      (0x2858)
#define REG_Jpeg_Stamvsize_High     (0x2859)
#define REG_Jpeg_Bistmode           (0x285a)
#define REG_Jpeg_Bistfail           (0x285b)
#define REG_Jpeg_Bisterrmap         (0x285c)
#define REG_Jpeg_Bistfinish         (0x285d)
#define REG_Jpeg_Tselect            (0x285e)
#define REG_Jpeg_Jpgprobe           (0x285f)
#define REG_Jpeg_Srcwidth_Low       (0x2860)
#define REG_Jpeg_Srcwidth_High      (0x2861)
#define REG_Jpeg_Srcheight_Low      (0x2862)
#define REG_Jpeg_Srcheight_High     (0x2863)
#define REG_Jpeg_Offsetx_Low        (0x2864)
#define REG_Jpeg_Offsetx_High       (0x2865)
#define REG_Jpeg_Offsety_Low        (0x2866)
#define REG_Jpeg_Offsety_High       (0x2867)
#define REG_Jpeg_Scl_Mcux           (0x2868)
#define REG_Jpeg_Scl_Mcuy_Low       (0x286a)
#define REG_Jpeg_Scl_Mcuy_High      (0x286b)
#define REG_Jpeg_Rec_Mcux           (0x286c)
#define REG_Jpeg_Rec_Mcuy_Low       (0x286e)
#define REG_Jpeg_Rec_Mcuy_High      (0x286f)
//ME engone
#define REG_Jpeg_Meoutaddr_Byte0    (0x2870)
#define REG_Jpeg_Meoutaddr_Byte1    (0x2871)
#define REG_Jpeg_Meoutaddr_Byte2    (0x2872)
#define REG_Jpeg_Meoutaddr_Byte3    (0x2873)
#define REG_Jpeg_Meinterval         (0x2874)
#define REG_Jpeg_Bnumber            (0x2875)
#define REG_Jpeg_Mestart            (0x2876)
#define REG_Jpeg_Memode             (0x2877)
#define REG_Jpeg_Sramtestsel        (0x2878)
#define REG_Jpeg_Reserved           (0x287c)
#define REG_Jpeg_DateCode           (0x287e)
// Q-table
#define REG_Jpeg_Yqtable            (0x2880)
#define REG_Jpeg_Cqtable            (0x28c0)

#endif


