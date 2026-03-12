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
 * @file	  ctlsetup.c
 * @brief	  Module related to setup packet of control pipe
 * @author	  Chi-Yeh Tsai
 * @since	  2010-01-01
 * @date	  2010-01-01
*/

/**************************************************************************
 *						  H E A D E R	F I L E S
 **************************************************************************/
#include "general.h"
#include "ctlsetup.h"
//#include "hal.h"
#include "main.h"
#include "intr.h"
#include "stdreq.h"
#include "vndreq.h"
#include "rsvreq.h"
#include "Clsreq.h" 
#include "ctlin.h"
#include "usbaudc.h"
//#include "usbsidc.h"
#include "usbmsdc2.h"
#include "uiflow.h"
//#include "sidcOption.h"
#include "usbopt.h" 
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
PCTRL_COMMAND	G_pCtrlCommand;

xdata	UINT16	G_usCtrlDataLength = 0;
xdata	UINT16	G_usCtrlDataIndex = 0;
xdata	UINT8	G_ucCtrlPacketIndex = 0;
xdata	UINT8	G_UsbConnectStatus = 1; //@WYEO, 1008

xdata	UINT8	G_ucCtrlPhase; 
/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
 
/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
//-----------------------------------------------------------------------------
//CTLSETUP_Packet
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLSETUP_Packet(void)
 * @brief	  process setup packet of control pipe
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLSETUP_Packet(void) USING_1
{
	UINT8 type;

	if (G_ucCtrlPhase != K_CommandPhase)
	{
		//phase transition error - setup packet should not appear at current phase
		//Since hardware will reset the control pipe whenever
		//setup packet arrives, we just force the phase to command phase.
		G_ucCtrlPhase = K_CommandPhase;
	}

	CTLSETUP_GetSetupPacket();		//read setup packet

	G_usCtrlDataLength = G_pCtrlCommand->wLength;
	G_usCtrlDataIndex = 0x0000;

	//check Type of bmRequestType
	type = G_pCtrlCommand->bmRequestType & 0x60;
	if (type == 0x00)
	{
		CTLSETUP_StandardRequest(); 	//Standard Request
	}
	else if (type == 0x20)
	{
		CTLSETUP_ClassRequest();		//Class Request
	}
	else if (type == 0x40)
	{
		CTLSETUP_VendorRequest();		//Vendor Request
	}
	else //if (type == 0x60)
	{
		CTLSETUP_ReservedRequest(); 	//Reserved Request
	}
}

//-----------------------------------------------------------------------------
//CTLSETUP_StandardRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLSETUP_StandardRequest(void)
 * @brief	  process standard request of setup packet
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLSETUP_StandardRequest(void) USING_1
{
	UINT8 xferDir;

	xferDir = G_pCtrlCommand->bmRequestType & 0x80;

	if (G_pCtrlCommand->wLength == 0)	  //no data transaction
	{
		STDREQ_NoDataPhase();
		G_ucCtrlPhase = K_InResponsePhase;
		//CTLIN_Packet();
	}
	else					//data transaction
	{
		if (xferDir == 0x80)
		{
			#if TAG_TODO/* mark for 1628 FPGA */
			//dpsSaveGucStorData_intr();
			#endif
			//in followed
			STDREQ_PreInDataPhase();
			G_ucCtrlPhase = K_InDataPhase;
			CTLIN_Packet();
			#if TAG_TODO /* mark for 1628 FPGA */
			//dpsLoadGucStorData_intr();
			#endif
		}
		else
		{
			//out followed
			STDREQ_PreOutDataPhase();
			G_ucCtrlPhase = K_OutDataPhase;
		}
	}
}

