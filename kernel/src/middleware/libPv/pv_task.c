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
 * @file      Pv_task.c
 * @brief     Set preview buffer/size settings
 * @author    Will Chiu
 * @since     2010-01-01
 * @date      2010-01-01
*/



/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h"
#include "pv_task.h"
#include "initio.h"
#include "app_still_view.h"
#include "sp1k_snap_api.h"
#include "snap_qtable.h"
#include "doslink.h"
#include "disp_api.h"
#include "hal_cdsp.h"
#include "hal_cdsp_core.h"
#include "hal.h"
#include "sp1k_pv_api.h"
#include "hal_dram.h" 
#include "pv_debug.h"
#include "utility.h"
#include "sp1k_disp_api.h"
#include "hal_disp.h"
#include "os_api.h"
#include "hal_global.h"
#include "front_logi.h"
#include "hal_front.h"
#include "app_video_view.h"
#include "sp1k_ae_api.h"
//#include "video_mem.h"
#include "sp1k_video_api.h"
#include "Reg_cdsp.h" 
#include "Reg_front.h" 
#include "app_video_view.h"
#include "sp1k_rsvblk_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define __FILE	__FILE_ID_PV_TASK__


#define G_HSize_P K_Preview_Width
#define G_VSize_P K_Preview_Height

#define PV_TASK_TAG_TODO
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT16 pvHsize = K_Preview_Width;
UINT16 pvVsize = K_Preview_Height;

UINT16 pvClampHsize;
UINT16 pvClampVsize;

//UINT8 pvZfUpdateFlag; 
//UINT8 videoZfUpdateFlag;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
/**
 * @fn        void pvSizeSet(UINT16 hSize, UINT16 vSize)
 * @brief     set preview size
 * @param     [in] hSize : preveiw horizontal size
 * @param     [in] vSize : preveiw vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvSizeSet(
	UINT16 hSize, 
	UINT16 vSize
) 
{
	//UINT8 ratX, ratY;  /* will mark */

	pvHsize = hSize;
	pvVsize = vSize;

	/* xian ++ 20090915 */
	pvClampHsize = hSize;
	pvClampVsize = vSize;

	#ifndef PV_TASK_TAG_TODO 
	dispPnlAspectRatioGet(&ratX, &ratY);  	
	#else
	//ratX=4;
	//ratY=3;		
	#endif

	/*
	if (hSize * ratY > vSize * ratX) {
		pvVsize = hSize * ratY / ratX;
	}
	else if (hSize * ratY < vSize * ratX) {
		pvHsize = vSize * ratX / ratY;
	}
	*/

	//pvZfUpdateFlag =0;		/* initial zf udate flag */
	//videoZfUpdateFlag=0;	/* initial zf udate flag */
	
	pvSizeSetPrintf("hSize=%d,vSize=%d\n",hSize,vSize);
	pvSizeSetPrintf("ratX=%bd,ratY=%bd\n",ratX,ratY);
	pvSizeSetPrintf("pvClampHsize=%d,pvClampVsize=%d\n",pvClampHsize,pvClampVsize);
	 
}

/**
 * @fn        void pvSizeGet(UINT16 *phSize, UINT16 *pvSize)
 * @brief     get preview size
 * @param     [out] hSize : preview horizontal size
 * @param     [out] vSize : preview vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvSizeGet(
	UINT16 *hSize, 
	UINT16 *vSize
) USING_0
{
	*hSize = pvHsize;
	*vSize = pvVsize;

	pvSizeGetPrintf("pvHsize=%d,pvVsize=%d\n",pvHsize,pvVsize);
}

/**
 * @fn        void pvIntrSizeGet(UINT16 *phSize, UINT16 *pvSize)
 * @brief     get preview size
 * @param     [out] hSize : preview horizontal size
 * @param     [out] vSize : preview vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvIntrSizeGet(
	UINT16 *hSize, 
	UINT16 *vSize
) USING_0
{
	*hSize = pvHsize;
	*vSize = pvVsize;

	pvSizeGetPrintf("pvHsize=%d,pvVsize=%d\n",pvHsize,pvVsize);
}


/**
 * @fn        void pvClampSizeGet(UINT16 *phSize, UINT16 *pvSize)
 * @brief     get preview clamp size
 * @param     [in] hSize : preview clamp horziton size
 * @param     [in] vSize : preview clamp vertical size
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
void 
pvClampSizeGet(
	UINT16 *hSize, 
	UINT16 *vSize
) USING_0
{
	*hSize = pvClampHsize;
	*vSize = pvClampVsize;

	pvClampSizeGetPrintf("pvClampHsize=%d,pvClampVsize=%d\n",pvClampHsize,pvClampVsize);
}

/**
 * @fn        UINT32 pvFreeBuffGet(void)
 * @brief     get free dram address 
 * @param     NONE
 * @retval    return = free dram address 
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/
UINT32 pvFreeBuffGet(void) USING_0
{
	return K_SDRAM_CommonEndAddr;
}


/**
 * @fn        void pvBufSave(void)
 * @brief     save preview buffer
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Will Chiu
 * @since     2010-5-12
 * @todo      N/A
 * @bug       N/A
*/

#if 0
void 
pvBufSave(
	void
)
{
  UINT8  ret;
  UINT8 name[12]= "A0000003YUV";
  UINT32 filesize;

  filesize = (((UINT32)G_HSize_P*(UINT32)G_VSize_P)*(UINT32)2);
  ret = dramFileSave(name ,K_SDRAM_PvBufAddrA, filesize);
}
#endif

/*
void 
pvFwZFSet(
	UINT8 val
)
{
	pvZfUpdateFlag = val&0x01;

}
#include "sp1k_snap_api.h"
*/
