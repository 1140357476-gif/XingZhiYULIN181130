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
 * @file		hal_disp_para.c
 * @brief		hal disp parameter setting function table.
 * @author		Matt Wang
 * @since		2008-06-17
 * @date		2008-12-08
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_disp_internal.h"
#include "hal_disp.h"

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef void (code *HAL_DispParaSetFunc_t)(SINT16 *paraAry);

typedef struct {
	HAL_DispParaSetFunc_t func;
} HAL_DispParaSetFuncEntry_t;

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* the order and totla of entries in HAL_DispParaSetFuncTbl[] must be
   the same to HAL_DispParaId_e. */
static code HAL_DispParaSetFuncEntry_t HAL_DispParaSetFuncTbl[] = {
    {HAL_DispParaSwRst                      },
	{HAL_DispParaClkEnableSet               },
	{HAL_DispParaClkCfgSet                  },
	{HAL_DispParaClkPhaseDlyCfg 			},
	{HAL_DispParaSignalPolarCfg             },
	{HAL_DispParaGlobEnableSet              },
	{HAL_DispParaImgLayerEnableSet          },
	{HAL_DispParaImgLayerFlipSet            },
	{HAL_DispParaImgLayerMonochromeEnableSet},
	{HAL_DispParaOsdLayerEnableSet          },
	{HAL_DispParaBgColorSet                 },
	{HAL_DispParaReqLenthSet                },
	{HAL_DispParaDeviceTypeSet              },
	{HAL_DispParaDeviceTotalRegionSet       },
	{HAL_DispParaDeviceActiveRegionSet      },
	{HAL_DispParaDeviceImgRegionCropSet     },
	{HAL_DispParaDeviceImgRegionRenderSet   },
	{HAL_DispParaDeviceOsdRegionCropSet     },
	{HAL_DispParaDeviceOsdRegionRenderSet   },
	{HAL_DispParaOsdBlendFactorSet          },
	{HAL_DispParaInterlaceCfgSet            },
	{HAL_DispParaLcdDitherEnableSet         },
	{HAL_DispParaUps051PixelShiftCfgSet     },
	{HAL_DispParaUps051LightviewEnableSet   },
	{HAL_DispParaUps051RGB888EnableSet      },
	{HAL_DispParaUps052RgbOnlyEnableSet     },
	{HAL_DispParaUpsHsdDelayCfgSet          },
	{HAL_DispParaUpsClkPhaseSet             },
	{HAL_DispParaUpsLayoutModeSet           },
	{HAL_DispParaUpsColorFilterCfgSet       },
	{HAL_DispParaCcirOutputSequenceCfgSet   },
	{HAL_DispRaraEdgeTypeSet                },
	{HAL_DispRaraEdgeGainSet                },
	{HAL_DispParaGammaEnableSet             },
	{HAL_DispParaRBSwitchSet                },
	{HAL_DispParaColorMatrixTblCfgSet       },
	{HAL_DispParaColorMatrixEnableSet		},
	{HAL_DispParaColorMatrixFmtCfg   		},
	{HAL_DispParaTvDacPowerEnableSet        },
	{HAL_DispParaTvSaturationSet            },
	{HAL_DispParaTvHueSet                   },
	{HAL_DispParaLcmCmdBusTypeSet           },
	{HAL_DispParaLcmCmdBitWidthSet          },
	{HAL_DispParaLcmCmdPulseWidthSet        },
	{HAL_DispParaLcmCmdActiveLevelSet       },
	{HAL_DispParaLcmDataFormatSet           },
	{HAL_DispParaLcmRgb666PackEnableSet     },
	{HAL_DispParaLcmRgb666Format10EnableSet },
	{HAL_DispParaLcmPixelAlignSet           },
	{HAL_DispParaLcmColorPackageSet         },
	{HAL_DispParaLcmSwitchGEnableSet        },
	{HAL_DispParaLcmChangeRbEnableSet       },
	{HAL_DispParaLcmOneHalfPixelEnableSet   },
	{HAL_DispParaLcmDitherEnableSet         },
};

/**
 * @brief	set hal disp misc parameters.
 * @param	id = [in] parameter id, one of HAL_DispParaId_e.
 * @param	para1 = [in] parameter #1.
 * @param	para2 = [in] parameter #2.
 * @param	para3 = [in] parameter #3.
 * @param	para4 = [in] parameter #4.
 * @param	upd = [in] 0: no update, 1: zfifo update, 2: cpu update, 3: sof update.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-05-23
 * @todo	N/A
 * @bug		N/A
*/
UINT8
HAL_DispParaSet(
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

	upd = upd;

	if ( id < HAL_DISP_PARA_MAX_ID && HAL_DispParaSetFuncTbl[id].func != NULL ) {
		/* call sub-function in function table. */
 		paraAry[0] = para1;
		paraAry[1] = para2;
		paraAry[2] = para3;
		paraAry[3] = para4;
		(HAL_DispParaSetFuncTbl[id].func)(paraAry);
	}

	/* dummy code for function call tree correction s */
	if (temp) {
		temp = (UINT8)HAL_DispParaSetFuncTbl;
	}
	/* dummy code for function call tree correction e */
	
	return SUCCESS;
}

