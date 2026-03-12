/**************************************************************************
 *
 *        Copyright (c) 2008-2008 by Sunplus mMedia Inc., Ltd.
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
 * @file		bp_main.c
 * @brief		badpixel handle function
 * @author	Phil Lin
 * @since		2008-11-19
 * @date		2008-11-18
 */
 
/**************************************************************************
* 						   H E A D E R	 F I L E S
**************************************************************************/
#include "general.h" 
#include "initio.h" 
#include "main.h" 

#include "bp_private.h" 
#include "bp_api.h"

#include "sp1k_rsvblk_api.h"
#include "front_logi.h"
#include "app_still_view.h"
#include "dbg_file_list.h"
#include "hal_cdsp.h"
#include "dbg_def.h"
#include "hal_gprm.h"
#include "hal.h"
#include "hal_global.h"
#include "reg_cdsp.h"
#include "pv_task.h"
#if MODULE_BP_OPT 
/**************************************************************************
 *  						 C O N S T A N T S  						  *
 **************************************************************************/

/**************************************************************************
 *  							M A C R O S 							  *
 **************************************************************************/
#define __FILE	__FILE_ID_BP_MAIN__

#define DBG_BP_EN	0

/* the high/low byte switch in capture badpixel compensation */
#define BP_BYTE_SW_EN	0

/**************************************************************************
 *  						D A T A    T Y P E S						  *
 **************************************************************************/

/**************************************************************************
 *  					   G L O B A L    D A T A   					  *
 **************************************************************************/

static resBpCb_t xdata resBp[RES_BP_MAX];
static bpPara_t* xdata pBpPara;

UINT8 xdata g_planeWidth;
UINT8 xdata g_planeHeight;
UINT16 xdata g_stripWidth;
UINT8 xdata g_stripHeight;
UINT8 xdata g_threshold;
UINT32 xdata g_badPixelCnt;
UINT32 xdata g_bpNumMaxPrv_HD;
UINT32 xdata g_bpNumMaxPrv;
UINT32 xdata g_bpNumMaxCap;
UINT32 xdata* xdata gp_bpNumMax;

#if TAG_TODO
extern xdata UINT16 snapSensorXSize,snapSensorYSize;
extern xdata UINT16 snapSensorCropXSize,snapSensorCropYSize;
extern xdata UINT16 snapRawXSize;
extern xdata UINT16 snapXSize;
extern appPvZoom_t xdata PvZoomCtrl;
#else
xdata UINT16 snapSensorXSize,snapSensorYSize;
xdata UINT16 snapSensorCropXSize,snapSensorCropYSize;
xdata UINT16 snapRawXSize;
xdata UINT16 snapXSize;
extern appPvZoom_t xdata PvZoomCtrl;
#endif
/**************************************************************************
 *  			   E X T E R N A L    R E F E R E N C E S   			  *
 **************************************************************************/

