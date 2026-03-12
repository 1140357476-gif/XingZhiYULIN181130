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
 * @file      front_logi.c
 * @brief     front logical layer
 * @author    Matt Wang
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_front.h"
#include "front_logi.h"
#include "hal_cdsp.h"
#include "hal_cdsp_iq.h" 
#include "reg_def.h"
#include "cdsp_init.h"
#include "hal_front_para.h"
#include "sp1k_gpio_api.h"
#include "hal_front_common.h"
#include "cdsp_task.h"
#include "front_debug.h"
#include "ae_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_LIB_FORNT_LOGIC__

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
		
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
/* device management. */
UINT8 frontDevTot = 0;
UINT8 frontDevAct = 0;
frontDev_t frontDev[FRONT_DEVICE_TOT];

/* global variable as fake local variable. */
static UINT8 ret; 
static UINT8 snapMethod;

UINT8 frontPrevSensorZoomStatus;
UINT8 frontPrevSensorZoomEn;
UINT8 frontPrevSensorZoom2xGainEn;
UINT8 frontPrevSensorZoomSnapStatus;



/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 PreGain;
extern UINT32 PreShutter;
extern UINT8 G_ExpGainSetPos;
extern UINT8 G_GainAfterExp;
extern UINT8 g_AE_Stable;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT8 frontDevOpen(c_t *popFunc, UINT16 opFuncSize)
 * @brief     open device (register operating function)
 * @param     [in] popFunc : operating function table
 * @param     [in] opFuncSize : function table size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontDevOpen(
	frontOperatingFunc_t *popFunc,
	UINT16 opFuncSize
)
{
	frontDev_t *pdev;


	/* clear at first time. */
	if ( frontDevTot == 0 ) {
		/* clear allocated function table*/
		memset(frontDev, 0, sizeof(frontDev_t) * FRONT_DEVICE_TOT);
	}

	/* check for free room. */
	if ( frontDevTot == FRONT_DEVICE_TOT ) {
		return NULL;  /* no room. */
	}

	/* hook operating functions. */
	pdev = &frontDev[frontDevTot];
	memcpy(&pdev->opFunc, (void *)popFunc, opFuncSize);

	frontDevTot++;

	frontDevOpenPrintf("%s open devTot=%bd\n", (*pdev->opFunc.pfOpDevNameGet)(), frontDevTot);

	return frontDevTot;  /* device id. */
}

/**
 * @fn        UINT8 frontExtendOpFuncSet(UINT8 id, UINT8 opId, c_t pfExtOp)
 * @brief     set (register) extended operating function
 * @param     [in] id : device id (1~FRONT_DEVICE_TOT)
 * @param     [in] opId : extended operating function id [ 0~(FRONT_EXT_OP_TOT-1)]
 * @param     [in] pfExtOp : extended operating function pointer
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontExtendOpFuncSet(
	UINT8 id,
	UINT8 opId,
	pfFrontExtOperatingFunc_t pfExtOp
)
{
	frontDev_t *pdev;


	/* check id validation. */
	if ( id == 0 || id > frontDevTot || opId >= FRONT_EXT_OP_TOT ) {
		return FAIL;
	}
	id--;
	pdev = &frontDev[id];

	/* hook desired extended operating function. */
	pdev->extOpFunc[opId] = pfExtOp;

	frontExtendOpFuncSetPrintf("%s ext op id=%bd opId=%bd op=0x%08X\n", (*pdev->opFunc.pfOpDevNameGet)(), id+1, opId, pfExtOp);

	return SUCCESS;
}

/**
 * @fn        UINT8 frontDevActSet(UINT8 id)
 * @brief     set current actived device
 * @param     [in] id : device id (1~FRONT_DEVICE_TOT)
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontDevActSet(
	UINT8 id
)
{
	/* check id validation. */
	if ( id == 0 || id > frontDevTot ) {
		frontDevActSetPrintf("frontDevActSet FAIL\n");
		return FAIL;
	}
	else {
		frontDevAct = id;
		frontDevActSetPrintf("frontDevActSet %d\n", (UINT16)frontDevAct);
		return SUCCESS;
	}
}

