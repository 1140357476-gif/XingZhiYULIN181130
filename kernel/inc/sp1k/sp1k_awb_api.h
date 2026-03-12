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

#ifndef _SP1K_AWB_API_H__
#define _SP1K_AWB_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "general.h" 
#include "sp1k_front_api.h"
#include "sp1k_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum
{	
	AWB_MODE_Auto = 0x0,
	AWB_MODE_Spots=0x0,

	AWB_MODE_Normal = 0x00,
	AWB_MODE_Tungsten,// =  AWB_MODE_Normal,		
	AWB_MODE_Fluorescent_L,
	AWB_MODE_Fluorescent,
	AWB_MODE_Fluorescent_H, 
	AWB_MODE_DayLight, 

	AWB_MODE_Outdoor = 0x20,
	AWB_MODE_Sunny,// = AWB_MODE_Outdoor,
	AWB_MODE_Cloudy,
	AWB_MODE_Sunset,
	AWB_MODE_Forest,
	AWB_MODE_Sward,

	AWB_MODE_Rects=0x40,
	AWB_MODE_Flash,// = AWB_MODE_Rects,
	AWB_MODE_Calibation,
	
	AWB_MODE_Avg=0x80,
	AWB_MODE_Max,

	AWB_MODE_Force=0xf0,
	AWB_MODE_Enable=0xfd,
	AWB_MODE_Disable=0xfe,
	AWB_MODE_Get=0xff
}awb_mode_t;

typedef enum
{
	AWB_PROC_Preview=0x0,
	AWB_PROC_AVI,
	AWB_PROC_Init,
	AWB_PROC_Snap,
	AWB_PROC_Max,
	AWB_PROC_Enable=0xfd,
	AWB_PROC_Disable=0xfe,	
	AWB_PROC_Get	=0xff,
}awb_proc_t;

typedef enum
{
	AWB_SCENE_Normal=0,
	AWB_SCENE_Outdoor,
	AWB_SCENE_RectOne,
	//AWB_SCENE_Flash,
	//AWB_SCENE_Calibration,
	//AWB_SCENE_Dark,
	AWB_SCENE_NUM,
	
}AWB_SCENE_t;
typedef enum 
{
	AWB_PARA_RunIdx,
	AWB_PARA_Stable,
	AWB_PARA_DarkSatMode,
	AWB_PARA_AeInfoAddr,
	AWB_PARA_SpotSwitchDelayCnt,//0~255 def:2 
	AWB_PARA_ColorTemperatureMin,//def 2000K
	AWB_PARA_ColorTemperatureMax,//def 7500K
	AWB_PARA_JumpStep,//def 7500K
}awb_param_t;
typedef enum
{
	AWB_FUNC_Disable = 0,
	//AWB_FUNC_Default=0,
	AWB_FUNC_Enable,	
		
	AWB_FUNC_Get,
	AWB_FUNC_Mode= 0x20,
}awb_func_t;

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

extern UINT8 awbEnable(UINT8 en);
extern UINT16 awbGlobeGainSet(	UINT16 gain	);
extern void awbCalibStart();
extern void awbCalibDo(UINT8 idx);
extern void awbCalibCtt(UINT8 isCalib);
//extern void awbCalibLv10();
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#ifdef SP1K_DOXY_GEN 
/**
 * @fn		  UINT16 sp1kAwbGlobleGainSet(UINT16 gain)
 * @brief	  set globe gain* awb gain	to cdsp
 * @param	  [in] gain :base on 256, if gain==0xffff,return globle gain only
 * @retval	  last globle gain
 * @see 	  NULL
 * @author	  wangjg
 * @since	  2010-5-4
 * @todo	  N/A
 * @bug 	  N/A
*/
UINT16 sp1kAwbGlobleGainSet(UINT16 gain);

