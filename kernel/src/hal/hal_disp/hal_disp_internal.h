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
 * @file		hal_disp_internal.h
 * @brief		hal disp internal header file.
 * @author		Matt Wang
 * @since		2008-06-17
 * @date		2008-08-19
 */

#ifndef __HAL_DISP_INTERNAL_H__
#define __HAL_DISP_INTERNAL_H__

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
extern UINT8 HAL_DispCs0Act;
extern UINT8 HAL_DispCs0Inact;
extern UINT8 HAL_DispA0Cmd;
extern UINT8 HAL_DispA0Data;

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
void HAL_DispParaSwRst(SINT16 *paraAry);
void HAL_DispParaClkEnableSet(SINT16 *paraAry);
void HAL_DispParaClkCfgSet(SINT16 *paraAry);
void HAL_DispParaClkPhaseDlyCfg(SINT16 *paraAry);
void HAL_DispParaSignalPolarCfg(SINT16 *paraAry);
void HAL_DispParaGlobEnableSet(SINT16 *paraAry);
void HAL_DispParaImgLayerEnableSet(SINT16 *paraAry);
void HAL_DispParaImgLayerFlipSet(SINT16 *paraAry);
void HAL_DispParaImgLayerMonochromeEnableSet(SINT16 *paraAry);
void HAL_DispParaOsdLayerEnableSet(SINT16 *paraAry);
void HAL_DispParaBgColorSet(SINT16 *paraAry);
void HAL_DispParaReqLenthSet(SINT16 *paraAry);
void HAL_DispParaDeviceTypeSet(SINT16 *paraAry);
void HAL_DispParaDeviceTotalRegionSet(SINT16 *paraAry);
void HAL_DispParaDeviceActiveRegionSet(SINT16 *paraAry);
void HAL_DispParaDeviceImgRegionCropSet(SINT16 *paraAry);
void HAL_DispParaDeviceImgRegionRenderSet(SINT16 *paraAry);
void HAL_DispParaDeviceOsdRegionCropSet(SINT16 *paraAry);
void HAL_DispParaDeviceOsdRegionRenderSet(SINT16 *paraAry);
void HAL_DispParaOsdBlendFactorSet(SINT16 *paraAry);
void HAL_DispParaInterlaceCfgSet(SINT16 *paraAry);
void HAL_DispParaLcdDitherEnableSet(SINT16 *paraAry);
void HAL_DispParaUps051PixelShiftCfgSet(SINT16 *paraAry);
void HAL_DispParaUps051LightviewEnableSet(SINT16 *paraAry);
void HAL_DispParaUps051RGB888EnableSet(SINT16 *paraAry);
void HAL_DispParaUps052RgbOnlyEnableSet(SINT16 *paraAry);
void HAL_DispParaUpsHsdDelayCfgSet(SINT16 *paraAry);
void HAL_DispParaUpsClkPhaseSet(SINT16 *paraAry);
void HAL_DispParaUpsLayoutModeSet(SINT16 *paraAry);
void HAL_DispParaUpsColorFilterCfgSet(SINT16 *paraAry);
void HAL_DispParaCcirOutputSequenceCfgSet(SINT16 *paraAry);
void HAL_DispRaraEdgeTypeSet(SINT16* paraAry);
void HAL_DispRaraEdgeGainSet(SINT16* paraAry);
void HAL_DispParaGammaEnableSet(SINT16 *paraAry);
void HAL_DispParaRBSwitchSet(SINT16 *paraAry);
void HAL_DispParaColorMatrixTblCfgSet(SINT16 *paraAry);
void HAL_DispParaColorMatrixEnableSet(SINT16 *paraAry);
void HAL_DispParaColorMatrixFmtCfg(SINT16 *paraAry);
void HAL_DispParaTvDacPowerEnableSet(SINT16 *paraAry);
void HAL_DispParaTvSaturationSet(SINT16 *paraAry);
void HAL_DispParaTvHueSet(SINT16 *paraAry);
void HAL_DispParaLcmCmdBusTypeSet(SINT16 *paraAry);
void HAL_DispParaLcmCmdBitWidthSet(SINT16 *paraAry);
void HAL_DispParaLcmCmdPulseWidthSet(SINT16 *paraAry);
void HAL_DispParaLcmCmdActiveLevelSet(SINT16 *paraAry);
void HAL_DispParaLcmDataFormatSet(SINT16 *paraAry);
void HAL_DispParaLcmRgb666PackEnableSet(SINT16 *paraAry);
void HAL_DispParaLcmRgb666Format10EnableSet(SINT16 *paraAry);
void HAL_DispParaLcmPixelAlignSet(SINT16 *paraAry);
void HAL_DispParaLcmColorPackageSet(SINT16 *paraAry);
void HAL_DispParaLcmSwitchGEnableSet(SINT16 *paraAry);
void HAL_DispParaLcmChangeRbEnableSet(SINT16 *paraAry);
void HAL_DispParaLcmOneHalfPixelEnableSet(SINT16 *paraAry);
void HAL_DispParaLcmDitherEnableSet(SINT16 *paraAry);

#endif  /* __HAL_DISP_INTERNAL_H__ */