/**
 * @fn        UINT8 frontDevActGet(void)
 * @brief     get current actived device
 * @param     NONE
 * @retval    return = current actived device id, 1~FRONT_DEVICE_TOT
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontDevActGet(
	void
)
{
	frontDevActGetPrintf("frontDevActGet %d\n", (UINT16)frontDevAct);

	return frontDevAct;
}

/**
 * @fn        UINT8 frontIntrDevActGet(void)
 * @brief     intr get current actived device
 * @param     NONE
 * @retval    return = current actived device id, 1~FRONT_DEVICE_TOT
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontIntrDevActGet(
	void
)
{
	frontDevActGetPrintf("frontDevActGet %d\n", (UINT16)frontDevAct);

	return frontDevAct;
}


/**
 * @fn        UINT8 frontDevIdCheck(UINT8 *pid, frontDev_t **ppdev)
 * @brief     get front id and frontDev_t point
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontDevIdCheck(
	UINT8 *pid,
	frontDev_t **ppdev
)
{
	frontDevIdCheckPrintf("pid = %bd, ppdev = %d\n", *pid, *ppdev);

	if ( frontDevAct == 0 || *pid > frontDevTot ) {
		return FAIL;
	}
	if ( *pid == 0 ) {
		*pid = frontDevAct - 1;
	}
	else {
		*pid = *pid - 1;
	}
	*ppdev = &frontDev[*pid];
	return SUCCESS;
}

/**
 * @fn        UINT8 frontIntrDevIdCheck(UINT8 *pid, frontDev_t **ppdev)
 * @brief     get front id and frontDev_t point
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontIntrDevIdCheck(
	UINT8 *pid,
	frontDev_t **ppdev
)
{
	frontIntrDevIdCheckPrintf("pid = %bd, ppdev = %d\n", *pid, *ppdev);

	if ( frontDevAct == 0 || *pid > frontDevTot ) {
		return FAIL;
	}
	if ( *pid == 0 ) {
		*pid = frontDevAct - 1;
	}
	else {
		*pid = *pid - 1;
	}
	*ppdev = &frontDev[*pid];
	return SUCCESS;
}

/**
 * @fn        UINT8 frontFrameRateSet(UINT8 id, UINT8 fps)
 * @brief     set frame rate, called before further operating
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] fps : frame rate, 30 or lower
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontFrameRateSet(
	UINT8 id,
	UINT8 fps
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function FrameRateSet(). */
	(*pdev->opFunc.pfOpFrameRateSet)(fps);

	frontFrameRateSetPrintf("%s fps f=%bd\n", (*pdev->opFunc.pfOpDevNameGet)(), fps);

	return SUCCESS;
}

/**
 * @fn        UINT8 frontCapabilityGet(UINT8 id, tPrevCapabDesc_t **ppprevCap, apCapabDesc_t **ppsnapCap)
 * @brief     get device capability
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] ppPrevCap : pointer to device preview capability
 * @param     [in] ppSnapCap : pointer to device snap capability
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontCapabilityGet(
	UINT8 id,
	frontPrevCapabDesc_t **ppPrevCap,
	frontSnapCapabDesc_t **ppSnapCap
)
{
	frontDev_t *pdev;
	frontCapabilityArg_t arg;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function CapabilityGet(). */
	arg.ppprevCap = ppPrevCap;
	arg.ppsnapCap = ppSnapCap;
	
	(*pdev->opFunc.pfOpCapabilityGet)(&arg);

	frontCapabilityGetPrintf("%s capa", (*pdev->opFunc.pfOpDevNameGet)());
	if ( *ppPrevCap ) {
		frontCapabilityGetPrintf(" pw=%d ph=%d", (*ppPrevCap)->width, (*ppPrevCap)->height);
	}
	if ( *ppSnapCap ) {
		frontCapabilityGetPrintf(" sw=%d sh=%d", (*ppSnapCap)->width, (*ppSnapCap)->height);
	}
	frontCapabilityGetPrintf("\n");

	return SUCCESS;
}

/**
 * @fn        UINT8 frontIntrCapabilityGet(UINT8 id, tPrevCapabDesc_t **ppprevCap, apCapabDesc_t **ppsnapCap)
 * @brief     get device capability
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] ppPrevCap : pointer to device preview capability
 * @param     [in] ppSnapCap : pointer to device snap capability
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontIntrCapabilityGet(
	UINT8 id,
	frontPrevCapabDesc_t **ppPrevCap,
	frontSnapCapabDesc_t **ppSnapCap
)
{
	frontDev_t *pdev;
	frontCapabilityArg_t arg;


	/* check id validation. */
	frontIntrDevIdCheck(&id, &pdev);

	/* run physical device operating function CapabilityGet(). */
	arg.ppprevCap = ppPrevCap;
	arg.ppsnapCap = ppSnapCap;
	(*pdev->opFunc.pfOpIntrCapabilityGet)(&arg);

	frontIntrCapabilityGetPrintf("%s capa", (*pdev->opFunc.pfOpDevNameGet)());
	if ( *ppPrevCap ) {
		frontIntrCapabilityGetPrintf(" pw=%d ph=%d", (*ppPrevCap)->width, (*ppPrevCap)->height);
	}
	if ( *ppSnapCap ) {
		frontIntrCapabilityGetPrintf(" sw=%d sh=%d", (*ppSnapCap)->width, (*ppSnapCap)->height);
	}
	frontIntrCapabilityGetPrintf("\n");

	return SUCCESS;
}

