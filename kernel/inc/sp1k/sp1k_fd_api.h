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
 * @author   wangjg
 * @since     2010-04-21
 * @date      2010-04-21
*/

#ifndef _FD_API_H__
#define _FD_API_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "sp1k_front_api.h" 
#include "knl_func_opt.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum{
	FD_PARAM_FaceNumMax=0,
	//FD_PARAM_RectResolution,
	//FD_PARAM_Mode,
	//FD_PARAM_ReadOnly,
	FD_PARAM_FaceNum,// = FD_PARAM_ReadOnly,	
	FD_PARAM_Rotate,
	FD_PARAM_ShowMode,//bit0:show all/Key(only), bit1:show sub(mouse/eye)
	FD_PARAM_UINT16,
	FD_PARAM_UINT32=FD_PARAM_UINT16,
	FD_PARAM_IIAddr = FD_PARAM_UINT32,
	FD_PARAM_Num,
}fd_param_t;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 #define FD_ENABLE FUNC_KNL_FD
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 fdEnable(UINT8 en); 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
/**
 * @fn		  void sp1kFdReset(void)
 * @brief	         sp1k fd reset function.
 * @param       NONE
 * @retval	  NONE
*/
void sp1kFdReset(void);

/**
 * @fn        void sp1kFdInit(void)
 * @brief     initialize  libfd
 * @retval    NONE
 * @see       NULL
*/
void sp1kFdInit(void);

/**
 * @fn        void fdSvcProc(void* ptr)
 * @brief     fdSvcProc
 * @param     [in] ptr
 * @retval    NONE
*/
void fdSvcProc(void* ptr);

/**
 * @fn        void fdIntr(void)
 * @brief     trigger fdDetect() by interrupt 
 * @param     NONE
 * @retval    NONE
*/
void fdIntr(void); 

/**
 * @fn        UINT32 sp1kFdParamSet(UINT8 idx, UINT32 val)
 * @brief     sp1k Fd Param Set
 * @param     [in] idx    
 *\n			FD_PARAM_FaceNumMax, fd system detection max face number
 *\n			FD_PARAM_FaceNum, current face number
 *\n			FD_PARAM_Rotate, Rotate of current face
 *\n			FD_PARAM_ShowMode, osd show mode, bit0:show all/Key(only), bit1:show sub(mouse/eye)
 *\n			FD_PARAM_UINT16, detected source address (16bit)
 *\n			FD_PARAM_UINT32, detected source address (32bit)
 *\n			FD_PARAM_IIAddr, detected source address
 * @param     [in] val	
 *\n				1:write 
 *\n				0xffffffff return param val
 * @retval    return = lst value
*/
UINT32 sp1kFdParamSet(UINT8 idx, UINT32 val);

/**
 * @fn        UINT32 sp1kFdParamGet(UINT8 idx)
 * @brief     sp1k Fd Param Get
 * @param     [in] idx    
 *\n			FD_PARAM_FaceNumMax, fd system detection max face number
 *\n			FD_PARAM_FaceNum, current face number
 *\n			FD_PARAM_Rotate, Rotate of current face
 *\n			FD_PARAM_ShowMode, osd show mode, bit0:show all/Key(only), bit1:show sub(mouse/eye)
 *\n			FD_PARAM_UINT16, detected source address (16bit)
 *\n			FD_PARAM_UINT32, detected source address (32bit)
 *\n			FD_PARAM_IIAddr, detected source address
 * @retval    return = lst value
*/
UINT32 sp1kFdParamGet(UINT8 idx);

/**
 * @fn        UINT8 sp1kPbFdProcBlink(UINT32 addrQVGA, UINT32 addrVGA)
 * @brief     sp1k Pb Fd Proc Blink (this function not any effect)
 * @param     [in] addrQVGA    :address
 *\n			[in] addrVGA      :address
 * @retval    return FALSE
*/
UINT8 sp1kPbFdProcBlink(UINT32 addrQVGA, UINT32 addrVGA);

/**
 * @fn        UINT8 sp1kFdEnable(void)
 * @brief     fd Enble function.
 * @param     NONE
 * @retval    return = last status
*/
UINT8 sp1kFdEnable(void);

/**
 * @fn        UINT8 sp1kFdDisable(void)
 * @brief     fd disable function.
 * @param     NONE
 * @retval    return = last status
*/
UINT8 sp1kFdDisable(void);

/**
 * @fn        UINT8 sp1kFdGetStatus(void)
 * @brief     Get fd status function.
 * @param     NONE
 * @retval    return = last status
*/
UINT8 sp1kFdGetStatus(void);

/**
 * @fn		  UINT8 sp1kFdSetSmile(UINT8 en)
 * @brief	  	 Fd Set Smile en function.
 * @param[in]	  en:
 *\n				0: smile disable
 *\n				1: smile enable
 * @retval	  return = last status
*/
UINT8 sp1kFdSetSmile(UINT8 en);



#define sp1kFdParamGet(idx)	sp1kFdParamSet(idx,0xffffffff)
#define sp1kFdEnable()		fdEnable(1)
#define sp1kFdDisable()		fdEnable(0)
#define sp1kFdGetStatus()	fdEnable(0xff)
#define sp1kFdInit()		//sp1kFdReset()




#endif
