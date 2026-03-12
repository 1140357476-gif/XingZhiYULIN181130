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
 * @file      ctlsetup.h
 * @brief     Brief
 * @author    Chi-Yeh Tsai
 * @since     2010-01-01
 * @date      2010-01-01
*/
#ifndef _ctlsetup_h_
#define _ctlsetup_h_
/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
//Standard Request Code
#define K_STDREQCODE_GET_STATUS         0x00
#define K_STDREQCODE_CLEAR_FEATURE      0x01
#define K_STDREQCODE_SET_FEATURE        0x03
#define K_STDREQCODE_SET_ADDRESS        0x05
#define K_STDREQCODE_GET_DESCRIPTOR     0x06
#define K_STDREQCODE_SET_DESCRIPTOR     0x07
#define K_STDREQCODE_GET_CONFIGURATION  0x08
#define K_STDREQCODE_SET_CONFIGURATION  0x09
#define K_STDREQCODE_GET_INTERFACE      0x0A
#define K_STDREQCODE_SET_INTERFACE      0x0B
#define K_STDREQCODE_SYNCH_FRAME        0x0C
//Descriptor Type
#define K_DEVICE_DESCRIPTOR             0x0100
#define K_CONFIGURATION_DESCRIPTOR      0x0200
#define K_STRING_DESCRIPTOR             0x0300
#define K_INTERFACE_DESCRIPTOR          0x0400
#define K_ENDPOINT_DESCRIPTOR           0x0500
#define K_DEVICE_QUALIFIER_DESCRIPTOR   0x0600
#define K_OTHERSPEED_DESCRIPTOR   		0x0700
//Phase of Transfer
#define K_CommandPhase                  0x00
#define K_OutDataPhase                  0x01
#define K_OutDataPhaseWrite             0x02
#define K_OutResponsePhase              0x03
#define K_InDataPhase                   0x81
#define K_InDataPhaseRead               0x82
#define K_InResponsePhase               0x83
#define K_InCompletionPhase             0x84
#define K_XferDirMask                   0x80


#ifdef GENERAL_CLASS
	#define DEVICE_CLASS					0x00
	#define DEVICE_SUBCLASS					0x00
	#define DEVICE_PROTOCOL					0x00
#else
	#define DEVICE_CLASS					0xEF
	#define DEVICE_SUBCLASS					0x02
	#define DEVICE_PROTOCOL					0x01
#endif 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#define K_CTRL_COMMAND_SIZE     0x08
typedef struct _CTRL_COMMAND
{
        UINT8   bmRequestType;
        UINT8   bRequest;
        UINT16  wValue;
        UINT16  wIndex;
        UINT16  wLength;
} CTRL_COMMAND, *PCTRL_COMMAND;
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern  PCTRL_COMMAND   G_pCtrlCommand;

extern  xdata   UINT16  G_usCtrlDataLength;
extern  xdata   UINT16  G_usCtrlDataIndex;
extern  xdata   UINT8   G_ucCtrlPacketIndex;
extern  xdata   UINT8   G_ucCtrlPhase;
extern  xdata   UINT8   G_UsbConnectStatus;

extern  code    UINT8   DeviceDescriptor[];
extern  code    UINT8   ConfigurationDescriptor[]; 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void CTLSETUP_Packet(void);
void CTLSETUP_StandardRequest(void);
void CTLSETUP_ClassRequest(void);
void CTLSETUP_VendorRequest(void);
void CTLSETUP_ReservedRequest(void);
void CTLSETUP_GetSetupPacket(void);

#endif /*_ctlsetup_h_*/