/**
 * @fn        UINT8 frontDeviceInfoGet(UINT8 id, UINT8 infId, UINT16 *ppara1, UINT16 *ppara2)
 * @brief     get device information
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] infId : information id, one of frontDeviceInfo_e
 * @param     [in] ppara1 : return parameter #1, depends on frontDeviceInfo_e
 * @param     [in] ppara2 : return parameter #2, depends on frontDeviceInfo_e
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/

UINT8
frontDeviceInfoGet(
	UINT8 id,
	UINT8 infId,
	UINT16 *ppara1,
	UINT16 *ppara2
)
{
	frontDev_t *pdev;
	frontCapabilityArg_t arg;
	frontPrevCapabDesc_t *pcap;
	frontSnapCapabDesc_t *psnap;
	UINT16 para1, para2;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function CapabilityGet(). */
	arg.ppprevCap = &pcap;
	arg.ppsnapCap = &psnap;
	(*pdev->opFunc.pfOpCapabilityGet)(&arg);

	para1 = 0;
	para2 = 0;
	
	switch ( infId ) {
		case FRONT_DEV_INFO_DATA_FMT:
			if ( (pdev->opFunc).expGainSetPos == AE_EXP_GAIN_NOT_ACTION_FOR_YUV ) {
				para1 = FRONT_DATA_FMT_YUV;
			}
			else {
				para1 = FRONT_DATA_FMT_RAW;
			}

			break;
		case FRONT_DEV_INFO_BAYER_FMT:
			para1 = (UINT16)(pcap->pvBayerPtn);
			para2 = (UINT16)(psnap->snapBayerPtn);
			break;
		case FRONT_DEV_INFO_PREVIEW_LINE_TOL:	//fan.xiao add for pv ob cal
			para1 = pcap->lineTot;
			break;
		case FRONT_DEV_INFO_PREVIEW_PCLK:	
			para1 = pcap->pclkFreq &  0xffff;
			para2 = pcap->pclkFreq >> 16;
			break;
		case FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR:
			para1 = (UINT16)(pcap->sensorZoomFactor);
			break;
	}

	if ( ppara1 != NULL ) {
		*ppara1 = para1;
	}
	if ( ppara2 != NULL ) {
		*ppara2 = para2;
	}

	frontDeviceInfoGetPrintf("%s dev info i=%bd p1=%d p2=%d\n", (*pdev->opFunc.pfOpDevNameGet)(), infId, para1, para2);

	return SUCCESS;
}


/**
 * @fn        UINT8 frontPowerOn(UINT8 id)
 * @brief     power on device
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontPowerOn(
	UINT8 id
)
{
	frontDev_t *pdev;

	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* reset front interface. */
	HAL_FrontReset();

	/* run physical device operating function PowerOn(). */
	(*(pdev->opFunc).pfOpPowerOn)();

	/* enable ae/awb. */
	G_ExpGainSetPos = (pdev->opFunc).expGainSetPos;
	G_GainAfterExp = (pdev->opFunc).gainAfterExp;

	/* if yuv sensor, no ae function. */
	if ( G_ExpGainSetPos == AE_EXP_GAIN_NOT_ACTION_FOR_YUV ) {  
		g_AE_Stable = 1;
	}

	frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

	frontPowerOnPrintf("%s power on\n", (*pdev->opFunc.pfOpDevNameGet)());
	
	//printf("front power on\n");
	return SUCCESS;
}


/**
 * @fn        UINT8 frontPowerOff(UINT8 id)
 * @brief     power off device
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontPowerOff(
	UINT8 id
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function PowerOff(). */
	(*(pdev->opFunc).pfOpPowerOff)();

	frontPowerOffPrintf("%s power off\n", (*pdev->opFunc.pfOpDevNameGet)());

	return SUCCESS;
}

/**
 * @fn        UINT8 frontPreviewModeSet(UINT8 id)
 * @brief     set preview mode
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontPreviewModeSet(
	UINT8 id
)
{
	frontDev_t *pdev;
	UINT8 f2cg, EoltoSolPixelCnt;
	frontCapabilityArg_t arg;
	frontPrevCapabDesc_t *pcap;
	UINT16 WSize;
	UINT16 hBlank;

	#if (FRONT_PROBE_MODE_SET_DBG)
	frontProbeModeSet(0x09, 0x12, 0xff);	/* probe2 sel sensor single, probe mode, probe2 enable */
	#endif

	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function CapabilityGet(). */
	arg.ppprevCap = &pcap;
	arg.ppsnapCap = NULL;
	(*pdev->opFunc.pfOpCapabilityGet)(&arg);

	/* set sram enable */
	if ( (pcap->sramModeEn == FRONT_SRAM_MODE_ENABLE) && 
		(G_ExpGainSetPos != AE_EXP_GAIN_NOT_ACTION_FOR_YUV) && 
		(XBYTE[REG_Front_Ys128en] & 0x10) != 0x10 ) {	//check yuv input enable	
		//XBYTE[REG_Cdsp_FrontSramen] = XBYTE[REG_Cdsp_FrontSramen] | 0x13;			//sram enable	
		
		/* set sram param */
		WSize = (UINT16)(pcap->width);	
		WSize = WSize >> 1;
		XBYTE[REG_Cdsp_FrontFbufsize_low] = (UINT8)(WSize & 0x00ff);			//fbufsize lo
		XBYTE[REG_Cdsp_FrontFbufsize_high] = (UINT8)(( WSize & 0xff00 ) >> 8);	//fbufsize hi
		XBYTE[REG_Cdsp_FrontSramen] = XBYTE[REG_Cdsp_FrontSramen] | 0x17;			//sram enable/3d denoise 	
		//frontSysInfo("SRAM enable\n");
	}
	else {
		/* set sram disable */
		XBYTE[REG_Cdsp_FrontSramen] = XBYTE[REG_Cdsp_FrontSramen] & (~0x13);		//sram disable	
		//frontSysInfo("SRAM disable\n");
	}

	/* set Eol to Sol pixel count */
	hBlank = (UINT16)(pcap->lineTot) - (UINT16)(pcap->width);
	if ((hBlank>>1) > 255) {
		EoltoSolPixelCnt = 0xFF;
	}
	else {
		EoltoSolPixelCnt = (hBlank>>1);
	}
	XBYTE[REG_Cdsp_FrontFintval] = EoltoSolPixelCnt;

	/* run physical device operating function PreviewModeSet(). */
	f2cg = XBYTE[REG_Front_F2CdspGate];  /* save old setting. */
	XBYTE[REG_Front_F2CdspGate] = 0x11;
	(*(pdev->opFunc).pfOpPreviewModeSet)();
	XBYTE[REG_Front_F2CdspGate] = f2cg;  /* restore old setting. */
	
	//frontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 0, 0, 0, 0, 1);

	PreGain = 0xFF;
 	PreShutter = 0xFFFF;

	frontPreviewModeSetPrintf("%s prev set\n", (*pdev->opFunc.pfOpDevNameGet)());

	return SUCCESS;
}

