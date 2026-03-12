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
 * @file      osd_mem.c
 * @brief     osd memory function
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#define __FILE __FILE_ID_OSD_MEM__ 

#include "general.h" 
#include "dbg_def.h" 
#include "initio.h" 
#include "res_macro.h"

#include "osd_com.h" 
#include "osd_api.h" 
#include "osd_mem.h" 

#include "sp1k_rsvblk_api.h"

#if MODULE_OSD_OPT
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if 0
#define osdMemPrintf printf
#else
#define osdMemPrintf /DBG_SLASH
#endif
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static osdMem_t xdata gOsdMem; //osd buffer control block
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static UINT32 gOsdIconSize;
static UINT32  gOsdEngSize;
static UINT32 gOsdFontSize;
static UINT32 gOsdStrSize;
void osdSrcSizeSet(osdSrcSize_t *pSrcSize)
{
	gOsdIconSize = pSrcSize->iconSize;
	gOsdEngSize = pSrcSize->EngSize;
	gOsdFontSize = pSrcSize->FontSize;
	gOsdStrSize = pSrcSize->StrSize;
}

UINT32 osdMemSizeInit(void)
{
	osdMem_t* pMem = &gOsdMem;
	UINT32 bitDivIcon;
	UINT32 bitDivLang;
	gOsdFmt_t* pFmt;
	UINT32 osdTotalSize;
	const UINT32 osdMemSizeTotByte = -1;
	//UINT32 EnSize;
	//sp1kNandRsvSizeGet(0x5a,NULL,&EnSize);
	
	osdFmtCfg(OSD_FMT_2BIT, OSD_FMT_2BIT);
	/* check the osd resource file format */
	osdFmtGet(&pFmt);
	if(pFmt->icon == OSD_FMT_2BIT) {
		bitDivIcon = 4; //osd resource file format: 2bit
	} else { 
		bitDivIcon = 1; //osd resource file format: 8bit
	}
	if(pFmt->lang == OSD_FMT_2BIT) {
		bitDivLang = 4; //osd resource file format: 2bit
	} else { 
		bitDivLang = 1; //osd resource file format: 8bit
	}

	pMem->dispA.size = 320UL * 240UL; //320*240
	pMem->dispB.size = 320UL * 240UL; //320*240
	pMem->datDraw.size = 320UL * 240L;//32UL;
	pMem->icon.size = gOsdIconSize+1;//ICON_FILE_SIZE;//512UL * 640UL / bitDivIcon;//80K  

	pMem->cInfo.size =0;// 0x100UL * 4UL;
	pMem->char8.size = gOsdEngSize+1 ;// 16K//include chinese & some western europe language.
	pMem->char16.size =gOsdFontSize+1;// 512UL * 384UL / bitDivLang;//48K
	pMem->res.size=0;// 288*32*2UL;
														
	osdTotalSize = pMem->dispA.size + pMem->dispB.size + pMem->datDraw.size +pMem->icon.size+\
		pMem->cInfo.size + pMem->char8.size + pMem->char16.size+pMem->res.size;
		
	/* check memory size */
	ASSERT( \
		osdTotalSize < osdMemSizeTotByte, \
		0 \
	);

	osdMemPrintf("(disp A).sizeB = %lx\n",  pMem->dispA.size);
	osdMemPrintf("(disp B).sizeB = %lx\n", pMem->dispB.size);
	osdMemPrintf("(datDraw).sizeB = %lx\n", pMem->datDraw.size);
	osdMemPrintf("(icon).sizeB = %lx\n", pMem->icon.size);
	osdMemPrintf("(cInfo).sizeB = %lx\n", pMem->cInfo.size);
	osdMemPrintf("(char8).sizeB = %lx\n",pMem->char8.size);
	osdMemPrintf("(char16).sizeB = %lx\n", pMem->char16.size);
	
	//printf("osdtotalsize = %lx\n",osdTotalSize);
	return osdTotalSize>>1;

}


/**
 * @fn        UINT8 osdMemInit(void)
 * @brief     osdMemInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-25
 * @todo      N/A
 * @bug       N/A
*/
UINT8
osdMemInit(
	void
)
{
	osdMem_t* pMem = &gOsdMem;
	const UINT32 osdMemStartAddrWord = K_SDRAM_OsdBufAddr;
	
	pMem->dispA.addr = osdMemStartAddrWord;
	
	pMem->dispB.addr = pMem->dispA.addr + (pMem->dispA.size >> 1);

	pMem->datDraw.addr = pMem->dispB.addr + (pMem->dispB.size >> 1);

	pMem->icon.addr = pMem->datDraw.addr + (pMem->datDraw.size >> 1);

	pMem->cInfo.addr = pMem->icon.addr + (pMem->icon.size >> 1);
	
	pMem->char8.addr = pMem->cInfo.addr + (pMem->cInfo.size >> 1);
	
	pMem->char16.addr = pMem->char8.addr + (pMem->char8.size >> 1);
	pMem->res.addr = pMem->char16.addr + (pMem->char16.size >> 1);

	osdMemPrintf("(disp A).addrW = %lx\n", pMem->dispA.addr);
	osdMemPrintf("(disp B).addrW = %lx\n", pMem->dispB.addr);
	osdMemPrintf("(datDraw).addrW = %lx\n", pMem->datDraw.addr);
	osdMemPrintf("(icon).addrW = %lx\n", pMem->icon.addr);
	osdMemPrintf("(cInfo).addrW = %lx\n", pMem->cInfo.addr);
	osdMemPrintf("(char8).addrW = %lx\n", pMem->char8.addr);
	osdMemPrintf("(char16).addrW = %lx\n", pMem->char16.addr);

	return SUCCESS;
}


/**
 * @fn        void osdMemCbGet(osdMem_t** ppMem)
 * @brief     osdMemCbGet
 * @param     [out] ppMem
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
void
osdMemCbGet(
	const osdMem_t** ppMem
)
{
	*ppMem = &gOsdMem;
}
#endif /* MODULE_OSD_OPT */
