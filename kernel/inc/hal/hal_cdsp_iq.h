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
//-----------------------------------------------------------------------------
// Header file
//-----------------------------------------------------------------------------
#ifndef __HAL_CDSP_IQ_H__
#define __HAL_CDSP_IQ_H__

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Function Decleration
//-----------------------------------------------------------------------------

void HAL_CdspBayerPatternSet(UINT8 ptn);

UINT8 HAL_CdspColorMtxSet( UINT16 a00, UINT16 a01, UINT16 a02, UINT16 a10, UINT16 a11, UINT16 a12, UINT16 a20, UINT16 a21, UINT16 a22 );

UINT8 HAL_CdspBriContEnSet(UINT8 ybycen);
UINT8 HAL_CdspYThrEnSet(UINT8 ythren);
UINT8 HAL_CdspYRevrseEnSet(UINT8 yreven);
UINT8 HAL_CdspYThrSet(UINT8 ythr);

UINT8 HAL_CdspYBrightOffsetSet(SINT8 yb);
UINT8 HAL_CdspYContrastOffsetSet(UINT8 yc);

UINT8 HAL_CdspSatHueEnSet(UINT8 enable);
UINT8 HAL_CdspSatHueParaSet(UINT8 uvsat,UINT8 huesin1data,UINT8 huecosdata,UINT8 huesin2data,UINT8 uOffset ,UINT8 vOffset);


/* WB Offset */
UINT8 HAL_CdspWBGainEnSet( UINT8 enable );
UINT8 HAL_CdspWBGainEnGet( void );
UINT8 HAL_CdspWBOffsetEnSet( UINT8 enable );
UINT8 HAL_CdspWBOffsetEnGet( void );
UINT8 HAL_CdspWBGainSet(UINT16 rGain, UINT16 grGain, UINT16 bGain, UINT16 gbGain);
UINT8 HAL_CdspWBGainGet(UINT8 *rGain, UINT8 *grGain, UINT8 *bGain, UINT8 *gbGain);
UINT8 HAL_CdspWBOffsetSet( UINT16 rOffset, UINT16 grOffset, UINT16 bOffset, UINT16 gbOffset);
UINT8 HAL_CdspWBOffsetGet( UINT16 *rOffset, UINT16 *grOffset, UINT16 *bOffset, UINT16 *gbOffset);
UINT8 HAL_CdspWBOffsetCfgSet(UINT8 gOsVdEn, UINT8 wbOValidOn);



/* Gamma */
UINT8 HAL_CdspGammaEnSet( UINT8 enable );
UINT8 HAL_CdspGammaEnGet( void );
UINT8 HAL_CdspGammalTbInit(UINT8 *pGammaData);

/* Bad Pixel */
UINT8 HAL_CdspBPEnSet( UINT8 enable );
UINT8 HAL_CdspBPEnGet( void );



/*Lens Correction*/
//UINT8 halCdspLCEnableSet( UINT8 enable );
UINT8 HAL_CdspLscEnSetGet( void );
UINT8 HAL_CdspLscWinSizeSet(UINT8 lscHSize);
UINT8 HAL_CdspLscWinStepSet(UINT8 lscHStep,UINT8 lscVStep);
//void HAL_CdspLscWinStepGet(UINT8 *lscHStep,UINT8 *lscVStep);

UINT8 HAL_CdspLscWinIncStepSet(UINT8 lscHOIncStep,UINT8 lscHEIncStep,UINT8 lscVOIncStep,UINT8 lscVEIncStep);
UINT8 HAL_CdspLscResolution(UINT8 lscRes);
UINT8 HAL_CdspLscWinOffsetSet(UINT8 lscHOff,UINT8 lscVOff);
UINT8 HAL_CdspLscEnSet(UINT8 lscEnable);
UINT8 HAL_CdspLscEnGet(void);
UINT8 HAL_CdspLscModeSet(UINT8 lscChMode);

UINT8 HAL_CdspLscWinAddrset(UINT32 lscWinAddr);



/* Y Edge ,Y Coring */
void HAL_CdspYuvFilterBufSet(UINT8 yBufEn);
//void HAL_CdspEdgeCoringMedianEnSet(UINT8 yEdgeEn,UINT8 lEdgeEn,UINT8 yCorEn,UINT8 uvMedianEn);
void HAL_CdspYEdgeEnSet(UINT8 yEdgeEn);
void HAL_CdspLEdgeEnSet(UINT8 lEdgeEn);
void HAL_CdspYCoringEnSet(UINT8 yCoringEn);
void HAL_CdspUVMedianEnSet(UINT8 uvMedianEn);
void HAL_CdspMedianModeSet(UINT8 medianMode);

