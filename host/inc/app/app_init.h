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
#ifndef __APP_INIT_H__
#define __APP_INIT_H__

#include "solution.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define LIFE_SPAN_TEST		0

#define NAND_SUPPORT_ONLY	0
#define NAND_SUPPORT		1

#define TV_PLAY_OPTION		0
#define TV_FULL_OPTION		1
#define TV_NOSUP_OPTION		2
#define TV_OPTION       	TV_FULL_OPTION

/*
	1, First LUN = Build in
	2, First LUN = Card
*/
#define MLUN_OPTION			1

typedef struct
{
	UINT32 currentBank;
	UINT32 strAddr;
	UINT32 strSize;
	UINT32 strIdxAddr;
	UINT32 strIdxSize;
	UINT32 paraAddr;
	UINT32 paraSize;
	UINT32 adfAddr;
	UINT32 adfSize;
    
}UIToolInfo;


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
void memInit(void);
void appInit(void);
void appMemDosRamInit(void);


#endif  /* __APP_INIT_H__ */

