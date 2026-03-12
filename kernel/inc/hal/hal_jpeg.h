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
 * @file      hal_jpeg.h
 * @brief     header file of hal_jpeg.c
 * @author    Tq.zhang
 * @since     2010-04-22
 * @date      2010-04-22
*/
#ifndef __HAL_JPEG_H__
#define __HAL_JPEG_H__

#include "general.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define HAL_JPEG_VLC_BOUND_MARGIN ( 16 )

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
UINT8 HAL_JpegQTableWrite( UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt );
UINT8 HAL_JpegQTableRead( UINT8 *QTableIndex, UINT8 *QTable, UINT8 zigzagOpt );
UINT8 HAL_JpegMjpgTrig( UINT32 VLCAAddr, UINT32 VLCBAddr, UINT8 Option );
void HAL_JpegMjpgAbort( void );
UINT8 HAL_JpegEncode( UINT32 SrcAddr, UINT32 VLCAddr, UINT32 DCTDCAddr, UINT8 Option, UINT8 wait );
UINT8 HAL_JpegDecode( UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr, UINT8 PbRescalSize, UINT8 Option, UINT16 MCURstNo );
UINT8 HAL_JpgZFactorSet( UINT16 hfactor, UINT16 vfactor, UINT16 SrcWidth, UINT16 SrcHeight, UINT16 DstWidth, UINT16 DstHeight, UINT8 option );
UINT8 HAL_JpgROISet( UINT8 option, UINT16 SrcHsize, UINT16 SrcVsize, UINT16 SrcOffsetX, UINT16 SrcOffsetY, UINT16 DetHsize, UINT16 DetVsize, UINT8 rot );
void HAL_JpegVlcBoundSet( UINT8 enable,UINT8 intrEn, UINT32 size );
UINT8 HAL_JpegReadEncodeSize( UINT32 *Compsize );
void HAL_JpegDecodeWait(void);
UINT8 HAL_JpgInit(UINT32 jpgsrcaddr,UINT32 vlcaddr); 
void HAL_JpegDataStampSet(UINT32 addrWord,UINT16 DramHsize,UINT16 xoff,UINT16 yoff,UINT8  xsize,UINT16 ysize,UINT8  thrhold);
void HAL_JpegDataStampEnable(UINT8 en, UINT8 RevMode);
void HAL_JpegSetVlcOffset(UINT16 offset);
void HAL_JpegBypassScaleUp(UINT32 SrcYuvAddrWord, UINT16 SrcWidth, UINT16 SrcHeight, 
							UINT16 RoiWidth, UINT16 RoiHeight, UINT16 hOffset, UINT16 vOffset,
							UINT32 DstYuvAddrWord, UINT16 DstWidth, UINT16 DstHeight, UINT8 option);
UINT8 HAL_JpgRoiEncSet(UINT16 SrcWidth, UINT16 SrcHeight, 
	                                     UINT16 RoiWidth, UINT16 RoiHeight, UINT16 RoiXOff, UINT16 RoiYOff,
	                                     UINT16 DstWidth, UINT16 DstHeight, UINT8 option); 
void HAL_JpegSetSourceSize(UINT16 SrcHsize, UINT16 SrcVsize);
void HAL_JpegSetDramHSize(UINT16 hsize);
void HAL_JpegSetVlcSize(UINT16 VlcWidth, UINT16 VlcHeight);


#endif  /* __HAL_JPEG_H__ */
