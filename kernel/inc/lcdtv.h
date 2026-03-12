/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2007 by Sunplus mMedia Inc.                      *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  mMedia Inc. All rights are reserved by Sunplus mMedia Inc.  This      *
 *  software may only be used in accordance with the corresponding        *
 *  license agreement. Any unauthorized use, duplication, distribution,   *
 *  or disclosure of this software is expressly forbidden.                *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus mMedia Inc.                                *
 *                                                                        *
 *  Sunplus mMedia Inc. reserves the right to modify this software        *
 *  without notice.                                                       *
 *                                                                        *
 *  Sunplus mMedia Inc.                                                   *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __LCDTV_H__
#define __LCDTV_H__

//------------------------------------------------------------------------------
//Constant
//------------------------------------------------------------------------------
//LCD/TV encoder output control
#define K_DISPLAY_CompTvNtsc    0 //0x00
#define K_DISPLAY_CompTvPal     1 //0x01
#define K_DISPLAY_Ccir656Ntsc   2 //0x02
#define K_DISPLAY_Ccir656Pal    3 //0x03
#define K_DISPLAY_Ccir601Ntsc8  4 //0x04
#define K_DISPLAY_Ccir601Pal8   5 //0x05
#define K_DISPLAY_Ccir601Ntsc16 6 //0x06
#define K_DISPLAY_Ccir601Pal16  7 //0x07
#define K_DISPLAY_Ups051        8 //0x08
#define K_DISPLAY_Epson         9 //0x09
#define K_DISPLAY_Casio         10 //0x0a
#define K_DISPLAY_StnLcd        11 //0x0b
#define K_DISPLAY_VgaTftLcd     12 //0x0c
#define K_DISPLAY_Au015An02     13 //0x0d
#define K_DISPLAY_StnLcd160     14 //0x0e
#define K_DISPLAY_Svga          15 //0x0f
#define K_DISPLAY_Au015Bl01     16 //0x10	//patch5.2.1@cwh@AU015BL01;2003/08/08
#define K_DISPLAY_Tc015Treb1    17 //0x11	//patch5.2.1@cwh@Toppoly TC015TREB1;2003/08/27
// patch 5.2.1_28@mattwang@rename CASIO2G to CASIO2G1181 beg
#define K_DISPLAY_Casio2G1181   18 //0x12	//patch5.2.1@cwh@Casio2G ;2003/09/24
// patch 5.2.1_28@mattwang@rename CASIO2G to CASIO2G1181 end
// patch 5.2.1_28@mattwang@add CASIO2G1163 LCD pannel beg
#define K_DISPLAY_Casio2G1163   19 //0x13
// patch 5.2.1_28@mattwang@add CASIO2G1163 LCD pannel end
// patch 5.2.1_30@mattwang@add STNLCD128 LCD pannel beg
#define K_DISPLAY_StnLcd128     20 //0x14
// patch 5.2.1_30@mattwang@add STNLCD128 LCD pannel end
// patch 5.2.1_33@mattwang@add AU015AN04 LCD pannel beg
#define K_DISPLAY_Au015An04     21 //0x15
// patch 5.2.1_33@mattwang@add AU015AN04 LCD pannel end

