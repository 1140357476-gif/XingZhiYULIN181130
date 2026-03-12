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
 * @file      osd_mem.h
 * @brief     osd memory function
 * @author    Phil Lin
 * @since     2010-01-01
 * @date      2010-01-01
*/

#ifndef __OSD_MEM_H__
#define __OSD_MEM_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "osd_com.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct osdMemCtrl_s {
	UINT32 addr; //word address
	UINT32 size; //byte size
} osdMemCtrl_t;

typedef struct osdSrcSize_s
{
	UINT32 iconSize;
	UINT32 EngSize;
	UINT32 FontSize;
	UINT32 StrSize;
}osdSrcSize_t;


typedef struct osdMem_s {
	osdMemCtrl_t dispA; //osd display buffer A
	osdMemCtrl_t dispB; //osd display buffer B
	osdMemCtrl_t datDraw; //temp buffer for data draw
	osdMemCtrl_t icon; //osd icon buffer
	osdMemCtrl_t cInfo; //information for osd char (height / width / absolute address for osd resource)
	osdMemCtrl_t char8; //char shown in byte
	osdMemCtrl_t char16; //char shown in word
	osdMemCtrl_t res; //resource shown in word
} osdMem_t;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if MODULE_OSD_OPT

UINT32 osdMemSizeInit(void);
UINT8 osdMemInit(void);
void osdMemCbGet(osdMem_t** ppMem);
void osdSrcSizeSet(osdSrcSize_t *pSrcSize);

#else

#define osdMemSizeInit()	0
#define osdMemInit()
#define osdMemCbGet(ppMem)	(*(ppMem) = NULL)
#define osdSrcSizeSet(pSrcSize)

#endif /* MODULE_OSD_OPT */

#endif  /* __OSD_MEM_H__ */
