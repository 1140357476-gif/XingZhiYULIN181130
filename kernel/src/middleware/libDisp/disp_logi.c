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
 * @file		disp_logi.c
 * @brief		display logical layer.
 * @author		Matt Wang
 * @since		2008-05-26
 * @date		2008-09-17
 */

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "hal_disp.h"
#include "hal_dram.h"
#include "disp_api.h"
#include "hal.h"
#include "reg_disp.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

#if 0
#define dispLogPrintf printf
#else
#define dispLogPrintf /DBG_SLASH
#endif

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct dispDev_s {
	dispOperatingFunc_t opFunc;
} dispDev_t;

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#define DISP_DEV_ID_CHECK() \
	if ( dispDevAct == 0 || id > dispDevTot ) { \
		return FAIL; \
	} \
	if ( id == 0 ) { \
		id = dispDevAct; \
	} \
	pdev = &dispDev[id - 1]; \

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
/* device management. */
static UINT8 dispDevTot = 0;
static UINT8 dispDevAct = 0;
static dispDev_t dispDev[DISP_DEVICE_TOT];

/* global variable as fake local variable. */
static UINT8 ret;

/**
 * @brief	open device (register operating function).
 * @param	*popFunc = [in] operating function table.
 * @param	opFuncSize = [in] function table size.
 * @retval	return = NULL or device id (1~3).
 * @see
 * @author	Matt Wang
 * @since	2008-05-26
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispDevOpen(
	dispOperatingFunc_t *popFunc,
	UINT16 opFuncSize
)
{
	dispDev_t *pdev;


	/* clear at first time. */
	if ( dispDevTot == 0 ) {
		/* clear allocated function table*/
		memset(dispDev, 0, sizeof(dispDev_t) * DISP_DEVICE_TOT);
	}

	/* check for free room. */
	if ( dispDevTot == DISP_DEVICE_TOT ) {
		return NULL;  /* no room. */
	}

	/* hook operating functions. */
	pdev = &dispDev[dispDevTot];
	memcpy(&pdev->opFunc, (void *)popFunc, opFuncSize);
	dispLogPrintf("%s open\n", (*pdev->opFunc.pfOpDevNameGet)());
	dispDevTot++;

	return dispDevTot;  /* device id. */
}

/**
 * @brief	set current actived device.
 * @param	id = [in] device id, 1~3.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-05-26
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispDevActSet(
	UINT8 id
)
{
	/* check id validation. */
	if ( id == 0 || id > dispDevTot ) {
		dispLogPrintf("dispDevActSet FAIL\n");

		return FAIL;
	}

	dispDevAct = id;
	dispLogPrintf("dispDevActSet %d\n", (UINT16)dispDevAct);

	return SUCCESS;
}

/**
 * @brief	get current actived device.
 * @param	None.
 * @retval	return = current actived device id, 1~3.
 * @see
 * @author	Matt Wang
 * @since	2008-05-26
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispDevActGet(
	void
)
{
	dispLogPrintf("dispDevActGet %d\n", (UINT16)dispDevAct);

	return dispDevAct;
}

/**
 * @brief	get device capability.
 * @param	id = [in] device id, 1~3.
 * @param   *pcapabDesc = [out] device capability.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-05-29
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispCapabilityGet(
	UINT8 id,
	dispCapabDesc_t *pcapabDesc
)
{
	dispDev_t *pdev;


	/* check id validation. */
	DISP_DEV_ID_CHECK();

	(*pdev->opFunc.pfOpCapabilityGet)(pcapabDesc);
	dispLogPrintf("%s capability x=%d y=%d w=%d h=%d\n", (*pdev->opFunc.pfOpDevNameGet)(), pcapabDesc->xoff, pcapabDesc->yoff, pcapabDesc->width, pcapabDesc->height);

	return SUCCESS;
}

/**
 * @brief	power on device, only be called by dispPowerOn().
 * @param	id = [in] device id, 1~3.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-05-29
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispPowerOn(
	UINT8 id
)
{
	dispDev_t *pdev;


	/* check id validation. */
	DISP_DEV_ID_CHECK();

	/* enable global and clk. */
	HAL_DispParaSet(HAL_DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 0);
	HAL_DispParaSet(HAL_DISP_PARA_CLK_ENABLE, 1, 0, 0, 0, 0);

	/* set background color. */
	#if 1  /* normal flow. */
	HAL_DispParaSet(HAL_DISP_PARA_BG_COLOR, 0, 0, 0, 0, 0);  /* black. */
	#else  /* test only, in order of R, G, B. */
	HAL_DispParaSet(HAL_DISP_PARA_BG_COLOR, 0, 0, 255, 0, 0);  /* blue. */
	#endif

	/* run physical device operating function PowerOn(). */
	(*pdev->opFunc.pfOpPowerOn)();
	dispLogPrintf("%s power on\n", (*pdev->opFunc.pfOpDevNameGet)());
	if(id != 1 && id != 4){//Panel clock disabled while TV plug in
		XBYTE[REG_Disp_SwRst] &= ~0x10; //panel clock enable
	}
	/* img and osd scale update */
	HAL_DispScaleUpdate();
	HAL_DispOsdScaleUpdate();

	/* disable image and osd layers. */
	HAL_DispParaSet(HAL_DISP_PARA_IMG_LAYER_ENABLE, 0, 0, 0, 0, 0);
	HAL_DispParaSet(HAL_DISP_PARA_OSD_LAYER_ENABLE, 0, 0, 0, 0, 0);

	/* dram zf update and set the register of zf update done */
	HAL_DramZfUpdTrig(
		HAL_DRAM_ZF_UPD_LAYER_OSD |\
		HAL_DRAM_ZF_UPD_LAYER_PB /* HAL_DRAM_ZF_UPD_LAYER_PV */ \
	);

	return SUCCESS;
}

/**
 * @brief	power off device, only be called by dispPowerOff().
 * @param	id = [in] device id, 1~3.
 * @retval	return = SUCCESS or FAIL.
 * @see
 * @author	Matt Wang
 * @since	2008-05-29
 * @todo	N/A
 * @bug		N/A
*/
UINT8
dispPowerOff(
	UINT8 id
)
{
	dispDev_t *pdev;
	UINT8 i;


	/* check id validation. */
	DISP_DEV_ID_CHECK();

	/* set background color. */
	HAL_DispParaSet(HAL_DISP_PARA_BG_COLOR, 0, 0, 0, 0, 0);  /* black. */

	/* disable image and osd layers. */
	HAL_DispParaSet(HAL_DISP_PARA_IMG_LAYER_ENABLE, 0, 0, 0, 0, 0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_PB);
	HAL_DispParaSet(HAL_DISP_PARA_OSD_LAYER_ENABLE, 0, 0, 0, 0, 0);
	HAL_DramZfUpdTrig(HAL_DRAM_ZF_UPD_LAYER_OSD);

	/* make sure black showing enough time and updating completed. */
	for ( i = 4; i > 0; i-- ) {
		HAL_DispSignalWait(HAL_DISP_WAIT_VVALID, HAL_DISP_WAIT_RISING);
	}

	/* run physical device operating function PowerOff(). */
	(*pdev->opFunc.pfOpPowerOff)();
	dispLogPrintf("%s power off\n", (*pdev->opFunc.pfOpDevNameGet)());

	/* disable global and clk. */
	HAL_DispParaSet(HAL_DISP_PARA_GLOB_ENABLE, 0, 0, 0, 0, 0);
	HAL_DispParaSet(HAL_DISP_PARA_CLK_ENABLE, 0, 0, 0, 0, 0);

	return SUCCESS;
}

