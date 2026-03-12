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
#ifndef __SP1K_BP_API_H__
#define __SP1K_BP_API_H__

/** \file sp1k_bp_api.h
 * @brief		Spca1628 badpixel related interfaces
 */

#include "bp_api.h"
#include "sp1k_def.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/


/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#ifdef SP1K_DOXY_GEN
/**
 * @fn void sp1kBpPrvEn(UINT8 enable)
 * @brief		Enable/disable preview badpixel compensation function
 * @param[in]	enable: 1: enable, 0: disable
 * @retval	none
 * @see	   sp1kBpPrvAddrSet();
*/
void sp1kBpPrvEn(UINT8 enable);

/**
 * @fn UINT8 sp1kBpPrvAddrSet(UINT32 addr)
 * @brief		Enable / disable badpixel function in preview mode
 * @param[in]	addr: preview badpixel resource file address in dram (type of word)
 * @retval	error code -  success: 0, fail: 1
 * @see	   sp1kBpPrvEn();
*/
UINT8 sp1kBpPrvAddrSet(UINT32 addr);

/**
 * @fn UINT8 sp1kBpResCfg(resBadPixel_t resType, UINT16 resId)
 * @brief		configure badpixel compensation resource file
 * @param[in]	resType: 0: preview mode 1: capture mode
 * @param[in]	resId: the id of each resource file
 * @retval	error code -  success: 0, fail: 1
 * @see	   sp1kBpResLoad(); sp1kBpResInfoGet();
*/
UINT8 sp1kBpResCfg(resBadPixel_t resType, UINT16 resId);

/**
 * @fn UINT32 sp1kBpResLoad(resBadPixel_t resType, UINT32 addr)
 * @brief		load badpixel compensation resource file
 * @param[in]	resType: 0: preview mode 1: capture mode
 * @param[in]	addr: the destination address in dram (word)
 * @retval	0: false, others: resource file size
 * @see	   sp1kBpResCfg(); sp1kBpResInfoGet();
*/
UINT32 sp1kBpResLoad(resBadPixel_t resType, UINT32 addr);

/**
 * @fn UINT8 sp1kBpResInfoGet(resBadPixel_t resType, resBpCb_t** pRes)
 * @brief		get the badpixel resource file information
 * @param[in]	resType: the type of badpixel resource file
 * @param[out]	pRes: pointer to the resource file info struct pointer
 * @retval	error code -  success: 0, fail: 1
 * @see	   sp1kBpResCfg(); sp1kBpResLoad();
*/
UINT8 sp1kBpResInfoGet(resBadPixel_t resType, resBpCb_t** pRes);

/**
 * @fn UINT8 sp1kBpCfg(bpPara_t* pPara)
 * @brief		configure threshold & max number of badpixel function
 * @param[in]	pPara: point to the badpixel configuration parameter
 * @retval	error code -  success: 0, fail: 1, param error: -1
 * @see	   sp1kBpFind();
*/
UINT8 sp1kBpCfg(bpPara_t* pPara);

/**
 * @fn UINT8 sp1kBpFind(bpMode_t mode, UINT32 rawAddr, UINT16 rawWidth, UINT16 rawHeight, UINT32 badPixAddr, UINT32* badPixNum)
 * @brief		badpixel find process
 * @param[in]	mode: 0:preview mode, 1: capture mode
 * @param[in] 	rawAddr: raw image source address(word)
 * @param[in] 	rawWidth: the width of raw image
 * @param[in] 	rawHeight: the height of raw image
 * @param[in] 	badPixAddr: badpixel description data destination address
 * @param[out] 	badPixNum: badpixel total number
 * @retval	error code -  success: 0, fail: 1
 * @see	   sp1kBpCfg();
*/
UINT8 sp1kBpFind(bpMode_t mode, UINT32 rawAddr, UINT16 rawWidth, UINT16 rawHeight, UINT32 badPixAddr, UINT32* badPixNum);

/**
 * @fn UINT8 sp1kBpFix(UINT32 rawAddr, UINT16 rawWidth, UINT16 rawHeight)
 * @brief		badpixel fix in capture mode
 * @param[in]	addr: badpixel resource file address in dram
 * @param[in]	rawWidth: the width of raw image
 * @param[in]	rawHeight: the height of raw image
 * @retval	error code -  success: 0, fail: 1
 * @see	   none
*/
UINT8 sp1kBpFix(UINT32 rawAddr, UINT16 rawWidth, UINT16 rawHeight);


#else

#define sp1kBpPrvEn			bpPrvEn
#define sp1kBpPrvAddrSet	bpPrvAddrSet

#define sp1kBpResCfg		bpResCfg
#define sp1kBpResLoad		bpResLoad
#define sp1kBpResInfoGet	bpResInfoGet

#define sp1kBpCfg			bpCfg
#define sp1kBpFind			bpFind

#define sp1kBpFix			bpFix
#define sp1kBpCoordConvert     bpCoordConvert

#define sp1kBpParaSet		bpParaSet
/* Definition for F/W compatible, Pls use the new badpixel functions above */
#if 1
#define sp1kCalBpProcAll(a)
#define sp1kCalBpProcRaw(a)
#define sp1kCalBpProcFind(a)
#endif

#endif

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/


/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

#endif  /* __SP1K_DISP_API_H__ */