/**************************************************************************
 *  			 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
static UINT8 bpRawRead(UINT32 addr, UINT32 width, UINT32 xPos, UINT32 yPos, UINT16* prgb);
static UINT8 bpRawWrite(UINT32 addr, UINT32 width, UINT32 xPos, UINT32 yPos, UINT16 prgb);

//-----------------------------------------------------------------------------
//bpRawRead
//-----------------------------------------------------------------------------
/**
 * @brief		read specified pixel of raw image
 * @param	addr: raw data source address
 			width: the width of raw image
 			xPos: the horizontal offset of raw image
			yPos: the vertical offset of raw image
			prgb: the value of the specified pixel
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-19
 * @todo		Something need to be done.
 * @bug 		a bug need to be resolved.
*/
static UINT8 bpRawRead(UINT32 addr, UINT32 width, UINT32 xPos, UINT32 yPos, UINT16* prgb)
{
	UINT32 offset;
	const UINT16 bytePerPixel = 1; /* the data length raw is 8bits */

	if (!prgb) {
		return FAIL;
	}

	offset = (yPos * width + xPos) * bytePerPixel;
	addr += (offset >> 1);

#if BP_ASSERT_EN
	if (addr <= K_SDRAM_CommonEndAddr || addr >= K_SDRAM_TotalSize) {
printf("xPos: %lx\n", (UINT32)xPos);
printf("yPos: %lx\n", (UINT32)yPos);
printf("offset: %lx\n", (UINT32)offset);
printf("addr: %lx\n", (UINT32)addr);
		__ASSERT(__FILE, __LINE, 1);
		return FAIL;
	}	
#endif //BP_ASSERT_EN

	BP_DRAM_SetStartAddress(addr, 1);

	if (offset & 0x01) {
		/* the raw data is in high byte */
		BP_DRAM_ReadPword((UINT8 *) prgb, ((UINT8 *) prgb) + 1);
	} else {
		/* the raw data is in low byte */
		BP_DRAM_ReadPword(((UINT8 *) prgb) + 1, (UINT8 *) prgb);
	}
	WRITEP8(prgb, 0, 0);
	
#if DBG_BP_EN
	printf("read X(%lu), Y(%lu) : %u\n", xPos, yPos, *prgb);
#endif

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//bpRawWrite
//-----------------------------------------------------------------------------
/**
 * @brief		write specified pixel of raw image
 * @param	addr: raw data source address
 			width: the width of raw image
 			xPos: the horizontal offset of raw image
			yPos: the vertical offset of raw image
			prgb: the value of the specified pixel
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug 	a bug need to be resolved.
*/
static UINT8 bpRawWrite(UINT32 addr, UINT32 width, UINT32 xPos, UINT32 yPos, UINT16 prgb)
{
	UINT32 offset;
	const UINT16 bytePerPixel = 1; /* the data length raw is 8bits */
	UINT16 raw;

	offset = (yPos * width + xPos) * bytePerPixel;

	addr += (offset >> 1);	
	
#if BP_ASSERT_EN	
	if (addr <= K_SDRAM_CommonEndAddr || addr >= K_SDRAM_TotalSize) {
		__ASSERT(__FILE, __LINE, 1);
		return FAIL;
	}	
#endif //BP_ASSERT_EN	

	BP_DRAM_SetStartAddress(addr, 1);	
	BP_DRAM_ReadPword(((UINT8 *) &raw) + 1, (UINT8 *) &raw);

	if (offset & 0x01) {
		/* the raw data is in high byte */
		WRITE8(raw, 0, READ8(prgb, 1));
	} else {
		/* the raw data is in low byte */
		WRITE8(raw, 1, READ8(prgb, 1));
	}

	BP_DRAM_SetStartAddress(addr, 0);
	BP_DRAM_WriteWord(READ8(raw, 1), READ8(raw, 0));
	
#if DBG_BP_EN
	printf("write X(%lu), Y(%lu) : %x , %x\n", xPos, yPos, prgb, raw);
#endif

	return SUCCESS;
}

//-----------------------------------------------------------------------------
//bpCfg
//-----------------------------------------------------------------------------
/**
 * @brief		configure threshold & max number of badpixel function
 * @param	pPara: point to the badpixel configuration parameter
 * @retval	0: success
 			1: fail
 			-1: param error
 * @see		none
 * @author	Phil Lin
 * @since	2008-11-24
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 bpCfg(bpPara_t* pPara)
{
	UINT32 rsvSize;
	resBpCb_t xdata* pResBp;

	if ( !pPara ) {
		return UNKNOWN;
	}

	pBpPara = pPara;
	
	g_planeWidth = 8;	//g_planeWidth * g_planeHeight < 256
	g_planeHeight = 8;
	#if BP_USE_MOE
	g_stripWidth = 16;//1024 / (UINT16) g_planeHeight; // buf size = 1024, 
	#else
	g_stripWidth = 1024 / (UINT16) g_planeHeight; // buf size = 1024,	
	#endif
	g_stripHeight = g_planeHeight;
	g_badPixelCnt = 0;
	g_threshold = pPara->threshold_B; //modify 20091124

	/* 1 -> for resource file end mark */
	g_bpNumMaxPrv_HD = pPara->prvBpMax_HD - 1;
	g_bpNumMaxPrv = pPara->prvBpMax - 1;
	g_bpNumMaxCap = pPara->capBpMax - 1;

	/* check if the resoure file id had already been registed */
	//if (pResBp->resId == 0) {
	//	return FAIL;
	//}
	
	pResBp = &resBp[RES_BP_PRV_HD];
	if (sp1kDiskRsvSizeGet(pResBp->resId, NULL, &rsvSize) == TRUE) {
		rsvSize >>= 2;
		if ((UINT32)pPara->prvBpMax_HD > rsvSize) {
			/* 
				preview badpixel max number defined is more large 
				than the size of resource file!
			*/
			g_bpNumMaxPrv_HD = rsvSize - 1;
		}
	}

	pResBp = &resBp[RES_BP_PRV_VGA];
	if (sp1kDiskRsvSizeGet(pResBp->resId, NULL, &rsvSize) == TRUE) {
		rsvSize >>= 2;
		if ((UINT32)pPara->prvBpMax > rsvSize) {
			/* 
				preview badpixel max number defined is more large 
				than the size of resource file!
			*/
			g_bpNumMaxPrv = rsvSize - 1;
		}
	}
	
	pResBp = &resBp[RES_BP_CAP];
	if (sp1kDiskRsvSizeGet(pResBp->resId, NULL, &rsvSize) == TRUE) {
		rsvSize >>= 1;	/*res is half size*/
		if ((UINT32)pPara->capBpMax > rsvSize) {
			/* 
				capture badpixel max number defined is more large 
				than the size of resource file!
			*/
			g_bpNumMaxCap = rsvSize - 1;
		}
	}

	return SUCCESS;
}