/**
 * @fn        UINT8 sp1kAwbParamGet(UINT8 idx)
 * @brief     sp1kAwbParamGet
 * @param     [in] idx	:param index
 * @retval    return = param val
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kAwbParamGet(UINT8 idx);


/**
 * @fn        UINT8 sp1kAwbEnable(void)
 * @brief     enable awb
 * @param     NONE
 * @retval     awb last status
 * @author    wangjg
 * @since     2010-5-14
*/
UINT8 sp1kAwbEnable(void);


/**
 * @fn        UINT8 sp1kAwbReset(void)
 * @brief     reset awb
 * @param     NONE
 * @retval     awb last status
 * @author    wangjg
 * @since     2010-5-14
*/
UINT8 sp1kAwbReset(void);


/**
 * @fn        UINT8 sp1kAwbDisable(void)
 * @brief     disable awb
 * @param     NONE
 * @retval    awb last status
 * @author    wangjg
 * @since     2010-5-14
*/
UINT8 sp1kAwbDisable(void);

/**
 * @fn        UINT8 sp1kAwbGetStatus(void)
 * @brief     return awb on/off
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-8
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kAwbGetStatus(void);

/**
 * @fn        UINT8 sp1kAwbModeGet(void)
 * @brief     sp1kAwbModeGet
 * @param     NONE
 * @retval    mode
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-6
 * @todo      N/A
 * @bug       N/A
*/
UINT8 sp1kAwbModeGet(void);

/**
 * @fn        void sp1kAwbCalibStart(void)
 * @brief     awb Calibration Start
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kAwbCalibStart();

/**
 * @fn        void sp1kAwbCalibDo(UINT8 idx)
 * @brief     compute gain and save to cttGain[idx]
 * @param     [in] idx
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kAwbCalibDo(UINT8 idx);

/**
 * @fn        void sp1kAwbCalibCtt(void)
 * @brief     create awbctt
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kAwbCalibCtt(UINT8 isCalib);
#if 0
/**
 * @fn        void sp1kAwbCalibLv10(void)
 * @brief     create or calib A_lv10 gain
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Wangjg
 * @since     2011-8-17
 * @todo      N/A
 * @bug       N/A
*/
void sp1kAwbCalibLv10();
#endif
#else

/**
 * @fn		  UINT16 sp1kAwbGlobleGainSet(UINT16 gain)
 * @brief	  set globe gain* awb gain	to cdsp
 * @param	  [in] gain :base on 256, if gain==0xffff,return globle gain only
 * @retval	  last globle gain
 * @see 	  NULL
 * @author	  wangjg
 * @since	  2010-5-4
 * @todo	  N/A
 * @bug 	  N/A
*/
#define sp1kAwbGlobeGainSet(gain)  awbGlobeGainSet(gain) 
/**
 * @fn        UINT8 sp1kAwbParamGet(UINT8 idx)
 * @brief     sp1kAwbParamGet
 * @param     [in] idx	:param index
 * @retval    return = param val
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
#define sp1kAwbParamGet(idx) sp1kAwbParamSet((idx),0xffffffff)

/**
 * @fn        UINT8 sp1kAwbEnable(void)
 * @brief     enable awb
 * @param     NONE
 * @retval     awb last status
 * @author    wangjg
 * @since     2010-5-14
*/
#define sp1kAwbEnable()		awbEnable(1)

/**
 * @fn        UINT8 sp1kAwbReset(void)
 * @brief     reset awb
 * @param     NONE
 * @retval     awb last status
 * @author    wangjg
 * @since     2010-5-14
*/
#define sp1kAwbReset()		awbEnable(2)

/**
 * @fn        UINT8 sp1kAwbDisable(void)
 * @brief     disable awb
 * @param     NONE
 * @retval    awb last status
 * @author    wangjg
 * @since     2010-5-14
*/
#define sp1kAwbDisable()	awbEnable(0)