/**
 * @fn        UINT8 frontSensorZoomModeSet(UINT8 id, UINT16 factor)
 * @brief     set Sensor Zoom mode
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] factor : zoom factor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontPreviewSensorZoomModeSet(
	UINT8 id,
	UINT16 factor
)
{
	frontDev_t *pdev;
	frontCapabilityArg_t arg;
	frontPrevCapabDesc_t *pcap;

	/* check HQ Dzoom enable */
	if (frontPrevSensorZoomEn == 0) { 
		return FAIL;
	}

	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function CapabilityGet(). */
	arg.ppprevCap = &pcap;
	arg.ppsnapCap = NULL;
	(*pdev->opFunc.pfOpCapabilityGet)(&arg);

	//printf("s factor=%d\n",factor);
	(*(pdev->opFunc).pfOpPreviewSensorZoomModeSet)(factor);

	frontPreviewModeSetPrintf("%s prev set\n", (*pdev->opFunc.pfOpDevNameGet)());

	return SUCCESS;
}

/**
 * @fn        UINT8 frontIntrPreviewSensorZoomModeSet(UINT8 id, UINT16 factor)
 * @brief     intr set Sensor Zoom mode
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] factor : zoom factor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontIntrPreviewSensorZoomModeSet(
	UINT8 id,
	UINT16 factor
)
{
	frontDev_t *pdev;
	frontCapabilityArg_t arg;
	frontPrevCapabDesc_t *pcap;

	/* check HQ Dzoom enable */
	if (frontPrevSensorZoomEn == 0) { 
		return FAIL;
	}

	/* check id validation. */
	frontIntrDevIdCheck(&id, &pdev);

	/* run physical device operating function CapabilityGet(). */
	arg.ppprevCap = &pcap;
	arg.ppsnapCap = NULL;
	(*pdev->opFunc.pfOpIntrCapabilityGet)(&arg);

	//printf("s factor=%d\n",factor);
	(*(pdev->opFunc).pfOpIntrPreviewSensorZoomModeSet)(factor);

	frontPreviewModeSetPrintf("%s prev set\n", (*pdev->opFunc.pfOpDevNameGet)());

	return SUCCESS;
}

/**
 * @fn        UINT8 frontSnapModeSet(UINT8 id)
 * @brief     set snap mode
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontSnapModeSet(
	UINT8 id
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* set sram disable */
	XBYTE[REG_Cdsp_FrontSramen] = XBYTE[REG_Cdsp_FrontSramen] & (~0x13);		//sram disable	

	/* set Eol to Sol pixel count */
	XBYTE[REG_Cdsp_FrontFintval] = 0x20;

	/* reset cdsp for snap crash */
	HAL_CdspRstBlock(CDSPRstCDSP);
	//XBYTE[REG_Cdsp_rstcdsp]=0x01;
	//XBYTE[REG_Cdsp_rstcdsp]=0x00;

	/* run physical device operating function SnapModeSet(). */
	(*(pdev->opFunc).pfOpSnapModeSet)();

	PreGain = 0;
 	PreShutter = 0;

	frontSnapModeSetPrintf("%s snap set\n", (*pdev->opFunc.pfOpDevNameGet)());

	return SUCCESS;
}


/**
 * @fn        UINT8 frontSnapShot(UINT8 id, UINT8 burst, UINT8 first)
 * @brief     snap shot
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] burst : 0: not burst shot, 1: burst shot
 * @param     [in] first : 0: not first, 1: first shot for burst shot
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontSnapShot(
	UINT8 id,
	UINT8 burst,
	UINT8 first,
	UINT8 mode,
	UINT8 scaleUp
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function SnapShot(). */
	(*(pdev->opFunc).pfOpSnapShot)(first, mode,scaleUp);
	
	if ( *pdev->extOpFunc[FRONT_EXT_OP_AFTER_SNAP_SHOT] != NULL ) {
		(*pdev->extOpFunc[FRONT_EXT_OP_AFTER_SNAP_SHOT])((void *)burst);
	}

	frontSnapShotPrintf("%s shot f=%bd\n", (*pdev->opFunc.pfOpDevNameGet)(), first);

	return SUCCESS;
}