//-----------------------------------------------------------------------------
//CTLSETUP_ClassRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLSETUP_ClassRequest(void)
 * @brief	  process class request of setup packet

 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLSETUP_ClassRequest(void) USING_1
{
	UINT8 xferDir;

	xferDir = G_pCtrlCommand->bmRequestType & 0x80;
#if (USBMSDC_OPTION == 2)		//Mass Storage Device Class (Bulk-Only)
//richie@0204
	if ( ((G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_MASS)  //yichang@replace
		&& (gUsbMode == K_UIOPTION_STORAGE_MSDC) )	
	{
		if (G_pCtrlCommand->wValue !=0x00 || G_pCtrlCommand->wIndex == 0xffff)
		{
			G_ucCtrlPhase = K_CommandPhase;
			XBYTE[REG_Usb_TransactionStallEn] = 0xfe;
		}
		else if (G_pCtrlCommand->wLength == 0)	  //no data transaction
		{
			USBMSDC_BoNoDataPhase();
			G_ucCtrlPhase = K_InResponsePhase;
			//CTLIN_Packet();
		}
		else					//data transaction
		{
			if (xferDir == 0x80)
			{
				//in followed
				if(G_pCtrlCommand->wLength != 0x01)
				{
					G_ucCtrlPhase = K_CommandPhase;
					XBYTE[REG_Usb_TransactionStallEn] = 0xfe;
				}
				else {
					USBMSDC_BoPreInDataPhase();
					G_ucCtrlPhase = K_InDataPhase;
					CTLIN_Packet();
				}
			}
			else
			{
				//out followed
				USBMSDC_BoPreOutDataPhase();
				G_ucCtrlPhase = K_OutDataPhase;
			}
		}
	}
#endif

//CX++ for pccam UVC
	if ( (G_UIStatus & 0xff00) == K_UISTATUS_USBMODE_PCCAM)
	{
		if (G_pCtrlCommand->wLength == 0)	   //no data transaction
		{
			G_ucCtrlPhase = K_InResponsePhase;
			
		}
		else					//data transaction
		{
			if (xferDir == 0x80)
			{
				//in followed
			       CLSREQ_UVCPreInDataPhase();
				G_ucCtrlPhase = K_InDataPhase;
				CTLIN_Packet();
			}
			else
			{
				//out followed
				//	  USBSIDC_PreOutDataPhase();
				G_ucCtrlPhase = K_OutDataPhase;
			}
		}
	}

}

//-----------------------------------------------------------------------------
//CTLSETUP_VendorRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLSETUP_VendorRequest(void)
 * @brief	  process vendor request of setup packet
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLSETUP_VendorRequest(void) USING_1
{
	UINT8 xferDir;

	xferDir = G_pCtrlCommand->bmRequestType & 0x80;

	if (G_pCtrlCommand->wLength == 0)	  //no data transaction
	{
		VNDREQ_NoDataPhase();
		G_ucCtrlPhase = K_InResponsePhase;
		//CTLIN_Packet();
	}
	else					//data transaction
	{
		if (xferDir == 0x80)
		{
			//in followed
			VNDREQ_PreInDataPhase();
			G_ucCtrlPhase = K_InDataPhase;
			CTLIN_Packet();
		}
		else
		{
			//out followed
			VNDREQ_PreOutDataPhase();
			G_ucCtrlPhase = K_OutDataPhase;
		}
	}
}

//-----------------------------------------------------------------------------
//CTLSETUP_ReservedRequest
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLSETUP_ReservedRequest(void)
 * @brief	  process reserved request of setup packet
 * @param	  NONE
 * @retval	  NONE
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLSETUP_ReservedRequest(void) USING_1
{
	UINT8 xferDir;

	xferDir = G_pCtrlCommand->bmRequestType & 0x80;

	if (G_pCtrlCommand->wLength == 0)	  //no data transaction
	{
		RSVREQ_NoDataPhase();
		G_ucCtrlPhase = K_InResponsePhase;
		//CTLIN_Packet();
	}
	else					//data transaction
	{
		if (xferDir == 0x80)
		{
			//in followed
			RSVREQ_PreInDataPhase();
			G_ucCtrlPhase = K_InDataPhase;
			CTLIN_Packet();
		}
		else
		{
			//out followed
			RSVREQ_PreOutDataPhase();
			G_ucCtrlPhase = K_OutDataPhase;
		}
	}
}

//-----------------------------------------------------------------------------
//CTLSETUP_GetSetupPacket
//-----------------------------------------------------------------------------
/**
 * @fn		  void CTLSETUP_GetSetupPacket(void)
 * @brief	  get data for current setup packet
 * @param	  NONE
 * @retval	  length of bulk out packet
 * @see 	  NULL
 * @author	  kai.wang
 * @since	  2010-5-13
 * @todo	  N/A
 * @bug 	  N/A
*/
void CTLSETUP_GetSetupPacket(void) USING_1
{
	UINT8 i;

	for (i = 0; i < K_CTRL_COMMAND_SIZE; i++) { /*Get 8 Bytes setup command data*/
		G_ucCtrlCommand[i] = XBYTE[REG_Usb_Ep0BufData];
	}			 
	G_pCtrlCommand = (PCTRL_COMMAND) G_ucCtrlCommand;
	G_pCtrlCommand->wValue = M_ByteSwapOfWord(G_pCtrlCommand->wValue);
	G_pCtrlCommand->wIndex = M_ByteSwapOfWord(G_pCtrlCommand->wIndex);
	G_pCtrlCommand->wLength = M_ByteSwapOfWord(G_pCtrlCommand->wLength);
}

