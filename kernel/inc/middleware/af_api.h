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
#ifndef __AF_API_H__
#define __AF_API_H__

#include "knl_func_opt.h"

#define AF_FIXTURE_EN	FUNC_KNL_AF_FIXTURE


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef struct _AFstr{
	UINT8 afFlt2Type;	//filter type : 0,-1,2,-1,0 filter type : 1,-4,6,-4,1
	UINT8 afRgbMode;
	UINT8 afPgType;
	UINT8 afFlt1Wt;
	UINT8 afFlt2Wt;
	UINT8 afCorThr;
	UINT8 afCorVal;

}AFstr;

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
void afWinEnSet(UINT8 en);
UINT8 afWinEnGet(void);
void afIntrWinValueGet( void);
void afWinCfg(UINT16 frontHSize,UINT16 frontVSize,UINT16 cropHSize,UINT16 cropVSize);
UINT32 afWinValueGet(void);



#endif //__AF_API_H__