/**
 * @fn        UINT8 frontGainSet(UINT8 id, UINT8 val, UINT8 isr, UINT8 upd)
 * @brief     set gain
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] val : gain value
 * @param     [in] isr : called by 0: regular function, 1: isr
 * @param     [in] upd : 0: immediately update, 1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontGainSet(
	UINT8 id,
	UINT8 val,
	UINT8 isr,
	UINT8 upd
)
{
	static UINT8 inUse = 0;
	UINT8 sr;
	frontDev_t *pdev;


	while ( inUse ) {
	}
	inUse = 1;

	if ( !isr ) {
		ENTER_CRITICAL(sr);
	}

	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function GainSet(). */
	(*(pdev->opFunc).pfOpGainSet)(val, 0/*isr*/, upd);

	frontGainSetPrintf("%s gain v=%bd i=%bd u=%bd\n", (*pdev->opFunc.pfOpDevNameGet)(), val, isr, upd);

	if ( !isr ) {
		EXIT_CRITICAL(sr);
	}

	inUse = 0;

	return SUCCESS;
}

/**
 * @fn        UINT8 frontExposureTimeSet(UINT8 id, UINT32 val, UINT8 isr, UINT8 upd)
 * @brief     set exposure time
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] val : gain value
 * @param     [in] isr : called by 0: regular function, 1: isr
 * @param     [in] upd : 0: immediately update, 1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontExposureTimeSet(
	UINT8 id,
	UINT32 val,
	UINT8 isr,
	UINT8 upd
)
{
	static UINT8 inUse = 0;
	UINT8 sr;
	frontDev_t *pdev;
	frontExposureTimeArg_t arg;


	while ( inUse ) {
	}
	inUse = 1;

	if ( !isr ) {
		ENTER_CRITICAL(sr);
	}

	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function ExposureTimeSet(). */
	arg.val = val;
	arg.isr = 0/*isr*/;
	arg.upd = upd;
	(*(pdev->opFunc).pfOpExposureTimeSet)(&arg);

	frontExposureTimeSetPrintf("%s exp time v=%ld i=%bd u=%bd\n", (*pdev->opFunc.pfOpDevNameGet)(), val, isr, upd);

	if ( !isr ) {
		EXIT_CRITICAL(sr);
	}

	inUse = 0;

	return SUCCESS;
}

/**
 * @fn        UINT8 frontIntrGainSet(UINT8 id, UINT8 val, UINT8 isr, UINT8 upd)
 * @brief     set gain
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] val : gain value
 * @param     [in] isr : called by 0: regular function, 1: isr
 * @param     [in] upd : 0: immediately update, 1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontIntrGainSet(
	UINT8 id,
	UINT8 val,
	UINT8 isr,
	UINT8 upd
)
{
	static UINT8 inUse = 0;
	UINT8 sr;
	frontDev_t *pdev;


	while ( inUse ) {
	}
	inUse = 1;

	if ( !isr ) {
		ENTER_CRITICAL(sr);
	}

	/* check id validation. */
	frontIntrDevIdCheck(&id, &pdev);

	/* run physical device operating function GainSet(). */
	(*(pdev->opFunc).pfOpIntrGainSet)(val, 0/*isr*/, upd);

	frontIntrGainSetPrintf("%s gain v=%bd i=%bd u=%bd\n", (*pdev->opFunc.pfOpDevNameGet)(), val, isr, upd);

	if ( !isr ) {
		EXIT_CRITICAL(sr);
	}

	inUse = 0;

	return SUCCESS;
}

/**
 * @fn        UINT8 frontIntrExposureTimeSet(UINT8 id, UINT32 val, UINT8 isr, UINT8 upd)
 * @brief     set exposure time
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] val : gain value
 * @param     [in] isr : called by 0: regular function, 1: isr
 * @param     [in] upd : 0: immediately update, 1: sync to vsync falling
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontIntrExposureTimeSet(
	UINT8 id,
	UINT32 val,
	UINT8 isr,
	UINT8 upd
)
{
	static UINT8 inUse = 0;
	UINT8 sr;
	frontDev_t *pdev;
	frontExposureTimeArg_t arg;


	while ( inUse ) {
	}
	inUse = 1;

	if ( !isr ) {
		ENTER_CRITICAL(sr);
	}

	/* check id validation. */
	frontIntrDevIdCheck(&id, &pdev);

	/* run physical device operating function ExposureTimeSet(). */
	arg.val = val;
	arg.isr = 0/*isr*/;
	arg.upd = upd;
	(*(pdev->opFunc).pfOpIntrExposureTimeSet)(&arg);

	frontIntrExposureTimeSetPrintf("%s exp time v=%ld i=%bd u=%bd\n", (*pdev->opFunc.pfOpDevNameGet)(), val, isr, upd);

	if ( !isr ) {
		EXIT_CRITICAL(sr);
	}

	inUse = 0;

	return SUCCESS;
}


/**
 * @fn        UINT8 frontOpticalBlackStatusGet(UINT8 id)
 * @brief     get optical black status
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontOpticalBlackStatusGet(
	UINT8 id
)
{
	frontDev_t *pdev;
	UINT8 ret;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function OpticalBlackStatusGet(). */
	ret = (*(pdev->opFunc).pfOpOpticalBlackStatusGet)();

	frontOpticalBlackStatusGetPrintf("%s ob sts\n", (*pdev->opFunc.pfOpDevNameGet)());

	return ret;
}

/**
 * @fn        UINT8 frontOpticalBlackCalibrate(UINT8 id)
 * @brief     calibrate optical black
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontOpticalBlackCalibrate(
	UINT8 id
)
{
	frontDev_t *pdev;
	UINT8 ret;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function OpticalBlackCalibrate(). */
	ret = (*(pdev->opFunc).pfOpOpticalBlackCalibrate)();

	frontOpticalBlackCalibratePrintf("%s ob cal on\n", (*pdev->opFunc.pfOpDevNameGet)());

	return ret;
}


