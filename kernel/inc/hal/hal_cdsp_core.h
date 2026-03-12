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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef _HAL_CDSP_CORE_H_
#define _HAL_CDSP_CORE_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define CDSP_MAX_LINEBUF          (UINT16)1280
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum cdspRegUpdate_e {
    IMMEDIATE,
	SYNCVD,
	SYNCZF,
}cdspRegUpdate_t; 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void HAL_CdspSyncModeSet(UINT8 mode);
UINT8 HAL_CdspSyncModeGet(void);


/*First Path*/
UINT8 HAL_CdspVScaleYuvSet(UINT8 syncMode, UINT16 srcSize, UINT16 dstSize);
UINT8 HAL_CdspHScaleYuvSet(UINT8 syncMode, UINT16 srcSize, UINT16 dstSize);
UINT8 HAL_Cdsp1stPathWriteOffset(UINT16 HOffset, UINT16 VOffset);
UINT8 HAL_Cdsp1stPathYuvSizeSet(UINT16 hSize,UINT16 vSize);
UINT8 HAL_Cdsp1stPathYuvSizeGet(UINT16 *hSize,UINT16 *vSize);

/*Second Path*/
UINT8 HAL_Cdsp2ndPathVScaleYuvSet(UINT8 syncMode, UINT16 srcSize, UINT16 dstSize);
UINT8 HAL_Cdsp2ndPathHScaleYuvSet(UINT8 syncMode, UINT16 srcSize, UINT16 dstSize);
UINT8 HAL_Cdsp2ndPathCfg(UINT8 skip,UINT8 range,UINT8 sync);
UINT8 HAL_Cdsp2ndPathEnSet(UINT8 en);
UINT8 HAL_Cdsp2ndPathWriteOffset(UINT16 HOffset, UINT16 VOffset);
UINT8 HAL_Cdsp2ndPathClampHSize(UINT8 en,UINT16 hsize);


/*Digital Zoom*/
UINT8 HAL_Cdsp1stPathZFactorSet(UINT8  UpdateMode, UINT16 hFactor, UINT16 vFactor, UINT16 dstHSize, UINT16 dstVSize, UINT16 roiHSize, UINT16 roiVSize,UINT16 aeDisFactor,UINT16 awbDisFactor);
UINT8 HAL_Cdsp1st3AWinSet(UINT16 aeHSize,UINT16 aeVSize,UINT16 aeDispFactor,UINT16 awbDispFactor);
UINT8 HAL_CdspAWBWinSet(UINT16 hSize,UINT16 vSize);

UINT8 HAL_CdspMaxZFactorGet(UINT16 *hZFactor, UINT16 *vZFactor);
UINT8 HAL_CdspMaxZFactorSet( UINT16 hZFactor, UINT16 vZFactor, UINT8 enable);
void HAL_CdspScaleFactorGet(UINT16 srcSize,UINT16 dstSize,UINT32 *scaleFactor,UINT32 *iniFactor,UINT16 zFactor);

void HAL_CdspHScaleSizeGet(UINT16 srcSize,UINT16 dstSize,UINT32 scaleFactor,UINT32 iniFactor,UINT16 zoomFactor,UINT16 *offset,UINT16 *outSize, UINT16 *cropSize);
void HAL_CdspVScaleSizeGet(UINT16 srcSize,UINT16 dstSize,UINT32 scaleFactor,UINT32 iniFactor,UINT16 zoomFactor,UINT16 *offset,UINT16 *outSize, UINT16 *cropSize);

void HAL_CdspRoiSizeSet(UINT16 hSize,UINT16 vSize);

/*CDSP Do*/




//UINT8 HAL_CdspImgDoABBuf( UINT8 mode, UINT16 hoffset,   UINT16 dohsize,   UINT16 voffset,   UINT16 dovsize,UINT8  hmirroren,UINT8  vmirroren, UINT16 writeOffset );
//UINT8 HAL_CdspImgVDoC_Scale(UINT8 mode,UINT16 hoffset,UINT16 dohsize,UINT16 voffset,UINT16 dovsize,UINT8 hmirroren,UINT8 vmirroren,UINT16 dosubwidth, UINT16 calrealdo, UINT16 tghsize,UINT16 tgvsize);
//UINT8 snapDateStampCdspCallback(UINT16 yPos,UINT16 lines);
//UINT8 HAL_CdspVScaleDoABBuf( UINT8 mode, UINT16 hoffset, UINT16 dohsize, UINT16 voffset, UINT16 dovsize, UINT8 hmirroren, UINT8 vmirroren, UINT16 hfactor, UINT16 vfactor, UINT16 hfactini, UINT16 vfactini, UINT16 calrealdo, UINT16 writeVOffset );
//void halCdspSubSizeCal( UINT8 mode, UINT32 vrmder, UINT16 *hSize, UINT16 *hSub, UINT16 *vSub );
//void cdspYuvCoringSet(UINT8 n);
//void cdspPostprocessSwitch(UINT8 opt);
//UINT8 HAL_CdspDoSub_YUVVertical_Scale(UINT8 mode,UINT16 hoffset,UINT16 dohsize,UINT16 voffset,UINT16 dovsize,UINT8 hmirroren,UINT8 vmirroren,UINT16 hfactor,UINT16 vfactor,UINT16 hfactini,UINT16 vfactini,UINT16 calrealdo,UINT16 wOffsetH,UINT16 wOffsetV,UINT8 pixel);
//UINT8 HAL_CdspImgSnapDo_Scale( UINT8 mode, UINT16 dohsize,  UINT16 dovsize, UINT8 hmirroren, UINT8 vmirroren,  UINT16 tghsize, UINT16 tgvsize, UINT16 hFactor, UINT16 vFactor);
//UINT8 HAL_CdspImgDo(UINT8 mode,UINT16 hoffset,UINT16 dohsize,UINT16 voffset,UINT16 dovsize,UINT8 hmirroren,UINT8 vmirroren,UINT16 dosubwidth,UINT16 dosubheight, UINT16 ScaleFactor);
//UINT8 HAL_CdspImgDoSub(UINT8  mode,UINT16 hoffset,UINT16 dohsize,UINT16 voffset,UINT16 dovsize,UINT8  hmirroren,UINT8  vmirroren,UINT16 hfactor,UINT16 vfactor,UINT16 writeHOffset,UINT16 writeVOffset,UINT8 number,UINT8 pixel,UINT32 vrmder);
//UINT8 HAL_CdspScaleSyncModeSet(UINT8 scalesvden,UINT8 scaleszfen);

#endif  /* _TEMPLATE_HDR_H_ */

