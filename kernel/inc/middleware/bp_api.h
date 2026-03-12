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
 * @file		bp_api.h
 * @brief		badpixel function definition.
 * @author	Phil Lin
 * @since		2008-11-24
 * @date		2008-11-24
 */
#ifndef __BP_API_H__
#define __BP_API_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "knl_func_opt.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define MODULE_BP_OPT   FUNC_KNL_BP
/**************************************************************************
 *  							M A C R O S 							  *
 **************************************************************************/

/**************************************************************************
 *  						D A T A    T Y P E S						  *
 **************************************************************************/	
typedef enum {
	BP_IMG_WHITE = 0, /* white image */
	BP_IMG_BLACK,  /* black image */
	BP_IMG_MAX,
} bpImg_t;

typedef enum {
	BP_MODE_PRV_HD = 0, /* preview mode */
	BP_MODE_PRV_VGA,	
	BP_MODE_CAP,  /* capture mode */
	BP_MODE_MAX,
} bpMode_t;

typedef enum{
	BP_TYPE_LEFT=0x0000,
	BP_TYPE_RIGHT=0x4000,
	BP_TYPE_UP=0x8000,
	BP_TYPE_DOWN=0xC000,
}bpType_t;

typedef enum {
	RES_BP_PRV_HD = 0, /* preview mode */
	RES_BP_PRV_VGA,	
	RES_BP_CAP,  /* capture mode */
	RES_BP_MAX,
} resBadPixel_t;

typedef enum {
	PV_MODE_HD = 0, /* preview mode */
	PV_MODE_VGA,	
}pvMode_t;

typedef struct resBpCb_s {
	UINT16 resId;
	UINT32 resAddr;
} resBpCb_t;

typedef struct bpPara_s {
	UINT16 prvWidth_HD;
	UINT16 prvHeight_HD;
	UINT16 prvWidth;
	UINT16 prvHeight;
	UINT16 capWidth;
	UINT16 capHeight;
	UINT16 threshold_B;
	UINT16 threshold_W;	
	UINT32 prvBpMax_HD;
	UINT32 prvBpMax;
	UINT32 capBpMax;
	UINT8 bpType;
	UINT32 preAvg;
	UINT16 eof;
} bpPara_t;

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
#if MODULE_BP_OPT

/* preview badpixel compensation related function */
void bpPrvEn(UINT8 enable);
UINT8 bpPrvAddrSet(UINT32 addr);

/* badpixel resource file related function */
UINT8 bpResCfg(resBadPixel_t resType, UINT16 resId);
UINT32 bpResLoad(resBadPixel_t resType, UINT32 addr);
UINT8 bpResInfoGet(resBadPixel_t resType, resBpCb_t** pRes);

/* badpixel find function */
UINT8 bpCfg(bpPara_t* pPara);
UINT8 bpCfgGet(bpPara_t **ppPara);
UINT8 bpFind(bpMode_t mode, UINT32 rawAddr, UINT16 rawWidth, UINT16 rawHeight, UINT32 badPixAddr, UINT32* badPixNum);

/* badpixel fix fucntion */
UINT8 bpFix(UINT32 rawAddr, UINT16 rawWidth, UINT16 rawHeight);
UINT8 bpCoordConvert(void);
UINT8 bpParaSet(UINT8 tpye,UINT32 shutter,UINT8 gain);

#else

/* preview badpixel compensation related function */
#define bpPrvEn(enable)
#define bpPrvAddrSet(addr)

/* badpixel resource file related function */
#define bpResCfg(resType, resId)    	SUCCESS //return value
#define bpCfgGet(ppPara)    			(*(ppPara) = NULL)
#define bpResLoad(resType, addr)    	SUCCESS //return value
#define bpResInfoGet(resType, pRes)

/* badpixel find function */
#define bpCfg(pPara)
#define bpFind(mode, rawAddr, rawWidth, rawHeight, badPixAddr, badPixNum)

/* badpixel fix fucntion */
#define bpFix(rawAddr, rawWidth, rawHeight)
#define bpCoordConvert()
#define bpParaSet(tpye,shutter,gain)

#endif /* MODULE_BP_OPT */

#endif  /* __BP_API_H__ */
