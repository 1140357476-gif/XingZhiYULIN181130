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
 * @file      File name
 * @brief     Brief
 * @author    Author's name
 * @since     2010-01-01
 * @date      2010-01-01
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "zoom_logi.h" 
#include "zoom_debug.h" 
#include "video_zoom.h"
#include "pv_zoom.h"
#include "reg_def.h"

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
/* device management. */
UINT8 zoomDevTot = 0;
zoomDev_t zoomDev[ZOOM_DEVICE_TOT];
zoomStillCapabDesc_t stillCapabDesc;
zoomVideoCapabDesc_t videoCapabDesc;
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/**
 * @fn        UINT8 zoomDevInit(void)
 * @brief     open pv and video zoom device (register blocking and nonblocking operating function)
 * @param    N/A
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevInit(
	void
)
{
	UINT8 type,sts;

	for ( type = ZOOM_LOGI_BLOCKING_TYPE; type < ZOOM_LOGI_TYPE_MAX; type++ ) {
		sts = pvZoomDevOpen(type);

		if (sts == FAIL) {
			return FAIL;
		}
	}
	
	for ( type = ZOOM_LOGI_BLOCKING_TYPE; type < ZOOM_LOGI_TYPE_MAX; type++ ) {
		sts = videoZoomDevOpen(type);

		if (sts == FAIL) {
			return FAIL;
		}
	}

	return SUCCESS;
}

/**
 * @fn        UINT8 zoomDevOpen(zoomOperatingFunc_t *popFunc,UINT16 opFuncSize)
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
zoomDevOpen(
	zoomOperatingFunc_t *popFunc,
	UINT16 opFuncSize
)
{
	zoomDev_t *pdev;


	/* clear at first time. */
	if ( zoomDevTot == 0 ) {
		/* clear allocated function table*/
		memset(zoomDev, 0, sizeof(zoomDev_t) * ZOOM_DEVICE_TOT);
	}

	/* check for free room. */
	if ( zoomDevTot == ZOOM_DEVICE_TOT ) {
		return NULL;  /* no room. */
	}

	/* hook operating functions. */
	pdev = &zoomDev[zoomDevTot];
	memcpy(&pdev->opFunc, (void *)popFunc, opFuncSize);

	zoomDevTot++;
	
	zoomDevOpenPrintf("zoom open devTot=%bd\n", zoomDevTot);

	return zoomDevTot;  /* device id. */
}

/**
 * @fn        UINT8 zoomDevActSet(UINT8 mode,UINT8 type)
 * @brief     set current actived zoom mode
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [in] type : typedef enum ZOOM_LOGI_TYPE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevActSet(
	UINT8 mode,
	UINT8 type
)
{
	/* check mode validation. */
	switch ( mode ) {
		case ZOOM_LOGI_STILL_MODE:
			/* still */
			stillCapabDesc.zoomType = type;
			break;
		case ZOOM_LOGI_VIDEO_MODE:
			/* video */
			videoCapabDesc.zoomType = type;
			break;
	}

	if ( type == ZOOM_LOGI_NONBLOCKING_TYPE ) {
		/* sof intr enable for dzoom */
		XBYTE[REG_Dram_Cdsp1Eof_Inten]|=0x20;	
	}

	zoomDevActSetPrintf("zoomDevActSet %d\n", (UINT16)type);
	
	return SUCCESS;
}

/**
 * @fn        UINT8 zoomDevActGet(UINT8 mode,UINT8 *type)
 * @brief     get current actived zoom type
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [out] *type : typedef enum ZOOM_LOGI_TYPE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevActGet(
	UINT8 mode,
	UINT8 *type
)
{
	/* check mode validation. */
	switch ( mode ) {
		case ZOOM_LOGI_STILL_MODE:
			/* still */
			*type = stillCapabDesc.zoomType;
			break;
		case ZOOM_LOGI_VIDEO_MODE:
			/* video */
			*type = videoCapabDesc.zoomType;
			break;
	}

	zoomDevActGetPrintf("zoomDevActSet %d\n", (UINT16)*type);
	
	return SUCCESS;
}