//Resolution of LCD/TV
#define K_DISPLAY_TvNtscWidth           ((UINT16)720)
#define K_DISPLAY_TvNtscHeight          ((UINT16)480)
#define K_DISPLAY_TvPalWidth            ((UINT16)720)
#define K_DISPLAY_TvPalHeight           ((UINT16)576)
#define K_DISPLAY_Ups051Width           ((UINT16)288)
#define K_DISPLAY_Ups051Height          ((UINT16)224)
#define K_DISPLAY_Au015An02Width        ((UINT16)312)
#define K_DISPLAY_Au015An02Height       ((UINT16)220)
#define K_DISPLAY_EpsonWidth            ((UINT16)312)
#define K_DISPLAY_EpsonHeight           ((UINT16)232)
#define K_DISPLAY_CasioWidth            ((UINT16)356)
#define K_DISPLAY_CasioHeight           ((UINT16)240)
#define K_DISPLAY_StnLcdWidth           ((UINT16)240)
#define K_DISPLAY_StnLcdHeight          ((UINT16)160)
#define K_DISPLAY_StnLcd160Width        ((UINT16)160)
#define K_DISPLAY_StnLcd160Height       ((UINT16)120)
#define K_DISPLAY_VgaTftLcdWidth        ((UINT16)640)
#define K_DISPLAY_VgaTftLcdHeight       ((UINT16)480)
#define K_DISPLAY_SvgaWidth             ((UINT16)800)
#define K_DISPLAY_SvgaHeight            ((UINT16)600)
#define K_DISPLAY_Au015Bl01Width        ((UINT16)520)	//patch5.2.1@cwh@AU015BL01;2003/08/08
#define K_DISPLAY_Au015Bl01Height       ((UINT16)250)
// patch 5.2.1_28@mattwang@rename CASIO2G to CASIO2G1181 beg
#define K_DISPLAY_Casio2G1181Width      ((UINT16)480)	//patch5.2.1@cwh@CASIO2G;2003/09/24
#define K_DISPLAY_Casio2G1181Height     ((UINT16)240)
// patch 5.2.1_28@mattwang@rename CASIO2G to CASIO2G1181 end
// patch 5.2.1_28@mattwang@add CASIO2G1163 LCD pannel beg
#define K_DISPLAY_Casio2G1163Width      ((UINT16)280)
#define K_DISPLAY_Casio2G1163Height     ((UINT16)226)
// patch 5.2.1_28@mattwang@add CASIO2G1163 LCD pannel end
// patch 5.2.1_30@mattwang@add STNLCD128 LCD pannel beg
#define K_DISPLAY_StnLcd128Width        ((UINT16)128)
#define K_DISPLAY_StnLcd128Height       ((UINT16)128)
// patch 5.2.1_30@mattwang@add STNLCD128 LCD pannel end
// patch 5.2.1_33@mattwang@add AU015AN04 LCD pannel beg
// patch 5.2.1_35@mattwang@modify AU015AN04 dimension beg
#define K_DISPLAY_Au015An04Width        ((UINT16)340)
#define K_DISPLAY_Au015An04Height       ((UINT16)232) //220)  // 20040805 mattwang modify
// patch 5.2.1_35@mattwang@modify AU015AN04 dimension end
// patch 5.2.1_33@mattwang@add AU015AN04 LCD pannel end

//yichang@0503

#define K_TmbCasioNineWidth                ((UINT16)108)
#define K_TmbCasioNineHeight               ((UINT16)72)
//patch4.4@richie@casio
//K_TmbCasioXGap change to 8 for color mistake when playback thumbnail in casio panel
#define K_TmbCasioXGap                     ((UINT16)8)
#define K_TmbCasioYGap                     ((UINT16)6)

#define K_TmbStnLcdNineWidth               ((UINT16)72)
#define K_TmbStnLcdNineHeight              ((UINT16)48)
#define K_TmbStnLcdXGap                    ((UINT16)6)
#define K_TmbStnLcdYGap                    ((UINT16)4)

#define K_TmbTvNtscNineWidth               ((UINT16)216)
#define K_TmbTvNtscNineHeight              ((UINT16)144)
#define K_TmbTvNtscXGap                    ((UINT16)18)
#define K_TmbTvNtscYGap                    ((UINT16)12)

#define K_TmbTvPalNineWidth               ((UINT16)216)
#define K_TmbTvPalNineHeight              ((UINT16)160)
#define K_TmbTvPalXGap                    ((UINT16)18)
#define K_TmbTvPalYGap                    ((UINT16)24)

#define K_TmbUps051NineWidth               ((UINT16)80)
#define K_TmbUps051NineHeight              ((UINT16)60)
#define K_TmbUps051XGap                    ((UINT16)18)
#define K_TmbUps051YGap                    ((UINT16)11)

