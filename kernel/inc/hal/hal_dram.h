/**************************************************************************
 *
 *        Copyright (c) 2007-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file		hal_dram.h
 * @brief		hal dram header file.
 * @author		Will Chiu
 * @since		2010-02-23
 * @date		2010-02-23
 */

#ifndef __HAL_DRAM_H__
#define __HAL_DRAM_H__
#include "general.h"
#include "asicreg.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define SDRAM_CardSetStartAddress(address, prefetch) HAL_DramStartAddrSet(address, prefetch)

#define MANU_CDSP_IDX 0 


typedef enum dramZFUpdate_e {
	HAL_DRAM_ZUPDATE_CDSP,
	HAL_DRAM_ZUPDATE_PV,
	HAL_DRAM_ZUPDATE_PB,
	HAL_DRAM_ZUPDATE_OSD,
	HAL_DRAM_ZUPDATE_MPEG,
	HAL_DRAM_ZUPDATE_VIDEO,
	HAL_DRAM_ZUPDATE_ALL,
}dramZFUpdate_t; 


//wangjg++ 20100223
typedef enum
{
	DRAM_RWFormat_8=0,	// BYTE sequential
	DRAM_RWFormat_16,	// WORD sequential    
	DRAM_RWFormat_32,	// DWORD sequential
}dram_rw_format_t;//byte-orders

typedef enum {
	HAL_DRAM_ZF_UPD_LAYER_PV  = (1 << 0),
	HAL_DRAM_ZF_UPD_LAYER_PB  = (1 << 1),
	HAL_DRAM_ZF_UPD_LAYER_OSD = (1 << 2),
	HAL_DRAM_ZF_UPD_CLEAR     = 0x00,
} HAL_DramZfUpdSrc_e;

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT8 HAL_Dram1stPathFrmBufAddrGet(UINT8 idx, UINT32 *addrW);

UINT8 HAL_Dram1stPathFrmBufAAddrSet(UINT32 AFBAddr);
UINT8 HAL_Dram1stPathFrmBufBAddrSet(UINT32 BFBAddr);
UINT8 HAL_Dram1stPathFrmBufCAddrSet(UINT32 CFBAddr);
UINT8 HAL_Dram1stPathFrmBufDAddrSet(UINT32 DFBAddr);
UINT8 HAL_Dram1stPathFrmBufAAddrSet_Intr(UINT32 AFBAddr);
UINT8 HAL_Dram1stPathFrmBufBAddrSet_Intr(UINT32 BFBAddr);
UINT8 HAL_Dram1stPathFrmBufCAddrSet_Intr(UINT32 CFBAddr);
UINT8 HAL_Dram1stPathFrmBufDAddrSet_Intr(UINT32 DFBAddr);

UINT8 HAL_Dram1stPathFrmBufSizeSet(UINT16, UINT16);
UINT8 HAL_Dram1stPathFrmBufSizeGet(UINT16 *hSize, UINT16 *vSize); 


UINT8 HAL_Dram2ndPathFrmBufAAddrSet(UINT32 AFBAddr);	   //Will: add second path
UINT8 HAL_Dram2ndPathFrmBufBAddrSet(UINT32 BFBAddr);
UINT8 HAL_Dram2ndPathFrmBufCAddrSet(UINT32 CFBAddr);
UINT8 HAL_Dram2ndPathFrmBufDAddrSet(UINT32 DFBAddr);
UINT8 HAL_Dram2ndPathFrmBufSizeSet(UINT16);

void  HAL_DramPreviewStart(UINT8 pbFreeze);
UINT8 HAL_DramRawFrmBufSizeSet(UINT16, UINT16);
UINT8 HAL_DramAVLCAddrSet(UINT32);
UINT8 HAL_DramBVLCAddrSet(UINT32);
UINT8 HAL_DramDoYuvSrcHSizeSet( UINT16 size );
//UINT8 HAL_SetDCTAddr(UINT32);
//UINT8 HAL_SetAUDAddr(UINT32);
//UINT8 HAL_SetFOSDAddr(UINT32);
UINT8 HAL_DramRawFrmBufAddrSet(UINT32);

