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
 * @file	  bulkin.c
 * @brief	  Module related to bulk in pipe
 * @author	  Chi-Yeh Tsai
 * @since	  2010-01-01
 * @date	  2010-01-01
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "bulkin.h"
#include "bulkout.h"
#include "intr.h"
#include "main.h"
#include "usbmsdc2.h"
//#include "usbsidc.h"
//#include "sidcOption.h"
//richie@si0425
#include "uiflow.h"
#include "usbopt.h" 
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

//-----------------------------------------------------------------------------
//BULKIN_Packet
//-----------------------------------------------------------------------------
/**
 * @fn		  void BULKIN_Packet(void)
 * @brief	  process packet of bulk in pipe of storage interface 
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void BULKIN_Packet(void) USING_1
{
//richie mark
#if (USBMSDC_OPTION == 2)
	if (G_ucPktStorIntrf == K_StorageInterface1)
	{
		//richie@si0417
		if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
			&& (gUsbMode == K_UIOPTION_STORAGE_MSDC) ) {	
			USBMSDC_BoBulkInData();
		}
	}
	else //if (G_ucPktStorIntrf == K_StorageInterface2)
	{
		//richie@si0417
		if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
			&& (gUsbMode == K_UIOPTION_STORAGE_MSDC) ) {	
			USBMSDC_BoBulkInData();
		}	
	}
#endif
}