UINT8 bpCfgGet(bpPara_t **ppPara)
{
	if (!ppPara || !pBpPara) {
		return FAIL;
	}

	*ppPara = pBpPara;

	return SUCCESS;
}

UINT8 bpParaSet(UINT8 tpye,UINT32 shutter,UINT8 gain)
{
	bpPara_t *pPara = pBpPara;

	pPara->preAvg=0; 
	pPara->bpType = tpye;
	/*add 20091124*/
	if(tpye==0){
		g_threshold = pPara->threshold_W;
	}else {
		g_threshold = pPara->threshold_B;
	}
	/*add 20091124*/
	frontExposureTimeSet(0,shutter,1,0);
	frontGainSet(0,gain,1,0);	
	return SUCCESS;
}
//-----------------------------------------------------------------------------
//bpDramBpFix
//-----------------------------------------------------------------------------
/**
 * @brief		badpixel compensation fix by f/w
 * @param	addr: raw image source address
 			Hsize: the width of raw image
			xoff: the horizontal offset of badpixel
			yoff: the vertical offset of badpixel
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 bpDramBpFix(UINT32 addr, UINT16 Hsize, UINT16 xoff, UINT16 yoff)
{
	UINT8 ret = SUCCESS;
	UINT16 readvalue = 0;

	if ((xoff == 0) || (xoff == 1)) {
		if ((yoff == 0) || (yoff == 1)) {
			//printf("left and top!\n");
			ret = SUCCESS;
		} else {
			/* read */ /* Use the pixel above two line */
			ret = bpRawRead(addr, Hsize, xoff, yoff - 2, &readvalue);

			/* copy */
			ret = bpRawWrite(addr, Hsize, xoff, yoff, readvalue);
		}
	} else {
		/* read */ /* Use the pixel left two column */
		ret = bpRawRead(addr, Hsize, xoff - 2, yoff, &readvalue);

		/* copy */
		ret = bpRawWrite(addr, Hsize, xoff, yoff, readvalue);
	}
	
	return ret ;
}

