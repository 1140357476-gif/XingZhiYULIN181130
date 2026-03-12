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
#ifndef __HAL_CDSP_H__
#define __HAL_CDSP_H__

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------
typedef enum CDSPrstIdx_e {
    CDSPRstCDSP = 0,
	CDSPRstAE,
	CDSPRstAWB,
	CDSPRstEDGE_LSC,
	CDSPRstDramV,
	CDSPRstDramV1,
	CDSPRstVDram,
	CDSPRstVDram1,
}CDSPrstIdx_t;

typedef enum cdspSkipWr_e {
    CDSP_SKIP_IMG 	   = 0xfe,
	CDSP_SKIP_AWB 	   = 0xfd,
	CDSP_SKIP_AE  	   = 0xfb,
	CDSP_SKIP_DISTEDGE = 0xf7,	
	CDSP_SKIP_ALL 	   = 0xf0,
	CDSP_SKIP_AWBAEEDGE= 0xf1,
	CDSP_SKIP_AEEDGE   = 0xf3,
}cdspSkipWr_t;

//-----------------------------------------------------------------------------
//Function Decleration
//-----------------------------------------------------------------------------


//UINT8  HAL_SetWBGlobalGainSet(UINT8);
//UINT8 HAL_LenscmpInit(UINT16 *plensdata);
UINT8 HAL_CdspCropSet(UINT8 crop_henable ,UINT8 crop_venable ,UINT8 crop_sync_enable, UINT16 crop_hoffset,UINT16 crop_hsize,UINT16 crop_voffset,UINT16 crop_vsize);
//UINT8 HAL_CdspCropGet(UINT8* crop_henable ,UINT8* crop_venable , UINT16* crop_hoffset,UINT16* crop_hsize,UINT16* crop_voffset,UINT16* crop_vsize);


UINT8 HAL_CdspHScaleRgbSet(UINT8 hscale_enable, UINT8 hscale_mode,UINT8 hscale_sync_mode,UINT16 shsize,UINT16 dhsize);
UINT8 HAL_CdspHscaleRGBEnable(UINT8 hscale_enable, UINT8 hscale_mode,UINT8 hscale_sync_mode);


void HAL_CdspWriteBufIdxSet(UINT8  bufIdx);
UINT8 HAL_CdspWriteBufIdxGet(void);
void HAL_CdspFrmCtrlEnSet(UINT8 en);


UINT8 HAL_CdspClockSet(UINT8 src,UINT8 clkdiv);


UINT8 HAL_CdspRstBlock(UINT8 cdspRstIdx);



//UINT8 HAL_CdspSpecialeffectSet(UINT8 intlvalidinon);

//UINT8 HAL_CdspColorMEn(UINT8 enable);
//UINT8 HAL_CdspColorMSet(UINT16 a00,UINT16 a01,UINT16 a02,UINT16 a10,UINT16 a11,UINT16 a12,UINT16 a20,UINT16 a21,UINT16 a22);

	
/* DRAMV/VDRAM  Dram <-> Cdsp */
UINT8 HAL_CdspDramVMirrorSet(UINT8 mirrorLine,UINT8 vsMirrorEn,UINT8 veMirrorEn);
UINT8 HAL_CdspDramVSkippixelSet(UINT8 skipPixel);
UINT8 HAL_CdspDramVSizeOffsetSet(UINT16 rHSize,UINT16 rHOffset,UINT16 rVSize,UINT16 rVOffset);
UINT8 HAL_CdspDramVModeSet(UINT8 remode);
//UINT8 HAL_CdspVDramMode(UINT8 mode,UINT8 modeSVd);

void HAL_CdspVDramSkipWrSyncSet(UINT8 sync);
void HAL_CdspVDramSkipWrSet(UINT8 mask,UINT8 val);
void HAL_CdspVDramSkipWrGet(UINT8 *val);


UINT8 HAL_CdspVDramDataType(UINT8 cdspType,UINT8 cdspSync,UINT8 cdspRange);
UINT8 HAL_CdspVDramSizeClamp(UINT8 hSizeClpEn, UINT16 hSize, UINT8 vSizeClpEn, UINT16 vSize, UINT8 sizeSVd);
UINT8 HAL_CdspRawImgSource(UINT8 srcSel);
UINT8 HAL_CdspCoreImgSource(UINT8 src,UINT8 sync);



UINT8 HAL_CdspEofWait( UINT8 Number );
UINT8 HAL_CdspDramVInterval(UINT16 lineInterval);



/* AE Functions */
UINT8 HAL_CdspAEWinEnSet(UINT8 aeWinEn, UINT8 aeHoldSvdEn, UINT8 aeSizeZf);
UINT8 HAL_CdspAEWinModeSet(UINT8 aeWinMode);
UINT8 HAL_CdspAEWinModeGet(void);
UINT8 HAL_CdspAEWinHoldSet(UINT8 aeWinHold);
UINT8 HAL_IntrCdspAEWinHoldSet(UINT8 aeWinHold);
UINT8 HAL_CdspAEWinTest(UINT8 aeWinTest);

