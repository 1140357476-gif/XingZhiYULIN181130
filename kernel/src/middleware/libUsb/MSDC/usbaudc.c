/**************************************************************************
 *		   Copyright(c) 2010 by i-Catch Technology Co., Ltd.			  *
 *																		  *
 *	This software is copyrighted by and is the property of i-Catch Tech-  *
 *	nology Co., Ltd. All rights are reserved by i-Catch Technology Co.,   *
 *	Ltd. This software may only be used in accordance with the			  *
 *	corresponding license agreement. Any unauthorized use, duplication,   *
 *	distribution, or disclosure of this software is expressly forbidden.  *
 *																		  *
 *	This Copyright notice "M U S T" not be removed or modified without	  *
 *	prior written consent of i-Catch Technology Co., Ltd.				  *
 *																		  *
 *	i-Catch Technology Co., Ltd. reserves the right to modify this		  *
 *	software without notice.											  *
 *																		  *
 *	i-Catch Technology Co., Ltd.										  *
 *	19-1, Innovation First Road, Science-Based Industrial Park, 		  *
 *	Hsin-Chu, Taiwan, R.O.C.											  *
 *																		  *
 *************************************************************************/
 
/**
 * @file	  usbaudc.c
 * @brief	  Module related to USB Audio Device Class
 * @author	  Chi-Yeh Tsai
 * @since	  2010-01-01
 * @date	  2010-01-01
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "usbopt.h"

#if (USBAUDC_OPTION == 1)

#include "usbaudc.h"
#include "ctlsetup.h" 
/**************************************************************************
 *							 C O N S T A N T S							  *
 **************************************************************************/
 
/**************************************************************************
 *								M A C R O S 							  *
 **************************************************************************/
 
/**************************************************************************
 *							D A T A    T Y P E S						  *
 **************************************************************************/
 
/**************************************************************************
 *						   G L O B A L	  D A T A						  *
 **************************************************************************/
 
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
 
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

/**
 * @fn		  void USBAUDC_NoDataPhase(void)
 * @brief	  process class request without data phase
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void USBAUDC_NoDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//USBAUDC_PreInDataPhase  
//-----------------------------------------------------------------------------
/**
 * @fn		  void USBAUDC_PreInDataPhase(void)
 * @brief	  pre-process setting of following in data phase
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void USBAUDC_PreInDataPhase(void) USING_1
{
	switch (G_pCtrlCommand->bRequest)
	{
		case K_USBAUDC_GET_CUR:
			break;
		
		case K_USBAUDC_GET_MIN:
			break;
		
		case K_USBAUDC_GET_MAX:
			break;
		
		case K_USBAUDC_GET_RES:
			break;
		
		case K_USBAUDC_GET_MEM:
			break;

		default:
			break;
	}				
}

//-----------------------------------------------------------------------------
//USBAUDC_PreOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn		  void USBAUDC_PreOutDataPhase(void)
 * @brief	  pre-process setting of following out data phase
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void USBAUDC_PreOutDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//USBAUDC_PostInDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn		  void USBAUDC_PostInDataPhase(void)
 * @brief	  post-process data of complete in data phase
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void USBAUDC_PostInDataPhase(void) USING_1
{
}

//-----------------------------------------------------------------------------
//USBAUDC_PostOutDataPhase
//-----------------------------------------------------------------------------
/**
 * @fn		  void USBAUDC_PostOutDataPhase(void)
 * @brief	  post-process data of complete out data phase
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void USBAUDC_PostOutDataPhase(void) USING_1
{
	switch (G_pCtrlCommand->bRequest)
	{
		case K_USBAUDC_SET_CUR:
			break;

		case K_USBAUDC_SET_MIN:
			break;

		case K_USBAUDC_SET_MAX:
			break;
		
		case K_USBAUDC_SET_RES:
			break;
		
		case K_USBAUDC_SET_MEM:
			break;

		default:
			break;
	}				
}

#endif