//-----------------------------------------------------------------------------
//bpPrvEn
//-----------------------------------------------------------------------------
/**
 * @brief	Enable/disable preview badpixel compensation function
 * @param	enable: 1: enable, 0: disable
 * @retval	none
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug 	a bug need to be resolved.
*/
void bpPrvEn(UINT8 enable)
{
	UINT8 val = 0;

	/* local reset */
	//XBYTE[0x21f0] = 0x01; 
	//XBYTE[0x21f0] = 0x00;
    HAL_CdspRstBlock(CDSPRstCDSP);

	if (enable) {
		val = 0x01;
	} else {
		val = 0x00;
	}

	XBYTE[REG_Cdsp_badpixen] = val;
}

//-----------------------------------------------------------------------------
//bpPrvAddrSet
//-----------------------------------------------------------------------------
/**
 * @brief		Set preview badpixel compensation data address in dram
 * @param	preview badpixel resource file address in dram (type of word)
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug 	a bug need to be resolved.
*/
UINT8 bpPrvAddrSet(UINT32 addr)
{
	/* param check */
	if (addr > (K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixelCoordBufSize) || \
		addr < K_SDRAM_BadPixelCoordBufAddr) {
		return FAIL;
	}

	XBYTE[REG_Cdsp_badbaseaddr_byte0] = READ8(addr, 3);
	XBYTE[REG_Cdsp_badbaseaddr_byte1] = READ8(addr, 2);
	XBYTE[REG_Cdsp_badbaseaddr_byte2] = READ8(addr, 1);
	XBYTE[REG_Cdsp_badbaseaddr_byte3] = READ8(addr, 0);
	
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//bpResCfg
//-----------------------------------------------------------------------------
/**
 * @brief		badpixel compensation resource file configure
 * @param	resType: 0: preview mode 1: capture mode
			resId: the id of each resource file
 * @retval	0: success
 			1: fail
 * @see		none 
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug 	a bug need to be resolved.
*/
UINT8 bpResCfg(resBadPixel_t resType, UINT16 resId)
{
	UINT8 ret = SUCCESS;
	resBpCb_t xdata* pResBp;

	/* the max number of resource file is 0x80 in disk A */ 
	if ( resId > 0x800 ) {
		return FAIL;
	}

	switch (resType) {
	case RES_BP_PRV_HD:
		pResBp = &resBp[RES_BP_PRV_HD];
		break;
	case RES_BP_PRV_VGA:
		pResBp = &resBp[RES_BP_PRV_VGA];
		break;
	case RES_BP_CAP:
		pResBp = &resBp[RES_BP_CAP];
		break;
	default:
		pResBp = NULL;
		ret = FAIL;
		//printf("invalid input param\n");
		break;
	}

	if ( pResBp ) {
		pResBp->resId = resId;
	}

	return ret;
}

//-----------------------------------------------------------------------------
//bpResLoad
//-----------------------------------------------------------------------------
/**
 * @brief		load badpixel compensation resource file
 * @param	resType: 0: preview mode 1: capture mode
			addr: the destination address in dram (word)
 * @retval	0: false
 			others: resource file size
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug 	a bug need to be resolved.
*/

UINT32 bpResLoad(resBadPixel_t resType, UINT32 addr)
{
	UINT32 offset, size = 0;
	resBpCb_t xdata* pResBp;

	/* param check */
	if (resType >= RES_BP_MAX) {
		return FALSE;
	}

	pResBp = &resBp[resType];

	/* check if the resoure file id had already be registed */
	if (pResBp->resId == 0) {
		return FALSE;
	}

	sp1kNandRsvRead(pResBp->resId, addr << 1);
	pResBp->resAddr = addr;

	sp1kNandRsvSizeGet(pResBp->resId, &offset, &size);
	if (size > (K_SDRAM_BadPixelCoordBufSize << 1)) {
		/* the size of resource file is larger than mem buffer */
		return FALSE;
	}

	return size;
}

//-----------------------------------------------------------------------------
//bpResInfoGet
//-----------------------------------------------------------------------------
/**
 * @brief		get the badpixel resource file information
 * @param	resType: the type of badpixel resource file
 			pRes: pointer to the resource file info struct pointer
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-21
 * @todo		Something need to be done.
 * @bug		a bug need to be resolved.
*/
UINT8 bpResInfoGet(resBadPixel_t resType, resBpCb_t** pRes)
{
	resBpCb_t xdata* pResBp;

	/* param check */
	if ( resType >= RES_BP_MAX ) {
		return FAIL;
	}

	/* check if the resource file id had already been resgisted */
	pResBp = &resBp[resType];
	if ( pResBp->resId == 0 ) {
		return FAIL;
	}

	*pRes = pResBp;
	return SUCCESS;
}

//-----------------------------------------------------------------------------
//bpFix
//-----------------------------------------------------------------------------
/**
 * @brief		badpixel fix in capture mode
 * @param	addr: badpixel resource file address in dram
			rawWidth: the width of raw image
			rawHeight: the height of raw image
 * @retval	0: success
 			1: fail
 * @see		none
 * @author	Phil Lin
 * @since		2008-11-18
 * @todo		Something need to be done.
 * @bug 	a bug need to be resolved.
*/
#if 0
UINT8 bpFix(UINT32 rawAddr, UINT16 rawWidth, UINT16 rawHeight)
{
	UINT8 ret;
	UINT16 xdata* data pData;
	UINT16 badpixthd = 0x10;
	UINT16 bpHtag;
	UINT16 bpVtag;
	UINT32 timeOut; 
	UINT32 resAddr;
	const UINT16 blkSize = 0x400;	/* dma to DMEM block size */
	//resBpCb_t xdata* pResBp = &resBp[RES_BP_CAP];

	timeOut = ((K_SDRAM_BadPixel_CapScaleBufSize << 1) / (UINT32) blkSize) + 1;
	resAddr = K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize+K_SDRAM_BadPixel_CapBufSize;//pResBp->resAddr;

	/* Word * Line */
	HAL_DRAMbpcfg(rawAddr, rawWidth >> 1, rawHeight);
	g_threshold = badpixthd;
	
	printf("bp fix start\n");
	dbgTimerStart();
	
	while (--timeOut) {
		/* load part of badpixel resource file to DMEM */
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaSetDramAddr(resAddr);
		HAL_DmaDo(0, 1, blkSize, 0, 0);
		pData = (UINT16 xdata *) 0x3800;
		while (1) {
			/* resource file parsing */
			bpHtag = *pData;	/* 16 bits for x */
			pData++;
			bpVtag = *pData;	/* next 16 bits for y */
			pData++;

			/* check end mark */
			if ((bpHtag == 0xffff) || (bpVtag == 0xffff)) {
				dbgTimerStop();				
				return SUCCESS;
			}
			
#if DBG_BP_EN
			printf("bp-> Y: %lx, X: %lx\n", (UINT32)bpVtag, (UINT32)bpHtag);
#endif //DBG_BP_EN
			/* call bad pixel compensation function */
#if (!BP_CMP_BY_FW)
			HAL_DRAMbpfix(badpixthd, bpHtag, bpVtag, 1);
#else
			/* for byte switch */
#if BP_BYTE_SW_EN
{
			/* for high/low byte switch */
			UINT8 tmp;
			
			tmp = READ8(bpHtag, 0);
			WRITE8(bpHtag, 0, READ8(bpHtag, 1));
			WRITE8(bpHtag, 1, tmp);
			
			tmp = READ8(bpVtag, 0);
			WRITE8(bpVtag, 0, READ8(bpVtag, 1));
			WRITE8(bpVtag, 1, tmp);
}
#endif //BP_BYTE_SW_EN
			ret = bpDramBpFix(rawAddr, rawWidth, bpHtag, bpVtag);
			if ( ret != SUCCESS ) {
			/*
				the badpixel data is invalid, exit the badpixel compensation process 
			*/
				dbgTimerStop();
				__ASSERT(__FILE, __LINE, 1);
				return FAIL;
			}
#endif //BP_CMP_BY_FW

			/* check if flush the next part of resource to DMEM */
			if (pData >= (UINT16 xdata *) 0x2000) {
				break;
			}
			//XBYTE[0x2030] ^= 0x01;	
		}

		/* end marker */
		if ((bpHtag == 0xffff) || (bpVtag == 0xffff)) {
			dbgTimerStop();			
			return SUCCESS;
		}

		resAddr += (UINT32) (blkSize >> 1);
	}
	
	dbgTimerStop();	
	if (!timeOut) {
		ASSERT(timeOut, 1);
		return FAIL;
	} else {
		return SUCCESS;
	}
}

UINT8 bpSnapCropScale(
	UINT16 sensorXSize,
	UINT16 sensorYSize,
	UINT16 cropXSize,
	UINT16 cropYSize,
	UINT16 rawXSize,
	UINT32 resSize,
	UINT32 srcaddr,
	UINT32 desaddr,
	UINT8  mode
	)
{
	UINT8   cropEn=0;
	UINT8   ScaleEn=0;
	UINT16 xoff,yoff,xcropend,ycropend;
	UINT16 cropxsize,cropysize;
	UINT32 factor1,tempfac;
	UINT16 factor;
	UINT16 einifactor,oinifactor;
	UINT16 coorx1,coorx2;
	UINT32 tempx1,tempx2;
	UINT16 oddflag;
	UINT8   nox2=1;
	UINT32 timeOut;
	const UINT16 blkSize = 0x400;	/* dma to DMEM block size */
	UINT16 xdata* data pData;
	UINT16 bpHtag;
	UINT16 bpVtag;
	UINT8   inum;

	// mode: mode[0]:'0' disable crop,'1'enable crop; mode[1]:'0'disable H Scale,'1'enable H Scale
	printf("bp convert start\n");
	dbgTimerStart();
	timeOut = ((resSize << 1) / (UINT32) blkSize) + 1;

	/*+++++++++++++ for crop coordinate correction ++++++++++++++*/
	cropEn = (sensorXSize>cropXSize) && (mode&0x01 == 0x01);
	xoff = (sensorXSize-cropXSize)>>1;
	yoff = (sensorYSize-cropYSize)>>1;
	cropxsize = (cropXSize+1) & ~1;
	cropysize = cropYSize;
	xoff = xoff & ~1;
	yoff = yoff & ~1;
	xcropend = xoff+cropxsize;
	ycropend = yoff+cropysize;
	/*---------------------------------------------------------*/

	/*+++++++++++++ for H Scale coordinate correction ++++++++++++*/
	ScaleEn = (cropXSize>rawXSize) && (mode&0x02 == 0x02);
	factor1 = ( ( ( UINT32 ) rawXSize << 16 ) / ( UINT32 ) cropXSize ) + 1;
	factor = ( UINT16 ) factor1;
	einifactor = ( factor / 2 ) + 0x8000;
	oinifactor = factor;
	/*---------------------------------------------------------*/

	while (--timeOut) {
		/* load part of badpixel resource file to DMEM */
		HAL_DmaSetSramAddr(0x0000);
		HAL_DmaSetDramAddr(srcaddr);
		HAL_DmaDo(0, 1, blkSize, 0, 0);
		pData = (UINT16 xdata *) 0x3800;
		while (1) {
			/* resource file parsing */
			bpHtag = *pData;	/* 16 bits for x */
			pData++;
			bpVtag = *pData;	/* next 16 bits for y */
			pData++;

			/* check end mark */
			if ((bpHtag == 0xffff) || (bpVtag == 0xffff)) {
				dbgTimerStop();		
				BP_DRAM_SetStartAddress(desaddr, 0);
				desaddr++;
				BP_DRAM_WriteWord(READ8(bpHtag, 0), READ8(bpHtag, 1));
				BP_DRAM_SetStartAddress(desaddr, 0);
				desaddr++;
				BP_DRAM_WriteWord(READ8(bpVtag, 0), READ8(bpVtag, 1));
				return SUCCESS;
			}
			
			if (cropEn){//TQ@20090820 for snap Zoom in, bp error issue
				if (bpHtag<xoff || bpVtag<yoff || bpHtag>=xcropend || bpVtag>=ycropend){
					continue;
					}
				bpHtag -= xoff;
				bpVtag -= yoff;
				}

			if (ScaleEn){// TQ@20090828 for snap H scale coordinate correction
				if (bpHtag<2){
					tempx1 = 0;
					nox2 = 1;   // X2 is NA
				}
				else{
					oddflag = bpHtag & 0x0001;
					if (oddflag){
						tempfac = (((UINT32)bpHtag>>1)-1)*(UINT32)factor+(UINT32)oinifactor;
						tempx1 = (((tempfac+65535)>>16)<<1) | 0x00000001;
					}
					else{
						tempfac = (((UINT32)bpHtag>>1)-1)*(UINT32)factor+(UINT32)einifactor;
						tempx1 = (((tempfac+65535)>>16)<<1);
					}
					if (((tempfac%65536)<factor) && (tempx1>=2)){
							tempx2 = tempx1 - 2;
							nox2 = 0;
					}
					else{
						nox2 = 1;
					}
				}
				coorx1 = (UINT16)tempx1;
				coorx2 = (UINT16)tempx2;
			}
			else {
				coorx1 = bpHtag;
				nox2 = 1;
			}
		
			for (inum=0;inum<(2-nox2);inum++){// when have done H Scale, maybe fix two point
				if (inum==0){
					bpHtag = coorx1;
				}
				else{
					bpHtag = coorx2;
				}
				
#if DBG_BP_EN
			printf("bp-> X: %lx, Y: %lx\n", (UINT32)bpHtag, (UINT32)bpVtag);
#endif //DBG_BP_EN

			/* call bad pixel compensation function */
			BP_DRAM_SetStartAddress(desaddr, 0);
			desaddr++;
			BP_DRAM_WriteWord(READ8(bpHtag, 0), READ8(bpHtag, 1));
			BP_DRAM_SetStartAddress(desaddr, 0);
			desaddr++;
			BP_DRAM_WriteWord(READ8(bpVtag, 0), READ8(bpVtag, 1));

}

			/* check if flush the next part of resource to DMEM */
			if (pData >= (UINT16 xdata *) 0x2000) {
				break;
			}
			//XBYTE[0x2030] ^= 0x01;	
		}

		/* end marker */
		if ((bpHtag == 0xffff) || (bpVtag == 0xffff)) {
			dbgTimerStop();	
			BP_DRAM_SetStartAddress(desaddr, 0);
			desaddr++;
			BP_DRAM_WriteWord(READ8(bpHtag, 0), READ8(bpHtag, 1));
			BP_DRAM_SetStartAddress(desaddr, 0);
			desaddr++;
			BP_DRAM_WriteWord(READ8(bpVtag, 0), READ8(bpVtag, 1));
			return SUCCESS;
		}

		srcaddr += (UINT32) (blkSize >> 1);
	}
	
	return FAIL;
}

UINT8 bpCoordConvert(void)
{
	UINT16 dzoomXSize16;
	UINT8 ret;
	
	frontSnapSizeGet(0, &snapSensorXSize, &snapSensorYSize);

	ASSERT( ( PvZoomCtrl.factor != 0 ), 0 );

	snapSensorCropXSize = (UINT16)( (UINT32)snapSensorXSize * (UINT32)100 / (UINT32)PvZoomCtrl.factor );
	snapSensorCropYSize = (UINT16)( (UINT32)snapSensorYSize * (UINT32)100 / (UINT32)PvZoomCtrl.factor );

	// raw buffer x size must be 16 alignment.
	dzoomXSize16 = ( snapSensorCropXSize ) >> 4 << 4;
 	snapRawXSize = ( snapXSize < dzoomXSize16 ) ? snapXSize : dzoomXSize16;
  
	ret = bpSnapCropScale(snapSensorXSize, snapSensorYSize,
					snapSensorCropXSize, snapSensorCropYSize,
					snapRawXSize,
					(UINT32)16*(UINT32)1024,
					K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize,
					K_SDRAM_BadPixelCoordBufAddr + K_SDRAM_BadPixel_PrvBufSize+K_SDRAM_BadPixel_CapBufSize,
					(UINT8)3
					);

	return ret;

}
#endif
#endif /* MODULE_BP_OPT */