//DRAM
//UINT8 HAL_InitSDRAM(UINT8);
UINT8 HAL_GprmDramDma(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 wait);
UINT8 HAL_GprmDramDmaAdv(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 wait);

//void  HAL_DRAMindirectwrite(UINT32 wordaddr, UINT16 worddata);
//void  HAL_DRAMindirectread(UINT32 wordaddr, UINT16* worddata);
UINT8 HAL_DramStartAddrSet(UINT32, UINT8);
UINT8 HAL_DramWordDataRead(UINT8 *, UINT8 *);
UINT8 HAL_DramWordDataWrite(UINT8, UINT8);
void HAL_DramDispSrcSel(UINT8 srcSel);
UINT8 HAL_DramDispSrcGet(void);

//UINT8 HAL_DramWordDataRead(UINT32, UINT8 *, UINT8 *);
//UINT8 HAL_DramWordDataWrite(UINT32, UINT8, UINT8);
UINT8 HAL_SetJPEGVscale(UINT16, UINT16);
//UINT8 HAL_WriteQTable(UINT8, UINT8, UINT8*);
//UINT8 HAL_ReadQTable(UINT8, UINT8 *, UINT8 *);
//UINT8 HAL_Compress(UINT32, UINT32, UINT32, UINT8, UINT8);
//UINT8 HAL_DeCompress(UINT32, UINT16, UINT16, UINT32, UINT8, UINT8, UINT16);
//UINT8 HAL_AutoCompress(UINT32 VLCAAddr, UINT32 VLCBAddr, UINT8 Option);
//void HAL_VlcBoundSet( UINT16 enable, UINT32 size );
//void HAL_MjpgClipAbort(void);
//UINT8 HAL_ReadCompSize(UINT32*);
//UINT8 HAL_JpgZFactorSet( UINT16 hfactor, UINT16 vfactor, UINT16 SrcWidth, UINT16 SrcHeight, UINT16 DstWidth, UINT16 DstHeight, UINT8 option );
//UINT8 HAL_JpgROISet( UINT8 option, UINT16 SrcHsize, UINT16 SrcVsize, UINT16 SrcOffsetX, UINT16 SrcOffsetY, UINT16 DetHsize, UINT16 DetVsize, UINT8 rot );
//UINT8 HAL_CheckDecompress(void);


void HAL_DramPattSearch(UINT8 mode, UINT32 wordaddr, UINT32 length, UINT32 patt,
					   UINT32 *first, UINT32 *total, UINT8 *hit);


//UINT8 HAL_SetACDSPWidth(UINT16);
//UINT8 HAL_SetBCDSPWidth(UINT16);
//UINT8 HAL_SetRCDSPWidth(UINT16);
//UINT8 HAL_SetImageType(UINT8);
//UINT8 HAL_SetDisplayMode(UINT8, UINT8);
UINT8 HAL_DramVLCRdyClr(UINT8);
//UINT8 HAL_SelDRAMRefreshSrc(UINT8);
//UINT8 HAL_EnterExitSelfRefresh(UINT8);
//UINT8 HAL_SetAGBAddr(UINT32);
//UINT8 HAL_SetBGBAddr(UINT32);
//UINT8 HAL_SetAGBSize(UINT16, UINT16);
//UINT8 HAL_SetBGBSize(UINT16, UINT16);
//UINT8 HAL_DoDRAMDMA(UINT32, UINT32, UINT32);
UINT8 HAL_DramZUpdateWait(UINT8, UINT16 timeout);
UINT8 HAL_IntrDramZUpdateWait(UINT8);

void HAL_DramZUpdateClear(UINT8 val);
void HAL_IntrDramZUpdateClear(UINT8 val);

void HAL_DramZfUpdTrig(UINT8 layer);
void HAL_IntrDramZfUpdTrig(UINT8 layer);