/**
 * @fn        UINT8 frontShutterOpen(UINT8 id)
 * @brief     open shutter
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontShutterOpen(
	UINT8 id
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function ShutterOpen(). */
	(*(pdev->opFunc).pfOpShutterOpen)();

	frontShutterOpenPrintf("%s sht open\n", (*pdev->opFunc.pfOpDevNameGet)());

	return SUCCESS;
}


/**
 * @fn        UINT8 frontShutterClose(UINT8 id)
 * @brief     close shutter
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontShutterClose(
	UINT8 id
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function ShutterClose(). */
	(*(pdev->opFunc).pfOpShutterClose)();

	frontShutterClosePrintf("%s sht close\n", (*pdev->opFunc.pfOpDevNameGet)());

	return SUCCESS;
}

/**
 * @fn        UINT8 frontIntrPreviewDynamicTempoalDenoiseSet(UINT8 id)
 * @brief     preivew dynamic tempoal denoise
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
frontIntrPreviewDynamicTempoalDenoiseSet(
	UINT8 id,
	UINT8 en
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontIntrDevIdCheck(&id, &pdev);

	/* run physical device operating function IntrPreviewDynamicTempoalDenoiseSet(). */
	(*(pdev->opFunc).pfOpIntrPreviewDynamicTempoalDenoiseSet)(en);

	PreGain = 0xFF;
	PreShutter = 0;

	frontShutterClosePrintf("%s pv dy TD\n", (*pdev->opFunc.pfOpDevNameGet)());

	return SUCCESS;
}

/**
 * @fn        UINT8 frontLensCompensationSet(UINT8 id, UINT8 mode)
 * @brief     set lens compensation
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] mode : 0: preview, 1: snap lens compensation
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontLensCompensationSet(
	UINT8 mode
)
{
	
	frontDev_t *pdev;
	frontCapabilityArg_t arg;
	frontPrevCapabDesc_t *pcap1;
	frontSnapCapabDesc_t *pcap2;
	UINT16 pvHSize,pvVSize;
	UINT16 snapHSize,snapVSize;
 	UINT16 hOffset,vOffset;
	UINT8 id;
	//UINT8 hStep,vStep;
	
	/* apply lens compensation. */
	if ( mode == 0 ) {  /* preview. */
		//HAL_LensCompSizeSet(pprevCap->lcXmoff, pprevCap->lcXminc, pprevCap->lcYmoff, pprevCap->lcYminc);			   //will mark
		//HAL_LensCompCfgEnable(pprevCap->lcStepFact1, pprevCap->lcStepFact2, pprevCap->lcPathMode, pprevCap->lcRgbCh);//will mark
		//HAL_CdspLscModeSet(1);/* 4 channel mode */
        //HAL_CdspLscEnSet(0);  /* disable */

		#if 0
		HAL_FrontSizeGet(&pvHSize,&pvVSize);
		frontSnapSizeGet(0,&snapHSize,&snapVSize);
		#else
		/*front Dev Act Get*/
		id = frontDevActGet();
	
		/* check id validation. */
		frontDevIdCheck(&id, &pdev);
	
		/* run physical device operating function CapabilityGet(). */
		arg.ppprevCap = &pcap1;
		arg.ppsnapCap = &pcap2;
		(*pdev->opFunc.pfOpCapabilityGet)(&arg);
	
		pvHSize = (UINT16)pcap1->width;
		pvVSize = (UINT16)pcap1->height;
	
		snapHSize = (UINT16)pcap2->width;
		snapVSize = (UINT16)pcap2->height;
		#endif
		
		//HAL_CdspLscWinStepGet(&hStep,&vStep);

		//hStep += 4;	/* 0: 8x2=16 shift 4 bits , 1: 16x2=32 shift 5 bits , 2: 32x2=64 shift 6 bits */
		//vStep += 4;

		if ((snapHSize/pvHSize)<=1){
			HAL_CdspLscWinIncStepSet(1,1,1,1);  /* increasing step=1*/	
 	   		hOffset= (snapHSize-pvHSize);      //(0x2124) 
	   		vOffset= (snapVSize-pvVSize);     // (0x2126) 	
		}
		else if ((snapHSize/pvHSize)<=2){
			HAL_CdspLscWinIncStepSet(2,2,2,2);  				/* increasing step=2 for preview */
 			hOffset= ((snapHSize) - (pvHSize << 1));      //(0x2124) 
	   		vOffset= ((snapVSize) - (pvVSize << 1));     // (0x2126) 		   		
		}
		else {
			HAL_CdspLscWinIncStepSet(4,4,4,4);  				/* increasing step=2 for preview */
 			hOffset= (snapHSize - (pvHSize<<2));      //(0x2124) 
	   		vOffset= (snapVSize - (pvVSize<<2));     // (0x2126) 	
		}

		hOffset = hOffset >> 1 ;
		vOffset = vOffset >> 1 ;
		
 		HAL_CdspLscWinOffsetSet((UINT8)hOffset,(UINT8)vOffset);		
		//HAL_CdspLscWinIncStepSet(2,2,2,2);  				/* increasing step=1 for preview */
		//HAL_CdspLscWinIncStepSet(1,1,1,1);  /* increasing step=1 for capture */
	}
	else {  /* snap. */
		//HAL_LensCompSizeSet(psnapCap->lcXmoff, psnapCap->lcXminc, psnapCap->lcYmoff, psnapCap->lcYminc);			   //will mark
		//HAL_LensCompCfgEnable(psnapCap->lcStepFact1, psnapCap->lcStepFact2, psnapCap->lcPathMode, psnapCap->lcRgbCh);//will mark		
        //HAL_CdspLscModeSet(1);/* 4 channel mode */        
        //HAL_CdspLscEnSet(0);  /* disable */
		HAL_CdspLscWinIncStepSet(1,1,1,1);  				/* increasing step=1 for preview */
 		HAL_CdspLscWinOffsetSet(0,0);
	}

	frontLensCompensationSetPrintf("%s lens comp m=%bd\n", (*pdev->opFunc.pfOpDevNameGet)(), mode);
	
	return SUCCESS;
}