UINT8 HAL_CdspEdgeSuppressionMir (UINT8 edgeMirHLEn,UINT8 edgeMirHREn,UINT8 edgeMirVTEn,UINT8 edgeMirVDEn);
UINT8 HAL_CdspYCoringCoefficientSet(UINT8 *par);
UINT8 HAL_CdspEdgeYHDivSet(UINT8 yhDiv);
UINT8 HAL_CdspEdgeYHClampSet(UINT8 yhtClamp);
UINT8 HAL_CdspEdgeGainSet(UINT8 gain1, UINT8 gain2);
UINT8 HAL_CdspEdgeThrSet(UINT8 yhtLowThr,UINT8 yhtHighThr);
UINT8 HAL_CdspEdgeBwSet(UINT8 yhtBw);
UINT8 HAL_CdspEdgeFilterSet(	UINT8 ftr0,	UINT8 ftr5,	UINT8 ftr4,	UINT8 ftr3,	UINT8 ftr2,	UINT8 ftr1);

//void HAL_CdspEdgeCoringMedianEnGet(UINT8 *filterEn);
//void HAL_CdspEdgeYHDivGet(UINT8 *yhDiv);
//void HAL_CdspEdgeYHClampGet(UINT8 *yhtClamp);
//void HAL_CdspEdgeBwGet(UINT8 *yhtBw);
//void HAL_CdspEdgeGainGet(UINT8 *gain1, UINT8 *gain2);
//UINT8 HAL_CdspEdgeThrGet(UINT8 *yhtLowThr,UINT8 *yhtHighThr);
//void HAL_CdspEdgeFilterGet(UINT8 *ftr0,UINT8 *ftr5,UINT8 *ftr4,UINT8 *ftr3,UINT8 *ftr2,UINT8 *ftr1);


void HAL_CdspEffectYuvNormalSave( void );
void HAL_CdspEffectYuvNormalResotre( void );



//Dark Sub
UINT8 HAL_CdspDarkTDSizeSet(UINT16 hSize);
UINT8 HAL_IntrCdspDarkTDSizeSet(UINT16 hSize);
UINT8 HAL_CdspDarkCfg(UINT8 src,UINT8 mode);
UINT8 HAL_CdspDarkEnSet(UINT8 en);


//Still Bad Pixel
UINT8 HAL_CdspStillBpCfg(UINT8 mode,UINT8 sync,	UINT8 rip,UINT16 hSize);
UINT8 HAL_CdspStillBpEnSet(UINT8 en);
UINT8 HAL_CdspStillBpTblAddrSet(UINT32 bpTblAddr);
UINT8 HAL_CdspStillBpRAWAddrSet(UINT32 bpRawAddr);
UINT8 HAL_CdspStillBpTrigger(UINT8 bpTrig);
//UINT8 HAL_CdspBpCompensation(UINT8 badpixen,UINT8 badpixtesten, UINT16* bphitcount ,UINT32 badbaseaddr);


/*preview Bad Pixel*/
UINT8 HAL_CdspPvBpTblAddrSet(UINT32 pvBPTblAddr);
UINT8 HAL_CdspPvBpEnSet(UINT8 pvBpEn);



/* WDR */
UINT8 HAL_CdspWDRTblnit(UINT8* pWdrData);
UINT8 HAL_CdspWDREnSet(UINT8 en);

/* Temporal De-noise */
UINT8 HAL_CdspTDCfg(UINT8 rst,UINT8 sync,UINT8 zf);
UINT8 HAL_CdspTDEnSet(UINT8 en);
UINT8 HAL_IntrCdspTDEnSet(UINT8 en);

//Hue Correction
UINT8 HAL_CdspHueCorrectionEnSet(UINT8 enable  );
UINT8 HAL_CdspHueCorrectionXSet(UINT16 angleX,  UINT8 n1x, UINT8 n2x ,UINT8 n3x, UINT8 n4x,UINT8 shx,UINT8 ssx );
UINT8 HAL_CdspHueCorrectionYSet(UINT16 angleY,  UINT8 n1y, UINT8 n2y ,UINT8 n3y, UINT8 n4y,UINT8 shy,UINT8 ssy );
UINT8 HAL_CdspHueCorrectionZSet(UINT16 angleZ,  UINT8 n1z, UINT8 n2z ,UINT8 n3z, UINT8 n4z,UINT8 shz,UINT8 ssz, UINT8 yThrz );


#endif  /* __HAL_CDSP_IQ_H__ */


