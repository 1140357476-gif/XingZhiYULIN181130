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
 * @file      dist_func.h
 * @brief     header file of dist_func.c
 * @author    TQ.zhang
 * @since     2010-04-29
 * @date      2010-04-29
*/
#ifndef _DIST_FUNC_H__
#define _DIST_FUNC_H__

#if FUNC_KNL_DIST
	#define DIST_FUNC_EN 
#endif
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
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

void distVectorGet();
void distModeFuncSet(UINT8 mode);
void distModeFuncGet(UINT8* mode);
void distDispSet(void);
void distEdgeAddrGet(UINT8 mode,UINT32* EdgeAddrA,UINT32* EdgeAddrB);
void distMeOutAddrGet(UINT8 mode,UINT32* MeAddr0);
void distGmvSet(UINT8 GmvX, UINT8 GmvY);
void distGmvGet(UINT8* GmvX, UINT8* GmvY);
void distClampSizeSet(UINT8 ClmpX, UINT8 ClmpY);
void distClampSizeGet(UINT8* ClmpX, UINT8* ClmpY);
void distSwitchSize(UINT16 SrcXsize, UINT16 SrcYsize);

void distIntr();
void distSvcProc(UINT8* pt);

//void distFuncOpen(UINT8 mode);
//void sp1kDistDisable(void);
//void distFuncEn(UINT8 en);
//void distFuncEnGet(UINT8* en);
//void distCalMv(void);

/* Intrrupt Function */
//void distGmvSetIntr(void);


#endif