/**
 * @fn        UINT8 frontPreviewSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize)
 * @brief     get preview size
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] pHSize : preview horizontal size
 * @param     [in] pVSize : preview vertical size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontPreviewSizeGet(
	UINT8 id,
	UINT16 *pHSize,
	UINT16 *pVSize
)
{
	frontDev_t *pdev;
	frontCapabilityArg_t arg;
	frontPrevCapabDesc_t *pcap;

    
	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function CapabilityGet(). */
	arg.ppprevCap = &pcap;
	arg.ppsnapCap = NULL;
	(*pdev->opFunc.pfOpCapabilityGet)(&arg);

	*pHSize = pcap->width;
	*pVSize = pcap->height;

	frontPreviewSizeGetPrintf("%s psize get HSize=%d VSize=%d\n", (*pdev->opFunc.pfOpDevNameGet)(), *pHSize, *pVSize);

	return SUCCESS;
}

/**
 * @fn        UINT8 frontSnapSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize)
 * @brief     get snap size
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] pHSize : snap horizontal size
 * @param     [in] pVSize : snap vertical size
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontSnapSizeGet(
	UINT8 id,
	UINT16 *pHSize,
	UINT16 *pVSize
)
{
	frontDev_t *pdev;
	frontCapabilityArg_t arg;
	frontSnapCapabDesc_t *pcap;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	/* run physical device operating function CapabilityGet(). */
	arg.ppprevCap = NULL;
	arg.ppsnapCap = &pcap;
	(*pdev->opFunc.pfOpCapabilityGet)(&arg);

	*pHSize = pcap->width;
	*pVSize = pcap->height;

	frontSnapSizeGetPrintf("%s ssize get HSize=%d VSize=%d\n", (*pdev->opFunc.pfOpDevNameGet)(), *pHSize, *pVSize);

	return SUCCESS;
}


/**
 * @fn        UINT8 frontAeTargetLuminanceSet(UINT8 id, UINT8 val)
 * @brief     set ae target luminance
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] val : ae taget luminance
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontAeTargetLuminanceSet(
	UINT8 id,
	UINT8 val
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	if ( *pdev->extOpFunc[FRONT_EXT_OP_AE_TARGET_LUMINANCE_SET] == NULL ) {
		return FAIL;
	}

	(*pdev->extOpFunc[FRONT_EXT_OP_AE_TARGET_LUMINANCE_SET])((void *)val);

	return SUCCESS;
}

/**
 * @fn        UINT8 frontAeExposureCompensationSet(UINT8 id, UINT8 val)
 * @brief     set ae exposure compensation
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] val: compensation value
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontAeExposureCompensationSet(
	UINT8 id,
	UINT8 val
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	if ( *pdev->extOpFunc[FRONT_EXT_OP_AE_EXPOSURE_COMPENSATION_SET] == NULL ) {
		return FAIL;
	}

	(*pdev->extOpFunc[FRONT_EXT_OP_AE_EXPOSURE_COMPENSATION_SET])((void *)val);

	return SUCCESS;
}

/**
 * @fn        UINT8 frontAeFrameRateSet(UINT8 id, UINT16 val)
 * @brief     set ae frame rate
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] val : frame rate
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontAeFrameRateSet(
	UINT8 id,
	UINT16 val
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	if ( *pdev->extOpFunc[FRONT_EXT_OP_AE_FRAME_RATE_SET] == NULL ) {
		return FAIL;
	}

	(*pdev->extOpFunc[FRONT_EXT_OP_AE_FRAME_RATE_SET])((void *)val);

	return SUCCESS;
}


/**
 * @fn        UINT8 frontAwbModeSet(UINT8 id, UINT8 mode)
 * @brief     set awb mode
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] mode : awb mode
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/

UINT8
frontAwbModeSet(
	UINT8 id,
	UINT8 mode
)
{
	frontDev_t *pdev;


	/* check id validation. */
	frontDevIdCheck(&id, &pdev);

	if ( *pdev->extOpFunc[FRONT_EXT_OP_AWB_MODE_SET] == NULL ) {
		return FAIL;
	}

	(*pdev->extOpFunc[FRONT_EXT_OP_AWB_MODE_SET])((void *)mode);

	return SUCCESS;
}

