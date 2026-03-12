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
 * @file      hal_front_para.c
 * @brief     hal front parameter setting function table
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_front.h"
//#include "hal_front_internal.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (code *HAL_FrontParaSetFunc_t)(SINT16 *paraAry, UINT8 upd);
	
typedef struct {
	HAL_FrontParaSetFunc_t func;
} HAL_FrontParaSetFuncEntry_t;

/* the order and totla of entries in HAL_FrontParaSetFuncTbl[] must be
   the same to HAL_FrontParaId_e. */
static code HAL_FrontParaSetFuncEntry_t HAL_FrontParaSetFuncTbl[] = {
	{HAL_FrontParaMClkCfgSet                },
	{HAL_FrontParaPClkCfgSet                },
	{HAL_FrontParaSyncInvertEnableSet       },
	{HAL_FrontParaByPassEnableSet           },
	{HAL_FrontParaReshapeEnableSet          },
	{HAL_FrontParaReshapeRegionSet          },
	{HAL_FrontParaCropRegionSet             },
	{HAL_FrontParaFrontToCdspGatingEnableSet},
	{HAL_FrontParaTgTotalRegionSet          },
	{HAL_FrontParaTgSyncGeneratorEnableSet  },
	{HAL_FrontParaYuvOutputSequenceCfgSet   },
	{HAL_FrontParaYuvCcirModeSet            },
	{HAL_FrontParaYuvSub128EnableSet        },
	{HAL_FrontParaYuvInputEnableSet         },
};
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 HAL_FrontParaSet(UINT8 id, SINT16 para1, SINT16 para2, SINT16 para3, SINT16 para4, UINT8 upd)
 * @brief     set hal front misc parameters
 * @param     [in] id
 * @param     [in] para1 :parameter #1
 * @param     [in] para2 :parameter #2
 * @param     [in] para3 :parameter #3
 * @param     [in] para4 :parameter #4
 * @param     [in] upd : 
 *						0: immediately update
 *						1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-4-30
 * @todo      N/A
 * @bug       N/A
*/
UINT8
HAL_FrontParaSet(
	UINT8 id,
	SINT16 para1,
	SINT16 para2,
	SINT16 para3,
	SINT16 para4,
	UINT8 upd
)
{
	SINT16 paraAry[4];
	UINT8 temp = 0;

	if ( id < HAL_FRONT_PARA_MAX_ID && HAL_FrontParaSetFuncTbl[id].func != NULL ) {
		/* call sub-function in function table. */
 		paraAry[0] = para1;
		paraAry[1] = para2;
		paraAry[2] = para3;
		paraAry[3] = para4;
		(HAL_FrontParaSetFuncTbl[id].func)(paraAry, upd);
		
		/* dummy code for function call tree correction s */
		if (temp) {
			temp = (UINT8)HAL_FrontParaSetFuncTbl;
		}
		/* dummy code for function call tree correction e */

		return SUCCESS;
	}
	else {
		return FAIL;
	}
}