UINT8 HAL_CdspAEwinfactorset(UINT8 aeHAccFactor, UINT8 aeVAccFactor);
UINT8 HAL_CdspAEwinoffset(UINT16 aeHOffset, UINT16 aeVOffset);
UINT8 HAL_CdspAEwinsizeset(UINT8 aeHSize, UINT8  aeVSize);
UINT8 HAL_CdspAEwinintvlset(UINT8 aeHIntvl,UINT8 AEVIntvl);
UINT8 HAL_CdspAEwinAddrset(UINT32 aeWinAddr);
UINT8 HAL_CdspAEwinCntset(UINT8 aeHCnt,UINT8 aeVCnt);
UINT8 HAL_CdspAEWinRead(UINT8 aeWinNum,UINT8 *aeWin);

/*AWB Functions*/

UINT8 HAL_CdspAWBWinEn(UINT8 awbWinEn,UINT8 pAwbWinHold,UINT8 awbWinTest,UINT8 awbWinSvd,UINT8 awbSizeZfEn);
UINT8 HAL_IntrCdspAWBWinEn(UINT8 awbWinEn,UINT8 pAwbWinHold,UINT8 awbWinTest,UINT8 awbWinSvd,UINT8 awbSizeZfEn);

UINT8 HAL_CdspAWBWinScaleEn(UINT8 AWBhscaleen, UINT8 AWBvscaleen);
UINT8 HAL_CdspAWBWincfg(UINT16 AWBhstart, UINT16 AWBhend,UINT16 AWBvstart,UINT16 AWBvend,UINT16 AWBhscalefactor,UINT16 AWBvscalefactor);
UINT8 HAL_CdspAWBwinAddrset(UINT32 AWBWinaddr);
UINT8 HAL_CdspAWBWindowSizeSet(UINT16 AWBhstart,UINT16 AWBhend,UINT16 AWBvstart,UINT16 AWBvend, UINT8 WinhSize, UINT8 WinvSize);
UINT8 HAL_CdspAWBWndGainSet(UINT8 AWBrgain,UINT8 AWBbgain);
UINT8 HAL_CdspAWBWinRead(UINT16 AWBWinNum,UINT8 *AWBWin);

/* AF Functions */
UINT8 HAL_CdspAFWinEn(UINT8 afWinEn,UINT8 pAFWinHold,UINT8 afWinTest,UINT8 afFlt2Type,UINT8 afWinHoldSvd);
UINT8 HAL_CdspAFDataModeSet(UINT8 afRgbMode,UINT8 afPgType);
UINT8 HAL_CdspAFFilterWtSet(UINT8 afFlt1Wt,UINT8 afFlt2Wt);
UINT8 HAL_CdspAFCoringSet(UINT8 afCorThr,UINT8 afCorVal);
UINT8 HAL_CdspAFWinSizeSet(UINT16 afHStart,UINT16 afHEnd,UINT16 afVStart,UINT16 afVEnd);
UINT8 HAL_ReadAFWindow(UINT32* afWinValue);




/* Edge Window */
UINT8 HAL_CdspDistEdgeWinEn(UINT8 edgeWinEn,UINT8 edgeWinHold,UINT8 edgeWinTest,UINT8 edgePosition,	UINT8 edgeHoldSvdEn);
UINT8 HAL_CdspDistEdgeWinOffSet(UINT8 edgeHOffset, UINT8 edgeVOffset);
UINT8 HAL_CdspDistEdgeWinSizeSet(UINT8 edgeHSize, UINT8 edgeVSize);
UINT8 HAL_CdspDistEdgeWinCntSet(UINT8 edgeHCnt,UINT8 edgeVCnt);
UINT8 HAL_CdspDistEdgeWinThrSet(UINT8 edgeHThr,UINT8 edgeVThr);
UINT8 HAL_CdspDistEdgeWinAddrSet(UINT32 edgeWinAddrA,UINT32 edgeWinAddrB);
UINT8 HAL_CdspDistEdgeWinMap(UINT8 edgeHMap,UINT8 edgeVMap);
UINT8 HAL_CdspDistEdgeWinRead(UINT16 edgeWinNum,UINT8 *edgeWin);


//interpolation
UINT8 HAL_CdspIntplmir(UINT8 intplMirHLEn,UINT8 intplMirHREn,UINT8 intplMirVTEn,UINT8 intplMirVDEn);
UINT8 HAL_CdspIntplLowThrSet(	UINT8 intplLowThr);

//UINT8 HAL_CdspIntplCfg(	UINT8 intplLowThr, 	UINT8 IntplEdgeThr,  UINT8 intplSlntThr, UINT8 intplFtrOpt );

void HAL_CdspRegBackup(void);
void HAL_CdspRegBackupBufSet(UINT32 addr,UINT16 size);
void HAL_CdspRegBackupCntSet(UINT16 cnt);

void HAL_CdspRegGet(UINT16 addr,UINT8 *val);
void HAL_CdspRegSet(UINT16 addr,UINT8 val);


//UINT8 HAL_CarryWork( UINT32 *tmpvini, UINT16 *vrealdo, UINT16 dovsize, UINT16 vfactor );
//UINT8 HAL_Yuv422To444(UINT8 yuvinserten);
//UINT8 HAL_CdspRBClampSet(UINT8 enable,UINT8 RBClampvalue);


#endif  /* __HAL_CDSP_H__ */