/**
 * @fn        UINT8 frontProbeModeSet(UINT8 probsel, UINT8 probmode, UINT8 proben)
 * @brief     set probe mode
 * @param     [in] probsel : probe single selection, 1~FF
 * @param     [in] probmode : probe mode and interface selection
 * @param     [in] proben : probe mode switch
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Lin Jie
 * @since     2010-6-1
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontProbeModeSet(
	UINT8 probsel,
	UINT8 probmode,
	UINT8 proben
)
{
	#if (FRONT_PROBE_MODE_SET_DBG)
	frontProbeModeSetPrintf("probemode set\n");
	XBYTE[0x20E1] |= probsel;	//0x09;	//probe2 sel sensor single
	XBYTE[REG_Front_T_Probemode] |= probmode;	//0x12;	//probe mode
	XBYTE[0x20DF] |= proben;	//0xFF;	//probe2 enable
	#else
	probsel = probsel;
	probmode = probmode;
	proben = proben;
	#endif

	return SUCCESS;
}

/**
 * @fn        void frontSnapMethodSet(UINT8 method)
 * @brief     set front snap method mode
 * @param     [in] method : typedef enum frontSnapMode_e
 *\n							SNAP_MS_NORMAL,
 *\n							SNAP_OBCAL,
 *\n							SNAP_MSCAL_FRAME,
 *\n							SNAP_MSCAL_ROLLING,
 *\n							SNAP_MS_DARK,
 * @retval    NULL
 * @see       NULL
 * @author    will
 * @since     2010-6-1
 * @todo      N/A
 * @bug       N/A
*/
void
frontSnapMethodSet(
	UINT8 method
)
{
	snapMethod=method;
}

/**
 * @fn        UINT8 frontSnapMethodSet(void)
 * @brief     set front Snap Method
 * @param     N/A
 * @retval    return =  snap method mode
 *\n						typedef enum frontSnapMode_e
 *\n							SNAP_MS_NORMAL,
 *\n							SNAP_OBCAL,
 *\n							SNAP_MSCAL_FRAME,
 *\n							SNAP_MSCAL_ROLLING,
 *\n							SNAP_MS_DARK,
 * @see       NULL
 * @author    will
 * @since     2010-6-1
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontSnapMethodGet(
	void
)
{
	return snapMethod;
}	

/**
 * @fn        void frontDarkSubCtrl(UINT8 en,UINT8 mode,	UINT16 time)
 * @brief     set front Dark Sub Ctrl
 * @param     [in] en : Dark Sub en
 * @param     [in] mode : one of frontWaitMode_e.
 * @param     [in] time : time unit is 10 us
 * @retval    NULL
 * @see       NULL
 * @author    Will
 * @since     2010-6-1
 * @todo      N/A
 * @bug       N/A
*/
void 
frontDarkSubCtrl(
	UINT8 en,
	UINT8 mode,
	UINT16 time
)
{
	if(en==1){
		//frontSysInfo("dark on\n");
		HAL_CdspEofWait(1); 
		//XBYTE[REG_Cdsp_3Dtden] = 0x12;    /* Drak Enable */		
		HAL_CdspDarkEnSet(1);
		cdspPreProcessReload();
		frontSignalWait(FRONT_WAIT_VSYNC, mode, 1);							
		
		sp1kFrexSet(1); 
		HAL_Front10UsWait(time);
		
		sp1kFrexSet(0); 			
	}
	else{	
		//XBYTE[REG_Cdsp_3Dtden] = 0x10;    /* Drak disable */		
		HAL_CdspDarkEnSet(0);
	}
}

/**
 * @fn        UINT8 frontHighQualityDigitalZoomCapabilityGet(UINT8 id)
 * @brief     set High Quality Digital Zoom Capability Get
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontHighQualityDigitalZoomCapabilityGet(
	UINT8 id
)
{
	UINT16 para1;

	/* get switch factor */
	frontDeviceInfoGet(id, FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR, &para1, NULL);

	if (para1) {
		return SUCCESS;
	}
	else {
		return FAIL;
	}
}


/**
 * @fn        UINT8 frontHighQualityDigitalZoomModeSet(UINT8 id, UINT8 en)
 * @brief     High Quality Digital Zoom mode set
 * @param     [in] id : device id, 0: current active, or 1~FRONT_DEVICE_TOT
 * @param     [in] en : 0 disable, 1 enable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
frontHighQualityDigitalZoomModeSet(
	UINT8 id,
	UINT8 en
)
{
	UINT8 sts;

	/* check Capability */
	sts = frontHighQualityDigitalZoomCapabilityGet(id);

	if (sts == SUCCESS) {
		frontPrevSensorZoomEn = en;
		return SUCCESS;
	}
	else {
		frontPrevSensorZoomEn = 0;
		return FAIL;
	}

}

#if 0
/**
 * @fn        void frontCaptureIntervalNumSet(UINT8 en,UINT8 intervalNum,)
 * @brief     set front Capture Interval Number
 * @param     [in] en : Dark Sub en
 * @param     [in] mode : one of frontWaitMode_e.
 * @param     [in] time : time unit is 10 us
 * @retval    NULL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-6-1
 * @todo      N/A
 * @bug       N/A
*/
void 
frontCaptureIntervalNumSet(
	UINT8 en,
	UINT8 intervalNum,
)
{
}
#endif
void frontSnapRollingTimeGet(UINT16 *time);