/**
 * @fn        UINT8 zoomDevModeCheck(UINT8 *pmode,UINT8 *type,zoomDev_t **ppdev)
 * @brief     get zoom mode and zoomDev_t point
 * @param     [in] *mode : typedef enum ZOOM_LOGI_MODE
 * @param     [out] *type : typedef enum ZOOM_LOGI_TYPE
 * @param     [out] **ppdev : zoomOperatingFunc_t
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2010-5-24
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevModeCheck(
	UINT8 *pmode,
	UINT8 *type,
	zoomDev_t **ppdev
)
{
	UINT8 offSet;
		
	zoomDevModeCheckPrintf("pmode = %bd, ppdev = %d\n", *pmode, *ppdev);

	if ( *pmode > zoomDevTot ) {
		return FAIL;
	}

	switch ( *pmode ) {
		case ZOOM_LOGI_STILL_MODE:
			offSet = DEV_STILL_ZOOM_OFFSET + *type;
			break;
		case ZOOM_LOGI_VIDEO_MODE:
			offSet = DEV_VIDEO_ZOOM_OFFSET + *type;
			break;
	}
	
	*ppdev = &zoomDev[offSet];
	
	return SUCCESS;
}


/**
 * @fn        UINT8 zoomDevResourceLoad(UINT8 mode)
 * @brief     get current actived zoom type
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [in] para1 : parameter 1
 * @param     [in] para2 : parameter 2
 * @param     [in] para3 : parameter 3
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevResourceLoad(
	UINT8 mode,
	UINT32 para1,
	UINT32 para2,
	UINT32 para3
)
{
	UINT8 type;
	UINT32 paraArray[3];
	zoomDev_t *pdev;

	paraArray[0] = para1;
	paraArray[1] = para2;
	paraArray[2] = para3;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomResourceLoad == NULL ) {
		return FAIL;
	}
	
	/* run physical zoom operating function ZoomResourceLoad(). */
	(*pdev->opFunc.pfOpZoomResourceLoad)(paraArray);//para1,para2,para3);

	zoomDevResourceLoadPrintf("loadres m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomSet(UINT8 mode,UINT16 para1,UINT16 para2,UINT16 para3)
 * @brief     set current actived zoom
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [in] para1 : parameter 1
 * @param     [in] para2 : parameter 2
 * @param     [in] para3 : parameter 3
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomSet(
	UINT8 mode,
	UINT8 para1, 
	UINT16 para2, 
	UINT8 para3
)
{
	UINT8 type;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomSet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomSet(). */
	(*pdev->opFunc.pfOpZoomSet)(para1,para2,para3);

	zoomDevZoomSetPrintf("zoomset m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomCtrlParaSet(UINT8 mode,UINT16 para1,UINT16 para2)
 * @brief     set current Zoom Ctrl Parameter
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [in] para1 : parameter 1
 * @param     [in] para2 : parameter 2
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomCtrlParaSet(
	UINT8 mode,
	UINT8 para1,
	UINT16 para2
)
{
	UINT8 type;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomCtrlParaSet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomCtrlParaSet(). */
	(*pdev->opFunc.pfOpZoomCtrlParaSet)(para1,para2);

	zoomDevZoomCtrlParaSetPrintf("ZoomCtrlParaSet m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomCtrlParaGet(UINT8 mode,UINT16 para1,UINT16 *outpara1)
 * @brief     Get current Zoom Ctrl Parameter
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [in] para1 : parameter 1
 * @param     [out] *outpara1 : out parameter 1
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomCtrlParaGet(
	UINT8 mode,
	UINT8 para1,
	UINT16 *outpara1
)
{
	UINT8 type;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomCtrlParaGet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomCtrlParaGet(). */
	*outpara1 = (*pdev->opFunc.pfOpZoomCtrlParaGet)(para1);

	zoomDevZoomCtrlParaGetPrintf("ZoomCtrlParaGet m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomFactorGet(UINT8 mode,UINT16 *outpara1)
 * @brief     Get current Zoom factor
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [out] *outpara1 : Zoom factor
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomFactorGet(
	UINT8 mode,
	UINT16 *outpara1
)
{
	UINT8 type;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomFactorGet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomFactorGet(). */
	*outpara1 = (*pdev->opFunc.pfOpZoomFactorGet)();

	zoomDevZoomFactorGetPrintf("ZoomFactorGet m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomSmoothSet(UINT8 mode,UINT8 para1)
 * @brief     set Smooth Zoom Enable/Disable
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [in] para1 : parameter 1
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomSmoothSet(
	UINT8 mode,
	UINT8 para1
)
{
	UINT8 type;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomSmoothSet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomSmoothSet(). */
	(*pdev->opFunc.pfOpZoomSmoothSet)(para1);

	zoomDevZoomSmoothSetPrintf("ZoomSmoothSet m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomSmoothGet(UINT8 mode,UINT8 *outpara1)
 * @brief     get Smooth Zoom Enable/Disable
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [out] *outpara1 : Enable/Disable
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomSmoothGet(
	UINT8 mode,
	UINT8 *outpara1
)
{
	UINT8 type;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomSmoothGet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomSmoothGet(). */
	*outpara1 = (*pdev->opFunc.pfOpZoomSmoothGet)();

	zoomDevZoomSmoothGetPrintf("ZoomSmoothSet m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomSmoothStatusGet(UINT8 mode,UINT8 *outpara1)
 * @brief     get Smooth Zoom status
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [out] *outpara1 : FAIL is not Finish, SUCCESS is Finish
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomSmoothStatusGet(
	UINT8 mode,
	UINT8 *outpara1
)
{
	UINT8 type;
	UINT8 dummy = 0;/* avoid XDATA overflow ,don't modify*/
	zoomDev_t *pdev;

	if(dummy == 1)  {
		zoomDevInit();     /* avoid XDATA overflow*/
	}
	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomSmoothStatusGet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomSmoothStatusGet(). */
	*outpara1 = (*pdev->opFunc.pfOpZoomSmoothStatusGet)();

	zoomDevZoomSmoothStatusGetPrintf("ZoomSmoothSet m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomSmooth3APreSet(UINT8 mode)
 * @brief     Zoom Smooth 3A Pre porc set
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomSmooth3APreSet(
	UINT8 mode
)
{
	UINT8 type;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomSmooth3APreSet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomSmooth3APreSet(). */
	(*pdev->opFunc.pfOpZoomSmooth3APreSet)();

	zoomDevZoomSmooth3APreSetPrintf("ZoomSmooth3APreSet m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomSmooth3APostSet(UINT8 mode)
 * @brief     Zoom Smooth 3A Post porc set
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomSmooth3APostSet(
	UINT8 mode
)
{
	UINT8 type;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomSmooth3APostSet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomSmooth3APostSet(). */
	(*pdev->opFunc.pfOpZoomSmooth3APostSet)();

	zoomDevZoomSmooth3APostSetPrintf("ZoomSmooth3APostSet m=%bd, t=%bd\n", mode, type);

	return SUCCESS;

}

/**
 * @fn        UINT8 zoomDevZoomSmoothTeleSet(UINT8 mode)
 * @brief     Zoom Smooth Tele Set
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomSmoothTeleSet(
	UINT8 mode
)
{
	UINT8 type,sts;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomSmoothTeleSet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomSmoothTeleSet(). */
	sts = (*pdev->opFunc.pfOpZoomSmoothTeleSet)();

	zoomDevZoomSmoothTeleSetPrintf("ZoomSmoothTeleSet m=%bd, t=%bd\n", mode, type);

	return sts;

}

/**
 * @fn        UINT8 zoomDevZoomSmoothWideSet(UINT8 mode)
 * @brief     Zoom Smooth Wide Set
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8
zoomDevZoomSmoothWideSet(
	UINT8 mode
)
{
	UINT8 type,sts;

	zoomDev_t *pdev;

	/* get zoom type */
	zoomDevActGet(mode,&type);

	/* check mode validation. */
	zoomDevModeCheck(&mode, &type, &pdev);

	/* check func point exist */
	if ( *pdev->opFunc.pfOpZoomSmoothWideSet == NULL ) {
		return FAIL;
	}

	/* run physical zoom operating function ZoomSmoothWideSet(). */
	sts = (*pdev->opFunc.pfOpZoomSmoothWideSet)();

	zoomDevZoomSmoothWideSetPrintf("ZoomSmoothWideSet m=%bd, t=%bd\n", mode, type);

	return sts;

}

/**
 * @fn        UINT16 com_zoomDevZoomFactorGet(UINT8 mode)
 * @brief     Get current Zoom factor
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @retval    return = Zoom factor
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT16
com_zoomDevZoomFactorGet(
	UINT8 mode
)
{

	UINT16 outpara1;
	
	zoomDevZoomFactorGet(mode,&outpara1);
	
	return outpara1;
}

/**
 * @fn        UINT8 com_zoomDevZoomSmoothGet(UINT8 mode)
 * @brief     get Smooth Zoom Enable/Disable
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @retval    return = Enable/Disable
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
com_zoomDevZoomSmoothGet(
	UINT8 mode
)
{
	UINT8 outpara1;
	
	zoomDevZoomSmoothGet(mode,&outpara1);
	
	return outpara1;
}

/**
 * @fn        UINT8 com_zoomDevZoomSmoothStatusGet(	UINT8 mode)
 * @brief     get Smooth Zoom status
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @retval    return =  FAIL is not Finish, SUCCESS is Finish
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8 
com_zoomDevZoomSmoothStatusGet(
	UINT8 mode
)
{
	UINT8 outpara1;
	
	zoomDevZoomSmoothStatusGet(mode,&outpara1);
	
	return outpara1;
}

/**
 * @fn        UINT8 com_zoomDevZoomCtrlParaGet(UINT8 mode,UINT8 para1)
 * @brief     get Smooth Zoom status
 * @param     [in] mode : typedef enum ZOOM_LOGI_MODE
 * @param     [in] para1 : Still Mode typedef enum PV_ZOOM_CTRL_PARA_ITEM;
 *                                 Video Mode typedef enum VIDEO_ZOOM_CTRL_PARA_ITEM;
 * @retval    return = paramter value
 * @see       NULL
 * @author    LinJieCheng
 * @since     2012-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT16
com_zoomDevZoomCtrlParaGet(
	UINT8 mode,
	UINT8 para1
)
{
	UINT16 outpara1;
	
	zoomDevZoomCtrlParaGet(mode,para1,&outpara1);
	
	return outpara1;
}
