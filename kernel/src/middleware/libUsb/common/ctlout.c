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
 * @file	  ctlout.c
 * @brief	  Module related to out packet of control pipe
 * @author	  Chi-Yeh Tsai
 * @since	  2010-01-01
 * @date	  2010-01-01
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "ctlout.h"
#include "ctlsetup.h"
#include "ctlin.h"
#include "usbaudc.h"
//#include "usbsidc.h"
#include "usbmsdc2.h"
#include "stdreq.h"
#include "vndreq.h"
#include "rsvreq.h"
#include "main.h"
#include "uiflow.h"
//#include "sidcOption.h"
#include "usbopt.h"
//#include "class.h"
#include "Clsreq.h" 
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
//CTLOUT_Packet
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLOUT_Packet(void)
 * @brief	  process out packet of control pipe
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLOUT_Packet(void) USING_1
{
	UINT8 type;

	if (G_ucCtrlPhase == K_OutDataPhase)			  //out data phase				  
	{
		//check Type of bmRequestType
		type = G_pCtrlCommand->bmRequestType & 0x60;
		if (type == 0x00)
		{
			CTLOUT_StandardRequest();		//Standard request
		}
		else if (type == 0x20)
		{
			CTLOUT_ClassRequest();			//Class request
		}
		else if (type == 0x40)
		{
			CTLOUT_VendorRequest(); 		//Vendor request
		}
		else //if (type == 0x60)
		{
			CTLOUT_ReservedRequest();		//Reserved request
		}
	}
	else if (G_ucCtrlPhase == K_OutResponsePhase)	  //out response phase
	{
		//just out zero packet - 0 byte in EP0 FIFO
		//patch4.3@richie@oh0530
		G_ucCtrlPhase = K_CommandPhase;
	}
	else											//phase transition error
	{
		//phase transition error - out packet should not appear
		//						   at current phase
		//just out zero packet - 0 byte in EP0 FIFO
	}
}

//-----------------------------------------------------------------------------
//CTLOUT_StandardRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLOUT_StandardRequest(void)
 * @brief	  process out packet of standard request
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLOUT_StandardRequest(void) USING_1
{
	CTLOUT_GetOutPacketData();

	//phase transition
	if ((G_ucCtrlPacketIndex == 8) &&	//bMaxPacketSize of Control pipe
		(G_usCtrlDataIndex < G_usCtrlDataLength))
	{
		G_ucCtrlPhase = K_OutDataPhase;
	}
	else
	{
		STDREQ_PostOutDataPhase();
		G_ucCtrlPhase = K_InResponsePhase;
		//CTLIN_Packet();
	}
}

//-----------------------------------------------------------------------------
//CTLOUT_ClassRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLOUT_ClassRequest(void)
 * @brief	  process out packet of class request  
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLOUT_ClassRequest(void) USING_1
{
#if (USBMSDC_OPTION == 2)		//Mass Storage Device Class (Bulk-Only)
	//richie@0204
        if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
		&& (gUsbMode == K_UIOPTION_STORAGE_MSDC) )	
	{
		CTLOUT_GetOutPacketData();

		if ((G_ucCtrlPacketIndex == 8) &&	//bMaxPacketSize of Control pipe
			(G_usCtrlDataIndex < G_usCtrlDataLength))
		{
			G_ucCtrlPhase = K_OutDataPhase;
		}
		else
		{
			USBMSDC_BoPostOutDataPhase();
			G_ucCtrlPhase = K_InResponsePhase;
			//CTLIN_Packet();
		}
	}
#endif

        //cx add for UVC
        if ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_PCCAM) 
	{
            CTLOUT_GetOutPacketData();

            if ((G_ucCtrlPacketIndex == 64)&&	//bMaxPacketSize of Control pipe
		(G_usCtrlDataIndex < G_usCtrlDataLength))
            {
                G_ucCtrlPhase = K_OutDataPhase;
            }
            else
            {
                CLSREQ_UVCPostOutDataPhase();
                G_ucCtrlPhase = K_InResponsePhase;
            }
        }
}

//-----------------------------------------------------------------------------
// CTLOUT_VendorRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLOUT_VendorRequest(void)
 * @brief	  process out packet of vendor request
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLOUT_VendorRequest(void) USING_1
{
	CTLOUT_GetOutPacketData();

	//phase transition
	if ((G_ucCtrlPacketIndex == 64) &&	 //bMaxPacketSize of Control pipe
		(G_usCtrlDataIndex < G_usCtrlDataLength))
	{
		G_ucCtrlPhase = K_OutDataPhase;
	}
	else
	{
		VNDREQ_PostOutDataPhase();
		G_ucCtrlPhase = K_InResponsePhase;
		//CTLIN_Packet();
	}
}

//-----------------------------------------------------------------------------
//CTLOUT_ReservedRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLOUT_ReservedRequest(void)
 * @brief	  process out packet of reserved request
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLOUT_ReservedRequest(void) USING_1
{
	CTLOUT_GetOutPacketData();

	//phase transition
	if ((G_ucCtrlPacketIndex == 64) &&	 //bMaxPacketSize of Control pipe
		(G_usCtrlDataIndex < G_usCtrlDataLength))
	{
		G_ucCtrlPhase = K_OutDataPhase;
	}
	else
	{
		RSVREQ_PostOutDataPhase();
		G_ucCtrlPhase = K_InResponsePhase;
		CTLIN_Packet();
	}
}

//-----------------------------------------------------------------------------
//CTLOUT_GetOutPacketData
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLOUT_GetOutPacketData(void)
 * @brief	  get data for current Out packet
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLOUT_GetOutPacketData(void) USING_1
{
	G_ucCtrlPacketIndex = 0;			//initialize packet index
	while ((G_ucCtrlPacketIndex < 64) && (G_usCtrlDataIndex < G_usCtrlDataLength))
	{
		G_ucCtrlData[G_usCtrlDataIndex] = XBYTE[REG_Usb_Ep0BufData];
		G_ucCtrlPacketIndex++;
		G_usCtrlDataIndex++;
	}
}