//patch5.0@richie@sdram mapping begin
//UINT8 HAL_ScaleUpImage(UINT32, UINT16, UINT16, UINT32, UINT16, UINT16, UINT32);
//UINT8 HAL_ScaleUpImage_H(UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr, UINT16 DstHsize);
//UINT8 HAL_ScaleUpImage_V(UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize ,UINT32 DstAddr, UINT16 DstVsize);
//UINT8 HAL_ScaleDownImage(UINT32, UINT16, UINT16, UINT32, UINT16, UINT16, UINT32);
//UINT8 HAL_ScaleDownImage_H(UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize,UINT32 DstAddr, UINT16 DstHsize);
//UINT8 HAL_ScaleDownImage_V(UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr,UINT16 DstVsize);
//patch5.0@richie@sdram mapping end

UINT8 HAL_DramDRAMDarkAddrSet(UINT32 Addr);
UINT8 HAL_DramDRAMDarkSizeSet(UINT16 hsize);

void HAL_DramSnapRip(UINT8 type);

void HAL_DramDispBufSel(UINT8 sel);
void HAL_DramCodeSentry(UINT32 LowBoundaryAddrW,UINT32 HighBoundaryAddrW,UINT32 RedirectAddrW);

void HAL_DramRWBytes(UINT32 addrW,UINT8* pt,UINT32 cnt,UINT8 isRead,UINT8 fmt);

void HAL_DramFrmBufNumSet(UINT8 sel);


#define HAL_DramSetAddr(addr,isRead)	M_DRAM_SetStartAddress(addr,isRead)

#define HAL_DramWriteByte(UINT8_val0,UINT8_val1)	M_DRAM_WriteWord(UINT8_val0,UINT8_val1)
#define HAL_DramWrite16(UINT16_value)	HAL_DramWriteByte(*((UINT8*)&UINT16_value + 1 ),*((UINT8*)&UINT16_value))
#define HAL_DramWrite32(UINT32_value)	HAL_DramWriteByte(*((UINT8*)&UINT32_value + 3 ),*((UINT8*)&UINT32_value + 2));\
										HAL_DramWriteByte(*((UINT8*)&UINT32_value + 1 ),*((UINT8*)&UINT32_value))
#define HAL_DramWriteBytesAddr(addrW,pt,UINT32_cnt,UINT8_fmt)	HAL_DramRWBytes(addrW,(UINT8*)pt,UINT32_cnt,0,UINT8_fmt)
#define HAL_DramWriteBytes(pt,UINT32_cnt,UINT8_fmt)			HAL_DramWriteBytesAddr(0,pt,UINT32_cnt,UINT8_fmt)

#define HAL_DramReadByte(ptUINT8_val0,ptUINT8_val1)	M_DRAM_ReadWord((*(ptUINT8_val0)),(*(ptUINT8_val1)))
#define HAL_DramRead16(ptUINT16_value)	M_DRAM_ReadWord(*((UINT8*)(ptUINT16_value) + 1 ),*((UINT8*)(ptUINT16_value)))
#define HAL_DramRead32(ptUINT32_value)	M_DRAM_ReadWord(*((UINT8*)(ptUINT32_value) + 3 ),*((UINT8*)(ptUINT32_value) + 2));\
										M_DRAM_ReadWord(*((UINT8*)(ptUINT32_value) + 1 ),*((UINT8*)(ptUINT32_value)))
#define HAL_DramReadBytesAddr(addrW,pt,UINT32_cnt,UINT8_fmt)	HAL_DramRWBytes(addrW,(UINT8*)pt,UINT32_cnt,1,UINT8_fmt)
#define HAL_DramReadBytes(pt,UINT32_cnt,UINT8_fmt)				HAL_DramReadBytesAddr(0,pt,UINT32_cnt,UINT8_fmt)

UINT8 HAL_DramDmaWrite(void xdata* pSrc, UINT32 dramAddrWord, UINT16 sizeByte, UINT8 wait);
UINT8 HAL_DramDmaRead(void xdata* pDst, UINT32 dramAddrWord, UINT16 sizeByte, UINT8 wait);

void HAL_DramABBufAutoMap(UINT8 en);
void HAL_DramWrYuvVDram1(UINT8 en);


#endif  /* __HAL_DRAM_H__ */