#define K_TmbAu015NineWidth               ((UINT16)96)
#define K_TmbAu015NineHeight              ((UINT16)60)
#define K_TmbAu015XGap                    ((UINT16)6)
#define K_TmbAu015YGap                    ((UINT16)10)

#define K_TmbEpsonNineWidth               ((UINT16)96)
#define K_TmbEpsonNineHeight              ((UINT16)72)
#define K_TmbEpsonXGap                    ((UINT16)6)
#define K_TmbEpsonYGap                    ((UINT16)4)

#define K_TmbStnLcd160NineWidth           ((UINT16)48)
#define K_TmbStnLcd160NineHeight          ((UINT16)32)
#define K_TmbStnLcd160XGap                ((UINT16)4)
#define K_TmbStnLcd160YGap                ((UINT16)6)

#define K_TmbVgaTftLcdNineWidth           ((UINT16)200)
#define K_TmbVgaTftLcdNineHeight          ((UINT16)144)
#define K_TmbVgaTftLcdXGap                ((UINT16)10)
#define K_TmbVgaTftLcdYGap                ((UINT16)12)

#define K_TmbSvgaNineWidth                ((UINT16)240)
#define K_TmbSvgaNineHeight               ((UINT16)180)
#define K_TmbSvgaXGap                     ((UINT16)20)
#define K_TmbSvgaYGap                     ((UINT16)20)

#define K_TmbAu015Bl01NineWidth               ((UINT16)148)	//patch5.2.1@cwh@AU015BL01;2003/08/08
#define K_TmbAu015Bl01NineHeight              ((UINT16)68)
#define K_TmbAu015Bl01XGap                    ((UINT16)16)
#define K_TmbAu015Bl01YGap                    ((UINT16)10)

// patch 5.2.1_28@mattwang@rename CASIO2G to CASIO2G1181 beg
#define K_TmbCasio2G1181NineWidth               ((UINT16)144)	//patch5.2.1@cwh@CASIO2G;2003/09/24
#define K_TmbCasio2G1181NineHeight              ((UINT16)68)
#define K_TmbCasio2G1181XGap                    ((UINT16)10)
#define K_TmbCasio2G1181YGap                    ((UINT16)8)
// patch 5.2.1_28@mattwang@rename CASIO2G to CASIO2G1181 end

// patch 5.2.1_28@mattwang@add CASIO2G1163 LCD pannel beg
#define K_TmbCasio2G1163NineWidth               ((UINT16)76)
#define K_TmbCasio2G1163NineHeight              ((UINT16)64)
#define K_TmbCasio2G1163XGap                    ((UINT16)12)
#define K_TmbCasio2G1163YGap                    ((UINT16)8)
// patch 5.2.1_28@mattwang@add CASIO2G1163 LCD pannel end

// patch 5.2.1_30@mattwang@add STNLCD128 LCD pannel beg
#define K_TmbStnLcd128NineWidth           ((UINT16)38)
#define K_TmbStnLcd128NineHeight          ((UINT16)38)
#define K_TmbStnLcd128XGap                ((UINT16)2)
#define K_TmbStnLcd128YGap                ((UINT16)2)
// patch 5.2.1_30@mattwang@add STNLCD128 LCD pannel end

// patch 5.2.1_33@mattwang@add AU015AN04 LCD pannel beg
#define K_TmbAu015An04NineWidth               ((UINT16)92)
#define K_TmbAu015An04NineHeight              ((UINT16)60)
#define K_TmbAu015An04XGap                    ((UINT16)8)
#define K_TmbAu015An04YGap                    ((UINT16)10)
// patch 5.2.1_33@mattwang@add AU015AN04 LCD pannel end

//------------------------------------------------------------------------------
//External Link
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//Function Prototype
//------------------------------------------------------------------------------

#endif /*__LCDTV_H__*/