/**
 * @fn        UINT8 sp1kAwbGetStatus(void)
 * @brief     return awb on/off
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-8
 * @todo      N/A
 * @bug       N/A
*/
#define sp1kAwbGetStatus()	awbEnable(0xff)
/**
 * @fn        UINT8 sp1kAwbModeGet(void)
 * @brief     sp1kAwbModeGet
 * @param     NONE
 * @retval    mode
 * @see       NULL
 * @author    wangjg
 * @since     2010-7-6
 * @todo      N/A
 * @bug       N/A
*/
#define sp1kAwbModeGet()	sp1kAwbModeSet(AWB_MODE_Get)

#define sp1kAwbCalibStart awbCalibStart
#define sp1kAwbCalibDo awbCalibDo
#define sp1kAwbCalibCtt awbCalibCtt
//#define sp1kAwbCalibLv10 awbCalibLv10
#endif


/**
 * @fn        void sp1kAwbInit(UINT8 isCreate)
 * @brief     initialize awb
 * @param     [in] isCreate:1,Create Queue
 * @retval    NONE
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
void sp1kAwbInit();
#define sp1kAwbInit(a)  sp1kAwbInit() 


void sp1kAwbGainAdjust(UINT16 rGainPer , UINT16 bGainPer);

/**
 * @fn        UINT8 sp1kAwbParamSet(UINT8 idx, idx,UINT8 val)
 * @brief     set awb param list
 * @param     [in] idx	:param index
 * @param     [in] val	:param val(0xffffffff:get val)
 * @retval    return = param val
 * @see       NULL
 * @author    wangjg
 * @since     2010-5-4
 * @todo      N/A
 * @bug       N/A
*/
UINT32 
sp1kAwbParamSet(
	awb_param_t idx,
	UINT32 val
	);



/**
 * @fn        UINT8 sp1kAwbModeSet(UINT8 mode)
 * @brief     old interface  compatibility   
 			set AWB enable/disable/mode
 * @param     [in] mode :AWB_MODE_Get return current mode
 * @retval    return = lst mode
 * @author    wangjg
 * @since     2010-5-14
*/ 
UINT8 
sp1kAwbModeSet(
	UINT8 mode
	);




#define AWB_OLD_API_SUPPORT
#ifdef AWB_OLD_API_SUPPORT


//#define sp1kAwbParaInit(val) //sp1kAwbInit()

typedef enum{
	SP1K_AWB_MODE_DISABLE = AWB_MODE_Disable,/*< One mode of WB which means WB is Disabled.*/
	SP1K_AWB_MODE_AUTO = AWB_MODE_Auto,/*< One mode of WB which include all CTC curve.*/
	SP1K_AWB_MODE_TUNGSTENG = AWB_MODE_Tungsten,/*< One mode of WB which set the range only near "TUNGSTENG".*/
	SP1K_AWB_MODE_FLUORESCENT = AWB_MODE_Fluorescent,/*< One mode of WB which set the range only near "FLUORESCENT".*/
	SP1K_AWB_MODE_DAYLIGHT = AWB_MODE_Sunny,/*< One mode of WB which set the range only near "DAYLIGHT".*/
	SP1K_AWB_MODE_CLOUDY = AWB_MODE_Cloudy,/*< One mode of WB which set the range only near "CLOUDY".*/
	//SP1K_AWB_MODE_CUSTOM,/*< One mode of WB which will let user set rb gain.*/
	//SP1K_AWB_MODE_UNITY,/*< One mode of WB which means WB is Disabled.*/
	SP1K_AWB_MODE_CONTINUE = AWB_MODE_Enable,/*< One mode of WB which means WB will not change and keep the last value.*/
	//SP1K_AWB_MODE_FIX,//FLASH FIX MODE
}awb_mode_old_t; 

//Awb/AWB compatibility   
//#define sp1kAWBModeSet   sp1kAwbModeSet
//#define sp1kAWBParaInit	sp1kAwbParaInit
//#define sp1kAWBScaleSet  sp1kAwbScaleSet

//#define sp1kAwbScaleSet(selector, val)	//invalid
//#define AAA_AWBVideoReordStatusSet(en) //sp1kAwbParamSet(AWB_PARA_Proc, ((en)?AWB_PROC_AVI:AWB_PROC_Preview)))


#endif
#endif
