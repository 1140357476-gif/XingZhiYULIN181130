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
 * @file	  ctlin.c
 * @brief	  Module related to in packet of control pipe
 * @author	  Chi-Yeh Tsai
 * @since	  2010-01-01
 * @date	  2010-01-01
*/
/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "ctlin.h"
#include "ctlsetup.h"
#include "stdreq.h"
#include "vndreq.h"
#include "rsvreq.h"
#include "hal.h"
#include "main.h"
#include "usbaudc.h"
//#include "usbsidc.h"
#include "usbmsdc2.h"
#include "cardlink.h"
#include "uiflow.h"
//#include "sidcOption.h"
#include "usbopt.h"
//#include "class.h"
//#include "Clsreq.h" 
#include "reg_def.h"
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
//CTLIN_Packet
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLIN_Packet(void)
 * @brief	  process in packet of control pipe
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLIN_Packet(void) USING_1
{
	UINT8 type;

	if (G_ucCtrlPhase == K_InDataPhase) 			  //in data phase
	{
		//check Type of bmRequestType
		type = G_pCtrlCommand->bmRequestType & 0x60;
		if (type == 0x00)
		{
			CTLIN_StandardRequest();		//Standard request
		}
		else if (type == 0x20)
		{				
			CTLIN_ClassRequest();			//Class request
		}
		else if (type == 0x40)
		{
			CTLIN_VendorRequest();			//Vendor request
		}				
		else //if (type == 0x60)
		{
			CTLIN_ReservedRequest();		//Reserved request
		}
	}
	else if (G_ucCtrlPhase == K_InResponsePhase)	  //in response phase	  
	{
		//just in zero packet - 0 byte in EP0 FIFO
		//patch4.3@richie@oh0530
		G_usbState |= 0x01;
		G_ucCtrlPhase = K_CommandPhase;
	}
	else											//phase transition error
	{
		G_usbState |= 0x01;
		//phase transition error - in packet should not appear
		//						   at current phase
		//just in zero packet - 0 byte in EP0 FIFO
	}
}

//-----------------------------------------------------------------------------
//CTLIN_StandardRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLIN_StandardRequest(void)
 * @brief	  process in packet of standard request
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLIN_StandardRequest(void) USING_1
{
	CTLIN_SetInPacketData();		//just send the data if any 
	
	//phase transition
	if (G_ucCtrlPacketIndex > 0)		//n byte in EP0 FIFO	
	{
		G_ucCtrlPhase = K_InDataPhase;
	}
	else							//0 byte in EP0 FIFO
	{
		STDREQ_PostInDataPhase();
		G_ucCtrlPhase = K_OutResponsePhase;
	}
}

//-----------------------------------------------------------------------------
//CTLIN_ClassRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLIN_ClassRequest(void)
 * @brief	  process in packet of class request
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLIN_ClassRequest(void) USING_1
{  						  
	//G_ucCtrlPacketIndex = 0;	//initialize packet index

#if (USBMSDC_OPTION == 2)		//Mass Storage Device Class (Bulk-Only)
	if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
		&& (gUsbMode == K_UIOPTION_STORAGE_MSDC) )	
	{
		CTLIN_SetInPacketData();		//just send the data if any 

		//phase transition
		if (G_ucCtrlPacketIndex > 0)		//n byte in EP0 FIFO	
		{
			G_ucCtrlPhase = K_InDataPhase;
		}
		else							//0 byte in EP0 FIFO
		{
			USBMSDC_BoPostInDataPhase();
			G_ucCtrlPhase = K_OutResponsePhase;
		}
	}
#endif
	//cx add for pccamer  probe and commit 
	if ( (G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_PCCAM)
	{
		CTLIN_SetInPacketData();
	 
		 //phase transition
		if ((G_ucCtrlPacketIndex > 0) &&(G_usCtrlDataIndex <= G_usCtrlDataLength))		//n byte in EP0 FIFO	
		{
			G_ucCtrlPhase = K_InDataPhase;
		}
		else							//0 byte in EP0 FIFO
		{
			G_ucCtrlPhase = K_OutResponsePhase;
		}
	}
//cx add end
}

//-----------------------------------------------------------------------------
// CTLIN_VendorRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLIN_VendorRequest(void)
 * @brief	  process in packet of vendor request
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLIN_VendorRequest(void) USING_1
{
	CTLIN_SetInPacketData();		//just send the data if any 
	
	//phase transition
	if (G_ucCtrlPacketIndex > 0)		//n byte in EP0 FIFO	
	{
		G_ucCtrlPhase = K_InDataPhase;
	}
	else							//0 byte in EP0 FIFO
	{
		VNDREQ_PostInDataPhase();
		G_ucCtrlPhase = K_OutResponsePhase;
	}
}

//-----------------------------------------------------------------------------
//CTLIN_ReservedRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLIN_ReservedRequest(void)
 * @brief	  process in packet of reserved request
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLIN_ReservedRequest(void) USING_1
{
	CTLIN_SetInPacketData();		//just send the data if any 
	
	//phase transition
	if (G_ucCtrlPacketIndex > 0)		//n byte in EP0 FIFO	
	{
		G_ucCtrlPhase = K_InDataPhase;
	}
	else							//0 byte in EP0 FIFO
	{
		RSVREQ_PostInDataPhase();
		G_ucCtrlPhase = K_OutResponsePhase;
	}
}

//-----------------------------------------------------------------------------
//CTLIN_SetInPacketData
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLIN_SetInPacketData(void)
 * @brief	  set data for next in packet
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLIN_SetInPacketData(void)
{
	G_ucCtrlPacketIndex = 0;	//initialize packet index

	while ((G_ucCtrlPacketIndex < 64) && (G_usCtrlDataIndex < G_usCtrlDataLength))
	{
		XBYTE[REG_Usb_Ep0BufData] = G_pucCtrlDataPointer[G_usCtrlDataIndex]; 
		G_ucCtrlPacketIndex++;
		G_usCtrlDataIndex++;
	}
}
